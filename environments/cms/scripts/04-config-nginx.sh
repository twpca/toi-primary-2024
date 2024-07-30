#!/bin/bash -ex

readonly user=${DEFAULT_USER-$(whoami)}
readonly SRCDIR=/home/$user/provision-files

cp ${SRCDIR}/nginx/nginx.conf /etc/nginx
cp ${SRCDIR}/nginx/upstreams-cms.conf /etc/nginx/conf.d
cp ${SRCDIR}/nginx/site-*.conf /etc/nginx/sites-available

# Disable defautl site and enable CMS ones
unlink /etc/nginx/sites-enabled/default
ln -s /etc/nginx/sites-available/site-cms.conf /etc/nginx/sites-enabled/cms

systemctl restart nginx
