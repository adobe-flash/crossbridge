#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys,os.path,os,getopt,time,subprocess,re

class RunSmokes():
    # shows detailed output for each test
    verbose=False

    # timeout in seconds
    timeout=120

    # test file
    testfile='./runsmokes.txt'

    # config, if None let runtests.py compute
    config=None

    allpasses=0
    allfails=0

    failed=False
    failures=''

    startTime=time.time()

    # list of actual env variables
    envsubs=[]

    def usage(self):
        print('usage: %s [options]' % os.path.basename(sys.argv[0]))
        print('-t --time       time in seconds to run,  tests will keep running until the time has been exceeded')
        print('-f --testfile   file containing commands to run')

    def setoptions(self):
        options = 't:f:hv'
        longoptions= ['time=','testfile=','help','verbose']
        try:
            opts,args = getopt.getopt(sys.argv[1:],options,longoptions)
        except:
            self.usage()
            sys.exit(1)
        for o,v in opts:
            if o in ('-t','--time'):
                self.timeout=int(v)
            if o in ('-f','--testfile'):
                self.testfile=v
            if o in ('-v','--verbose'):
                self.verbose=True
            if o in ('-h','--help'):
                self.usage()
                sys.exit(1)
        return opts

    def __init__(self):
        self.setoptions()

    def run(self):
        print("starting smoke tests")
        print("    test file is %s" % self.testfile)
        if self.timeout > 0:
            print("    will run tests until timeout of %ds is exceeded" % self.timeout)
        else:
            print("    will run tests until all tests are complete, no timeout")
        print('')
        try:
            infile=open(self.testfile,'r')
        except:
            print("ERROR reading file %s" % testfile)
            sys.exit(1)
        for test in infile:
            test=test.strip()
            if len(test)==0 or test[0]=='#':
                continue
            if self.timeout > 0:
                if time.time()-self.startTime>self.timeout:
                    break
            self.runtest(test)
        if self.timeout > 0:
            if time.time()-self.startTime<self.timeout:
                print('\ntests finished after %d seconds, did not exceed timeout of %d seconds' % (time.time()-self.startTime,self.timeout))
            else:
                print('\nexceeded timeout of %d seconds, actual time %d seconds' % (self.timeout,time.time()-self.startTime))
        else:
            print('\ntests finished after %d seconds' % (time.time()-self.startTime))
        infile.close()

    def showstats(self):
        print("\nenvironment variables:")
        for env in self.envsubs:
            print("    %s = %s" % (env,os.environ[env]))
        print("\npasses             : %d" % self.allpasses)
        print("failures           : %d" % self.allfails)
        if self.allfails>0:
            print("\nlist of test failures:\n%s" % self.failures)

    def replaceEnv(self,line):
        envs=re.findall('\$\{[A-Za-z0-9_\-]+\}',line)    
        for env in envs:
            env=env[2:-1]
            val = ''
            if not (env in os.environ):
                if env != 'RTARGS':
                    print("ERROR: environment variable '%s' is not defined, exiting" % env)
                    sys.exit(1)
            else:
                val=os.environ[env]
                if (env in self.envsubs)==False:
                    self.envsubs.append(env)
            line=re.sub('\$\{%s\}' % env, val, line)
        return line

    def scrub(self,out):
        out=re.sub('passes',' passes',out)
        out=re.sub('failures',' failures',out)
        out=re.sub('assertions',' assertions',out)
        out=re.sub('tests skipped',' tests skipped',out)
        return out

    def runtest(self,rawcommand):
        command=self.replaceEnv(rawcommand)
        if os.name=='nt':
            command=command.replace(';','&&')
        detail=""
        out=err=""
        try:
            process=subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
            (out,err)=process.communicate()
            # convert to string from byte for py3 compatibility
            out = out.decode('latin_1')
            err = err.decode('latin_1')
            exitcode=process.returncode
        except:
            detail="ERROR: running test"
            exitcode=1        
        tm=time.time()-self.startTime

        passes=0
        fails=0
        expectfail=0
        unpass=0
        asserts=0
        skips=0
        
        if exitcode!=0:
            detail+=" exitcode=%d" % exitcode
        if self.verbose:
            print("[OUTPUT %s]" % command)
            out=self.scrub(out)
            print(out)
            print("[ENDOUTPUT %s]" % command)
        for line in out.splitlines():
            line=line.strip()
            if re.search('time$',line):
                passes+=1
            if re.search('crash$',line):
                fails+=1
            if re.search("PASSED$",line):
                passes+=1
            if re.search("FAILED$",line):
                fails+=1
            if re.search("^passes",line):
                n=int(line.split()[2])
                if n>0 and passes==0:
                    passes+=1
            if re.search("^failures",line):
                n=int(line.split()[2])
                if n>0 and fails==0:
                    fails+=1
            if re.search("^expected failures",line):
                passes+=1
                expectfail=int(line.split()[3])
                detail+=" ef: %d" % expectfail
            if re.search("^unexpected passes",line):
                fails+=1
                unpass=int(line.split()[3])
                detail+=" up: %d" % unpass
            if re.search("^assertions",line):
                fails+=1
                asserts=int(line.split()[2])
                detail+=" as: %d" % asserts
            if re.search("^tests skipped",line):
                passes+=1
                skips=int(line.split()[3])
                detail+=" sk: %d" % skips
        detail="p:%d f:%d %s" % (passes,fails,detail)
        if passes==0 or fails>0 or exitcode!=0:
            if fails==0:
                fails+=1
            status="failed"
            self.failed=True
            out=self.scrub(out)
            self.failures+="%s\n%s\n[OUTPUT]\n%s%s\n[END OUTPUT]\n" % (rawcommand,command,out,err)
        else:
            status="passed"
        print("%s %ds %s %s" % (status,tm,rawcommand,detail))
        sys.stdout.flush()
        if fails>0:
            self.allfails+=1
        else:
            self.allpasses+=1

## main
r = RunSmokes()
r.run()
r.showstats()
if r.failed:
    sys.exit(1)
