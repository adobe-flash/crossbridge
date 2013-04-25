#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os,re,subprocess,os.path,sys,time

class RunTestLib:
    "base class to run cmdline tests"

    __shared_state = {}

    def __init__(self):
        self.__dict__=self.__shared_state
        if self.__shared_state=={}:
            self.firstinit()

    def firstinit(self):
        self.verbose=True
        self.avm=self.checkenv('AVM','path to vm shell')
        self.avmrd=self.checkenv('AVMRD','path to debugger vm shell',False)
        self.asc=self.checkenv('ASC','path to actionscript compiler asc.jar')
        self.gabc=self.checkenv('BUILTINABC','path to generated/builtin.abc')
        self.sabc=self.checkenv('SHELLABC','path to generated/shell_toplevel.abc')
        self.loadConfig()

    def loadConfig(self):
        file=open('testconfig.txt')
        self.testconfig={}
        for line in file:
            if len(line.strip())==0 or line.strip().startswith('#'):
                continue
            fields=line.split(',')
            if len(fields)<3:
                print("incorrect number of fields in line '%s'" % line)
                continue
            self.testconfig[fields[0].strip()]=(fields[1].strip(),fields[2].strip())

    def checkenv(self,env,desc='',required=True):
        val=None
        if env in os.environ:
            val=os.environ[env]
        if val==None:
            if required:
                print("required environment variable '%s' is not set... it is '%s'" % (env,desc))
                sys.exit(1)
            print("optional environment variable '%s' is not set ... it is '%s'" % (env,desc))
        else:
            val=val.strip()
        return val

    def run_command(self,cwd,command,input=None):
        self.verbose_print(command)
        if cwd==None:
            cwd=os.getcwd()
        proc = subprocess.Popen(command.split(), stdout=subprocess.PIPE,stdin=subprocess.PIPE, shell=False,cwd=cwd)
        if input:
            proc.stdin.write(input.encode('latin_1'))
        (stdo,stde)=proc.communicate()
        stdo = stdo.decode('latin_1') if stdo else None
        stde = stde.decode('latin_1') if stde else None
        return (proc.returncode,stdo,stde)

    def run_command_async(self,cwd=None,command=None,input=None,sleep=None):
        self.verbose_print(command)
        if cwd==None:
            cwd=os.getcwd()
        proc = subprocess.Popen(command.split(), stdout=subprocess.PIPE,stdin=subprocess.PIPE, shell=False,cwd=cwd)
        if input!=None:
            proc.stdin.write(input)
        if sleep!=None:
            time.sleep(sleep)
        return proc

    def compile(self,file,imports=None,args=None):
        print("compiling %s" % file)
        if imports==None:
            imports="-import %s -import %s" % (self.gabc,self.sabc)
        if args==None:
            args=''
        cmd="java -jar %s %s %s %s" % (self.asc,imports,args,file)
        cwd=os.getcwd()
        (exitcode,stdo,stde)=self.run_command(cwd,cmd)
        if exitcode!=0:
            print("ERROR: compile error\nstdout=%s\nstderr-%s\n" % (stdo,stde))
            sys.exit(1)
        return (exitcode,stdo,stde)

    def set_verbose(self,v):
        self.verbose=v

    def verbose_print(self,msg):
        if self.verbose:
            print(msg)

    def replace_crlf(self,s):
        return s.replace('\r','<cr>').replace('\n','<lf>')

    def run_test(self,name,command=None,cwd=None,input=None,expectedout=None,expectederr=None,expectedcode=None,actualcode=None,actualout=None,actualerr=None):
        if command!=None:
            (actualcode,actualout,actualerr)=self.run_command(cwd,command,input)
        result=True
        msg=''
        if cwd:
            msg+="cwd: %s\n" % cwd
        msg+="command: %s\n" % command
        if input:
            msg+="input: %s\n" % input
        if expectedcode!=None:
            if expectedcode==actualcode:
                msg+=' exit code matched %d\n' % actualcode
            else:
                result=False
                msg+=" exit code expected '%d' got '%d'\n" % (expectedcode,actualcode)
        elif self.verbose and actualcode!=None:
            msg+=" exit code=%d\n" % actualcode
        if expectedout!=None:
            for eout in expectedout:
                if re.search(eout,actualout):
                    msg+=" output matched expected '%s'\n" % eout
                else:
                    result=False
                    msg+=" output failed to match '%s'\n" % (eout)
        if (self.verbose or result==False) and actualout!=None:
            if len(actualout)>80:
                msg+="[OUTPUT START]\n%s\n[OUTPUT END]\n" % actualout
            else:
                msg+="output='%s'\n" % actualout

        if expectederr!=None:
            for eerr in expectederr:
                if p1.search(eerr,actualerr):
                    msg+=" error matched expected '%s'\n" % eerr
                else:
                    result=False
                    msg+=" error failed to match '%s'\n" % (eerr)
            if (self.verbose or result==False) and actualerr!=None:
                if len(actualerr)>80:
                    msg+="[START STDERR]\n%s\n[END STDERR]\n" % actualerr
                else:
                    msg+="stderr='%s'\n" % actualerr
 
        msg=name + " " + msg

        if os.path.exists("%s.output"%name):
            os.remove("%s.output"%name)
    
        type='normal'
        if name in self.testconfig:
            (type,notes)=self.testconfig[name]
        if type=='skip':
            msg=msg + " SKIPPED, result %s" % result
            print("%-30s SKIPPED, result %s" % (name,result))
        elif result and type=='normal':
            msg=msg + " PASSED"
            print("%-30s PASSED" % name)
        elif result and type=='expectfail':
            msg=msg + " UNEXPECTED PASS : %s" % notes
            print("%-30s UNEXPECTED PASS : %s" % (name,name))
        elif result==False  and type=='expectfail':
            msg=msg + " EXPECTED FAIL " + notes
            print("%-30s EXPECTED FAIL, see %s.output" % (name,name))
        else:
            msg=msg + " FAILED"
            print("%-30s FAILED, see %s.output" % (name,name))
        if result==False:
            out_file=open("%s.output"%name,"w")
            out_file.write(msg)
            out_file.close()


if __name__ == "__main__":
    rt=RunTestLib()
#    rt.run_test("test1",avm,expectedcode=1,expectedout=['avmplus shell','\[-Dforcemir]'])
    rt.run_command(None,'%s -d debug.abc' % rt.avm,'quit\n')
