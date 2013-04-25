#!/usr/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

'''
Act as a proxy for calls the adb. During automated testing we have
seen the adb process hang and not return for no apparet reason. Subsequent
calls of the exact same adb command to the same device will suceed. The
purpose of this is to handle all calls to adb, kill the adb call if the
execution time takes long than a specififed number of seconds and repeat
the call a specified number of times.
'''


import os,re,sys,subprocess,killableprocess,datetime,tempfile


def main(argv=None):
    if argv is None:
        argv = sys.argv[1:] # don't include the script name

    ADB_TIMEOUT=os.getenv('ADB_TIMEOUT', 300)
    ADB_MAX_ATTEMPTS=os.getenv('ADB_MAX_ATTEMPTS', 2)
    attempt=0

    args=""
    for arg in argv:
        args+=" %s" % arg


    cmd="adb %s" % (args)
    #print "cmd: %s" % cmd
    while attempt < ADB_MAX_ATTEMPTS:

        attempt=attempt+1
        # Need to call the subprocess using a file object for stdout. There have been
        # problems with the OS blocking when large amounts of data is being returned
        # in stdout when making the call to adb. Poth processes must be buffering and
        # causing the process to become blocked.
        f = tempfile.NamedTemporaryFile(delete=False)
        p=killableprocess.Popen(cmd, shell=True, stdout=f ,stderr=subprocess.PIPE)
        try:
            p.wait(ADB_TIMEOUT)
        except OSError:
            '''
            We have seen the following error happen in the build system, so guard against
            this error and just treat like a timeout and retry the call
                "killableprocess.py", line 177, in kill
                    os.killpg(self.pid, signal.SIGKILL)
                    OSError: [Errno 3] No such process
            '''
            p.returncode = 127
        f.flush()
        f.close()

        # -9 and 127 are returned by killableprocess when a timeout happens
        if  p.returncode == -9 or p.returncode == 127:
            failure=file('/tmp/adb_failures', 'a')
            failure.write('%s, %s, %s\n' % (datetime.datetime.now(), attempt, cmd))
            failure.flush()
            failure.close()
            continue

        stdout=""
        outfile = open(f.name, 'r')
        for line in outfile:
            stdout+=line
        outfile.close()
        os.unlink(f.name)
        return stdout

if __name__ == "__main__":
    print main()



