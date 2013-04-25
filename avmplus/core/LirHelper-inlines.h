/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus {

using namespace nanojit;

REALLY_INLINE BuiltinType LirHelper::bt(Traits *t)
{
    return Traits::getBuiltinType(t);
}

REALLY_INLINE LIns* LirHelper::InsConst(int32_t c)
{
    return lirout->insImmI(c);
}

REALLY_INLINE LIns* LirHelper::binaryIns(LOpcode op, LIns *a, LIns *b)
{
    return lirout->ins2(op,a,b);
}

REALLY_INLINE LIns* LirHelper::InsConstPtr(const void *p)
{
    return lirout->insImmP(p);
}

REALLY_INLINE LIns* LirHelper::InsConstAtom(Atom c)
{
    return lirout->insImmP((void*)c);
}

REALLY_INLINE LIns* LirHelper::i2p(LIns *i)
{
    return lirout->insI2P(i);
}

REALLY_INLINE LIns* LirHelper::ui2p(LIns *i)
{
    return lirout->insUI2P(i);
}

REALLY_INLINE LIns* LirHelper::p2i(LIns *i)
{
#ifdef NANOJIT_64BIT
    return lirout->ins1(LIR_q2i, i);
#else
    return i;
#endif
}

REALLY_INLINE LIns* LirHelper::p2dIns(LIns* v)
{
#ifdef NANOJIT_64BIT
    return lirout->ins1(LIR_q2d, v);
#else
    return lirout->ins1(LIR_i2d, v);
#endif
}

#ifdef VMCFG_FLOAT
REALLY_INLINE LIns* LirHelper::i2fIns(LIns* v)
{
    return lirout->ins1(LIR_i2f, v);
}

REALLY_INLINE LIns* LirHelper::ui2fIns(LIns* v)
{
    return lirout->ins1(LIR_ui2f, v);
}

REALLY_INLINE LIns* LirHelper::d2fIns(LIns* v)
{
    return lirout->ins1(LIR_d2f, v);
}

REALLY_INLINE LIns* LirHelper::f2dIns(LIns* v)
{
    return lirout->ins1(LIR_f2d, v);
}

REALLY_INLINE LIns* LirHelper::f4tofIns(LIns* v,eFloat4Component comp)
{
    switch(comp){
    case F4_X: return lirout->ins1(LIR_f4x, v);
    case F4_Y: return lirout->ins1(LIR_f4y, v);
    case F4_Z: return lirout->ins1(LIR_f4z, v);
    case F4_W: return lirout->ins1(LIR_f4w, v);
    }
}
#endif

#if NJ_SOFTFLOAT_SUPPORTED
REALLY_INLINE LIns* LirHelper::qlo(LIns* q)
{
    return lirout->ins1(LIR_dlo2i, q);
}
#endif

REALLY_INLINE LIns* LirHelper::eqp0(LIns* ptr)
{
    return lirout->insEqP_0(ptr);
}

REALLY_INLINE LIns* LirHelper::eqi0(LIns* ptr)
{
    return lirout->insEqI_0(ptr);
}

REALLY_INLINE LIns* LirHelper::eqp(LIns* a, Atom b)
{
    return eqp(a, InsConstAtom(b));
}

REALLY_INLINE LIns* LirHelper::eqp(LIns* a, LIns* b)
{
    return lirout->ins2(LIR_eqp, a, b);
}

REALLY_INLINE LIns* LirHelper::ltup(LIns* a, LIns* b)
{
    return lirout->ins2(LIR_ltup, a, b);
}

REALLY_INLINE LIns* LirHelper::choose(LIns* cond, Atom t, LIns* f)
{
    return lirout->insChoose(cond, InsConstAtom(t), f, use_cmov);
}

REALLY_INLINE LIns* LirHelper::addp(LIns* a, Atom mask)
{
    return lirout->ins2(LIR_addp, a, InsConstAtom(mask));
}

REALLY_INLINE LIns* LirHelper::andp(LIns* a, Atom mask)
{
    return lirout->ins2(LIR_andp, a, InsConstAtom(mask));
}

REALLY_INLINE LIns* LirHelper::orp(LIns* a, Atom mask)
{
    return lirout->ins2(LIR_orp, a, InsConstAtom(mask));
}

REALLY_INLINE LIns* LirHelper::addi(LIns* a, int32_t mask)
{
    return lirout->ins2(LIR_addi, a, InsConst(mask));
}

REALLY_INLINE LIns* LirHelper::ori(LIns* a, int32_t mask)
{
    return lirout->ins2(LIR_ori, a, InsConst(mask));
}

REALLY_INLINE LIns* LirHelper::xorp(LIns* a, Atom mask)
{
    return lirout->ins2(LIR_xorp, a, InsConstAtom(mask));
}

REALLY_INLINE LIns* LirHelper::subp(LIns* a, int32_t offset)
{
    return lirout->ins2(LIR_subp, a, InsConstAtom(offset));
}

REALLY_INLINE LIns* LirHelper::retp(LIns* a)
{
    return lirout->ins1(LIR_retp, a);
}

REALLY_INLINE LIns* LirHelper::label()
{
    return lirout->ins0(LIR_label);
}

REALLY_INLINE LIns* LirHelper::jlti(LIns *a, int32_t b)
{
    return lirout->insBranch(LIR_jt, lirout->ins2(LIR_lti, a, InsConst(b)), NULL);
}

REALLY_INLINE LIns* LirHelper::jgti(LIns *a, int32_t b)
{
    return lirout->insBranch(LIR_jt, lirout->ins2(LIR_gti, a, InsConst(b)), NULL);
}

REALLY_INLINE LIns* LirHelper::jnei(LIns *a, int32_t b)
{
    return lirout->insBranch(LIR_jf, lirout->ins2(LIR_eqi, a, InsConst(b)), NULL);
}

REALLY_INLINE LIns* LirHelper::stp(LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    AvmAssert(val->isP());
    return lirout->insStore(LIR_stp, val, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::sti(LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    AvmAssert(val->isI());
    return lirout->insStore(LIR_sti, val, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::std(LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    AvmAssert(val->isD());
    return lirout->insStore(LIR_std, val, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::ldi(LIns* p, int32_t d, AccSet accSet)
{
    return lirout->insLoad(LIR_ldi, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::ldd(LIns* p, int32_t d, AccSet accSet)
{
    return lirout->insLoad(LIR_ldd, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::ldp(LIns* p, int32_t d, AccSet accSet)
{
    return lirout->insLoad(LIR_ldp, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::livep(LIns* a)
{
    return lirout->ins1(LIR_livep, a);
}

REALLY_INLINE LIns* LirHelper::param(int id, const char *name)
{
    LIns* param = lirout->insParam(id, 0);
#ifdef NJ_VERBOSE
    if (frag->lirbuf->printer)
        frag->lirbuf->printer->lirNameMap->addName(param, name);
#else
    (void)name;
#endif
    return param;
}

REALLY_INLINE LIns* LirHelper::lshi(LIns* a, int32_t b)
{
    return lirout->ins2(LIR_lshi, a, InsConst(b));
}

REALLY_INLINE LIns* LirHelper::rshi(LIns* a, int32_t b)
{
    return lirout->ins2(LIR_rshi, a, InsConst(b));
}

REALLY_INLINE LIns* LirHelper::lshp(LIns* a, int32_t b)
{
    return lirout->ins2(LIR_lshp, a, InsConst(b));
}

REALLY_INLINE LIns* LirHelper::rshp(LIns* a, int32_t b)
{
    return lirout->ins2(LIR_rshp, a, InsConst(b));
}

REALLY_INLINE LIns* LirHelper::rshup(LIns* a, int32_t b)
{
    return lirout->ins2(LIR_rshup, a, InsConst(b));
}

/**
 * Returns true if mask has exactly one bit set
 * see http://aggregate.org/MAGIC/#Is%20Power%20of%202
 */
REALLY_INLINE bool exactlyOneBit(uint32_t m)
{
    AvmAssert(m != 0);
    return (m & (m-1)) == 0;
}

REALLY_INLINE LIns* LirHelper::InsConstDbl(double d)
{
    return lirout->insImmD(d);
}
#ifdef VMCFG_FLOAT
REALLY_INLINE LIns* LirHelper::InsConstFlt(float f)
{
    return lirout->insImmF(f);
}

REALLY_INLINE LIns* LirHelper::stf(LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    AvmAssert(val->isF());
    return lirout->insStore(LIR_stf, val, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::ldf(LIns* p, int32_t d, AccSet accSet)
{
    return lirout->insLoad(LIR_ldf, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::stf4(LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    AvmAssert(val->isF4());
    return lirout->insStore(LIR_stf4, val, p, d, accSet);
}

REALLY_INLINE LIns* LirHelper::ldf4(LIns* p, int32_t d, AccSet accSet)
{
    return lirout->insLoad(LIR_ldf4, p, d, accSet);
}
#endif

// address calc instruction
REALLY_INLINE LIns* LirHelper::lea(int32_t disp, LIns* base) {
    return lirout->ins2(LIR_addp, base, InsConstPtr((void*)disp));
}    
} // namespace
