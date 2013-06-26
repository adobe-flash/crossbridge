##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 xalancbmk test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
include $(LEVEL)/Makefile.config

CPPFLAGS += -DNDEBUG -DAPP_NO_THREADS -DXALAN_INMEM_MSG_LOADER        \
            -DPROJ_XMLPARSER -DPROJ_XMLUTIL -DPROJ_PARSERS            \
            -DPROJ_SAX4C -DPROJ_SAX2 -DPROJ_DOM -DPROJ_VALIDATORS     \
            -DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER \
            -DXML_USE_PTHREADS                                        \
            -I$(SPEC_BENCH_DIR)/src/xercesc                           \
            -I$(SPEC_BENCH_DIR)/src/xercesc/dom                       \
            -I$(SPEC_BENCH_DIR)/src/xercesc/dom/impl                  \
            -I$(SPEC_BENCH_DIR)/src/xercesc/sax                       \
            -I$(SPEC_BENCH_DIR)/src/xercesc/util/MsgLoaders/InMemory  \
            -I$(SPEC_BENCH_DIR)/src/xercesc/util/Transcoders/Iconv    \
            -I$(SPEC_BENCH_DIR)/src/xalanc/include

ifeq ($(TARGET_OS),Darwin)
  CPPFLAGS += -DSPEC_CPU_MACOSX
endif
ifeq ($(TARGET_OS),Linux)
  CPPFLAGS += -DSPEC_CPU_LINUX
endif

LDFLAGS = -lstdc++ -lm
LIBS    = -lstdc++ -lm

include ../../Makefile.spec2006

# When building with -g, opt and llvm-link can take a long time to run.
RUNTIMELIMIT = 1500

ifdef SMALL_PROBLEM_SIZE

Output/483.xalancbmk.out-nat Output/483.xalancbmk.out-simple Output/483.xalancbmk.out-llc Output/483.xalancbmk.out-llc-beta Output/483.xalancbmk.out-opt-beta: Output/small-test.xml Output/100mb.xsd

Output/small-test.xml: $(SPEC_BENCH_DIR)/data/test/input/test.xml
	$(VERB) sed -E -e 's/([a-z]+ ){5}//g' $< > $@

Output/100mb.xsd: $(SPEC_BENCH_DIR)/data/test/input/100mb.xsd
	$(VERB) cp $< $@

endif

ifeq ($(RUN_TYPE),test)
ifdef SMALL_PROBLEM_SIZE
  RUN_OPTIONS      = -v ../small-test.xml xalanc.xsl
else
  RUN_OPTIONS      = -v test.xml xalanc.xsl
endif
  STDOUT_FILENAME := test.out
else
  RUN_OPTIONS      = -v allbooks.xml xalanc.xsl
  STDOUT_FILENAME := train.out
endif
