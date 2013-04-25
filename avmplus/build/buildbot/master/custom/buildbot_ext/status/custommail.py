# -*- test-case-name: buildbot.test.test_status -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


# the email.MIMEMultipart module is only available in python-2.2.2 and later

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

from aggregateMailNotifier import AggregateMailNotifier


class CustomMail(AggregateMailNotifier):
    '''Custom Mail class that extends default buildbot mail behavior.'''
    
    # compare_attrs is used to compare two instances of the same class - the attributes in this list are compared to each
    # other to determine if it is the same class object
    compare_attrs = AggregateMailNotifier.compare_attrs + ['branch']
    
    def __init__(self, fromaddr, mode="all", categories=None, builders=None, branch="",
                 addLogs=False, relayhost="localhost",
                 subject="[buildbot-%(result)s] %(blamelist)s: %(branch)s rev %(src)s",
                 lookup=None, extraRecipients=[],
                 sendToInterestedUsers=True,
                 schedulerGroups = [],
                 schedulerGroupsSendFirst = 1
                 ):
                     
        self.builders = None;
        self.branch = branch;
        
        
        
        AggregateMailNotifier.__init__(self, fromaddr, mode, categories, builders,
                 addLogs, relayhost,
                 subject,
                 lookup, extraRecipients,
                 sendToInterestedUsers,
                 schedulerGroups,
                 schedulerGroupsSendFirst
                 )

    
    def getBuildsStatusFileinfo(self, build, blamelist, source):
        # get the previous builds status from file (if exists)
        # buildinfo pickle file structure:
        #  {buildstep:{blamelist:'', revision:'', status:'FAILURE' OR 'WARNING'}}
        # the buildinfo.pck file is located in the platform subdirectory of the master
        
        filename = os.path.join(build.getBuilder().basedir,'buildinfo.pck')
        
        
        buildinfoFileCreated = True
        buildinfoLoaded=True
        
        try:
            buildinfoFile = open(filename, 'r+b')
            buildinfo = load(buildinfoFile)
            buildinfoFile.close()
        except:
            buildinfoLoaded=False
            
        try:
            #overwrite old info file
            buildinfoFile = open(filename, 'wb')
            if not buildinfoLoaded:
                buildinfo = {}
        except:  # can't write so give up
            buildinfoFileCreated = False
        
        failSteps = []
        # remove any failures that now pass, add new failures
        if buildinfoFileCreated:
            for step in build.getSteps():
                result = step.getResults()
                stepname = step.getName()
                if result[0] == SUCCESS:
                    # remove any failures / warnings for this step
                    if stepname in buildinfo:
                        del buildinfo[stepname]
                elif result[0] == FAILURE:
                    # did this step fail previously?
                    if stepname in buildinfo:
                        if buildinfo[stepname]['status'] == 'FAILURE':
                            failSteps.append(stepname)
                    else:
                        # new failure, add to buildinfo
                        buildinfo[stepname] = {'blamelist': blamelist, 'revision':source, 'status':'FAILURE'}
                elif result[0] == WARNINGS:
                    if stepname in buildinfo:
                        if buildinfo[stepname]['status'] == 'WARNING':
                            failSteps.append(stepname)
                    else:
                        # new warning, add to buildinfo
                        buildinfo[stepname] = {'blamelist': blamelist, 'revision':source, 'status':'WARNING'}
            # save the buildinfo file
            dump(buildinfo, buildinfoFile)
            buildinfoFile.close()
            
        return failSteps, buildinfo
    
    
    def buildFinished(self, name, build, results):
        # extend buildFinished to also check branch
        if build.getSourceStamp().branch != self.branch:
            return
            
        AggregateMailNotifier.buildFinished(self, name, build, results)
    
    def buildMessage(self, name, build, results):
        
        projectName = self.status.getProjectName()
        sourcestamp = build.getSourceStamp()
        source = sourcestamp.revision
        buildurl = "%sbuilders/%s/builds/%d" % (self.status.getBuildbotURL(),name,build.getNumber())
        
        # Delimiting wht "," causes email subject line to contain a TAB character for some reason
        blamelist = "|".join(build.getResponsibleUsers())
        p = re.compile(" <[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?>")
        blamelist = p.sub("", blamelist)
        
        res = "pass"
        if results == FAILURE:
            res = "fail"
        
        text = ''
        text += 'Slave:  %s\n' % name
        text += 'Branch:    %s\n' % self.branch
        
        text += self.getChangesText(sourcestamp.changes)
        
        if results == FAILURE:
            text += "Log(s) of failed build steps:\n"
            for step in build.getSteps():
                if step.getResults()[0] == FAILURE:
                    text += "        %s\n" % ''.join(step.getResults()[1])
                    text += "        %s/steps/%s/logs/stdio\n" % (buildurl,''.join(step.getResults()[1]))
                    text += "\n"
        
        failSteps, buildinfo = self.getBuildsStatusFileinfo(build, blamelist, source)
        
        # add previous failures to email
        if failSteps:
            text += 'Buildsteps that were failing before this build:\n'
            for step in failSteps:
                text += '    %s Revision: %s Blamelist: %s\n' %  (step, buildinfo[step]['revision'], buildinfo[step]['blamelist'])
            text += "\n"
        
        m = Message()
        m.set_payload(text)
        
        m['Date'] = formatdate(localtime=True)
        m['Subject'] = self.subject % { 'result': res,
                                        'blamelist': blamelist,
                                        'branch': self.branch,
                                        'src': source,
                                        }

        m['From'] = self.fromaddr
        # m['To'] is added later

        if self.addLogs:
            for log in build.getLogs():
                name = "%s.%s" % (log.getStep().getName(),
                                  log.getName())
                a = MIMEText(log.getText())
                a.add_header('Content-Disposition', "attachment",
                             filename=name)
                m.attach(a)

        # now, who is this message going to?
        dl = []
        recipients = self.extraRecipients[:]
        if self.sendToInterestedUsers and self.lookup:
            for u in build.getInterestedUsers():
                d = defer.maybeDeferred(self.lookup.getAddress, u)
                d.addCallback(recipients.append)
                dl.append(d)
        d = defer.DeferredList(dl)
        d.addCallback(self._gotRecipients, recipients, m)
        return d

    def getChangesText(self, changes):
        text = 'Changes in this build:\n'
        for n, change in enumerate(changes):
            text += '\n'
            text += '%s.\tRevision: %s\n' % ((n+1), change.revision)
            text += '\tUser: %s\n' % change.who
            commentsList = change.comments.split('\n')
            if commentsList:
                text += '\tComments: %s\n' % commentsList.pop(0)
                # print the rest of the comments (if any)
                for comment in commentsList:
                    text += '\t\t%s\n' % comment
        text += '\n'
        return text
    
    def sendAggregateMail(self, cachedResults):
        # cachedResults is a list of dicts: {'name':name, 'build':build, 'results':results}
        projectName = self.status.getProjectName()
        
        # build will be the same for all messages, so just use the first one
        build = cachedResults[0]['build']
        
        sourcestamp = build.getSourceStamp()
        source = sourcestamp.revision
        
        # Delimiting wht "," causes email subject line to contain a TAB character for some reason
        blamelist = "|".join(build.getResponsibleUsers())
        p = re.compile(" <[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?>")
        blamelist = p.sub("", blamelist)
        
        slaves = []
        results = []
        for result in cachedResults:
            slaves.append(result['name'])
            results.append(result['results'])

        text = ''
        text += 'Slave(s):  %s\n' % ', '.join(slaves)
        text += 'Branch:    %s\n' % self.branch
        
        text += self.getChangesText(sourcestamp.changes)
        
        if FAILURE in results:
            text += "Log(s) of failed build steps:\n"
            for result in cachedResults:
                if result['results'] == FAILURE:
                    build = result['build']
                    text += "  Slave: %s\n" % result['name']
                    for step in build.getSteps():
                        if step.getResults()[0] == FAILURE:
                            text += "        %s\n" % ''.join(step.getResults()[1])
                            buildurl = "%sbuilders/%s/builds/%d" % (self.status.getBuildbotURL(),result['name'],build.getNumber())
                            text += "        %s/steps/%s/logs/stdio\n" % (buildurl,''.join(step.getResults()[1]))
                            text += "\n"
                    failSteps, buildinfo = self.getBuildsStatusFileinfo(build, blamelist, source)
                    # add previous failures to email
                    if failSteps:
                        text += '  Buildsteps that were failing before this build:\n'
                        for step in failSteps:
                            text += '    %s Revision: %s Blamelist: %s\n' %  (step, buildinfo[step]['revision'], buildinfo[step]['blamelist'])
                        text += "\n"
        
        m = Message()
        m.set_payload(text)
        
        m['Date'] = formatdate(localtime=True)
        m['Subject'] = self.subject % { 'result': 'fail',
                                        'blamelist': blamelist,
                                        'branch': self.branch,
                                        'src': source,
                                        }

        m['From'] = self.fromaddr
        # m['To'] is added later

        if self.addLogs:
            for result in cachedResults:
                build = result['build']
                for log in build.getLogs():
                    name = "%s.%s" % (log.getStep().getName(),
                                      log.getName())
                    a = MIMEText(log.getText())
                    a.add_header('Content-Disposition', "attachment",
                                 filename=name)
                    m.attach(a)

        # now, who is this message going to?
        dl = []
        recipients = self.extraRecipients[:]
        if self.sendToInterestedUsers and self.lookup:
            for u in build.getInterestedUsers():
                d = defer.maybeDeferred(self.lookup.getAddress, u)
                d.addCallback(recipients.append)
                dl.append(d)
        d = defer.DeferredList(dl)
        d.addCallback(self._gotRecipients, recipients, m)
        return d


    def sendMessage(self, m, recipients):
        s = m.as_string()
        ds = []
        twlog.msg("sending mail (%d bytes) to" % len(s), recipients)
        ds.append(sendmail(self.relayhost, self.fromaddr, recipients, s))
        return defer.DeferredList(ds)

class PassedMailNotifier(CustomMail):
    '''Mail class that sends out mail for an all-passed build'''
    
    def __init__(self, fromaddr, mode="all", categories=None, builders=None, branch="",
                 addLogs=False, relayhost="localhost",
                 subject="[buildbot-%(result)s] %(blamelist)s: %(branch)s rev %(src)s",
                 lookup=None, extraRecipients=[],
                 sendToInterestedUsers=True,
                 schedulerGroups = [],
                 schedulerGroupsSendFirst = 0
                 ):
        
        # regardless of mode given, override to "all" (required to know if some steps failed)
        mode = "all"
        
        CustomMail.__init__(self, fromaddr, mode, categories, builders, branch,
                 addLogs, relayhost,
                 subject,
                 lookup, extraRecipients,
                 sendToInterestedUsers,
                 schedulerGroups,
                 schedulerGroupsSendFirst
                 )
        
    def sendAggregateMail(self, cachedResults):
        # cachedResults is a list of dicts: {'name':name, 'build':build, 'results':results}
        projectName = self.status.getProjectName()
        
        # build will be the same for all messages, so just use the first one
        build = cachedResults[0]['build']
        
        sourcestamp = build.getSourceStamp()
        source = sourcestamp.revision
        
        # Delimiting wht "," causes email subject line to contain a TAB character for some reason
        blamelist = "|".join(build.getResponsibleUsers())
        p = re.compile(" <[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?>")
        blamelist = p.sub("", blamelist)
        
        slaves = []
        results = []
        for result in cachedResults:
            slaves.append(result['name'])
            results.append(result['results'])

        # If _any_ build failed, then we do not send this email (failure emails are dealt with separately)
        if FAILURE in results:
            return

        text = ''
        text += 'Congratulations, your %s build has passed all acceptance tests!\n' % self.branch
        
        text += '\n'
        text += self.getChangesText(sourcestamp.changes)
        
        m = Message()
        m.set_payload(text)
        
        m['Date'] = formatdate(localtime=True)
        m['Subject'] = self.subject % { 'result': 'pass',
                                        'blamelist': blamelist,
                                        'branch': self.branch,
                                        'src': source,
                                        }

        m['From'] = self.fromaddr
        # m['To'] is added later

        if self.addLogs:
            for result in cachedResults:
                build = result['build']
                for log in build.getLogs():
                    name = "%s.%s" % (log.getStep().getName(),
                                      log.getName())
                    a = MIMEText(log.getText())
                    a.add_header('Content-Disposition', "attachment",
                                 filename=name)
                    m.attach(a)

        # now, who is this message going to?
        dl = []
        recipients = self.extraRecipients[:]
        if self.sendToInterestedUsers and self.lookup:
            for u in build.getInterestedUsers():
                d = defer.maybeDeferred(self.lookup.getAddress, u)
                d.addCallback(recipients.append)
                dl.append(d)
        d = defer.DeferredList(dl)
        d.addCallback(self._gotRecipients, recipients, m)
        return d