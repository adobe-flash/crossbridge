LEVEL = ../..

include $(LEVEL)/Makefile.config

PROG = hmmcalibrate
SourceDir := $(HMMER_ROOT)

CPPFLAGS = -DSSE2
LIBS +=    -lpthread
LDFLAGS += -lpthread

ifeq ($(ARCH),PowerPC)
TARGET_FLAGS += -maltivec
LCCFLAGS += -maltivec
endif

# Make sure HMMER temporary file is removed, it will break the test.
$(shell rm -f $(HMMER_ROOT)/globin.hmm.xxx)

ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = --fixed 400 --cpu 1 --num 200000 --seed 1158818515 $(HMMER_ROOT)/globin.hmm
else
RUN_OPTIONS = --fixed 400 --cpu 1 --num 80000 --seed 1158818515 $(HMMER_ROOT)/globin.hmm
endif

include $(LEVEL)/MultiSource/Makefile.multisrc
