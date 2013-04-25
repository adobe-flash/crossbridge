# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

STATIC_LIBRARIES += avmplus
avmplus_BUILD_ALL = 1

ifdef USE_GCC_PRECOMPILED_HEADERS
avmplus_PCH_SRC := $(srcdir)/avmplus.h
avmplus_PCH := $(curdir)/avmplus-precompiled.h
endif

avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/AbcEnv.cpp \
  $(curdir)/AbcParser.cpp \
  $(curdir)/ActionBlockConstants.cpp \
  $(curdir)/ArrayClass.cpp \
  $(curdir)/ArrayObject.cpp \
  $(curdir)/atom.cpp \
  $(curdir)/AvmCore.cpp \
  $(curdir)/avm.cpp \
  $(curdir)/avmfeatures.cpp \
  $(curdir)/AvmLog.cpp \
  $(curdir)/avmplusDebugger.cpp \
  $(curdir)/avmplusHashtable.cpp \
  $(curdir)/avmplusList.cpp \
  $(curdir)/BigInteger.cpp \
  $(curdir)/BooleanClass.cpp \
  $(curdir)/BuiltinTraits.cpp \
  $(curdir)/ByteArrayGlue.cpp \
  $(curdir)/CdeclThunk.cpp \
  $(curdir)/ClassClass.cpp \
  $(curdir)/ClassClosure.cpp \
  $(curdir)/CodegenLIR.cpp \
  $(curdir)/Coder.cpp \
  $(curdir)/d2a.cpp \
  $(curdir)/DataIO.cpp \
  $(curdir)/DateClass.cpp \
  $(curdir)/DateObject.cpp \
  $(curdir)/DescribeTypeClass.cpp \
  $(curdir)/Deopt.cpp \
  $(curdir)/Deopt-CL.cpp \
  $(curdir)/Domain.cpp \
  $(curdir)/DomainEnv.cpp \
  $(curdir)/DomainMgr.cpp \
  $(curdir)/E4XNode.cpp \
  $(curdir)/ErrorClass.cpp \
  $(curdir)/ErrorConstants.cpp \
  $(curdir)/Exception.cpp \
  $(curdir)/exec.cpp \
  $(curdir)/exec-jit.cpp \
  $(curdir)/exec-osr.cpp \
  $(curdir)/exec-verifyall.cpp \
  $(curdir)/FloatClass.cpp \
  $(curdir)/Float4Class.cpp \
  $(curdir)/FrameState.cpp \
  $(curdir)/FunctionClass.cpp \
  $(curdir)/IntClass.cpp \
  $(curdir)/Interpreter.cpp \
  $(curdir)/InvokerCompiler.cpp \
  $(curdir)/JSONClass.cpp \
  $(curdir)/LirHelper.cpp \
  $(curdir)/MathClass.cpp \
  $(curdir)/MathUtils.cpp \
  $(curdir)/MethodClosure.cpp \
  $(curdir)/MethodEnv.cpp \
  $(curdir)/MethodInfo.cpp \
  $(curdir)/Multiname.cpp \
  $(curdir)/MultinameHashtable.cpp \
  $(curdir)/Namespace.cpp \
  $(curdir)/NamespaceClass.cpp \
  $(curdir)/NamespaceSet.cpp \
  $(curdir)/NativeFunction.cpp \
  $(curdir)/NumberClass.cpp \
  $(curdir)/ObjectClass.cpp \
  $(curdir)/peephole.cpp \
  $(curdir)/PoolObject.cpp \
  $(curdir)/PrintWriter.cpp \
  $(curdir)/PromiseChannelGlue.cpp \
  $(curdir)/ProxyGlue.cpp \
  $(curdir)/QCache.cpp \
  $(curdir)/RegExpClass.cpp \
  $(curdir)/RegExpObject.cpp \
  $(curdir)/Sampler.cpp \
  $(curdir)/ScopeChain.cpp \
  $(curdir)/ScriptBuffer.cpp \
  $(curdir)/ScriptObject.cpp \
  $(curdir)/StackTrace.cpp \
  $(curdir)/StringBuffer.cpp \
  $(curdir)/StringClass.cpp \
  $(curdir)/StringObject.cpp \
  $(curdir)/Toplevel.cpp \
  $(curdir)/Traits.cpp \
  $(curdir)/TypeDescriber.cpp \
  $(curdir)/UnicodeUtils.cpp \
  $(curdir)/VectorClass.cpp \
  $(curdir)/Verifier.cpp \
  $(curdir)/VTable.cpp \
  $(curdir)/WordcodeEmitter.cpp \
  $(curdir)/WordcodeTranslator.cpp \
  $(curdir)/wopcodes.cpp \
  $(curdir)/XMLClass.cpp \
  $(curdir)/XMLListClass.cpp \
  $(curdir)/XMLListObject.cpp \
  $(curdir)/XMLObject.cpp \
  $(curdir)/XMLParser16.cpp \
  $(curdir)/Date.cpp \
  $(curdir)/AbcData.cpp \
  $(curdir)/AvmPlusScriptableObject.cpp \
  $(curdir)/instr.cpp \
  $(curdir)/DictionaryGlue.cpp \
  $(curdir)/ObjectIO.cpp \
  $(curdir)/AvmSerializer.cpp \
  $(curdir)/ConcurrencyGlue.cpp \
  $(curdir)/Isolate.cpp \
  $(NULL)

#  $(curdir)/avmplus.cpp \

# See manifest.mk in root directory for the dependencies
# on $(topsrcdir)/generated/builtin.h.

# Use of '%' [to force a pattern-rule] instead of '$(curdir)/..' or
# '$(topsrcdir)' [which would then not be a pattern-rule] is crucial
# (ie "deliberate", ie "hack"); see Bug 632086
%/generated/builtin.h %/generated/builtin.cpp %/generated/builtin.abc: $(topsrcdir)/core/*.as
	$(CMD)cd $(topsrcdir)/core; python builtin.py $(avmplus_BUILTINFLAGS)

.PHONY: core-tracers
core-tracers: $(topsrcdir)/generated/builtin.abc $(topsrcdir)/generated/shell_toplevel.abc
ifdef AVMSHELL_TOOL
	$(MSG)true "Generating core-tracers via AVMSHELL_TOOL=$(AVMSHELL_TOOL)"
	$(CMD)cd $(topsrcdir)/core; AVM=$(AVMSHELL_TOOL) python ./builtin-tracers.py
else
	$(MSG)true "Skipping core-tracers generation since AVMSHELL_TOOL unset"
endif

# 1. Use of '$(topsrcdir)/generated' is deliberate; we use absolute
#    paths for code being generated outside build dir.
#
# 2. Use of '$(curdir)/AbcData.$(II_SUFFIX)' is also deliberate:
#    preprocessed file as target must be specified via same path that
#    is used in root manifest.mk.
#
# Further discussion at Bug 632086.
$(curdir)/AbcData.$(II_SUFFIX): $(topsrcdir)/generated/builtin.cpp

# Bug 654996: use target-specific variable to disable -Werror in these cases
ifeq ($(TARGET_OS),darwin)
$(curdir)/Interpreter.$(OBJ_SUFFIX): avmplus_CXXFLAGS += -Wno-error
$(curdir)/ByteArrayGlue.$(OBJ_SUFFIX): avmplus_CXXFLAGS += -Wno-error
endif
ifeq ($(TARGET_OS),android)
$(curdir)/Interpreter.$(OBJ_SUFFIX): avmplus_CXXFLAGS += -Wno-error
$(curdir)/ByteArrayGlue.$(OBJ_SUFFIX): avmplus_CXXFLAGS += -Wno-error
endif
