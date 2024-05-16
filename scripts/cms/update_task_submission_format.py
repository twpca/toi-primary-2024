"""update_task_submission_format"""

import argparse

from cms.db import SessionGen
from cmscontrib import importing


def init():
  """init"""
  parser = argparse.ArgumentParser()
  parser.add_argument('task', type=str, help='task')
  parser.add_argument('format', type=str, help='format', nargs='+')
  return parser.parse_args()


def main():
  """main"""
  arg = init()

  with SessionGen() as session:
    task = importing.task_from_db(arg.task, session)
    task.submission_format = [x.strip() for x in arg.format]
    session.commit()


main()
