#!/bin/bash

set -x

SYSREPOCFG=/usr/bin/sysrepocfg
SYSREPOCTL=/usr/bin/sysrepoctl
LOCATION_COMMON=/opt/dev/common
LOCATION=/opt/dev/challenges/6_get_out

# Install modules
$SYSREPOCTL --install $LOCATION/model_chall_6.yang

$SYSREPOCFG --edit=$LOCATION/nacm.xml --format=xml --datastore=candidate
$SYSREPOCFG --edit=$LOCATION_COMMON/ietf-netconf-server.xml --format=xml --datastore=candidate

$SYSREPOCFG --copy-from candidate --datastore=running
$SYSREPOCFG --copy-from candidate --datastore=startup