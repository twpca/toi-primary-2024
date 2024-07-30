# Environments

## Prerequisite

* judge-runner & lualatex
  * [docker](https://docs.docker.com/get-docker/)
* cms
  * [vagrant](https://developer.hashicorp.com/vagrant/downloads)
  * virtualbox

## judge-runner

```
$ cd {PROJECT_ROOT}
$ pushd judge-runner
$ sudo ./build.sh
$ popd
```

### cgroup issue

docker host 如果是跑 cgroup v2 的話目前 isolate 不支援，就算在 docker 裡面也沒辦法跑。
有些 OS (e.g. Debian bullseye) 目前預設跑 cgroup v2，如果發生這樣的情況最簡單(?)的方法就是強制把版本降回來。

測試自己是不是 v2:

```
$ stat -fc %T /sys/fs/cgroup/
cgroup2fs

$ ls /sys/fs/cgroup/memory/
ls: cannot access '/sys/fs/cgroup/memory/': No such file or directory
```

如果是的話，在 `/etc/default/grub` 的 `GRUB_CMDLINE_LINUX_DEFAULT` 多加入:

```
cgroup_enable=memory swapaccount=1 systemd.unified_cgroup_hierarchy=false systemd.legacy_systemd_cgroup_controller=false
```

然後

```
$ sudo update-grub
$ sudo reboot
```

refs: https://www.debian.org/releases/bullseye/armel/release-notes/ch-information.en.html#openstack-cgroups

## lualatex

(跑很久，如果不需要編譯題本可以跳過)

```
$ cd {PROJECT_ROOT}
$ pushd lualatex
$ ./download_font.sh # 下載約 3GB 的字體
$ sudo ./build.sh
$ popd
```

## cms

hint: `cms/Vagrantfile` 建議邊看邊改，裡面很多東西是寫死的...

```
$ cd {PROJECT_ROOT}
$ pushd lualatex
$ vagrant up
$ popd
```
