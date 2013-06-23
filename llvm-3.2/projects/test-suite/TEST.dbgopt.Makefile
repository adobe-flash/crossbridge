##===- TEST.dbgopt.Makefile --------------------------------*- Makefile -*-===##
#
# This test checks whether presense of debugging information influences
# the optimizer or not. 
#
# $ clang -fno-verbose-asm -g -mllvm --disable-debug-info-print -Os \
#      -S foo.c -o foo.first.s
# $ clang -fno-verbose-asm -Os -S foo.c -o foo.second.s
# $ diff foo.first.s foo.second.s
#
##===----------------------------------------------------------------------===##

TESTNAME = $*
.PRECIOUS: Output/%.first.s Output/%.second.s 

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.diff

Output/%.s: %.c Output/.dir $(INCLUDES)
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -g -Os \
	  -fno-verbose-asm -mllvm --disable-debug-info-print \
	  -S ${PROJ_SRC_DIR}/$*.c -o Output/$*.first.s
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -Os \
	  -fno-verbose-asm -S ${PROJ_SRC_DIR}/$*.c -o Output/$*.second.s
	echo "---------------------------------------------------------------" \
	   > Output/$*.dbgopt.report.txt; \
	echo ">>> ========= '$*' Program" >> Output/$*.dbgopt.report.txt; \
	echo "---------------------------------------------------------------\n"\
	   >> Output/$*.dbgopt.report.txt; 
	@-if diff Output/$*.first.s Output/$*.second.s > $@; then \
	 echo "TEST: PASS" >> Output/$*.dbgopt.report.txt; \
	else \
	 echo "TEST: FAIL" >> Output/$*.dbgopt.report.txt; \
	fi

Output/%.s: %.cpp Output/.dir $(INCLUDES)
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -g -Os \
	  -fno-verbose-asm -mllvm --disable-debug-info-print \
	  -S ${PROJ_SRC_DIR}/$*.cpp -o Output/$*.first.s
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -Os -S \
	  -fno-verbose-asm ${PROJ_SRC_DIR}/$*.cpp -o Output/$*.second.s
	echo "---------------------------------------------------------------" \
	   > Output/$*.dbgopt.report.txt; \
	echo ">>> ========= '$*' Program" >> Output/$*.dbgopt.report.txt; \
	echo "---------------------------------------------------------------\n"\
	   >> Output/$*.dbgopt.report.txt; 
	@-if diff Output/$*.first.s Output/$*.second.s > $@; then \
	 echo "TEST: PASS" >> Output/$*.dbgopt.report.txt; \
	else \
	 echo "TEST: FAIL" >> Output/$*.dbgopt.report.txt; \
	fi

Output/%.s: %.cc Output/.dir $(INCLUDES)
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -g -Os \
	  -fno-verbose-asm -mllvm -disable-debug-info-print \
	  -S ${PROJ_SRC_DIR}/$*.cc -o Output/$*.first.s
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -Os -S \
	  -fno-verbose-asm ${PROJ_SRC_DIR}/$*.cc -o Output/$*.second.s
	echo "---------------------------------------------------------------" \
	   > Output/$*.dbgopt.report.txt; \
	echo ">>> ========= '$*' Program" >> Output/$*.dbgopt.report.txt; \
	echo "---------------------------------------------------------------\n"\
	   >> Output/$*.dbgopt.report.txt; 
	@-if diff Output/$*.first.s Output/$*.second.s > $@; then \
	 echo "TEST: PASS" >> Output/$*.dbgopt.report.txt; \
	else \
	 echo "TEST: FAIL" >> Output/$*.dbgopt.report.txt; \
	fi

Output/%.s: %.m Output/.dir $(INCLUDES)
	-$(LLVMCC) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -g -Os \
	  -fno-verbose-asm -mllvm --disable-debug-info-print \
	  -S ${PROJ_SRC_DIR}/$*.m -o Output/$*.first.s
	-$(LLVMCC) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -Os -S \
	  -fno-verbose-asm ${PROJ_SRC_DIR}/$*.m -o Output/$*.second.s
	echo "---------------------------------------------------------------" \
	   > Output/$*.dbgopt.report.txt; \
	echo ">>> ========= '$*' Program" >> Output/$*.dbgopt.report.txt; \
	echo "---------------------------------------------------------------\n"\
	   >> Output/$*.dbgopt.report.txt; 
	@-if diff Output/$*.first.s Output/$*.second.s > $@; then \
	 echo "TEST: PASS" >> Output/$*.dbgopt.report.txt; \
	else \
	 echo "TEST: FAIL" >> Output/$*.dbgopt.report.txt; \
	fi

Output/%.s: %.mm Output/.dir $(INCLUDES)
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -g -Os \
	  -fno-verbose-asm -mllvm --disable-debug-info-print \
	  -S ${PROJ_SRC_DIR}/$*.mm -o Output/$*.first.s
	-$(LLVMCC) $(CPPFLAGS) $(CFLAGS) $(LOPTFLAGS) $(X_TARGET_FLAGS) -Os -S \
	  -fno-verbose-asm ${PROJ_SRC_DIR}/$*.mm -o Output/$*.second.s
	echo "---------------------------------------------------------------" \
	   > Output/$*.dbgopt.report.txt; \
	echo ">>> ========= '$*' Program" >> Output/$*.dbgopt.report.txt; \
	echo "---------------------------------------------------------------\n"\
	   >> Output/$*.dbgopt.report.txt; 
	@-if diff Output/$*.first.s Output/$*.second.s > $@; then \
	 echo "TEST: PASS" >> Output/$*.dbgopt.report.txt; \
	else \
	 echo "TEST: FAIL" >> Output/$*.dbgopt.report.txt; \
	fi

Asms    := $(sort $(addsuffix .s, $(notdir $(basename $(Source)))))
AllAsms := $(addprefix Output/,$(Asms))

Output/%.diff: $(AllAsms)
	

