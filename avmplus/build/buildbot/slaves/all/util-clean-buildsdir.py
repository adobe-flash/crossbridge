#!/usr/bin/env python

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

import os
import sys
import time
import shutil
from optparse import OptionParser
from datetime import date

@staticmethod
def _onerror(func, path, exc_info): 

     import stat
     if not os.access(path, os.W_OK):
        try:
            os.chmod(path, stat.S_IWUSR)
            func(path)
        except Exception,e:
            print e
            print "buildbot_status: WARNINGS"
            pass

def main():
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)
    parser.add_option("-p", "--path", type="string", dest="directory",
                      help="path to tree to delete")
    parser.add_option("-i", "--interval", type="int", dest="interval",
                      help="number of days to preserve folders")
    
    (options, args) = parser.parse_args()
    if not options.directory or not options.interval:
        parser.error("-p <path> and -i <interval> arguments required")
    
    path = options.directory        
    numdays = options.interval * 86400 # 86400 = number of seconds in a day
    now = time.time()
    
    for r,d,f in os.walk(path):
        for dir in d:
             timestamp = os.path.getmtime(os.path.join(r,dir))
             if now-timestamp > numdays:
                 try:
                      print "Removing: %s -- Date: %s" % (os.path.join(r,dir), date.fromtimestamp(timestamp))
                      shutil.rmtree(os.path.join(r,dir), onerror=_onerror)  # comment to test
                 except Exception,e:
                      print e
                      print "buildbot_status: WARNINGS"
                      pass
                      
if __name__ == "__main__":
    main()
