LEVEL=..
include $(LEVEL)/Makefile.programs

all:: timeit timeit-target fpcmp TargetConfig.mk

timeit: timeit.c
	$(ORIGINAL_CC) $(CFLAGS) -O3 -o $@ $<

timeit-target: timeit.c
	$(LD_ENV_OVERRIDES) $(LCC) -o $@ $< $(LDFLAGS) $(CFLAGS) $(TARGET_FLAGS) -O3

fpcmp: fpcmp.c
	$(ORIGINAL_CC) $(CFLAGS) -O3 -o $@ $<

TargetConfig.mk: TargetConfig.mk.in
	$(LCC) $(LDFLAGS) $(CFLAGS) $(TARGET_FLAGS) -E -o $@ -x c $<

clean::
	rm -f timeit timeit-target fpcmp TargetConfig.mk
