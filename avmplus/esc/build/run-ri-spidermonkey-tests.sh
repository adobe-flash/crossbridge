# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

SMTESTDIR=../../../../mtn/com.mozilla.es4.smlnj/tests/spidermonkey/
DIR=../../test/acceptance

$DIR/runtests.py -v --ext=.js --esc --escbin=../bin/ $SMTESTDIR
