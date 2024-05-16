"""judge all tests"""
import argparse
import json
import os
import subprocess
import sys
from . import common


def init_args():
  """init_args"""
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '--probs',
      nargs='*',
      default=[],
      help='Problems wanted to be validated, leave it blank to run all.')
  parser.add_argument(
      '--ignore-probs',
      nargs='*',
      default=[],
      help='Problems wanted to be ignored, leave it blank to run all.')
  parser.add_argument(
      '--writers',
      nargs='*',
      default=[],
      help='Judge only some writers code, leave it blank to run all.')
  parser.add_argument('--ignore-solutions',
                      action='store_true',
                      help='Ignore all solutions.')
  parser.add_argument('--ignore-validators',
                      action='store_true',
                      help='Ignore all validators.')
  parser.add_argument('-f',
                      '--force-update',
                      action='store_true',
                      help='Regen testdatas and update hash values in config.'
                      ' (will remove autogen tests under generator/)')
  parser.add_argument(
      '--sleep',
      action='store_true',
      help=
      'Sleep after compile finish (resolve permission issue for samba mount)')
  return parser.parse_args()


def generate_new_hash(tests):
  """generate_new_hash"""
  result = {}
  for _, case in tests.items():
    for t in case:
      base = os.path.basename(t['input_file'])
      result[base] = t.input_sha1
  return result


def add_autogen_suffix(root):
  """add_autogen_suffix"""
  for f in common.list_files(root, file_only=True, full_path=False):
    base, ext = os.path.splitext(f)
    if ext != '.in':
      continue
    filename_from = os.path.join(root, '%s%s' % (base, ext))
    filename_to = os.path.join(root, '%s_autogen%s' % (base, ext))
    os.rename(filename_from, filename_to)


def remove_autogen_tests(root):
  """remove_autogen_tests"""
  for f in common.list_files(root, file_only=True, full_path=False):
    base, ext = os.path.splitext(f)
    if ext != '.in' and ext != '.out':
      continue
    if not base.endswith('_autogen'):
      continue
    filename = os.path.join(root, '%s%s' % (base, ext))
    print('Remove: %s' % filename)
    os.unlink(filename)


def regen_hashes(prob, tests):
  """regen_hashes"""
  input_hash = {
      os.path.basename(t['input_file']): t.input_sha1
      for case in tests.values() for t in case
  }

  config = common.get_problem_config()
  config[prob.prob_id]['generator']['input_hash'] = input_hash
  with open('config/problems.json', 'w') as f:
    json.dump(config, f, indent=2, ensure_ascii=False)
    f.write('\n')


def check_hashes(prob, tests):
  """check_hashes"""
  for _, case in tests.items():
    for t in case:
      base = os.path.basename(t['input_file'])
      if t.input_sha1 != prob.generator.input_hash.get(base):
        print('Hash check failed, expect %s, got %s' %
              (prob.generator.input_hash.get(base), t.input_sha1))
        print('hash generated:')
        print(
            json.dumps(generate_new_hash(tests),
                       indent=2,
                       ensure_ascii=False,
                       sort_keys=True))
        sys.exit(1)


def check_duplicate_files(tests):
  """check_duplicate_files"""
  for _, case in tests.items():
    for t in case:
      in_base = os.path.basename(t['input_file'])
      in_to = os.path.join(os.getcwd(), 'testdata', t.prob_id, in_base)
      out_base = os.path.basename(t['output_file'])
      out_to = os.path.join(os.getcwd(), 'testdata', t.prob_id, out_base)
      if not os.path.exists(os.path.dirname(out_to)):
        os.makedirs(os.path.dirname(out_to), exist_ok=True)
      if os.path.exists(in_to) or os.path.exists(out_to):
        print('Failed: Input %s %s exists' % (in_to, out_to))
        sys.exit(1)


def main():
  """main"""
  args = init_args()
  problems = common.get_probs()

  for prob in problems:
    prob_id = prob.prob_id

    # ignore problem
    if args.probs and prob_id not in args.probs:
      continue
    if args.ignore_probs and prob_id in args.ignore_probs:
      continue
    if 'generator' not in prob or not prob.generator:
      continue

    print('Generate tests, prob = %s' % prob_id)

    # clear generator dir
    remove_autogen_tests(os.path.join(os.getcwd(), 'generator'))

    # generate .in
    common.Compiler.compile(prob.generator.generator, sleep=args.sleep)
    subprocess.check_call(prob.generator.generator.execute.split(' '),
                          cwd=prob.generator.generator.dirname)
    add_autogen_suffix(os.path.join(os.getcwd(), 'generator'))
    tests = common.get_tests(prob_id,
                             root=os.path.join(os.getcwd(), 'generator'),
                             check_output=False)
    if os.path.exists(prob.generator.generator.execname):
      os.unlink(prob.generator.generator.execname)

    # generate .out
    common.Compiler.compile(prob.generator.solution, sleep=args.sleep)
    for _, case in tests.items():
      for t in case:
        subprocess.check_call(prob.generator.solution.execute.split(' '),
                              cwd=prob.generator.solution.dirname,
                              stdin=open(t['input_file']),
                              stdout=open(t['output_file'], 'w'))
    if os.path.exists(prob.generator.solution.execname):
      os.unlink(prob.generator.solution.execname)

    # check hashes
    if not args.force_update:
      check_hashes(prob, tests)
    else:
      regen_hashes(prob, tests)

    # check duplicate files / delete autogen tests
    if not args.force_update:
      check_duplicate_files(tests)
    else:
      remove_autogen_tests(os.path.join(os.getcwd(), 'testdata', t.prob_id))

    # move files
    for _, case in tests.items():
      for t in case:
        in_base = os.path.basename(t['input_file'])
        in_to = os.path.join(os.getcwd(), 'testdata', t.prob_id, in_base)
        out_base = os.path.basename(t['output_file'])
        out_to = os.path.join(os.getcwd(), 'testdata', t.prob_id, out_base)
        os.makedirs(os.path.dirname(out_to), exist_ok=True)
        print('Generate: ', in_to, out_to)
        os.rename(t['input_file'], in_to)
        os.rename(t['output_file'], out_to)


if __name__ == '__main__':
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  main()
