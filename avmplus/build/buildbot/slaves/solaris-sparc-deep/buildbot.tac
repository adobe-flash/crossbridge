#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from twisted.application import service
from buildbot.slave.bot import BuildSlave

basedir = r'/export/home/build/buildbot/tamarin-redux/solaris-sparc-deep'
host = '10.116.42.110'
port = 9750
slavename = 'solaris-sparc-deep'
passwd = 'asteam'
keepalive = 600
usepty = 1
umask = None

application = service.Application('buildslave')
s = BuildSlave(host, port, slavename, passwd, basedir, keepalive, usepty,
               umask=umask)
s.setServiceParent(application)

