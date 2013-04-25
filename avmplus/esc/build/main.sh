# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Interactive/non-interactive interpreter with eval.
#
# If file name arguments are present then they are compiled, 
# loaded, and executed before the shell exits.  Otherwise 
# the shell enters a repl.

if [[ "$ESCBIN" = "" ]]
then
	ESCBIN=../bin
fi

$ESCBIN/shell \
    $ESCBIN/debug.es.abc \
    $ESCBIN/util.es.abc \
    $ESCBIN/bytes-tamarin.es.abc \
    $ESCBIN/util-tamarin.es.abc \
    $ESCBIN/lex-char.es.abc \
    $ESCBIN/lex-token.es.abc \
    $ESCBIN/lex-scan.es.abc \
    $ESCBIN/ast.es.abc \
    $ESCBIN/define.es.abc \
    $ESCBIN/parse.es.abc \
    $ESCBIN/asm.es.abc \
    $ESCBIN/abc.es.abc \
    $ESCBIN/emit.es.abc \
    $ESCBIN/cogen.es.abc \
    $ESCBIN/cogen-stmt.es.abc \
    $ESCBIN/cogen-expr.es.abc \
    $ESCBIN/esc-core.es.abc \
    $ESCBIN/eval-support.es.abc \
    $ESCBIN/esc-env.es.abc \
    $ESCBIN/main.es.abc \
    -- $@
