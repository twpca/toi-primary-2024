"""judge all tests"""
import argparse
import itertools
import os
import sys
import typing
from . import common


class Score:
  """score"""
  Task = typing.Tuple[str, int]  # prob_id, subtask
  Possibility = typing.Tuple[typing.List[Task], int]  # ([Tasks], score)
  scores: typing.List[Possibility] = []

  task_count = 0
  smap: typing.Dict[int, typing.List[Possibility]] = {}  # score -> possiblity
  s100map: typing.Dict[int,
                       typing.List[Possibility]] = {}  # score%100 -> possiblity

  def __init__(self, lst):
    self.scores = lst

    tasks = set()
    for s in self.scores:
      tasks |= self.tasks(s)
      x = s[1]
      if x not in self.smap:
        self.smap[x] = []
      if x % 100 not in self.s100map:
        self.s100map[x % 100] = []
      self.smap[x].append(s)
      self.s100map[x % 100].append(s)
    self.task_count = len(tasks)

  def print_type0(self, limit=100):
    """print_type0: score is multiple of 100"""
    if 0 not in self.s100map:
      return

    first = True
    for p in self.s100map[0]:
      if limit <= 0:
        break
      if first:
        print('\n========== Found score is multiple of 100: ==========')
        first = False
      limit -= 1
      print(p[1], ':', self.tasks_str(p))

  def print_type1(self, limit=100):
    """print_type1: same"""
    first = True
    for _, lst in self.smap.items():
      sz = len(lst)
      for i in range(0, sz):
        for j in range(i + 1, sz):
          if limit <= 0:
            break
          if self.has_intersect(lst[i], lst[j]):
            continue
          if first:
            print('\n========== Found same: ==========')
            first = False
          limit -= 1
          print(lst[i][1], ':', self.tasks_str(lst[i]))
          print(lst[j][1], ':', self.tasks_str(lst[j]))
          print(' ')

  def print_type2(self, limit=100):
    """print_type2: same score%100"""
    first = True
    for _, lst in self.s100map.items():
      sz = len(lst)
      for i in range(0, sz):
        for j in range(i + 1, sz):
          if limit <= 0:
            break
          if lst[i][1] == lst[j][1]:
            continue
          if self.has_intersect(lst[i], lst[j]):
            continue

          # score(p0) <= score(p1)
          p0 = lst[i]
          p1 = lst[j]
          if p0[1] > p1[1]:
            p0, p1 = p1, p0

          remain = self.task_count - len(self.tasks(p0))
          if remain * 100 + p0[1] >= p1[1]:
            if first:
              print('\n========== Found same %100: ==========')
              first = False
            limit -= 1
            print(lst[i][1], ':', self.tasks_str(lst[i]))
            print(lst[j][1], ':', self.tasks_str(lst[j]))
            print(' ')

  @classmethod
  def has_intersect(cls, p1: Possibility, p2: Possibility):
    """has_intersect"""
    s = set()
    for i in p1[0]:
      s.add(i)
    for j in p2[0]:
      if j in s:
        return True
    return False

  @classmethod
  def tasks(cls, score: Possibility):
    """tasks"""
    result = set()
    for p in score[0]:
      result.add(p[0])
    return result

  @classmethod
  def tasks_str(cls, score: Possibility):
    """tasks_str"""
    result = ''
    for p in score[0]:
      if result:
        result += ', '
      result += '%s_%d' % (p[0], p[1])
    return result

  @classmethod
  def all_subset(cls, lst):
    """all_subset"""
    sz = len(lst)
    for cnt in range(0, sz + 1):
      for p in itertools.combinations(lst, cnt):
        yield p

  @classmethod
  def valid_subset(cls, subtasks: set, prob, cms_prob):
    """valid_subset"""
    num_tasks = len(prob['score'])

    if len(subtasks) == 0:
      return False
    if len(subtasks) == num_tasks:
      return False

    if 'inclusive_tests' in cms_prob:
      for task, dependents in cms_prob['inclusive_tests'].items():
        for d in dependents:
          if (int(task) in subtasks) and (int(d) not in subtasks):
            return False
    return True

  @classmethod
  def from_prob(cls, prob_id, prob, cms_prob):
    """from_prob"""
    assert 'score' in prob
    tasks = len(prob['score'])
    result = []

    for p in cls.all_subset(list(range(1, tasks + 1))):
      s = set()
      total = 0
      for i in p:
        s.add(i)
        total += prob['score'][i - 1]

      if cls.valid_subset(s, prob, cms_prob):
        s = [(prob_id, x) for x in s]
        result.append((s, total))
    return cls(result)

  def print(self):
    """print"""
    for i in self.scores:
      print(self.tasks(i))
      print(i[0], ': ', i[1])

  def __mul__(self, other):
    if len(self.scores) == 0:
      return Score(other.scores)
    if len(other.scores) == 0:
      return Score(self.scores)

    result = []
    for i in self.scores:
      for j in other.scores:
        result.append((list(sorted(i[0] + j[0])), i[1] + j[1]))
    return Score(result)


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
  return parser.parse_args()


def main():
  """main"""
  args = init_args()
  problem_conf = common.get_problem_config()
  cms_conf = common.load_cms_conf()

  total = Score([])
  for prob_id, prob in problem_conf.items():
    # ignore problem
    if args.probs and prob_id not in args.probs:
      continue
    if args.ignore_probs and prob_id in args.ignore_probs:
      continue
    if 'score' not in prob:
      continue

    x = Score.from_prob(prob_id, prob, common.get_cms_task(prob_id, cms_conf))
    total *= x

  #total.print()
  total.print_type0()
  total.print_type1()
  total.print_type2()


if __name__ == '__main__':
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  main()
