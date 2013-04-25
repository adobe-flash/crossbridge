#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1



# Release_mips-linux
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_release $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_release
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_release $shell_release"

# Debug_mips-linux
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_debug $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_debug
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_debug $shell_debug"

