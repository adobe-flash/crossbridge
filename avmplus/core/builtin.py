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

classpath = os.environ.get('ASC')
if classpath == None:
    classpath = "../utils/asc.jar"
    #print "ERROR: ASC environment variable must point to asc.jar"
    #exit(1)

javacmd = "java -ea -DAS3 -DAVMPLUS -classpath "+classpath
asc = javacmd+" macromedia.asc.embedding.ScriptCompiler "

print("ASC="+classpath)
print("Building builtins...")

# https://bugzilla.mozilla.org/show_bug.cgi?id=697977
if len(sys.argv) == 1:
    print('To build the float/float4 enabled builtins pass the following:');
    print('    >$ ./builtin.py -config CONFIG::VMCFG_FLOAT=true -abcfuture');
    print('');
    print('To compile the builtins without float/float4 support:');
    print('    >$ ./builtin.py -config CONFIG::VMCFG_FLOAT=false');
    exit(1);

if warn_notwriteable('../generated/builtin.abc'):
    sys.exit(0)   # exit 0 so build will continue
    
configs = " ".join(sys.argv[1:])


os.system(asc + " -builtin -apiversioning -out builtin builtin.as Math.as Error.as Date.as RegExp.as JSON.as XML.as IDataInput.as IDataOutput.as ByteArray.as Proxy.as flash_net_classes.as Dictionary.as IDynamicPropertyOutput.as IDynamicPropertyWriter.as DynamicPropertyOutput.as ObjectInput.as ObjectOutput.as IExternalizable.as ObjectEncoding.as concurrent.as PromiseChannel.as " + configs)

rm("builtin.h")
rm("builtin.cpp")
mv("builtin.abc", "../generated/builtin.abc")

print("Generating native thunks...")
os.system("python ../utils/nativegen.py ../generated/builtin.abc")

print("Done.")
