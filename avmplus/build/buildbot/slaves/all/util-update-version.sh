#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

cd $basedir/core
mv avmplusVersion.h avmplusVersion.h.orig
sed "s/cyclone/${change}:${changeid}/g" avmplusVersion.h.orig > avmplusVersion.h

# Move back into the default location
cd $basedir/build/buildbot/slaves/scripts