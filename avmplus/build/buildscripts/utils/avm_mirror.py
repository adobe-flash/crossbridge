#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.

# Gist of the script flow:
# 1. read in what the last mirror point was from an external file
# 2. get a list of changes from p4 since that last change
# 3. in tamarin do a pull update (in the event there was a change outside of p4)
# 4. loop over the changes:
#    a. sync to the change
#    b. capture the p4 commit message, user, date and CL#
#    c. rm everything in tamarin minus .hg/
#    d. copy everything from p4 clientspace into tamarin MINUS tamarin-merge.txt
#    e. hg commit using p4 message (append CL# to message), p4 user and p4 date if possible
#    f. hg push this change (this will make sure that if there is another change available
#       that for some reason bombs out, we have at least pushed to mozilla what we could)
#    g. rewrite the external tracking file with this CL#
#    h. if the push fails....

# Information on how to setup the host of this script can
# be found @ https://zerowing.corp.adobe.com/x/QwU5JQ

import os, marshal, sys, subprocess, shutil, tempfile
import datetime, pytz
from subprocess import Popen, PIPE

class P4Mirror():
    # Required config information:
    # tamarin-redux repo location
    TAMARIN_REPO=os.getenv('TAMARIN_REPO')
    LASTBUILD_FILE="lastbuild.txt"
    P4ROOT=os.getenv('P4ROOT')
    P4USER=os.getenv('P4USER')
    P4CLIENT=os.getenv('P4CLIENT')
    P4PORT=os.getenv('P4PORT')
    P4AVMPLUS=os.getenv('P4AVMPLUS')
    OLDHEAD=0
    HEAD=0
    script_dir="./"
    env=os.environ

    def __init__(self):
        # Ensure that all necessary env variables have been configure
        self.checkConfig()

        self.log("Read in last mirror point")
        self.getLastMirror()
        self.log("\t%s\n" % self.OLDHEAD)

        self.log("Get the current HEAD")
        self.getCurrentHead()
        self.log("\t%s\n" % self.HEAD)

        self.log("Get changes from Perforce")
        changes=self.getChanges()
        self.log("Found %s changes to process..." % len(changes))
        if len(changes) == 0:
            sys.exit(0)

        self.log("Sync the hg mirror repo")
        self.syncHGRepo()
        self.log("\n")

        self.log("Process Changes")
        self.processChanges(changes)

    def checkConfig(self):
        exit=False
        # Ensure that all necessary env variables have been configure
        if self.TAMARIN_REPO == None:
            self.log("TAMARIN_REPO is not set")
            exit=True
        if self.LASTBUILD_FILE == None:
            self.log("LASTBUILD_FILE is not set")
            exit=True
        if self.P4ROOT == None:
            self.log("P4ROOT is not set")
            exit=True
        if self.P4USER == None:
            self.log("P4USER is not set")
            exit=True
        if self.P4CLIENT == None:
            self.log("P4CLIENT is not set")
            exit=True
        if self.P4PORT == None:
            self.log("P4PORT is not set")
            exit=True
        if self.P4AVMPLUS == None:
            self.log("P4AVMPLUS is not set")
            exit=True

        if exit:
            sys.exit(1)

        # Do a quick sanity check to make sure that self.TAMARIN_REPO/.hg/hgrc exists
        # before we start deleting items from this location later in processChanges()
        if not os.path.exists(self.TAMARIN_REPO+"/.hg/store"):
            self.log("TAMARIN_REPO does not contain a proper mercurial repo")
            self.log("TAMARIN_REPO: %s" % self.TAMARIN_REPO)
            sys.exit(1)

        if not os.path.exists(self.LASTBUILD_FILE):
            self.log("LASTBUILD_FILE does not exist")
            self.log("LASTBUILD_FILE: %s" % self.LASTBUILD_FILE)
            sys.exit(1)

    def getLastMirror(self):
        # read last mirror changelist
        bf=open(self.LASTBUILD_FILE, 'r')
        for line in bf:
            self.OLDHEAD = int(line)
        bf.close()

    def getCurrentHead(self):
        stdout, stderr, exit = self.run_pipe(cmd="p4 counter change", env=self.env)
        for line in stdout:
            self.HEAD = int(line)
        if stderr:
            for line in stderr:
                self.log(line)
        if exit:
            sys.exit(exit)

    def getChanges(self):
        changes = []
        # Actually need to get all of the changes from OLDHEAD+1 to HEAD
        cmd = "p4 -G changes -s submitted //%s/...@%s,%s" % (self.P4CLIENT, self.OLDHEAD+1, self.HEAD)
        pipe = Popen( cmd.split(), stdout=PIPE).stdout
        try: # The -G option on p4 returns a python object, so need to be loaded via marshal.load()
            while 1:
                record = marshal.load( pipe )
                changes.append( record )
        except EOFError:
                pass
        pipe.close()
        # Need to loop backwards through this list as the latest changes is at the start
        changes.reverse()
        return changes

    def syncHGRepo(self):
        # Script being really paranoid about local changes....
        cmd = "hg revert --all"
        stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
        for line in stdout:
            self.log(line)
        if stderr:
            for line in stderr:
                self.log(line)
        if exit:
            sys.exit(exit)
        # Script being really paranoid about local changes....
        cmd = "hg purge"
        stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
        for line in stdout:
            self.log(line)
        if stderr:
            for line in stderr:
                self.log(line)
        if exit:
            sys.exit(exit)

        cmd = "hg pull"
        stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
        for line in stdout:
            self.log(line)
        if stderr:
            for line in stderr:
                self.log(line)
        if exit:
            sys.exit(exit)

        cmd = "hg update -C -r tip"
        stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
        for line in stdout:
            self.log(line)
        if stderr:
            for line in stderr:
                self.log(line)
        if exit:
            sys.exit(exit)

    def processChanges(self, changes):
        '''
        Loop over the changes:
            a. sync to the change
            b. capture the p4 commit message, user, date and CL#
            c. rm everything in tamarin minus .hg/
            d. copy everything from p4 clientspace into tamarin MINUS tamarin-merge.txt
            e. hg commit using p4 message (append CL# to message), p4 user and p4 date if possible
            f. hg push this change (this will make sure that if there is another change available
               that for some reason bombs out, we have at least pushed to mozilla what we could)
            g. if the push fails....
            h. rewrite the external tracking file with this CL#
        '''
        user=''
        changelist=''
        desc=''
        date=''
        for dict in changes:
            changelist = dict["change"]
            self.log("\nProcessing changelist: %s" % changelist)

            #########################################
            # a. sync to the change
            #########################################
            self.log("Sync to the change...")
            cmd = "p4 sync %s@%s" % (self.P4AVMPLUS, changelist)
            stdout, stderr, exit = self.run_pipe(cmd=cmd, env=self.env)
            for line in stdout:
                self.log(line)
            if stderr:
                for line in stderr:
                    self.log(line)
            if exit:
                sys.exit(exit)
            #########################################
            # b. capture the p4 commit message, user,
            #    date and CL#
            #########################################
            cmd = "p4 -G describe -s %s" % (dict["change"])
            pipe = Popen( cmd.split(), stdout=PIPE).stdout
            try: # The -G option on p4 returns a python object, so need to be loaded via marshal.load()
                while 1:
                    record = marshal.load( pipe )
            except EOFError:
                pass
            pipe.close()

            user = record["user"]
            date = record["time"]
            desc = record["desc"]

            cmd = "p4 -G user -o %s" % (user)
            pipe = Popen( cmd.split(), stdout=PIPE).stdout
            try: # The -G option on p4 returns a python object, so need to be loaded via marshal.load()
                while 1:
                    record = marshal.load( pipe )
            except EOFError:
                pass
            pipe.close()

            user = "%s <%s>" % (record["FullName"], record["Email"])

            #########################################
            # c. rm everything in tamarin minus .hg/
            #########################################
            self.log("Clean out the mirror repo...")
            for filename in os.listdir(self.TAMARIN_REPO):
                fullpath = "%s/%s" % (self.TAMARIN_REPO, filename)
                if filename != ".hg":
                    if os.path.isfile(fullpath):
                        os.unlink(fullpath)
                    else:
                        shutil.rmtree(fullpath)

            #########################################
            # d. copy everything from p4 clientspace into tamarin
            #########################################
            self.log("Repopulate the mirror repo from p4 workspace...")
            for filename in os.listdir(self.P4ROOT):
                src = "%s/%s" % (self.P4ROOT, filename)
                dest = "%s/%s" % (self.TAMARIN_REPO, filename)
                if os.path.isfile(src):
                    shutil.copy2(src, dest)
                else:
                    shutil.copytree(src, dest)

            #########################################
            # e. hg commit using p4 message (append CL# to message),
            #    p4 user and p4 date if possible
            #########################################
            self.log("Commit the change to the mirror repo...")
            commit_message = desc + "\nCL@" + changelist
            fd, temp_path = tempfile.mkstemp()
            os.write(fd, commit_message)
            os.close(fd)

            # Massage the date
            d = datetime.datetime.fromtimestamp(float(date), pytz.timezone("US/Pacific"))
            date = d.strftime("%a %b %d %H:%M:%S %Y %z")

            cmd = "hg commit --addremove --user \"%s\" --date \"%s\" --logfile %s" % (user, date, temp_path)
            self.log(cmd)
            stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
            for line in stdout:
                self.log(line)
            if stderr:
                for line in stderr:
                    self.log(line)
            if exit:
                sys.exit(exit)

            # Make sure to remove the temp file
            os.unlink(temp_path)

            #########################################
            # f. hg push this change (this will make sure that if
            #    there is another change available that for some
            #    reason bombs out, we have at least pushed to mozilla
            #    what we could)
            #########################################
            self.log("Push the change to the mirror repo...")
            cmd = "hg push"
            stdout, stderr, exit = self.run_pipe(cmd=cmd, cwd=self.TAMARIN_REPO, env=self.env)
            for line in stdout:
                self.log(line)
            #########################################
            # g. if the push fails....
            #########################################
            if stderr:
                for line in stderr:
                    self.log(line)
            if exit:
                sys.exit(exit)

            #########################################
            # h. rewrite the external tracking file with this CL#
            #########################################
            self.log("Update changelist tracking file...")
            bf=open(self.LASTBUILD_FILE, 'w')
            bf.write(changelist)
            bf.close()

            self.log("Completed changelist: %s\n\n#########################################" % changelist)


    def run_pipe(self, cmd, cwd=None, env=None):
        if cwd==None:
            cwd=self.script_dir
        # run a command and return a tuple of (output, exitCode)
        if env==None:
            process = subprocess.Popen(cmd, cwd=cwd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        else:
            process = subprocess.Popen(cmd, cwd=cwd, shell=True, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        (output, err) = process.communicate()
        output = output.split('\n') if output else []
        if output and output[-1].strip() == '': # strip empty line at end
            output = output[:-1]

        exitCode = process.returncode
        #process.close()
        return (output, err, exitCode)

    def log(self, str):
        print(str)

if __name__ == "__main__":
    f=P4Mirror()


