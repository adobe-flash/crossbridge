#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# acts as a proxy to remotely run abc files on android using adb, returns shell output to stdout
# usage: ./android_shell.py <vmargs> file.abc
# assumes the android shell is deployed to /data/local/tamarin/avmshell
#

import os,re,sys,subprocess,killableprocess,datetime,adb_proxy

# Return either the USAGE or the -Dversion information
if len(sys.argv)==1 or sys.argv[1]=='-Dversion' or sys.argv[1]=='-Dselftest':
    if len(sys.argv)==1:
        arg=""
    else:
        arg=sys.argv[1]

    # find all connected devices
    devices=[]
    stdout=adb_proxy.main(['devices'])
    for line in stdout.split('\n'):
        tokens=line.split()
        if len(tokens)==2 and tokens[1]=='device':
            devices.append(tokens[0])

    stdout=adb_proxy.main(["-s %s shell \"cd /data/local/tamarin;./avmshell %s\"" % (devices[0],arg)])
    print(stdout)
    sys.exit(0)



args=""
adbargs=""

threadid=0
androidid=None
filelist=[]
for arg in sys.argv[1:]:
    if arg.startswith("--threadid="):
        try:
            threadid=int(arg[11:])
        except:
            print(sys.exc_info)
            sys.exit(1)
    elif arg.startswith("--androidid="):
        androidid=arg[12:]
        adbargs="-s %s" % androidid
    elif re.search(".abc",arg):
        flatfile=arg.replace('/','.')
        filelist.append(arg)
        stdout=adb_proxy.main(["%s push %s /data/local/tamarin/%s" % (adbargs,arg,flatfile)])
        args+=" %s" % flatfile
    else:
        args+=" %s" % arg


stdout=adb_proxy.main(["%s shell \"/data/local/tamarin/android_runner.sh %s\"" % (adbargs,args)])
for line in stdout.split('\n'):
    if re.search("EXITCODE=",line):
        exitcode=1
        try:
            exitcode=int(line[line.find("EXITCODE=")+9:])
        except:
            print(sys.exc_info())
        sys.exit(exitcode)
    else:
        print(line)
            

print("error: stdout did not contain EXITCODE=")
sys.exit(1)
