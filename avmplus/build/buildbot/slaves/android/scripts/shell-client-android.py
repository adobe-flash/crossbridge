#!/usr/bin/env python
# -*- python -*-
# ex: set syntax=python:
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys,socket,os,time,re

port=None
host=None
if os.environ.has_key("SHELLPORT"):
    try:
        port=int(os.environ.get("SHELLPORT"))
    except:
        print("error: parsing SHELLPORT")
if os.environ.has_key("SHELLSERVER"):
    host=os.environ.get("SHELLSERVER")

if len(sys.argv)>1 and re.search('^--shellserver=',sys.argv[1]):
    shellserver=sys.argv[1][14:]
    if shellserver.find(':')>-1:
        host=shellserver[0:shellserver.find(':')]
        try:
            port=int(shellserver[shellserver.find(':')+1:])
        except:
            True
        sys.argv=sys.argv[1:]

if (host==None or port==None):
    print("error: SHELLPORT and SHELLSERVER must be set")
    sys.exit(1)

args=""
for item in sys.argv[1:]:
    args+=item+" "


s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))
s.send("abc_android %s" % args)
result=''
timeout=300
starttime=time.time()
while True:
    newdata=s.recv(1024)
#    print("data: "+newdata)
    result+=newdata
    if re.search("EXITCODE=[0-9]+\s",result):
        break
    if result.find("-- application argument")>-1:
        break
    if result.find("\n$")>-1:
        break    
    if time.time()-starttime>timeout:
        print("error: timeout detected")
        break
s.close()
if re.search("EXITCODE=[0-9]+\s",result):
    exitcode=result[result.find("EXITCODE")+9:]
    if exitcode.find("$")>-1:
        exitcode=exitcode[0:exitcode.find("$")]
    try:
        exitcode=int(exitcode.strip())
    except:
        True
    result=result[0:result.find("EXITCODE")]
else:
    exitcode=0
print(result)
sys.exit(exitcode)



