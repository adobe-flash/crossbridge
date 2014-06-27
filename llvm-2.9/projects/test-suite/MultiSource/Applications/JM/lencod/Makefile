LEVEL = ../../../..
PROG     = lencod
CPPFLAGS = -D __USE_LARGEFILE64 -D _FILE_OFFSET_BITS=64
LDFLAGS  = -lm $(TOOLLINKOPTS)
FP_TOLERANCE = 0.025

ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = -d $(PROJ_SRC_DIR)/data/encoder_small.cfg -p InputFile=$(PROJ_SRC_DIR)/data/foreman_part_qcif_444.yuv -p LeakyBucketRateFile=$(PROJ_SRC_DIR)/data/leakybucketrate.cfg -p QmatrixFile=$(PROJ_SRC_DIR)/data/q_matrix.cfg
else
RUN_OPTIONS = -d $(PROJ_SRC_DIR)/data/encoder.cfg -p InputFile=$(PROJ_SRC_DIR)/data/foreman_part_qcif_444.yuv -p LeakyBucketRateFile=$(PROJ_SRC_DIR)/data/leakybucketrate.cfg -p QmatrixFile=$(PROJ_SRC_DIR)/data/q_matrix.cfg
endif

include ../../../Makefile.multisrc

clean::
	rm -f data.txt log.dat stats.dat
