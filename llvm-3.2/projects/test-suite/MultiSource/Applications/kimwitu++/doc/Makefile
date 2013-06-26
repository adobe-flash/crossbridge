# Kimwitu++ documentation Makefile
# Copyright (c) 2000 Michael Piefel, Humboldt-University Berlin
# This is GPLed, but who would want it?

.PHONY: all pdf tex dvi force index clean html

XMLs := kpp-main.xml kpp-intro.xml kpp-manual.xml kpp-cook.xml kpp-rpn.xml
FIGs := syntaxtreex.fig simplify.fig fprintdot.fig
FIGDIR:= figures
IMGDIR:= imagesgen
DEST:=manual

EPSs := $(subst .fig,.eps,$(FIGs))
PDFs := $(subst .fig,.pdf,$(FIGs))
PNGs := $(subst .fig,.png,$(FIGs))
PSTEXs := $(addprefix $(IMGDIR)/, $(subst .fig,.tex,$(FIGs)))
FIGs := $(addprefix $(FIGDIR)/,$(FIGs))
EPSs := $(addprefix $(IMGDIR)/,$(EPSs))
PDFs := $(addprefix $(IMGDIR)/,$(PDFs))
PNGs := $(addprefix $(IMGDIR)/,$(PNGs))

all:
	@echo "Call with pdf, tex or target document name"

pdf: $(DEST).pdf

tex: $(DEST).tex

dvi: $(DEST).dvi

html: $(DEST).html

force:
	touch $(DEST).tex

index:
	makeindex $(MIFLAGS) $(DEST)

$(DEST).dvi: $(DEST).tex $(EPSs) $(PSTEXs)
%.dvi: %.tex
	latex $<


#%.tex: %.xml
#	xsltproc formatters/docbook2tex.xsl $< > $@

.PRECIOUS: kpp-main.fo

%.fo: %.xml
	xsltproc formatters/kimwitu++.xsl $< > $@

%.pdf: %.fo
#	New FOP does not seem to need this adaption any more
#	xsltproc formatters/fo2fop.xsl $< > $@-fop.fo
#	fop $@-fop.fo $@
	fop $< -pdf $@

$(DEST).tex: $(XMLs) formatters/docbook2tex.xsl
	formatters/docbook2tex.sh $< $@

%.tex: %.xml formatters/docbook2tex.xsl
	formatters/docbook2tex.sh $< $@

$(DEST).html: $(XMLs) $(PNGs)
	formatters/docbook2html.sh $< $@

$(IMGDIR)/%.eps:$(FIGDIR)/%.fig
	if [ ! -d $(IMGDIR) ] ; then \
		mkdir $(IMGDIR) ; \
	fi
	fig2dev -L pstex -b1 $? $@

$(IMGDIR)/%.pdf:$(FIGDIR)/%.fig
	if [ ! -d $(IMGDIR) ] ; then \
		mkdir $(IMGDIR) ; \
	fi
	fig2dev -L pdftex -b1 $? $@

$(IMGDIR)/%.png:$(FIGDIR)/%.fig
	if [ ! -d $(IMGDIR) ] ; then \
		mkdir $(IMGDIR) ; \
	fi
	fig2dev -L png -b1 -S4 $? $@

$(IMGDIR)/%.tex:$(FIGDIR)/%.fig
	if [ ! -d $(IMGDIR) ] ; then \
		mkdir $(IMGDIR) ; \
	fi
	fig2dev -L pstex_t -p $(basename $@) $? $@

$(DEST).pdf: $(DEST).tex $(PDFs) $(PSTEXs)
%.pdf: %.tex
	pdflatex $<

clean:
	rm -f *.aux *.log *.toc *.dvi *.out *.idx *.ind *.loe *.ilg *.fot *.fo
	rm -f .kkkk_eeee .kkkk_kkkk
	rm -rf $(IMGDIR)

veryclean: clean
	rm -f manual.tex manual.pdf manual.html
