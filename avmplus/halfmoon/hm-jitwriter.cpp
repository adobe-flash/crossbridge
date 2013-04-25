/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

/**
 * Notes on hooking up to CodegenLIR.
 *
 * Lots of code in CodegenLIR depends on having FrameState handy,
 * so it can access Value.traits, etc, for stuff.  We could factor
 * that out so most code takes Value arguments, and a new driver_
 * could provide Values that correspond to IR values.
 *
 * Worse: deadvars() and VarTracker assume the layout of the frame
 * is whatever the verifier had.  A complete boiling of the code and
 * IR would destroy any such thing.
 *
 * In debugger mode, the generated code must have an array of tags
 * and vars that the debugger can inspect.  We don't want to do that at
 * all for the high level JIT.  Debugging will only work on the interpreter
 * or on CT code, and we'll use bailout to support it.
 *
 * VarTracker is essential for eliminating a lot of the stack motion,
 * but we'd take care of that in the high level IR.  Same for null
 * pointer check elimination; we'd take care of that too.
 *
 * deadvars() needs an equivalent pass on the high level jit that
 * eliminates SetlocalInstrs that will never be read from.  There are two
 * interesting cases: 1) exceptions, 2) debugger.  For the time being
 * we can ignore both, if we count on bailing out for both.
 *
 * This implies we can't completely get rid of the existing jit; we will
 * want to use it for any method the new jit can't compile.  We might also
 * need it eventually as a baseline jit if we have a 3-stage system.
 *
 * Strategies:
 *
 * 1. Drive the current jit through the CodeWriter interface.  Requires
 * writing a scheduling pass on the IR that generates a linear sequence
 * of codewriter calls, to simulate what verifier does.  This includes
 * stack scheduling - sending push, pop, getlocal, etc to the jit.
 *
 * FrameState usage is all over the place in CodegenLIR.  This could be a
 * major refactoring pain in the neck.  On the other hand, the fact that
 * FS is used everywhere means we need its info everywhere.
 *
 * 2. Factor CodegenLIR into two layers.  The middle layer would not have
 * dependencies on FrameState or AbcOpcode, and could be driven from a
 * walk of our CFG or IG.
 *
 * 3. Write a from-scratch emitter that drives nanojit.  We can factor
 * useful pieces of code out of CodegenLIR instead of copy-pasting them,
 * but the overall structure of the emitter would be completely new.
 *
 */

JitWriter::JitWriter(MethodInfo* method, Toplevel *tl, AbcEnv *abc_env)
: method_(method)
, console_(method->pool()->core->console)
, jit_mgr_(0)
, abc_(0)
, current_block_(0)
, abc_env_(abc_env)
, toplevel_(tl)
, calling_context_(NULL) //matz_inline_experiment
{
}


JitWriter::JitWriter(MethodInfo* method, const Context *calling_context)
: method_(method)
, console_(method->pool()->core->console)
, jit_mgr_(0)
, abc_(0)
, current_block_(0)
, abc_env_(calling_context->abc_env)
, toplevel_(calling_context->toplevel)
, calling_context_(calling_context) {
  assert(calling_context_ && "calling_context cannot be null");
}

JitWriter::~JitWriter() {
  abc_->~AbcGraph();
  if (enable_profiler) profiled_info_->~ProfiledInformation();
  cleanup();
}

void JitWriter::writePrologue(const FrameState* frame, const uint8_t* pc,
                              CodegenDriver*) {
  assert(frame->abc_pc == pc);
  (void) frame;
  (void) pc;
  jit_mgr_ = JitManager::init(method_->pool());

#ifdef AVMPLUS_VERBOSE
  if (enable_welcome)
    console_ << "analyze   " << method_ << "\n";
#endif

  abc_ = new (alloc_) AbcGraph(method_);
  if (enable_profiler) {
      profiled_info_ = new (alloc_) ProfiledInformation();
  }
  startBlock(frame);
}

void JitWriter::writeEpilogue(const FrameState*) {
  if (enable_verbose) {
#ifdef AVMPLUS_VERBOSE
    console_ << "analyze   " << method_ << "\n";
#endif
  }

  Context cxt(method_, method_->pool()->core->console, toplevel_, abc_env_, calling_context_);

  InstrGraph* ir = parseAbc(method_, jit_mgr_->lattice(), jit_mgr_->infos(),
                            jit_mgr_->mgr_alloc(), abc_,
                            toplevel_, abc_env_, profiled_info_,
                            cxt);
  jit_mgr_->set_ir(method_, ir);
  switch (enable_mode) {
    case kModeInterpret:
      jit_mgr_->set_interp(method_, ir);
      break;
    case kModeLirStubs:
    case kModeLir:
      jit_mgr_->set_lir(method_, ir, profiled_info_);
      break;
  }
}

GprMethodProc JitWriter::finish() {
  return jit_mgr_->getImpl(method_);
}

InstrGraph* JitWriter::ir() {
  return jit_mgr_->ir(method_);
}

void JitWriter::analyze(AbcOpcode abcop, const uint8_t* pc,
                        const FrameState* frame) {
  assert(pc == frame->abc_pc);
  (void) frame;

#ifdef AVMPLUS_VERBOSE
  if (enable_verbose) {
    console_ << "analyze " << opcodeInfo[abcop].name << '\n';
  }
#else
  (void) abcop;
#endif

  if (current_block_ && pc > current_block_->start && abc_->haveBlock(pc)) {
#ifdef AVMPLUS_VERBOSE
    if (enable_verbose)
      console_ << "surprise\n";
#endif
    abc_->analyzeEnd(current_block_, pc);
    finishBlock(pc);
    newBlock(pc, frame);
  }
}

void JitWriter::finishBlock(const uint8_t* nextpc) {
  current_block_->end = nextpc;
  current_block_ = 0;
}

void JitWriter::newBlock(const uint8_t* abc_pc, const FrameState* frame) {
  if (current_block_ && current_block_->start == abc_pc) {
    // we just started this block.  ignore.
    return;
  }
  current_block_ = abc_->newAbcBlock(abc_pc);
  current_block_->start_withbase = frame->withBase;
  abc_->analyzeExceptions(current_block_);
}

void JitWriter::startBlock(const FrameState* frame) {
  newBlock(frame->abc_pc, frame);
  MethodSignaturep signature = method_->getMethodSignature();
  const Type** types = new (abc_->alloc0()) const Type*[signature->frame_size()];
  FrameRange<const FrameValue> from = range(&frame->value(0), frame, signature);
  FrameRange<const Type*> t = range(types, frame, signature);
  for (; !from.empty(); from.popFront(), t.popFront())
    t.front() = jit_mgr_->lattice()->makeType(from.front());
  current_block_->start_types = types;
}

static bool needSavedScopes(const uint8_t *pc, AbcOpcode op) {
  switch ((AbcOpcode) *pc) {
    case OP_getlex:
    case OP_findpropstrict:
    case OP_findproperty:
      switch (op) {
        case OP_findpropstrict:
        case OP_findproperty:
        case OP_getscopeobject:
        case OP_getouterscope:
        case OP_finddef:
        case OP_findpropglobal:
        case OP_findpropglobalstrict:
        case OP_getglobalscope:
          return true;
        default:
          break;
      }
    default:
      break;
  }
  return false;
}

void JitWriter::write(const FrameState* frame, const uint8_t* pc,
                      AbcOpcode abcop, Traits*) {
  analyze(abcop, pc, frame);
  if (needSavedScopes(pc, abcop))
    abc_->abc_instrs.put(pc, new (abc_->alloc_) AbcInstr(abcop));
  if (abcop == OP_lookupswitch || isEndOpcode(abcop)) {
    uint32_t imm30 = 0, imm30b = 0;
    int imm8 = 0, imm24 = 0;
    const uint8_t* nextpc = pc;
    AvmCore::readOperands(nextpc, imm30, imm24, imm30b, imm8);
    if (abcop == OP_lookupswitch)
      abc_->analyzeSwitch(current_block_, pc, nextpc, imm24, imm30b + 1);
    finishBlock(nextpc);
  }
}

void JitWriter::writeOp1(const FrameState* frame, const uint8_t *pc,
                         AbcOpcode abcop, uint32_t opd1, Traits*) {
  analyze(abcop, pc, frame);
  if (needSavedScopes(pc, abcop))
    abc_->abc_instrs.put(pc, new (abc_->alloc_) AbcInstr(abcop, opd1));
  if (isBranchOpcode(abcop)) {
    const uint8_t* nextpc = pc + 4;
    int offset = int32_t(opd1);
    if (abcop == OP_jump) {
      abc_->analyzeEnd(current_block_, nextpc + offset);
      finishBlock(nextpc);
    } else {
      abc_->analyzeBranch(current_block_, abcop, nextpc, offset);
      finishBlock(nextpc);
      newBlock(nextpc, frame);
    }
  }
}

void JitWriter::writeOp2(const FrameState* frame, const uint8_t *pc,
                         AbcOpcode abcop, uint32_t, uint32_t, Traits*) {
  analyze(abcop, pc, frame);
}

void JitWriter::writeMethodCall(const FrameState* frame, const uint8_t *pc,
                                AbcOpcode abcop, MethodInfo*, uintptr_t,
                                uint32_t, Traits*) {
  analyze(abcop, pc, frame);
}

void JitWriter::writeNip(const FrameState* frame, const uint8_t *pc,
                         uint32_t count) {
  while (count--) {
    analyze(OP_swap, pc, frame);
    analyze(OP_pop, pc, frame);
  }
}

void JitWriter::writeCheckNull(const FrameState* frame, uint32_t) {
  analyze(OP_convert_o, frame->abc_pc, frame);
}

void JitWriter::writeCoerce(const FrameState* frame, uint32_t, Traits*) {
  analyze(OP_coerce, frame->abc_pc, frame);
}

void JitWriter::writeBlockStart(const FrameState* frame) {
  assert((!current_block_ || current_block_->start <= frame->abc_pc) &&
         "didn't expect blocks out of order");
  const uint8_t* pc = frame->abc_pc;
  if (current_block_ && current_block_->start < pc) {
    abc_->analyzeEnd(current_block_, pc);
    finishBlock(pc);
  }
  startBlock(frame);
  analyze(OP_label, pc, frame);
}

} // namespace avmplus
#endif // VMCFG_HALFMOON
