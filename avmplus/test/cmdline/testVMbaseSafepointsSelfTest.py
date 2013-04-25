#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test('VMbaseSafepoints(-Dselftest=vmbase,safepoints)','%s -Dselftest=vmbase,safepoints' % r.avm,expectedcode=0,
               expectedout=[
                                "\['start', 'vmbase', 'safepoints'\]",
                                "\['test', 'vmbase', 'safepoints', 'simple'\]",
                                "\['pass', 'vmbase', 'safepoints', 'simple'\]",
                                "\['test', 'vmbase', 'safepoints', 'producer_consumer'\]",
                                "\['pass', 'vmbase', 'safepoints', 'producer_consumer'\]",
                                "\['test', 'vmbase', 'safepoints', 'nested_producer_consumer'\]",
                                "\['pass', 'vmbase', 'safepoints', 'nested_producer_consumer'\]",
                                "\['end', 'vmbase', 'safepoints'\]"
                            ]
   )

if __name__ == '__main__':
    r=RunTestLib()
    run()
