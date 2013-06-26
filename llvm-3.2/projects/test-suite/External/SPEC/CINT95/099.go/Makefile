LEVEL = ../../../..
BM=099.go

include ../../Makefile.spec95

ifeq ($(RUN_TYPE),test)
STDIN_FILENAME  = null.in
STDOUT_FILENAME = null.out
RUN_OPTIONS     = 40 19
else
STDIN_FILENAME  = 2stone9.in
STDOUT_FILENAME = 2stone9.out
RUN_OPTIONS     = 50 9
endif

ifeq ($(ARCH),Alpha)

# So on alpha, gcc misoptimizes things.  So conditionally (on if we are on alpha)
# change the optimization levels of the cbe and native
# note llc compiles things correctly, this is only a gcc workaround

Output/%.o: %.c Output/.dir
	-$(CC) $(CPPFLAGS) $(CFLAGS) -O1 -c $< -o $@

$(PROGRAMS_TO_TEST:%=Output/%.cbe): \
Output/%.cbe: Output/%.cbe.c
	-$(CC) $< $(LDFLAGS) $(CFLAGS) -fno-strict-aliasing -O0 -o $@

endif
