##===- TEST.lineinfo.Makefile -----------------------------*- Makefile -*--===##
#
# This test is used to measure quality of debugging information.
#
##===----------------------------------------------------------------------===##

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))
COLLECTOR := $(PROJ_SRC_ROOT)/CollectDebugInfoUsingLLDB.py 

REPORTS_TO_GEN := dbg
REPORTS_SUFFIX := $(addsuffix .report.txt, $(REPORTS_TO_GEN))

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.dbg Output/%.dbg.opt Output/%.native.dbg Output/%.native.dbg.opt
	$(PROJ_SRC_ROOT)/PrintLineNo.sh $*; \
	$(PROJ_SRC_ROOT)/FindMissingLineNo.py $* $(PROJ_SRC_ROOT);

