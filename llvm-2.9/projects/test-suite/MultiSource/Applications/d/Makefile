LEVEL = ../../..
PROG = make_dparser
Source=make_dparser.c write_ctables.c gram.c lex.c lr.c arg.c parse.c scan.c symtab.c util.c version.c grammar.g.c
CPPFLAGS =  -DD_BUILD_VERSION=5725
RUN_OPTIONS=-v $(PROJ_SRC_DIR)/grammar.g
HASH_PROGRAM_OUTPUT = 1
include ../../Makefile.multisrc
