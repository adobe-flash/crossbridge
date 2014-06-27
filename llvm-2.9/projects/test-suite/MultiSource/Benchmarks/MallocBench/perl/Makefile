LEVEL = ../../../..
PROG = perl
REQUIRES_EH_SUPPORT = 1
CPPFLAGS += -DHAS_STRERROR -DHAS_MKDIR -DHAS_RMDIR -U_POSIX_SOURCE -D__USE_MISC
LIBS += -lm -lcrypt
LDFLAGS += -lm -lcrypt

Source := array.c doarg.c eval.c regexec.c \
          cmd.c doio.c form.c perl.c stab.c usersub.c \
          cons.c dolist.c hash.c perly.c str.c util.c \
          consarg.c dump.c malloc.c regcomp.c toke.c

RUN_OPTIONS = $(PROJ_SRC_DIR)/INPUT/endsort.perl $(PROJ_SRC_DIR)/INPUT/endsort.perl
include ../../../Makefile.multisrc

