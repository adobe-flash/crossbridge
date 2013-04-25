#!/usr/bin/env python
# -*- Mode: Python; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys,os
thisdir = os.path.dirname(__file__)
rootdir = thisdir + "/.."
sys.path.append(rootdir)
import utils.exactgc

utils.exactgc.gen_shell(rootdir + '/generated')


