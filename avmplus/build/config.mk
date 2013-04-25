# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

GARBAGE :=

VPATH = $(topsrcdir)

curdir := .
srcdir := $(topsrcdir)

COMPILE_CPPFLAGS = $(CPPFLAGS) $(APP_CPPFLAGS)
COMPILE_CXXFLAGS = $(CXXFLAGS) $(APP_CXXFLAGS)
COMPILE_CFLAGS = $(CFLAGS) $(APP_CFLAGS)

ifdef ENABLE_DEBUG
COMPILE_CPPFLAGS += $(DEBUG_CPPFLAGS)
COMPILE_CXXFLAGS += $(DEBUG_CXXFLAGS)
COMPILE_CFLAGS += $(DEBUG_CFLAGS)
LDFLAGS += $(DEBUG_LDFLAGS)
else
COMPILE_CPPFLAGS += $(OPT_CPPFLAGS)
COMPILE_CXXFLAGS += $(OPT_CXXFLAGS)
COMPILE_CFLAGS += $(OPT_CFLAGS)
endif

ifdef MACOSX_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET
endif

# Bugzilla 615532: The VERBOSE flag to make controls how much output
# is printed.  By default we set VERBOSE=1 (so that high-level status
# messages are printed by default -- the ones that start with "true").
#
# You can set VERBOSE to 0 to eliminate the status messages (or just
# pass the "-s" flag to make, the standard way to eliminate command
# echo'ing before execution; supporting this trick is the reason for
# our use of 'true' to emit status messages).
#
# You can also set VERBOSE to values greater than 1 to see
# progressively more messages and the command invocations themselves.

VERBOSE ?= 1

ifeq ($(VERBOSE),0)
VERB_GT0=@
VERB_GT1=@
VERB_GT2=@
VERB_GT3=@
ECHO=true
else ifeq ($(VERBOSE),1) # This is the default case
VERB_GT0=
VERB_GT1=@
VERB_GT2=@
VERB_GT3=@
ECHO=echo
else ifeq ($(VERBOSE),2)
VERB_GT0=
VERB_GT1=
VERB_GT2=@
VERB_GT3=@
ECHO=echo
else ifeq ($(VERBOSE),3)
VERB_GT0=
VERB_GT1=
VERB_GT2=
VERB_GT3=@
ECHO=echo
else ifeq ($(VERBOSE),4)
VERB_GT0=
VERB_GT1=
VERB_GT2=
VERB_GT3=
ECHO=echo
else
$(error "VERBOSE $(VERBOSE) must be set to integer between 0 and 4")
endif

MSG=$(VERB_GT0)       # 0: High-level status messages
MXG=$(VERB_GT1)       # 1: A few extra status messages
CMD=$(VERB_GT2)       # 2: Main build commands: CC, CXX, link, etc
NIT=$(VERB_GT3)       # 3: Nitty gritty

ifeq ($(VERBOSE),0)
CALCDEPS_VERBOSE_OPT=-q
else ifeq ($(VERBOSE),1)
CALCDEPS_VERBOSE_OPT=
else ifeq ($(VERBOSE),2)
CALCDEPS_VERBOSE_OPT=
else ifeq ($(VERBOSE),3)
CALCDEPS_VERBOSE_OPT=
else ifeq ($(VERBOSE),4)
CALCDEPS_VERBOSE_OPT=-v
endif

ifeq ($(VERB_GT3),) # Bugzilla 750915: keep test-run's stderr for VERBOSE >= 4
CXX_PCH_PIPE_STDERR=
else # at VERBOSE < 4: pipe stderr for test run to /dev/null
CXX_PCH_PIPE_STDERR=  2> /dev/null
endif

PCH_SUFFIX ?= gch

GLOBAL_DEPS := Makefile

all::

# Usage: from within RECURSE_DIRS
# Variables:
#   $(1) = $(curdir)
#   $(2) = subdirectory
# Note: positional parameters passed to $(call) are expanded "early". This is
# the magic that sets and revert $(curdir) and $(srcdir) properly.
define RECURSE_DIR
# Bug 632086: Generate paths without "./"; closer to canonical form make needs.
curdir := $(subst ./,,$(1)/$(2))
srcdir := $(topsrcdir)/$$(curdir)
include $(topsrcdir)/$$(curdir)/manifest.mk
curdir := $(1)
endef

# Usage: $(call REAL_RECURSE_DIRS,$(DIRS),$(curdir))
define REAL_RECURSE_DIRS
$(foreach dir,$(1),$(eval $(call RECURSE_DIR,$(2),$(dir))))
endef

# Usage: $(call RECURSE_DIRS,$(DIRS)))
define RECURSE_DIRS
$(call REAL_RECURSE_DIRS,$(1),$(curdir))
endef

# Usage: $(eval $(call THING_SRCS,$(thingname)))
define THING_SRCS
$(1)_CPPFLAGS ?= $(COMPILE_CPPFLAGS)
$(1)_CXXFLAGS ?= $(COMPILE_CXXFLAGS)
$(1)_BUILTINFLAGS = $(BUILTIN_BUILDFLAGS)
$(1)_CFLAGS ?= $(COMPILE_CFLAGS)
$(1)_CPPFLAGS += $($(1)_EXTRA_CPPFLAGS)
$(1)_CXXFLAGS += $($(1)_EXTRA_CXXFLAGS)
$(1)_CFLAGS += $($(1)_EXTRA_CFLAGS)
$(1)_INCLUDES += $(INCLUDES)
$(1)_DEFINES += $(DEFINES)

$(1)_CXXOBJS = $$($(1)_CXXSRCS:%.cpp=%.$(OBJ_SUFFIX))

$(1)_COBJS = $$($(1)_CSRCS:%.c=%.$(OBJ_SUFFIX))

$(1)_ASMOBJS = $$($(1)_ASMSRCS:%.armasm=%.$(OBJ_SUFFIX))

$(1)_MASMOBJS = $$($(1)_MASMSRCS:%.asm=%.$(OBJ_SUFFIX))

ifdef $(1)_PCH
$(1)_PCH_OPTION = -include $$($(1)_PCH)
$(1)_PCH_OBJ = $$($(1)_PCH:%.h=%.h.$(PCH_SUFFIX))
$(1)_PCH_CHECK = $$($(1)_PCH:%.h=%.$(OBJ_SUFFIX))
$(1)_PCH_II = $$($(1)_PCH:%.h=%.$(II_SUFFIX))
$(1)_PCH_DEPS = $$($(1)_PCH:%.h=%.deps)
ifndef $(1)_PCH_SRC
error "must define $(1)_PCH_SRC) alongside $(1)_PCH=$$($(1)_PCH)"
endif
else
$(1)_PCH_OPTION =
$(1)_PCH_OBJ =
$(1)_PCH_CHECK =
$(1)_PCH_II =
$(1)_PCH_DEPS =
endif

GARBAGE += \
  $$($(1)_CXXOBJS) \
  $$($(1)_COBJS) \
  $$($(1)_ASMOBJS) \
  $$($(1)_MASMOBJS) \
  $$($(1)_PCH_OBJ) \
  $$($(1)_PCH:.h=.$(II_SUFFIX)) \
  $$($(1)_PCH:.h=.deps) \
  $$($(1)_CXXOBJS:.$(OBJ_SUFFIX)=.$(II_SUFFIX)) \
  $$($(1)_CXXOBJS:.$(OBJ_SUFFIX)=.deps) \
  $$($(1)_COBJS:.$(OBJ_SUFFIX)=.$(I_SUFFIX)) \
  $$($(1)_COBJS:.$(OBJ_SUFFIX)=.deps) \
  $(NULL)

ifdef $(1)_PCH
$$($(1)_PCH:.h=.$(II_SUFFIX)): $$($(1)_PCH_SRC)
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Preprocessing $$< to $$@"
	$(CMD)$(CXX) -E $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) $$< > $$@
	$(MXG)true "Extracting deps from $$@"
	$(CMD)$(PYTHON) $(topsrcdir)/build/dependparser.py $$($(1)_PCH_DEPS) < $$@

$$($(1)_PCH).$(PCH_SUFFIX): $$($(1)_PCH_SRC) $$($(1)_PCH:.h=.$(II_SUFFIX))
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Precompiling $$< to $$@"
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(CMD)$(CXX) $(OUTOPTION)$$@ $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) -c $$<

$$($(1)_PCH_CHECK): $$($(1)_PCH_OBJ) FORCE
	$(MXG)true "Checking validity of precompiled header $$<"
	$(CMD)$(CXX) $(OUTOPTION)$$@ $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_PCH_OPTION) -x c++ -c - < /dev/null $(CXX_PCH_PIPE_STDERR) || $(ECHO) "PCH invalid; regenerating precompiled header $$<" && $(CXX) $(OUTOPTION)$$< $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) -c $$($(1)_PCH_SRC)
endif # defined(thingname_PCH)

$$($(1)_CXXOBJS:.$(OBJ_SUFFIX)=.$(II_SUFFIX)): %.$(II_SUFFIX): %.cpp $$(GLOBAL_DEPS)
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Preprocessing $$*"
	$(CMD)$(CXX) -E $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) $$< > $$@
	$(MXG)true "Extracting deps from $$*"
	$(CMD)$(PYTHON) $(topsrcdir)/build/dependparser.py $$*.deps < $$@

$$($(1)_CXXOBJS): %.$(OBJ_SUFFIX): %.cpp %.$(II_SUFFIX) $$(GLOBAL_DEPS) | $$($(1)_PCH_CHECK) 
	$(MSG)true "Compiling $$*"
	$(CMD)$(CXX) $(OUTOPTION)$$@ $$($(1)_CPPFLAGS) $$($(1)_CXXFLAGS) $$($(1)_DEFINES) $$($(1)_PCH_OPTION) $$($(1)_INCLUDES) -c $$<

$$($(1)_COBJS:.$(OBJ_SUFFIX)=.$(I_SUFFIX)): %.$(I_SUFFIX): %.c $$(GLOBAL_DEPS)
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Preprocessing $$*"
	$(CMD)$(CC) -E $$($(1)_CPPFLAGS) $$($(1)_CFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) $$< > $$@
	$(CMD)$(PYTHON) $(topsrcdir)/build/dependparser.py $$*.deps < $$@

$$($(1)_COBJS): %.$(OBJ_SUFFIX): %.c %.$(I_SUFFIX) $$(GLOBAL_DEPS)
	$(MSG)true "Compiling $$*"
	$(CMD)$(CC) $(OUTOPTION)$$@ $$($(1)_CPPFLAGS) $$($(1)_CFLAGS) $$($(1)_DEFINES) $$($(1)_INCLUDES) -c $$<

$$($(1)_ASMOBJS): %.$(OBJ_SUFFIX): %.armasm $$(GLOBAL_DEPS)
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Assembling $$*"
	$(CMD)$(ASM) -o $$@ $$($(1)_ASMFLAGS) $$<

$$($(1)_MASMOBJS): %.$(OBJ_SUFFIX): %.asm $$(GLOBAL_DEPS)
	$(NIT)test -d $$(dir $$@) || mkdir -p $$(dir $$@)
	$(MSG)true "Assembling $$*"
	$(CMD)$(MASM) -Fo $$@ $$($(1)_MASMFLAGS) $$<

$(1).thing.pp: FORCE
	$(MXG)true "Calculate deps from $$@"
	$(CMD)$(PYTHON) $(topsrcdir)/build/calcdepends.py $(CALCDEPS_VERBOSE_OPT) $$@ $$($(1)_CXXOBJS:.$(OBJ_SUFFIX)=.$(II_SUFFIX)) $$($(1)_COBJS:.$(OBJ_SUFFIX)=.$(I_SUFFIX)) $$($(1)_PCH:.h=.$(II_SUFFIX))

include $(1).thing.pp

endef # THINGS_SRCS

# Usage: $(eval $(call STATIC_LIBRARY_RULES,$(static_library_thingname)))
define STATIC_LIBRARY_RULES
  $(1)_BASENAME ?= $(1)
  $(1)_NAME = $(LIB_PREFIX)$$($(1)_BASENAME).$(LIB_SUFFIX)

$$($(1)_DIR)$$($(1)_NAME): $$($(1)_CXXOBJS) $$($(1)_COBJS) $$($(1)_ASMOBJS) $$($(1)_MASMOBJS)
	$(MSG)true "Library $$*"
	$(CMD)$(call MKSTATICLIB,$$@) $$($(1)_CXXOBJS) $$($(1)_COBJS) $$($(1)_ASMOBJS) $$($(1)_MASMOBJS)

GARBAGE += $$($(1)_DIR)$$($(1)_NAME)

ifdef $(1)_BUILD_ALL
all:: $$($(1)_DIR)$$($(1)_NAME)
endif

endef

# Usage: $(eval $(call DLL_RULES,$(dll_thingname)))
define DLL_RULES
  $(1)_BASENAME ?= $(1)
  $(1)_NAME = $(LIB_PREFIX)$$($(1)_BASENAME).$(DLL_SUFFIX)
  $(1)_DEPS = \
    $$($(1)_EXTRA_DEPS) \
    $$(foreach lib,$$($(1)_STATIC_LIBRARIES) $$($(1)_DLLS),$$($$(lib)_NAME)) \
    $$(GLOBAL_DEPS) \
    $(NULL)
  $(1)_LDFLAGS = \
    $$(LDFLAGS) \
    $$($(1)_EXTRA_LDFLAGS) \
    $$(foreach lib,$$(OS_LIBS),$(call EXPAND_LIBNAME,$$(lib))) \
    $$(OS_LDFLAGS) \
    $(NULL)

$$($(1)_DIR)$$($(1)_NAME): $$($(1)_CXXOBJS) $$($(1)_COBJS) $$($(1)_DEPS)
	$(MXG)true "Make dynamic-linked shared library $$@"
	$(CMD)$(call MKDLL,$$@) $$($(1)_CXXOBJS) $$($(1)_COBJS) \
	  $(LIBPATH). $$(foreach lib,$$($(1)_STATIC_LIBRARIES),$$(call EXPAND_LIBNAME,$$(lib))) \
	  $$(foreach lib,$$($(1)_DLLS),$$(call EXPAND_DLLNAME,$$(lib))) \
	  $$($(1)_LDFLAGS)

GARBAGE += $$($(1)_DIR)$$($(1)_NAME)

ifdef $(1)_BUILD_ALL
all:: $$($(1)_DIR)$$($(1)_NAME)
endif

endef

# Usage: $(eval $(call PROGRAM_RULES,$(program_thingnaame)))
define PROGRAM_RULES
  $(1)_BASENAME ?= $(1)
  $(1)_NAME ?= $$($(1)_BASENAME)$(PROGRAM_SUFFIX)
  $(1)_DEPS = \
    $$($(1)_EXTRA_DEPS) \
    $$(foreach lib,$$($(1)_STATIC_LIBRARIES) $$($(1)_DLLS),$$($$(lib)_NAME)) \
    $$(GLOBAL_DEPS) \
    $(NULL)
  $(1)_LDFLAGS = \
    $$(LDFLAGS) \
    $$($(1)_EXTRA_LDFLAGS) \
    $$(foreach lib,$$(OS_LIBS),$(call EXPAND_LIBNAME,$$(lib))) \
    $$(OS_LDFLAGS) \
    $(NULL)

$$($(1)_DIR)$$($(1)_NAME): $$($(1)_CXXOBJS) $$($(1)_DEPS)
	$(MSG)true "Link $$@"
	$(CMD)$(call MKPROGRAM,$$@) \
	  $$($(1)_CXXOBJS) \
	  $(LIBPATH). $$(foreach lib,$$($(1)_STATIC_LIBRARIES),$$(call EXPAND_LIBNAME,$$(lib))) \
	  $$(foreach lib,$$($(1)_DLLS),$$(call EXPAND_DLLNAME,$$(lib))) \
	  $$($(1)_LDFLAGS)
	$(ifneq ($POSTMKPROGRAM,''))
		$(CMD)$(call POSTMKPROGRAM,$$@)

GARBAGE += $$($(1)_DIR)$$($(1)_NAME)

ifdef $(1)_BUILD_ALL
all:: $$($(1)_DIR)$$($(1)_NAME)
endif

endef

# Delete target files if their recipe had an error exit-status code.
.DELETE_ON_ERROR:

.PHONY: all FORCE
