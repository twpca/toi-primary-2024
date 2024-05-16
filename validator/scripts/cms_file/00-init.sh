#/bin/bash
set -e

cmsDropDB -y
cmsInitDB
cmsAddAdmin admin --password "ZAQ!2wsx"
cmsAddUser -p "p1" "" "" "u1"
