LEVEL = ../../../..
#RUN_OPTIONS     = 
STDIN_FILENAME  = crafty.in
STDOUT_FILENAME = crafty.out

Source = $(addprefix $(SPEC_BENCH_DIR)/src/,  \
         attacks.c draw.c enprise.c init.c iterate.c make.c nexte.c output.c \
	 preeval.c resign.c searchr.c swap.c utility.c boolean.c drawn.c     \
	 evaluate.c input.c lookup.c movgen.c nextr.c phase.c quiesce.c      \
	 root.c setboard.c time.c validate.c edit.c history.c interupt.c     \
	 main.c next.c option.c ponder.c repeat.c search.c store.c unmake.c  \
	 valid.c)

include $(LEVEL)/Makefile.config

ifdef ARCH_HAS_LONGLONG
  CPPFLAGS += -DHAS_LONGLONG
endif

ifeq ($(ARCH),Alpha)
  CPPFLAGS += -DALPHA -DLINUX
endif
ifeq ($(ARCH),x86_64)
  CPPFLAGS += -DLONG_HAS_64BITS
endif
ifeq ($(ARCH),PowerPC)
   CPPFLAGS += -DLINUX_PPC32
endif
ifeq ($(ARCH),Sparc)
   CPPFLAGS += -DSUN_BSD
endif
ifeq ($(TARGET_OS),Darwin)
  CPPFLAGS += -DUNIX -DLINUX
endif
ifeq ($(TARGET_OS),Linux)
  CPPFLAGS += -DUNIX -DLINUX
endif
ifeq ($(ENDIAN),little)
  CPPFLAGS += -DLITTLE_ENDIAN_ARCH
endif

include ../../Makefile.spec2000

