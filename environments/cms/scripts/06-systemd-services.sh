#!/bin/bash -ex

readonly user=${DEFAULT_USER-$(whoami)}
readonly SRCDIR=/home/$user/provision-files

# Copy systemd units
cp ${SRCDIR}/systemd/* /etc/systemd/system
systemctl daemon-reload

# Enable cmsAdminWebServer, cmsRankingWebServer cmsLogService
systemctl enable cms@AdminWebServer.service cms@RankingWebServer.service cms@LogService.service

# Enable cmsResourceService (with a little naming tweak for semantic purposes)
systemctl enable cmsResourceManager.service