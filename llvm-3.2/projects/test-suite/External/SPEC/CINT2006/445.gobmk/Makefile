##===- Makefile --------------------------------------------*- Makefile -*-===##
#
# This makefile contains information for building the SPEC2006 gobmk test.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..

include $(LEVEL)/Makefile.config

SGFSources       := \
         $(addprefix $(SPEC_BENCH_DIR)/src/sgf/,                               \
           sgf_utils.c sgftree.c sgfnode.c)
ENGINESources    := \
         $(addprefix $(SPEC_BENCH_DIR)/src/engine/,                            \
           aftermath.c board.c cache.c combination.c dragon.c filllib.c        \
           fuseki.c genmove.c hash.c influence.c interface.c matchpat.c        \
           move_reasons.c movelist.c optics.c owl.c printutils.c readconnect.c \
           reading.c score.c semeai.c sgfdecide.c sgffile.c shapes.c           \
           showbord.c utils.c value_moves.c worm.c globals.c persistent.c      \
           handicap.c surround.c)
INTERFACESources := \
         $(addprefix $(SPEC_BENCH_DIR)/src/interface/,                         \
           gtp.c main.c play_ascii.c play_gtp.c play_solo.c play_test.c)
PATTERNSSources  := \
         $(addprefix $(SPEC_BENCH_DIR)/src/patterns/,                          \
           connections.c dfa.c helpers.c transform.c owl_attackpat.c conn.c    \
           patterns.c apatterns.c dpatterns.c owl_vital_apat.c eyes.c          \
           influence.c barriers.c endgame.c aa_attackpat.c owl_defendpat.c     \
           fusekipat.c fuseki9.c fuseki13.c fuseki19.c josekidb.c handipat.c)
UTILSSources     := \
         $(addprefix $(SPEC_BENCH_DIR)/src/utils/,                             \
           getopt.c getopt1.c gg_utils.c random.c)

Source := $(addprefix sgf_,$(notdir $(SGFSources)))             \
          $(addprefix engine_,$(notdir $(ENGINESources)))       \
          $(addprefix interface_,$(notdir $(INTERFACESources))) \
          $(addprefix patterns_,$(notdir $(PATTERNSSources)))   \
          $(addprefix utils_,$(notdir $(UTILSSources)))

CPPFLAGS += -DHAVE_CONFIG_H                   \
            -I$(SPEC_BENCH_DIR)/src/sgf       \
            -I$(SPEC_BENCH_DIR)/src/engine    \
            -I$(SPEC_BENCH_DIR)/src/interface \
            -I$(SPEC_BENCH_DIR)/src/patterns  \
            -I$(SPEC_BENCH_DIR)/src/utils     \
            -I$(SPEC_BENCH_DIR)/src           \
            -I$(SPEC_BENCH_DIR)/src/include 

include ../../Makefile.spec2006

ifeq ($(RUN_TYPE),test)
  RUN_OPTIONS     := --quiet --mode gtp
  STDIN_FILENAME  := capture.tst
  STDOUT_FILENAME := capture.out
else
  RUN_OPTIONS     := --quiet --mode gtp
  STDIN_FILENAME  := arb.tst
  STDOUT_FILENAME := arb.out
endif

##===----------------------------------------------------------------------===##
# Copy the source files to the working directory. Some files have the
# same names but are in different directories, so we add a prefix to
# the filenames to prevent overwriting files.
##===----------------------------------------------------------------------===##

$(addprefix sgf_,$(notdir $(SGFSources))) : sgf_% : \
$(SPEC_BENCH_DIR)/src/sgf/%
	cp $< $@
$(addprefix engine_,$(notdir $(ENGINESources))) : engine_% : \
$(SPEC_BENCH_DIR)/src/engine/%
	cp $< $@
$(addprefix interface_,$(notdir $(INTERFACESources))) : interface_% : \
$(SPEC_BENCH_DIR)/src/interface/%
	cp $< $@
$(addprefix patterns_,$(notdir $(PATTERNSSources))) : patterns_% : \
$(SPEC_BENCH_DIR)/src/patterns/%
	cp $< $@
$(addprefix utils_,$(notdir $(UTILSSources))) : utils_% : \
$(SPEC_BENCH_DIR)/src/utils/%
	cp $< $@

##===----------------------------------------------------------------------===##
# Clean up all of the copied source files.
##===----------------------------------------------------------------------===##

clean::
	$(RM) -f $(Source)
