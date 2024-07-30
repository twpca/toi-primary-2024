"""pdf.py"""
import json
import os
import re
import subprocess
import sys
import tempfile
from . import common


def get_probs(conf):
  """get_probs"""
  result = []
  for key in conf:
    if 'pdf' in conf[key] and conf[key]['pdf']:
      result.append(key)
  return result


def get_samples(p):
  """get_samples"""
  samples = []
  tests = common.get_tests(p)
  if '0' not in tests:
    common.print_warning('prob %s has no sample tests' % p)
    return ''

  for x in tests['0']:
    inf = ''
    ouf = ''
    with open(x.input_file) as f1:
      inf = f1.read()
    with open(x.output_file) as f2:
      ouf = f2.read()
    samples.append((inf, ouf))

  result = '\\begin{example}\n'
  for sin, sout in samples:
    result += '\\exmp{\n'
    result += sin
    result += '}{%\n'
    result += sout
    result += '}%\n'
  result += '\\end{example}\n'
  return result


def get_md(p, examp, conf):
  """get_md"""
  md = ''
  fn = 'problems/%s.md' % p

  if not os.path.exists(fn):
    common.print_warning('file %s not exists' % (fn))
    return ''

  with open(fn, encoding='utf-8') as f:
    md = f.read()

  md = re.sub(r'\$`([^$]+)`\$', r'\\begin{math}\1\\end{math}', md)
  md = re.sub(r'```math([^`]+)```', r'\\begin{displaymath}\1\\end{displaymath}',
              md)
  md = re.sub(r'```testformat([^`]+)```',
              r'\\begin{format}\n\\f{\n\1\n}\n\\end{format}', md)

  fig_re = r'(?:!\[([^\[\]]*)\]\(([^()]+\.(?:png|pdf))\))'
  fig_re_scale = fig_re + r'(?:\s*<!--\s*scale=(\S+)\s*-->)'
  fig_re_width = fig_re + r'(?:\s*<!--\s*width=(\S+)\s*-->)'
  md = re.sub(
      fig_re_scale,
      r'\\begin{figure}[h]\n'  #
      r'\\centering\n'  #
      r'\\includegraphics[scale=\3]{\2}\n'  #
      r'\\caption{\1}\n'  #
      r'\\end{figure}\n',
      md)
  md = re.sub(
      fig_re_width,
      r'\\begin{figure}[h]\n'  #
      r'\\centering\n'  #
      r'\\includegraphics[width=\3]{\2}\n'  #
      r'\\caption{\1}\n'  #
      r'\\end{figure}\n',
      md)
  md = re.sub(
      fig_re,
      r'\\begin{figure}[h]\n'  #
      r'\\centering\n'  #
      r'\\includegraphics{\2}\n'  #
      r'\\caption{\1}\n'  #
      r'\\end{figure}\n',
      md)

  # score template
  if 'score' in conf[p]:
    if sum(conf[p]['score']) != 100:
      common.print_warning('Problem %s score_sum = %d' %
                           (p, sum(conf[p]['score'])))
    for i in range(0, len(conf[p]['score'])):
      needle = '{{score.subtask%d}}' % (i + 1)
      replace_to = '$%d$' % conf[p]['score'][i]
      md = md.replace(needle, replace_to)

  try:
    needle = '## 範例測試\n'
    idx = md.index(needle)
    md = md[0:idx + len(needle)] + '\n' + examp + md[idx + len(needle):]
  except ValueError:
    common.print_warning('prob %s has no sample section' % p)
  return md


def md_to_tex(md):
  """md_to_tex"""
  tmpd = tempfile.mkdtemp()
  fin = os.path.join(tmpd, 'universe.md')
  fout = os.path.join('problems/', 'universe.tex')
  with open(fin, 'w', encoding='utf-8') as f:
    f.write(md)

  cmds = [
      'pandoc',
      '-r',
      'markdown-auto_identifiers',
      '-w',
      'latex',
      fin,
      '-o',
      fout  #
  ]
  subprocess.call(cmds)


def main():
  """main"""
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  conf = common.get_problem_config()
  probs = get_probs(conf)
  md = ''
  for prob in probs:
    md += get_md(prob, get_samples(prob), conf) + '\n'
  md_to_tex(md)

  # compile two times
  for _ in range(0, 2):
    subprocess.check_call(['lualatex', '-halt-on-error', 'problems.tex'],
                          cwd='problems/')


if __name__ == '__main__':
  main()
