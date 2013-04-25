/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"

#ifdef VMCFG_HALFMOON

#include "profiler/profiler-main.h"

namespace halfmoon {
using avmplus::MethodInfo;
using avmplus::MathUtils;
using avmplus::DomainMgr;
using avmplus::TraitsBindings;

using profiler::MethodProfile;
using profiler::ProfiledState;
using profiler::RecordedType;

// OP_end means we fell off the end of a label.
static const AbcOpcode OP_end = AbcOpcode(-1);

  AbcBuilder::AbcBuilder(MethodInfo* method, AbcGraph* abc, InstrFactory* factory, Toplevel* toplevel, ProfiledInformation* profiled_information,
                         bool has_reachable_exceptions)
: alloc_(factory->alloc())
, alloc0_(factory->alloc())
, method_(method)
, sig_(method->getMethodSignature())
, abc_(abc)
, pool_(method->pool())
, console_(pool_->core->console)
, lattice_(factory->lattice())
, scope_base_(sig_->local_count())
, stack_base_(scope_base_ + sig_->max_scope())
, framesize_(stack_base_ + sig_->max_stack())
, num_vars_(framesize_ + 2 + (has_reachable_exceptions ? framesize_ : 0))
, effect_pos_(framesize_)
, state_pos_(framesize_ + 1)
, setlocal_pos_(has_reachable_exceptions ? (framesize_ + 2) : 0)
, frame_(new (alloc0_) Def*[num_vars_])
, code_pos_(sig_->abc_code_start())
, pc_(NULL)
, has_reachable_exceptions_(has_reachable_exceptions)
, return_label_(0)
, throw_label_(0)
, factory_(*factory)
, ir_(factory->createGraph())
, builder_(ir_, factory)
, profiled_info_(profiled_information)
, toplevel_(toplevel)
{
  // Initialize useful entries in kind_map_
  for (int i = 0; i < 256; i++)
    kind_map_[i] = InstrKind(-1);
  kind_map_[OP_li8] = HR_abc_li8;
  kind_map_[OP_li16] = HR_abc_li16;
  kind_map_[OP_li32] = HR_abc_li32;
  kind_map_[OP_lf32] = HR_abc_lf32;
  kind_map_[OP_lf64] = HR_abc_lf64;
  kind_map_[OP_convert_s] = HR_abc_convert_s;
  kind_map_[OP_esc_xelem] = HR_abc_esc_xelem;
  kind_map_[OP_esc_xattr] = HR_abc_esc_xattr;
  kind_map_[OP_typeof] = HR_abc_typeof;
  kind_map_[OP_checkfilter] = HR_abc_checkfilter;
  kind_map_[OP_add] = HR_abc_add;
  kind_map_[OP_nextname] = HR_abc_nextname;
  kind_map_[OP_nextvalue] = HR_abc_nextvalue;
  kind_map_[OP_lessthan] = HR_abc_lessthan;
  kind_map_[OP_lessequals] = HR_abc_lessequals;
  kind_map_[OP_greaterthan] = HR_abc_greaterthan;
  kind_map_[OP_greaterequals] = HR_abc_greaterequals;
  kind_map_[OP_equals] = HR_abc_equals;
  kind_map_[OP_instanceof] = HR_abc_instanceof;
  kind_map_[OP_istypelate] = HR_abc_istypelate;
  kind_map_[OP_astypelate] = HR_abc_astypelate;
  kind_map_[OP_in] = HR_abc_in;
  kind_map_[OP_si8] = HR_abc_si8;
  kind_map_[OP_si16] = HR_abc_si16;
  kind_map_[OP_si32] = HR_abc_si32;
  kind_map_[OP_sf32] = HR_abc_sf32;
  kind_map_[OP_sf64] = HR_abc_sf64;
  kind_map_[OP_divide] = HR_abc_divide;
  kind_map_[OP_modulo] = HR_abc_modulo;
  kind_map_[OP_subtract] = HR_abc_subtract;
  kind_map_[OP_multiply] = HR_abc_multiply;
  kind_map_[OP_add_i] = HR_abc_add_i;
  kind_map_[OP_subtract_i] = HR_abc_subtract_i;
  kind_map_[OP_multiply_i] = HR_abc_multiply_i;
  kind_map_[OP_bitor] = HR_abc_bitor;
  kind_map_[OP_bitand] = HR_abc_bitand;
  kind_map_[OP_bitxor] = HR_abc_bitxor;
  kind_map_[OP_lshift] = HR_abc_lshift;
  kind_map_[OP_rshift] = HR_abc_rshift;
  kind_map_[OP_urshift] = HR_abc_urshift;
  kind_map_[OP_bitnot] = HR_abc_bitnot;
  kind_map_[OP_negate_i] = HR_abc_negate_i; // NOTE: was HR_negate_i
  kind_map_[OP_sxi1] = HR_abc_sxi1;
  kind_map_[OP_sxi8] = HR_abc_sxi8;
  kind_map_[OP_sxi16] = HR_abc_sxi16;
  kind_map_[OP_negate] = HR_abc_negate;
  kind_map_[OP_not] = HR_abc_not;
  kind_map_[OP_findpropstrict] = HR_abc_findpropstrict;
  kind_map_[OP_findproperty] = HR_abc_findproperty;
  kind_map_[OP_increment] = HR_abc_increment;
  kind_map_[OP_decrement] = HR_abc_decrement;
  kind_map_[OP_inclocal] = HR_abc_increment;
  kind_map_[OP_declocal] = HR_abc_decrement;
  kind_map_[OP_increment_i] = HR_abc_increment_i;
  kind_map_[OP_decrement_i] = HR_abc_decrement_i;
  kind_map_[OP_inclocal_i] = HR_abc_increment_i;
  kind_map_[OP_declocal_i] = HR_abc_decrement_i;
  kind_map_[OP_dxnslate] = HR_abc_dxnslate;
}

AbcBuilder::~AbcBuilder() {
}

InstrGraph* AbcBuilder::visitBlocks(Seq<AbcBlock*> *list) {
  // Build the instr graph by visiting ABC blocks in reverse postorder.
  buildStart();

  for (SeqRange<AbcBlock*> b(list); !b.empty(); b.popFront()) {
    AbcBlock* abc_block = b.front();

    // Exception blocks shouldn't be normally reachable
    AvmAssert(abc_block->label == NULL || !pcIsHandler(abc_block->start));

    // Lazily fill in the exception handler blocks before parsing them
    if (abc_block->label == NULL && pcIsHandler(abc_block->start)) {
      AvmAssert(has_reachable_exceptions_);
      ensureCatchBlockLabel(abc_block);
    }
    visitBlock(abc_block);
  }

  // In the current world of setjmp/longjmp style exception handling
  // it's sufficient to link all the exception blocks off the start
  // block.  Later if we have more precise handling of exception
  // blocks it might be beneficial to directly track which blocks
  // might have exception handlers.  So for now this code is disabled
#if 0
  for (SeqRange<AbcBlock*> b(list); !b.empty(); b.popFront()) {
    AbcBlock* abc_block = b.front();
    if (abc_block->label == NULL)
      continue;
    for (int c = 0; c < abc_block->max_catch_blocks; c++) {
      if (abc_block->catch_blocks[c] != NULL) {
        BlockEndInstr* end = InstrGraph::blockEnd(abc_block->label);
        if (end == NULL) continue;
        CatchBlockInstr* handler = (CatchBlockInstr*)abc_block->catch_blocks[c]->label;
        linkExceptionEdge(abc_block->label, handler);
      }
    }
  }  
#endif

  finish();

  if (enable_printir)
    listCfg(console_, ir_);

  assert(checkPruned(ir_) && checkSSA(ir_));
  Context cxt(method_);
  propagateTypes(ir_);
  removeDeadCode(&cxt, ir_);
  return ir_;
}

void AbcBuilder::visitBlock(AbcBlock* abc_block) {
  if (!abc_block->label)
    return;
  startBlock(abc_block);

  bool in_try = false;
  for (int c = 0; c < abc_block->max_catch_blocks; c++) {
    if (abc_block->catch_blocks[c] != NULL) {
      in_try = true;
      break;
    }
  }

  if (!in_try && has_reachable_exceptions_) {
    safepointStmt(abc_block->start);
  }

  const uint8_t* end = abc_block->end;
  for (const uint8_t* pc = abc_block->start;;) {
    uint32_t imm30 = 0, imm30b = 0;
    int imm8 = 0, imm24 = 0;
    const uint8_t* nextpc = pc;
    AbcOpcode abcop = readInstr(nextpc, end, imm30, imm30b, imm24, imm8);
    if (abcop == OP_end) {
      jumpStmt(); // Add the fall-through edge.
      break;
    }
    // TODO: could we restrict to just this:
    //       (abc_block->in_try && opcodeInfo[abcop].canThrow)
    // if (needAbcState())
    if (in_try && opcodeInfo[abcop].canThrow)
      safepointStmt(pc);
    // TODO: This is preposterous without a filter for catchable
    // canThrow instructions.  We must also safepoint targets of
    // backward branches.  Avoid safepointing calls.
    //emitThrowSafepoint(pc);
    visitInstr(pc, abcop, imm30, imm30b, imm8);
    assert(checkFrame(frame_, stackp_, scopep_));
    if (isBlockEnd(abcop))
      break;
    pc_ = pc = nextpc;
  }
  pc_ = NULL;
}

bool AbcBuilder::shouldSpeculate() {
  bool speculate = enable_profiler > 0;
  profiler::PROFILING_STATE state =
    JitManager::getProfile(method_)->current_profiler_state_;

  // We may deopt, so we should stop speculating
  speculate &= (state == profiler::GATHERED);
  if (enable_profiler) {
    assert (state != profiler::NONE);
    assert (state != profiler::PROFILING);
  }
  return speculate;
}

/// If the verifier has a type for us, give us def with that type
/// Otherwise if we have profiler information, speculatively coerce to that type
Def* AbcBuilder::getTypedDef(const uint8_t* pc, Def* current_def,
                             int input_index, int input_count,
                             int output_count) {
  const Type* current_type = type(current_def);
  if (shouldSpeculate() && isAny(current_type)) {
    RecordedType profiled_param_type = getRecordedType(pc, input_index,
                                                       input_count,
                                                       output_count);
    return toSpeculativeType(current_def, profiled_param_type);
  } else {
    return current_def;
  }
}

void AbcBuilder::speculateParameters(StartInstr* start) {
  assert (shouldSpeculate());
  saveState(code_pos_, 0, 0);

  int local_id = 0;
  for (int n = start->data_param_count() - 1; local_id < n; ++local_id) {
    Def* local_def = start->data_param(local_id + 1);
    int input_count = start->data_param_count() - 1;
    int output_count = 0;

    local_def = getTypedDef(code_pos_, local_def, local_id, input_count,
                            output_count);
    setLocal(local_id, local_def);
  }
}

void AbcBuilder::InitVisitor::defaultVal(Atom val, uint32_t slot,
                                         Traits* slot_traits) {
  Def* default_val;
  Lattice* lattice = &abc->lattice_;
  InstrGraphBuilder* builder = &abc->builder_;
  switch (builtinType(slot_traits)) {
    case BUILTIN_int: {
      const Type* c = lattice->makeIntConst(AvmCore::integer_i(val));
      default_val = builder->addConst(c);
      break;
    }
    case BUILTIN_uint: {
      const Type* c = lattice->makeUIntConst(AvmCore::integer_u(val));
      default_val = builder->addConst(c);
      break;
    }
    default: {
      // TODO need Lattice::makeAtomConst(Atom)
      //
      if (AvmCore::isUndefined(val)) {
        default_val = builder->addConst(lattice->makeAtomConst(NULL, val));
      } else if (AvmCore::isNull(val)) {
        // NOTE: for now just pass it through, type system swaps traits
        default_val = builder->addConst(lattice->makeAtomConst(slot_traits,
                                                               val));
      } else {
        Traits* val_traits = abc->toplevel_->toTraits(val);
        const Type* val_type = lattice->makeType(val_traits);
        const Type* slot_type = lattice->makeType(slot_traits);
        if (!subtypeof(val_type, slot_type))
          assert(false && "default value type is not a subtype of slot type");
        default_val = builder->addConst(lattice->makeAtomConst(val_traits,
                                                               val));
      }
      break;
    }
  }

  Def* args[] = { default_val };
  CallStmt2* setslot = builder->factory().newCallStmt2(HR_setslot,
                                                       abc->effect(),
                                                       abc->ordinalConst(slot),
                                                       object, 1, args);
  builder->addInstr(setslot);
  abc->set_effect(setslot->effect_out());
}

void AbcBuilder::emitInitializers(Def* object) {
  InitVisitor visitor(this, object);
  Traits* traits = method_->declaringTraits();
  const TraitsBindings *tb = traits->getTraitsBindings();
  traits->visitInitBody(&visitor, toplevel_, tb);
}

/// build start instr from given begin instr
///
void AbcBuilder::buildStart() {
  StartInstr* start = factory_.newStartInstr(method_);
  builder_.addInstr(start);
  ir_->begin = start;
  set_effect(start->effect_out());

  ConstantExpr* never = factory_.newConstantExpr(HR_never, BOT);
  builder_.addInstr(never);
  never_def_ = never->value();

  ConstantExpr* newstate = factory_.newConstantExpr(HR_newstate, STATE);
  builder_.addInstr(newstate);

  if (method_->isConstructor())
    emitInitializers(start->data_param(1));

  set_state(newstate->value());
  scopep_ = scope_base_ - 1;
  stackp_ = stack_base_ - 1;
  withbase_ = -1;

  int local_id = 0;
  for (int n = start->data_param_count() - 1; local_id < n; ++local_id) {
    Def* local_def = start->data_param(local_id + 1);
    setLocal(local_id, local_def);
  }

  for (; local_id < scope_base_; ++local_id) {
    setLocal(local_id, createConst(lattice_.void_type));
  }

  if (shouldSpeculate()) {
    speculateParameters(start);
  }

  addGoto(abc_->entry());
}

void AbcBuilder::finish() {
  if (return_label_)
    ir_->end = cast<StopInstr>(ir_->blockEnd(return_label_));
  if (throw_label_)
    ir_->exit = cast<StopInstr>(ir_->blockEnd(throw_label_));
}

void AbcBuilder::safepointStmt(const uint8_t* pc) {
  // assert (needAbcState());
  Def** vars = new (alloc0_) Def*[num_vars_];
  int argc = 0;

  for (int i = 0, n = scope_base_; i < n; i++) {
    vars[argc++] = frame_[setlocal_pos_ + i];
  }

  // scopep_ starts at scope_base - 1
  for (int i = scope_base_; i <= scopep_; i++) {
    vars[argc++] = frame_[setlocal_pos_ + i];
  }

  // stackp_ starts at stack_base - 1
  for (int i = stack_base_; i <= stackp_; i++) {
    vars[argc++] = frame_[setlocal_pos_ + i];
  }
  assert(argc < num_vars_);

  Def* effect = this->effect();
  SafepointInstr* instr = factory_.newSafepointInstr(effect, argc, vars);
  instr->vpc = int(pc - code_pos_);
  instr->sp = stackp_;
  instr->scopep = scopep_;

  set_state(instr->state_out());
  builder_.addInstr(instr);
  set_effect(instr->effect_out());
}

/// DEOPT
/// In the future, we will want to filter the frame slots
/// with liveness information.  At present, we assume that
/// all locals and active scopes and operands are live.
DeoptSafepointInstr* AbcBuilder::emitBailoutSafepoint(const uint8_t* pc) {
  if (!enable_deopt) return NULL;  // DEOPT FIXME
  // TODO: We could re-use a fixed-sized buffer,
  // allocated once per compilation.
  Def** vars = new (alloc0_) Def*[framesize_];
  int argc = 0;

  for (int i = 0, n = sig_->local_count(); i < n; i++) {
    vars[argc++] = getLocal(i);
  }

  // scopep_ starts at scope_base - 1
  for (int i = scope_base_; i <= scopep_; i++) {
    vars[argc++] = getLocal(i);
  }

  // stackp_ starts at stack_base - 1
  for (int i = stack_base_; i <= stackp_; i++) {
    vars[argc++] = getLocal(i);
  }

  Def* effect = this->effect();
  DeoptSafepointInstr* instr =
    factory_.newDeoptSafepointInstr(effect, argc, vars);
  builder_.addInstr(instr);
  set_effect(instr->effect_out());
  instr->kind = kBailoutSafepoint;
  instr->vpc = uint32_t(pc - code_pos_);
  instr->scopep = scopep_;
  instr->stackp = stackp_;
  instr->vlen = 0;
  instr->nargs = 0;
  instr->rtype = SST_MAX_VALUE; // FIXME: bogus placeholder
  instr->minfo = NULL;

  return instr;
}

/// DEOPT
DeoptSafepointInstr* AbcBuilder::emitThrowSafepoint(const uint8_t* pc) {
  if (!enable_deopt) return NULL;  // DEOPT FIXME
  // TODO: We could re-use a fixed-sized buffer,
  // allocated once per compilation.
  Def** vars = new (alloc0_) Def*[framesize_];
  int argc = 0;

  for (int i = 0, n = sig_->local_count(); i < n; i++) {
    vars[argc++] = getLocal(i);
  }

  // we do not need to preserve scopes or operands

  Def* effect = this->effect();
  DeoptSafepointInstr* instr =
    factory_.newDeoptSafepointInstr(effect, argc, vars);
  builder_.addInstr(instr);
  set_effect(instr->effect_out());
  instr->kind = kThrowSafepoint;
  instr->vpc = uint32_t(pc - code_pos_);
  instr->scopep = scopep_;
  instr->stackp = stackp_;
  instr->vlen = 0;
  instr->nargs = 0;
  instr->rtype = SST_MAX_VALUE; // FIXME: bogus placeholder
  instr->minfo = NULL;

  return instr;
}

/// DEOPT
void AbcBuilder::emitFinish(DeoptSafepointInstr* sfp) {
  if (!enable_deopt) return;
  Def* effect = this->effect();
  DeoptFinishInstr* instr = factory_.newDeoptFinishInstr(effect);
  builder_.addInstr(instr);
  set_effect(instr->effect_out());
  instr->safepoint = sfp;
}

/// DEOPT
DeoptSafepointInstr* AbcBuilder::emitCallSafepoint(const uint8_t* pc, int nargs) {
  if (!enable_deopt) return NULL;  // DEOPT FIXME
  // TODO: We could re-use a fixed-sized buffer,
  // allocated once per compilation.
  Def** vars = new (alloc0_) Def*[framesize_];
  int argc = 0;

  for (int i = 0, n = sig_->local_count(); i < n; i++) {
    vars[argc++] = getLocal(i);
  }

  for (int i = scope_base_; i <= scopep_; i++) {
    vars[argc++] = getLocal(i);
  }

  // TODO: We do not need to preserve the function arguments
  // unless a bailout may occur at this call site.  It is not
  // clear that we will ever use bailouts in this way.
  for (int i = stack_base_; i <= stackp_ /*- nargs*/; i++) {
    vars[argc++] = getLocal(i);
  }

  Def* effect = this->effect();
  DeoptSafepointInstr* instr =
    factory_.newDeoptSafepointInstr(effect, argc, vars);
  builder_.addInstr(instr);
  set_effect(instr->effect_out());
  instr->kind = kCallSafepoint;
  instr->vpc = uint32_t(pc - code_pos_);
  instr->scopep = scopep_;
  instr->stackp = stackp_;
  instr->vlen = 0;
  instr->nargs = nargs;
  instr->rtype = SST_MAX_VALUE; // FIXME: bogus placeholder
  instr->minfo = NULL;

  return instr;
}

/// DEOPT
void AbcBuilder::emitFinishCall(DeoptSafepointInstr* sfp, Def* arg) {
  if (!enable_deopt) return;
  if (!arg) {
    // Void function call.
    sfp->rtype = SST_MAX_VALUE; // hack: need SST_void
    emitFinish(sfp);
  } else {
    Def* effect = this->effect();
    DeoptFinishCallInstr* instr = factory_.newDeoptFinishCallInstr(effect, arg);
    builder_.addInstr(instr);
    set_effect(instr->effect_out());
    instr->safepoint = sfp;
  }
}

void AbcBuilder::set_effect(Def* effect) {
  // Def* new_effect = peephole(effect);
  // assert(effect == new_effect);
  assert(kind(type(effect)) == kTypeEffect);
  frame_[effect_pos_] = effect;
}

void AbcBuilder::set_state(Def* state) {
  frame_[state_pos_] = state;
}

AbcOpcode peek(const uint8_t *pc, const uint8_t* end) {
  return pc != end ? AbcOpcode(*pc) : OP_end;
}

/**
 * Read one instruction, including peephole analysis.  Any simplification
 * done in this stage reduces work for later stages.  A machine generated
 * automaton from utils/peephole.as could be adapted.
 */
AbcOpcode AbcBuilder::readInstr(const uint8_t* &pc, const uint8_t* end,
                                uint32_t &imm30, uint32_t& imm30b, int &imm24,
                                int &imm8) {
  again: if (pc == end)
    return OP_end; // reached end w/ no opcode
  printAbcInstr(pc);
  AbcOpcode abcop = AbcOpcode(*pc);
  AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
  if (pc == end || !enable_peephole)
    return abcop;
  switch (abcop) {
    case OP_coerce_a:
    case OP_nop:
    case OP_label:
      goto again;
  }
  switch (abcop) {
    case OP_pushundefined:
      abcop = peek(pc, end);
      if (abcop == OP_pop) {
        ++pc;
        goto again;
      }
      if (abcop == OP_coerce_a)
        ++pc;
      if (abcop == OP_setlocal) {
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        return OP_kill;
      }
      return OP_pushundefined;
    case OP_lessthan:
      if (peek(pc, end) == OP_iffalse) {
        printAbcInstr(pc);
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        //console_ << "lessthan+iffalse =>> ifnlt\n";
        return OP_ifnlt;
      }
      return abcop;
    case OP_convert_d:
    case OP_coerce_d:
      if (peek(pc, end) == OP_increment) {
        printAbcInstr(pc++);
        //console_ << "convert_d+increment =>> increment\n";
        return OP_increment;
      }
      return abcop;
    case OP_convert_b:
      if (peek(pc, end) == OP_iffalse)
        goto again;
      if (peek(pc, end) == OP_iftrue)
        goto again;
      return abcop;
    case OP_not:
      if (peek(pc, end) == OP_iffalse) {
        printAbcInstr(pc); // Skip OP_iffalse.
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        return OP_iftrue;
      }
      if (peek(pc, end) == OP_iftrue) {
        printAbcInstr(pc);
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        return OP_iffalse;
      }
      return abcop;
    case OP_equals:
      if (peek(pc, end) == OP_not && peek(pc + 1, end) == OP_iftrue) {
        printAbcInstr(pc++); // Skip OP_not.
        printAbcInstr(pc); // Skip OP_iftrue.
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        return OP_ifne;
      }
      if (peek(pc, end) == OP_iffalse) {
        printAbcInstr(pc); // Skip OP_iffalse.
        AvmCore::readOperands(pc, imm30, imm24, imm30b, imm8);
        return OP_ifne;
      }
      if (peek(pc, end) == OP_convert_b) {
        printAbcInstr(pc++); // Skip OP_convert_b.
        return abcop;
      }
      return abcop;
    default:
      return abcop;
  }
}

void AbcBuilder::printAbcInstr(const uint8_t* pc) {
  if (enable_verbose)
    halfmoon::printAbcInstr(method_, console_, pc);
}

/// Peephole optimize this def.
///
Def* AbcBuilder::peephole(Def *ref) {
  // fixme: we ignore phis & labels because phi/labels at loop headers
  // are incomplete during IR construction.  We could do better by only
  // ignoring loop header labels, for now we just ignore all of them.
  switch (kind(definer(*ref))) {
    case HR_label:
      return ref;
  }
  return halfmoon::peephole(ref, ir_, &factory_);
}

///
/// static ABC->HR mapping data, used by AbcBuilder::visitInstr
///

// each of the following gives 4 HR alternatives for
// an abc property instruction, indexed by AbcBuilder::NameArity:
//
//    kNameKnown,   // name with no args, e.g. o.name
//    kNameIndex,   // name with index on stack, e.g. o[index]
//    kNameNs,      // name with namespace on stack, e.g. o.ns::name
//    kNameNsIndex  // name with namespace and index on stack o.ns::[index]

// OP_getproperty
static const InstrKind getproperty_kinds[] = {
    HR_abc_getprop, HR_abc_getpropx, HR_abc_getpropns, HR_abc_getpropnsx
};

// OP_getsuper
static const InstrKind getsuper_kinds[] = {
    HR_abc_getsuper, HR_abc_getsuperx, HR_abc_getsuperns, HR_abc_getsupernsx
};

// OP_getdescendants
static const InstrKind getdescendants_kinds[] = {
    HR_abc_getdescendants, HR_abc_getdescendantsx, HR_abc_getdescendantsns,
    HR_abc_getdescendantsnsx
};

// OP_setproperty
static const InstrKind setproperty_kinds[] = {
    HR_abc_setprop, HR_abc_setpropx, HR_abc_setpropns, HR_abc_setpropnsx
};

// OP_setsuper
static const InstrKind setsuper_kinds[] = {
    HR_abc_setsuper, HR_abc_setsuperx, HR_abc_setsuperns, HR_abc_setsupernsx
};

// OP_initproperty
static const InstrKind initproperty_kinds[] = {
    HR_abc_initprop, HR_abc_initpropx, HR_abc_initpropns, HR_abc_initpropnsx
};

// OP_deleteproperty
static const InstrKind deleteproperty_kinds[] = {
    HR_abc_deleteprop, HR_abc_deletepropx, HR_abc_deletepropns,
    HR_abc_deletepropnsx
};

// OP_callproperty, OP_callpropvoid
static const InstrKind callproperty_kinds[] = {
    HR_abc_callprop, HR_abc_callpropx, HR_abc_callpropns, HR_abc_callpropnsx
};

// OP_callsuper, OP_callsupervoid
static const InstrKind callsuper_kinds[] = {
    HR_abc_callsuper, HR_abc_callsuperx, HR_abc_callsuperns, HR_abc_callsupernsx
};

// OP_callproplex
static const InstrKind callproplex_kinds[] = {
    HR_abc_callproplex, HR_abc_callproplexx, HR_abc_callproplexns,
    HR_abc_callproplexnsx
};

// OP_constructprop
static const InstrKind constructprop_kinds[] = {
    HR_abc_constructprop, HR_abc_constructpropx, HR_abc_constructpropns,
    HR_abc_constructpropnsx
};

// OP_findproperty
static const InstrKind findprop_kinds[] = {
    HR_abc_findproperty, HR_abc_findpropertyx, HR_abc_findpropertyns,
    HR_abc_findpropertynsx
};

// OP_findpropstrict
static const InstrKind findstrict_kinds[] = {
    HR_abc_findpropstrict, HR_abc_findpropstrictx, HR_abc_findpropstrictns,
    HR_abc_findpropstrictnsx
};

// An IfOpInfo carries an HR if-instruction and a sense
// (the relationship of test result to outgoing branches).
struct IfOpInfo {
  bool sense;
  InstrKind kind;
};

/// return the IfOpInfo for a given abc op
///
static inline const IfOpInfo& getIfOpInfo(AbcOpcode abcop) {
  // map from OP_ to HR_ conditionals
  static const IfOpInfo if_map[] = {
    { false, HR_abc_lessthan }, // ifnlt    = 0x0C
    { false, HR_abc_lessequals }, // ifnle    = 0x0D
    { false, HR_abc_greaterthan }, // ifngt    = 0x0E
    { false, HR_abc_greaterequals }, // ifnge    = 0x0F
    { false, HR_MAX }, // jump     = 0x10 (unused slot)
    { false, HR_MAX }, // iftrue   = 0x11 (unused slot)
    { false, HR_MAX }, // iffalse  = 0x12 (unused slot)
    { true,  HR_abc_equals }, // ifeq     = 0x13
    { false, HR_abc_equals }, // ifne     = 0x14
    { true,  HR_abc_lessthan }, // iflt     = 0x15
    { true,  HR_abc_lessequals }, // ifle     = 0x16
    { true,  HR_abc_greaterthan }, // ifgt     = 0x17
    { true,  HR_abc_greaterequals }, // ifge     = 0x18
    { true,  HR_abc_strictequals }, // ifstricteq=0x19
    { false, HR_abc_strictequals }, // ifstrictne=0x1A
  };
  return if_map[abcop - OP_ifnlt];
}

RecordedType AbcBuilder::getRecordedType(const uint8_t* pc, int input_index,
                                         int input_count, int output_count) {
  MethodProfile* profile = JitManager::getProfile(method_);
  ProfiledState* profile_types = profile->getProfileState(int(pc - code_pos_),
                                                          input_count,
                                                          output_count);
  return profile_types->getInputType(input_index);
}

bool AbcBuilder::isType(Def* val, RecordedType recorded_type) {
  const Type* val_type = type(val);
  switch (recorded_type) {
  case profiler::kDOUBLE: return isNumber(val_type);
  case profiler::kOBJECT: return isScriptObject(val_type);
  case profiler::kSTRING: return isString(val_type);
  case profiler::kINTEGER: return isInt(val_type); // fixme: what about 29bit/53bit/uint
  case profiler::kBOOLEAN: return isBoolean(val_type);
  case profiler::kARRAY: {
    return (val_type == lattice_.array_type[kTypeNullable]) ||
      (val_type == lattice_.array_type[kTypeNotNull]);
  }
  case profiler::kVARIADIC:
  case profiler::kUNINITIALIZED: {
    // I think this means halfmoon lost type data that the verifier had
    return true;
  }

  case profiler::kVECTOR: assert (false);
  case profiler::kNUMERIC: return false;  // represents multiple types
  default:
    printf("Unknown type rep %d\n", (int) recorded_type);
    assert (false && "Unknown type");
    break;
  }

  assert (false);
  return false;
}

/// Stores the current ABC state by emitting setlocals for every value
/// in the abc stack frame.
/// scopep offset is the scope depth.
/// stackp_offset is the stack depth
/// we cannot rely on the stackp_ / scopep_ vars because defs may already have
/// been popped / pushed prior to calling savestate.
void AbcBuilder::saveState(const uint8_t*,
                          int, int) {
  assert(false);
}

bool AbcBuilder::hasType(Def* val, RecordedType recorded_type) {
  return isType(val, recorded_type) || !isAny(type(val));
}

InstrKind AbcBuilder::getSpeculativeKind(RecordedType recorded_type) {
  switch (recorded_type) {
  case profiler::kDOUBLE: return HR_speculate_number;
  case profiler::kINTEGER: return HR_speculate_int;
  case profiler::kSTRING: return HR_speculate_string;
  case profiler::kOBJECT: return HR_speculate_object;
  case profiler::kARRAY: return HR_speculate_array;
  case profiler::kNUMERIC : return HR_speculate_numeric;
  case profiler::kBOOLEAN: return HR_speculate_bool;
  default:
    printf("Unknown type in getSpeculativeKind: %d\n", (int) recorded_type);
    assert (false);
  }

  assert (false);
  return HR_speculate_number;
}

Def* AbcBuilder::toSpeculativeType(Def* val, RecordedType recorded_type) {
  if (hasType(val, recorded_type)
    || (recorded_type == profiler::kVARIADIC)
    // Profiling strings doesn't seem to buy anything, quick test.
    || (recorded_type == profiler::kSTRING)) {
    return val;
  }
  
  return binaryExpr(getSpeculativeKind(recorded_type), val, state());
}

Def* AbcBuilder::typeSpecializedBinary(AbcOpcode abcop, const uint8_t* pc,
                                       Def* lhs, Def* rhs) {
  assert(shouldSpeculate());
  int left_input = 0;
  int right_input = 1;

  int input_count = 2;
  int output_count = 1;

  RecordedType left_profiled_type = getRecordedType(pc, left_input,
                                                    input_count,
                                                    output_count);
  RecordedType right_profiled_type = getRecordedType(pc, right_input,
                                                     input_count,
                                                     output_count);

  if (!hasType(lhs, left_profiled_type)
    || !hasType(rhs, right_profiled_type)) {
      saveState(pc, 0, input_count);
      lhs = toSpeculativeType(lhs, left_profiled_type);
      rhs = toSpeculativeType(rhs, right_profiled_type);
  }

  return binaryStmt(kind_map_[abcop], lhs, rhs);
}

/// visit an abc instruction
///
void AbcBuilder::visitInstr(const uint8_t* pc, AbcOpcode abcop, uint32_t imm30,
                            uint32_t imm30b, int32_t imm8) {
  (void) pc;
  Def* temp1, *temp2, *temp3;
  Def** args;
  DeoptSafepointInstr* sfp;
  switch (abcop) {
    default:
      assert(false && "Unsupported opcode");

    case OP_istype:
      pushDef(binaryStmt(HR_abc_istype, traitsConst(imm30), popDef()));
      break;

    case OP_debug:
    case OP_bkpt:
    case OP_bkptline:
    case OP_label:
    case OP_nop:
    case OP_timestamp:
      break;

    case OP_debugfile: {
      // Steal code from CodegenLIR and put it into the stubs for this
      if (enable_trace) {
        Def* filename = createConst(lattice_.makeStringConst(pool_, imm30));
        debugInstr(HR_debugfile, filename);
      }
      break;
    }
    case OP_debugline: {
      // Steal code from CodegenLIR and put it into the stubs for this
      if (enable_trace) {
        Def* lineno = createConst(lattice_.makeIntConst(imm30));
        debugInstr(HR_debugline, lineno);
      }
      break;
    }

    case OP_jump:
      jumpStmt();
      break;

    case OP_lookupswitch:
      switchStmt(imm30b + 1, popDef());
      break;

    case OP_returnvalue:
      returnStmt(coerceExpr(sig_->returnTraits(), popDef()));
      break;

    case OP_returnvoid:
      returnStmt(
          coerceExpr(sig_->returnTraits(), createConst(lattice_.void_type)));
      break;

    case OP_throw:
      throwStmt(popDef());
      break;

    case OP_iftrue:
    case OP_iffalse:
      ifStmt(abcop == OP_iftrue, coerceExpr(lattice_.boolean_traits, popDef()));
      break;

    case OP_ifnlt:
    case OP_ifnle:
    case OP_ifngt:
    case OP_ifnge:
    case OP_ifeq:
    case OP_ifne:
    case OP_iflt:
    case OP_ifle:
    case OP_ifgt:
    case OP_ifge: {
      const IfOpInfo& info = getIfOpInfo(abcop);
      temp2 = popDef();
      temp1 = popDef();
      ifStmt(info.sense, binaryStmt(info.kind, temp1, temp2));
      break;
    }

    case OP_ifstricteq:
    case OP_ifstrictne: {
      const IfOpInfo& info = getIfOpInfo(abcop);
      Def** args = popArgs(2);
      temp1 = binaryExpr(info.kind, args[0], args[1]);
      ifStmt(info.sense, temp1);
      break;
    }

    case OP_getlocal0:
    case OP_getlocal1:
    case OP_getlocal2:
    case OP_getlocal3:
      pushDef(getLocal(abcop - OP_getlocal0));
      break;

    case OP_setlocal0:
    case OP_setlocal1:
    case OP_setlocal2:
    case OP_setlocal3:
      setLocal(abcop - OP_setlocal0, popDef());
      break;

    case OP_getlocal:
      pushDef(getLocal(imm30));
      break;

    case OP_setlocal:
      setLocal(imm30, popDef());
      break;

    case OP_dup:
      pushDef(peekDef());
      break;

    case OP_pop:
      popDef();
      break;

    case OP_popscope:
      scopep_--;
      if (withbase_ >= scopep_)
        withbase_ = -1;
      break;

    case OP_pushwith:
      temp1 = nullCheck(popDef());
      setLocal(++scopep_, temp1);
      if (withbase_ == -1)
        withbase_ = scopep_;
      break;

    case OP_pushscope:
      temp1 = nullCheck(popDef());
      setLocal(++scopep_, temp1);
      break;

    case OP_swap:
      temp1 = popDef();
      temp2 = popDef();
      pushDef(temp1);
      pushDef(temp2);
      break;

    case OP_kill:
      setLocal(imm30, createConst(lattice_.void_type));
      break;

    case OP_pushnamespace:
      pushConst(lattice_.makeNamespaceConst(pool_->getNamespace(imm30)));
      break;

    case OP_pushdouble:
      pushConst(lattice_.makeDoubleConst(pool_->cpool_double[imm30]->value));
      break;

    case OP_pushuint:
      pushConst(lattice_.makeUIntConst(pool_->cpool_uint[imm30]));
      break;

    case OP_pushstring:
      pushConst(lattice_.makeStringConst(pool_, imm30));
      break;

    case OP_pushbyte:
      pushConst(lattice_.makeIntConst(int8_t(imm8)));
      break;

    case OP_pushshort:
      pushConst(lattice_.makeIntConst(int16_t(imm30)));
      break;

    case OP_pushint:
      pushConst(lattice_.makeIntConst(pool_->cpool_int[imm30]));
      break;

    case OP_pushnull:
      pushConst(lattice_.null_type);
      break;

    case OP_pushundefined:
      pushConst(lattice_.void_type);
      break;

    case OP_pushnan:
      pushConst(lattice_.makeDoubleConst(MathUtils::kNaN));
      break;

    case OP_pushtrue:
      pushConst(lattice_.makeBoolConst(true));
      break;

    case OP_pushfalse:
      pushConst(lattice_.makeBoolConst(false));
      break;

    case OP_getscopeobject:
      pushDef(getLocal(scope_base_ + imm8));
      break;

    case OP_getglobalscope:
      pushDef(getglobalscope());
      break;

    case OP_getouterscope:
      pushDef(getouterscopeStmt(imm30));
      break;

    case OP_coerce_a:
//      temp1 = popDef();
//      pushDef(isAtomModel(type(temp1)) ? temp1 :
//          unaryExpr(toatomKind(temp1), temp1));
      break;

    case OP_astype:
      pushDef(binaryExpr(HR_abc_astype, traitsConst(imm30), popDef()));
      break;

    case OP_coerce:
      pushDef(coerceExpr(getNamedTraits(imm30), popDef()));
      break;

    case OP_instanceof:
      temp2 = popDef(); // RHS is null checked by instanceof since it throws a different exception
      temp1 = popDef();
      pushDef(binaryStmt(kind_map_[abcop], temp1, temp2));
      break;

    case OP_istypelate:
    case OP_astypelate:
    case OP_in:
      temp2 = nullCheck(popDef()); // RHS object cannot be null.
      temp1 = popDef();
      pushDef(binaryStmt(kind_map_[abcop], temp1, temp2));
      break;

    case OP_divide:
    case OP_modulo:
    case OP_subtract:
    case OP_multiply:
    case OP_add_i:
    case OP_subtract_i:
    case OP_multiply_i:
    case OP_bitor:
    case OP_bitand:
    case OP_bitxor:
    case OP_lshift:
    case OP_rshift:
    case OP_urshift:
      temp2 = popDef();
      temp1 = popDef();
      pushDef(binaryStmt(kind_map_[abcop], temp1, temp2));
      break;

    case OP_lessthan:
    case OP_greaterthan:
    case OP_lessequals:
    case OP_greaterequals:
    case OP_equals:
    case OP_nextname:
    case OP_nextvalue:
    {
      temp2 = popDef();
      temp1 = popDef();
      pushDef(binaryStmt(kind_map_[abcop], temp1, temp2));
      break;
    }

    case OP_add:
    {
      temp2 = popDef();
      temp1 = popDef();
      pushDef(shouldSpeculate() ?
        typeSpecializedBinary(abcop, pc, temp1, temp2) :
        binaryStmt(kind_map_[abcop], temp1, temp2));
      break;
    }
    
    case OP_strictequals:
      temp2 = popDef();
      temp1 = popDef();
      pushDef(binaryExpr(HR_abc_strictequals, temp1, temp2));
      break;

    case OP_convert_o:
      pushDef(nullCheck(popDef()));
      break;

    case OP_coerce_s:
      pushDef(coerceExpr(lattice_.string_traits, popDef()));
      break;

    case OP_coerce_u:
    case OP_convert_u:
      pushDef(coerceExpr(lattice_.uint_traits, popDef()));
      break;

    case OP_coerce_i:
    case OP_convert_i:
      pushDef(coerceExpr(lattice_.int_traits, popDef()));
      break;

    case OP_coerce_d:
    case OP_convert_d:
      pushDef(coerceExpr(lattice_.number_traits, popDef()));
      break;

    case OP_coerce_b:
    case OP_convert_b:
      pushDef(coerceExpr(lattice_.boolean_traits, popDef()));
      break;

    case OP_coerce_o:
      pushDef(coerceExpr(lattice_.object_traits, popDef()));
      break;

    case OP_inclocal:
    case OP_declocal:
    case OP_inclocal_i:
    case OP_declocal_i:
      //setLocal(imm30, unaryTemplate(kind_map_[abcop], getLocal(imm30)));
      setLocal(imm30, unaryStmt(kind_map_[abcop], getLocal(imm30)));
      break;

    case OP_increment:
    case OP_decrement:
    case OP_increment_i:
    case OP_decrement_i:
    case OP_bitnot:
    case OP_negate_i:
    case OP_sxi1:
    case OP_sxi8:
    case OP_sxi16:
    case OP_negate:
    case OP_not:
    case OP_li8:
    case OP_li16:
    case OP_li32:
    case OP_lf32:
    case OP_lf64:
    case OP_checkfilter:
      pushDef(unaryStmt(kind_map_[abcop], popDef()));
      break;

    case OP_dxnslate:
      unaryStmt(kind_map_[abcop], popDef());
      break;

    case OP_si8:
    case OP_si16:
    case OP_si32:
    case OP_sf32:
    case OP_sf64:
      temp2 = popDef(); // addr
      temp1 = popDef(); // value
      binaryStmt(kind_map_[abcop], temp1 /* value */, temp2 /* addr */);
      break;

    case OP_convert_s:
    case OP_esc_xelem:
    case OP_esc_xattr:
      pushDef(unaryStmt(kind_map_[abcop], popDef()));
      break;

    case OP_typeof:
      pushDef(unaryExpr(kind_map_[abcop], popDef()));
      break;

    case OP_dxns:
      unaryStmt(HR_abc_dxns, createConst(lattice_.makeStringConst(pool_, imm30)));
      break;

    case OP_newclass:
      // fixme: create template for this.  needs nary templates.
      temp1 = coerceExpr(lattice_.class_traits, popDef());
      pushDef(newclassStmt(scope_count(), temp1, scopes(), imm30));
      break;

    case OP_newfunction:
      temp1 = createConst(lattice_.makeMethodConst(pool_, imm30));
      pushDef(naryStmt1(HR_newfunction, temp1, scopes(), scope_count()));
      break;

    case OP_newcatch:
      pushDef(newcatchStmt(imm30));
      break;

    case OP_newactivation:
      pushDef(unaryStmt(HR_newactivation, env_param()));
      break;

    case OP_finddef:
      temp1 = constName(imm30);
      pushDef(finddefStmt(temp1));
      break;

    case OP_findpropstrict:
      pushDef(findStmt(findstrict_kinds, imm30, abc_->abc_instr(pc)));
      break;

    case OP_findproperty:
      pushDef(findStmt(findprop_kinds, imm30, abc_->abc_instr(pc)));
      break;

    case OP_getlex:
      pushDef(getlexStmt(imm30, abc_->abc_instr(pc)));
      break;

    case OP_getproperty:
      pushDef(callStmt(getproperty_kinds, imm30, 0));
      break;

    case OP_getsuper:
      pushDef(callStmt(getsuper_kinds, imm30, 0));
      break;

    case OP_deleteproperty:
      pushDef(callStmt(deleteproperty_kinds, imm30, 0));
      break;

    case OP_getdescendants:
      pushDef(callStmt(getdescendants_kinds, imm30, 0));
      break;

    case OP_setproperty:
      callStmt(setproperty_kinds, imm30, 1);
      break;

    case OP_initproperty:
      initStmt(imm30);
      break;

    case OP_setsuper:
      callStmt(setsuper_kinds, imm30, 1);
      break;

    case OP_callproperty:
      sfp = emitCallSafepoint(pc, imm30b);
      temp1 = callStmt(callproperty_kinds, imm30, imm30b);
      emitFinishCall(sfp, temp1);
      pushDef(temp1);
      break;

    case OP_callproplex:
      sfp = emitCallSafepoint(pc, imm30b);
      temp1 = callStmt(callproplex_kinds, imm30, imm30b);
      emitFinishCall(sfp, temp1);
      pushDef(temp1);
      break;

    case OP_constructprop:
      sfp = emitCallSafepoint(pc, imm30b);
      temp1 = callStmt(constructprop_kinds, imm30, imm30b);
      emitFinishCall(sfp, temp1);
      pushDef(temp1);
      break;

    case OP_callsuper:
      sfp = emitCallSafepoint(pc, imm30b);
      temp1 = callStmt(callsuper_kinds, imm30, imm30b);
      emitFinishCall(sfp, temp1);
      pushDef(temp1);
      break;

    case OP_callpropvoid:
      sfp = emitCallSafepoint(pc, imm30b);
      callStmt(callproperty_kinds, imm30, imm30b);
      emitFinishCall(sfp, NULL);
      break;

    case OP_callsupervoid:
      sfp = emitCallSafepoint(pc, imm30b);
      callStmt(callsuper_kinds, imm30, imm30b);
      emitFinishCall(sfp, NULL);
      break;

    case OP_callstatic: {
      // fixme: coerce args first, but this requires knowing obj type, which
      //        we don't have inside of loops.
      sfp = emitCallSafepoint(pc, imm30b);
      args = popArgs(imm30b);
      MethodInfo* callee = pool_->getMethodInfo(imm30);
      assert(callee->method_id() == (int)imm30);
      temp1 = popDef();
      temp1 = coerceArgs(temp1, args, imm30b, callee);
      temp1 = nullCheck(temp1); // obj
      temp2 = ordinalConst(imm30);
      temp2 = binaryExpr(HR_loadenv_env, temp2, env_param());
      temp3 = callStmt2(HR_callstatic, temp2, temp1, args, imm30b);
      emitFinishCall(sfp, temp3);
      pushDef(temp3);
      break;
    }

    case OP_call:
      // stack: function thisarg args...
      sfp = emitCallSafepoint(pc, imm30);
      args = popArgs(imm30); // imm30 = extra_argc
      temp2 = popDef(); // thisarg
      temp1 = popDef(); // function
      temp3 = callStmt2(HR_call, temp1, temp2, args, imm30);
      emitFinishCall(sfp, temp3);
      pushDef(temp3);
      break;

    case OP_construct:
      sfp = emitCallSafepoint(pc, imm30);
      args = popArgs(imm30);
      temp2 = createConst(lattice_.null_type); // thisarg placeholder
      temp1 = popDef(); // constructor function
      temp3 = callStmt2(HR_construct, temp1, temp2, args, imm30);
      emitFinishCall(sfp, temp3);
      pushDef(temp3);
      break;

    case OP_constructsuper:
      sfp = emitCallSafepoint(pc, imm30);
      constructsuperStmt(imm30);
      emitFinishCall(sfp, NULL);
      break;

    case OP_getslot:
      pushDef(getslotStmt(popDef(), imm30 - 1));
      break;

    case OP_setslot: {
      // fixme: verify coerce/nullcheck order.
      int slot = imm30 - 1;
      temp2 = popDef(); // value
      temp1 = popDef(); // object
      setslotStmt(slot, temp1, temp2);
      break;
    }

    case OP_getglobalslot:
      temp1 = getglobalscope();
      pushDef(getslotStmt(temp1, imm30 - 1));
      break;

    case OP_setglobalslot:
      temp1 = getglobalscope();
      temp2 = popDef();
      setslotStmt(imm30 - 1, temp1, temp2);
      break;

    case OP_hasnext2:
      pushDef(hasnext2Stmt(&frame_[imm30], &frame_[imm30b]));
      if (has_reachable_exceptions_) {
        // Make sure to update the state of the locals
        setLocal(imm30, getLocal(imm30));
        setLocal(imm30b, getLocal(imm30b));
      }
      break;

    case OP_hasnext:
      temp1 = popDef(); // index:int
      temp2 = popDef(); // object
      pushDef(binaryStmt(HR_abc_hasnext, temp2, temp1));
      break;

    case OP_newobject:
      pushDef(naryStmt(HR_newobject, 2 * imm30));
      break;

    case OP_newarray:
      pushDef(naryStmt(HR_newarray, imm30));
      break;

    case OP_applytype:
      // FIXME: abc format allows N type parameters as args, but Vector
      // is the only class works, and it only allows 1 arg.  There is a
      // misplaced assert() in VectorClass::applyTypeArgs().
      pushDef(naryStmt(HR_applytype, imm30 + 1));
      break;
  }
  if (enable_framestate) 
    printFrameState();
}

Def* AbcBuilder::getglobalscope() {
  return method_->declaringScope()->size == 0 ? getLocal(scope_base_) :
      getouterscopeStmt(0);
}

/// Expand getlex into (fatgetprop name (findstrict name [scopes]))
///
Def* AbcBuilder::getlexStmt(uint32_t name_index, AbcInstr* abc_instr) {
  Def* object = findStmt(findstrict_kinds, name_index, abc_instr);
  return callStmt2(getproperty_kinds[kNameKnown], constName(name_index),
                   object, 0, 0);
}

Def* AbcBuilder::findStmt(const InstrKind* kinds, uint32_t name_index,
                          AbcInstr* abc_instr) {
  Def* name = constName(name_index);
  Def* env = env_param();
  Def** scopes = this->scopes();
  int scope_count = this->scope_count();
  NameArity arity = nameArity(name);
  switch (arity) {
    default: assert(false && "bad arity");
    case kNameKnown: {
      switch (abc_instr->abc_op) {
        default: assert(false && "bad AbcOpcode");
        case OP_getscopeobject:
          return scopes[abc_instr->index];
        case OP_getouterscope:
          return getouterscopeStmt(abc_instr->index);
        case OP_finddef:
          assert(abc_instr->index == name_index);
          return finddefStmt(name);
        case OP_findpropglobal:
        case OP_findpropglobalstrict:
        case OP_findproperty:
        case OP_findpropstrict:
          return naryStmt3(kinds[arity], name, env, ordinalConst(withbase_ == -1 ? -1 : withbase_ - scope_base_), scopes, scope_count);
        case OP_getglobalscope:
          return getglobalscope();
      }
      break;
    }
    case kNameIndex:
    case kNameNs:
      return naryStmt4(kinds[arity], name, env, ordinalConst(withbase_ == -1 ? -1 : withbase_ - scope_base_), popDef(), scopes, scope_count);
    case kNameNsIndex: {
      Def* index = popDef();
      Def* ns = popDef();
      return naryStmt4(kinds[arity], name, env, ordinalConst(withbase_ == -1 ? -1 : withbase_ - scope_base_), ns, index, scopes, scope_count);
    }
  }
}

Def* AbcBuilder::finddefStmt(Def *name) {
  return binaryStmt(HR_abc_finddef, name, env_param());
}

Def* AbcBuilder::coerceExpr(Traits* traits, Def *val) {
  if (coerceIsNop(&lattice_, val, traits))
      return val;
  Def* traits_in = traitsConst(traits);
  BinaryStmt* coerce = factory_.newBinaryStmt(coerceKind(traits), effect(),
                                              traits_in, val);
  builder_.addInstr(coerce);
  set_effect(coerce->effect_out());
  return peephole(coerce->value_out());
}

Def* AbcBuilder::toNumber(Def* val) {
  if (isNumber(type(val)))
    return val;
  // tonumber is a statement because it can call Object.valueOf()
  return unaryStmt(HR_tonumber, val);
}

Def* AbcBuilder::toInt(Def* val) {
  assert (false && "Not quite supported");
  if (isInt(type(val)))
    return val;
  // toint is a statement because it can call Object.valueOf()
  return unaryStmt(HR_toint, val);
}

bool AbcBuilder::needAbcState() {
  if (enable_vmstate) return true;
  if (has_reachable_exceptions_) {
    return handlerCoversPc(pc_);
  }
  return false;
}

bool AbcBuilder::handlerCoversPc(const uint8_t* pc) {
  if (has_reachable_exceptions_) {
    ExceptionHandlerTable* exTable = method_->abc_exceptions();
    for (int i=0, n=exTable->exception_count; i < n; i++) {
      ExceptionHandler* handler = &exTable->exceptions[i];
      if (pc >= code_pos_ + handler->from && pc < code_pos_ + handler->to) {
        return true;
      }
    }
  }
  return false;
}

bool AbcBuilder::pcIsHandler(const uint8_t* pc) {
  if (has_reachable_exceptions_) {
    ExceptionHandlerTable* exTable = method_->abc_exceptions();
    for (int i=0, n=exTable->exception_count; i < n; i++) {
      ExceptionHandler* handler = &exTable->exceptions[i];
      if (pc == code_pos_ + handler->target) {
        return true;
      }
    }
  }
  return false;
}

void AbcBuilder::createSetLocalInstr(int i, Def* val) {
  SetlocalInstr* setlocal = factory_.newSetlocalInstr(i, state(), val);
  // set_state(setlocal->state_out());
  builder_.addInstr(setlocal);
  frame_[setlocal_pos_ + i] = setlocal->state_out();
}

void AbcBuilder::setLocal(int i, Def* val) {
  frame_[i] = val;
  // We have to save all assignments because setlocal and safepoints
  // are used for deopt in addition to exceptions.
  if (!has_reachable_exceptions_) {
    return;
  }

  createSetLocalInstr(i, val);
}

Traits* AbcBuilder::getNamedTraits(uint32_t type_name_index) {
  DomainMgr* domainMgr = pool_->core->domainMgr();
  const Multiname* name = pool_->precomputedMultiname(type_name_index);
  Traits* traits = domainMgr->findTraitsInPoolByMultiname(pool_, *name);

  assert (traits != NULL);
  assert (traits != (Traits*)BIND_AMBIGUOUS);

  if (name->isParameterizedType()) {
    Traits* param_traits = name->getTypeParameter() ?
            getNamedTraits(name->getTypeParameter()) :
            NULL ;
    traits = pool_->resolveParameterizedType(toplevel_, traits, param_traits);
  }
  return traits;
}

/// Make a constant multiname
///
Def* AbcBuilder::constName(uint32_t index) {
  return createConst(lattice_.makeNameConst(pool_, index));
}

/// Return the kind of parameters that the name requires as values.
///
AbcBuilder::NameArity AbcBuilder::nameArity(const Type* name) {
  const Multiname* multiname = nameVal(name);
  if (multiname->isRtname())
    return multiname->isRtns() ? kNameNsIndex : kNameIndex;
  else
    return multiname->isRtns() ? kNameNs : kNameKnown;
}

void AbcBuilder::pushConst(const Type* type) {
  pushDef(createConst(type));
}

void AbcBuilder::pushDef(Def *val) {
  assert(stackp_ >= stack_base_ - 1 && stackp_ + 1 < framesize_);
  setLocal(++stackp_, val);
}

Def* AbcBuilder::naryStmt1(InstrKind kind, Def *param, // param, function, or id
                          Def* args[], int argc) {
  NaryStmt1* call = factory_.newNaryStmt1(kind, effect(), param, argc, args);
  builder_.addInstr(call);
  set_effect(call->effect_out());
  return call->value_out();
}

Def* AbcBuilder::naryStmt2(InstrKind kind, Def *name, // name, function, or id
                          Def *env, Def* args[], int extra_argc) {
  NaryStmt2* stmt = factory_.newNaryStmt2(kind, effect(), name, env,
                                          extra_argc, args);
  return finishStmt(stmt, stmt->effect_out(), stmt->value_out());
}

Def* AbcBuilder::naryStmt3(InstrKind kind, Def* name, // multiname
                           Def* env, Def *index, Def* args[], int extra_argc) {
  NaryStmt3* stmt = factory_.newNaryStmt3(kind, effect(), name, env, index,
                                          extra_argc, args);
  return finishStmt(stmt, stmt->effect_out(), stmt->value_out());
}

Def* AbcBuilder::naryStmt4(InstrKind kind, Def* name, // multiname
                           Def* env, Def* ns, Def *index, Def* args[],
                           int extra_argc) {
  NaryStmt4* stmt = factory_.newNaryStmt4(kind, effect(), name, env, ns, index,
                                          extra_argc, args);
  return finishStmt(stmt, stmt->effect_out(), stmt->value_out());
}

Def* AbcBuilder::naryStmt4(InstrKind kind, Def* name, // multiname
                           Def* env, Def* ns, Def *index, Def* index2, Def* args[],
                           int extra_argc) {
  NaryStmt4* stmt = factory_.newNaryStmt4(kind, effect(), name, env, ns, index, index2,
                                          extra_argc, args);
  return finishStmt(stmt, stmt->effect_out(), stmt->value_out());
}

Def* AbcBuilder::callStmt2(InstrKind kind, Def *name, // name, function, or id
                          Def *object, Def* args[], int extra_argc) {
  CallStmt2* call = factory_.newCallStmt2(kind, effect(), name, object,
                                          extra_argc, args);
  return finishStmt(call, call->effect_out(), call->value_out());
}

Def* AbcBuilder::finishStmt(Instr* instr, Def* effect, Def* value) {
  builder_.addInstr(instr);
  set_effect(effect);
  return value;
}

Def* AbcBuilder::callStmt3(InstrKind kind, Def* name, // multiname
                           Def* index, // [index]
                           Def *object, Def* args[], int extra_argc) {
  CallStmt3* call = factory_.newCallStmt3(kind, effect(), name, index, object,
                                          extra_argc, args);
  return finishStmt(call, call->effect_out(), call->value_out());
}

Def* AbcBuilder::callStmt4(InstrKind kind, Def* name, // multiname
                           Def* ns, // namespace argument
                           Def* index, // index argument
                           Def *object, Def* args[], int extra_argc) {
  CallStmt4* call = factory_.newCallStmt4(kind, effect(), name, ns, index,
                                          object, extra_argc, args);
  builder_.addInstr(call);
  set_effect(call->effect_out());
  return call->value_out();
}

Def* AbcBuilder::callStmt(const InstrKind* kinds, uint32_t index, int argc) {
  Def* name = constName(index);
  Def** args = popArgs(argc);
  switch (nameArity(name)) {
    default:
      assert(false && "illegal arity");
    case kNameKnown: {
      Def* object = nullCheck(popDef()); // fixme: move to templates.
      return callStmt2(kinds[kNameKnown], name, object, args, argc);
    }
    case kNameIndex: {
      Def* index = popDef();
      Def* object = nullCheck(popDef()); // fixme: move to templates.
      return callStmt3(kinds[kNameIndex], name, index, object, args, argc);
    }
    case kNameNs: {
      Def* ns = popDef();
      Def* object = nullCheck(popDef()); // fixme: move to templates.
      return callStmt3(kinds[kNameNs], name, ns, object, args, argc);
    }
    case kNameNsIndex: {
      Def* index = popDef();
      Def* ns = popDef();
      Def* object = nullCheck(popDef()); // fixme: move to templates.
      return callStmt4(kinds[kNameNsIndex], name, ns, index, object, args, argc);
    }
  }
}

/// Special optimization for initproperty.  If the slot is var, then turn
/// this into a setproperty command.
Def* AbcBuilder::initStmt(uint32_t index) {
  const Type* name = lattice_.makeNameConst(pool_, index);
  if (nameArity(name) == kNameKnown) {
    Traits* object_traits = getTraits(type(peekDef()));
    if (object_traits) {
      const TraitsBindings* tb = object_traits->getTraitsBindings();
      Traits* declarer;
      Binding b = tb->findBindingAndDeclarer(*nameVal(name), declarer);
      if (isVarSlot(b) || (isConstSlot(b) && declarer->init == method_)) {
        // turn initproperty into setproperty if we found a var or legal const slot.
        return callStmt(setproperty_kinds, index, 1);
      }
    }
  }
  return callStmt(initproperty_kinds, index, 1);
}

Def* AbcBuilder::newclassStmt(int scope_count, Def *base, Def* scopes[],
                              uint32_t class_id) {
  Allocator scratch;
  Def * class_traits = traitsConst(pool_->getClassTraits(class_id));
  return naryStmt2(HR_newclass, class_traits, base, scopes, scope_count);
}

Def** AbcBuilder::popArgs(int argc) {
  assert(stackp_ - argc >= stack_base_-1 && stackp_ - argc < framesize_);
  stackp_ -= argc;
  return &frame_[stackp_ + 1];
}

Def *AbcBuilder::popDef() {
  return popArgs(1)[0];
}

Def* AbcBuilder::peekDef() {
  assert(stackp_ - 1 >= stack_base_-1 && stackp_ - 1 < framesize_);
  return frame_[stackp_];
}

// semantics:
//  1. coerce object to super type
//  2. emitCoerceArgs: coerce arguments (N down to 1), then coerce
//     object to parameter 0 type
//  3. checknull object <- fixme: should this be step 3 or step 1?
//  4. invoker super constructor
void AbcBuilder::constructsuperStmt(int argc) {
  Traits* base_traits = method_->declaringTraits()->base;
  Def** args = popArgs(argc);
  Def* object = coerceExpr(base_traits, popDef());
  object = coerceArgs(object, args, argc, base_traits->init);
  object = nullCheck(object);
  callStmt2(HR_constructsuper, env_param(), object, args, argc);
}

// Coerce args left to right.
Def* AbcBuilder::coerceArgs(Def* object, Def** args, int argc,
                            MethodInfo* callee) {
  MethodSignaturep signature = callee->getMethodSignature();
  object = coerceExpr(signature->paramTraits(0), object);
  for (int i = 0; i < argc; ++i) {
    Traits* t =
        i + 1 <= signature->param_count() ? signature->paramTraits(i + 1) : NULL;
    args[i] = coerceExpr(t, args[i]);
  }
  return object;
}

/// add n-ary statement on passed args to ir graph in progress.
/// constrol input is taken from last statement added.
/// new statement's data output is returned, unless peephole()
/// bypasses (but note that stmt is linked into ir regardless)
///
Def* AbcBuilder::naryStmt(InstrKind kind, int argc) {
  NaryStmt0* stmt = factory_.newNaryStmt0(kind, effect(), argc, popArgs(argc));
  builder_.addInstr(stmt);
  set_effect(stmt->effect_out());
  return peephole(stmt->value_out());
}

/// add binary expr on passed args to ir graph in progress.
/// new instr's data output is returned, unless peephole()
/// bypasses (but note that instr is linked into ir regardless)
///
Def* AbcBuilder::binaryExpr(InstrKind kind, Def *arg0, Def *arg1) {
  BinaryExpr* instr = factory_.newBinaryExpr(kind, arg0, arg1);
  builder_.addInstr(instr);
  return peephole(instr->value_out());
}

/// add binary statement on passed args to ir graph in progress.
/// constrol input is taken from last statement added.
/// new statement's data output is returned, unless peephole()
/// bypasses (but note that stmt is linked into ir regardless)
///
Def* AbcBuilder::binaryStmt(InstrKind kind, Def *arg0, Def *arg1) {
  BinaryStmt* stmt = factory_.newBinaryStmt(kind, effect(), arg0, arg1);
  builder_.addInstr(stmt);
  set_effect(stmt->effect_out());
  return peephole(stmt->value_out());
}

/// add unary expr on passed arg to ir graph in progress.
/// new instr's data output is returned, unless peephole()
/// bypasses (but note that instr is linked into ir regardless)
///
Def* AbcBuilder::unaryExpr(InstrKind kind, Def* arg) {
  UnaryExpr* instr = factory_.newUnaryExpr(kind, arg);
  builder_.addInstr(instr);
  return peephole(instr->value_out());
}

/// add unary statement on passed arg to ir graph in progress.
/// control input is taken from last statement added.
/// new statement's data output is returned, unless peephole()
/// bypasses (but note that stmt is linked into ir regardless)
///
Def* AbcBuilder::unaryStmt(InstrKind kind, Def* arg) {
  UnaryStmt* stmt = factory_.newUnaryStmt(kind, effect(), arg);
  builder_.addInstr(stmt);
  set_effect(stmt->effect_out());
  return peephole(stmt->value_out());
}

/// add return or throw statement to ir graph.
///
void AbcBuilder::stopStmt(InstrKind kind, Def *value, LabelInstr** label_ptr) {
  LabelInstr* label = *label_ptr;
  if (!label) {
    *label_ptr = label = factory_.newLabelInstr(2); // effect, value
    ir_->addBlock(label);
    StopInstr* stop = factory_.newStopInstr(kind, &label->params[0], &label->params[1]);
    ir_->addInstrAfter(label, stop);
  }
  GotoInstr* goto_instr = factory_.newGotoStmt(label);
  goto_instr->args[0] = effect();
  goto_instr->args[1] = value;
  builder_.addInstr(goto_instr);
}

Def * AbcBuilder::nullCheck(Def* ptr) {
  const Type* ptr_type = type(ptr);
  if (!isNullable(ptr_type) && ptr_type->kind != kTypeVoid)
    return ptr;

  //ir_->addNullcheck(&effect_, &ptr_out);
  UnaryStmt* stmt = factory_.newUnaryStmt(HR_cknull, effect(), ptr);
  builder_.addInstr(stmt);
  Def* ptr_out = stmt->value_out();

  // Refs to ptr now should point to notnull(ptr)
  // fixme: this only renames local references.  What about others earlier
  // that just aren't in the locals?
  // should we insert a pi/sigma instr instead?
  for (FrameRange<Def*> r = frameRange(frame_); !r.empty(); r.popFront())
    if (r.front() == ptr)
      r.front() = ptr_out;

  set_effect(stmt->effect_out());
  return ptr_out;
}

/// add unary statement on passed arg to ir graph in progress.
/// control input is taken from last statement added.
/// new statement's data output is returned, unless peephole()
/// bypasses (but note that stmt is linked into ir regardless)
///
void AbcBuilder::debugInstr(InstrKind kind, Def* arg) {
  DebugInstr* stmt = factory_.newDebugInstr(kind, effect(), arg);
  builder_.addInstr(stmt);
  set_effect(stmt->effect_out());
}

/// Handle an if statement with the given sense and condition
///
void AbcBuilder::ifStmt(bool sense, Def* cond) {
  const Type* cond_type = type(cond);
  if (isConst(cond_type)) {
    // cond is already known to be const, replace if with goto
    int succ = int(boolVal(cond_type) == sense);
    addGoto(abc_block_->succ_blocks[succ]);
  } else {
    addIf(sense, cond);
  }
}

/// Add an if instruction with the given sense and condition
///
void AbcBuilder::addIf(bool sense, Def* cond) {
  IfInstr* instr = factory_.newIfInstr(cond, num_vars_, never_def_);
  setFrameArgs(instr);
  builder_.addInstr(instr);
  addArm(1, instr->arm(sense));
  addArm(0, instr->arm(!sense));
}

/// helper - pass set block end args to frame state
///
void AbcBuilder::setFrameArgs(BlockEndInstr* instr) {
  Use* args = getArgs(instr);
  assert(numArgs(instr) == num_vars_ && "arg count not sync'd to frame");

  FrameRange<Use> arg_range = frameRange(args);
  FrameRange<Def*> from_def = frameRange(frame_);
  for (; !arg_range.empty(); arg_range.popFront(), from_def.popFront())
    arg_range.front() = from_def.front();

  args[effect_pos_] = effect();
  args[state_pos_] = state();

  if (has_reachable_exceptions_) {
    // bring down the state of the locals from the label too
    FrameIndexRange fir(stackp_, scopep_, stack_base_);
    for (; !fir.empty(); fir.popFront()) {
      args[setlocal_pos_ + fir.front()] = frame_[setlocal_pos_ + fir.front()];
    }
  }
}

/// Map the given successor of the current abc_block_
/// to a given ArmInstr and current stack/scope chain.
///
void AbcBuilder::addArm(int i, ArmInstr* arm, bool switch_arm) {
  AbcBlock* to_block = abc_block_->succ_blocks[i];
  AbcBlock* current_block = abc_block_;

  if (shouldSpeculate() && !switch_arm) {
    // -4 because end is AFTER the jmp instruction.
    // Our map uses the actual branch instruction address.
    int start_pc = int(current_block->end - 4 - abc_->code_pos());
    int target_pc = int(to_block->start - abc_->code_pos());
    MethodProfile* profile = JitManager::getProfile(method_);
    double branch_probability = profile->getBranchProbability(start_pc,
                                                              target_pc);
    profiled_info_->addBranchProbability(arm, branch_probability);
  }

  if (to_block->num_preds > 1 || abc_block_ == to_block) {
    // Target block either has or will need a label, so create an empty
    // block for this arm, ending in a goto.
    builder_.addInstr(arm);
    set_effect(&arm->params[effect_pos_]);
    addGoto(to_block);
  } else {
    // Target block's only predecessor is the CondInstr that owns this arm.
    assert(!to_block->label);
    ir_->addBlock(arm);
    to_block->label = arm;
    to_block->start_sp = stackp_;
    to_block->start_scopep = scopep_;
  }
}

/// Handle a switch statement with the given selector and case count
///
void AbcBuilder::switchStmt(uint32_t num_cases, Def* sel) {
  const Type* sel_type = type(sel);
  if (isConst(sel_type)) {
    // sel is already known to be const, replace switch with goto
    int sel_value = int(intVal(sel_type));
    uint32_t succ = min(uint32_t(sel_value), num_cases);
    addGoto(abc_block_->succ_blocks[succ]);
    return;
  }
  AbcBlock** succs = abc_block_->succ_blocks;
  while (num_cases > 0 && succs[num_cases] == succs[num_cases - 1])
    --num_cases;
  addSwitch(num_cases, sel);
}

/// Add a switch statement with the given index expression and cases.
///
void AbcBuilder::addSwitch(uint32_t num_cases, Def* sel) {
  SwitchInstr* instr = factory_.newSwitchInstr(sel, num_cases, num_vars_,
                                               never_def_);
  setFrameArgs(instr);
  builder_.addInstr(instr);
  bool switch_arm = true;
  for (uint32_t i = 0; i < num_cases; i++)
    addArm(i, instr->case_arm(i), switch_arm);
  addArm(num_cases, instr->default_arm(), switch_arm);
}

/// Add a goto instruction and set up its target block
/// if encountering it for the first time.
///
void AbcBuilder::addGoto(AbcBlock* to_block) {
  LabelInstr* label = ensureBlockLabel(to_block);
  GotoInstr* go = factory_.newGotoStmt(label, never_def_);
  setFrameArgs(go);
  builder_.addInstr(go);
}

/// helper - check/initialize an AbcBlock's LabelInstr
///
CatchBlockInstr* AbcBuilder::ensureCatchBlockLabel(AbcBlock* abc_block) {
  stackp_ = abc_block->start_sp = stack_base_;
  scopep_ = abc_block->start_scopep = scope_base_ - 1;
  withbase_ = abc_block->start_withbase;
  assert(withbase_ == -1 || withbase_ > 0);
  if (withbase_ != -1) {
    withbase_ += scope_base_;
  }

  // set up the label with param capacity for the current frame
  CatchBlockInstr* catch_block = factory_.newCatchBlockInstr(num_vars_);
  catch_block->vpc = int(abc_block->start - code_pos_);

  for (int i = 0; i < num_vars_; ++i) {
    new (&catch_block->params[i]) Def(catch_block, lattice_.void_type);
  }

  setType(&catch_block->params[effect_pos_], EFFECT);
  setType(&catch_block->params[state_pos_], STATE);

  abc_block->label = catch_block;
  ir_->addBlock(catch_block);

  FrameRange<Def> p = frameRange(catch_block->params);
  FrameRange<const Type*> t = abc_block->startTypesRange(stack_base_);
  for (; !t.empty(); t.popFront(), p.popFront()) {
    if (t.front() != NULL) {
      // The model for the exception edges must be Atom for now to
      // ensure consistent representations.  It could be relaxed if
      // all exception edges had the same model.
      new (&p.front()) Def(catch_block, lattice_.makeAtom(t.front()));
    }
  }
  
  if (has_reachable_exceptions_) {
    // these start out simply as state
    FrameIndexRange fir(stackp_, scopep_, stack_base_);
    for (; !fir.empty(); fir.popFront()) {
      setType(&catch_block->params[setlocal_pos_ + fir.front()], STATE);
    }
  }

  linkExceptionEdge(ir_->begin, catch_block);
  return catch_block;
}

void AbcBuilder::linkExceptionEdge(BlockStartInstr* block, CatchBlockInstr* catch_block) {
  BlockEndInstr* end = InstrGraph::blockEnd(block);
  ExceptionEdge* edge = new (alloc_) ExceptionEdge(block, catch_block);
  if (enable_verbose)
    console_ << "creating exception edge i" << block->id << " -> i" << catch_block->id << "\n";
  // Link the edge
  ExceptionEdge* N = catch_block->catch_preds;
  if (!N) {
    catch_block->catch_preds = edge;
    edge->next_exception = edge->prev_exception = edge;
  } else {
    ExceptionEdge* P = N->prev_exception;
    edge->next_exception = N;
    edge->prev_exception = P;
    N->prev_exception = edge;
    P->next_exception = edge;
  }
  if (end->catch_blocks == NULL) {
    end->catch_blocks = new (alloc_) SeqBuilder<ExceptionEdge*>(alloc_);
  }
  end->catch_blocks->add(edge);
}

/// helper - check/initialize an AbcBlock's LabelInstr
///
LabelInstr* AbcBuilder::ensureBlockLabel(AbcBlock* abc_block) {
  BlockStartInstr* block_start = abc_block->label;
  if (!block_start) {
    // set up the label with param capacity for the current frame
    LabelInstr* label = factory_.newLabelInstr(num_vars_);
    abc_block->label = label;
    abc_block->start_sp = stackp_;
    abc_block->start_scopep = scopep_;
    ir_->addBlock(label);
    return label;
  }
  // sanity check abcbuilder state vs. label state
  assert(stackp_ == abc_block->start_sp);
  assert(scopep_ == abc_block->start_scopep);
  assert(withbase_ == abc_block->start_withbase || 
         withbase_ == abc_block->start_withbase + scope_base_);
  return cast<LabelInstr>(block_start);
}

/** Insert a timeout check.  If cktimeout returns true then we branch
 * to a throw.  In reality, cktimeout itself will throw, but this way
 * infinite loops have an exit path, which our IR requires
 */
void AbcBuilder::cktimeout() {
  Def* b = unaryStmt(HR_cktimeout, env_param());
  IfInstr* instr = factory_.newIfInstr(b, 1, effect());
  builder_.addInstr(instr);

  // one arm passes b to throw label
  ArmInstr* throw_arm = instr->arm(1);
  builder_.addInstr(throw_arm);
  set_effect(&throw_arm->params[0]);
  throwStmt(b);

  // other arm falls through to the first loop body block
  ArmInstr* loop_arm = instr->arm(0);
  builder_.addInstr(loop_arm);
  set_effect(&loop_arm->params[0]);
}

/// add a jump statement to the given target
///
void AbcBuilder::jumpStmt() {
  addGoto(abc_block_->succ_blocks[0]);
}

Def* AbcBuilder::newcatchStmt(uint32_t handler_id) {
  Traits* catch_traits =
      method_->abc_exceptions()->exceptions[handler_id].scopeTraits;
  return unaryStmt(HR_newcatch, traitsConst(catch_traits));
}

void AbcBuilder::setslotStmt(uint32_t slot, Def *obj, Def *val) {
  Def* args[] = { val };
  callStmt2(HR_abc_setslot, ordinalConst(slot), obj, args, 1);
}

Def* AbcBuilder::getslotStmt(Def *obj, uint32_t slot) {
  return callStmt2(HR_abc_getslot, ordinalConst(slot), obj,
                  0, 0);
}

Def* AbcBuilder::getouterscopeStmt(uint32_t scope_index) {
  Def* index = ordinalConst(scope_index);
  return binaryExpr(HR_getouterscope, index, env_param());
}

/// Build and add a hasnext2 stmt.  Obj and ctr are in/out parameters.
///
Def* AbcBuilder::hasnext2Stmt(Def** obj, Def** ctr) {
  Hasnext2Stmt* stmt = factory_.newHasnext2Stmt(effect(), *obj, *ctr);
  builder_.addInstr(stmt);
  set_effect(stmt->effect_out());
  *obj = stmt->object_out();
  *ctr = stmt->counter_out();
  return stmt->value_out();
}

/// Start processing this abc_block.
///
void AbcBuilder::startBlock(AbcBlock* abc_block) {
  // set abcbuilder state to this block
  abc_block_ = abc_block;
  stackp_ = abc_block->start_sp;
  scopep_ = abc_block->start_scopep;
  withbase_ = abc_block->start_withbase;
  assert(withbase_ == -1 || withbase_ > 0);
  if (withbase_ != -1) {
    withbase_ += scope_base_;
  }
  BlockStartInstr* label = abc_block->label;
  assert(label);

  if (abc_block->dfs_loop) {
    // Set label params to pessimistic types from CodegenDriver
    // since we have not seen all predecessors yet.
    FrameRange<Def> p = frameRange(label->params);
    FrameRange<const Type*> t = abc_block->startTypesRange(stack_base_);
    for (; !t.empty(); t.popFront(), p.popFront())
      setType(&p.front(), t.front());
    setType(&label->params[effect_pos_], EFFECT);
    setType(&label->params[state_pos_], STATE);

    if (has_reachable_exceptions_) {
      // these start out simply as state
      FrameIndexRange fir(stackp_, scopep_, stack_base_);
      for (; !fir.empty(); fir.popFront()) {
        setType(&label->params[setlocal_pos_ + fir.front()], STATE);
      }
    }

  } else {
    // Compute valid types for label params.
    builder_.computeType(label);
  }
  
  set_effect(&label->params[effect_pos_]);
  set_state(&label->params[state_pos_]);

  FrameRange<Def> from = frameRange(label->params);
  FrameRange<Def*> to = frameRange(frame_);
  for (; !from.empty(); from.popFront(), to.popFront())
    to.front() = &from.front();

  if (has_reachable_exceptions_) {
    // bring down the state of the locals from the label too
    FrameIndexRange fir(stackp_, scopep_, stack_base_);
    for (; !fir.empty(); fir.popFront()) {
      frame_[setlocal_pos_ + fir.front()] = &label->params[setlocal_pos_ + fir.front()];
    }
  }

  builder_.setPos(label);
  if (enable_verbose)
    printStartState(abc_block);

  assert(checkFrame(frame_, stackp_, scopep_));

  // Ignore the global configuration flag for timeouts because it can lead to
  // IR graphs with no ends, if the graph has an infinite loop.  We require
  // builtin code not to have infinite loops.
  if (abc_block->abc_loop && !method_->isNonInterruptible())
    cktimeout();
}

void AbcBuilder::printStartState(AbcBlock* abc_block) {
  console_ << (abc_block->dfs_loop ? "LOOP:" : "BLOCK:") << " ["
      << int(abc_block->start - code_pos_) << "-"
      << int(abc_block->end - code_pos_) << "] effect=";
  if (abc_block->label)
    printInstr(console_, abc_block->label);
}

void AbcBuilder::printFrameState() {
  // locals
  console_ << "[ ";
  for (int i = 0; i < scope_base_; i++) {
    printDef(console_, frame_[i]);    
    console_ << ":" << typeName(frame_[i]);
    if (i+1 < scope_base_)
      console_ << ' ';
  }
  console_ << " ] {";
  
  // scope chain
  for (int i = scope_base_; i < scopep_ + 1; i++) {
    printDef(console_, frame_[i]);
    console_ << ":" << typeName(frame_[i]);
    if (i+1 < stack_base_)
      console_ << ' ';
  }
  console_ << " } ( ";
  
  // stack
  for (int i = stack_base_; i < stackp_ + 1; i++) {
    printDef(console_, frame_[i]);
    console_ << ":" << typeName(frame_[i]);
    if (i+1 < framesize_)
      console_ << ' ';
  }
  console_ << " ) effect ";

  printDef(console_, effect());
  console_ << ":" << typeName(effect());

  console_ << " withbase_ " << withbase_;

  console_ << "\n";
  
}

} // namespace avmplus

#endif // VMCFG_HALFMOON
