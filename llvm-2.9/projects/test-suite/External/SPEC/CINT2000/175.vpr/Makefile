LEVEL = ../../../..
LDFLAGS = -lm
RUN_OPTIONS     = net.in arch.in place.out dum.out -nodisp -place_only -init_t 5 -exit_t 0.005 -alpha_t 0.9412 -inner_num 2
STDOUT_FILENAME := place_log.out
CPPFLAGS := -DNO_GRAPHICS

# SPEC allows a relative FP Tolerance of 0.1 for this test
# Compilers using the fpstack on x86 require this
FP_TOLERANCE := 0.1
include ../../Makefile.spec2000
