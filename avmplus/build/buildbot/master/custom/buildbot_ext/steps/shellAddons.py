# -*- test-case-name: buildbot.test.test_steps,buildbot.test.test_properties -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


from buildbot.steps.shell import ShellCommand
from buildbot.steps.transfer import FileDownload
from buildbot.status.builder import SUCCESS, FAILURE, SKIPPED, WARNINGS
from buildbot.process.buildstep import LoggingBuildStep, RemoteShellCommand

import buildbot.status.builder
from twisted.python import log
import re

class BaseShellCommand(ShellCommand):
    messages=[]
    script_status=""
    
    def __init__(self, **kwargs):
        ShellCommand.__init__(self, **kwargs)
        self.messages=[]
        self.script_status=""

    def start(self):
        res="SUCCESS"
        for result in self.build.results:
            if result == WARNINGS and res == "SUCCESS":
                res = "WARNINGS"
            if result == FAILURE:
                res = "FAILURE"
        self.build.setProperty("status", res, "BaseShellCommand")
        ShellCommand.start(self)


    def createSummary(self, log):
        lines=log.readlines()
        for line in lines:
            if line.startswith('url:'):
               items=line.split()
               url=items[1]
               items=items[2:]
               desc=''
               for item in items:
                   desc="%s %s" % (desc,item)
               self.addURL(desc, url)
            if line.startswith("message:"):
                message = line[len("message:"):].strip()
                self.messages.append(message)
            if line.startswith("buildbot_status:"):
                # valid values: [ SUCCESS | FAILURE | WARNINGS ]
                self.script_status = line[len("buildbot_status:"):].strip()


    def getText(self, cmd, results):
        text=ShellCommand.getText(self,cmd,results)
        for message in self.messages:
            text.append('%s' % message)
        return text
        
    def evaluateCommand(self,cmd):
        
        if cmd.rc != 0:
            return buildbot.status.builder.FAILURE
        if self.script_status != "":
            if self.script_status == "SUCCESS":
                return buildbot.status.builder.SUCCESS
            elif self.script_status == "FAILURE":
                return buildbot.status.builder.FAILURE
            elif self.script_status == "WARNINGS":
                return buildbot.status.builder.WARNINGS
            else:
                # This is an unknown status, FAIL the step so that it is investigated
                return buildbot.status.builder.FAILURE
                
        return buildbot.status.builder.SUCCESS


class PerformanceShellCommand(ShellCommand):
    versions=[]
    numtests=0

    def createSummary(self,log1):
        self.versions=[]
        self.numtests=0
        lines=log1.readlines()
        for line in lines:
            if line.startswith('installing'):
                version=line.split()[1]
                self.versions.append(version)
            try:
                line.index('running test')
                self.numtests += 1
            except:
                self.numtests+=0
        self.versions.sort()
    def getText(self,cmd,results):
        text=ShellCommand.getText(self,cmd,results)
        for version in self.versions:
            text.append('tested player %s' % version)
        text.append(' ran %d tests' % self.numtests)
        return text

class SizeReportShellCommand(BaseShellCommand):

    sizeBytes = '-'
    sizeKBytes = '-'
    sizeExtra = '-'

    def createSummary(self,log1):
        BaseShellCommand.createSummary(self,log1)
        lines=log1.readlines()
        for line in lines:
          if line.startswith('size_bytes'):
            self.sizeBytes=line.split()[1]
          if line.startswith('size_K')>0:
            self.sizeKBytes=line.split()[1]
          if line.startswith('size_extra')>0:
            self.sizeExtra=line.split()[1]
          
  
    def getText(self, cmd, results):
        text=BaseShellCommand.getText(self,cmd,results)
        sz="%.2f" % (float(self.sizeKBytes)-1+float(self.sizeExtra)/1024.0)
        text.append("%s K %d bytes" % (sz,int(self.sizeBytes)))
        return text

class BuildShellCommand(BaseShellCommand):
    def createSummary(self,log1):
        BaseShellCommand.createSummary(self, log1)
        

class PerfShellCommand(BaseShellCommand):
    perfchange = '-'

    def createSummary(self,log1):
        BaseShellCommand.createSummary(self, log1)
        lines=log1.readlines()
        for line in lines:
          if line.startswith('perfchange:'):
             items=line.split()
             try:
                 self.perfchange=float(items[1][:-1])
             except:
                 self.perfchange="error"

    def getText(self, cmd, results):
        text=BaseShellCommand.getText(self,cmd,results)
        text.append("Performance change: %s%%\n" % self.perfchange)
        return text
        
    def evaluateCommand(self,cmd):
        if cmd.rc != 0:
            return buildbot.status.builder.FAILURE
        if self.perfchange > -3:
            return buildbot.status.builder.SUCCESS
        elif self.perfchange > -10:
            return buildbot.status.builder.WARNINGS
        else:
            return buildbot.status.builder.FAILURE
            

class PerfNoColorShellCommand(BaseShellCommand):
    perfchange = '-'

    def createSummary(self,log1):
        BaseShellCommand.createSummary(self, log1)
        lines=log1.readlines()
        for line in lines:
          if line.startswith('perfchange:'):
             items=line.split()
             try:
                 self.perfchange=float(items[1][:-1])
             except:
                 self.perfchange="error"

    def getText(self, cmd, results):
        text=BaseShellCommand.getText(self,cmd,results)
        text.append("Performance change: %s%%\n" % self.perfchange)
        return text

class TestSuiteShellCommand(BaseShellCommand):

    passes = 0
    fails = 0
    unexpectedpasses = 0
    expectedfails = 0
    skipped = 0
    asserts = 0

    def createSummary(self,log1):
        BaseShellCommand.createSummary(self,log1)
        lines=log1.readlines()
        for line in lines:
          if line.find('total passes')>-1:
              fields=line.split()
              self.passes=int(fields[4])
              self.unexpectedpasses=int(fields[7])
          if line.find('total failures')>-1:
              fields=line.split()
              self.fails=int(fields[4])
              self.expectedfails=int(fields[7])
          if line.startswith('unexpected'):
              fields=line.split()
              try:
                  self.unexpectedpasses=int(fields[3])
              except:
                  print("Error parsing unexpected passes")
          if line.startswith('expected'):
              fields=line.split()
              try:
                  self.expectedfails=int(fields[3])
              except:
                  print("Error parsing expected failures")
          if line.startswith('failures'):
              fields=line.split()
              self.fails=int(fields[2])
          if line.startswith('passes'):
              fields=line.split()
              self.passes=int(fields[2])
          if line.startswith('tests skipped'):
              fields=line.split()
              self.skipped=int(fields[3])
          if line.startswith('assertions'):
              fields=line.split()
              self.asserts=int(fields[2])
  
    def evaluateCommand(self,cmd):
        if cmd.rc != 0:
            return buildbot.status.builder.FAILURE
        if self.fails>0 or self.unexpectedpasses>0:
            return buildbot.status.builder.FAILURE
        if self.passes==0 and self.fails==0 and self.unexpectedpasses==0:
            return buildbot.status.builder.FAILURE
        
        # Before we say it was a success, check to see if there were assertions.
        # This will only get checked if the above have already passed, this is 
        # the last check prior to passing the step.
        if self.asserts>0:
            # Treat assertions as a warning
            return buildbot.status.builder.FAILURE
        else:
            return buildbot.status.builder.SUCCESS

    def getText(self, cmd, results):
        text=BaseShellCommand.getText(self,cmd,results)
        text.append("test results")
        text.append("passes:%d </br>" % self.passes)
        text.append("failures:%d</br>" % self.fails)
        text.append("skipped:%d</br>" % self.skipped)
        text.append("unexp pass:%d</br>" % self.unexpectedpasses)
        text.append("exp fails:%d</br>" % self.expectedfails)
        text.append("assertions:%d</br>" % self.asserts)
        return text


class BuildShellCheckCommand(BaseShellCommand):
    # Use this if you wish to stop the build entirely on failure
    haltOnFailure = True
    def createSummary(self,log1):
        BaseShellCommand.createSummary(self, log1)

def parseSendchangeArguments(args):
    """This function parses the arguments that the Buildbot patch uploader
       sends to Buildbot via the "changed files". It takes an argument of a
       list of files and returns a dictionary with key/value pairs
    """
    parsedArgs = {}
    for arg in args:
        try:
            (key, value) = arg.split(":", 1)
            value = value.lstrip().rstrip()
            parsedArgs[key] = value
        except:
            pass

    return parsedArgs


class BuildRequestDownload(FileDownload):
    """This step reads a Change for a filename and downloads it to the slave.
    """

    haltOnFailure = True

    def __init__(self, isOptional=False, patchDir=".", **kwargs):
        """arguments:
        @type  patchDir:    string
        @param patchDir:    The directory on the master that holds the patches
                            This directory is relative to the base buildmaster
                            directory.
                            ie. /home/buildmaster/project
                            Defaults to '.'
        'workdir' is assumed to be 'build' and should be passed if it is
        anything else.
        'isOptional' is assumed to be False; if the patch is optional, pass True.
        """

        self.patchDir = patchDir
        self.isOptional = isOptional
        # mastersrc and slavedest get overridden in start()
        if not 'workdir' in kwargs:
            kwargs['workdir'] = "build"
        FileDownload.__init__(self, mastersrc=".", slavedest=".", **kwargs)

    def start(self):
        changes = self.step_status.build.getChanges()

        if len(changes) < 1:
            return SKIPPED

        args = parseSendchangeArguments(changes[0].files)

        if not 'infoFile' in args and self.isOptional:
            return SKIPPED

        self.mastersrc = "%s/%s" % (self.patchDir, args['infoFile'])
        self.slavedest = "%s" % (args['infoFile'])

        # now that everything is set-up, download the file
        FileDownload.start(self)

class ShellCommandToken(BaseShellCommand):
    
    commandOriginal = []
    def __init__(self, isOptional=False, patchDir=".", **kwargs):
        """arguments:
        @type  patchDir:    string
        @param patchDir:    The directory on the master that holds the patches
                            This directory is relative to the base buildmaster
                            directory.
                            ie. /home/buildslave/project
                            Defaults to '.'
        'workdir' is assumed to be 'build' and should be passed if it is
        anything else.
        'isOptional' is assumed to be False; if the patch is optional, pass True.
        """

        self.patchDir = patchDir
        self.isOptional = isOptional

        self.commandOriginal = []
        for item in kwargs['command']:
            self.commandOriginal.append(item)

        if not 'workdir' in kwargs:
            kwargs['workdir'] = "build"
        BaseShellCommand.__init__(self, **kwargs)
        
        
    def start(self):
        changes = self.step_status.build.getChanges()

        if len(changes) < 1:
            return SKIPPED

        args = parseSendchangeArguments(changes[0].files)

        if not 'infoFile' in args and self.isOptional:
            return SKIPPED
        
        #log.msg("command CLEAN [BEFORE]: %s" % self.command)
        #log.msg("commandOriginal: %s" % self.commandOriginal)
        self.command = []
        for item in self.commandOriginal:
            self.command.append(item)
        #log.msg("command CLEAN [AFTER]: %s" % self.command)
        
        
        #log.msg("command [BEFORE]: %s" % self.command)
        
        f = open(self.patchDir +"/" + args['infoFile'])
        for line in f.readlines():
            if line.startswith("repoPath"):
                repoPath = line.split()[1]
            if line.startswith("revision"):
                revision = line.split()[1]
            if line.startswith("branch:"):
                branch = line.split()[1]
        
        builderName = self.step_status.build.getBuilder().getName()
        
        log.msg("ShellCommandToken -> repoPath: %s" % repoPath)
        log.msg("ShellCommandToken -> revision: %s" % revision)
        log.msg("ShellCommandToken -> branch: %s" % branch)
        log.msg("ShellCommandToken -> builderName: %s" % builderName)
        
        for index, item in enumerate(self.command):
            self.command[index] = self.command[index].replace("$repoPath$", repoPath)
            self.command[index] = self.command[index].replace("$revision$", revision)
            self.command[index] = self.command[index].replace("$branch$", branch)
            self.command[index] = self.command[index].replace("$builderName$", builderName)
        
        #log.msg("command [AFTER]: %s" % self.command)
        
        #log.msg("workdir [BEFORE]: %s" % self.remote_kwargs['workdir'])
        self.remote_kwargs['workdir'] = self.remote_kwargs['workdir'].replace("$branch$", branch)
        #log.msg("workdir [AFTER]: %s" % self.remote_kwargs['workdir'])
        
        BaseShellCommand.start(self)


class SandboxClone(BaseShellCommand):
    changeDir = ""
    dest = ""

    def __init__(self, dest=".", changeDir=".", **kwargs):
        """arguments:
        @type  changeDir:    string
        @param changeDir:    The directory on the master that holds the processed
                             change requests. This directory is relative to the base 
                             buildmaster directory.
                             Defaults to 'changes/processed'
        """
        self.changeDir = changeDir
        self.dest = dest
        BaseShellCommand.__init__(self, **kwargs)
        
        # need to explicitly tell add our custom arguments to the factory
        self.addFactoryArguments(changeDir=changeDir,
                                 dest=dest)
        
        
    
    def start(self):
        changes = self.step_status.build.getChanges()

        # I think that was only here as a safety check since it used to only
        # be used for sandbox builds which were supposed to only have a single change
        #if len(changes) < 1:
        #    return SKIPPED

        # The list of files changed for this build also contains an additional
        # entry the is the name of the build trigger file, we need to find that
        # file so that we can pass the build information along the build process
        for changefile in changes[0].files:
            if changefile.startswith("change-"):
                f = open(self.changeDir +"/" + changefile)
                for line in f.readlines():
                    if line.startswith("url:"):
                        hg_url = line[line.find(":")+1:].strip()
                        break

                self.command = []
                self.command.append("hg")
                self.command.append("clone")
                self.command.append(hg_url)
                self.command.append(self.dest)
                break
        BaseShellCommand.start(self)

