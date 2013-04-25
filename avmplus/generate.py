#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# generate.py - generate all exactgc tracers
import sys,os
rootdir = os.path.dirname(__file__)
sys.path.append(rootdir)
import utils.exactgc

utils.exactgc.gen_builtins(rootdir + '/generated')
utils.exactgc.gen_shell(rootdir + '/generated')

