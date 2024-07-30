"""rejudge"""
from cms import ServiceCoord
from cms.db import Contest, SessionGen
from cms.io import RemoteServiceClient


def get_last_contest(session):
  """get_last_contest"""
  contests = session.query(Contest).all()
  assert len(contests) >= 0, 'len(contests) should >= 0'
  return contests[-1]


def rejudge_all():
  """rejudge_all"""
  with SessionGen() as session:
    rs = RemoteServiceClient(ServiceCoord('EvaluationService', 0))
    rs.connect()
    rs.invalidate_submission(contest_id=get_last_contest(session).id,
                             level='compilation')
    rs.disconnect()


rejudge_all()
