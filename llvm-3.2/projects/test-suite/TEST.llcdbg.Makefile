##===- TEST.llcdbg.Makefile --------------------------------*- Makefile -*-===##
#
# This test checks whether presence of debug declarations influences
# the code generator or not. 
#
# If input.bc includes llvm.dbg intrinsics and llvm.dbg variables then
# the code in first.s and second.s should match. Otherwise debugging information
# is influencing the code generator.  The Dwarf info in first.s and second.s is
# normally quite different, so the data directives that appear in Dwarf are
# stripped out before comparison.
#
# This has only been used on Darwin; the data directives to strip and grep magic
# might be different elsewhere.
#
##===----------------------------------------------------------------------===##

TESTNAME = $*
TEST_TARGET_FLAGS = -g -O0
LLC_DEBUG_FLAGS = -O0 -regalloc=local $(LLCFLAGS)
.PRECIOUS: Output/%.first.s Output/%.second.s Output/%.t2b.s Output/%.t1b.s

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.diff

Output/%.t1a.bc: Output/%.linked.rbc Output/.dir $(LOPT)
	$(LOPT) -strip-debug-declare -strip-nondebug $< -f -o $@

Output/%.t1b.s: Output/%.t1a.bc Output/.dir $(LLC)
	$(LLC) $(LLC_DEBUG_FLAGS) $< -o $@

Output/%.first.s: Output/%.t1b.s Output/.dir $(LLC)
	grep -v '.long' < $< | grep -v '.byte' | grep -v '.short' | grep -v '.asci' | grep -v '.quad' | grep -v '## DW_AT' | grep -v '## Abbrev' | grep -v '## End Of Children' | grep -v '## DIE' | grep -v '## $$' | grep -v '^$$' > $@

Output/%.t2a.bc: Output/%.linked.rbc Output/.dir $(LOPT)
	$(LOPT) -strip-nondebug $< -f -o $@

Output/%.t2b.s: Output/%.t2a.bc Output/.dir $(LLC)
	$(LLC) $(LLC_DEBUG_FLAGS) $< -o $@

Output/%.second.s: Output/%.t2b.s Output/.dir
	grep -v DEBUG_VALUE < $< | grep -v '.long' | grep -v '.byte' | grep -v '.short' | grep -v '.asci' | grep -v '## DW_AT' | grep -v '## Abbrev' |  grep -v '## End Of Children' | grep -v '## DIE' | grep -v '## $$' | grep -v '.quad' | grep -v '^$$' > $@

Output/%.diff: Output/%.first.s Output/%.second.s
	@-if diff $^ > $@; then \
	 echo "--------- TEST-PASS: $*"; \
	else \
	 echo "--------- TEST-FAIL: $*"; \
	fi
