/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

class Label {
public:
    Label() : address(~0U), backpatches(NULL) {}
    uint32_t address;           // either ~0U or the address of the label
    Seq<uint32_t>* backpatches; // not a SeqBuilder so that we don't have to spend space on storing an allocator in every label object
};

class DefaultValue {
public:
    DefaultValue(uint32_t tag, uint32_t index) : tag(tag), index(index) {}
    const uint32_t tag;
    const uint32_t index;
};


// Context management

enum CtxType {
    CTX_Activation,
    CTX_Break,
    CTX_Catch,
    CTX_Continue,
    CTX_Goto,
    CTX_Finally,
    CTX_Function,
    CTX_ClassMethod,
    CTX_Program,
    CTX_With
};

class Ctx {
public:
    Ctx(CtxType tag, Ctx* next)
        : tag(tag)
        , next(next)
    {
    }
    
    const CtxType  tag;
    Ctx * const    next;
    
    bool mustPushThis();           // context requires a 'this' value to be pushed onto the scope chain
    bool mustPushScopeReg();       // context requires a scope object stored in a scope register to be pushed onto the scope chain
    
    VarScopeCtx * findVarScope();  // Should never be NULL
};

class ControlFlowCtx : public Ctx {
public:
    ControlFlowCtx(CtxType tag, Label* label, Ctx* ctx0)
        : Ctx(tag, ctx0)
        , label(label)
    {
    }
    Label * const label;
};

class BreakCtx : public ControlFlowCtx {
public:
    BreakCtx(Label* label, Ctx* ctx0, Str* label_name=NULL)
        : ControlFlowCtx(CTX_Break, label, ctx0)
        , label_name(label_name)
    {
    }
    Str * const label_name;
};

class ContinueCtx : public ControlFlowCtx {
public:
    ContinueCtx(Label* label, Seq<Str*>* label_names, Ctx* ctx0)
        : ControlFlowCtx(CTX_Continue, label, ctx0)
        , label_names(label_names)
    {
    }
    Seq<Str*>* const label_names;
};

struct GotoLabel {
    GotoLabel(Str* label_name, Label* address, bool ambiguous)
        : label_name(label_name)
        , address(address)
        , ambiguous(ambiguous)
    {
    }
    Str* label_name;
    Label* address;
    const bool ambiguous;
};

class GotoCtx : public Ctx {
public:
    GotoCtx(Ctx* ctx0)
        : Ctx(CTX_Goto, ctx0)
        , next_goto(NULL)
        , label_names(NULL)
    {
    }
    
    GotoCtx* next_goto;            // Populated
    Seq<GotoLabel*>* label_names;  //   during analysis
};
    
// Represents a scope to restore to the scope chain, pushed by 'with'
// or 'catch' or reified activation objects, see subclasses.

class ScopeCtx : public Ctx {
public:
    ScopeCtx(CtxType tag, uint32_t scope_reg, Ctx* ctx0)
        : Ctx(tag, ctx0)
        , scope_reg(scope_reg)
    {
    }
    const uint32_t scope_reg;
};

class WithCtx : public ScopeCtx {
public:
    WithCtx(uint32_t scope_reg, Ctx* ctx0)
        : ScopeCtx(CTX_With, scope_reg, ctx0)
    {
    }
};

struct CatchCtx : public ScopeCtx {
    CatchCtx(uint32_t scope_reg, Ctx* ctx0)
        : ScopeCtx(CTX_Catch, scope_reg, ctx0)
    {
    }
};

class ActivationCtx : public ScopeCtx {
public:
    ActivationCtx(uint32_t scope_reg, Ctx* ctx0)
        : ScopeCtx(CTX_Activation, scope_reg, ctx0)
    {
    }
};

class FinallyCtx : public Ctx {
public:
    FinallyCtx(Allocator* allocator, Label* Lfinally, uint32_t returnreg, Ctx* ctx0)
        : Ctx(CTX_Finally, ctx0)
        , Lfinally(Lfinally)
        , returnreg(returnreg)
        , nextLabel(0)
        , returnLabels(allocator)
    {
    }
    
    uint32_t addReturnLabel(Label* l);
    
    Label * const Lfinally;
    const uint32_t returnreg;
    uint32_t nextLabel;
    SeqBuilder<Label*> returnLabels;
};

class VarScopeCtx : public Ctx {
public:
    VarScopeCtx(CtxType tag, uint32_t nsset, Seq<Namespace*>* openNamespaces, Allocator* allocator, Ctx* prev)
        : Ctx(tag, prev)
        , nsset(nsset)
        , openNamespaces(openNamespaces)
    {
        (void)allocator;
    }
    const uint32_t nsset;                   // Will be something other than 0 if this variable scope has a multiname qualifier (namespace set)
    Seq<Namespace*>* const openNamespaces;  // Not including ""
};

class FunctionCtx : public VarScopeCtx {
public:
    FunctionCtx(Allocator* allocator, uint32_t nsset, Seq<Namespace*>* openNamespaces, Ctx* prev)
        : VarScopeCtx(CTX_Function, nsset, openNamespaces, allocator, prev)
    {
    }
};

class ProgramCtx : public VarScopeCtx {
public:
    ProgramCtx(Allocator* allocator, uint32_t nsset, Seq<Namespace*>* openNamespaces, uint32_t capture_reg)
        : VarScopeCtx(CTX_Program, nsset, openNamespaces, allocator, NULL)
        , capture_reg(capture_reg)
    {
    }
    const uint32_t capture_reg;
};

/**
 * Instruction and value emitter.
 *
 * A 'cogen' abstracts away the details of emitting bytecodes and constant
 * values.
 */
class Cogen {
public:
    Cogen(Compiler* compiler, ABCFile* abc, ABCTraitsTable* traits, ABCMethodBodyInfo* body, uint32_t first_temp=1);
    
    /* Instruction emitters.  Most of these are trivially expanded
     * inline to other calls; some have more complex logic.  For
     * example, I_getlocal(0) expands to OP_getlocal0, the
     * dxns instructions set the uses_dxns flag, and the debug
     * instructions check whether debugging is enabled.  All methods
     * help compute the heights of the value and scope stacks.
     *
     * Please maintain an alphabetical list.
     */
    void I_add();
    void I_add_i();
    void I_applytype(uint32_t nargs);
    void I_astype(uint32_t index);
    void I_astypelate();
    void I_bitand();
    void I_bitnot();
    void I_bitor();
    void I_bitxor();
    void I_call(uint32_t argc);
    void I_callmethod(uint32_t index, uint32_t nargs);
    void I_callproperty(uint32_t index, uint32_t nargs);
    void I_callproplex(uint32_t index, uint32_t nargs);
    void I_callpropvoid(uint32_t index, uint32_t nargs);
    void I_callstatic(uint32_t index, uint32_t nargs);
    void I_callsuper(uint32_t index, uint32_t nargs);
    void I_callsupervoid(uint32_t index, uint32_t nargs);
    void I_construct(uint32_t argc);
    void I_constructprop(uint32_t index, uint32_t nargs);
    void I_constructsuper(uint32_t argc);
    void I_checkfilter();
    void I_coerce(uint32_t index);
    void I_coerce_a();
    void I_coerce_s();
    void I_coerce_b();
    void I_coerce_d();
    void I_coerce_i();
    void I_coerce_u();
    void I_convert_o();
    void I_convert_s();
    void I_debug(uint8_t debug_type, uint32_t index, uint8_t reg, uint32_t extra=0);
    void I_debugfile(uint32_t index);
    void I_debugline(uint32_t linenum);
    void I_declocal(uint32_t reg);
    void I_declocal_i(uint32_t reg);
    void I_decrement();
    void I_decrement_i();
    void I_deleteproperty(uint32_t index);
    void I_divide();
    void I_dxns(uint32_t index);
    void I_dxnslate();
    void I_dup();
    void I_equals();
    void I_esc_xattr();
    void I_esc_xelem();
    void I_findproperty(uint32_t index);
    void I_findpropstrict(uint32_t index);
    void I_getdescendants(uint32_t index);
    void I_getglobalscope();
    void I_getglobalslot(uint32_t index);
    void I_getlex(uint32_t index);
    void I_getlocal(uint32_t index);
    void I_getouterscope(uint32_t index);
    void I_getproperty(uint32_t index);
    void I_getscopeobject(uint32_t index);
    void I_getslot(uint32_t index);
    void I_getsuper(uint32_t index);
    void I_greaterequals();
    void I_greaterthan();
    void I_hasnext();
    void I_hasnext2(uint32_t object_reg, uint32_t index_reg);
    void I_ifeq(Label* label);
    void I_iffalse(Label* label);
    void I_ifge(Label* label);
    void I_ifgt(Label* label);
    void I_ifle(Label* label);
    void I_iflt(Label* label);
    void I_ifne(Label* label);
    void I_ifnge(Label* label);
    void I_ifngt(Label* label);
    void I_ifnle(Label* label);
    void I_ifnlt(Label* label);
    void I_ifstricteq(Label* label);
    void I_ifstrictne(Label* label);
    void I_iftrue(Label* label);
    void I_in();
    void I_inclocal(uint32_t reg);
    void I_inclocal_i(uint32_t reg);
    void I_increment();
    void I_increment_i();
    void I_initproperty(uint32_t index);
    void I_instanceof();
    void I_istype(uint32_t index);
    void I_istypelate();
    void I_jump(Label* label);
    void I_kill(uint32_t index);
    void I_label(Label* label);
    void I_lessequals();
    void I_lessthan();
    void I_lookupswitch(Label* default_label, Label** case_labels, uint32_t ncases);
    void I_lshift();
    void I_modulo();
    void I_multiply();
    void I_multiply_i();
    void I_negate();
    void I_negate_i();
    void I_newactivation();
    void I_newarray(uint32_t nvalues);
    void I_newcatch(uint32_t index);
    void I_newclass(uint32_t index);
    void I_newfunction(uint32_t index);
    void I_newobject(uint32_t npairs);
    void I_newvoid(uint32_t index);
    void I_nextname();
    void I_nextvalue();
    void I_nop();
    void I_not();
    void I_pop();
    void I_popscope();
    void I_pushbyte(int8_t b);
    void I_pushfalse();
    void I_pushdouble(uint32_t index);
    void I_pushfloat(uint32_t index);
    void I_pushint(uint32_t index);
    void I_pushnamespace(uint32_t index);
    void I_pushnan();
    void I_pushnull();
    void I_pushscope();
    void I_pushshort(int16_t v);
    void I_pushstring(uint32_t index);
    void I_pushtrue();
    void I_pushuint(uint32_t index);
    void I_pushundefined();
    void I_pushwith();
    void I_returnvalue();
    void I_returnvoid();
    void I_rshift();
    void I_setglobalslot(uint32_t index);
    void I_setlocal(uint32_t index);
    void I_setproperty(uint32_t index);
    void I_setslot(uint32_t index);
    void I_setsuper(uint32_t index);
    void I_strictequals();
    void I_subtract();
    void I_subtract_i();
    void I_swap();
    void I_throw();
    void I_typeof();
    void I_urshift();
    void I_opcode(AbcOpcode opcode);

    uint32_t emitString(Str* value);    // cpool index
    uint32_t emitInt(int32_t value);    // cpool index
    uint32_t emitUInt(uint32_t value);  // cpool index
    uint32_t emitDouble(double value);  // cpool index
    uint32_t emitFloat(float value);    // cpool index
    uint32_t emitNamespace(NameComponent* ns);
    uint32_t emitNamespace(uint32_t name);
    uint32_t emitSlotTrait(uint32_t name, uint32_t type);
    uint32_t emitConstTrait(uint32_t name, uint32_t type);
    uint32_t emitMethodTrait(uint32_t name, uint32_t method);
    uint32_t emitException(uint32_t from, uint32_t to, uint32_t target, uint32_t type, uint32_t name_index);

    static uint32_t emitTypeName(Compiler* compiler, Type* t);
    static void checkVectorType(Compiler* compiler, Type * t);
    static void checkVectorType(Compiler* compiler, QualifiedName * t);

    Label*   newLabel();
    uint32_t getTemp();
    void unstructuredControlFlow(Ctx* ctx, bool (hit)(Ctx*,void*), void* package, bool jump, SyntaxError msg, uint32_t pos=0);
    void standardMethod(bool is_function, Seq<Str*>* params, Seq<FunctionDefn*>* functions, Seq<Str*>* vars, Seq<Stmt*>* stmts);
    uint32_t arguments(Seq<Expr*>* args, Ctx* ctx);
    void startCatch();

    AbcOpcode binopToOpcode(Binop op, bool* isNegated);
    uint32_t buildNssetWithPublic(Seq<Namespace*>* ns);

    Compiler* const compiler;
    ABCFile* const abc;
    Allocator * const allocator;

    uint32_t getMaxStack() const;
    uint32_t getMaxScope() const;
    uint32_t getLocalCount() const;
    uint32_t getCodeLength() const;
    uint8_t getFlags() const;
    uint8_t* serializeCodeBytes(uint8_t* b) const;
    
private:
    void callMN(AbcOpcode op, uint32_t index, uint32_t nargs);
    void propU30(AbcOpcode op, uint32_t index);
    void emitOp(AbcOpcode op);
    void emitOpU30(AbcOpcode op, uint32_t u30);
    void emitOpU30Special(AbcOpcode op, uint32_t u30, uint32_t stack_adjust);
    void emitOpU30U30(AbcOpcode op, uint32_t u30_1, uint32_t u30_2);
    void emitOpU8(AbcOpcode op, uint8_t b);
    void emitOpS8(AbcOpcode op, int8_t b);
    void emitOpS16(AbcOpcode op, uint16_t s);
    void emitJump(AbcOpcode op, Label* label);
    void fixupBackpatches(uint8_t* b) const;
    void stackMovement(AbcOpcode opcode);
    void stackMovement(AbcOpcode opcode, bool hasRTNS, bool hasRTName, uint32_t argc);

    // Mapping from subset of tokens to attributes and operator values
    static const struct BinopMapping {
        unsigned isNegated:1;
        unsigned abcOpcode:8;
    } binopMapping[];
    
    ByteBuffer code;
    
    SeqBuilder<Label*> labels;
    ABCTraitsTable * const traits;
    ABCMethodBodyInfo * const body;
    uint32_t last_linenum;
    uint32_t label_counter;
    uint32_t temp_counter;
    bool sets_dxns;
    bool need_activation;
    uint32_t stack_depth;
    uint32_t max_stack_depth;
    uint32_t scope_depth;
};

}}
