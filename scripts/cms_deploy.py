"""cms_deploy"""
import argparse
import os
import shutil
import subprocess
import sys
from . import common


def init_args():
  """init_args"""
  parser = argparse.ArgumentParser()
  parser.add_argument('server', help='server name')
  return parser.parse_args()


def get_server(cms_conf, name):
  """get_server"""
  for i in cms_conf['test_servers']:
    if i.get('name') == name:
      return i
  return None


def copy_key(src):
  """copy_key"""
  os.makedirs(os.path.expanduser('~/.ssh'), exist_ok=True)
  res = os.path.expanduser('~/.ssh/test_key')
  shutil.copy2(src, res)
  os.chmod(res, 0o600)
  return res


def ssh(host, user, key, *args, **kwargs):
  """ssh"""
  cmd = [
      'ssh',
      '-o',
      'StrictHostKeyChecking no',
      '%s@%s' % (user, host),
      '-i',
      key,
  ]
  if kwargs.get('option'):
    cmd += ['-o', kwargs.get('option')]
  if kwargs.get('port'):
    cmd += ['-p', kwargs.get('port')]
  if kwargs.get('cwd'):
    cmd += ['cd', kwargs.get('cwd'), '&&']
  cmd += list(args)
  print(cmd)
  subprocess.check_call(cmd)


def scp(key, src, dst, *args, **kwargs):
  """scp"""
  cmd = ['scp', '-o', 'StrictHostKeyChecking no', '-i', key]
  if kwargs.get('option'):
    cmd += ['-o', kwargs.get('option')]
  if kwargs.get('port'):
    cmd += ['-P', kwargs.get('port')]
  cmd += [src, dst]
  print(cmd)
  subprocess.check_call(cmd)


def main():
  """main"""
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', buffering=1)
  cms_conf = common.load_cms_conf()
  args = init_args()
  test_server = get_server(cms_conf, args.server)

  host = test_server['host']
  user = test_server['user']
  key = copy_key(test_server['key'])
  del test_server['host'], test_server['user'], test_server['key']
  tmpdir = '/tmp/cms_test'

  ssh(host, user, key, 'rm', '-rf', '%s' % (tmpdir), **test_server)
  ssh(host, user, key, 'mkdir', tmpdir, **test_server)
  scp(key, 'cms_artifact.zip', '%s@%s:%s' % (user, host, tmpdir), **test_server)
  ssh(host, user, key, 'unzip', os.path.join(tmpdir, 'cms_artifact.zip'), '-d',
      tmpdir, **test_server)
  ssh(host, user, key, 'chmod', '+x', os.path.join(tmpdir, '*.sh'),
      **test_server)
  ssh(host, user, key, './00-init.sh', cwd=tmpdir, **test_server)
  ssh(host, user, key, './01-import.sh', cwd=tmpdir, **test_server)
  ssh(host, user, key, 'sudo ./02-import_tests.sh', cwd=tmpdir, **test_server)
  #ssh(host, user, key, 'sudo', 'systemctl', 'restart',
  #    'cmsResourceManager.service', **test_server)
  ssh(host, user, key, 'python3', 'cms/rejudge.py', cwd=tmpdir, **test_server)
  ssh(host, user, key, './03-gen_report.sh', cwd=tmpdir, **test_server)
  scp(key, '%s@%s:%s' % (user, host, os.path.join(tmpdir, 'result.html')), '.',
      **test_server)
  ssh(host, user, key, 'rm', '-rf', '%s' % (tmpdir), **test_server)

  if not os.path.exists('result.html'):
    return 1
  return 0


if __name__ == '__main__':
  sys.exit(main())
