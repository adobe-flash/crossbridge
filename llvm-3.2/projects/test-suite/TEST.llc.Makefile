##===- TEST.llc.Makefile -----------------------------------*- Makefile -*-===##
#
# This test tries running the compiler on all of the programs and
# reports on detailed pass execution times and register allocation and
# peephole optimization statistics.
#
##===----------------------------------------------------------------------===##

LLC_OPTS = $(LLCFLAGS) -o=/dev/null -stats -time-passes
CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: Output/%.llvm.bc $(LLC)
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-(time -p $(LLC) $(LLC_OPTS) $<) >> $@ 2>&1

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@cat $<

# Define REPORT_DEPENDENCIES so that the report is regenerated if llc changes
#
REPORT_DEPENDENCIES := $(LLC)
