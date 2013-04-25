#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test('shell -Dtimeout','%s -Dtimeout testdata/sleep.abc' % r.avm,expectedcode=1,expectedout=['Error #1502'])

if __name__ == '__main__':
    r=RunTestLib()
    r.compile("testdata/sleep.as")
    run()
