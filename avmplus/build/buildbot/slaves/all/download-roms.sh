#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

remotekerneldir="http://asteam/hg/tamarin-central-private/raw-file/tip/test/performance/canaries/fc64util/kernel/"
files="basic.901226-01.bin characters.901225-01.bin kernal.901227-03.bin"

test "$basedir" = "" && {
    basedir="."
}

kerneldir=$basedir/other-licenses/test/performance/canaries/fc64util/kernel
test -d $kerneldir || mkdir -p $kerneldir

for f in $files
do
    echo downloading rom to $kerneldir/$f
    wget --quiet --output-file=$kerneldir/$f $remotekerneldir/$f
    ls -l $kerneldir/$f
done