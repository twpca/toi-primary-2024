"""update_task_score"""

import argparse

from cms.db import Dataset, Testcase, SessionGen
from cmscontrib import importing


def init():
  """init"""
  parser = argparse.ArgumentParser()
  parser.add_argument('task', type=str, help='task')
  parser.add_argument('score_type', type=str, help='score_type')
  parser.add_argument('score_type_parameters',
                      type=str,
                      help='score_type_parameters')
  return parser.parse_args()


def main():
  """main"""
  arg = init()

  with SessionGen() as session:
    task = importing.task_from_db(arg.task, session)

    for ds in task.datasets:
      ds.score_type = arg.score_type
      ds.score_type_parameters = eval(arg.score_type_parameters)
      session.commit()

      # set public cases
      for ts in ds.testcases:
        cases = session.query(Testcase).filter(Testcase.codename == ts).filter(
            Testcase.dataset_id == ds.id).all()
        for case in cases:
          if case.codename.startswith('0_') or case.codename.startswith('00_'):
            case.public = True
            session.commit()


main()
