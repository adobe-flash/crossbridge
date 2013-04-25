# -*- Mode: Python; indent-tabs-mode: nil -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os

def run_for_output(cmd):
    if type(cmd) == list:
        # XXX need to escape arguments better... am I in a shell?
        cmd = " ".join(cmd)

    pipe = os.popen(cmd, "r")
    output = pipe.read()
    exitval = pipe.close()
    if (exitval):
        raise Exception("Command failed: '" + cmd + "'")

    return output
