#!/bin/bash
# -*-Mode: Shell-script; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

# This script is designed to be run from the buildbot/slaves/all dir in redux

set -x

# move to slaves dir
cd ..
if [[ -d "scripts" ]]; then
    rm -r scripts;
fi
