#!/usr/bin/env python
# -*- python -*-
# ex: set syntax=python:
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys,socket,os,time

port=None
host=None
if os.environ.has_key("SHELLPORT"):
    try:
        port=int(os.environ.get("SHELLPORT"))
    except:
        print("error: parsing SHELLPORT")
if os.environ.has_key("SHELLSERVER"):
    host=os.environ.get("SHELLSERVER")

if (host==None or port==None):
    print("error: SHELLPORT and SHELLSERVER must be set")
    sys.exit(1)

args=""
for item in sys.argv[1:]:
    args+=item+" "


s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host,port))
s.send("%s" % args)
result=''
timeout=300
starttime=time.time()
exitcode=0
while True:
    result+=s.recv(1024)
    if result.find("setup finished")>-1 or result.find("ok:")>-1:
        break
    if result.find("setup failed")>-1 or result.find("error:")>-1:
        exitcode=1
        break
    if time.time()-starttime>timeout:
        print("ERROR: timed out after %d sec" % timeout)
        exitcode=1
        break
print(result)
s.close()
sys.exit(exitcode)



