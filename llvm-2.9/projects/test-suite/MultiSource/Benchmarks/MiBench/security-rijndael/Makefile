LEVEL = ../../../..

PROG     = security-rijndael
LDFLAGS  = -lm
#RUN_OPTIONS = input_large.asc output_large.enc e 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321
RUN_OPTIONS = $(PROJ_SRC_DIR)/output_large.enc Output/output_large.dec d 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321
HASH_PROGRAM_OUTPUT = 1

include $(LEVEL)/MultiSource/Makefile.multisrc
