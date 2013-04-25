/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

The flashrunner tool converts an abc file to a swf. The script builds and runs
the swf from an abc file.  When the swf is run the output is sent over a socket
and printed to stdout.  

Setup:
Follow one-time setup instructions below.

test the setup by running the flashrunner.sh directly:

$ <tamarin>/test/utils/flashrunner/flashrunner.sh hello.abc
- expected output should be print statement output  e.g. 'hello'
- can edit flashrunner.sh set delay=3 (# seconds to pause the player before sending results over socket and closing)
   will see any print statements in the player player

set AVM to flashrunner.sh and call runtests on a set of tests.

$ export AVM=<tamarin>/test/util/flashrunner/flashrunner.sh
$ cd <tamarin>/test/acceptance
$ ./runtests.py as3/AbcDecoder

One time setup:

the following environment variables are required:
ASC - full path to asc.jar
PLAYERGLOBALABC - full path to playerglobal.abc
BUILTINABC - full path to global.abc 
    (e.g. export BUILTINABC=c:/hg/tamarin-redux/core/builtin.abc)
player -
    full path to standalone player executable
    or full path to browser
    or ff or firefox - default firefox path
    or ie or internetexplorer - default ie path
    or safari - default safari browser 

Windows only:
PYTHONWIN - set to full path for native windows python 
    (e.g. export PYTHONWIN=c:/python26/python.exe


to enable local networking set the tamarin directory to your FlashPlayerTrust
    windows: c:/Windows/system32/Macromed/Flash/FlashPlayerTrust
    mac: /Library/Application Support/Macromedia/FlashPlayerTrust
    linux: edit /etc/adobe/FlashPlayerTrust
    in the trust dir create a .cfg file with the path to your tamarin repository:
       test.cfg:
       c:/hg/tamarin-redux

For firefox to disable the Resume previous state warning
    type about:config in url
    set browser.sessionstore.resume_from_crash to false

