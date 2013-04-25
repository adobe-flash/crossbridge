# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Translate .asm to .abc
#
# You must first build a tamarin executable and store it under the
# name "shell" (or "shell.exe") in the directory $DIR.
#
# usage:
#   abcdecode.sh <filename>
#
# abcdecode.sh must be run from $DIR, or you must change the value of
# $DIR to be the absolute path of the bin directory (that has the
# shell and the abc files for ESC).

DIR=../bin

$DIR/shell\
    $DIR/debug.es.abc \
    $DIR/util.es.abc \
    $DIR/lex-char.es.abc \
    $DIR/lex-token.es.abc \
    $DIR/lex-scan.es.abc \
    $DIR/ast.es.abc \
    $DIR/define.es.abc \
    $DIR/parse.es.abc \
    $DIR/util-tamarin.es.abc \
    $DIR/bytes-tamarin.es.abc \
    $DIR/util-tamarin.es.abc \
    $DIR/asm.es.abc \
    $DIR/abc.es.abc \
    $DIR/abc-encode.es.abc \
    $DIR/abc-decode.es.abc \
    $DIR/emit.es.abc \
    $DIR/cogen.es.abc \
    $DIR/cogen-stmt.es.abc \
    $DIR/cogen-expr.es.abc \
    $DIR/esc-core.es.abc \
    $DIR/abcdec.es.abc \
    -- $@
