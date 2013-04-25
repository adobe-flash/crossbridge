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



# Since all is good, lets post the builds
ssh stage.mozilla.org "~/setupbuilds.sh $branch $change-$changeid"


# Release
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_release $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_release
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_release $shell_release"

# Release_Debugger
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_release_debugger $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_release_debugger
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_release_debugger $shell_release_debugger"

# Debug
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_debug $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_debug
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_debug $shell_debug"

#Debug_Debugger
. ../all/util-upload-scp-mozilla.sh $buildsdir/$change-${changeid}/$platform/$shell_debug_debugger $scp_mozilla/$branch/$change-${changeid}/$platform/$shell_debug_debugger
echo "url: ftp://ftp.mozilla.org/pub/js/tamarin/builds/$branch/$change-${changeid}/$platform/$shell_debug_debugger $shell_debug_debugger"

