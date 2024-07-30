"""update_contest_language"""

import argparse

from cms.db import Contest, SessionGen


def init():
  """init"""
  parser = argparse.ArgumentParser()
  parser.add_argument('langs', help='langs', nargs='+')
  return parser.parse_args()


def get_last_contest(session):
  """get_last_contest"""
  contests = session.query(Contest).all()
  assert len(contests) >= 0, 'len(contests) should >= 0'
  return contests[-1]


def main():
  """main"""
  arg = init()
  with SessionGen() as session:
    contest = get_last_contest(session)
    contest.languages = arg.langs
    session.commit()


main()
