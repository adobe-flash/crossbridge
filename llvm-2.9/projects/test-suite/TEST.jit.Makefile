##===- TEST.jit.Makefile -----------------------------------*- Makefile -*-===##
#
# This test tries running the Just-In-Time compiler on all of the programs to
# see which ones work and which ones don't.  It provides a report to tabulate
# this nicely and provides some simple statistics.
#
##===----------------------------------------------------------------------===##

JIT_OPTS = -force-interpreter=false -stats -time-passes
CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: Output/%.llvm.bc $(LLI)
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-(time -p $(LLI) $(JIT_OPTS) $< $(RUN_OPTIONS) > /dev/null \
                < $(STDIN_FILENAME)) >> $@ 2>&1

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@cat $<

# Define REPORT_DEPENDENCIES so that the report is regenerated if lli changes
#
REPORT_DEPENDENCIES := $(LLI)
