# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# For GNU make.
#
# Recompile the compiler using the compiler in ../bin.
#
# Fall back on the script in comp-es.sh if this makefile gives you trouble.
# Please update comp-es.sh if you change TARGETS below.
#
# ESC uses some of the extensions (currently "dynamic override") so
# -extensions must be enabled when compiling ESC.

ESC=./esc.sh
#ESCOPT=-debug -extensions -Xprofile
ESCOPT=-debug -extensions

DIR=../src

SRCS=$(DIR)/debug.es $(DIR)/util.es $(DIR)/util-tamarin.es $(DIR)/lex-char.es $(DIR)/lex-token.es \
	$(DIR)/lex-scan.es $(DIR)/ast.es $(DIR)/define.es $(DIR)/parse.es \
	$(DIR)/bytes-tamarin.es $(DIR)/asm.es $(DIR)/abc.es $(DIR)/abc-parse.es \
	$(DIR)/abc-encode.es $(DIR)/abc-encode-cpp.es $(DIR)/abc-decode.es \
	$(DIR)/abcenc.es $(DIR)/abcdec.es \
	$(DIR)/emit.es $(DIR)/cogen.es $(DIR)/cogen-stmt.es $(DIR)/cogen-expr.es \
	$(DIR)/esc-core.es $(DIR)/esc.es $(DIR)/main.es $(DIR)/sampler.es \
	$(DIR)/eval-support.es $(DIR)/esc-env.es

TARGETS=$(DIR)/debug.es.abc $(DIR)/util.es.abc $(DIR)/util-tamarin.es.abc $(DIR)/lex-char.es.abc $(DIR)/lex-token.es.abc \
	$(DIR)/lex-scan.es.abc $(DIR)/ast.es.abc $(DIR)/define.es.abc $(DIR)/parse.es.abc \
	$(DIR)/bytes-tamarin.es.abc $(DIR)/asm.es.abc $(DIR)/abc.es.abc $(DIR)/abc-parse.es.abc \
	$(DIR)/abc-encode.es.abc $(DIR)/abc-encode-cpp.es.abc $(DIR)/abc-decode.es.abc \
	$(DIR)/abcenc.es.abc $(DIR)/abcdec.es.abc \
	$(DIR)/emit.es.abc $(DIR)/cogen.es.abc $(DIR)/cogen-stmt.es.abc $(DIR)/cogen-expr.es.abc \
	$(DIR)/esc-core.es.abc $(DIR)/esc.es.abc $(DIR)/main.es.abc $(DIR)/sampler.es.abc \
	$(DIR)/eval-support.es.abc $(DIR)/esc-env.es.abc

default: $(TARGETS)
	cp $(TARGETS) ../bin

all:	clean
	$(MAKE)

clean:
	rm -f $(TARGETS)

recompile: clean
	$(ESC) $(ESCOPT) $(SRCS)
	$(MAKE)

$(TARGETS): %.es.abc: %.es
	$(ESC) $(ESCOPT) $<
