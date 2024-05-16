"""judge all tests"""
import argparse
from concurrent.futures import ProcessPoolExecutor
import json
import multiprocessing
import os
import sys
import time
from . import common


def print_results(results, expect_verdict=None, show_details=False):
  """print results"""
  res = all([x['success'] for x in results])
  if expect_verdict:
    res &= any([x['verdict'] == expect_verdict for x in results])

  if show_details:
    time = [x['run_result'].get('time', 0.0) / 1000 for x in results]
    verdict = [x['verdict'] for x in results]
    score = [
        x.get('score') or (1.0 if x['verdict'] == 'AC' else 0.0)
        for x in results
    ]

  if res:
    print('\033[32m' + 'PASS' + '\033[0m')
  else:
    print('\033[31m' + 'FAIL' + '\033[0m')

  if show_details:
    print('   results: ', verdict)
    print('   time: ', time)
    if 'PC' in verdict:
      print('   score: ', score)
      print('   score_min: ', min(score))

  if res:
    return True

  for result in results:
    # not expect_verdict == validator
    if (not expect_verdict and
        not result['success']) or (expect_verdict and
                                   result['verdict'] != expect_verdict):
      print(json.dumps(
          result,
          indent=2,
          ensure_ascii=False,
          sort_keys=True,
      ))
  return False


def init_args():
  """init_args"""
  parser = argparse.ArgumentParser()
  parser.add_argument('--workers',
                      default=8,
                      type=int,
                      help='number of parallel workers for running submissions')
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
  parser.add_argument(
      '--sleep',
      action='store_true',
      help=
      'Sleep after compile finish (resolve permission issue for samba mount)')
  return parser.parse_args()


def compile_all(args, problems):
  """compile_all"""
  exes = []
  for prob in problems:
    prob_id = prob.prob_id
    if args.probs and prob_id not in args.probs:
      continue
    if args.ignore_probs and prob_id in args.ignore_probs:
      continue

    exes.append(prob.checker)
    for exe in prob.validators:
      exes.append(exe)
    for exe in prob.solutions:
      exes.append(exe)
    if prob.manager:
      exes.append(prob.manager)

  print('Pre compiling...: ', end='', flush=True)

  with ProcessPoolExecutor(max_workers=32) as pool:
    m = multiprocessing.Manager()
    lock = m.Lock()
    results = list(
        pool.map(common.Compiler.parallel_compile, exes, [lock] * len(exes)))
    for i in range(0, len(exes)):
      common.Compiler.update_result(exes[i], results[i])
  print('DONE\n')
  if args.sleep:
    time.sleep(1)


def main():
  """main"""
  args = init_args()
  problems = common.get_probs()
  result = True

  # pre compile
  compile_all(args, problems)

  with ProcessPoolExecutor(max_workers=args.workers) as pool:
    m = multiprocessing.Manager()
    lock = m.Lock()

    for prob in problems:
      prob_id = prob.prob_id
      # ignore problem
      if args.probs and prob_id not in args.probs:
        continue
      if args.ignore_probs and prob_id in args.ignore_probs:
        continue

      for testcase in prob.tests:
        print('Problem %s Testcase %s:' % (prob_id, testcase))

        if not args.ignore_validators:
          print(' validator:')
          for v in prob.validators:
            if args.writers and v.writer not in args.writers:
              continue
            if v.testcase == 'all' or v.testcase == testcase:
              fn = os.path.relpath(v.filename,
                                   os.path.join(os.getcwd(), 'validator'))
              _n = len(prob.tests[testcase])
              results = list(
                  pool.map(common.run_validator, [v] * _n,
                           prob.tests[testcase]))
              print('  %s: ' % fn, end='')
              result &= print_results(results)

        if not args.ignore_solutions:
          print(' solution:')
          for s in prob.solutions:
            if args.writers and s.writer not in args.writers:
              continue
            if s.testcase == 'all' or s.testcase == testcase:
              fn = os.path.relpath(s.filename,
                                   os.path.join(os.getcwd(), 'solution'))
              _n = len(prob.tests[testcase])
              results = list(
                  pool.map(common.run_solution, [s] * _n, prob.tests[testcase],
                           [prob] * _n, [lock] * _n))
              print('  %s: ' % fn, end='')
              result &= print_results(results, s.expect, show_details=True)
          print('')

  # clean
  for prob in problems:
    common.remove_exes(prob)

  return result


if __name__ == '__main__':
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  success = main()
  if success:
    print('All tests passed')
    sys.exit(0)
  else:
    print('Test failed')
    sys.exit(1)
