/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

inline bool Ctx::mustPushThis() { return tag == CTX_ClassMethod || tag == CTX_Program; }
inline bool Ctx::mustPushScopeReg() { return tag == CTX_With || tag == CTX_Catch || tag == CTX_Activation; }

inline uint32_t FinallyCtx::addReturnLabel(Label* l)
{
    returnLabels.addAtEnd(l);
    return nextLabel++;
}

inline uint32_t Cogen::emitInt(int32_t i) { return abc->addInt(i); }
inline uint32_t Cogen::emitUInt(uint32_t u) { return abc->addUInt(u); }
inline uint32_t Cogen::emitDouble(double d) { return abc->addDouble(d); }
inline uint32_t Cogen::emitFloat(float f) { return abc->addFloat(f); }
inline uint32_t Cogen::emitString(Str* str) { return abc->addString(str); }
inline uint32_t Cogen::emitSlotTrait(uint32_t name, uint32_t type) { return traits->addTrait(ALLOC(ABCSlotTrait, (name, type, TRAIT_Slot))); }
inline uint32_t Cogen::emitConstTrait(uint32_t name, uint32_t type) { return traits->addTrait(ALLOC(ABCSlotTrait, (name, type, TRAIT_Const))); }
inline uint32_t Cogen::emitMethodTrait(uint32_t name, uint32_t method) { return traits->addTrait(ALLOC(ABCMethodTrait, (name, method))); }
inline uint32_t Cogen::emitNamespace(uint32_t name) { return abc->addNamespace(CONSTANT_Namespace, name); }

inline uint32_t Cogen::getTemp()
{
    return temp_counter++;
}

inline uint32_t Cogen::getMaxStack() const { return max_stack_depth; }
inline uint32_t Cogen::getMaxScope() const { return scope_depth; }
inline uint32_t Cogen::getLocalCount() const { return temp_counter; }
inline uint32_t Cogen::getCodeLength() const { return code.size(); }

inline void Cogen::startCatch()
{
    stack_depth = 1;
    if (stack_depth > max_stack_depth)
        max_stack_depth = stack_depth;
}

inline uint8_t Cogen::getFlags() const
{
    return traits->getCount() > 0 ? abcMethod_NEED_ACTIVATION : 0;
}

inline AbcOpcode Cogen::binopToOpcode(Binop op, bool* isNegated)
{
    *isNegated = binopMapping[op].isNegated;
    return (AbcOpcode)(binopMapping[op].abcOpcode);
}

inline void Cogen::I_add() { emitOp(OP_add); }
inline void Cogen::I_add_i() { emitOp(OP_add_i); }
inline void Cogen::I_applytype(uint32_t nargs) { emitOpU30(OP_applytype, nargs); }
inline void Cogen::I_astype(uint32_t index) { emitOpU30(OP_astype, index); }
inline void Cogen::I_astypelate() { emitOp(OP_astypelate); }
inline void Cogen::I_bitand() { emitOp(OP_bitand); }
inline void Cogen::I_bitnot() { emitOp(OP_bitnot); }
inline void Cogen::I_bitor() { emitOp(OP_bitor); }
inline void Cogen::I_bitxor() { emitOp(OP_bitxor); }
inline void Cogen::I_checkfilter() { emitOp(OP_checkfilter); }
inline void Cogen::I_coerce(uint32_t index) { emitOpU30(OP_coerce, index); }
inline void Cogen::I_coerce_a() { emitOp(OP_coerce_a); }
inline void Cogen::I_coerce_s() { emitOp(OP_coerce_s); }
inline void Cogen::I_coerce_b() { emitOp(OP_coerce_b); }
inline void Cogen::I_coerce_d() { emitOp(OP_coerce_d); }
inline void Cogen::I_coerce_i() { emitOp(OP_coerce_i); }
inline void Cogen::I_coerce_u() { emitOp(OP_coerce_u); }
inline void Cogen::I_convert_o() { emitOp(OP_convert_o); }
inline void Cogen::I_convert_s() { emitOp(OP_convert_s); }
inline void Cogen::I_declocal(uint32_t reg) { emitOpU30(OP_declocal, reg); }
inline void Cogen::I_declocal_i(uint32_t reg) { emitOpU30(OP_declocal_i, reg); }
inline void Cogen::I_decrement() { emitOp(OP_decrement); }
inline void Cogen::I_decrement_i() { emitOp(OP_decrement_i); }
inline void Cogen::I_divide() { emitOp(OP_divide); }
inline void Cogen::I_dup() { emitOp(OP_dup); }
inline void Cogen::I_dxns(uint32_t index) { emitOpU30(OP_dxns, index); }
inline void Cogen::I_dxnslate() { sets_dxns=true; emitOp(OP_dxnslate); }
inline void Cogen::I_equals() { emitOp(OP_equals); }
inline void Cogen::I_esc_xattr() { emitOp(OP_esc_xattr); }
inline void Cogen::I_esc_xelem() { emitOp(OP_esc_xelem); }
inline void Cogen::I_getglobalscope() { emitOp(OP_getglobalscope); }
inline void Cogen::I_getglobalslot(uint32_t index) { emitOpU30(OP_getglobalslot, index); }
inline void Cogen::I_getlex(uint32_t index) { emitOpU30(OP_getlex, index); }
inline void Cogen::I_getouterscope(uint32_t index) { emitOpU30(OP_getouterscope, index); }
inline void Cogen::I_getscopeobject(uint32_t index) { emitOpU30(OP_getscopeobject, index); }
inline void Cogen::I_getslot(uint32_t index) { emitOpU30(OP_getslot, index); }
inline void Cogen::I_greaterequals() { emitOp(OP_greaterequals); }
inline void Cogen::I_greaterthan() { emitOp(OP_greaterthan); }
inline void Cogen::I_hasnext() { emitOp(OP_hasnext); }
inline void Cogen::I_ifeq(Label* label) { emitJump(OP_ifeq, label); }
inline void Cogen::I_iffalse(Label* label) { emitJump(OP_iffalse, label); }
inline void Cogen::I_ifge(Label* label) { emitJump(OP_ifge, label); }
inline void Cogen::I_ifgt(Label* label) { emitJump(OP_ifgt, label); }
inline void Cogen::I_ifle(Label* label) { emitJump(OP_ifle, label); }
inline void Cogen::I_iflt(Label* label) { emitJump(OP_iflt, label); }
inline void Cogen::I_ifne(Label* label) { emitJump(OP_ifne, label); }
inline void Cogen::I_ifnge(Label* label) { emitJump(OP_ifnge, label); }
inline void Cogen::I_ifngt(Label* label) { emitJump(OP_ifngt, label); }
inline void Cogen::I_ifnle(Label* label) { emitJump(OP_ifnle, label); }
inline void Cogen::I_ifnlt(Label* label) { emitJump(OP_ifnlt, label); }
inline void Cogen::I_ifstricteq(Label* label) { emitJump(OP_ifstricteq, label); }
inline void Cogen::I_ifstrictne(Label* label) { emitJump(OP_ifstrictne, label); }
inline void Cogen::I_iftrue(Label* label) { emitJump(OP_iftrue, label); }
inline void Cogen::I_in() { emitOp(OP_in); }
inline void Cogen::I_inclocal(uint32_t reg) { emitOpU30(OP_inclocal, reg); }
inline void Cogen::I_inclocal_i(uint32_t reg) { emitOpU30(OP_inclocal_i, reg); }
inline void Cogen::I_increment() { emitOp(OP_increment); }
inline void Cogen::I_increment_i() { emitOp(OP_increment_i); }
inline void Cogen::I_instanceof() { emitOp(OP_instanceof); }
inline void Cogen::I_istype(uint32_t index) { emitOpU30(OP_istype, index); }
inline void Cogen::I_istypelate() { emitOp(OP_istypelate); }
inline void Cogen::I_jump(Label* label) { emitJump(OP_jump, label); }
inline void Cogen::I_kill(uint32_t index) { emitOpU30(OP_kill, index); }
inline void Cogen::I_lessequals() { emitOp(OP_lessequals); }
inline void Cogen::I_lessthan() { emitOp(OP_lessthan); }
inline void Cogen::I_lshift() { emitOp(OP_lshift); }
inline void Cogen::I_modulo() { emitOp(OP_modulo); }
inline void Cogen::I_multiply() { emitOp(OP_multiply); }
inline void Cogen::I_multiply_i() { emitOp(OP_multiply_i); }
inline void Cogen::I_negate() { emitOp(OP_negate); }
inline void Cogen::I_negate_i() { emitOp(OP_negate_i); }
inline void Cogen::I_newactivation() { need_activation=true; emitOp(OP_newactivation); }
inline void Cogen::I_newcatch(uint32_t index) { emitOpU30(OP_newcatch, index); }
inline void Cogen::I_newclass(uint32_t index) { emitOpU30(OP_newclass, index); }
inline void Cogen::I_newfunction(uint32_t index) { emitOpU30(OP_newfunction, index); }
inline void Cogen::I_nextname() { emitOp(OP_nextname); }
inline void Cogen::I_nextvalue() { emitOp(OP_nextvalue); }
inline void Cogen::I_nop() { emitOp(OP_nop); }
inline void Cogen::I_not() { emitOp(OP_not); }
inline void Cogen::I_pop() { emitOp(OP_pop); }
inline void Cogen::I_popscope() { emitOp(OP_popscope); }
inline void Cogen::I_pushdouble(uint32_t index) { emitOpU30(OP_pushdouble, index); }
#ifdef VMCFG_FLOAT
inline void Cogen::I_pushfloat(uint32_t index) { emitOpU30(OP_pushfloat, index); }
#endif
inline void Cogen::I_pushfalse() { emitOp(OP_pushfalse); }
inline void Cogen::I_pushint(uint32_t index) { emitOpU30(OP_pushint, index); }
inline void Cogen::I_pushnamespace(uint32_t index) { emitOpU30(OP_pushnamespace, index); }
inline void Cogen::I_pushnan() { emitOp(OP_pushnan); }
inline void Cogen::I_pushnull() { emitOp(OP_pushnull); }
inline void Cogen::I_pushscope() { scope_depth++;  emitOp(OP_pushscope); }
inline void Cogen::I_pushshort(int16_t v) { emitOpS16(OP_pushshort, v); }
inline void Cogen::I_pushstring(uint32_t index) { emitOpU30(OP_pushstring, index); }
inline void Cogen::I_pushtrue() { emitOp(OP_pushtrue); }
inline void Cogen::I_pushuint(uint32_t index) { emitOpU30(OP_pushuint, index); }
inline void Cogen::I_pushundefined() { emitOp(OP_pushundefined); }
inline void Cogen::I_pushwith() { scope_depth++;  emitOp(OP_pushwith); }
inline void Cogen::I_returnvalue() { emitOp(OP_returnvalue); }
inline void Cogen::I_returnvoid() { emitOp(OP_returnvoid); }
inline void Cogen::I_rshift() { emitOp(OP_rshift); }
inline void Cogen::I_setglobalslot(uint32_t index) { emitOpU30(OP_setglobalslot, index); }
inline void Cogen::I_strictequals() { emitOp(OP_strictequals); }
inline void Cogen::I_subtract() { emitOp(OP_subtract); }
inline void Cogen::I_subtract_i() { emitOp(OP_subtract_i); }
inline void Cogen::I_swap() { emitOp(OP_swap); }
inline void Cogen::I_throw() { emitOp(OP_throw); }
inline void Cogen::I_typeof() { emitOp(OP_typeof); }
inline void Cogen::I_urshift() { emitOp(OP_urshift); }

inline void Cogen::I_call(uint32_t nargs) { emitOpU30(OP_call, nargs); }
inline void Cogen::I_construct(uint32_t nargs) { emitOpU30(OP_construct, nargs); }
inline void Cogen::I_constructsuper(uint32_t nargs) { emitOpU30(OP_constructsuper, nargs); }

inline void Cogen::I_callmethod(uint32_t index, uint32_t nargs) { emitOpU30U30(OP_callmethod, index, nargs); }
inline void Cogen::I_callstatic(uint32_t index, uint32_t nargs) { emitOpU30U30(OP_callstatic, index, nargs); }

inline void Cogen::I_callsuper(uint32_t index, uint32_t nargs) { callMN(OP_callsuper, index, nargs); }
inline void Cogen::I_callproperty(uint32_t index, uint32_t nargs) { callMN(OP_callproperty, index, nargs); }
inline void Cogen::I_constructprop(uint32_t index, uint32_t nargs) { callMN(OP_constructprop, index, nargs); }
inline void Cogen::I_callproplex(uint32_t index, uint32_t nargs) { callMN(OP_callproplex, index, nargs); }
inline void Cogen::I_callsupervoid(uint32_t index, uint32_t nargs) { callMN(OP_callsupervoid, index, nargs); }
inline void Cogen::I_callpropvoid(uint32_t index, uint32_t nargs) { callMN(OP_callpropvoid, index, nargs); }

inline void Cogen::I_deleteproperty(uint32_t index) { propU30(OP_deleteproperty, index); }
inline void Cogen::I_getdescendants(uint32_t index) { propU30(OP_getdescendants, index); }
inline void Cogen::I_getproperty(uint32_t index) { propU30(OP_getproperty, index);; }
inline void Cogen::I_getsuper(uint32_t index) { propU30(OP_getsuper, index);; }
inline void Cogen::I_findproperty(uint32_t index) { propU30(OP_findproperty, index); }
inline void Cogen::I_findpropstrict(uint32_t index) { propU30(OP_findpropstrict, index); }
inline void Cogen::I_initproperty(uint32_t index) { propU30(OP_initproperty, index); }
inline void Cogen::I_setproperty(uint32_t index) { propU30(OP_setproperty, index); }
inline void Cogen::I_setsuper(uint32_t index) { propU30(OP_setsuper, index); }

inline void Cogen::I_hasnext2(uint32_t object_reg, uint32_t index_reg) { emitOpU30U30(OP_hasnext2, object_reg, index_reg); }

inline void Cogen::I_newarray(uint32_t nargs) { emitOpU30Special(OP_newarray, nargs, nargs); }
inline void Cogen::I_newobject(uint32_t nargs) { emitOpU30Special(OP_newobject, nargs, 2*nargs); }

inline void Cogen::I_pushbyte(int8_t b) { emitOpS8(OP_pushbyte, b); }

inline void Cogen::I_setslot(uint32_t index) { emitOpU30(OP_setslot, index); }
inline void Cogen::I_opcode(AbcOpcode opcode) { emitOp(opcode); }

}}
