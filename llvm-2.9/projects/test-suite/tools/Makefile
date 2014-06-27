LEVEL=..
include $(LEVEL)/Makefile.programs

all:: timeit fpcmp

timeit: timeit.c
	$(CC) $(CFLAGS) -O3 -o $@ $<

fpcmp: fpcmp.c
	$(CC) $(CFLAGS) -O3 -o $@ $<
