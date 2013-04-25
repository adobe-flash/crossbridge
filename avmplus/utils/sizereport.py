#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import subprocess,sys,datetime,re
from os import environ
from glob import glob
from getopt import getopt
from os.path import basename,exists
from platform import system
from socket import *

file="../objdir-release/shell/avmshell.map"
#file="../platform/win32/obj_9/shell/Release/avmplus.map"
if 'MAPFILE' in environ:
    file=environ['MAPFILE'].strip()

globs = {'sizereport':'./sizereport', 'file': file, 'prefix':'', 'socketlog':False,'config':'',
    'product':'unknown', 'verbose':False,'version':'unknown','serverHost':'10.60.48.47','serverPort':1188 }

def usage(c):
    print("usage: %s [options]" % basename(sys.argv[0]))
    print(" -m --map        specify map file to parse")
    print(" -d --socketlog  log results to socketserver")
    print(" -p --prefix     prefix for output line")
    print(" --vmversion     version of vm for socket log")
    print(" --config        configuration string for socket log")
    print(" --product       product (branch) for socket log")
    print(" --verbose       enable verbose messages")
    exit(c)

def runSizeReport():
    p=subprocess.Popen(args=[globs['sizereport'],globs['file']], shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (out,err)=p.communicate()
    exitcode=p.returncode
    if globs['verbose']:
        print('finished running sizereport exit code=%d' % exitcode)
    print('output=%s' % out)
    return (exitcode,out)

def printResult(out):
    print("%s SizeReport " % globs['prefix'])
    for line in out.split('\n'):
        tokens=line.split()
        if len(tokens)>2:
            print("%s %-7s %-10s %-5s" % (globs['prefix'],tokens[0],tokens[1],tokens[2]))
            if re.search("(\.obj|\.dll)", tokens[1]): # look for unclassified files
                print("buildbot_status: WARNINGS") # produce a buildbot warning if any are found

def saveResult(out):
    out="version: %s\n%s" % (globs['version'],out)
    open('lastsizereport.txt','w').writelines(out)

def printResultDiff(out):
    lastlines=open('lastsizereport.txt','r').readlines()
    lastdata={}
    for line in lastlines:
        tokens=line.split()
        if len(tokens)==2:
            print("curr version: %s\nprev version: %s" % (globs['version'],tokens[1]))
        elif len(tokens)>2:
            lastdata[tokens[1]]=int(tokens[0])
    print("%-8s %-6s %-5s %-6s" % ('name','size','chg','percent'))
    for line in out.split('\n'):
        tokens=line.split()
        if len(tokens)>1:
            if tokens[1] in lastdata:
                diff=int(tokens[0])-lastdata[tokens[1]]
                if diff>0:
                    res="+%s" % diff
                else:
                    res="%s" % diff
            else:
                res='n/a'
            print("%-8s %-6s %-5s %-6s" % (tokens[1],convertK(tokens[0]),res,tokens[2]))
            if re.search("(\.obj|\.dll)", tokens[1]): # look for unclassified files
                print("buildbot_status: WARNINGS") # produce a buildbot warning if any are found

def printResultDiffSummary(out):                
    lastlines=open('lastsizereport.txt','r').readlines()
    lastdata={}
    for line in lastlines:
        tokens=line.split()
        if len(tokens)==2:
            print("%s curr version: %s\n%s prev version: %s" % (globs['prefix'],globs['version'],globs['prefix'],tokens[1]))
        elif len(tokens)>2:
            lastdata[tokens[1]]=int(tokens[0])
    print("%s %-8s %-6s %s" % (globs['prefix'],"name","size","change"))
    for line in out.split('\n'):
        tokens=line.split()
        if len(tokens)>1:
            if tokens[1] in lastdata:
                diff=int(tokens[0])-lastdata[tokens[1]]
                if diff==0:
                    print("%s %-8s %-6s nochange" % (globs['prefix'],tokens[1],convertK(tokens[0])))
                else:
                    if diff>0:
                        res="+%s" % convertK(diff)
                    else:
                        res="%s" % convertK(diff)
                    print("%s %-8s %-6s %s" % (globs['prefix'],tokens[1],convertK(tokens[0]),res))
            else:
                print("%s %-8s %-6s" % (globs['prefix'],tokens[1],convertK(tokens[0])))

def convertK(val):
    if abs(int(val))>1024:
        return "%dK" % (int(val)/1024)
    return val
                
def logResult(out):
    os = {'CYGWIN_NT-5.1':'WIN','CYGWIN_NT-5.2':'WIN','CYGWIN_NT-6.0-WOW64':'WIN','Windows':'WIN','Darwin':'MAC','Linux':'LNX','Solaris':'SOL','SunOS':'SOL',}[system()]
    msg=''
    date=datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # for consistency in the performance report, pretend result came from windows performance host
    ip='10.60.147.240';
    for line in out.split('\n'):
        fields=line.split()
        if len(fields)>1:
            msg+='addresult2::sizereport/%s::memory::%s::0::%s::1::%s::%s::%s::%s::%s::%s;' % (fields[1],fields[0],fields[0],os,globs['config'],globs['version'],globs['product'],date,ip)
    msg+='exit;'
    if globs['verbose']:
        print('sending result to socket server: %s' % msg)
    try:
        s = socket(AF_INET, SOCK_STREAM)    # create a TCP socket
        s.settimeout(10)
        s.connect((globs['serverHost'], globs['serverPort'])) # connect to server on the port
        s.send("%s;exit\r\n" % msg)         # send the data
        data = s.recv(1024)
        s.close()
    except :
        print("Socket error occured:")
        print('sending result to socket server: %s' % msg)
        print(sys.exc_info())
        print('buildbot_status: WARNINGS')

    print('finished sending results to socket server')

try:
    opts, args = getopt(sys.argv[1:], "m:dp:h",["map=","prefix=","config=","product=","vmversion=","help","verbose","socketlog"])
except:
    usage(2)

if not args:
    args=["."]
for o,v in opts:
    if o in ("-m","--map"):
        globs['file']=v
    elif o in ("-p","--prefix"):
        globs['prefix']=v
    elif o in ("--vmversion"):
        globs['version']=v
    elif o in ("--config"):
        globs['config']=v
    elif o in ("--product"):
        globs['product']=v
    elif o in ("-d","--socketlog"):
        globs['socketlog']=True
    elif o in ("--verbose"):
        globs['verbose']=True
    else:
        usage(2)

(exitcode,out)=runSizeReport()
if exitcode!=0:
    print("error bad exit code %d" % exitcode)
    sys.exit(1)
if exists('lastsizereport.txt'):
    printResultDiff(out)
    if globs['prefix']!='':
        printResultDiffSummary(out)
else:
    printResult(out)
saveResult(out)
if globs['socketlog']:
    logResult(out)
