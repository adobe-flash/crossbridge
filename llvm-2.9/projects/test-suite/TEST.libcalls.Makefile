##===- TEST.libcalls.Makefile ------------------------------*- Makefile -*-===##
#
# This recursively traverses the programs, and runs the -simplify-libcalls pass
# on each *.linked.rbc bytecode file with -stats set so that it is possible to
# determine which libcalls are being optimized in which programs.
# 
# Usage: 
#     make TEST=libcalls summary (short summary)
#     make TEST=libcalls (detailed list with time passes, etc.)
#     make TEST=libcalls report
#     make TEST=libcalls report.html
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.$(TEST).report.txt
	@cat $<

$(PROGRAMS_TO_TEST:%=Output/%.$(TEST).report.txt):  \
Output/%.$(TEST).report.txt: Output/%.linked.rbc $(LOPT) \
	$(PROJ_SRC_ROOT)/TEST.libcalls.Makefile 
	$(VERB) $(RM) -f $@
	@echo "---------------------------------------------------------------" >> $@
	@echo ">>> ========= '$(RELDIR)/$*' Program" >> $@
	@echo "---------------------------------------------------------------" >> $@
	@-$(LOPT) -simplify-libcalls -stats -debug-only=simplify-libcalls \
	         -time-passes -disable-output $< 2>>$@ 
summary:
	@$(MAKE) TEST=libcalls | egrep '======|simplify-libcalls -'

.PHONY: summary
REPORT_DEPENDENCIES := $(LOPT)
