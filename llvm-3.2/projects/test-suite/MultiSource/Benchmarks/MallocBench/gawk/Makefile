LEVEL = ../../../..
RUN_OPTIONS = -f $(SourceDir)/INPUT/adj.awk type=l linelen=70 indent=5 $(SourceDir)/INPUT/words-large.awk
PROG = gawk
LIBS += -lm
LDFLAGS += -lm

Source = alloca.c awk.tab.c debug.c field.c main.c msg.c regex.c array.c  \
         builtin.c eval.c io.c node.c

all::

awk.tab.c: awk.y
	$(YACC) -v awk.y
	mv y.tab.c awk.tab.c

CPPFLAGS += -DBCOPY_MISSING -DSPRINTF_INT -DDOPRNT_MISSING  -DGCVT_MISSING -DSTRCASE_MISSING -DSTRTOD_MISSING  -DTMPNAM_MISSING
include ../../../Makefile.multisrc

clean::
	rm -f awk.tab.c
