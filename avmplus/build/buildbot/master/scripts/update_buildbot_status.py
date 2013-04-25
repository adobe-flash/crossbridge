#!/usr/bin/env python
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os.path,sys,getopt

basedir=os.path.dirname(sys.argv[0])
publichtml=os.path.abspath(basedir+"/../public_html")
statushtml=publichtml+"/status.html"
usage="""
-m --message= new message
-s --style= style from buildbot.css [Successstatus,Failurestatus]
"""

color='Passstatus'
message='Tree is open'
options='m:s:'
longOptions=['message=','style=']
try:
    opts, args=getopt.getopt(sys.argv[1:],options,longOptions)
except:
    print('usage:\n%s'%usage)
    sys.exit(1)
for (o,v) in opts:
    if o in ('-m','--message'):
        message=v
    elif o in ('-s','--style'):
        color=v

contents='<link href="buildbot.css" rel="stylesheet" type="text/css">\n<center><div class="%s">%s<br></div>\n<Center>' % (color,message)

open(statushtml,"w").write(contents)


