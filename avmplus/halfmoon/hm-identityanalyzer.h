/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef IDENTITYANALYZER_H_
#define IDENTITYANALYZER_H_

namespace halfmoon {

/// IdentityAnalyzer computes identities for each kind of instruction.
/// A def of an instruction has an identity def, if that def's value is always
/// the same as one of the instruction's inputs.
///
/// Instructions are not mutated or created, and this analyzer does not have
/// any global knowlege.  Identities are found just by looking at adjacent
/// instructions.
///
class IdentityAnalyzer: public KindAdapter<IdentityAnalyzer, Def*> {
public:
  IdentityAnalyzer(Def* def, Lattice* lattice)
  : def_(def)
  , lattice_(lattice)
  {}

  Def* identity(Instr*);

public: // dispatch() adapter methods.
  Def* do_default(Instr*) { return def_; }
  Def* do_arm(ArmInstr*);
  Def* do_label(LabelInstr*);
  Def* do_toprimitive(UnaryStmt*);
  Def* do_coerce(BinaryStmt*);
  Def* do_castobject(UnaryExpr* i) { return coerceIdentity(i, lattice_->object_type[kTypeNullable]); }
  Def* do_caststring(UnaryStmt* i) { return coerceIdentity(i, lattice_->string_type[kTypeNullable]); }
  Def* do_tonumber(UnaryStmt* i) { return coerceIdentity(i, lattice_->double_type); }
  Def* do_toint(UnaryStmt* i) { return coerceIdentity(i, lattice_->int_type); }
  Def* do_touint(UnaryStmt* i) { return coerceIdentity(i, lattice_->uint_type); }
  Def* do_toboolean(UnaryExpr* i) { return coerceIdentity(i, lattice_->boolean_type); }
  Def* do_doubletoint32(UnaryExpr* i) { return coerceIdentity(i, lattice_->int_type); }
  Def* do_abc_findproperty(NaryStmt3* i) { return doFindStmt(i); }
  Def* do_abc_findpropstrict(NaryStmt3* i) { return doFindStmt(i); }
  Def* do_atom2scriptobject(UnaryExpr* i) { return doModelChange(i, HR_scriptobject2atom); }
  Def* do_cast(BinaryStmt*);
  Def* do_cknull(UnaryStmt*);
  Def* do_cknullobject(UnaryStmt* i) { return do_cknull(i); }
  Def* do_u2i(UnaryExpr*);
  Def* do_speculate_number(BinaryExpr*);

private:
  /// Generic analyzer for lexical 'find' instructions.
  ///
  Def* doFindStmt(NaryStmt3*);

  /// Helper method: return the appropriate identity def based on which
  /// use value_in is, and what def_ is pointing to.
  ///
  Def* identity(BinaryStmt*, Def* value_in);
  Def* identity(UnaryStmt*, Def* value_in);
  Def* identity(NaryStmt3*, Def* value_in);

  /// Helper method: return UnaryStmt.effect_in or value_in based on
  /// what def_ is pointing to.
  ///
  Def* identity(UnaryStmt*);

  /// Identity for coerce opcodes
  ///
  Def* coerceIdentity(UnaryStmt*, const Type* to_type);
  Def* coerceIdentity(UnaryExpr*, const Type* to_type);
  Def* doModelChange(UnaryExpr*, InstrKind);

private:
  Def* const def_;        // The existing def_ we are analyzing.
  Lattice* const lattice_;
};

} // namespace halfmoon
#endif // IDENTITYANALYZER_H_
