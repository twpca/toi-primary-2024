"""get_result"""
import argparse
import time
import os
import sys

from cms.db import SessionGen, Submission, Testcase

TRY_LIMIT = 120  # 120*5=600 seconds


def init():
  """init"""
  parser = argparse.ArgumentParser()
  parser.add_argument('output', type=str, help='output file')
  return parser.parse_args()


def needs_wait(submissions):
  """needs_wait"""
  ng = [
      1,  # COMPILING
      3,  # EVALUATING
      4,  # SCORING
  ]
  # TODO: if s[1] is None, retrigger
  s = [s[1].get_status() if s[1] else None for s in submissions]
  if None in s:
    return True
  for code in ng:
    if code in s:
      return True
  return False


def get_testcase(session, dataset_id, testcase_id):
  """get_testcase"""
  return session.query(Testcase).filter(
      Testcase.codename == testcase_id).filter(
          Testcase.dataset_id == dataset_id).first()


def to_score(x):
  """to_score"""
  if int(x) == x:
    return int(x)
  return x


def get_max_resource(testcases):
  """get_max_resource"""
  t = 0
  m = 0
  for test in testcases:
    t = max(t, test['time'] or 0)
    m = max(m, test['memory'] or 0)
  return (t, m)


def get_overview_table(submissions):
  """get_overview_table"""
  # get cases
  res = ''
  cases = []
  if submissions:
    details = submissions[0][1].score_details
    for case in details:
      cases.append('%s (%s)' % (case['idx'], case['max_score']))

  res += '<table border="1">\n'
  res += '<tr><th>Solution</th>'
  for case in cases:
    res += '<th>%s</th>' % case
  res += '<th>Score</th>'
  res += '</tr>\n'

  for s, r in submissions:
    res += '<tr><td>%s/%s</td>' % (s.participation.user.username, s.comment)
    details = r.score_details
    for case in details:
      resource = get_max_resource(case['testcases'])
      res += '<td>%s <small>(%ss / %skb)</small></td>' % (
          to_score(case['max_score'] * case['score_fraction']),
          resource[0],
          resource[1] // 1024,
      )
    res += '<td>%s</td></tr>\n' % (to_score(r.score))
  res += '</table>\n'
  return res


def get_submissions(session):
  """get_submissions"""
  for try_count in range(0, TRY_LIMIT):
    submissions = session.query(Submission).all()
    submissions = [(s, s.get_result()) for s in submissions]
    if try_count < TRY_LIMIT - 1 and needs_wait(submissions):
      print('Submission still running, wait 5 seconds')
      time.sleep(5)
      continue

  res = ''

  # group by tasks
  tasks = {}
  for s, r in submissions:
    task_id = s.task.name
    if task_id not in tasks:
      tasks[task_id] = []
    tasks[task_id].append((s, r))

  for task, ss in tasks.items():
    res += '<details>'
    res += '<summary><h2>Task: %s</h2></summary>\n' % (task)
    res += '<h3>Overview</h3>\n'
    res += get_overview_table(ss)

    for s, r in ss:
      res += '<h3>Submission: %s/%s (%s points)</h3>\n' % (
          s.participation.user.username, s.comment, to_score(r.score))
      details = r.score_details

      res += '<table border="1">\n'
      for case in details:
        resource = get_max_resource(case['testcases'])
        res += '<tr><td colspan="5">Case %d (%s/%s) <b>Time</b>:%ss <b>Mem</b>:%skb</td></tr>\n' % (
            case['idx'],
            to_score(case['max_score'] * case['score_fraction']),
            case['max_score'],
            resource[0],
            resource[1] // 1024,
        )
        for test in case['testcases']:
          to = get_testcase(session, r.dataset.id, test['idx'])
          ev = r.get_evaluation(to)
          res += '<tr><td>%s</td><td>%ss</td><td>%d kb</td><td>%s</td><td>%s</td></tr>\n' % (
              test['idx'], test['time'],
              (test['memory'] or 0) // 1024, test['outcome'], ev.outcome)
      res += '</table>\n'
    res += '</details>'

  return res


def main():
  """main"""
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  args = init()
  text = ''
  with SessionGen() as session:
    text = get_submissions(session)
  with open(args.output, 'w') as f:
    f.write(text)


main()
