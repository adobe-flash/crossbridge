LEVEL = ../../..

PROG     = burg
CPPFLAGS = -DDEBUG
CFLAGS  += -std=gnu89
LDFLAGS  = -lstdc++

ExtraSource := y.tab.c

STDIN_FILENAME = $(PROJ_SRC_DIR)/sample.gr

include $(LEVEL)/Makefile.config

Source  := $(ExtraSource) $(wildcard $(PROJ_SRC_DIR)/*.c)

include	$(LEVEL)/MultiSource/Makefile.multisrc

#
# JTC -
#	This is stupid.  I bet we'll yacc twice because of this, but for now it
#	seems to work.  Ideally, these yacc rules should be in a master rule
#	file for the test suite, separate from the LLVM build rules.
# 
y.tab.h:: gram.y
	$(YACC) -d $<

y.tab.c: gram.y
	$(YACC) -d $<

lex.c: y.tab.h

src:
	echo Sources = $(Source)

clean::
	rm -f y.tab.* gram.cpp gram.h
