##===- TEST.typesafe.Makefile ------------------------------*- Makefile -*-===##
#
# This test simply checks to see if programs are typesafe according to the
# -unsafepointertypes analysis.
#
##===----------------------------------------------------------------------===##

$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.$(TEST).%: Output/%.llvm.bc
	$(LANALYZE) -unsafepointertypes $<

