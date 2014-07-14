##===- TEST.dbg.Makefile ----------------------------------*- Makefile -*--===##
#
# This test is used to measure quality of debugging information.
#
##===----------------------------------------------------------------------===##

#----------------------------------------------------------------------
# Be sure to add the python path that points to the LLDB shared library.
# On MacOSX csh, tcsh:
#   setenv PYTHONPATH /Developer/Library/PrivateFrameworks/LLDB.framework/Resources/Python
# On MacOSX sh, bash:
#   export PYTHONPATH=/Developer/Library/PrivateFrameworks/LLDB.framework/Resources/Python
#----------------------------------------------------------------------

CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
RELDIR  := $(subst $(PROGDIR),,$(CURDIR))

REPORTS_TO_GEN := dbg
REPORTS_SUFFIX := $(addsuffix .report.txt, $(REPORTS_TO_GEN))

Output/%.bp: %.c Output/.dir
	$(LCC) $(CPPFLAGS) $(CFLAGS) -g -c -emit-llvm $< -o $@.bc
	$(LOPT) -print-breakpoints-for-testing $@.bc -o $@

Output/%.bp: %.cpp Output/.dir
	$(LCXX) $(CPPFLAGS) $(CFLAGS) -g -c -emit-llvm $< -o $@.bc
	$(LOPT) -print-breakpoints-for-testing $@.bc -o $@

Output/%.bp: %.m Output/.dir
	$(LCC) $(CPPFLAGS) $(CFLAGS) -g -c -emit-llvm $< -o $@.bc
	$(LOPT) -print-breakpoints-for-testing $@.bc -o $@

Output/%.bp: %.mm Output/.dir
	$(LCXX) $(CPPFLAGS) $(CFLAGS) -g -c -emit-llvm $< -o $@.bc
	$(LOPT) -print-breakpoints-for-testing $@.bc -o $@

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.bp Output/%.dbg Output/%.dbg.opt Output/%.native.dbg Output/%.native.dbg.opt
	@-is_skip=0; \
	if test "$*" == "reversefile"; then \
	  is_skip=1; \
	fi; \
	if test "$*" == "spellcheck"; then \
	  is_skip=1; \
	fi; \
	if test "$*" == "sumcol"; then \
	  is_skip=1; \
	fi; \
	if test "$*" == "wc"; then \
	  is_skip=1; \
	fi; \
	if test "$*" == "wordfreq"; then \
	  is_skip=1; \
	fi; \
	if test "$*" == "exptree"; then \
	  is_skip=1; \
	fi; \
	if test $$is_skip == 0; then \
	  $(LLVM_SRC_ROOT)/utils/CollectDebugInfoUsingLLDB.py Output/$*.dbg Output/$*.bp Output/$*.dbg.out; \
	  $(LLVM_SRC_ROOT)/utils/CollectDebugInfoUsingLLDB.py Output/$*.dbg.opt Output/$*.bp Output/$*.dbg.opt.out; \
	  $(LLVM_SRC_ROOT)/utils/CollectDebugInfoUsingLLDB.py Output/$*.native.dbg Output/$*.bp Output/$*.native.dbg.out; \
	  $(LLVM_SRC_ROOT)/utils/CollectDebugInfoUsingLLDB.py Output/$*.native.dbg.opt Output/$*.bp Output/$*.native.dbg.opt.out; \
	  $(LLVM_SRC_ROOT)/utils/CompareDebugInfo.py $*; \
	fi

