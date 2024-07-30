"""judge common lib"""
from concurrent.futures import ProcessPoolExecutor
from typing import Any
from typing import Dict
from typing import List
from typing import Union
from typing import Tuple
from distutils.dir_util import copy_tree
import distutils
import hashlib
import json
import jsonschema
import multiprocessing
import os
import re
import shutil
import subprocess
import tempfile
import time
import toml


class TemplateDict():
  """dict -> class 転換"""

  def __init__(self, d: Dict) -> None:
    for key in d:
      setattr(self, key, self.value_to_object(key, d[key]))

  def value_to_object(self, key: str, value: Any) -> None:
    """value を適切な TemplateDict タイプに転換"""
    # 単純な python タイプです
    if not isinstance(value, Dict):
      return value
    # type が指定された場合
    if hasattr(self, key) and hasattr(getattr(self, key), '__class__'):
      ClassType = type(getattr(self, key))
      return ClassType(value)
    # type が指定されていない
    return TemplateDict(value)

  def to_str(self) -> str:
    """文字列に変換"""
    return json.dumps(vars(self),
                      indent=2,
                      ensure_ascii=False,
                      sort_keys=True,
                      default=lambda x: getattr(x, '__dict__', str(x)))

  def dict(self) -> Dict:
    """to dict"""
    return self.__dict__

  def get(self, key, val=None):
    """get"""
    if key in self.__dict__:
      return self.__dict__[key]
    return val

  def __str__(self):
    return self.to_str()

  def __repr__(self):
    return self.to_str()

  def __iter__(self):
    return iter(self.__dict__.keys())

  def __getitem__(self, key):
    return self.__dict__[key]

  def __setitem__(self, key, value):
    self.__dict__[key] = self.value_to_object(key, value)


class Excutable(TemplateDict):
  """solution / validator"""
  dirname: str
  filename: str
  execname: str
  compile: str
  execute: str
  testcase: Union[str, None]  # "1", "2", ... "sample"
  expect: str  # "AC", "WA", "TLE"...
  writer: str
  makefile: Union[str, None]
  stub_dir: Union[str, None]
  cms_filename: Union[str, None]
  cms_submit_together: List[Dict[str, str]]


class Testdata(TemplateDict):
  """problem testdata"""
  prob_id: str
  testcase: str  # "1", "2", ... "sample"
  input_file: str
  output_file: str
  input_sha1: str


class Generator(TemplateDict):
  """generator descriptor"""
  generator: Excutable
  solution: Excutable
  tests: List[Testdata]
  input_hash: Dict[str, str]


class Problem(TemplateDict):
  """problem descriptor"""
  prob_id: str
  tests: Dict[str, List[Testdata]]  # testcase -> testdatas
  solutions: List[Excutable]
  validators: List[Excutable]
  time_limit: int  # ms
  memory_limit: int  # kilobytes
  checker: Excutable
  manager: Excutable
  communication_processes: int  # needs refactor
  generator: Generator
  has_stub: bool


class Compiler():
  """compiler"""
  compiled_results = {}

  @classmethod
  def popen2(cls, exe: Excutable, sleep=False):
    """popen2"""
    p = subprocess.Popen(exe.compile.split(' '),
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         cwd=exe.dirname)
    returncode = p.wait()
    stdout, stderr = p.communicate()
    if os.path.exists(exe.execname):  # .py doesn't have exec
      os.chmod(exe.execname, os.stat(exe.execname).st_mode | 0o111)
    if sleep:
      time.sleep(0.5)
    return {
        'returncode': returncode,
        'stdout': stdout.decode('utf-8', errors='ignore'),
        'stderr': stderr.decode('utf-8', errors='ignore')
    }

  @classmethod
  def parallel_compile(cls, exe: Excutable, lock):
    """parallel_compile"""
    if 'stub_dir' in exe and exe.stub_dir:
      with lock:
        try:
          for f in os.listdir(exe.stub_dir):
            shutil.copy2(os.path.join(exe.stub_dir, f),
                         os.path.join(exe.dirname, f))
          result = cls.popen2(exe)
        finally:
          for f in os.listdir(exe.stub_dir):
            path = os.path.join(exe.dirname, f)
            if os.path.isfile(path):
              os.remove(path)
            else:
              shutil.rmtree(path, ignore_errors=False)
      return result
    else:
      return cls.popen2(exe)

  @classmethod
  def update_result(cls, exe: Excutable, res: str):
    """update_result"""
    fn = exe.filename
    cls.compiled_results[fn] = res

  @classmethod
  def compile(cls, exe: Excutable, sleep=False):
    """compile"""
    fn = exe.filename
    if fn not in cls.compiled_results:
      if 'stub_dir' in exe and exe.stub_dir:
        try:
          for f in os.listdir(exe.stub_dir):
            shutil.copy2(os.path.join(exe.stub_dir, f),
                         os.path.join(exe.dirname, f))
          cls.compiled_results[fn] = cls.popen2(exe, sleep=sleep)
        finally:
          for f in os.listdir(exe.stub_dir):
            path = os.path.join(exe.dirname, f)
            if os.path.isfile(path):
              os.remove(path)
            else:
              shutil.rmtree(path, ignore_errors=False)
      else:
        cls.compiled_results[fn] = cls.popen2(exe, sleep=sleep)
    return cls.compiled_results[fn]


def list_files(root: str,
               file_only: bool = False,
               dir_only: bool = False,
               full_path: bool = True) -> List[str]:
  """ディレクトリ走査"""
  assert not (file_only and dir_only)
  result = []
  for curDir, _, files in os.walk(root):
    if not file_only:
      result.append(curDir)
    if not dir_only:
      result += [os.path.join(curDir, fn) for fn in files]

  if not full_path:
    result = [os.path.relpath(fn, root) for fn in result]
  result.sort()
  return result


def split(file_name: str,
          split_dir: bool = False) -> Tuple[Union[List[str], str], str, str]:
  """split filename"""
  dirname = os.path.dirname(file_name)
  basename = os.path.basename(file_name)
  filename, ext = os.path.splitext(basename)

  if split_dir:
    dirname = dirname.split(os.path.sep)

  return (dirname, filename, ext)


def join(dirname: Union[List[str], str],
         filename: str = '',
         ext: str = '') -> str:
  """merge to filepath"""
  if isinstance(dirname, list):
    dirname = os.path.sep.join(dirname)
  basename = filename + ext
  return os.path.join(dirname, basename)


def file_sha1(fn):
  """file_sha1"""
  m = hashlib.sha1()
  with open(fn, 'rb') as f:
    for chunk in iter(lambda: f.read(4096 * m.block_size), b''):
      m.update(chunk)
  return m.hexdigest()


def get_tests(prob_id, root=None, check_output=True):
  """get testdatas"""
  result = {}
  if not root:
    root = 'testdata/%s/' % (prob_id)
  full_root = os.path.join(os.getcwd(), root)
  files = list_files(root, file_only=True, full_path=False)

  for f in files:
    base, ext = os.path.splitext(f)
    if ext != '.in' and ext != '.out':
      print('get_testdata(%s): ignore file: %s' % (prob_id, f))

    # check pair files
    extr = '.out' if ext == '.in' else '.out'
    fr = base + extr
    if check_output:
      assert fr in files

    # add result
    if ext == '.in':
      testcase = f.split('_')[0]
      if not testcase in result:
        result[testcase] = []
      result[testcase].append(
          Testdata({
              'prob_id': prob_id,
              'testcase': testcase,
              'input_file': os.path.join(full_root, f),
              'output_file': os.path.join(full_root, fr),
              'input_sha1': file_sha1(os.path.join(full_root, f))
          }))
  return result


def get_execs(root, prob_id, fetch_notest=False, stub_dir=None):
  """get executables"""
  result = []
  files = list_files(root, file_only=True)

  def find_makefile(file_list, d):
    makefiles = ['makefile', 'Makefile']
    for m in makefiles:
      if join(d, m, '') in file_list:
        return join(d, m, '')
    return None

  def _get_custom_config(root):
    f = os.path.join(root, 'config.toml')
    if os.path.exists(f):
      return toml.load(open(f, encoding='utf8'))
    return {}

  def _get_custom_prob_conf(f, ignore_test=False):
    d, f, ext = split(f, split_dir=True)
    custom_config = _get_custom_config(os.path.join(root, d[1]))
    if 'execs' not in custom_config:
      return None, None

    for exe in custom_config['execs']:
      if exe['filename'] != f + ext:
        continue
      # found
      tests = []
      overwrite = {}
      if ignore_test:
        tests = [{'testcase': 'all', 'expect': 'AC'}]
      elif 'tests' in exe:
        tests = exe['tests']
      if 'overwrite' in exe:
        overwrite = exe['overwrite']
      return tests, overwrite
    return None, None

  def _get_common_config(f):
    compile_tmpl = 'g++ -std=gnu++1z -O2 -Wall -Wshadow %s -o %s'
    run_tmpl = './%s'
    d, f, ext = split(f, split_dir=True)
    makefile = find_makefile(files, d)

    comp = compile_tmpl % (f + ext, f)
    if makefile:
      comp = '/usr/bin/env make ' + f

    execute = run_tmpl % (f)
    if makefile:
      execute = '/usr/bin/env make -s run_' + f

    return {
        'dirname': os.path.join(os.getcwd(), join(d, '', '')),
        'filename': os.path.join(os.getcwd(), join(d, f, ext)),
        'execname': os.path.join(os.getcwd(), join(d, f, '')),
        'compile': comp,
        'execute': execute,
        'makefile': makefile,
        'writer': d[1],
        'stub_dir': stub_dir,
    }

  def _get_filename_config(f):
    reg = r'^([^_-]+)(?:_(\d+))?(?:_([A-Z]+))?(?:-(.+))?\.(?:[^.]+)$'
    _, f, ext = split(f, split_dir=True)
    match = re.match(reg, f + ext)
    if not match:
      return {}

    prob_id = match.group(1)
    testcase = 'all'
    expect = 'AC'

    if match.group(2):
      testcase = match.group(2)
    if match.group(3):
      expect = match.group(3)

    return {
        'prob_id': prob_id,
        'testcase': testcase,
        'expect': expect,
    }

  for f in files:
    econf = {**_get_common_config(f), **_get_filename_config(f)}
    tests, overwrite = _get_custom_prob_conf(f, ignore_test=fetch_notest)
    if tests is not None:
      for test in tests:
        result.append({**econf, **test, **overwrite})
    else:
      result.append(econf)

  # filter
  result = [
      Excutable(x) for x in result if 'prob_id' in x and x['prob_id'] == prob_id
  ]
  return result


def make_simple_exec(f, make_run=False):
  """make_simple_exec for checker"""
  d, f, ext = split(f)
  return Excutable({
      'dirname':
          os.path.join(os.getcwd(), join(d, '', '')),
      'filename':
          os.path.join(os.getcwd(), join(d, f, ext)),
      'execname':
          os.path.join(os.getcwd(), join(d, f, '')),
      'compile':
          'make %s' % f,
      'execute':
          'make run_%s' %
          f if make_run else os.path.join(os.getcwd(), join(d, f, '')),
      'testcase':
          '',
      'expect':
          '',
      'makefile':
          '',
      'writer':
          ''
  })


def make_generator(generator, solutions):
  """make_generator"""
  sol = None
  expect_filename = os.path.join(os.getcwd(), 'solution', generator['solution'])
  solution_files = [s['filename'] for s in solutions]
  for s in solutions:
    if s['filename'] == expect_filename:
      sol = s
  if sol is None:
    raise ValueError(f'No {expect_filename!r} in {solution_files!r}')
  return Generator({
      'generator':
          make_simple_exec(os.path.join('generator', generator['generator']),
                           make_run=True),
      'input_hash':
          generator['input_hash'],
      'solution':
          sol,
  })


def get_problem_config():
  """get_problem_config"""
  with open('config/problems.json') as f:
    contents = json.load(f)
  with open('scripts/problems.schema.json') as f:
    schema = json.load(f)
  jsonschema.validate(contents, schema)
  return contents


def get_probs(fetch_notest=False):
  """get problems"""
  result = []
  for prob_id, v in get_problem_config().items():
    stub_dir = None
    if os.path.exists(os.path.join('stub', prob_id)):
      stub_dir = os.path.join('stub', prob_id)

    result.append(
        Problem({
            'prob_id':
                prob_id,
            'tests':
                get_tests(prob_id),
            'solutions':
                get_execs('solution',
                          prob_id,
                          fetch_notest=fetch_notest,
                          stub_dir=stub_dir),
            'validators':
                get_execs('validator', prob_id),
            'time_limit':
                v['time_limit'],
            'memory_limit':
                v.get('memory_limit', None),
            'checker':
                make_simple_exec(os.path.join('checker', v['checker'])),
            'manager':
                make_simple_exec(os.path.join('checker', v['manager']))
                if 'manager' in v else None,
            'communication_processes':
                v.get('communication_processes', 0),
            'generator':
                make_generator(
                    v['generator'],
                    get_execs('solution',
                              prob_id,
                              fetch_notest=True,
                              stub_dir=stub_dir)) if 'generator' in v else None,
            'has_stub':
                True if stub_dir else False,
        }))
  return result


def run_validator(exe, test, job_id=0):
  """run_validator"""
  del job_id  # unused
  result_c = Compiler.compile(exe)

  fin = open(test.input_file)
  envs = dict(os.environ, ARGS='--testcase %s' % (test.testcase))
  cmd = exe.execute.split(' ')
  if len(cmd) < 2 or cmd[1] != 'make':
    cmd += ['--testcase', test.testcase]
  result = subprocess.run(cmd,
                          stdin=fin,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          cwd=exe.dirname,
                          env=envs)

  ret = {
      'success': result_c['returncode'] == 0 and result.returncode == 0,
      'compile_returncode': result_c['returncode'],
      'stdout': result.stdout.decode('utf-8', errors='ignore'),
      'stderr': result.stderr.decode('utf-8', errors='ignore'),
      'returncode': result.returncode,
      'testdata': test.input_file,
  }
  if result_c['returncode'] != 0:
    ret['compile_stdout'] = result_c['stdout']
    ret['compile_stdout'] = result_c['stderr']
  return ret


def get_isolate_box(lock):
  """get_isolate_box"""
  isolate_root = '/var/local/lib/isolate/%d/box/'
  job_id = -1
  with lock:
    for i in range(0, 1000):
      folder = isolate_root % i
      if not os.path.exists(folder):
        job_id = i
        subprocess.run(
            ['isolate', '--box-id=%d' % job_id, '--cg', '--init'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        break
  return job_id


class Isolate:
  """Isolate manager"""
  box_id: int
  isolate_root: str
  meta_file: str
  processes: int = 64
  dir: List[str]
  time: int = 1000  # ms
  mem: int = None
  stdin: str = None
  stdout: str = None
  stderr: str = None
  cmd: List[str]

  def __init__(self, lock):
    self.box_id = get_isolate_box(lock)
    self.isolate_root = '/var/local/lib/isolate/%d/box/' % self.box_id
    self.meta_file = os.path.join(self.isolate_root, 'meta')
    self.cmd = []  # avoid static
    self.dir = [
        '/etc/alternatives',  # java support
    ]

  def parse_isolate_meta(self):
    """_parse_isolate_meta"""
    assert os.path.isfile(self.meta_file)
    result = {}
    with open(self.meta_file) as f:
      lines = f.read().split('\n')
      for line in lines:
        match = re.match(r'(\S+):(.*)', line)
        if match:
          result[match.group(1)] = match.group(2)
    if 'cg-mem' in result:
      result['cg-mem'] = int(result['cg-mem'])
    if 'max-rss' in result:
      result['max-rss'] = int(result['max-rss'])
    if 'exitcode' in result:
      result['exitcode'] = int(result['exitcode'])
    if 'time' in result:
      result['time'] = int(float(result['time']) * 1000)
    if 'time-wall' in result:
      result['time-wall'] = int(float(result['time-wall']) * 1000)
    return result

  def get_path(self, fn):
    """get real path under isolate box"""
    return os.path.join(self.isolate_root, fn)

  def get_result(self):
    """get_result"""
    verdict = ''
    result_r = self.parse_isolate_meta()
    mem_usage = max(result_r.get('max-rss', 0), result_r.get('cg-mem', 0))

    if (result_r.get('status', '') == 'TO' or
        result_r.get('time', 0) > self.time):
      verdict = 'TLE'
    elif (result_r.get('status', '') == 'ML' or
          (self.mem and mem_usage > self.mem)):
      verdict = 'MLE'
    elif result_r.get('exitcode', 1) != 0:
      verdict = 'RE'

    if os.path.isfile(self.meta_file):
      os.remove(self.meta_file)
    return verdict, result_r

  def copy_tree(self, src, dst=''):
    """copy_tree: copy files under src to root"""
    # https://stackoverflow.com/questions/9160227/dir-util-copy-tree-fails-after-shutil-rmtree
    distutils.dir_util._path_created = {}
    copy_tree(src, self.get_path(dst))

  def copy_file(self, src, dst):
    """copy_file"""
    shutil.copy(src, self.get_path(dst))

  def cleanup(self):
    """cleanup"""
    subprocess.run(['isolate', '--box-id=%d' % self.box_id, '--cleanup'])

  def _get_run_args(self):
    """get_run_args"""
    args = [
        'isolate',
        '--box-id=%d' % self.box_id,
        '--cg',  # control group
        '-e',  # full env
        '--silent',
        '--meta=%s' % self.meta_file,
    ]

    # time limit
    args += [
        '--time=%s' % (self.time / 1000),
        '--wall-time=%s' % (self.time / 1000 * 10),
    ]

    # TODO: Kill the program if it uses more memory than it should.

    # processes
    if self.processes:
      args.append('--processes=%d' % self.processes)

    # dirs
    for d in self.dir:
      args.append('--dir=%s' % d)

    # stdin, stdout, stderr
    if self.stdin:
      args.append('--stdin=%s' % self.stdin)
    if self.stdout:
      args.append('--stdout=%s' % self.stdout)
    if self.stderr:
      args.append('--stderr=%s' % self.stderr)

    args += [
        '--run',
        '--',
    ]
    return args

  def print_debug(self):
    """print_debug"""
    args = self._get_run_args()
    print('args: ', args)
    print('cmd: ', self.cmd)

    files = [
        (self.stdout, self.get_path(self.stdout)),
        (self.stderr, self.get_path(self.stderr)),
    ]
    for name, fn in files:
      if not os.path.isfile(fn):
        print('File %s not exists', name)
        continue
      with open(fn, 'r') as f:
        print('File %s: ' % name, f.readlines())

  def run(self):
    """run"""
    args = self._get_run_args()
    subprocess.run(args + self.cmd,
                   stdout=subprocess.PIPE,
                   stderr=subprocess.PIPE,
                   cwd=self.isolate_root)


def merge_isolate_results(processes: List[Isolate], time: int, mem: int = None):
  """merge_isolate_results"""

  results = [p.parse_isolate_meta() for p in processes]

  verdict = ''
  result_r = {}
  for r in results:
    result_r['max-rss'] = r.get('max-rss', 0) + result_r.get('max-rss', 0)
    result_r['cg-mem'] = r.get('cg-mem', 0) + result_r.get('cg-mem', 0)
    result_r['time'] = r.get('time', 0) + result_r.get('time', 0)
    result_r['exitcode'] = max(r.get('exitcode', 1),
                               result_r.get('exitcode', 0))  # pick max

  mem_usage = max(result_r.get('max-rss', 0), result_r.get('cg-mem', 0))
  if any([r.get('status', '') == 'TO' for r in results]):
    verdict = 'TLE'
  elif result_r['time'] > time:
    verdict = 'TLE'
  elif any([r.get('status', '') == 'ML' for r in results]):
    verdict = 'MLE'
  elif mem and mem_usage > mem:
    verdict = 'MLE'
  elif any([r.get('exitcode', 1) != 0 for r in results]):
    verdict = 'RE'
  return verdict, result_r


def run_isolates(procs: List[Isolate]):
  """run_isolates: procs may contains None object"""
  procs = [x for x in procs if x]
  if len(procs) > 1:
    with ProcessPoolExecutor(max_workers=len(procs)) as pool:
      pool.map(Isolate.run, procs)
  elif len(procs) == 1:
    procs[0].run()


def run_solution(exe, test, prob, lock):
  """run_solution"""
  verdict = ''
  checker_stderr = ''
  compiler_stderr = ''
  result_r = {}
  tmpdir = None
  communication = True if prob.communication_processes else False
  processes = prob.communication_processes or 1
  isolate_mgr = None
  manager_r = None
  isolate_sols = []
  fifo_user_to_manager = []
  fifo_manager_to_user = []

  # compile
  result_c = Compiler.compile(exe)
  if result_c['returncode']:
    verdict = 'CE'
    compiler_stderr = result_c['stderr']

  if communication and prob.manager:
    result_cm = Compiler.compile(prob.manager)
    if not verdict and result_cm['returncode']:
      verdict = 'IE'  # internal error
      compiler_stderr = result_cm['stderr']

  # prepare tmpdir
  if communication:
    tmpdir = tempfile.mkdtemp()
    fifo_user_to_manager = [
        os.path.join(tmpdir, 'u%d_to_m' % i) for i in range(0, processes)
    ]
    fifo_manager_to_user = [
        os.path.join(tmpdir, 'm_to_u%d' % i) for i in range(0, processes)
    ]
    os.chmod(tmpdir, 0o755)
    for i in range(0, processes):
      os.mkfifo(fifo_user_to_manager[i])
      os.mkfifo(fifo_manager_to_user[i])
      os.chmod(fifo_user_to_manager[i], 0o666)
      os.chmod(fifo_manager_to_user[i], 0o666)

    isolate_mgr = Isolate(lock)
    isolate_mgr.time = prob.time_limit * 2
    isolate_mgr.stdin = 'in'
    isolate_mgr.stdout = 'out'
    isolate_mgr.stderr = 'stderr'
    isolate_mgr.dir.append('/tmp=%s:rw' % tmpdir)
    isolate_mgr.cmd = ['./%s' % os.path.basename(prob.manager.execute)]  # TODO
    isolate_mgr.copy_tree(prob.manager.dirname)
    isolate_mgr.copy_file(test.input_file, 'in')
    for i in range(0, processes):
      isolate_mgr.cmd += [
          '/tmp/%s' % os.path.basename(fifo_user_to_manager[i]),
          '/tmp/%s' % os.path.basename(fifo_manager_to_user[i]),
      ]

  for i in range(0, processes):
    tmp = Isolate(lock)
    tmp.time = prob.time_limit
    tmp.mem = prob.memory_limit
    tmp.stdin = 'in'
    tmp.stdout = 'out'
    tmp.stderr = 'stderr'
    tmp.cmd = exe.execute.split(' ')
    tmp.copy_tree(exe.dirname)
    tmp.copy_file(test.input_file, 'in')
    if communication:
      tmp.dir.append('/tmp=%s:rw' % tmpdir)
      tmp.cmd += [
          '/tmp/%s' % os.path.basename(fifo_manager_to_user[i]),
          '/tmp/%s' % os.path.basename(fifo_user_to_manager[i]),
      ]
    isolate_sols.append(tmp)

  # run prog
  if not verdict:
    run_isolates(isolate_sols + [isolate_mgr])
    verdict, result_r = merge_isolate_results(isolate_sols, prob.time_limit,
                                              prob.memory_limit)
    if isolate_mgr:
      v, manager_r = isolate_mgr.get_result()
      if not verdict and v:
        verdict = 'IE'

  # run checker
  score = None
  if not verdict and isolate_mgr:
    final_output = isolate_mgr.get_path('out')
    if not os.path.isfile(final_output):
      verdict = 'IE'
      checker_stderr = 'No manager output'
    else:
      lines = ''
      with open(final_output) as f:
        lines = ''.join(f.readlines()).strip()
      checker_stderr = 'manager score: %s' % lines
      sread = float(lines)
      if sread == 1:
        verdict = 'AC'
      elif sread == 0:
        verdict = 'WA'
      else:
        verdict = 'PC'
        score = sread
  elif not verdict:
    final_output = isolate_sols[0].get_path('out')
    Compiler.compile(prob.checker)
    res = subprocess.run(
        [prob.checker.execute, test.input_file, final_output, test.output_file],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=prob.checker.dirname)
    if res.returncode == 0:
      verdict = 'AC'
    elif res.returncode == 7:  # points should be printed to stdout
      verdict = 'PC'
      score = float(res.stdout.decode('utf-8', errors='ignore'))
      checker_stderr = res.stderr.decode('utf-8', errors='ignore')
    elif res.returncode >= 50:  # should define TESTSYS in checker
      verdict = 'PC'
      score = (res.returncode - 50) / 200
      checker_stderr = res.stderr.decode('utf-8', errors='ignore')
    else:
      verdict = 'WA'
      checker_stderr = res.stderr.decode('utf-8', errors='ignore')

  if isolate_mgr:
    isolate_mgr.cleanup()
  for p in isolate_sols:
    p.cleanup()
  if tmpdir:
    shutil.rmtree(tmpdir)

  success = False
  if exe.expect == 'AC':
    success = verdict == exe.expect
  else:
    success = exe.expect not in ['CE']

  return {
      'success': success,
      'run_result': result_r,
      'manager_result': manager_r,
      'verdict': verdict,
      'testdata': test.input_file,
      'checker_stderr': checker_stderr,
      'compiler_stderr': compiler_stderr,
      'score': score,
  }


def remove_exes(prob: Problem):
  """remove_exes"""

  for exe in prob.validators:
    if os.path.isfile(exe.execname):
      os.remove(exe.execname)

  for exe in prob.solutions:
    if os.path.isfile(exe.execname):
      os.remove(exe.execname)

  if os.path.isfile(prob.checker.execname):
    os.remove(prob.checker.execname)

  if prob.manager and os.path.isfile(prob.manager.execname):
    os.remove(prob.manager.execname)


def print_warning(msg: str):
  """print_warning"""
  print('\033[31m' + 'WARNING' + '\033[0m: ' + msg)


def load_cms_conf():
  """load_cms_conf"""
  cms_conf = toml.load(open('config/cms.toml', encoding='utf8'))
  if 'tasks' not in cms_conf:
    cms_conf['tasks'] = []
  return cms_conf


def get_cms_task(prob_id, cms_conf):
  """get_cms_task"""
  res = {}
  for t in cms_conf['tasks']:
    if t['name'] == prob_id:
      res = t
      break

  # apply template
  for key in cms_conf['task_template']:
    if key not in res:
      res[key] = cms_conf['task_template'][key]

  res['n_input'] = 0
  return res
