# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import re
from email.Message import Message
from email.Utils import formatdate
from email.MIMEText import MIMEText
try:
    from email.MIMEMultipart import MIMEMultipart
    canDoAttachments = True
except ImportError:
    canDoAttachments = False
import urllib
import string
import os
from cPickle import load, dump

from zope.interface import implements
from twisted.internet import defer
from twisted.mail.smtp import sendmail
from twisted.python import log as twlog

from buildbot import interfaces, util
from buildbot.status import base
from buildbot.status.builder import FAILURE, SUCCESS, WARNINGS
from buildbot.status.mail import MailNotifier


class BuilderGroup():
    def __init__(self):
        self.builderNames = []
        self.currentBuild = 0
        self.sent = 0
        self.cachedMessages = []
        
    def cacheMessage(self, name, build, results):
        self.cachedMessages.append({'name':name, 'build':build, 'results':results})

    def reset(self):
        # reset all values
        self.cachedMessages = []
        self.sent = 0
        self.currentBuild = 0
        
    def addBuilderNames(self, names):
        if isinstance(names, list):
            self.builderNames.extend(names)
        else:
            self.builderNames.append(names)
            
    def containsBuilder(self, name):
        if name in self.builderNames:
            return True
        else:
            return False
            
    
        
        

class AggregateMailNotifier(MailNotifier):
    '''
    The AggregateMailNotifier extends MailNotifier to add support for emails to be batched together
    into single emails that only fire once a scheduler group of builders is finished building.
    '''
    
    # compare_attrs is used to compare two instances of the same class - the attributes in this list are compared to each
    # other to determine if it is the same class object
    compare_attrs = MailNotifier.compare_attrs + ['schedulerGroupsSendFirst', 'schedulerGroups']
    
    def __init__(self, fromaddr, mode="all", categories=None, builders=None,
                 addLogs=False, relayhost="localhost",
                 subject="",
                 lookup=None, extraRecipients=[],
                 sendToInterestedUsers=True,
                 schedulerGroups = [],
                 schedulerGroupsSendFirst = 1
                 ):
        '''
        @type  schedulerGroupsSendFirst: int
        @param schedulerGroupsSendFirst: How many mails to send to interested users before we
                                            group the rest of the emails.
        @type  schedulerGroups: list (can be list of lists)
        @param schedulerGroups: list of schedulers to group emails by.  Multiple schedulers can
                                be grouped together by passing them in as a list.  This may be a
                                list of lists, but can not be nested beyond that.
        '''
        self.schedulerGroupsSendFirst = schedulerGroupsSendFirst
        
        
        # builderGroups is a list BuilderGroups
        self.builderGroups = []
        for schedulerGroup in schedulerGroups:
            group = BuilderGroup()
            if isinstance(schedulerGroup, list):    
                for scheduler in schedulerGroup:
                    group.addBuilderNames(scheduler.listBuilderNames())
            else:
                group.addBuilderNames(schedulerGroup.listBuilderNames())
            self.builderGroups.append(group)
                    
        
        MailNotifier.__init__(self, fromaddr, mode, categories, builders,
                 addLogs, relayhost,
                 subject,
                 lookup, extraRecipients,
                 sendToInterestedUsers)
                 

    def shouldSendMessage(self, name, build, results):
        # Based on name, build, results - should a message be sent? Boolean return value.
        if self.builders is not None and name not in self.builders:
            return False
        if self.categories is not None and \
               builder.category not in self.categories:
            return False
        if self.mode == "failing" and results != FAILURE:
            return False
        if self.mode == "passing" and results != SUCCESS:
            return False
        if self.mode == "problem":
            if results != FAILURE:
                return False
            prev = build.getPreviousBuild()
            if prev and prev.getResults() == FAILURE:
                return False
        return True
    
    def buildFinished(self, name, build, results):
        # Called every time a machine finishes a phase
        if self.builderGroups:
            currentBuilder = build.getBuilder()
            currentBuilderName = currentBuilder.getName()
            revision = build.getSourceStamp().revision
            
            for builderGroup in self.builderGroups:
                if builderGroup.containsBuilder(currentBuilderName):
                    if builderGroup.currentBuild == revision:
                        if self.shouldSendMessage(name, build, results):
                            # Append to cache
                            builderGroup.cacheMessage(name, build, results)
                            if self.schedulerGroupsSendFirst:
                                if builderGroup.sent < self.schedulerGroupsSendFirst:
                                    builderGroup.sent += 1
                                    MailNotifier.buildFinished(self, name, build, results)
                    elif builderGroup.currentBuild != revision:
                        # new revision - reset BuilderGroup just in case
                        builderGroup.reset()
                        builderGroup.currentBuild = revision
                        if self.shouldSendMessage(name, build, results):
                            # Append to cache
                            builderGroup.cacheMessage(name, build, results)
                            if self.schedulerGroupsSendFirst:
                                builderGroup.sent = 1
                                # send this email out
                                return MailNotifier.buildFinished(self, name, build, results)
                    # Are there any builders left? if not send the grouped message
                    for builder in builderGroup.builderNames:
                        # The current builder will always have a building state,
                        # so do not check state if current builder
                        if builder != currentBuilderName:
                            state = self.status.getBuilder(builder).getState()[0]
                            if state == "building":     # if any are building we can return
                                return
                    # Nothing is building - send out the aggregated message
                    if len(builderGroup.cachedMessages) == builderGroup.sent:
                        # We've already sent out all the messages in the cache
                        # so there is no need to send out the aggregated messages
                        return
                    # Note that this code will only get run if the two conditions above are not met (both return nothing)
                    d = self.sendAggregateMail(builderGroup.cachedMessages)
                    builderGroup.reset()
                    return d
                    
        else:   # send message like regular MailNotifier
            # this comment copied from MailNotifier:
            # for testing purposes, buildMessage returns a Deferred that fires
            # when the mail has been sent. To help unit tests, we return that
            # Deferred here even though the normal IStatusReceiver.buildFinished
            # signature doesn't do anything with it. If that changes (if
            # .buildFinished's return value becomes significant), we need to
            # rearrange this.
            return MailNotifier.buildFinished(self, name, build, results)
            


    def sendAggregateMail(self, cachedResults):
        # override this function
        # cachedMessages is a list of dicts: {'name':name, 'build':build, 'results':results}
        pass