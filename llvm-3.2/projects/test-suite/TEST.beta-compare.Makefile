##===- TEST.beta-compare.Makefile --------------------------*- Makefile -*-===##
#
# This test runs llc and llc-beta and generates numbers to compare their static
# values produced by -stats.
#
##===----------------------------------------------------------------------===##

LLC_OPTS = $(LLCFLAGS) -o=/dev/null -stats -time-passes
CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).llc-info.txt): \
Output/%.$(TEST).llc-info.txt: Output/%.llvm.bc $(LLC)
	- $(LLC) $(LLC_OPTS) $< -info-output-file=$@

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).llc-beta-info.txt): \
Output/%.$(TEST).llc-beta-info.txt: Output/%.llvm.bc $(LLC)
	- $(LLC) $(LLC_OPTS) $(LLCBETAOPTION) $< -info-output-file=$@


$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt): \
Output/%.$(TEST).report.txt: Output/%.$(TEST).llc-info.txt Output/%.$(TEST).llc-beta-info.txt
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	-@printf "LLC: " >> $@
	-grep 'Number of machine instrs printed' Output/$*.$(TEST).llc-info.txt >> $@
	-@printf "LLCBETA: " >> $@
	-grep 'Number of machine instrs printed' Output/$*.$(TEST).llc-beta-info.txt >> $@

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@cat $<

$(PROGRAMS_TO_TEST:%=build.$(TEST).%): \
build.$(TEST).%: Output/%.llc Output/%.llc-beta
	@echo "Finished Building: $<"

# Define REPORT_DEPENDENCIES so that the report is regenerated if llc changes
#
REPORT_DEPENDENCIES := $(LLC)
