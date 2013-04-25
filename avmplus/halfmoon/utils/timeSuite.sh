#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

root=`hg root`
test="$root/halfmoon/test"
utils="$root/halfmoon/utils"
rm -f "$test/benchmarkResults.csv"

find $test/benchmarks -name \*.abc | xargs rm

python "$utils/timeTestSuite.py" $1 "$root/utils/asc.jar" "$root/generated/builtin.abc" "$test/benchmarks"
