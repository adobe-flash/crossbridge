# -*- test-case-name: buildbot.test.test_status -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


# the email.MIMEMultipart module is only available in python-2.2.2 and later

from twisted.internet import defer
from twisted.python import log

from buildbot import interfaces, util
from buildbot.status import base
from buildbot.status.builder import FAILURE, SUCCESS, WARNINGS



class XMLStatus(base.StatusReceiverMultiService):
    compare_attrs = ["logdir"]
    
    def __init__(self, logdir):
        """
        """
        self.logdir = logdir
        base.StatusReceiverMultiService.__init__(self)

    def setServiceParent(self, parent):
        """
        @type  parent: L{buildbot.master.BuildMaster}
        """
        base.StatusReceiverMultiService.setServiceParent(self, parent)
        self.setup()

    def setup(self):
        self.status = self.parent.getStatus()
        self.status.subscribe(self)

    def disownServiceParent(self):
        self.status.unsubscribe(self)
        return base.StatusReceiverMultiService.disownServiceParent(self)

    def builderAdded(self, name, builder):
        return self # subscribe to this builder

    def builderRemoved(self, name):
        pass

    def builderChangedState(self, name, state):
        pass
    def buildStarted(self, name, build):
        pass
    def buildFinished(self, name, build, results):
        f=open('%s/%s.xml' % (self.logdir,name), 'w')
        status = ""
        if results == FAILURE:
           status = "FAILURE"
        elif results == SUCCESS:
           status = "SUCCESS"
        else: #WARNINGS
           status = "WARNING"
        projectName = self.status.getProjectName()
        f.write('<?xml version="1.0" encoding="UTF-8"?><buildbot><master>%s</master><name>%s</name><status>%s</status></buildbot>' % (projectName,name,status));
        
