LEVEL = ../..

include $(LEVEL)/Makefile.config

PROG = povray

## Copy all of the povray sources into the local directory.
POVRAYSources := $(wildcard $(POVRAY_ROOT)/source/*.c)
RAWLIBPNGSources := $(wildcard $(POVRAY_ROOT)/source/libpng/*.c)
NONLIBPNGSources := $(addprefix $(POVRAY_ROOT)/source/libpng/, ansi2knr.c pngtest.c example.c)
LIBPNGSources := $(filter-out $(NONLIBPNGSources), $(RAWLIBPNGSources))

Source := $(notdir $(POVRAYSources)) $(notdir $(LIBPNGSources))

CPPFLAGS += -I$(POVRAY_ROOT)/source/unix -I$(PROJ_SRC_ROOT)/runtime/libpng -DPREFIX=\"$(PROJ_OBJ_DIR)\" -DSYSCONFDIR=\"$(POVRAY_ROOT)\" -IOutput/src -UHAVE_LIBVGA -I$(POVRAY_ROOT)/source/libpng -I$(POVRAY_ROOT)/source
LIBS +=    -lz -lm
LDFLAGS += -lz -lm

RUN_OPTIONS = -I$(POVRAY_ROOT)/scenes/advanced/chess2.pov -L$(POVRAY_ROOT)/include -GA$<.junk -O-
include $(LEVEL)/MultiSource/Makefile.multisrc

$(notdir $(POVRAYSources)) : % : $(POVRAY_ROOT)/source/%
	cp $< $@
$(notdir $(LIBPNGSources)) : % : $(POVRAY_ROOT)/source/libpng/%
	cp $< $@

