##===- TEST.buildrepo.Makefile -----------------------------*- Makefile -*-===##
#
# This "test" is used to copy all compiled bytecode files into the repository.
#
##===----------------------------------------------------------------------===##

ifeq ($(BYTECODE_REPOSITORY),)
ERROR: BYTECODE_REPOSITORY must be defined to build a repository!
endif

# Calculate the directory we should copy the bytecode file into.  This is
# relative to BYTECODE_REPOSITORY and the current directory this program is in.
#
CURDIR  := $(shell cd .; pwd)
PROGDIR := $(PROJ_SRC_ROOT)
DESTDIR := $(BYTECODE_REPOSITORY)/$(subst $(PROGDIR),,$(CURDIR))

.PRECIOUS: $(DESTDIR)/.dir $(DESTDIR)/%.bc

# To Make a file up-to-date, just copy it over.
$(PROGRAMS_TO_TEST:%=$(DESTDIR)/%.bc): \
$(DESTDIR)/%.bc: Output/%.llvm.bc
	cp $< $@

# buildrepo 'test' just requires files in their final destination to be
# up-to-date
#
$(PROGRAMS_TO_TEST:%=test.$(TEST).%): \
test.buildrepo.%: $(DESTDIR)/%.bc
