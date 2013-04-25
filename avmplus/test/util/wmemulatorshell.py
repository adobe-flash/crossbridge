#!/usr/bin/env python
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# script to run an abc test on a windows mobile emulator
# - requires the wmrunner.exe program to be running in the emulator

import os,sys,shutil,time,datetime,random,re

def runTest():
    global startime
    
    dir=None

    # search for an unlocked emulator directory
    while dir==None:
        dirs=os.listdir(dirbase)
        random.shuffle(dirs)    
#        print("SHELL: looking for free emulator : %d" % (time.time()-starttime))
        for eachdir in dirs:
            try:
                sdate=str(datetime.datetime.today())+"\n"
                if os.path.isdir(dirbase+'/'+eachdir) and os.path.exists(dirbase+'/'+eachdir+'/lock')==False:
                    file=open(dirbase+'/'+eachdir+'/lock','w')
                    file.write(sdate)
                    file.close()
                    dir=dirbase+'/'+eachdir
                    time.sleep(.1)
                    file=open(dir+'/lock','r')
                    sdateread=file.read()
                    file.close()
                    if sdateread!=sdate:
                        try:
                            os.unlink(dir+'/lock')
                        except:
                            pass
                        return (-1,'ERROR: writing lock file')
                    time.sleep(.1)
                    if os.path.exists(dir+'/lock')==False: 
                        print("ERROR: lock file does not exist")
                        return (-1,'ERROR: lock file does not exist')
                    break
            except:
                print("ERROR: exception lock file")
                try:
                    os.unlink(dir+"/lock")
                except:
                    pass
                return (-1,"ERROR: exception writing lock file")
        time.sleep(.1)     
#    print("SHELL: running emulator %s : %d" % (dir,time.time()-starttime))
    ddir="\\Storage Card\\media"
    cmdfile=dir+"/nextvm.txt"
    dlog='%s/media/%s.log' % (dir,base)
    dabc='%s/media/%s.abc' % (dir,base)
    exitcodefile='%s.exitcode' % dabc
    ctr=0

    # clean up old log and abc files
    try:
        ctr=0
        while os.path.exists(cmdfile) and ctr<20:
            time.sleep(.1)
            ctr+=1
        if os.path.exists(dlog):
            os.unlink(dlog)
        if os.path.exists(dabc):
            os.unlink(dabc)
        if os.path.exists(exitcodefile):
            os.unlink(exitcodefile)
    except:
        print("ERROR: exception deleting file")
        try:
            os.unlink(dir+'/lock')
        except:
            pass
        return (-1,"ERROR: exception deleting file")

    args=""
    newdlog=None
    for arg in sys.argv[1:-1]:
        if re.search('.abc',arg)!=None:
            shutil.copy(cwd+'/'+arg,dir+'/media')
            if arg.find('/')>-1:
                arg=arg[arg.rfind('/')+1:]
            args+=' \"%s\%s\" ' % (ddir,arg)
            if newdlog==None:
                dlog='%s/media/%s.log' % (dir,arg[0:arg.find('.')])
                newdlog=dlog
                try:
                    if os.path.exists(dlog):
                       os.unlink(dlog)
                except:
                    return (-1,"ERROR: deleting log file %s" % dlog)
        else:
            args+=' '+arg           

    # copy .abc test to the emulator directory
    try:
        shutil.copy(cwd+"/"+abc,dabc)
    except:
        print("ERROR: copying abc file")
        try:
            os.unlink(dir+'/lock')
        except:
            pass
        return (-1,"ERROR: copying abc file")
    try:
        file=open(cmdfile,"w")
        file.write("-log %s \"%s\%s.abc\" " % (args,ddir,base))
        file.close()
    except:
        print("ERROR: write command file failed")
        try:
            os.unlink(dir+'/lock')
        except:
            pass
        return (-1,"ERROR: write command file failed")
        
#   wait until emulator deletes nextvm.txt command file
#    print("SHELL: wrote file %s : %d" % (cmdfile,time.time()-starttime))
    while os.path.exists(cmdfile):
        time.sleep(.1)
        
#    print("SHELL: detected test finished %d" % (time.time()-starttime))
    if os.path.exists(dlog)==False:
        try:
            os.unlink(dir+'/lock')
        except:
            pass
        return (-1,"ERROR: cannot find log %s" % dlog)
    ctr=0
    while os.path.exists(dlog)==False and ctr<50:
        time.sleep(.1)
        ctr+=1

# read the exitcode file
    exitcode=0
    ctr=0
    while os.path.exists(exitcodefile)==False and ctr<50:
        time.sleep(.4)
        ctr+=1
    if os.path.exists(exitcodefile):
        try:
            file=open(exitcodefile,'r')
            exitcodestr=file.read()
            exitcode=int(exitcodestr.strip())
            file.close()
        except:
            print('exception reading exit code file')
            try:
                os.unlink(dir+'/lock')
            except:
                pass
            return (-1,"exception reading exit code file")
    else:
        print("ERROR: cannot find exit code file %s" % exitcodefile)
        if os.path.exists(dir+'/lock'):
            os.unlink(dir+'/lock')
        return (-1,"ERROR: cannot find exit code file %s" % exitcodefile)

#  remove lock, another thread can use the emulator while the shell reads the output log
#    print("SHELL: finished %s : %d" % (abc,time.time()-starttime))
    try:
        os.unlink(dir+'/lock')
    except:
        print("exception deleting %s/lock" % dir)
        return (-1,"exception deleting %s/lock" %dir)

# read and print the log file
    try:
       file=open(dlog,'r')
       sysout=file.read()
       print(sysout)
    except:
       print("ERROR: failed to read log  %s" % dlog)
       return (-1,"ERROR: failed to read log %s" % dlog)
    return (exitcode,"succeeded")


# main
if len(sys.argv)==1:
    print("ERROR: not enough arguments, usage: %s arguments... test_file.abc" % sys.argv[0])
    sys.exit(1)

starttime=time.time()
cwd=os.getcwd()

# abc to test
abc=sys.argv[-1]
# flatten the directory path
abc=abc.replace('/','\\')
base=abc[0:abc.rfind('.')].replace('\\','_')

# find an available emulator, all emulators representing by directories under dirbase
dirbase=cwd+"/../util/emulator_files"
if 'EMULATORDIR' in os.environ:
    dirbase=os.environ['EMULATORDIR'].strip()
if os.path.isdir(dirbase)==False:
    print("ERROR: emulator directory '%s' not found" % dirbase)
    sys.exit(1)

# if .log file is not written try multiple attempts
attempts=0
retrys=0
while retrys<5:
    #print("attempt %d, retry %d" % (attempts,retrys))
    (res,sysout)=runTest()
    #print("exit code=%d reason=%s"  % (res,sysout))
    if res!=-1 and res!=1:
        sys.exit(res)
    attempts+=1
    if res==1:
        retrys+=1
    time.sleep(1)
    try:
        file=open(dirbase+'/../failures.log','a')
        file.write("%s attempt %d %s %s\n" %(str(datetime.datetime.today()),attempts,sys.argv[1:],sysout))
        file.close()
    except:
        print("ERROR: writing failures.log")
    time.sleep(1)
sys.exit(res)

