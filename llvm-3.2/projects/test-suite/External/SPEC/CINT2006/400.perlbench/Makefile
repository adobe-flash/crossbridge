##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 400.perlbench
# test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

# FIXME: The "test" run type outputs timestamps to STDERR. However, the
# RunSafely.sh script dups STDERR to STDOUT. So testing the difference between
# the two outputs would require ugly hacks. Let's just run the "train" right
# now. If we need to run "test" later, we can apply some type of hack to get it
# to work.

RUN_TYPE := train

include $(LEVEL)/Makefile.config

CPPFLAGS += -DPERL_CORE -DI_TIME

ifeq ($(TARGET_OS),Darwin)
ifeq ($(ARCH),PowerPC)
  CPPFLAGS += -DSPEC_CPU_MACOSX_PPC
endif
ifeq ($(ARCH),x86)
  CPPFLAGS += -DSPEC_CPU_MACOSX_IA32
endif
endif

ifeq ($(ENDIAN),little)
  CPPFLAGS += -D__LITTLE_ENDIAN__
else
  CPPFLAGS += -D__BIG_ENDIAN__
endif

Source = av.c deb.c doio.c doop.c dump.c globals.c gv.c hv.c locale.c mg.c     \
         numeric.c op.c pad.c perl.c perlapi.c perlio.c perlmain.c perly.c     \
         pp.c pp_ctl.c pp_hot.c pp_pack.c pp_sort.c pp_sys.c regcomp.c         \
         regexec.c run.c scope.c sv.c taint.c toke.c universal.c utf8.c util.c \
         xsutils.c Base64.c Cwd.c Dumper.c HiRes.c IO.c Peek.c attrs.c poll.c  \
         stdio.c DynaLoader.c MD5.c Storable.c Parser.c specrand.c Hostname.c  \
         Opcode.c

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  # test.out isn't specified in the REF_IN_DIR/test/data/output directory, so
  # use one of the .out files that does exist in that directory.
  RUN_OPTIONS      = -I./lib test.pl
  STDOUT_FILENAME := append.out
  STDERR_FILENAME := append.err
else
  RUN_OPTIONS      = -I./lib scrabbl.pl
  STDIN_FILENAME  := scrabbl.in
  STDOUT_FILENAME := scrabbl.out
  STDERR_FILENAME := scrabbl.err
endif
