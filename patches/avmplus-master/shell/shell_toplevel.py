#!/usr/bin/env python
# -*- Mode: Python; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import shutil
import stat
import sys

classpath = os.environ.get('ASC')
if classpath == None:
    classpath = "../utils/asc.jar"
    #print "ERROR: ASC environment variable must point to asc.jar"
    #exit(1)

def mv(oldfile, newfile):
    shutil.copyfile(oldfile,newfile)
    os.remove(oldfile)

def rm(file):
    if os.access(file, os.F_OK) == True:
        os.remove(file)

def warn_notwriteable(file):
    if os.path.exists(file) and not os.stat(file).st_mode & stat.S_IWUSR:
        print("warning: %s is not writeable" % file)
        return True
    return False

javacmd = "java -ea -DAS3 -DAVMPLUS -classpath "+classpath
asc = javacmd+" macromedia.asc.embedding.ScriptCompiler "

print("ASC="+classpath)
print("Building shell_toplevel...")

configs = " ".join(sys.argv[1:])

if warn_notwriteable('../generated/shell_toplevel.abc'):
    sys.exit(0) # exit 0 so build will continue

# compile builtins
os.system(asc+" -import ../generated/builtin.abc -builtin "+configs+" -apiversioning -out shell_toplevel shell_toplevel.as Domain.as IKernel.as ShellPosix.as ../extensions/Sampler.as ../extensions/Trace.as Endian.as Worker.as WorkerDomain.as")

rm("shell_toplevel.h")
rm("shell_toplevel.cpp")
mv("shell_toplevel.abc", "../generated/shell_toplevel.abc")

print("Generating native thunks...")
os.system("python ../utils/nativegen.py ../generated/builtin.abc ../generated/shell_toplevel.abc")

print("Done.")
