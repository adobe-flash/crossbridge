#!/usr/bin/env python
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from cmdutils import *
import os

def run():
    r=RunTestLib()
    r.run_test(
      name="mmfx_new_array_opts_canFail",
      command="%s -Dselftest=mmgc,mmfx_array,mmfx_new_array_opts_canFail -memlimit 1024" % r.avm,
      expectedout=["['start', 'mmgc', 'mmfx_array']",
                   "['test', 'mmgc', 'mmfx_array', 'mmfx_new_array_opts_canFail']",
                   "['pass', 'mmgc', 'mmfx_array', 'mmfx_new_array_opts_canFail']",
                   "['end', 'mmgc', 'mmfx_array']"]
    )

    r.run_test(
      name="mmfx_new_array_opts_oom",
      command="%s -Dselftest=mmgc,mmfx_array,mmfx_new_array_opts_oom -memlimit 1024" % r.avm,
      expectedcode=128
    )

    r.run_test(
      name="mmfx_new_array_oom",
      command="%s -Dselftest=mmgc,mmfx_array,mmfx_new_array_oom -memlimit 1024" % r.avm,
      expectedcode=128
    )


if __name__ == '__main__':
    r=RunTestLib()
    run()
