#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
import sys,socket,os,getopt,time

host=''
port=0
if os.environ.has_key("WINRSSERVERHOST"):
    host=os.environ.get("WINRSSERVERHOST")
if os.environ.has_key("WINRSSERVERPORT"):
    port=int(os.environ.get("WINRSSERVERPORT"))
timeout=-1
winrs_host=''
winrs_user=''
winrs_pass=''

usage='''
winrs.py args ...
--port=
--timeout=
--host=
--cmd=
--file=
--skipExit
'''
cmd='winrs '
longoptions=['port=','host=','cmd=','timeout=','file=','skipexit']
skipExit=False
infile=None

ctr=0
while ctr<len(sys.argv):
    arg=sys.argv[ctr]
    if arg.startswith('-r:'):
        winrs_host=arg[3:]
        del sys.argv[ctr]
    elif arg.startswith('-u:'):
        winrs_user=arg[3:]
        del sys.argv[ctr]
    elif arg.startswith('-p:'):
        winrs_pass=arg[3:]
        del sys.argv[ctr]
    else:
        ctr+=1
try:
    opts,args=getopt.getopt(sys.argv[1:],[],longoptions)
except:
    print(usage)
    sys.exit(1)
for o,v in opts:
    if o in ('--file'):
        infile=v
    if o in ('--port'):
        port=int(v)
    if o in ('--host'):
        host=v
    if o in ('--cmd'):
        cmd=v
    if o in ('--timeout'):
        try:
            timeout=int(v)
        except:
            None
    if o in ('--skipexit'):
        skipExit=True

for arg in args:
   cmd+=arg+' '

if len(args)==0 and infile==None and cmd=='':
    print("must specify an input file or a command")
    print(usage)
    sys.exit(1)

if host=='':
   print("must specify host through --host or environment variable WINRSSERVERHOST")
   sys.exit(1)

if port==0:
   print("must specify port through --port or environment variable WINRSSERVERPORT")
   sys.exit(1)


start=time.time()
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    s.connect((host,port))
except:
    print("could not connect to host=%s port=%s" % (host,port))
    sys.exit(1)

s.send("set timeout -1\n")
start=time.time()
result=''
while True:
    result+=s.recv(1024)
    if result.find('$'):
        break
    if time.time()-start>30:
        print("error failed to set timeout")
        sys.exit(1)
    time.sleep(.5)


if infile!=None:
    lines=open(infile)
    count=0
    for line in lines:
        if line=='':
            continue
        s.send(line+'\n')
        result=s.recv(1024)
        if result.find("finished")==-1:
            print("error: did not receive ack")
            break
        print('[%d] %s' % (count,line))
        count+=1
    print("done sending file")
else:
    s.send(cmd+'\n')
result=''
exitSent=False
while True:
    result+=s.recv(1024)
    if result.find('$') and exitSent==False:
        exitSent=True
        s.send('exit\n')
        if skipExit:
            break
    if result.find('closing connection')>-1:
        break
    if timeout!=-1 and time.time()-start>timeout:
        result+='timed out after %d' % timeout
        break
    time.sleep(.5)
s.close()
if skipExit:
    code=0
else:
    fnd=result.find('closing connection')
    code=1
    if fnd>-1:
        result=result[0:fnd-3]
        code=0
print(result)
#print("exitcode=%d" % code)
sys.exit(code)


