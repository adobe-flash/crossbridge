/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_EVAL

#include "eval.h"

namespace avmplus
{
    namespace RTC
    {
        using namespace ActionBlockConstants;

        VarScopeCtx * Ctx::findVarScope()
        {
            Ctx* ctx = this;
            while (ctx->tag != CTX_Function && ctx->tag != CTX_ClassMethod && ctx->tag != CTX_Program)
                ctx = ctx->next;
            return (VarScopeCtx*)ctx;
        }
        
        const Cogen::BinopMapping Cogen::binopMapping[] = {
        {0, 0},                 // unused
        {0, OP_astypelate},     // OPR_as
        {0, OP_add},            // OPR_plus,
        {0, 0},                 // OPR_assign,
        {0, 0},                 // OPR_init,
        {0, 0},                 // OPR_comma,
        {0, OP_subtract},       // OPR_minus,
        {0, OP_multiply},       // OPR_multiply,
        {0, OP_divide},         // OPR_divide,
        {0, OP_modulo},         // OPR_remainder,
        {0, OP_lshift},         // OPR_leftShift,
        {0, OP_rshift},         // OPR_rightShift,
        {0, OP_urshift},        // OPR_rightShiftUnsigned,
        {0, OP_instanceof},     // OPR_instanceof
        {0, OP_in},             // OPR_in
        {0, OP_istypelate},     // OPR_is
        {0, OP_bitand},         // OPR_bitwiseAnd,
        {0, OP_bitor},          // OPR_bitwiseOr,
        {0, OP_bitxor},         // OPR_bitwiseXor,
        {0, 0},                 // OPR_logicalAnd,
        {0, 0},                 // OPR_logicalOr,
        {0, OP_lessthan},       // OPR_less,
        {0, OP_lessequals},     // OPR_lessOrEqual,
        {0, OP_greaterthan},    // OPR_greater,
        {0, OP_greaterequals},  // OPR_greaterOrEqual,
        {0, OP_equals},         // OPR_equal,
        {1, OP_equals},         // OPR_notEqual,
        {0, OP_strictequals},   // OPR_strictEqual,
        {1, OP_strictequals},   // OPR_strictNotEqual
        };

        Cogen::Cogen(Compiler *compiler, ABCFile* abc, ABCTraitsTable* traits, ABCMethodBodyInfo* body, uint32_t first_temp)
            : compiler(compiler)
            , abc(abc)
            , allocator(compiler->allocator)
            , code(compiler->allocator)
            , labels(compiler->allocator)
            , traits(traits)
            , body(body)
            , last_linenum(0)
            , label_counter(0)
            , temp_counter(first_temp)
            , sets_dxns(false)
            , need_activation(false)
            , stack_depth(0)
            , max_stack_depth(0)
            , scope_depth(0)
        {
        }

        uint8_t* Cogen::serializeCodeBytes(uint8_t* b) const
        {
            code.serialize(b);
            fixupBackpatches(b);
            return b + code.size();
        }

        /* Stack height tracking is "simplest possible": we assume that even for
         * unconditional branches the stack height in the taken branch and in the
         * untaken (impossible) branch are the same; this means that anyone jumping
         * to a label following a jump must be careful to match the stack height.
         * This does not seem to be an issue in practice; the verifier keeps us
         * honest; and we don't need to track the expected stack height at every
         * label.  Also, we're not interested in the actual stack height everywhere,
         * just a conservative approximation to it.
         */
        void Cogen::stackMovement(AbcOpcode opcode)
        {
            stack_depth = stack_depth + opcodeInfo[opcode].stack;
            AvmAssert((int32_t)stack_depth >= 0);
            if (stack_depth > max_stack_depth)
                max_stack_depth = stack_depth;
        }

        void Cogen::stackMovement(AbcOpcode opcode, bool hasRTNS, bool hasRTName, uint32_t pops)
        {
            stack_depth = stack_depth + opcodeInfo[opcode].stack - hasRTNS - hasRTName - pops;
            AvmAssert((int32_t)stack_depth >= 0);
            if (stack_depth > max_stack_depth)
                max_stack_depth = stack_depth;
        }
        
        void Cogen::emitOp(AbcOpcode opcode)
        {
            code.emitU8((uint8_t)opcode);
            stackMovement(opcode);
        }
        
        void Cogen::emitOpU30(AbcOpcode opcode, uint32_t u30)
        {
            code.emitU8((uint8_t)opcode);
            code.emitU30(u30);
            stackMovement(opcode);
        }

        void Cogen::emitOpU30Special(AbcOpcode opcode, uint32_t u30, uint32_t pops)
        {
            code.emitU8((uint8_t)opcode);
            code.emitU30(u30);
            stackMovement(opcode, false, false, pops);
        }
        
        void Cogen::emitOpU30U30(AbcOpcode opcode, uint32_t u30_1, uint32_t u30_2)
        {
            code.emitU8((uint8_t)opcode);
            code.emitU30(u30_1);
            code.emitU30(u30_2);
            stackMovement(opcode);
        }
        
        void Cogen::emitOpU8(AbcOpcode opcode, uint8_t b)
        {
            code.emitU8((uint8_t)opcode);
            code.emitU8(b);
            stackMovement(opcode);
        }

        void Cogen::emitOpS8(AbcOpcode opcode, int8_t b)
        {
            code.emitU8((uint8_t)opcode);
            code.emitS8(b);
            stackMovement(opcode);
        }
        
        void Cogen::I_getlocal(uint32_t index) {
            if (index < 4)
                emitOp((AbcOpcode)(OP_getlocal0 + index));
            else
                emitOpU30(OP_getlocal, index);
        }

        void Cogen::I_setlocal(uint32_t index) {
            if (index < 4)
                emitOp((AbcOpcode)(OP_setlocal0 + index));
            else
                emitOpU30(OP_setlocal, index);
        }

        void Cogen::I_debugfile(uint32_t index)
        {
            if (compiler->debugging)
                emitOpU30(OP_debugfile, index);
        }
        
        void Cogen::I_debugline(uint32_t linenum)
        {
            if (compiler->debugging && linenum > last_linenum)
            {
                last_linenum = linenum;
                emitOpU30(OP_debugline, linenum);
            }
        }

        void Cogen::callMN(AbcOpcode opcode, uint32_t index, uint32_t nargs) {
            code.emitU8((uint8_t)opcode);
            code.emitU30(index);
            code.emitU30(nargs);
            stackMovement(opcode, abc->hasRTNS(index), abc->hasRTName(index), nargs);
        }
        
        void Cogen::propU30(AbcOpcode opcode, uint32_t index)
        {
            code.emitU8((uint8_t)opcode);
            code.emitU30(index);
            stackMovement(opcode, abc->hasRTNS(index), abc->hasRTName(index), 0);
        }
        
        Label* Cogen::newLabel()
        {
            Label* l = ALLOC(Label, ());
            labels.addAtEnd(l);
            return l;
        }
        
        void Cogen::emitJump(AbcOpcode opcode, Label* label)
        {
            // OPTIMIZEME: don't need to register backpatches for branches to known labels.
            code.emitU8((uint8_t)opcode);
            code.emitS24(3);
            stackMovement(opcode);
            label->backpatches = ALLOC(Seq<uint32_t>, (code.size() - 3, label->backpatches));
        }
        
        void Cogen::I_label(Label* label)
        {
            AvmAssert(label->address == ~0U);
            label->address = code.size();
            code.emitU8((uint8_t)OP_label);
        }
        
        // The location to be patched must contain a signed adjustment that will be
        // added to the offset value.  For regular jump instructions this should be '3',
        // because the jump is relative to the end of the instruction - 3 bytes after
        // the address of the offset field.  For lookupswitch it is a value that depends
        // on the location within the lookupswitch instruction of the offset word,
        // because the jump is relative to the start of the instruction.
        
        void Cogen::fixupBackpatches(uint8_t* b) const
        {
            for ( Seq<Label*>* labels = this->labels.get() ; labels != NULL ; labels = labels->tl ) {
                uint32_t addr = labels->hd->address;
                bool backward = false;
                AvmAssert(addr != ~0U);
                for ( Seq<uint32_t>* backpatches = labels->hd->backpatches ; backpatches != NULL ; backpatches = backpatches->tl ) {
                    uint32_t loc = backpatches->hd;
                    int32_t adjustment = readS24(b + loc);
                    int32_t offset = (int32_t)(addr - (loc + adjustment));
                    backward = backward || offset < 0;
                    emitS24(b + loc, offset);
                }
                if (!backward) {
                    // Work around verifier bug: if a branch to this label is never a backward
                    // branch then replace OP_label with OP_nop.  The verifier always assumes
                    // that OP_label is the target of a backward branch.
                    b[addr] = OP_nop;
                }
            }
        }
        
        uint32_t Cogen::emitException(uint32_t from, uint32_t to, uint32_t target, uint32_t type, uint32_t name_index)
        {
            return body->exceptions.addAtEnd(ALLOC(ABCExceptionInfo, (from, to, target, type, name_index)));
        }
        
        /*static*/
        uint32_t Cogen::emitTypeName(Compiler* compiler, Type* t)
        {
            ABCFile* abc = &compiler->abc;
            if (t == NULL)
                return 0;
            if (t->tag() == TAG_simpleType)
            {
                QualifiedName* n = ((SimpleType*)t)->name;
                if (n->qualifier != NULL) {
                    AvmAssert(n->qualifier->tag() == TAG_simpleName);
                    uint32_t ns = abc->addNamespace(CONSTANT_Namespace, abc->addString(((SimpleName*)n->qualifier)->name));
                    return abc->addQName(ns, abc->addString(((SimpleName*)n->name)->name));
                }
                else {
                    AvmAssert(n->name->tag() == TAG_simpleName);
                    return abc->addMultiname(compiler->NSS_public, abc->addString(((SimpleName*)n->name)->name));
                }
            }
            else if (t->tag() == TAG_instantiatedType)
            {
                checkVectorType(compiler, ((InstantiatedType*)t)->basename);
                return abc->addTypeName(compiler->ID_Vector, emitTypeName(compiler, ((InstantiatedType*)t)->tparam));
            }
            else
            {
                compiler->internalError(t->pos, "Should not happen");
                return 0;
            }
        }
    
        /*static*/
        void Cogen::checkVectorType(Compiler* compiler, QualifiedName* t)
        {
            // Note we do not allow eg "public::Vector" since Vector is actually not
            // in the public namespace.  It is in a namespace that is magically opened
            // by the use of type application syntax.
            if (t->qualifier != NULL ||
                t->name->tag() != TAG_simpleName ||
                ((SimpleName*)t->name)->name != compiler->SYM_Vector)
                compiler->syntaxError(t->pos, SYNTAXERR_ILLEGAL_TYPENAME);
        }

        /*static*/
        void Cogen::checkVectorType(Compiler* compiler, Type* t)
        {
            if (t->tag() != TAG_simpleType)
                compiler->syntaxError(t->pos, SYNTAXERR_ILLEGAL_TYPENAME);
            checkVectorType(compiler, ((SimpleType*)t)->name);
        }

        void Cogen::I_lookupswitch(Label* default_label, Label** case_labels, uint32_t ncases)
        {
            AvmAssert( ncases > 0 );
            AvmAssert( default_label != NULL );
            // AvmAssert( forall c in case_labels c != NULL );

            uint32_t here = code.size();
            code.emitU8((uint8_t)OP_lookupswitch);
            code.emitS24((int32_t)(here - code.size()));
            default_label->backpatches = ALLOC(Seq<uint32_t>, (code.size() - 3, default_label->backpatches));
            code.emitU30(ncases - 1);
            for ( uint32_t i=0 ; i < ncases ; i++ ) {
                Label* label = case_labels[i];
                code.emitS24((int32_t)(here - code.size()));
                label->backpatches = ALLOC(Seq<uint32_t>, (code.size() - 3, label->backpatches));
            }
            stackMovement(OP_lookupswitch);
        }

        void FunctionDefn::cogenGuts(Compiler* compiler, Ctx* ctx, bool isMethod, ABCMethodInfo** info, ABCMethodBodyInfo** body)
        {
            Allocator* allocator = compiler->allocator;
            ABCFile* abc = &compiler->abc;
            ABCTraitsTable* traits;
            Str* name = this->name;
            if (name == NULL)
                name = compiler->SYM_anonymous;
            
            SeqBuilder<uint32_t> param_types(allocator);
            SeqBuilder<DefaultValue*> default_values(allocator);
            uint32_t numdefaults = 0;
            for ( Seq<FunctionParam*>* params = this->params ; params != NULL ; params = params->tl ) {
                param_types.addAtEnd(Cogen::emitTypeName(compiler, params->hd->type_name));
                if (params->hd->default_value != NULL) {
                    Expr* dv = params->hd->default_value;
                    uint32_t cv = 0;
                    uint32_t ct = 0;
                    switch (dv->tag()) {
                        case TAG_literalString:
                            ct = CONSTANT_Utf8;
                            cv = abc->addString(((LiteralString*)dv)->value);
                            break;
                        case TAG_literalUInt:
                            ct = CONSTANT_UInt;
                            cv = abc->addUInt(((LiteralUInt*)dv)->value);
                            break;
                        case TAG_literalInt:
                            ct = CONSTANT_Int;
                            cv = abc->addInt(((LiteralInt*)dv)->value);
                            break;
                        case TAG_literalDouble:
                            ct = CONSTANT_Double;
                            cv = abc->addDouble(((LiteralDouble*)dv)->value);
                            break;
#ifdef VMCFG_FLOAT
                        case TAG_literalFloat:
                            ct = CONSTANT_Float;
                            cv = abc->addFloat(((LiteralFloat*)dv)->value);
                            break;
#endif
                        case TAG_literalBoolean:
                            if (((LiteralBoolean*)dv)->value)
                                ct = CONSTANT_True;
                            else
                                ct = CONSTANT_False;
                            break;
                        case TAG_literalNull:
                            ct = CONSTANT_Null;
                            break;
                        default:
                            // EXTENDME: we can sort-of support arbitrary default values here if we want to.
                            //
                            // AS3 does not support default value other than the six cases above.  Doing better
                            // would be nice.
                            //
                            // We can use one of the obscure namespace default values as a placeholder, then
                            // generate code to test for that value and compute the correct default value.
                            // But the signature of the function won't be right; the type of the argument
                            // must be '*'.  May be close enough, as long as we assign a provided argument
                            // value to a typed slot and get a type check on entry.
                            compiler->syntaxError(params->hd->default_value->pos, SYNTAXERR_IMPOSSIBLE_DEFAULT);
                    }
                    numdefaults++;
                    default_values.addAtEnd(ALLOC(DefaultValue, (ct, cv)));
                }
            }
            *info = ALLOC(ABCMethodInfo, (compiler, abc->addString(name), numparams, param_types.get(), numdefaults, default_values.get(), Cogen::emitTypeName(compiler, return_type_name)));
            traits = ALLOC(ABCTraitsTable, (compiler));
            *body = ALLOC(ABCMethodBodyInfo, (compiler, *info, traits, 1 + numparams + (uses_arguments || (rest_param != NULL)), empty_body));

            // Skipping code generation here is a fairly minor optimization, this will
            // only kick in for interface and native bodies (which will subsequently be
            // discarded).

            if (!empty_body) {
                Cogen* body_cogen = &(*body)->cogen;
                if (isMethod) {
                    body_cogen->I_getlocal(0);
                    body_cogen->I_pushscope();
                }

                cogen(body_cogen, ctx);
            
                if (isMethod)
                    body_cogen->I_popscope();
            }

            uint32_t flags = 0;
            AvmAssert( !(uses_arguments && (rest_param != NULL)) );
            if (uses_arguments)
                flags |= abcMethod_NEED_ARGUMENTS;
            if (rest_param != NULL)
                flags |= abcMethod_NEED_REST;
            if (uses_dxns)
                flags |= abcMethod_SETS_DXNS;
            (*info)->setFlags((uint8_t)((*body)->getFlags() | flags));
        }

        void CodeBlock::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            ABCFile* abc = cogen->abc;
            uint32_t activation = 0;    // 0 means "unallocated"
            FunctionDefn* fn = NULL;
            
            if (tag == CODE_Function)
                fn = (FunctionDefn*)this;
                
            cogen->I_debugfile(cogen->emitString(compiler->str_filename));
            
            if (tag == CODE_Program) {
                cogen->I_getlocal(0);
                cogen->I_pushscope();
            }

            if (fn && fn->inits != NULL) {
                // Erect a non-program context for the inits to execute within, to avoid actions being taken as for CODE_Program.
                VarScopeCtx* vs = ctx->findVarScope();
                Seq<Namespace*>* openNamespaces = vs->openNamespaces;
                uint32_t nsset = vs->nsset;
                FunctionCtx ctx0(cogen->allocator, nsset, openNamespaces, ctx);
                for ( Seq<Stmt*>* inits=fn->inits ; inits != NULL ; inits = inits->tl )
                    inits->hd->cogen(cogen, &ctx0);
            }

            if (fn && (fn->bindings != NULL || fn->uses_arguments)) {
                activation = cogen->getTemp();
                cogen->I_newactivation();
                cogen->I_dup();
                cogen->I_setlocal(activation);
                cogen->I_pushscope();
            }
            
            for ( Seq<Binding*>* bindings = this->bindings ; bindings != NULL ; bindings = bindings->tl ) {
                Binding* b = bindings->hd;
                uint32_t id = abc->addQName(cogen->emitNamespace(b->ns), cogen->emitString(b->name));
                uint32_t type_id = cogen->emitTypeName(compiler, b->type_name);
                switch (bindings->hd->kind) {
                    case TAG_namespaceBinding:  // FIXME: namespace bindings should be const, but the VM does not allow TAG_constBinding
                    case TAG_varBinding:
                        cogen->emitSlotTrait(id, type_id);
                        break;
                    case TAG_constBinding:
                        cogen->emitConstTrait(id, type_id);
                        break;
                    case TAG_classBinding:
                    case TAG_interfaceBinding:
                        // Handled elsewhere
                        break;
                    default:
                        compiler->internalError(0, "Unknown binding tag");
                }
            }

            for ( Seq<NamespaceDefn*>* namespaces = this->namespaces ; namespaces != NULL ; namespaces = namespaces->tl ) {
                uint32_t id = abc->addQName(cogen->emitNamespace(namespaces->hd->ns), cogen->emitString(namespaces->hd->name));
                uint32_t ns = 0;
                Expr* value = namespaces->hd->value;
                if (value == NULL)
                    ns = abc->addNamespace(CONSTANT_Namespace, cogen->emitString(compiler->intern(compiler->namespace_counter++)));
                else if (value->tag() == TAG_literalString)
                    ns = abc->addNamespace(CONSTANT_ExplicitNamespace, cogen->emitString(((LiteralString*)value)->value));
                if (tag == CODE_Program)
                    cogen->I_getlocal(0);
                else {
                    AvmAssert(activation != 0);
                    cogen->I_getlocal(activation);
                }
                if (ns != 0)
                    cogen->I_pushnamespace(ns);
                else {
                    // FIXME: semantic check for namespaces.
                    // Check that the name on the RHS is actually a ns
                    // Code is the same as for 'use default namespace'
                    // If we can't tell (name may be shadowed?) then
                    // emit code that checks at run-time.  If we can tell,
                    // then don't emit code for looking it up at run-time
                    // here, but just reference the definition of the other
                    // binding?  (That's an optimization.)
                    //value->cogen(cogen);
                    compiler->internalError(0, "Namespace should have been resolved before code generation");
                }
                cogen->I_initproperty(id);
            }
            
            if (fn) {
                uint32_t i=1;
                Seq<FunctionParam*>* params=fn->params;
                for ( ; params != NULL ; params = params->tl, i++ ) {
                    uint32_t id = abc->addQName(cogen->emitNamespace(compiler->parser.defaultNamespace()),
                                                cogen->emitString(params->hd->name));
                    AvmAssert(activation != 0);
                    cogen->I_getlocal(activation);
                    cogen->I_getlocal(i);
                    cogen->I_setproperty(id);
                }
                if (fn->uses_arguments || fn->rest_param) {
                    AvmAssert(activation != 0);
                    cogen->I_getlocal(activation);
                    cogen->I_getlocal(i);
                    if (fn->uses_arguments)
                        cogen->I_setproperty(abc->addQName(cogen->emitNamespace(compiler->parser.defaultNamespace()),
                                                           cogen->emitString(compiler->SYM_arguments)));
                    else
                        cogen->I_setproperty(abc->addQName(cogen->emitNamespace(compiler->parser.defaultNamespace()),
                                                           cogen->emitString(fn->rest_param->name)));
                }
            }
            
            for ( Seq<FunctionDefn*>* functions = this->functions ; functions != NULL ; functions = functions->tl ) {
                FunctionDefn* func = functions->hd;
                ABCMethodInfo* fn_info;
                ABCMethodBodyInfo* fn_body;
                func->cogenGuts(compiler, ctx, &fn_info, &fn_body);
                uint32_t fname = abc->addQName(cogen->emitNamespace(func->ns), cogen->emitString(func->name));
                cogen->I_getlocal(activation);
                cogen->I_newfunction(fn_info->index);
                cogen->I_setproperty(fname);
            }
            
            cogenBody(cogen, ctx, activation);
        }

        uint32_t Cogen::buildNssetWithPublic(Seq<Namespace*>* ns)
        {
            SeqBuilder<uint32_t> s(allocator);
            s.addAtEnd(compiler->NS_public);
            s.addAtEnd(compiler->NS_internal);
            while (ns != NULL) {
                if (ns->hd->tag() != TAG_commonNamespace)
                    compiler->internalError(0, "Namespace should have been resolved before now.");
                CommonNamespace* cns = (CommonNamespace*)ns->hd;
                s.addAtEnd(abc->addNamespace(CONSTANT_Namespace, abc->addString(cns->name)));
                ns = ns->tl;
            }
            return abc->addNsset(s.get());
        }
        
        uint32_t Cogen::emitNamespace(NameComponent* ns)
        {
            if (ns->tag() != TAG_builtinNamespace)
                compiler->internalError(0, "No support for user-defined namespaces yet");
            
            uint32_t nsid = compiler->NS_public;
            switch (((BuiltinNamespace*)ns)->t) {
                case T_Internal:
                    // FIXME: this could be "file internal" or "package internal"
                    // For now: file internal
                    nsid = compiler->NS_internal;
                    break;
                case T_Private:
                    nsid = compiler->NS_private;
                    break;
                case T_Protected:
                    compiler->internalError(0, "No code generation for protected namespaces yet");
                    break;
                case T_Public:
                    // FIXME: This could be "public public" or "package public"
                    // For now: public public
                    break;
            }
            return nsid;
        }

        void Program::cogenTypes(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, Seq<TypeDefn*>* classes)
        {
            cogen->I_getlocal(0);
            cogen->I_pushscope();
            for ( ; classes != NULL ; classes = classes->tl )
                cogenTypeHierarchy(cogen, ctx, global_traits, classes->hd);
            cogen->I_popscope();
        }

        void Program::cogenTypeHierarchy(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, TypeDefn* ty)
        {
            if (!ty->is_generated)
            {
                if (ty->is_searching)
                    cogen->compiler->syntaxError(ty->pos, SYNTAXERR_CIRCULAR_TYPE_HIERARCHY);
                ty->is_searching = true;
                if (ty->is_interface) {
                    InterfaceDefn* iface = static_cast<InterfaceDefn*>(ty);
                    // All interface
                    Seq<Str*>* bases = iface->extends;
                    while (bases != NULL) {
                        cogenTypeHierarchy(cogen, ctx, global_traits, findInterface(cogen, iface->pos, bases->hd));
                        bases = bases->tl;
                    }
                    cogenType(cogen, ctx, global_traits, iface);
                }
                else {
                    ClassDefn* cls = static_cast<ClassDefn*>(ty);
                    Str* base = cls->extends;
                    if (base != NULL && base != cogen->compiler->SYM_Object)
                        cogenTypeHierarchy(cogen, ctx, global_traits, findClass(cogen, cls->pos, base));
                    Seq<Str*>* bases = cls->implements;
                    while (bases != NULL) {
                        cogenTypeHierarchy(cogen, ctx, global_traits, findInterface(cogen, cls->pos, bases->hd));
                        bases = bases->tl;
                    }
                    cogenType(cogen, ctx, global_traits, cls);
                }
                ty->is_searching = false;
            }
        }

        void Program::cogenType(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, ClassDefn* cls)
        {
            Allocator* allocator = cogen->allocator;
            Compiler* compiler = cogen->compiler;
            ABCFile* abc = cogen->abc;
            
            uint32_t class_name = abc->addQName(cogen->emitNamespace(cls->ns), cogen->emitString(cls->name));
            uint32_t base_name = abc->addMultiname(compiler->NSS_public, // FIXME: should be set of open namespaces
                                                   cogen->emitString(cls->extends ? cls->extends : compiler->SYM_Object));
            uint8_t flags = ((cls->is_final ? CONSTANT_ClassFinal : 0) |
                             (cls->is_dynamic ? 0 : CONSTANT_ClassSealed));
            uint32_t protectedNS = 0;  // FIXME, if not 0 then set CONSTANT_ClassProtectedNS
            uint32_t interface_count;
            uint32_t* interfaces = interfaceRefs(cogen, cls->implements, &interface_count);
            uint32_t cinit = cogenConstructor(cogen, ctx, cls->cls_init);
            uint32_t iinit = cogenConstructor(cogen, ctx, cls->inst_init);

            ABCTraitsTable* traits = cogenClassTraits(cogen, ctx, cls->inst_methods, cls->inst_vars);
            ABCTraitsTable* ctraits = cogenClassTraits(cogen, ctx, cls->cls_methods, cls->cls_vars);    // FIXME: namespaces?

            ABCClassInfo* clsinfo = ALLOC(ABCClassInfo, (cinit, ctraits));
            ABCInstanceInfo* instinfo =
                ALLOC(ABCInstanceInfo, (class_name,
                                        base_name,
                                        flags,
                                        protectedNS,
                                        interface_count,
                                        interfaces,
                                        iinit,
                                        traits));
            uint32_t class_traits = abc->addClassAndInstance(clsinfo, instinfo);
            global_traits->addTrait(ALLOC(ABCClassTrait, (class_name, 0, class_traits)));

            int pushes=pushBaseClasses(cogen, cls);

            cogen->I_getglobalscope();
            cogen->I_findpropstrict(base_name);
            cogen->I_getproperty(base_name);
            cogen->I_newclass(class_traits);

            while (pushes--)
                cogen->I_popscope();

            cogen->I_initproperty(class_name);

            cls->is_generated = true;
        }
        
        void Program::cogenType(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, InterfaceDefn* iface)
        {
            Allocator* allocator = cogen->allocator;
            ABCFile* abc = cogen->abc;
            
            uint32_t iface_name = abc->addQName(cogen->emitNamespace(iface->ns), cogen->emitString(iface->name));
            uint32_t base_name = 0;
            uint8_t flags = CONSTANT_ClassInterface;
            uint32_t protectedNS = 0;
            uint32_t interface_count;
            uint32_t* interfaces = interfaceRefs(cogen, iface->extends, &interface_count);
            BodyInfo iface_body(allocator);
            SignatureInfo iface_signature(allocator);
            FunctionDefn* iface_cinit = ALLOC(FunctionDefn, (iface_signature, iface_body, NULL));
            uint32_t cinit = cogenConstructor(cogen, ctx, iface_cinit);  // must have a function that just returns
            iface_body.empty_body = true;
            FunctionDefn* iface_iinit = ALLOC(FunctionDefn, (iface_signature, iface_body, NULL));
            uint32_t iinit = cogenConstructor(cogen, ctx, iface_iinit);  // must have an empty body
            
            ABCTraitsTable* traits = cogenClassTraits(cogen, ctx, iface->inst_methods, NULL);
            ABCTraitsTable* ctraits = cogenClassTraits(cogen, ctx, NULL, NULL);
            
            ABCClassInfo* clsinfo = ALLOC(ABCClassInfo, (cinit, ctraits));
            ABCInstanceInfo* instinfo =
            ALLOC(ABCInstanceInfo, (iface_name,
                                    base_name,
                                    flags,
                                    protectedNS,
                                    interface_count,
                                    interfaces,
                                    iinit,
                                    traits));
            uint32_t class_traits = abc->addClassAndInstance(clsinfo, instinfo);
            global_traits->addTrait(ALLOC(ABCClassTrait, (iface_name, 0, class_traits)));
            
            cogen->I_getglobalscope();
            cogen->I_pushnull();
            cogen->I_newclass(class_traits);
            cogen->I_initproperty(iface_name);
            
            iface->is_generated = true;
        }
        
        uint32_t* Program::interfaceRefs(Cogen* cogen, Seq<Str*>* ifaces, uint32_t* interface_count)
        {
            Allocator* allocator = cogen->allocator;
            Compiler* compiler = cogen->compiler;
            ABCFile* abc = cogen->abc;
            
            uint32_t n = length(ifaces);
            uint32_t* refs = (uint32_t*)allocator->alloc(sizeof(uint32_t)*n);
            uint32_t i=0;
            while (ifaces != NULL) {
                refs[i] = abc->addMultiname(compiler->NSS_public, cogen->emitString(ifaces->hd));
                i++;
                ifaces = ifaces->tl;
            }
            *interface_count = n;
            return refs;
        }
        
        ABCTraitsTable* Program::cogenClassTraits(Cogen* cogen, Ctx* ctx, Seq<FunctionDefn*>* inst_methods, Seq<Binding*>* inst_vars)
        {
            Allocator* allocator = cogen->allocator;
            Compiler* compiler = cogen->compiler;
            ABCFile* abc = cogen->abc;
            ABCTraitsTable* traits = ALLOC(ABCTraitsTable, (compiler));
            for ( Seq<FunctionDefn*>* ms = inst_methods ; ms != NULL ; ms = ms->tl ) {
                ABCMethodInfo* fn_info;
                ABCMethodBodyInfo* fn_body;
                ms->hd->cogenGuts(cogen->compiler, ctx, true, &fn_info, &fn_body);
                traits->addTrait(ALLOC(ABCMethodTrait, (abc->addQName(cogen->emitNamespace(ms->hd->ns), cogen->emitString(ms->hd->name)),
                                                        fn_info->index)));
            }
            for ( Seq<Binding*>* bs = inst_vars ; bs != NULL ; bs = bs->tl ) {
                Binding* b = bs->hd;
                if (b->kind != TAG_varBinding && b->kind != TAG_constBinding)
                    continue;
                traits->addTrait(ALLOC(ABCSlotTrait, (abc->addQName(cogen->emitNamespace(b->ns), cogen->emitString(b->name)),
                                                      0, // FIXME: the type
                                                      (b->kind == TAG_varBinding ? TRAIT_Slot : TRAIT_Const))));
            }
            return traits;
        }
        
        uint32_t Program::cogenConstructor(Cogen* cogen, Ctx* ctx, FunctionDefn* constructor)
        {
            ABCMethodInfo* fn_info;
            ABCMethodBodyInfo* fn_body;
            constructor->cogenGuts(cogen->compiler, ctx, true, &fn_info, &fn_body);
            return fn_info->index;
        }

        int Program::pushBaseClasses(Cogen* cogen, ClassDefn* currcls)
        {
            Compiler* compiler = cogen->compiler;
            ABCFile* abc = &compiler->abc;
            Str* base = currcls->extends;
            if (base == NULL)
                base = compiler->SYM_Object;
            int n = 0;
            if (base != compiler->SYM_Object)
                n = pushBaseClasses(cogen, findClass(cogen, currcls->pos, base));
            uint32_t basenm = abc->addMultiname(compiler->NSS_public, cogen->emitString(base));
            cogen->I_findpropstrict(basenm);
            cogen->I_getproperty(basenm);
            cogen->I_pushscope();
            return n+1;
        }

        // FIXME: These need to know about imported classes and interfaces!  They need not always return
        // a full ClassDefn / InterfaceDefn, just a rump;
        // mostly we want the names of the base classes and interfaces.  The "imported" types
        // are the types already present in the environment (in the run-time compiler) or the
        // actually imported types from an ABC, in the batch compiler.  Looks like there will be some
        // hook in the HostContext.

        ClassDefn* Program::findClass(Cogen* cogen, uint32_t pos, Str* name)
        {
            for ( Seq<TypeDefn*>* classes=this->classes ; classes != NULL ; classes=classes->tl ) {
                if (classes->hd->name == name) {
                    if (classes->hd->is_interface)
                        break;
                    return static_cast<ClassDefn*>(classes->hd);
                }
            }
            cogen->compiler->syntaxError(pos, SYNTAXERR_MISSING_BASE_CLASS);
            /*NOTREACHED*/
            return NULL;
        }

        InterfaceDefn* Program::findInterface(Cogen* cogen, uint32_t pos, Str* name)
        {
            for ( Seq<TypeDefn*>* classes=this->classes ; classes != NULL ; classes=classes->tl ) {
                if (classes->hd->name == name) {
                    if (!classes->hd->is_interface)
                        break;
                    return static_cast<InterfaceDefn*>(classes->hd);
                }
            }
            cogen->compiler->syntaxError(pos, SYNTAXERR_MISSING_BASE_CLASS);
            /*NOTREACHED*/
            return NULL;
        }
    }
}

#endif // VMCFG_EVAL
