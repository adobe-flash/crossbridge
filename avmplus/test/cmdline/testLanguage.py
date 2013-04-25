#!/usr/bin/env python

#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    if r.avmrd==None:
        print("environment variable 'AVMRD' is not set")
        print("SKIPPED all tests")
        return

    r.run_test(
      'french',
      '%s -Dlanguage fr testdata/rt_error.abc'%r.avmrd,
      expectedout=['RangeError: Error #1002: L\'argument precision doit']
    )
    
    r.run_test(
      'italian',
      '%s -Dlanguage it testdata/rt_error.abc'%r.avmrd,
      expectedout=['RangeError: Error #1002: L\'argomento precision deve essere compreso tra']
    )
    
    r.run_test(
      'english',
      '%s -Dlanguage en testdata/rt_error.abc'%r.avmrd,
      expectedout=['RangeError: Error #1002: Number.toPrecision has a range of 1 to 21.']
    )

    
if __name__ == '__main__':
    r=RunTestLib()
    r.compile("testdata/rt_Error.as",None,"")
    run()
