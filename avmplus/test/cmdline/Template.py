#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

# template for testcase in cmdline testsuite
# the testsuite is for testing the avmshell or other executables, works by creating a python script containing:
#  - the executable and any arguments
#  - optionally any input text to be sent to the process
#  - optionally a list of string regexp pattern matched against the stdout
#  - optionally a list of string regexp pattern matched against the stdin
#  - optionally the exit code to verify
#  
# the cmdutils.py contains utilities

# below is a template

# import the utilities from cmdutils.py
from cmdutils import *

# define a run() function, this will be called by the runtests.py
def run():
    # instantiate the RunTestLib class, it will contain environment vars AVM,AVMRD,ASC,GLOBALABC,SHELLABC
    r=RunTestLib()
    # create any number of testcases
#    r.run_test(
#                name='testname',
#                command='the executable and any arguments',
#  optional              expectedcode=1,
#  optional              expectedout=['Error #1502']
#  optional              expectederr=[]
#  optional              cwd='testdata'
#               )

    r.run_test(name='shell usage',
               command=r.avm,
               expectedcode=1,
               expectedout=['.*\[-Ojit\].*use jit always.*',
                            '.*\[--\] application args'
                            ]
               )

# by specifying a main can run this test individually
if __name__ == '__main__':
    r=RunTestLib()
#   if any testdata/.as files need to be compiled to .abc use the .compile method
#    r.compile("testdata/sleep.as")
    run()
