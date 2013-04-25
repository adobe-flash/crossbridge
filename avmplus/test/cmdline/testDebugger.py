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
      'debugger basic',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='quit\n',
      expectedout=['6:  public class debug','(asdb)']
    )

    r.run_test(
      'debugger list',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='list\nlist 10\nquit\n',
      expectedout=['10:     public function pubfunc','12:[ ]+privfunc\(\);']
    )

    r.run_test(
      'debugger break',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 11\ninfo break\nquit\n',
      expectedout=['Breakpoint 1: file testdata.debug.as, 11.']
    )

    r.run_test(
      'debugger stacktrace',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='bt\nquit\n',
      expectedout=['#0   global.*global\$init']
    )

    r.run_test(
      'debugger break2',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 11\ndelete 1\nbreak 1\nbreak 15\ninfo break\ncontinue\nquit\n',
      expectedout=['Breakpoint 1 deleted.','2 at testdata.debug.as:15','Could not locate specified line.']
    )

    r.run_test(
      'debugger next',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 8\ncontinue\nnext\nstep\nquit\n',
      expectedout=['8:.*print\("in constructor\(\)"\);','9:.*}']
    )

    r.run_test(
      'debugger locals',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nnext\ninfo locals\nnext\ninfo locals\nquit\n',
      expectedout=['local1 = undefined','local2 = 10','local2 = 15']
    )

    r.run_test(
      'debugger arguments',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nnext\ninfo arguments\nquit\n',
      expectedout=['arg1 = 110','arg2 = 115']
    )

    r.run_test(
      'debugger exception',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='continue\nquit\n',
      expectedout=['Exception has been thrown:']
    )
  
    r.run_test(
      'debugger where',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nwhere\nquit\n',
      expectedout=['locals\(arg1=110,arg2=115\) at testdata.debug.as:19','init\(\) at testdata.debug.as:39']
    )
    r.run_test(
      'debugger bt',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nbt\nquit\n',
      expectedout=['locals\(arg1=110,arg2=115\) at testdata.debug.as:19','init\(\) at testdata.debug.as:39']
    )

    r.run_test(
      'debugger finish',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nfinish\nwhere\nquit\n',
      expectedout=['#0   global@[0-9a-z]+.global\$init\(\) at testdata.debug.as:39']
    )

    r.run_test(
      'debugger set',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='break 19\ncontinue\nnext\nset local2 = 5\ninfo locals\nquit\n',
      expectedout=['local2 = 5']
    )

    r.run_test(
      'debugger print',
      '%s -d testdata/debug.abc'%r.avmrd,
      input='print Function.\nquit\n',
      expectedout=['const length = 1','function get prototype\(\) = Function']
    )

if __name__ == '__main__':
    r=RunTestLib()
    r.compile("testdata/debug.as",None,"-d")
    run()
