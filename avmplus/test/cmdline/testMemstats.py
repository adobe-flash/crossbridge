#!/usr/bin/env python

#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

# import the utilities from cmdutils.py
import os
from cmdutils import *

def run():
    r=RunTestLib()
    r.run_test(name='memstats',
               command="%s -memstats testdata/memstats.abc" % r.avm,
               expectedout=[
                            'gross stats',
                            'managed overhead',
                            'gross stats end',
                            'sweep\\([0-9]+\\) reclaimed [0-9]+ whole pages'
                            ]
               )
    if os.name!='nt':
        print("pyspy     requires windows named pipes, does not work with cygwin python or non-windows operating systems")
    else:
        failed=False
        try:
            import win32event
            import win32pipe
            import win32file
        except:
            failed=True
            print("pyspy failed to load python win32 extension  FAILED")
        if failed==False:
            os.putenv('MMGC_PROFILE','1')
            proc=r.run_command_async(command="%s testdata/memstats.abc" % r.avm,sleep=2)
# pyspy source       
            e = "MMgc::MemoryProfiler::DumpFatties"
            h = None
            try:
                h = win32event.OpenEvent(win32event.EVENT_MODIFY_STATE, False, e)
            except Exception:
                print("Error: No registered event: %s FAILED!" % e)
                sys.exit(1)

            win32event.SetEvent(h)

            pipe = "\\\\.\\pipe\MMgc_Spy" 
            readHandle = None
            while True:
                try:
                    readHandle = win32file.CreateFile(pipe, win32file.GENERIC_READ, 0, None, win32file.OPEN_EXISTING, 0, None)
                    win32pipe.WaitNamedPipe(pipe, 100)
                except Exception:
                    pass
                if readHandle:
                    break

                while True:
                    try:
                        data = win32file.ReadFile(readHandle, 128)
                        sys.stdout.write(data[1])
                    except:
                        break

# by specifying a main can run this test individually
if __name__ == '__main__':
    r=RunTestLib()
    r.compile("testdata/memstats.as",None,"-d")
    run()
