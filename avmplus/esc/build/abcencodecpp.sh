# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

### translate .abc to .h/.cpp
../bin/shell ../bin/debug.es.abc ../bin/ast.es.abc ../bin/ast-decode.es.abc ../bin/ast-encode.es.abc ../bin/util.es.abc ../bin/lex-char.es.abc ../bin/lex-token.es.abc ../bin/lex-scan.es.abc ../bin/define.es.abc ../bin/parse.es.abc ../bin/util-tamarin.es.abc ../bin/bytes-tamarin.es.abc ../bin/util-tamarin.es.abc ../bin/asm.es.abc ../bin/abc.es.abc ../bin/abc-encode.es.abc ../bin/abc-parse.es.abc ../bin/emit.es.abc ../bin/cogen.es.abc ../bin/cogen-stmt.es.abc ../bin/cogen-expr.es.abc ../bin/abc-encode-cpp.es.abc -- $1 $2
