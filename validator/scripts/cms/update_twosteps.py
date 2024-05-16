"""update_twosteps"""

import argparse
import os

from cms.db import SessionGen, Manager
from cms.db.filecacher import FileCacher
from cmscontrib import importing


def init():
  """init"""
  parser = argparse.ArgumentParser()
  parser.add_argument('task', type=str, help='task')
  parser.add_argument('eval', type=str, help='eval')
  parser.add_argument('stub', type=str, help='stub')
  return parser.parse_args()


def main():
  """main"""
  arg = init()

  file_cacher = FileCacher()
  with SessionGen() as session:
    task = importing.task_from_db(arg.task, session)
    for ds in task.datasets:
      ds.task_type = 'TwoSteps'
      ds.task_type_parameters = [arg.eval]

      for fn in os.listdir(arg.stub):
        fn = os.path.join(arg.stub, fn)
        digest = file_cacher.put_file_from_path(
            fn, 'Stub %s for task %s' % (fn, arg.task))
        ds.managers[os.path.basename(fn)] = Manager(os.path.basename(fn),
                                                    digest)

    session.commit()


main()
