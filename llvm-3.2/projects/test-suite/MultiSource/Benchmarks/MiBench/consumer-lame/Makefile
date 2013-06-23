LEVEL = ../../../..

PROG     = consumer-lame
CPPFLAGS = -DHAVEMPGLIB -DLAMEPARSE -DNDEBUG -D__NO_MATH_INLINES -O -DLAMESNDFILE
LDFLAGS  = -lm
RUN_OPTIONS = -S $(PROJ_SRC_DIR)/large.wav Output/output_large.mp3

# Define USE_REFERENCE_OUTPUT which is a signal to Makefile.programs that the
# natively compiled program should not be run but that its output should be
# "faked" by copying the reference output. This is because gcc-4.6 miscompiles
# consumer-lame, resulting in a program that just dies horribly dumping core.
USE_REFERENCE_OUTPUT := 1

include $(LEVEL)/MultiSource/Makefile.multisrc
