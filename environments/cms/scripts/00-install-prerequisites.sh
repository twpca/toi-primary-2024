#!/bin/bash -ex

readonly user=${DEFAULT_USER-$(whoami)}
readonly SRCDIR=/home/$user/provision-files

initApt () {
  # Disable installing recommended or suggested packages
  cp ${SRCDIR}/apt-get/no-recommend /etc/apt/apt.conf.d/01-norecommend

  # Refresh repository indexes
  apt-get update
}

installReqs () {
  # Base dependencies and libs
  apt-get install -y  build-essential gettext python3.8 python3-setuptools python3.8-dev \
                      iso-codes shared-mime-info cgroup-tools stl-manual \
                      python3-pip libpq-dev libyaml-dev libffi-dev libcups2-dev \
                      libcap-dev unzip zip python3-testresources

  pip3 install toml pyyaml pypdf2

  # Database
  apt-get install -y postgresql postgresql-client

  # php
  add-apt-repository ppa:ondrej/php

  # Contest Languages
  apt-get install -y  openjdk-11-jre openjdk-11-jdk \
                      php7.4-cli php7.4-fpm fpc \
                      haskell-platform rustc mono-mcs

  # kotlin
  readonly AS_USER=${1-$user}
  su "$AS_USER" -c 'curl -s "https://get.sdkman.io" | bash'
  sudo -i -u "$AS_USER" "/bin/bash" -lc "source /home/$AS_USER/.sdkman/bin/sdkman-init.sh && sdk install kotlin"

  # Update pip
  pip3 install --force-reinstall -U pip
  # https://github.com/pypa/setuptools/issues/3772
  # https://stackoverflow.com/questions/75272737
  pip3 install --force-reinstall -U setuptools==58.3.0
  pip3 install --force-reinstall -U wheel

  # Optional: HTTP proxy, database management UI
  apt-get install -y nginx phppgadmin

  # I'm  removing the Printing service dependencies
  # apt-get install -y texlive-latex-base a2ps
}

growDisk () {
  apt-get install -y parted cloud-guest-utils cloud-initramfs-growroot arch-install-scripts
  swapoff -a
  fdisk -l
}

initApt "$@"
growDisk "$@"
installReqs "$@"
