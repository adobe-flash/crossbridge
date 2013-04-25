#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#


import getopt,os,re,subprocess,sys,datetime,shutil,platform,shutil

class CodeCoverageRunner:
    compile_csvfile="../all/codecoverage-compile.csv"
    test_csvfile="../all/codecoverage-test.csv"
    root_dir="../../../../"
    script_dir="./"
    buildnum=None
    bullseye_dir=None
    builds_dir=None
    platform=None
    exclude_regions=None
    
    options='b:'
    longOptions=['buildnum=','compilecsv=','testcsv=','buildsdir=']
    
    def __init__(self):
        self.parseOptions()
        self.determineOS()

        # Make sure that bullseyedir is set in the environment
        env=os.environ
        if env.has_key("bullseyedir") == False:
            print("error: must set bullseyedir environment variable to the bullseye/bin directory")
            sys.exit(1)

        self.bullseye_dir=env["bullseyedir"]
        self.exclude_regions=env["coverage_exclude_regions"]

        if self.builds_dir==None:
            # Determine the hash of the change, required to know what the build directory is
            out,exit = self.run_pipe(cmd="hg log -r %s | head -n 1 | awk -F: '{print $3}'" % self.buildnum)
            if out[0].find("unknown revision")!=-1:
                print "Unable to determine change has for hg revision %s" % (self.buildnum)
                sys.exit(1)
            self.builds_dir="../../../../../builds/%s-%s/%s" % (self.buildnum, out[0], self.platform)

            # Convert all relative paths to absolute
            self.builds_dir=os.path.abspath(self.builds_dir)
            self.root_dir=os.path.abspath(self.root_dir)
            self.script_dir=os.path.abspath(self.script_dir)

        if self.platform=='windows':
            self.compile_csvfile=self.convertFromCygwin(self.compile_csvfile)
            self.test_csvfile=self.convertFromCygwin(self.test_csvfile)
            self.root_dir=self.convertFromCygwin(self.root_dir)
            self.script_dir=self.convertFromCygwin(self.script_dir)
            self.bullseye_dir=self.convertFromCygwin(self.bullseye_dir)
            self.builds_dir=self.convertFromCygwin(self.builds_dir)

        
        compile=True
        test=True

        
        if compile:
            print('parsing %s...' % self.compile_csvfile)
            lines=open(self.compile_csvfile).read()
            # throw away header(1st)
            lines=lines.split('\n')[1:]
            for line in lines:
                if line.startswith("#"):
                    continue
                tokens=self.mysplit(line)
                # current format is:
                # display_name, shell_name, configure options, env_script, notes
                if len(tokens)<5:
                    continue
                shellname=tokens[1].strip()
                configure_args=tokens[2].strip()
                env_script=tokens[3].strip()
                self.compile_shell(shellname, configure_args, env_script)
            
        if test:
            print('parsing %s...' % self.test_csvfile)
            lines=open(self.test_csvfile).read()
            # throw away header(1st)
            lines=lines.split('\n')[1:]
            for line in lines:
                if line.startswith("#"):
                    continue
                tokens=self.mysplit(line)
                # current format is:
                # shell_name, suite, vm_args, config_string, script_args
                if len(tokens)<5:
                    continue
                shellname=tokens[0].strip()
                suite=tokens[1].strip()
                vm_args=tokens[2].strip()
                config_string=tokens[3].strip()
                script_args=tokens[4].strip()

                self.test_shell(shellname, suite, vm_args, config_string, script_args)

        # Merge all of the coverage files together
        # let the build system call ../all/codecoverage_process.sh
        #self.merge_coverage()
        
    def usage(self,c):
        print('usage: %s [options]' % sys.argv[0])
        print('    -b --buildnum build number that is being built')
        print('    --compilecsv  csv file defining what to compile,')
        print('                  defaults to %s' % self.compile_csvfile)
        print('    --testcsv     csv file defining what test to run,')
        print('                  defaults to %s' % self.test_csvfile)
        print('    --buildsdir   location to store final binaries,')
        print('                  shell and coverage file')
        sys.exit(c)

    def parseOptions(self):
        try:
            opts,args = getopt.getopt(sys.argv[1:], self.options,self.longOptions)
        except:
            print(sys.exc_info()[1])
            self.usage(2)

        for o,v in opts:
            if o in ('-b', '--buildnum'):
                self.buildnum=v
            if o in ('--compilecsv',):
                self.compile_csvfile=v
            if o in ('--testcsv',):
                self.test_csvfile=v
            if o in ('--buildsdir',):
                self.builds_dir=v
            
        if self.buildnum==None:
            print('--buildnum must be set')
            self.usage(1)
            
    def compile_shell(self, shellname, configure_args, env_script):
        '''
        Compile the shell with the given configure args.
        '''
        print "\n\n========================================"
        print "About to compile using the following: "
        print "shellname: "+shellname
        print "configure_args: "+configure_args
        print "env_script: "+env_script
        
        # Make sure that Bullseye is in the path
        env=os.environ
        if env["PATH"].find(self.bullseye_dir)==-1:
            env["PATH"]=self.bullseye_dir+":"+env["PATH"]

        # Setup the covfile based on the shellname being built
        env["COVFILE"]="%s/%s-%s.cov" % (self.builds_dir, self.platform, shellname)
        if os.path.exists(env["COVFILE"]):
            os.unlink(env["COVFILE"])

        # Make sure that the builds dir exists before starting the build
        # otherwise the covc process will fail creating the coverage file
        if not os.path.exists(self.builds_dir):
            os.makedirs(self.builds_dir)


        # Need to load up the env_script:
        #  - maintain original env variables that are being modified so that
        #    it can be reset once the subprocess call is completed
        #  - execute a replace() if the value contains the key name, substituting
        #    the original env value for the $KEY token
        env_orig=dict()
        env_script=self.script_dir+"/"+env_script
        lines=open(env_script).read()
        lines=lines.split('\n')
        for line in lines:
            if line.startswith("#"):
                continue
            if line.startswith("CONFIGURE_ARGS="):
                env["CONFIGURE_ARGS"]=line[len("CONFIGURE_ARGS="):]
                print("configure_args: %s %s" % (configure_args, env["CONFIGURE_ARGS"]))
                continue
            
            pattern = re.compile(r'^.*\=')
            m = pattern.match(line)
            if m:
                key=m.group()[:-1]
                value=line[len(key)+1:]
                value_orig=None
                if env.has_key(key):
                    value_orig=env[key]
                env_orig[key]=value_orig

                # Is there a $key token string that we need to replace?
                if value.find("$%s" % key)>-1:
                    value=value.replace("$%s" % key, value_orig)

                # Set the environment
                env[key]=value
        
        
        # Turn on code coverage
        cmd="%s/cov01 --on" % self.bullseye_dir
        stdout,exit = self.run_pipe(cmd=cmd, env=env)
        for line in stdout:
            print line

        # Use the compile-generic script to compile the shell
        cmd="../all/compile-generic.sh '%s' '%s' '%s' 'false' '' 'objdir-codecoverage' " % (self.buildnum, configure_args, shellname)
        stdout,exit = self.run_pipe(cmd=cmd, env=env)
        for line in stdout:
            print line
        if exit:
            print("\n\naborting code coverage run")
            print("message: Build %s failed" % shellname)
            sys.exit(exit)
        
        # Turn off code coverage
        cmd="%s/cov01 --off" % self.bullseye_dir
        stdout,exit = self.run_pipe(cmd=cmd, env=env)
        for line in stdout:
            print line

        # If running in JENKINS then copy binary to buids_dir
        if env["JENKINS_HOME"]:
            shutil.copy('%s/objdir-codecoverage/shell/%s' % (env["WS"], shellname), self.builds_dir)

        # Set the environ back to its original state
        for key in env_orig:
            # If there is an original value set it back
            if env_orig[key]:
                env[key]=env_orig[key]
            else: # There was no original value so delete the key/value
                del env[key]


    def test_shell(self, shellname, suite, vm_args="", config_string="", script_args=""):
        print "\n\n========================================"
        print "About to run tests using the following: "
        print "shellname: "+shellname
        print "suite: "+suite
        print "vm_args: "+vm_args
        print "config_string: "+config_string
        print "script_args: "+script_args
        # Make sure that Bullseye is in the path
        env=os.environ
        if env["PATH"].find(self.bullseye_dir)==-1:
            env["PATH"]=self.bullseye_dir+":"+env["PATH"]

        # Setup the covfile based on the shellname being built
        env["COVFILE"]="%s/%s-%s.cov" % (self.builds_dir, self.platform, shellname)

        if suite=="acceptance":
             # If running in JENKINS then copy binary to buids_dir
            if env["JENKINS_HOME"]:
                env["AVM"]="%s/%s" % (self.builds_dir, shellname)
            print "running acceptance"
            # Use the compile-generic script to compile the shell
            cmd="../all/run-acceptance-generic.sh '%s' '%s' '%s' '%s' '%s'" % (self.buildnum, shellname, vm_args, config_string, script_args)
            stdout,exit = self.run_pipe(cmd=cmd, env=env)
            for line in stdout:
                print line
            del env["AVM"]
            
        elif suite=="performance":
            print "running performance"
            env["AVM"]="%s/%s" % (self.builds_dir, shellname)
            env["ASC"]="%s/utils/asc.jar" % (self.root_dir)
            env["BUILTINABC"]="%s/generated/builtin.abc" % (self.root_dir)
            env["SHELLABC"]="%s/generated/shell_toplevel.abc" % (self.root_dir)
            cmd="python runtests.py %s" % script_args
            stdout,exit = self.run_pipe(cmd=cmd, env=env, cwd="%s/test/performance"%self.root_dir)
            for line in stdout:
                print line
            del env["AVM"]
            del env["ASC"]
            del env["BUILTINABC"]
            del env["SHELLABC"]

        elif suite=="selftest":
            print "running selftest"
            cmd="%s/%s -Dselftest %s" % (self.builds_dir, shellname, vm_args)
            stdout,exit = self.run_pipe(cmd=cmd, env=env)
            for line in stdout:
                print line

        elif suite=="commandline":
            print "running commandline"

            env["AVM"]="%s/%s" % (self.builds_dir, shellname)
            env["ASC"]="%s/utils/asc.jar" % (self.root_dir)
            env["BUILTINABC"]="%s/generated/builtin.abc" % (self.root_dir)
            env["SHELLABC"]="%s/generated/shell_toplevel.abc" % (self.root_dir)
            # Determine if the shell contains a debugger
            debugger=False
            cmd="%s/%s -Dversion" % (self.builds_dir, shellname)
            stdout,exit = self.run_pipe(cmd=cmd, env=env)
            for line in stdout:
     
                if line!=None and line.find("AVMFEATURE_DEBUGGER")>-1:
                    debugger=True
                    env["AVMRD"]="%s/%s" % (self.builds_dir, shellname)
            print "Are we running with debugger: %s" % debugger
            cmd="python runtests.py"
            stdout,exit = self.run_pipe(cmd=cmd, env=env, cwd="%s/test/cmdline"%self.root_dir)
            for line in stdout:
                print line
            del env["AVM"]
            del env["ASC"]
            del env["BUILTINABC"]
            del env["SHELLABC"]
            if debugger:
                del env["AVMRD"]
            
        # Get the current coverage data, execute from the root_dir so that path information is correct
        cmd="%s/covdir -q -m %s" % (self.bullseye_dir, self.exclude_regions)
        stdout,exit = self.run_pipe(cmd=cmd, env=env, cwd=self.root_dir)
        for line in stdout:
            print line


    def merge_coverage(self):
        print "\n\n========================================"
        print "Merging all coverage files together"
        # Make sure that Bullseye is in the path
        env=os.environ
        if env["PATH"].find(self.bullseye_dir)==-1:
            env["PATH"]=self.bullseye_dir+":"+env["PATH"]

        # Setup the covfile based on the shellname being built
        env["COVFILE"]="%s/%s-avmshell.cov" % (self.platform,self.builds_dir)
        if os.path.exists(env["COVFILE"]):
            os.unlink(env["COVFILE"])
        
        covfiles=""
        for file in os.listdir(self.builds_dir):
            if file.endswith(".cov"):
                covfiles+=" %s"%file
        
        # Get the current coverage data, execute from the root_dir so that path information is correct
        cmd="%s/covmerge -c %s" % (self.bullseye_dir, covfiles)
        stdout,exit = self.run_pipe(cmd=cmd, env=env, cwd=self.builds_dir)
        for line in stdout:
            print line
            
        # Get the current coverage data, execute from the root_dir so that path information is correct
        cmd="%s/covdir -q -m %s" % (self.bullseye_dir, self.exclude_regions)
        stdout,exit = self.run_pipe(cmd=cmd, env=env, cwd=self.root_dir)
        Total=None
        for line in stdout:
            if line.startswith("Total"):
                Total=line
            print line

        print ""
        print "message: total function coverage:           %s" % Total.split()[5]
        print "message: total condition/decision coverage: %s" % Total.split()[10]
        

    def run_pipe(self, cmd, cwd=None, env=None):
        if cwd==None:
            cwd=self.script_dir
        # run a command and return a tuple of (output, exitCode)
        if env==None:
            process = subprocess.Popen(cmd,cwd=cwd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        else:
            process = subprocess.Popen(cmd,cwd=cwd,shell=True,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        
        (output,err) = process.communicate()
        output = output.split('\n') if output else []
        if output and output[-1].strip() == '': # strip empty line at end
            output = output[:-1]

        exitCode = process.returncode
        return (output,exitCode)

    # workaround since python split does not allow " to span multiple tokens
    # "func1(param1,param)","foo","foo" would not split correctly
    def mysplit(self,line):
        tokens=[]
        while True:
            if len(line)==0:
                break
            if line[0]=='"':
                line=line[1:]
                if line.find('"')==-1:
                    tokens.append(line)
                    break
                tokens.append(line[0:line.find('"')])
                line=line[line.find('"')+2:]
            else:
                if line.find(',')==-1:
                    tokens.append(line)
                    break
                else:
                    tokens.append(line[0:line.find(',')])
                    line=line[line.find(',')+1:]
        return tokens


    def convertFromCygwin(self, cygpath):
        if cygpath.find('\\') == -1:
            try:
                f = self.run_pipe('cygpath -m %s' % cygpath)
                cygpath = ' '.join([i.strip() for i in f[0]])
            except:
                pass
            return cygpath

    def determineOS(self):
        _os = platform.system()
        ostype = ''

        if re.search('(CYGWIN_NT)', _os):
            ostype='windows'
            self.cygwin = True
        if re.search('(Windows)', _os):
            ostype='windows'
            self.useShell = False
        if re.search('(Darwin)', _os):
            ostype='mac'
        if re.search('(Linux)', _os):
            ostype='linux'
        
        if ostype == '':
            print("ERROR: os %s is unknown" % (platform.system()))
            exit(1)

        self.platform = ostype

if __name__ == '__main__':
    p = CodeCoverageRunner()
