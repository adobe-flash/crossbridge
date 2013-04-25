/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using namespace avmplus;

// Interpreter concept.
//
// We interpret the instr graph directly without a schedule.  Instructions
// are visited in order within blocks; the code must have previously been
// scheduled into some legal order (defs dominate uses, etc).
//
// We need a place for a value for each result of each instr, not just one
// per instr.  A single result per instr would be simpler.  We will have this
// problem for translation to LIR too.  We could precompute the size & layout
// of this array once.
//
// option 1 - (rejected) walk the IR directly.  Rejected because the data
// nodes must be evaluated in some sequential order, because we are on a
// sequential computer.  Whether we record the order (using a scheduler) or
// compute it on the fly in an iterator, it is scheduling.  So, require a
// schedule.
//
// option 2 - (rejected) compute DFS of all instructions in linear list.
// Rejected because computing the DFS order amounts to scheduling.
//
// option 3 - schedule, then walk the cfg.
//
// Any legal schedule is fine.  Need a mapping of instr to block,
// which we compute during scheduling (easy).  Since the schedule places
// labels, gotos, statements, expressions, and branches in the right order, we
// just evaluate instructions when visited and store results.
//
// Since any schedule will work, and since any other output phase would have
// to schedule, this is a way of checking schedules.

InterpreterData::InterpreterData(Allocator& alloc, InstrGraph* ir)
: max_argc_(0), ir_(ir) {
  Allocator0 alloc0(alloc);
  for (AllInstrRange i(ir); !i.empty(); i.popFront()) {
    Instr* instr = i.front();
    int argc = numUses(instr);
    max_argc_ = argc > max_argc_ ? argc : max_argc_;
  }
}

bool checkValues(Interpreter& frame, Instr* instr) {
  for (ArrayRange<Def> r = defRange(instr); !r.empty(); r.popFront())
    if (r.front().isUsed())
      frame.get(&r.front());
  return true;
}

void printInstr(Interpreter* frame, Instr* instr) {
  if (enable_trace) {
    if (isBlockStart(instr))
      return;
    frame->console() << "X ";
    printInstr(frame->console(), instr);
  }
}

/**
 * Run the CFG interpreter for the given method.  This implements the
 * same signature as all AS3 methods but returns a Val instead of a word
 * or double.  It is called from each of the ABI-specific stubs.
 */
Value Interpreter::stub(MethodEnv* env, int32_t argc, uint32_t* args) {
  MMgc::GC::AllocaAutoPtr vals_autoptr;
  MMgc::GC::AllocaAutoPtr args_autoptr;
  MMgc::GC::AllocaAutoPtr state_autoptr;
  const InterpreterData& interpreter = JitManager::interpreter(env);
  int num_values = interpreter.num_values();
  AvmCore* core = env->core();
  Value* values = (Value*) avmStackAlloc(core, vals_autoptr,
                                         num_values * sizeof(Value));
  void* argsout = avmStackAlloc(core, args_autoptr,
                                interpreter.max_argc_ * sizeof(Value));
  int frame_size = env->method->getMethodSignature()->frame_size();
  Value* saved_state = (Value*) avmStackAlloc(core, state_autoptr,
                                              frame_size * sizeof(Value*));
  core->stackCheck(env);

  Interpreter frame(interpreter, env, argc, args, values, (uint32_t*)argsout,
                    saved_state);

  for (BlockStartInstr* blk = interpreter.ir_->begin; true;
      blk = frame.next_block_) {
    // process the block, ordinary instructions, and branch.
    for (InstrRange i(blk); !i.empty(); i.popFront()) {
      Instr* instr = i.front();
      printInstr(&frame, instr);
      frame.next_block_ = 0;
      do_instr(&frame, instr);
      assert(checkValues(frame, instr));
    }
    Instr* branch = InstrGraph::blockEnd(blk);
    if (kind(branch) == HR_return) {
      StopInstr* return_instr = cast<StopInstr>(branch);
      return frame.get(return_instr->value_in());
    }
  }
}

uintptr_t Interpreter::intStub(MethodEnv* env, int32_t argc, uint32_t* args) {
  // sign extend int->intptr, then cast to uintptr
  return (intptr_t)stub(env, argc, args).int_;
}

uintptr_t Interpreter::uintStub(MethodEnv* env, int32_t argc, uint32_t* args) {
  // cast int32->uint32, then 0-extend to uintptr
  return (uint32_t)stub(env, argc, args).atom_;
}

uintptr_t Interpreter::gprStub(MethodEnv* env, int32_t argc, uint32_t* args) {
  return stub(env, argc, args).atom_;
}

double Interpreter::fprStub(MethodEnv* env, int32_t argc, uint32_t* args) {
  return stub(env, argc, args).double_;
}

void Interpreter::do_default(Instr* instr) {
  InstrKind k = kind(instr);
  if (k < Stubs::stub_count && stub_table[k])
    return stub_table[k](this, instr);
  if (enable_verbose) {
    printf("interpreter fails because on not implemented instruction:\n");
    fflush(NULL);
    printInstr(this, instr);
  }
  assert(false && "interpreter instruction not implemented");
}

Value paramToValue(const ParamIter& p) {
  switch (p.model()) {
    default:
      assert(false && "unknown param representation");
    case kModelAtom:
     return AtomValue(p.asAtom());
    case kModelScriptObject:
      return Value(p.asObject());
    case kModelString:
      return Value(p.asString());
    case kModelNamespace:
      return Value(p.asNamespace());
    case kModelInt:
      return Value(p.asInt());
    case kModelDouble:
      return Value(p.asDouble());
  }
}

/**
 * Convert a default value Atom to an interpreter value.
 */
Value atomToValue(Atom value, const ParamIter& p) {
  switch (p.model()) {
  default:
    assert(false && "unknown model type");
  case kModelScriptObject:
    return Value(AvmCore::isNull(value) ? (ScriptObject*)0 :
        AvmCore::atomToScriptObject(value));
  case kModelAtom:
    return AtomValue(value);
  case kModelDouble:
    return Value(AvmCore::number_d(value));
  case kModelInt:
    switch (p.builtinType()) {
      default: assert(false && "bad builtinType");
      case BUILTIN_int: return Value(AvmCore::integer_i(value));
      case BUILTIN_uint: return Value(AvmCore::integer_u(value));
      case BUILTIN_boolean: return Value(atomGetBoolean(value));
    }
  case kModelString:
    return Value(AvmCore::isNull(value) ? (String*) 0 :
        AvmCore::atomToString(value));
  case kModelNamespace:
    return Value(AvmCore::isNull(value) ? (Namespace*) 0 :
        AvmCore::atomToNamespace(value));
  }
}

void Interpreter::do_start(StartInstr* start) {
  assert(!env_->method->needArguments() && "arguments not supported yet");
  assert(!isThisBoxed() && "unboxThis() not implemented");
  // start signature for an as3 method is:
  // [effect, env, this, arg, ...].  The data_param() accessor
  // accesses [env, this, arg, ...].
  // + 3 because we have effect, env, this, then real args
  assert(numDefs(start) == signature()->param_count() + 3 +
         (env_->method->needRestOrArguments() ? 1 : 0));
  int i = 0;
  resultVal(start->data_param(i++)) = env_;
  ParamIter p(signature(), args_in_);
  for (; !p.empty(); p.popFront(), ++i) {
    // data_param[0] is env, so offset by 1
    if (i > argc_ + 1) {
      assert(signature()->optional_count() &&
             "interpreter invoked with too many arguments");
      // -2, 1 because i counts env and this; optional_index starts at 0.
      int optional_index = i - 2 - signature()->requiredParamCount();
      resultVal(start->data_param(i)) =
          atomToValue(signature()->getDefaultValue(optional_index), p);
    } else {
      resultVal(start->data_param(i)) = paramToValue(p);
    }
  }

  if (env_->method->needRest()) {
    resultVal(start->data_param(i++)) = env_->createRestHelper(argc_, args_in_);
  }
}

void Interpreter::do_arm(ArmInstr*) {
  // nothing to do.
}

void Interpreter::do_label(LabelInstr*) {
  // nothing to do.
}

/// helper: jump from block end to block start, passing arguments along.
/// Use a temp array to guarantee simultaneous copies; see test/swap.as
void Interpreter::jump(BlockEndInstr* end, BlockStartInstr* start) {
  assert(numArgs(end) == numDefs(start));
  Allocator scratch;
  int argc = numArgs(end);
  Value* temp = new (scratch) Value[argc];
  ArrayRange<Use> args(argRange(end));
  for (ArrayRange<Value> t(temp, argc); !t.empty(); t.popFront(), args.popFront())
    t.front() = get(args.front());
  ArrayRange<Def> defs(defRange(start));
  for (ArrayRange<Value> t(temp, argc); !t.empty(); t.popFront(), defs.popFront())
    resultVal(&defs.front()) = t.front();
  next_block_ = start;
}

void Interpreter::do_goto(GotoInstr* go) {
  jump(go, go->target);
}

void Interpreter::do_if(IfInstr* instr) {
  jump(instr, instr->arm(getBool(instr->cond()) == kBoolTrue));
}

void Interpreter::do_switch(SwitchInstr* instr) {
  jump(instr, instr->arm(getInt(instr->selector())));
}

void Interpreter::do_const(ConstantExpr* instr) {
  Value& result = resultVal(instr->value());
  const Type* t = type(instr->value());
  switch (kind(t)) {
    case kTypeName:
      result = Value(nameVal(t));
      break;
    case kTypeOrdinal:
      result = OrdValue(ordinalVal(t));
      break;
    case kTypeMethod:
      result = Value(methodVal(t));
      break;
    case kTypeTraits:
      result = Value(traitsVal(t));
      break;
    default:
      switch (model(t)) {
        case kModelScriptObject:
          result = Value(objectVal(t));
          break;
        case kModelAtom:
          result = AtomValue(atomVal(t, core()));
          break;
        case kModelNamespace:
          result = Value(nsVal(t));
          break;
        case kModelString:
          result = Value(stringVal(t));
          break;
        case kModelInt:
          result = isBoolean(t) ? Value(boolVal(t)) :
                   isInt(t) ? Value(intVal(t)) :
                   Value(uintVal(t));
          break;
        case kModelDouble:
          result = Value(doubleVal(t));
          break;
        default:
          assert(false && "Unsupported const type");
      }
  }
}

/// TODO use resolved stuff if available
///
void Interpreter::do_callmethod(CallStmt2* instr) {
  MethodEnv* callee_env = getEnv(instr->param_in());
  doCall(callee_env, instr->arg_count(), instr->args(), instr->value_out());
}

void Interpreter::do_callinterface(CallStmt2* instr) {
  // Signature_info is the interface method info
  MethodInfo* interface_info = halfmoon::getMethod(type(instr->param_in())); 

  // callee_env is the CONCRETE dispatched method info
  MethodEnv* callee_env = getEnv(instr->param_in());
  doCallInterface(interface_info, callee_env, instr->arg_count(), 
    instr->args(), instr->value_out());
}

void Interpreter::coerceArgs(MethodSignaturep method_sig, int actual_argc_out,
                  const Use* call_args) {
  ParamIter p(method_sig, args_out_);
  for (int i = 0; i < actual_argc_out; ++i, p.popFront()) {
    const Use& arg = call_args[i];
    assert(p.model() == model(type(arg)) && "Signature model doesn't match arg model");
    const Type* t = type(arg);
    switch (model(t)) {
      default:
        assert(false && "Unsupported param type");
      case kModelScriptObject:
        p.asObject() = getObject(arg);
        break;
      case kModelNamespace:
        p.asNamespace() = getNs(arg);
        break;
      case kModelString:
        p.asString() = getString(arg);
        break;
      case kModelAtom:
        p.asAtom() = getAtom(arg);
        break;
      case kModelDouble:
        p.asDouble() = getDouble(arg);
        break;
      case kModelInt:
        if (isInt(t))
          p.asIntptr() = (intptr_t)getInt(arg); // sign-extend
        else
          p.asIntptr() = (uintptr_t)(uint32_t)getInt(arg); // zero-extend
        break;
    }
  }
}

void Interpreter::doCallInterface(MethodInfo* interface_info, MethodEnv* callee_env, 
                         int actual_argc_out, const Use* call_args, Def* value_out) {
  coerceArgs(interface_info->getMethodSignature(), actual_argc_out, call_args);
  Value& result = resultVal(value_out);
  // AVM calling conventions don't count the 'this' argument.
  int argc = actual_argc_out - 1;
  const Type* t = type(value_out);
  ModelKind k = isBottom(t) ? kModelInvalid : model(t);
  uintptr_t iid = JitFriend::getIID(interface_info);

  ImtThunkEnv* interfaceEnv = (ImtThunkEnv*) callee_env;
  if (k != kModelDouble) {
    GprImtThunkProc function = JitFriend::envImplImtGpr(callee_env);
    uintptr_t retVal = function(interfaceEnv, argc, args_out_, iid);

    switch (k) {
      default:
        printf("ModelKind is: %d\n", k);
        assert(false && "Unsupported return type");
      case kModelInvalid: // return type was _, i.e. no value
        break;
      case kModelScriptObject:
        result = (ScriptObject*) retVal;
        break;
      case kModelString:
        result = (String*) retVal;
        break;
      case kModelNamespace:
        result = (Namespace*) retVal;
        break;
      case kModelAtom:
        result = AtomValue(retVal);
        break;
      case kModelInt:
        result = (int32_t) retVal;
        break;
    }
  } else {
    FprImtThunkProc function = JitFriend::envImplImtFpr(callee_env);
    double retVal = function(interfaceEnv, argc, args_out_, iid);
    result = retVal;
  }
}

void Interpreter::doCall(MethodEnv* callee_env, 
                         int actual_argc_out, const Use* call_args, Def* value_out) {
  coerceArgs(callee_env->method->getMethodSignature(), actual_argc_out, call_args);
  Value& result = resultVal(value_out);
  // AVM calling conventions don't count the 'this' argument.
  int argc = actual_argc_out - 1;
  const Type* t = type(value_out);
  ModelKind k = isBottom(t) ? kModelInvalid : model(t);
  if (k != kModelDouble) {
    GprMethodProc f = JitFriend::envImplGpr(callee_env);
    uintptr_t val = f(callee_env, argc, args_out_);
    switch (k) {
      default:
        printf("ModelKind is: %d\n", k);
        assert(false && "Unsupported return type");
      case kModelInvalid: // return type was _, i.e. no value
        break;
      case kModelScriptObject:
        result = (ScriptObject*) val;
        break;
      case kModelString:
        result = (String*) val;
        break;
      case kModelNamespace:
        result = (Namespace*) val;
        break;
      case kModelAtom:
        result = AtomValue((Atom)val);
        break;
      case kModelInt:
        result = (int32_t) val;
        break;
    }
  } else {
    FprMethodProc f = JitFriend::envImplFpr(callee_env);
    result = f(callee_env, argc, args_out_);
  }
}

void Interpreter::do_getslot(CallStmt2* instr) {
  const Use& object_in = instr->object_in();
  const Use& slot_in = instr->param_in();

  int slot = ordinalVal(type(slot_in));
  const Type* object_type = type(object_in);
  const Type* slot_type = lattice().getSlotType(object_type, slot);
  assert(subtypeof(type(instr->value_out()), slot_type));
  uint32_t offset = lattice().getSlotOffset(object_type, slot);
  Value& result = resultVal(instr->value_out());

  ScriptObject* object = getObject(object_in);
  uintptr_t ptr = uintptr_t(object) + offset;
  switch (model(slot_type)) {
    default:
      assert(false && "Unsupported model");
    case kModelAtom:
      result = AtomValue(*((Atom*) ptr));
      break;
    case kModelScriptObject:
      result = Value(*((ScriptObject**) ptr));
      break;
    case kModelString:
      result = Value(*((String**) ptr));
      break;
    case kModelNamespace:
      result = Value(*((Namespace**) ptr));
      break;
    case kModelInt:
      result = Value(*((int32_t*) ptr));
      break;
    case kModelDouble:
      result = Value(*((double*) ptr));
      break;
  }
}

void Interpreter::do_setslot(CallStmt2* instr) {
  const Use& name_in = instr->param_in();
  const Use& object_in = instr->object_in();
  const Use& value_in = instr->vararg(0);

  int slot = getOrdinal(name_in);
  const Type* value_type = type(value_in);
  const Type* object_type = type(object_in);
  assert(subtypeof(value_type, lattice().getSlotType(object_type, slot)));
  assert(submodelof(value_type, lattice().getSlotType(object_type, slot)));
  uint32_t offset = lattice().getSlotOffset(object_type, slot);

  ScriptObject* object = getObject(object_in);
  assert (object != 0);

  uintptr_t ptr = uintptr_t(object) + offset;
  switch (model(value_type)) {
    default:
      assert(false && "Unsupported rep");
    case kModelAtom:
      core()->atomWriteBarrier(core()->gc, object, (Atom*) ptr, getAtom(value_in));
      break;
    case kModelInt:
      *((int32_t*) ptr) = getInt(value_in);
      break;
    case kModelDouble:
      *((double*) ptr) = getDouble(value_in);
      break;
    case kModelString:
      WBRC(core()->gc, object, (void*)ptr, getString(value_in));
      break;
    case kModelNamespace:
      WBRC(core()->gc, object, (void*)ptr, getNs(value_in));
      break;
    case kModelScriptObject:
      WBRC(core()->gc, object, (void*)ptr, getObject(value_in));
      break;
  }
}

void Interpreter::do_speculate_number(BinaryExpr* instr) {
  Atom value = getAtom(instr->lhs_in());
  assert((atomKind(value) == kDoubleType) && "Simulate Bailout");
  resultVal(instr->value_out()) = core()->atomToDouble(value);
}

void Interpreter::do_speculate_int(BinaryExpr* instr) {
  Atom value = getAtom(instr->lhs_in());
  assert((atomKind(value) == kIntptrType) && "Simulate Bailout");
  resultVal(instr->value_out()) = core()->integer_i(value);
}

/// I don't think we actually have to generate any code for newstate?
/// It seems like a pure IR placeholder 
void Interpreter::do_newstate(ConstantExpr* instr) {
  resultVal(instr->value()) = StateValue();
}

/// Set locals are explicit stores of the AS3 stack
/// The interpreter doesn't have to do anything until a safepoint
/// actually occurs, then we'll walk the chain of setlocals
/// and emit the explicit stores.
void Interpreter::do_setlocal(SetlocalInstr* instr) {
  // I think we technically still have the same state object, just a side effect
  // occured on it.
  int stackIndex = instr->index;
  saved_state_[stackIndex] = get(instr->value_in());
  resultVal(instr->state_out()) = get(instr->state_in());
}

/// Fixme, change safepoint to also save scope stack and operand stack
/// Currently we only track locals.
void Interpreter::do_safepoint(SafepointInstr* /*instr*/) {
}

void Interpreter::do_abc_hasnext2(Hasnext2Stmt* instr) {
  Atom object = getAtom(instr->object_in());
  int counter = getInt(instr->counter_in());

  int b = env_->hasnextproto(object, counter);

  resultVal(instr->value_out()) = (b != 0);
  resultVal(instr->object_out()) = AtomValue(object);
  resultVal(instr->counter_out()) = counter;
}

} // end class

#include "generated/Stub_callers.hh"
#endif // VMCFG_HALFMOON
