#!/bin/bash -ex

readonly user=${DEFAULT_USER-$(whoami)}
readonly AS_USER=${1-$user}

# Adds cmsdb password and runs the migrations
migrateSchema() {
  local dbpswd=${1-notsecure}
  cd /home/${AS_USER}

  sed -i "s/your_password_here/${dbpswd}/" provision-files/cms-config/cms.conf

  set -x
  cp provision-files/cms-config/*.conf cms-src/config/

  cd /home/${AS_USER}/cms-src
  python3 prerequisites.py -y install

  cmsInitDB
}

# Create First Admin user
initialAdmUsr() {
  local adminpswd=${2-qwerasdf}
  cmsAddAdmin admin --password "$adminpswd"
}

migrateSchema "$@"
initialAdmUsr "$@"
