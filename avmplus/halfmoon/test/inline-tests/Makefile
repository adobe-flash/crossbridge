# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

HM=$(shell hg root)

OBJDIR=$(HM)/objdir
AVMSHELL=env INLINE=1 MODE=4 DEBUG_PRINT_INLINES=1 $(OBJDIR)/shell/avmshell -Dnodebugger
ABCDUMP=$(HM)/utils/abcdump.exe -- -abs 
ASC_CMD=java -jar $(HM)/utils/asc.jar -import $(HM)/generated/builtin.abc -in $(HM)/test/acceptance/shell.as 

ASC_STRICT_CMD=java -jar $(HM)/utils/asc.jar -strict -import $(HM)/generated/builtin.abc

%.abc: %.as
	$(ASC_CMD)  $*.as 

%.abcdump: %.abc
	$(ABCDUMP) $*.abc > $*.abcdump

TESTS=factorial-wrapper.abc factorial.abc inline-static.abc inline-final.abc inline-notfinal.abc inline-finalclass.abc fdc.abc
TESTS_SRC := $(TESTS:%.abc=%.as)
MODE=4


test: check_env $(TESTS)
	for t in $(TESTS); \
	do \
		echo ============ $$t =============; \
		if $(AVMSHELL) $$t; then echo $$t exit 0; else echo $$t exit non zero '(fails)'; exit 1; fi; \
	done
	echo ran and exit 0: $(TESTS_SRC)

check_env:
	test -d $(OBJDIR)
	test -x $(OBJDIR)/shell/avmshell


strict: 
	for t in $(TESTS_SRC); \
	do \
		echo ============ $$t =============; \
		if $(ASC_STRICT_CMD) $$t; then echo $$t exit 0; else echo $$t exit non zero '(fails)'; exit 1; fi; \
	done

fdc.abc: fdc.as
	java -jar $(HM)/utils/asc.jar -import $(HM)/generated/builtin.abc -in $(HM)/test/acceptance/shell.as  fdc.as

# TODO: rejig the env var names. after the fact I recognize the vars should include
# "CALLEE" and "CALLSITE"

# example of how to work on an inlining bug..
# skip inlining unrelated methods (to find actual broken inlines)
# then, when you know a caller/callee inline that breaks,
# set DEBUG_INLINE_ONLY to the caller and DEBUG_INLINE_CANDIDATE to name the callee.
#
# DEBUG_INLINE_SKIP lists candidates to skip inlining
# DEBUG_INLINE_CANDIDATE lists the callees that will be considered for inlining.
# DEBUG_INLINE_ONLY lists the functions whose callsites will be considered as inlining candidates
#
UNRELATED_SKIPS=Object$$/_dontEnumPrototype;FinalDefaultClass

#Assertion failed: "((address < (char*)container + Size(container)))" ("../MMgc/WriteBarrier-inlines.h":199)
SKIPLIST=;$(UNRELATED_SKIPS);FinalDefaultClass$$/getPrivStatString

#it's not simply the inline of setPrivStatString into testGetSetPrivStatString alone -- this one works 
#SKIPLIST=;$(UNRELATED_SKIPS);FinalDefaultClass$$/getPrivStatString;FinalDefaultClass/testGetSetPrivStatString;

#hmm. if also skip inlining setPrivStatString get Error #1023 instead,
#works mode 2
#SKIPLIST=;$(UNRELATED_SKIPS);FinalDefaultClass$$/getPrivStatString;FinalDefaultClass$$/setPrivStatString

#nothing left to inline.. works
#SKIPLIST=;$(UNRELATED_SKIPS);FinalDefaultClass/testGetSetPrivStatString;FinalDefaultClass$$/setPrivStatString;FinalDefaultClass$$/getPrivStatString;

SKIPLIST=;$(UNRELATED_SKIPS);FinalDefaultClass$$/setPrivStatString;FinalDefaultClass$$/getPrivStatString;

DEBUG_ENV= VERBOSE=0 PRINTIR=0 GML=1  DEBUG_PRINT_INLINES=1

#In this case we only attempt one inline, of the callee setPrivStatString into a callsite in testGetSetPrivStatString
fdc: fdc.abc
	env INLINE=1 MODE=$(MODE) $(DEBUG_ENV) DEBUG_INLINE_ONLY="FinalDefaultClassPackage::FinalDefaultClassAccessor/testGetSetPrivStatString" DEBUG_INLINE_CANDIDATE="FinalDefaultClass$$/setPrivStatString" $(OBJDIR)/shell/avmshell $(ARGS) -Dnodebugger fdc.abc

#illustrates skip list (when searching for busted inlines)
fdc2: fdc.abc
	env INLINE=1 MODE=$(MODE) $(DEBUG_ENV) DEBUG_INLINE_ONLY="FinalDefaultClass/testGetSetPrivStatString;FinalDefaultClassPackage::FinalDefaultClassAccessor/testGetSetPrivStatString" DEBUG_INLINE_SKIP="$(SKIPLIST)" $(OBJDIR)/shell/avmshell $(ARGS) -Dnodebugger fdc.abc

clean: 
	rm -f $(TESTS:%.abc=%.abcdump) $(TESTS)
