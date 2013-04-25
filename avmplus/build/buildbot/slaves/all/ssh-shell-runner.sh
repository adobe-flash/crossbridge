#!/bin/sh
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

# TODO: MIPS boxes are not sourcing /etc/profile or /etc/bashrc when
#       a non-interactive SSH connection is made. This means that the
#       env var TZ will not be set causing failures in date tests
#
#       Start of Daylight saving time : M3.2.0/02:00:00
#       meaning 3rd month, 2nd week, 0th day(sunday), 2 am
#       End of Daylight saving time : M11.1.0/02:00:00 
#       meaning 11th month, 1st week, 0th day(sunday), 2 am

export TZ="EST5EDT,M3.2.0/02:00:00,M11.1.0/02:00:00"

./avmshell $*
echo "EXITCODE=$?"

