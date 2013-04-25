#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test('trace','%s testdata/trace1.abc'  % r.avm,expectedout=['single line','array line1,array line2','arg1 arg2 arg3','null'])
    r.run_test('write','%s testdata/write.abc' %r.avm,expectedout=['part1part2'])
    r.run_test('exit','%s testdata/exit.abc' %r.avm,expectedcode=5)
    r.run_test('readline','%s testdata/readline.abc' %r.avm,input='test input\n',expectedout=['line=test input'])
    r.run_test('exec','%s testdata/exec.abc' %r.avm,expectedout=['result=0'])
    r.run_test('argv 0','%s testdata/argv.abc --' %r.avm,expectedout=['argv.length=0'])
    r.run_test('argv n','%s testdata/argv.abc -- zero one two"' %r.avm,expectedout=['argv.length=3','argv\[0\]=zero','argv\[1\]=one','argv\[2\]=two'])
    # Bug: https://bugzilla.mozilla.org/show_bug.cgi?id=507454
    #      avmshell commandline args with values can't assume the values are present
    #      Prior to fix the shell would segfault on these tests
    r.run_test('memlimit no value','%s -memlimit' %r.avm,expectedout=['Invalid GC option: -memlimit'],expectedcode=1)
    r.run_test('load no value','%s -load' %r.avm,expectedout=['Invalid GC option: -load'],expectedcode=1)
    r.run_test('gcwork no value','%s -gcwork' %r.avm,expectedout=['Invalid GC option: -gcwork'],expectedcode=1)
    r.run_test('cache_bindings no value','%s -cache_bindings' %r.avm,expectedout=['Unrecognized option -cache_bindings'],expectedcode=1)
    r.run_test('cache_metadata no value','%s -cache_metadata' %r.avm,expectedout=['Unrecognized option -cache_metadata'],expectedcode=1)
    r.run_test('cache_methods no value','%s -cache_methods' %r.avm,expectedout=['Unrecognized option -cache_methods'],expectedcode=1)
    r.run_test('stack no value','%s -stack' %r.avm,expectedout=['Unrecognized option -stack'],expectedcode=1)
    r.run_test('gcstack no value','%s -gcstack' %r.avm,expectedout=['Unrecognized option -gcstack'],expectedcode=1)
    if r.avmrd!=None:
        r.run_test('Dastrace no value','%s -Dastrace' %r.avmrd,expectedout=['Unrecognized option -Dastrace'],expectedcode=1)
        r.run_test('Dlanguage no value','%s -Dlanguage' %r.avmrd,expectedout=['Unrecognized option -Dlanguage'],expectedcode=1)

    

if __name__ == '__main__':
    r=RunTestLib()
    r.compile("testdata/exec.as")
    r.compile("testdata/exit.as")
    r.compile("testdata/readline.as")
    r.compile("testdata/trace1.as")
    r.compile("testdata/write.as")
    r.compile("testdata/argv.as")
    run()
