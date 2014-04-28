#-------------------------------------------------------------------------------

# Basic LINGUAS environment variable support

ALL_LINGUAS := $(basename $(wildcard *.po))

ifneq ($(LINGUAS),)
    LINGUAS := $(filter $(ALL_LINGUAS), $(LINGUAS))
else
    LINGUAS := $(ALL_LINGUAS)
endif

#-------------------------------------------------------------------------------

ROOT := ..

DOMAIN    := neverball
MODIR     := $(ROOT)/locale
POFILES   := $(LINGUAS:%=%.po)
MOFILES   := $(LINGUAS:%=$(MODIR)/%/LC_MESSAGES/$(DOMAIN).mo)
SOURCES   := \
    $(wildcard $(ROOT)/ball/*.[ch])  \
    $(wildcard $(ROOT)/share/*.[ch]) \
    $(wildcard $(ROOT)/putt/*.[ch])

COPYRIGHT := Robert Kooima
BUGADDR   := robert.kooima@gmail.com

XGETTEXT := xgettext
XGETTEXT_FLAGS := \
    --add-comments=Translators --from-code=UTF-8 \
    --keyword=_ --keyword=N_ --keyword=L_        \
    --keyword=sgettext                           \
    --default-domain="$(DOMAIN)"                 \
    --copyright-holder="$(COPYRIGHT)"            \
    --msgid-bugs-address="$(BUGADDR)"

#------------------------------------------------------------------------------

all: $(MOFILES)

clean:
	rm -rf $(MODIR)

#------------------------------------------------------------------------------

# Here it gets somewhat convoluted.  Basic idea is:  we want the
# update-po target which is widely supported in autotools-based
# internationalised packages.  update-po unconditionally updates the POT
# file, and thus forces PO and MO files to be updated as well.  The
# logic is loosely based on Makefile.in.in distributed with GNU gettext.

update-po: $(DOMAIN).pot-update $(POFILES) $(MOFILES)

$(MODIR)/%/LC_MESSAGES/$(DOMAIN).mo: %.po
	mkdir -p $(dir $@)
	msgfmt -c --statistics -o $@ $<

%.po: $(DOMAIN).pot
	msgmerge -U --previous $@ $<
	touch $@

$(DOMAIN).pot $(DOMAIN).pot-update:
	$(XGETTEXT) $(XGETTEXT_FLAGS) -o $(DOMAIN).pot \
	    --directory="$(ROOT)" $(SOURCES:$(ROOT)/%=%)

	# Force encoding to UTF-8
	sed "1,/^$$/ s/CHARSET/UTF-8/" < $(DOMAIN).pot > $(DOMAIN).pot.utf8
	mv -f $(DOMAIN).pot.utf8 $(DOMAIN).pot

	sh $(ROOT)/scripts/extractpo.sh $(ROOT) >> $(DOMAIN).pot
	grep -v '^# ' < mtrl.pot >> $(DOMAIN).pot
	msguniq -o $(DOMAIN).pot.uniq $(DOMAIN).pot
	mv -f $(DOMAIN).pot.uniq $(DOMAIN).pot

#------------------------------------------------------------------------------

stats:
	@find -name '*.po' | sort | xargs -n1 -I{} \
	    sh -c 'printf "{}: "; msgfmt -o /dev/null --statistics {}'

#------------------------------------------------------------------------------

