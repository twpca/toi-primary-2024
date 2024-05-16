"""parse_configs"""
import json
from . import common


def main():
  """main"""
  problems = common.get_probs()
  print('problems:')
  print(problems)

  cms_conf = common.load_cms_conf()
  print('cms_conf:')
  print(json.dumps(
      cms_conf,
      indent=2,
      ensure_ascii=False,
      sort_keys=True,
  ))


if __name__ == '__main__':
  main()
