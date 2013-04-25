# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Translate .abc to .asm
#
# You must first build a tamarin executable and store it under the
# name "shell" (or "shell.exe") in the directory $DIR.
#
# usage:
#   abcencode.sh <filename>
#
# abcencode.sh must be run from $DIR, or you must change the value of
# $DIR to be the absolute path of the bin directory (that has the
# shell and the abc files for ESC).

DIR=../bin

$DIR/shell\
    $DIR/debug.es.abc \
    $DIR/util.es.abc \
    $DIR/bytes-tamarin.es.abc \
    $DIR/util-tamarin.es.abc \
    $DIR/lex-token.es.abc \
    $DIR/asm.es.abc \
    $DIR/abc.es.abc \
    $DIR/abc-encode.es.abc \
    $DIR/abc-parse.es.abc \
    $DIR/abcenc.es.abc \
    -- $@


