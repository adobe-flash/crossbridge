/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {

InstrKind tomodelPunt() {
  assert(false && "cant convert models");
  return HR_MAX;
}

InstrKind toModelAtom(const Type* t) {
  switch (model(t)) {
    default: assert(false && "bad model for toModelAtom");
    case kModelString:       return HR_string2atom;
    case kModelDouble:       return HR_double2atom;
    case kModelScriptObject: return HR_scriptobject2atom;
    case kModelNamespace:    return HR_ns2atom;
    case kModelInt:
      return isBoolean(t) ? HR_bool2atom :
             isInt(t) ? HR_int2atom :
             isUInt(t) ? HR_uint2atom : tomodelPunt();
  }
}

InstrKind toModelDouble(const Type* t) {
  switch (model(t)) {
    default: assert(false && "bad model for toModelDouble");
  case kModelAtom: return HR_atom2double;
    case kModelInt:
      return isInt(t) ? HR_i2d :
             isUInt(t) ? HR_u2d :
             HR_MAX;
  }
}

InstrKind toModelInt(const Type* t, const Type* use_type) {
  // todo: double->int
  switch (model(t)) {
    default: assert(false && "bad input model for toModelInt");
    case kModelAtom: {
      if (isBoolean(t)) {
        return HR_atom2bool;
      } else {
        /// This must be bad - example src: var x:uint = 10;
        /// The type is a uiNumber[a]. the use_type says x MUST be the uint type
        /// So we have to change the model from atom to uint
        /// but uint and int share the same model kModelInt, BUT are unique types
        /// and uint / int are not subtypes of each other
        /// so we have to be explicit in which int we choose based on the use_type
        assert (isInt(t) || isUInt(t));
        if (isUInt(use_type)) {
          assert (!isInt(use_type));
          return HR_atom2uint;
        } else {
          assert (isInt(use_type) && !isUInt(use_type));
          return HR_atom2int;
        }
      }
    }  // end case atom
    case kModelDouble:
      if (isInt(t))
        return HR_d2i;
      if (isUInt(t))
        return HR_d2u;
      assert(false && "nonsense -> int");
      break;
  } // end switch
  return HR_MAX;
}

InstrKind toModelScriptobject(const Type* t) {
  switch (model(t)) {
  default:
    assert(false && "bad input model for toModelScriptobject");
  case kModelAtom:
    return HR_atom2scriptobject;
  }
}

InstrKind toModelKind(const Type* val_type, const Type* use_type) {
  switch (model(use_type)) {
    default:
      printf("unknown conversion %s -> %s\n", typeName(val_type),
             typeName(use_type));
      assert(false && "bad model");
    case kModelNamespace:
      return HR_atom2ns;
    case kModelAtom:
      return toModelAtom(val_type);
    case kModelScriptObject:
      return toModelScriptobject(val_type);
    case kModelString:
      return HR_atom2string;
    case kModelInt:
      return toModelInt(val_type, use_type);
    case kModelDouble:
      return toModelDouble(val_type);
  }
}

/// ModelFixer encapsulates state and helpers for inserting model-changing
/// instructions into the IR.
class ModelFixer {
public:
  ModelFixer(Context* cxt, InstrGraph* ir)
  : cxt_(cxt), ir_(ir), factory_(ir), analyzer_(ir) {
  }

  /** change opcodes when possible */
  void changeOps();

  /** insert conversions after defs */
  void fixDefs();

  /// Return the constraint for this use.  Instructions that have Any or
  /// Top in their signature get handled here.
  const Type* getConstraint(const Use& u);

private:
  Context* cxt_;
  InstrGraph* ir_;
  InstrFactory factory_;
  TypeAnalyzer analyzer_;
};

/**
 * Run a pass where we select alternative opcodes to reduce the number
 * of model conversions we'll have to insert.
 */
void ModelFixer::changeOps() {
  for (AllInstrRange i(ir_); !i.empty();) {
    Instr* instr = i.popFront();
    switch (kind(instr)) {
      case HR_cknull: {
        UnaryStmt* cknull = cast<UnaryStmt>(instr);
        const Type* ptr_type = type(cknull->value_in());
        if (model(ptr_type) == kModelScriptObject) {
          // cknull(ptr:ScriptObject) -> cknullobject(ptr)
          factory_.toUnaryStmt(HR_cknullobject, cknull);
          analyzer_.computeTypes(cknull);
        }
        break;
      }
    }
  }
}

/**
 * Insert model conversions after defs, where needed.  If multiple uses
 * need to use the same converted def, reuse the conversion instruction.
 */
void ModelFixer::fixDefs() {
  const int kConvertsMax = 4;
  Def* converts[kConvertsMax];  // space to memoize one conversion per ModelKind.
  InstrKind converts_kind[kConvertsMax];  // verify that the conversion is the same
  for (PostorderBlockRange b(ir_); !b.empty();) {
    for (InstrRange i(b.popFront()); !i.empty();) {
      Instr* instr = i.popBack();
      SigRange sr = outputSigRange(instr);
      for (ArrayRange<Def> dr = defRange(instr); !dr.empty(); sr.popFront()) {
        Def* d = &dr.popFront();
        int have_count = 0;
        const Type* def_type = type(d);
        const Type* sig_type = sr.front();
        if (isBottom(def_type))
          def_type = sig_type;
        for (UseRange u(*d); !u.empty(); ) {
          Use& use = u.popFront();
          const Type* constraint = getConstraint(use);
          if (!submodelof(def_type, constraint)) {
            // need a conversion.  The same model may require different conversions
            InstrKind convert_kind = toModelKind(def_type, constraint);
            int h = 0;
            for (; h < have_count; h++) {
              if (converts_kind[h] == convert_kind)
                break;
            }
            if (h >= have_count) {
              UnaryExpr* expr = factory_.newUnaryExpr(convert_kind, d);
              ir_->addInstrAfter(instr, expr);
              converts[h] = expr->value_out();
              converts_kind[h] = convert_kind;
              have_count++;
              assert(have_count <= kConvertsMax);
            }
            use = converts[h];
          }
        }
      }
    }
  }
}

/// Return the constraint for this use.  Instructions that have Any or
/// Top in their signature get handled here.
const Type* ModelFixer::getConstraint(const Use& u) {
  Instr* instr = user(u);
  int use_pos = pos(u);
  InstrKind k = kind(instr);
  switch (k) {
    case HR_goto: {
      // constraint is the corresponding label's def type.
      GotoInstr* go = cast<GotoInstr>(instr);
      return type(go->target->params[use_pos]);
    }
    case HR_if:
    case HR_switch:
      if (use_pos > 0)
        return TOP;
      break;
    case HR_setslot:
      if (use_pos == 3) {
        CallStmt2* setslot = cast<CallStmt2>(instr);
        const Type* obj_type = type(setslot->object_in());
        int slot = ordinalVal(type(setslot->param_in()));
        return ir_->lattice.getSlotType(obj_type, slot);
      }
      break;
    case HR_callinterface:
    case HR_callmethod:
    case HR_callstatic:
      if (use_pos >= 2) {
        CallStmt2* call = cast<CallStmt2>(instr);
        const Type* env_type = type(call->param_in());
        if (isEnv(env_type)) {
          MethodSignaturep sig = getMethod(type(call->param_in()))->getMethodSignature();
          return ir_->lattice.makeParamType(use_pos - 2, sig);
        }
      }
      break;
    case HR_return:
      if (use_pos == 1) {
        // return type is based on method signature
        MethodSignaturep sig = cxt_->method->getMethodSignature();
        return ir_->lattice.makeType(sig->returnTraits());
      }
      break;
  }
  const ShapeRep& rep = shape_reps[shape(instr)];
  if (rep.vararg == kVarIn && use_pos > rep.num_uses)
    use_pos = rep.num_uses;
  return inputSignature(instr)[use_pos];
}

ModelKind promoteModelKind(const Type* t) {
  if (isInt(t))
    return kModelInt;
  if (isUInt(t))
    return kModelInt;
  if (isNumber(t))
    return kModelDouble;
  if (isBoolean(t))
    return kModelInt;
  if (isString(t))
    return kModelString;
  if (isNamespace(t))
    return kModelNamespace;
  if (isScriptObject(t))
    return kModelScriptObject;
  return kModelAtom;
}

/// Run the model fixer once then clean up.
void fixModels(Context* cxt, InstrGraph* ir) {
  assert(checkTypes(ir, false));
  assert(checkPruned(ir) && checkSSA(ir));
  propagateTypes(ir);
  ModelFixer fixer(cxt, ir);
  fixer.changeOps();
  fixer.fixDefs();
  propagateTypes(ir);
  computeIdentities(ir);
  removeDeadCode(cxt, ir);
  assert(checkTypes(ir, true));
  assert(checkPruned(ir) && checkSSA(ir));
}

ModelKind defaultModelKind(Traits* traits) {
  switch (valueStorageType(builtinType(traits))) {
    default: assert(false && "bad SlotStorageType");
    case SST_atom: return kModelAtom;
    case SST_scriptobject: return kModelScriptObject;
    case SST_string: return kModelString;
    case SST_namespace: return kModelNamespace;
    case SST_int32: return kModelInt;
    case SST_uint32: return kModelInt;
    case SST_bool32: return kModelInt;
    case SST_double: return kModelDouble;
  }
}

SlotStorageType type2sst(const Type* t) {
  assert(isDataType(*t));
  switch (model(t)) {
    default: assert(false && "bad model");
    case kModelInt:
      return isInt(t) ? SST_int32 : isUInt(t) ? SST_uint32 : SST_bool32;
    case kModelDouble: return SST_double;
    case kModelString: return SST_string;
    case kModelScriptObject: return SST_scriptobject;
    case kModelAtom: return SST_atom;
    case kModelNamespace: return SST_namespace;
  }
}

}
#endif // VMCFG_HALFMOON
