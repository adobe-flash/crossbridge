LEVEL = ../../..
PROG     = siod
CPPFLAGS = -D__USE_MISC -D__USE_GNU -D__USE_SVID -D__USE_XOPEN_EXTENDED -D__USE_XOPEN -Dunix
LDFLAGS  = -lm $(TOOLLINKOPTS)

RUN_OPTIONS = -v1 $(PROJ_SRC_DIR)/test.scm

# Define USE_REFERENCE_OUTPUT which is a signal to Makefile.programs that the
# natively compiled program should not be run but that its output should be
# "faked" by copying the reference output. This is because gcc-4.6 miscompiles
# siod, resulting in a program that fails to parse the Fibonacci test.
USE_REFERENCE_OUTPUT := 1

include ../../Makefile.multisrc

