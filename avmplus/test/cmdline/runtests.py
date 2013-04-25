#!/usr/bin/env python
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
from os import listdir
import testAvmShell,testDebugger,testShellSystem
class RunTests (RunTestLib):
    def compileAll(self):
        print("compile all")
        # for testShellSystem.py
        r.compile("testdata/exec.as")
        r.compile("testdata/exit.as")
        r.compile("testdata/readline.as")
        r.compile("testdata/trace1.as")
        r.compile("testdata/write.as")
        r.compile("testdata/argv.as")
        # for testAvmShell.py
        r.compile("testdata/sleep.as")
        # for testDebuggerShell.py
        r.compile("testdata/debug.as",None,"-d")
        # for testMemstats.py
        r.compile("testdata/memstats.as",None)
        # for testLanguage.py
        r.compile("testdata/rt_error.as",None)

    def runAll(self):
        list=os.listdir(".")
        for f in list:
            if re.match("test.*\.py$",f):
                if f in self.testconfig:
                    (type,notes)=self.testconfig[f]
                    if type=='skip':
                        print("%-30s SKIPPED, %s" % (f,notes))
                        continue
                cl=f[0:f.rindex('.')]
                exec("import " + cl)
                print("%s.run()" % cl)
                exec(cl + ".run()")

if __name__ == "__main__":
    r=RunTests()
    r.compileAll()
    r.runAll()
