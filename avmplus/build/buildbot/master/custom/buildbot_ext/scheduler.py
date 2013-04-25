# -*- Mode: Python; indent-tabs-mode: nil -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from zope.interface import implements
from twisted.internet import reactor
from twisted.application import service, internet, strports
from twisted.python import log, runtime
from twisted.protocols import basic
from twisted.cred import portal, checkers
from twisted.spread import pb

from buildbot import interfaces, buildset, util, pbutil
from buildbot.status import builder
from buildbot.sourcestamp import SourceStamp
from buildbot.changes.maildir import MaildirService
from buildbot.process.properties import Properties

from buildbot import scheduler


class Scheduler(scheduler.Scheduler):
    '''The buildbot.scheduler.Scheduler class is sublclassed (with the same name to not have 
        to modify any of the master.cfg code) from the original source to add the ability to 
        handle buildsets'''

    def addChange(self, change):
        if self.branch is not None and change.branch not in self.branch:
            log.msg("%s ignoring off-branch %s" % (self, change))
            return
        if self.categories is not None and change.category not in self.categories:
            log.msg("%s ignoring non-matching categories %s" % (self, change))
            return
        if not self.fileIsImportant:
            self.addImportantChange(change)
        elif self.fileIsImportant(change):
            self.addImportantChange(change)
        else:
            self.addUnimportantChange(change)

    def fireTimer(self):
        # clear out our state
        self.timer = None
        self.nextBuildTime = None
        # Only build changes that are deemed important by self.fileIsImportant
        changes = self.importantChanges
        self.importantChanges = []
        self.unimportantChanges = []

        # Only add builders to the buildset that are currently online
        buildset_builderNames = []
        for builder_name in self.builderNames:
            # Get a builder from the BotMaster:
            try:
                builder = self.parent.botmaster.builders.get(builder_name)
            except AttributeError:
                # when this scheduler is used by the AnyBranchScheduler, botmaster is one level up from normal
                builder = self.parent.parent.botmaster.builders.get(builder_name)
                
            if builder.builder_status.getState()[0] == 'idle':
                buildset_builderNames.append(builder_name)

        # create a BuildSet, submit it to the BuildMaster
        bs = buildset.BuildSet(buildset_builderNames,
                               SourceStamp(changes=changes),
                               properties=self.properties)
        self.submitBuildSet(bs)

class PhaseTwoScheduler(Scheduler):
    changeDir = ""
    def __init__(self, name, branch, treeStableTimer, builderNames,
             fileIsImportant=None, properties={}, categories=None, builderDependencies=[], changeDir=".", priorities=[]):
        '''
        priorities : list of (branch, priorities) tuples that map branches to
                   priorities (int)
        '''
        Scheduler.__init__(self, name, branch, treeStableTimer, builderNames, 
            fileIsImportant, properties, categories)
        # - each builder must have a dependent that is in the upstream builder
        # - multiple builders can be dependent on the same upstream builder
        self.builderDependencies = builderDependencies
        self.changeDir = changeDir
        self.priorities = priorities

    def fireTimer(self):
        # clear out our state
        self.timer = None
        self.nextBuildTime = None
        changes = self.unimportantChanges + self.importantChanges
        self.importantChanges = []
        self.unimportantChanges = []

        buildset_builderNames = []
        # There may be multiple build requests, only process the very last one (latest)
        change = changes[-1]
        # Only add builders that are:
        #    1) dependent builder is listed in the change request, this means that the upstream builder was online
        #    2) builder is online

        # determine the priority based on branch
        # note that this is a list comprehension, so we need to "unpack" with
        # [0] at end
        priority = [p[1] for p in self.priorities if p[0] == change.branch][0]
        f = open("%s/change-%s.%s" % (self.changeDir, change.revision, priority))
        phase1Builders = []
        for line in f.readlines():
            if line.startswith("builders:"):
                phase1Builders = line[line.find(":")+1:].strip().split()
                break
            
        # builders is a list of builders that built the request in Phase1
        for p1Builder in phase1Builders:
            # Get the depenent builder
            for dependent in self.builderDependencies:
                if p1Builder == dependent[1]:
                    p2Builder = dependent[0]
                    # See if the builder is online
                    # Get a builder from the BotMaster:
                    builder = self.parent.botmaster.builders.get(p2Builder)
                    # Add the builder to the set if it is idle (not building and not offline)
                    if builder.builder_status.getState()[0] == 'idle':
                        buildset_builderNames.append(p2Builder)

        # create a BuildSet, submit it to the BuildMaster
        bs = buildset.BuildSet(buildset_builderNames,
                               SourceStamp(changes=changes),
                               properties=self.properties)
        self.submitBuildSet(bs)

class AnyBranchScheduler(scheduler.AnyBranchScheduler):
    '''The buildbot.scheduler.AnyBranchScheduler is subclassed to use our modified
       Scheduler (above) with the buildsets additions'''
    schedulerFactory = Scheduler
    

class BuilderDependent(scheduler.Dependent):
    """This scheduler runs some set of 'downstream' builds when the
    'upstream' scheduler has completed successfully."""
    
    fileIsImportant = None
    
    def __init__(self, name, upstream, callbackInterval, builderNames, builderDependencies, 
                    properties={}, fileIsImportant=None):
        scheduler.Dependent.__init__(self, name, upstream, builderNames, properties)
        # - each builder must have a dependent that is in the upstream builder
        # - multiple builders can be dependent on the same upstream builder
        self.builderDependencies = builderDependencies
        self.callbackInterval = callbackInterval
        # TODO: self.upstream is set to None upon init in Dependent ... not sure why, so this is a workaround
        # see http://github.com/djmitche/buildbot/commit/4066acfdd6477e59b00767c1c5607e4666e15d6d
        self.upstream = upstream
        
        if fileIsImportant:
            assert callable(fileIsImportant)
            self.fileIsImportant = fileIsImportant
        
        for builder_name in self.builderNames:
            dependent_builder = ''
            # Get the depenent builder
            for dependent in self.builderDependencies:
                if builder_name == dependent[0]:
                    dependent_builder = dependent[1]
                    break
            
            # Make sure there is a defined dependent builder
            if dependent_builder == '':
                errmsg = "The builder %s has no corresponding dependent builder" % builder_name
                assert False, errmsg
            
            # Make sure that the dependent builder is defined in the upstream scheduler
            if dependent_builder not in self.upstream.listBuilderNames():
            #if dependent_builder not in self.findUpstreamScheduler().listBuilderNames():
                errmsg = "The dependent builder %s is not defined in the upstream scheduler %s" % (dependent_builder, self.upstream.name)
                assert False, errmsg
                
        self.timer = None
        self.source_stamp = None
       
    def upstreamBuilt(self, ss):
        # Only add builders to the buildset that are currently online and if the dependent builder 
        # also built this change.
        
        # If a fileIsImportant function is defined, check the files before determining buildsets
        if self.fileIsImportant:
            startThisBuildSet = False
            # SourceStamp provides us with a tuple of changes, loop through and check them all
            for change in ss.changes:
                if self.fileIsImportant(change):
                    # If any file is important, we start the buildSet
                    startThisBuildSet = True
                    break
            if not startThisBuildSet:
                # no important files found, do not continue with this buildset
                return
        
        if self.source_stamp == None:
            # This is the first time that the ss has been seen, this is NOT a callback
            self.source_stamp = ss
        else:
            # This can either be a callback or a new ss passed in from upstream builder
            try:
                 revision_prev = int(self.source_stamp.revision)
                 revision_curr = int(ss.revision)
            except ValueError:
                errmsg = "BuilderDependent.upstreamBuilt has an unknown ss.revison: %s" % (ss.revision)
                assert False, errmsg

            # ss.revision is an OLDER REV than self.source_stamp.revision
            if revision_curr < revision_prev: 
                # This means we are a callback, and during sleep period a new build request 
                # has come in and is now in control, we can just stop running.
                return
            
            # ss.revision is a NEWER REV than self.source_stamp.revision
            elif revision_curr > revision_prev:
                # This would indicate a new build and we need to merge ss and self.source_stamp
                ss.mergeWith([self.source_stamp])
                self.source_stamp = ss
            
            # Revisions are the same so this is the callback
            # else revision_curr == revision_prev
        
        
        buildset_builderNames = []
        for builder_name in self.builderNames:
            # Get the depenent builder
            for dependent in self.builderDependencies:
                if builder_name == dependent[0]:
                    dependent_builder = dependent[1]
                    break
            
            # Get the dependent builder
            dep_builder = self.parent.getStatus().getBuilder(dependent_builder)
            # Determine the revision of the last build for the dependent builder
            # need to make sure that the builder has at least one previous build
            try:
                dep_ss = dep_builder.getLastFinishedBuild().getSourceStamp()
            except AttributeError:
                dep_ss = SourceStamp()
            
            
            # Only continue the check if the dependent builder built the sourceStamp, 
            if ss.revision == dep_ss.revision:
                # Get a builder from the BotMaster:
                builder = self.parent.botmaster.builders.get(builder_name)
                if builder.builder_status.getState()[0] == 'building':
                    # There is a builder in this scheduler that is active so we need
                    # to NOT start the build yet, but instead callback in X seconds
                    # to see if all of the builders are available.
                    self.timer = reactor.callLater(self.callbackInterval, self.upstreamBuilt, ss)
                    return
                # Add the builder to the set if it is idle (not building and not offline)
                if builder.builder_status.getState()[0] == 'idle':
                    buildset_builderNames.append(builder_name)
                    

        bs = buildset.BuildSet(buildset_builderNames, ss,
                    properties=self.properties)
        self.submitBuildSet(bs)
        
        # Clear the tracked source stamp
        self.source_stamp = None

