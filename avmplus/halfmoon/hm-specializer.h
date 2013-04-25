/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPECIALIZER_H_
#define SPECIALIZER_H_

namespace halfmoon {

enum ScopeKind {
  kScopeNotFound,   // nothing found
  kScopeLocal, // use local_def() to get local scope value
  kScopeOuter, // use index() to get captured scope value
  kScopeDomain // use finddef.
};

/**
 * Analyzer for lexical binding of findproperty opcodes.  This function will
 * search for a binding, then return a ScopeKind result and parameter depending
 * on the kind.
 */
ScopeKind findScope(Lattice*, NaryStmt3*, int* index);

/// Analyze property bindings.
///
struct CallAnalyzer {
  CallAnalyzer(const Type* obj_type, const Type* name_type,
                   Lattice* lattice, int extra_argc);
  Binding binding;
  MethodSignaturep signature;
  MethodInfo* method;
  int disp_id; // vtable index, if is_interface == false
  bool is_interface;
};

/// Specializer:  If we can turn a general instruction into a
/// more specific one, do so by changing the instruction.  The new
/// instruction may then be subject to further optimizations.  The graph
/// connectivity of the instruction will not be changed.
///
class Specializer: public KindAdapter<Specializer, void> {
public:
  Specializer(InstrGraph* ir);

  void specialize(Instr*);

public: // dispatch() adapter methods.
  void do_default(Instr*) { }
  void do_abc_findproperty(NaryStmt3* i) { return doFindStmt(i); }
  void do_abc_findpropstrict(NaryStmt3* i) { return doFindStmt(i); }
  void do_toint(UnaryStmt*);
  void do_touint(UnaryStmt*);
  void do_toboolean(UnaryExpr*);
  void do_coerce(BinaryStmt*);
  void do_construct(CallStmt2*);
  void do_abc_callprop(CallStmt2*);
  void do_abc_constructprop(CallStmt2*);
  void do_constructsuper(CallStmt2*);
  void do_abc_strictequals(BinaryExpr*);
  void do_abc_modulo(BinaryStmt*);
  void do_lessthan(BinaryExpr* i)      { doCompare(i, HR_lti, HR_ltui, HR_ltd); }
  void do_lessequals(BinaryExpr* i)    { doCompare(i, HR_lei, HR_leui, HR_led); }
  void do_greaterthan(BinaryExpr* i)   { doCompare(i, HR_gti, HR_gtui, HR_gtd); }
  void do_greaterequals(BinaryExpr* i) { doCompare(i, HR_gei, HR_geui, HR_ged); }
  void do_abc_equals(BinaryStmt*);
  void do_abc_add(BinaryStmt*);
  void do_abc_getprop(CallStmt2*);
  void do_abc_getpropx(CallStmt3*);
  void do_abc_setprop(CallStmt2*);
  void do_abc_setpropx(CallStmt3*);
  void do_abc_initprop(CallStmt2*);
  void do_doubletoint32(UnaryExpr*);
  void do_if(IfInstr*);
  void do_speculate_number(BinaryExpr* i) { removeSpeculate(i); }
  void do_speculate_int(BinaryExpr* i) { removeSpeculate(i); }
  void do_speculate_object(BinaryExpr* i) { removeSpeculate(i); }
  void do_speculate_string(BinaryExpr* i) { removeSpeculate(i); }
  void do_speculate_numeric(BinaryExpr* i) { removeSpeculate(i); }
  void do_speculate_bool(BinaryExpr* i) { removeSpeculate(i); }

private:
  void doCompare(BinaryExpr*, InstrKind int_kind, InstrKind uint_kind,
                 InstrKind double_kind);
  void doFindStmt(NaryStmt3*);
  bool specializeArithmetic(InstrKind k1, InstrKind k2, UnaryExpr* instr);
  InstrKind getLoadEnvKind(const Type* object, bool is_interface = false);
  bool specializeSlotCallProp(CallStmt2* instr, CallAnalyzer* call_analyzer);
  bool specializeMethodCallProp(CallStmt2* instr, CallAnalyzer* call_analyzer);
  void removeSpeculate(BinaryExpr* instr);
  bool haveSpeculateUses(SafepointInstr*);
  bool isSpeculate(Instr* instr);

private:
  InstrGraph* const ir_;
  InstrFactory factory_;
  Lattice* const lattice_;
};

} // namespace halfmoon
#endif // SPECIALIZER_H_
