"""cms_packer.py"""
import argparse
import os
import re
import shutil
import zipfile

import PyPDF2
import yaml
from . import common

TMP_FOLDER = 'cms_packing'


def init_args():
  """init_args"""
  parser = argparse.ArgumentParser()
  parser.add_argument('--no-sample',
                      action='store_true',
                      help='Dont upload sample tests.')
  return parser.parse_args()


def find_language(fn, cms_conf):
  """find_language"""
  ext = os.path.splitext(fn)[1]
  for l in cms_conf['contest-config']['mapping']:
    if ext in cms_conf['contest-config']['mapping'][l]:
      return l
  return None


def prepare_folder():
  """prepare_folder"""
  if os.path.exists(TMP_FOLDER):
    shutil.rmtree(TMP_FOLDER)
  os.makedirs(TMP_FOLDER)


def get_single_prob(task, problems):
  """get_single_prob"""
  for prob in problems:
    if prob.prob_id == task:
      return prob
  return None


def add_testdata(task, prob, problem_conf, ignore_sample=False):
  """add_testdata"""

  def _get_generated_cases(prob):
    if 'generator' in prob and (not prob.generator):
      return []
    inputs = prob.get('generator', {}).get('input_hash', {}).dict().keys()
    inputs = [re.match(r'(\d+)_.*', x).group(1) for x in inputs]
    inputs = [int(x) for x in inputs]
    return list(set(inputs))

  # no testdata
  if not prob:
    return
  cases = [int(case) for case in prob.tests]
  cases.sort()
  for i in range(0, len(problem_conf[task]['score']) + 1):
    if i == 0 and ignore_sample:
      continue
    if i not in cases and i not in _get_generated_cases(prob):
      common.print_warning('Testdata %s/%d_ not exist, skip packing task' %
                           (task, i))
      return

  zipt = zipfile.ZipFile(os.path.join(TMP_FOLDER, 'testdata_%s.zip' % task),
                         'w', zipfile.ZIP_DEFLATED)
  for case in prob.tests:
    for test in prob.tests[case]:
      inf = os.path.basename(test.input_file)
      ouf = os.path.basename(test.output_file)
      zipt.write(test.input_file, inf)
      zipt.write(test.output_file, ouf)


def add_contest(cms_conf):
  """add_contest"""
  res = cms_conf['contest']
  res['tasks'] = []
  for t in cms_conf['tasks']:
    res['tasks'].append(t['name'])

  # add contest yaml
  os.makedirs(os.path.join(TMP_FOLDER, 'contest'))
  yaml.dump(res, open(os.path.join(TMP_FOLDER, 'contest/contest.yaml'), 'w'))
  # add config
  shutil.copytree('config', os.path.join(TMP_FOLDER, 'config'))
  # add scripts
  shutil.copytree('scripts', os.path.join(TMP_FOLDER, 'scripts'))
  # add solution
  shutil.copytree('solution', os.path.join(TMP_FOLDER, 'solution'))
  # add generator
  shutil.copytree('generator', os.path.join(TMP_FOLDER, 'generator'))


def add_task(prob, cms_conf):
  """add_task"""
  prob_id = prob.prob_id
  root = os.path.join(TMP_FOLDER, 'contest', prob_id)
  os.makedirs(root, exist_ok=True)
  os.makedirs(os.path.join(root, 'input'), exist_ok=True)
  os.makedirs(os.path.join(root, 'output'), exist_ok=True)
  if prob.has_stub:
    shutil.copytree(os.path.join('stub', prob_id), os.path.join(root, 'sol'))
  task_conf = common.get_cms_task(prob_id, cms_conf)
  yaml.dump(task_conf, open(os.path.join(root, 'task.yaml'), 'w'))


def add_pdfs(cms_conf):
  """add_tasks"""
  try:
    pdf = PyPDF2.PdfFileReader('problems/problems.pdf')
    chapters = [
        pdf.getDestinationPageNumber(x)
        for x in pdf.getOutlines()
        if not isinstance(x, list)
    ]
    chapters += [pdf.getNumPages()]
  except:
    chapters = []

  use_dummy = False
  if len(chapters) - 1 != len(cms_conf['tasks']):
    common.print_warning('%d tasks, but found %d chapters in PDF, ignore.' %
                         (len(cms_conf['tasks']), len(chapters) - 1))
    use_dummy = True

  tasks = [t for t in cms_conf['tasks']]
  for idx, task_conf in enumerate(tasks):
    task = task_conf['name']
    task_conf = common.get_cms_task(task, cms_conf)
    root = os.path.join(TMP_FOLDER, 'contest', task)

    if use_dummy:
      os.makedirs(os.path.join(root, 'statement'), exist_ok=True)
      shutil.copy('scripts/cms_file/statement/statement.pdf',
                  os.path.join(root, 'statement'))
    else:
      os.makedirs(os.path.join(root, 'statement'), exist_ok=True)
      merger = PyPDF2.PdfFileMerger()
      merger.append(pdf, pages=(chapters[idx], chapters[idx + 1]))
      merger.write(os.path.join(root, 'statement', 'statement.pdf'))


def add_shells(problem_conf, cms_conf, problems, ignore_sample=False):
  """add_shells"""

  def _copy_checker(cms_conf, problems):
    res = ''
    ignore = ['diff.cpp']
    root = os.path.join(TMP_FOLDER, 'checker_cms')
    tasks = [t['name'] for t in cms_conf['tasks']]
    binaries = set()
    checkers = []
    managers = []

    for prob in problems:
      if prob.prob_id not in tasks:
        continue
      # add checker
      checker = prob.checker
      if os.path.basename(checker.filename) not in ignore:
        # add non duplicated binary
        binary = os.path.basename(checker.execute)
        checkers.append((prob.prob_id, binary))
        if binary not in binaries:
          binaries.add(binary)
          res += '%s\n' % (checker.compile)
      # add manager
      if prob.manager:
        binary = os.path.basename(prob.manager.execute)
        managers.append((prob.prob_id, binary))
        if binary not in binaries:
          binaries.add(binary)
          res += '%s\n' % (prob.manager.compile)

    if checkers or managers:
      # copy files
      shutil.copytree('checker', root)
      cs = os.path.join(root, 'testlib_cms.h')
      if os.path.exists(cs):
        shutil.move(cs, os.path.join(root, 'testlib.h'))
      # commands
      res = 'pushd checker_cms\n' + res
      for task, b in checkers:
        res += 'mkdir ../contest/%s/check -p\n' % (task)
        res += 'cp %s ../contest/%s/check/checker\n' % (b, task)
      for task, b in managers:
        res += 'mkdir ../contest/%s/check -p\n' % (task)
        res += 'cp %s ../contest/%s/check/manager\n' % (b, task)
      res += 'popd\n'
    return res

  def _get_generate_tests(cms_conf, problems):
    tasks = [t['name'] for t in cms_conf['tasks']]
    before = 'python3 -m scripts.generate_tests'
    res = []

    for prob in problems:
      if prob.prob_id not in tasks:
        continue
      if not prob.generator:
        continue
      res.append('zip testdata_%s.zip testdata/%s/* -j' %
                 (prob.prob_id, prob.prob_id))
    if res:
      res = [before] + res
    return '\n'.join(res)

  def _get_update_langs(cms_conf):
    res = 'python3 cms/update_contest_language.py'
    for lang in cms_conf['contest-config']['languages']:
      res += ' "%s"' % lang
    res += '\n'
    return res

  def _get_add_user(cms_conf, problems):
    res = ''
    tasks = [t['name'] for t in cms_conf['tasks']]
    users = set()
    for prob in problems:
      if prob.prob_id not in tasks:
        continue
      for v in prob.validators:
        users.add(v.writer)
      for s in prob.solutions:
        users.add(s.writer)
    for u in users:
      res += 'cmsAddUser -p "%s" "" "" "%s"\n' % (u, u)
      res += 'cmsAddParticipation "%s" -c 1\n' % u
    return res

  def _get_inclusive_tests(task_conf, case):
    inclusive_tests = task_conf.get('inclusive_tests', {})
    include_cases = inclusive_tests.get(case, [])
    res = '%s_.*' % case
    for c in include_cases:
      res += '|%s_.*' % c
    return res

  def _get_update_scores(problem_conf, cms_conf, ignore_sample=False):
    res = ''
    tasks = [t for t in cms_conf['tasks']]
    for task_conf in tasks:
      task = task_conf['name']
      task_conf = common.get_cms_task(task, cms_conf)
      scores = [[0, _get_inclusive_tests(task_conf, '0')]]
      for i in range(0, len(problem_conf[task]['score'])):
        scores.append([
            problem_conf[task]['score'][i],
            _get_inclusive_tests(task_conf, '%s' % (i + 1))
        ])

      # ignore sample
      if ignore_sample:
        scores = scores[1:]

      # comment out if testdata zip not exists
      test_zip = os.path.join(TMP_FOLDER, 'testdata_%s.zip' % task)
      line = 'python3 cms/update_task_score.py "%s" "%s" "%s"\n' % (
          task, task_conf['score_type'], str(scores).replace("'", '\\"'))
      if not os.path.exists(test_zip):
        line = '# ' + line
      res += line
    return res

  def _get_update_submission_format(cms_conf, problems):
    res = ''
    tasks = [t for t in cms_conf['tasks']]
    for task_conf in tasks:
      task = task_conf['name']
      task_conf = common.get_cms_task(task, cms_conf)
      if not task_conf.get('output_only') and not task_conf.get(
          'submission_format'):
        continue

      formats = []
      if task_conf.get('submission_format'):
        formats = task_conf['submission_format']
      elif task_conf.get('output_only'):
        prob = None
        for p in problems:
          if p.prob_id == task:
            prob = p
        for case in prob.tests:
          for test in prob.tests[case]:
            _, fn, _ = common.split(test.input_file)
            formats.append('output_%s.txt' % fn)

      line = 'python3 cms/update_task_submission_format.py "%s"' % task
      for f in formats:
        line += ' "%s"' % f
      line += '\n'

      test_zip = os.path.join(TMP_FOLDER, 'testdata_%s.zip' % task)
      if not os.path.exists(test_zip):
        line = '# ' + line
      res += line
    return res

  def _get_update_two_steps(cms_conf, problems):
    res = ''
    tasks = [t for t in cms_conf['tasks']]
    for task_conf in tasks:
      task = task_conf['name']
      prob = None
      for p in problems:
        if p.prob_id == task:
          prob = p
      task_conf = common.get_cms_task(task, cms_conf)
      if task_conf.get('task_type') != 'TwoSteps':
        continue

      ignore = ['diff.cpp']
      ev = 'diff'
      if os.path.basename(prob.checker.filename) not in ignore:
        ev = 'comparator'
      res += 'sudo python3 cms/update_twosteps.py "%s" "%s" "%s"\n' % (
          task, ev, 'contest/%s/sol' % task)
    return res

  def _get_add_tests(cms_conf):
    res = ''
    tasks = [t['name'] for t in cms_conf['tasks']]
    for task in tasks:
      z = os.path.join(TMP_FOLDER, 'testdata_%s.zip' % task)
      if os.path.exists(z):
        res += 'cmsAddTestcases "%s" "testdata_%s.zip" "%s" "%s"\n' % (
            task, task, '*.in', '*.out')
    return res

  def _copy_solutions(cms_conf, problems):
    res = ''
    tasks = [t['name'] for t in cms_conf['tasks']]
    for prob in problems:
      if prob.prob_id not in tasks:
        continue
      # if no testdata, do not submit
      z = os.path.join(TMP_FOLDER, 'testdata_%s.zip' % prob.prob_id)
      if not os.path.exists(z):
        common.print_warning('No testdata, ignore submissions of %s' %
                             prob.prob_id)
        continue

      for s in prob.solutions:
        f = os.path.join(TMP_FOLDER, 'submissions', s.writer)
        os.makedirs(f, exist_ok=True)
        # do not submit duplicate solutions
        if os.path.exists(os.path.join(f, os.path.basename(s.filename))):
          continue
        shutil.copy2(s.filename, f)

        # filelist
        cms_file = prob.prob_id + '.%l' if not s.get(
            'cms_filename') else s.cms_filename
        filelist = [(cms_file, 'submissions/%s/%s' % (
            s.writer,
            os.path.basename(s.filename),
        ))]
        if s.get('cms_submit_together') != None:
          for s2 in s.cms_submit_together:
            s2fn = 'submissions/%s/%s' % (s.writer,
                                          os.path.basename(s2['filename']))
            filelist.append((s2['cms_filename'], s2fn))
            s2fno = 'solution/%s/%s' % (s.writer,
                                        os.path.basename(s2['filename']))
            shutil.copy2(s2fno, f)
        fileargs = ''
        for f in filelist:
          fileargs += '-f "%s:%s" ' % (f[0], f[1])

        # lang
        lang = find_language(os.path.basename(s.filename), cms_conf)
        lang_arg = '-l "%s"' % lang
        if not lang:
          lang_arg = ''
        res += 'python3 cms/add_submission.py %s %s --comment "%s" "%s" "%s" \n' % (
            fileargs,
            lang_arg,
            os.path.basename(s.filename),
            s.writer,
            prob.prob_id,
        )
    return res

  shutil.copytree('scripts/cms', os.path.join(TMP_FOLDER, 'cms'))
  shutil.copy2('scripts/cms_file/00-init.sh', TMP_FOLDER)
  shutil.copy2('scripts/cms_file/03-gen_report.sh', TMP_FOLDER)
  with open('scripts/cms_file/01-import.sh') as f:
    content = f.read()
    content = content.replace('{{shell_compile_checker}}',
                              _copy_checker(cms_conf, problems))
    content = content.replace('{{shell_generate_tests}}',
                              _get_generate_tests(cms_conf, problems))
    content = content.replace('{{shell_update_langs}}',
                              _get_update_langs(cms_conf))
    content = content.replace('{{shell_add_testcases}}',
                              _get_add_tests(cms_conf))
    content = content.replace(
        '{{shell_update_scores}}',
        _get_update_scores(problem_conf, cms_conf, ignore_sample))
    content = content.replace('{{shell_update_two_steps}}',
                              _get_update_two_steps(cms_conf, problems))
    content = content.replace('{{shell_update_submission_format}}',
                              _get_update_submission_format(cms_conf, problems))
    # wb to prevent line ending issues
    with open(os.path.join(TMP_FOLDER, '01-import.sh'), 'wb') as f2:
      f2.write(content.encode())

  # copy solutions
  shell_add_submission = _copy_solutions(cms_conf, problems)

  with open('scripts/cms_file/02-import_tests.sh') as f:
    content = f.read()
    content = content.replace('{{shell_add_user}}',
                              _get_add_user(cms_conf, problems))
    content = content.replace('{{shell_add_submission}}', shell_add_submission)
    # wb to prevent line ending issues
    with open(os.path.join(TMP_FOLDER, '02-import_tests.sh'), 'wb') as f2:
      f2.write(content.encode())


def main():
  """main"""
  args = init_args()
  problem_conf = common.get_problem_config()
  cms_conf = common.load_cms_conf()
  problems = common.get_probs(fetch_notest=True)

  prepare_folder()
  add_contest(cms_conf)
  add_pdfs(cms_conf)
  for task in cms_conf['tasks']:
    prob = get_single_prob(task['name'], problems)
    print(prob.prob_id)
    add_testdata(task['name'], prob, problem_conf, args.no_sample)
    add_task(prob, cms_conf)
  add_shells(problem_conf, cms_conf, problems, args.no_sample)
  shutil.make_archive('cms_artifact', 'zip', 'cms_packing')


if __name__ == '__main__':
  main()
