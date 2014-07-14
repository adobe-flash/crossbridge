##===- TEST.dbgopt.Makefile --------------------------------*- Makefile -*-===##
#
# This test checks whether presence of debugging information influences
# the optimizer or not. 
#
# If input.bc includes llvm.dbg intrinsics and llvm.dbg variables then
# first.bc and second.bc should match. Otherwise debugging information
# is influencing the optimizer.
#
# $ opt input.bc -strip-nondebug -strip-debug -std-compile-output -strip -o first.bc
# $ opt input.bc -strip-nondebug -std-compile-output -strip-debug -strip -o second.bc
#
##===----------------------------------------------------------------------===##

TESTNAME = $*
TEST_TARGET_FLAGS = -g -O0
.PRECIOUS: Output/%.first.ll Output/%.second.ll

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.diff

Output/%.t1a.bc: Output/%.linked.rbc Output/.dir $(LOPT)
	$(LOPT) -strip-debug-declare -strip-nondebug $< -f -o $@

Output/%.t1b.bc: Output/%.t1a.bc Output/.dir $(LOPT)
	$(LOPT) -strip-debug $< -f -o $@

Output/%.t1c.bc: Output/%.t1b.bc Output/.dir $(LOPT)
	$(LOPT) -std-compile-opts $< -f -o $@

Output/%.t1d.bc: Output/%.t1c.bc Output/.dir $(LOPT)
	$(LOPT) -strip $< -f -o $@

Output/%.first.ll: Output/%.t1d.bc $(LDIS)
	/bin/cp -f $< Output/$*.t.bc
	$(LDIS) Output/$*.t.bc -f -o $@

Output/%.t2b.bc: Output/%.t1a.bc Output/.dir $(LOPT)
	$(LOPT) -std-compile-opts $< -f -o $@

Output/%.t2c.bc: Output/%.t2b.bc Output/.dir $(LOPT)
	$(LOPT) -strip-debug $< -f -o $@

Output/%.t2d.bc: Output/%.t2c.bc Output/.dir $(LOPT)
	$(LOPT) -strip $< -f -o $@

# force both t's to have the same name to avoid bogus filename difference
# force first to be completed before second to avoid race condition copying t
Output/%.second.ll: Output/%.t2d.bc Output/%.first.ll $(LDIS)
	/bin/rm -rf Output/$*.t.bc
	/bin/cp -f $< Output/$*.t.bc
	$(LDIS) Output/$*.t.bc -f -o $@

Output/%.diff: Output/%.first.ll Output/%.second.ll
	@-if diff $^ > $@; then \
	 echo "--------- TEST-PASS: $*"; \
	else \
	 echo "--------- TEST-FAIL: $*"; \
	fi
