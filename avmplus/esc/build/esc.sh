# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Translate .es to .es.abc using esc on tamarin
#
# You must first build a tamarin executable and store it under the
# name "shell" (or "shell.exe") in the directory $DIR.
#
# Usage:
#   esc.sh [<option> ...] <filename> ...
#
# Options:
#
#   -[no-]es3
#      Recognize only (not only) ES3 keywords (not "future reserved words")
#
#   -[no-]extensions
#      Turn on (off) some extensions to ES4 (UTSL)
#
#   -g
#   -[no-]debug
#      Emit (do not emit) debug information
#
#   -Xprofile
#      Profile the compiler: dump a statistical profile to the file 
#      esc.profile following compilation.
#
# Defaults are -no-es3 -no-extensions -no-debug.
#
# esc.sh must be run from $DIR, or you must change the value of
# $DIR to be the absolute path of the bin directory (that has the
# shell and the abc files for ESC).
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed

DIR=../bin

$DIR/shell \
    $DIR/debug.es.abc \
    $DIR/util.es.abc \
    $DIR/bytes-tamarin.es.abc \
    $DIR/util-tamarin.es.abc \
    $DIR/lex-char.es.abc \
    $DIR/lex-token.es.abc \
    $DIR/lex-scan.es.abc \
    $DIR/ast.es.abc \
    $DIR/define.es.abc \
    $DIR/parse.es.abc \
    $DIR/asm.es.abc \
    $DIR/abc.es.abc \
    $DIR/abc-encode.es.abc \
    $DIR/abc-decode.es.abc \
    $DIR/emit.es.abc \
    $DIR/cogen.es.abc \
    $DIR/cogen-stmt.es.abc \
    $DIR/cogen-expr.es.abc \
    $DIR/esc-core.es.abc \
    $DIR/eval-support.es.abc \
    $DIR/esc-env.es.abc \
    $DIR/sampler.es.abc \
    $DIR/esc.es.abc \
    -- $@
