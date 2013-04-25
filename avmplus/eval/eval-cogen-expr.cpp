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
        
        // Abstracts the name computation.  For stack allocation.
        
        class Name {
        public:
            Name(Cogen* cogen, Ctx* ctx, Expr* expr, bool strict);
            Name(Cogen* cogen, Ctx* ctx, QualifiedName* name);
            ~Name();
            
            void setup();
            
            Cogen * const cogen;
            uint32_t nsreg;
            uint32_t namereg;
            uint32_t sym;

        private:
            void computeName(QualifiedName* qname, Ctx* ctx);
        };

        // Compute the base object (on the stack) and the name information.
        Name::Name(Cogen* cogen, Ctx* ctx, Expr* expr, bool strict)
            : cogen(cogen)
            , nsreg(0)
            , namereg(0)
        {
            Tag tag = expr->tag();
            AvmAssert( tag == TAG_objectRef || tag == TAG_qualifiedName );
            if (tag == TAG_objectRef)
                ((ObjectRef*)expr)->obj->cogen(cogen, ctx);
            computeName((tag == TAG_objectRef ? ((ObjectRef*)expr)->name : (QualifiedName*)expr), ctx);
            if (tag == TAG_qualifiedName) {
                if (strict)
                    cogen->I_findpropstrict(sym);
                else
                    cogen->I_findproperty(sym);
            }
        }

        // Compute the name information.
        Name::Name(Cogen* cogen, Ctx* ctx, QualifiedName* qname)
            : cogen(cogen)
            , nsreg(0)
            , namereg(0)
        {
            computeName(qname, ctx);
        }
        
        // Significant performance improvements for qualified names if the namespace can be resolved at compile time.
        // See notes in the implementation of the 'namespace' definition; the machinery has to be implemented for
        // 'use default namespace' in any case.

        void Name::computeName(QualifiedName* qname, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            bool ns_wildcard = false;
            bool ns_builtin = false;
            if (qname->qualifier != NULL) {
                switch (qname->qualifier->tag()) {
                    case TAG_simpleName: {
                        uint32_t id = cogen->abc->addQName(compiler->NS_public,
                                                           cogen->emitString(((SimpleName*)(qname->qualifier))->name),
                                                           false);
                        nsreg = cogen->getTemp();
                        cogen->I_findpropstrict(id);
                        cogen->I_getproperty(id);
                        cogen->I_coerce(compiler->ID_Namespace);
                        cogen->I_setlocal(nsreg);
                        break;
                    }
                    case TAG_wildcardName:
                        ns_wildcard = true;
                        break;
                    case TAG_builtinNamespace:
                        ns_builtin = true;
                        break;
                    default:
                        compiler->internalError(qname->pos, "QName qualifiers can't be computed names");
                }
            }
            NameComponent* name = qname->name;
            switch (name->tag()) {
                case TAG_simpleName:
                    if (nsreg != 0) {
                        AvmAssert(!ns_wildcard && !ns_builtin);
                        sym = cogen->abc->addRTQName(cogen->emitString(((SimpleName*)name)->name),
                                                     qname->is_attr);
                    }
                    else if (ns_builtin) {
                        AvmAssert(!ns_wildcard);
                        uint32_t ns = cogen->emitNamespace(qname->qualifier);
                        sym = cogen->abc->addQName(ns,
                                                   cogen->emitString(((SimpleName*)name)->name),
                                                   qname->is_attr);
                    }
                    else {
                        VarScopeCtx* vs = ctx->findVarScope();
                        // here we either have just the public namespace, or the public namespace
                        // plus some open namespaces; in the latter case we need to generate a
                        // multiname presumably.  We don't want to have to call addNsset here
                        // every time so be sure to cache the nsset for the current scope somewhere.
                        if (vs->nsset != 0 && !ns_wildcard && !qname->is_attr)
                            sym = cogen->abc->addMultiname(vs->nsset,
                                                           cogen->emitString(((SimpleName*)name)->name),
                                                           false);
                        else
                            sym = cogen->abc->addQName((ns_wildcard ? 0 : compiler->NS_public),
                                                       cogen->emitString(((SimpleName*)name)->name),
                                                       qname->is_attr);
                    }
                    break;
                case TAG_wildcardName:
                    if (nsreg != 0)
                        sym = cogen->abc->addRTQName(0, qname->is_attr);
                    else
                        sym = cogen->abc->addMultiname(compiler->NSS_public, 0, qname->is_attr);
                    break;
                case TAG_computedName:
                    if (ns_wildcard)
                        compiler->syntaxError(qname->pos, SYNTAXERR_ILLEGAL_QNAME);
                    namereg = cogen->getTemp();
                    ((ComputedName*)name)->expr->cogen(cogen, ctx);
                    cogen->I_setlocal(namereg);
                    if (nsreg != 0)
                        sym = cogen->abc->addRTQNameL(qname->is_attr);
                    else {
                        if (qname->is_attr)
                            sym = compiler->MNL_public_attr;
                        else
                            sym = compiler->MNL_public;
                    }
                    break;
            }
        }
        
        Name::~Name()
        {
            if (nsreg != 0) cogen->I_kill(nsreg);
            if (namereg != 0) cogen->I_kill(namereg);
        }
        
        void Name::setup()
        {
            if (nsreg)
                cogen->I_getlocal(nsreg);
            if (namereg)
                cogen->I_getlocal(namereg);
        }
        
        void SimpleType::cogen(Cogen* cogen, Ctx* ctx)
        {
            name->cogen(cogen, ctx);
        }

        void InstantiatedType::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            Cogen::checkVectorType(compiler, basename);
            cogen->I_findpropstrict(compiler->ID_Vector);
            cogen->I_getproperty(compiler->ID_Vector);
            // FIXME: representing * as NULL is an anti-pattern (also elsewhere)
            if (tparam == NULL)
                cogen->I_pushnull();
            else
                tparam->cogen(cogen, ctx);
            tparam->cogen(cogen, ctx);
            cogen->I_applytype(1);
        }

        void QualifiedName::cogen(Cogen* cogen, Ctx* ctx)
        {
            Name n(cogen, ctx, this);
            n.setup();
            cogen->I_findpropstrict(n.sym);
            n.setup();
            cogen->I_getproperty(n.sym);
        }
        
        void ObjectRef::cogen(Cogen* cogen, Ctx* ctx)
        {
            obj->cogen(cogen, ctx);
            if ((name->qualifier == NULL || name->qualifier->tag() == TAG_wildcardName) && name->name->tag() == TAG_wildcardName && !name->is_attr)
                cogen->I_callproperty(cogen->compiler->ID_children, 0);
            else {
                Name n(cogen, ctx, name);
                n.setup();
                cogen->I_getproperty(n.sym);
            }
        }
        
        void InstantiatedTypeRef::cogen(Cogen* cogen, Ctx* ctx)
        {
            type->cogen(cogen, ctx);
        }

        void RefLocalExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            cogen->I_getlocal(local);
        }

        void ConditionalExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* L0 = cogen->newLabel();
            Label* L1 = cogen->newLabel();
            
            e1->cogen(cogen, ctx);
            cogen->I_iffalse(L0);
            e2->cogen(cogen, ctx);
            cogen->I_coerce_a();
            cogen->I_jump(L1);
            cogen->I_label(L0);
            e3->cogen(cogen, ctx);
            cogen->I_coerce_a();
            cogen->I_label(L1);
        }

        void AssignExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            AvmAssert( lhs->tag() == TAG_objectRef || lhs->tag() == TAG_qualifiedName );

            // Compute the object onto the stack, and elements of the name into locals if necessary
            bool is_assign = op == OPR_assign || op == OPR_init;
            Name n(cogen, ctx, lhs, !is_assign);

            // Read the value if we need it
            if (!is_assign) {
                cogen->I_dup();
                n.setup();
                cogen->I_getproperty(n.sym);
            }
            
            // Compute the rhs
            rhs->cogen(cogen, ctx);
            
            // Compute the operator if we need it
            if (!is_assign) {
                bool isNegated;
                cogen->I_opcode(cogen->binopToOpcode(op, &isNegated));
                if (isNegated)
                    cogen->I_not();
            }
            
            // Perform the update and generate the result
            uint32_t t = cogen->getTemp();
            cogen->I_setlocal(t);
            n.setup();
            cogen->I_getlocal(t);
            if (op == OPR_assign)
                cogen->I_setproperty(n.sym);
            else
                cogen->I_initproperty(n.sym);
            cogen->I_getlocal(t);
            cogen->I_kill(t);
        }

        void BinaryExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            if (op == OPR_logicalAnd) {
                Label* L0 = cogen->newLabel();
                
                lhs->cogen(cogen, ctx);
                cogen->I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
                cogen->I_dup();
                cogen->I_coerce_b();
                cogen->I_iffalse(L0);
                cogen->I_pop();
                rhs->cogen(cogen, ctx);
                cogen->I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
                cogen->I_label(L0);
            }
            else if (op == OPR_logicalOr) {
                Label* L0 = cogen->newLabel();
                
                lhs->cogen(cogen, ctx);
                cogen->I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
                cogen->I_dup();
                cogen->I_coerce_b();
                cogen->I_iftrue(L0);
                cogen->I_pop();
                rhs->cogen(cogen, ctx);
                cogen->I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
                cogen->I_label(L0);
            }
            else if (op == OPR_comma) {
                lhs->cogen(cogen, ctx);
                cogen->I_pop();
                rhs->cogen(cogen, ctx);
            }
            else {
                lhs->cogen(cogen, ctx);
                rhs->cogen(cogen, ctx);
                bool isNegated;
                cogen->I_opcode(cogen->binopToOpcode(op, &isNegated));
                if (isNegated)
                    cogen->I_not();
            }
        }

        void UnaryExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            switch (op) {
                case OPR_delete: {
                    if (expr->tag() == TAG_qualifiedName || expr->tag() == TAG_objectRef) {
                        Name n(cogen, ctx, expr, false);
                        n.setup();
                        cogen->I_deleteproperty(n.sym);
                    }
                    else {
                        // FIXME: could be a TAG_instantiatedType
                        // FIXME: e4x requires that if the value computed here is an XMLList then a TypeError (ID 1119) is thrown.
                        expr->cogen(cogen, ctx);
                        cogen->I_pop();
                        cogen->I_pushtrue();
                    }
                    break;
                }
                    
                case OPR_void:
                    expr->cogen(cogen, ctx);
                    cogen->I_pop();
                    cogen->I_pushundefined();
                    break;

                case OPR_typeof:
                    if (expr->tag() == TAG_qualifiedName) {
                        Name n(cogen, ctx, (QualifiedName*)expr);
                        n.setup();
                        cogen->I_findproperty(n.sym);
                        n.setup();
                        cogen->I_getproperty(n.sym);
                    }
                    else
                        expr->cogen(cogen, ctx);
                    cogen->I_typeof();
                    break;

                case OPR_preIncr:
                    incdec(cogen, ctx, true, true);
                    break;
                    
                case OPR_preDecr:
                    incdec(cogen, ctx, true, false);
                    break;
                    
                case OPR_postIncr:
                    incdec(cogen, ctx, false, true);
                    break;
                    
                case OPR_postDecr:
                    incdec(cogen, ctx, false, false);
                    break;
                    
                case OPR_unplus:
                    expr->cogen(cogen, ctx);
                    cogen->I_coerce_d();
                    break;
                    
                case OPR_unminus:
                    expr->cogen(cogen, ctx);
                    cogen->I_negate();
                    break;
                    
                case OPR_bitwiseNot:
                    expr->cogen(cogen, ctx);
                    cogen->I_bitnot();
                    break;
                    
                case OPR_not:
                    expr->cogen(cogen, ctx);
                    cogen->I_not();
                    break;
                    
                default:
                    compiler->internalError(pos, "Unrecognized unary operation");
            }
        }
        
        void UnaryExpr::incdec(Cogen* cogen, Ctx* ctx, bool pre, bool inc)
        {
            (void)ctx;
            Name n(cogen, ctx, expr, true);
            cogen->I_dup();
            n.setup();
            cogen->I_getproperty(n.sym);

            uint32_t t = cogen->getTemp();

            if (pre) {
                if (inc)
                    cogen->I_increment();
                else
                    cogen->I_decrement();
                cogen->I_dup();
                cogen->I_setlocal(t);
            }
            else {
                // Postfix ops return value after conversion to number.
                cogen->I_coerce_d();
                cogen->I_dup();
                cogen->I_setlocal(t);
                if (inc)
                    cogen->I_increment();
                else
                    cogen->I_decrement();
            }

            n.setup();
            cogen->I_setproperty(n.sym);

            cogen->I_getlocal(t);
            cogen->I_kill(t);
        }
        
        void ThisExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            cogen->I_getlocal(0);
        }
        
        void LiteralFunction::cogen(Cogen* cogen, Ctx* ctx)
        {
            if (function->name != NULL) {
                // For a named function expression F with name N, create a new
                // expression (function() { F; return N })() and generate code
                // for that instead.  Note that F then becomes a local function
                // definition.
                Allocator* allocator = cogen->allocator;
                SignatureInfo signature(allocator);
                BodyInfo body(allocator);
                BuiltinNamespace* ns = cogen->compiler->parser.defaultNamespace();
                body.bindings.addAtEnd(ALLOC(Binding, (ns, function->name, NULL, TAG_varBinding)));
                body.functionDefinitions.addAtEnd(function);
                body.stmts.addAtEnd(ALLOC(ReturnStmt,
                                            (0, ALLOC(QualifiedName,
                                                      (ns, ALLOC(SimpleName,
                                                                    (function->name)),
                                                       false,
                                                       0)))));
                Expr* e = ALLOC(CallExpr,
                                (ALLOC(LiteralFunction,
                                       (ALLOC(FunctionDefn, (signature, body)))),
                                 NULL,
                                 0));
                e->cogen(cogen, ctx);
            }
            else {
                ABCMethodInfo* fn_info;
                ABCMethodBodyInfo* fn_body;
                function->cogenGuts(cogen->compiler, ctx, &fn_info, &fn_body);
                cogen->I_newfunction(fn_info->index);
            }
        }

        void LiteralObject::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            uint32_t i=0;
            for ( Seq<LiteralField*>* fields = this->fields ; fields != NULL ; fields = fields->tl ) {
                cogen->I_pushstring(cogen->emitString(fields->hd->name));
                fields->hd->value->cogen(cogen, ctx);
                i++;
            }
            cogen->I_newobject(i);
        }
        
        void LiteralArray::cogen(Cogen* cogen, Ctx* ctx)
        {
            uint32_t i = 0;
            Seq<Expr*>* exprs = elements;
            Compiler* compiler = cogen->compiler;
            
            // Use newarray to construct the dense prefix
            for ( ; exprs != NULL ; exprs = exprs->tl ) {
                Expr* e = exprs->hd;
                if (e == NULL)
                    break;
                e->cogen(cogen, ctx);
                i++;
            }
            cogen->I_newarray(i);
            
            // Then init the other defined slots one by one
            if (exprs != NULL) {
                bool last_was_undefined = false;
                for ( ; exprs != NULL ; exprs = exprs->tl, i++ ) {
                    Expr* e = exprs->hd;
                    if (e != NULL) {
                        cogen->I_dup();
                        e->cogen(cogen, ctx);
                        cogen->I_setproperty(cogen->abc->addQName(compiler->NS_public, cogen->emitString(compiler->intern(i))));
                        last_was_undefined = false;
                    }
                    else
                        last_was_undefined = true;
                }
                if (last_was_undefined) {
                    cogen->I_dup();
                    cogen->I_pushint(cogen->emitInt(i));
                    cogen->I_setproperty(compiler->ID_length);
                }
            }
        }

        void LiteralVector::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            cogen->I_findpropstrict(compiler->ID_Vector);
            cogen->I_getproperty(compiler->ID_Vector);
            // FIXME: representing * as NULL is an anti-pattern (also elsewhere)
            if (type == NULL)
                cogen->I_pushnull();
            else
                type->cogen(cogen, ctx);
            cogen->I_applytype(1);
            cogen->I_pushint(cogen->emitInt(length(this->elements)));
            cogen->I_construct(1);
            int32_t offset=0;
            for ( Seq<Expr*>* elements = this->elements ; elements != NULL ; elements = elements->tl ) {
                cogen->I_dup();
                cogen->I_pushint(cogen->emitInt(offset));
                elements->hd->cogen(cogen, ctx);
                cogen->I_setproperty(compiler->MNL_public);
                offset++;
            }
        }

        void LiteralRegExp::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            
            Compiler* compiler = cogen->compiler;
            
            // value is "/.../flags"
            //
            // OPTIMIZEME: silly to recompile the regular expression every time it's evaluated, even if
            // ES3.1 allows it (not sure what AS3 allows / requires; ES3 requires compilation once).
            const wchar* s = value->s;
            const wchar* t = s + value->length - 1;
            while (*t != '/')
                t--;
            
            // Creating a new RegExp object every time is not compatible with ES3, but it is
            // what ASC does, and with luck ES3.1 will change to match this behavior.
            //
            // FIXME: semantics: findpropstrict(""::RegExp) is not quite right here.
            // Doing so creates a spoofing hole / surprising trap.  We want an OP_newregexp instruction.
            cogen->I_findpropstrict(compiler->ID_RegExp);
            cogen->I_pushstring(cogen->emitString(compiler->intern(s+1, uint32_t(t-s-1))));
            cogen->I_pushstring(cogen->emitString(compiler->intern(t+1, uint32_t(value->length-(t-s+1)))));
            cogen->I_constructprop(compiler->ID_RegExp, 2);
        }

        void LiteralNull::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            cogen->I_pushnull();
        }
        
        void LiteralUndefined::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            cogen->I_pushundefined();
        }
        
        void LiteralInt::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            if (value >= -128 && value < 128)
                cogen->I_pushbyte((uint8_t)(value & 0xFF));
            else
                cogen->I_pushint(cogen->emitInt(value));
        }

        void LiteralUInt::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            if (value < 128)
                cogen->I_pushbyte((uint8_t)(value & 0xFF));
            else
                cogen->I_pushuint(cogen->emitUInt(value));
        }
        
        void LiteralDouble::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            if (MathUtils::isNaN(value))
                cogen->I_pushnan();
            else
                cogen->I_pushdouble(cogen->emitDouble(value));
        }
        
#ifdef VMCFG_FLOAT
        void LiteralFloat::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            // Handle NaN same as other values
            cogen->I_pushfloat(cogen->emitFloat(value));
        }
#endif

        void LiteralBoolean::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            if (value)
                cogen->I_pushtrue();
            else
                cogen->I_pushfalse();
        }

        void LiteralString::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            cogen->I_pushstring(cogen->emitString(value));
        }

        uint32_t Cogen::arguments(Seq<Expr*>* args, Ctx* ctx)
        {
            uint32_t i = 0;
            for ( ; args != NULL ; args = args->tl, i++ )
                args->hd->cogen(this, ctx);
            return i;
        }

        void CallExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            switch (fn->tag()) {
                case TAG_qualifiedName: {
                    // This code is incorrect if the name that's being referenced is
                    // bound by 'with', because in that case the binding object should be
                    // pushed as the receiver object (according to ES-262).  But the AVM+
                    // does not have an instruction that performs the correct operation:
                    // callproplex passes NULL as the receiver object, while callproperty
                    // passes a non-NULL object.  So in the context of a WITH we would
                    // have to simulate the correct behavior by performing a scope chain
                    // walk, querying each WITH object for the property and calling it
                    // if present, otherwise calling the function if it is lexically bound,
                    // otherwise calling the global function.  ASC has the same problem
                    // (and also does not solve it), so no actual bug here, just an
                    // incompatibility.
                    Name n(cogen, ctx, fn, true);
                    n.setup();
                    cogen->I_callproplex(n.sym, cogen->arguments(arguments, ctx));
                    break;
                }
                case TAG_objectRef: {
                    Name n(cogen, ctx, fn, false);
                    n.setup();
                    cogen->I_callproperty(n.sym, cogen->arguments(arguments, ctx));
                    break;
                }
                default:
                    fn->cogen(cogen, ctx);
                    cogen->I_pushnull();
                    cogen->I_call(cogen->arguments(arguments, ctx));
            }
        }

        void NewExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            fn->cogen(cogen, ctx);
            cogen->I_construct(cogen->arguments(arguments, ctx));
        }

        void XmlInitializer::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            uint32_t id = is_list ? compiler->ID_XMLList : compiler->ID_XML;
            
            cogen->I_findpropstrict(id);
            cogen->I_getproperty(id);
            cogen->I_pushstring(cogen->emitString(compiler->SYM_));
            for ( Seq<Expr*>* exprs = this->exprs ; exprs != NULL ; exprs = exprs->tl ) {
                exprs->hd->cogen(cogen, ctx);
                cogen->I_convert_s();
                cogen->I_add();
            }
#if 0 && defined DEBUG
            cogen->I_dup();
            cogen->I_findpropstrict(compiler->ID_print);
            cogen->I_swap();
            cogen->I_callpropvoid(compiler->ID_print, 1);
#endif
            cogen->I_construct(1);
        }
        
        void EscapeExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            expr->cogen(cogen, ctx);
            switch (esc) {
                case ESC_attributeValue:
                    cogen->I_esc_xattr();
                    break;
                case ESC_elementValue:
                    cogen->I_esc_xelem();
                    break;
                default:
                    break;
            }
        }

        // OPTIMIZEME?  A more space-conserving method would be to pass a predicate
        // and an object to a common filter function.  But it only makes a difference
        // if filter expressions are very common, and they probably aren't.
        
        void FilterExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            Compiler* compiler = cogen->compiler;
            uint32_t t_xmllist = cogen->getTemp();
            uint32_t t_length = cogen->getTemp();
            uint32_t t_result = cogen->getTemp();
            uint32_t t_result_index = cogen->getTemp();
            uint32_t t_index = cogen->getTemp();
            uint32_t t_scope = cogen->getTemp();
            uint32_t t_item = cogen->getTemp();
            Label* L_again = cogen->newLabel();
            Label* L_skip = cogen->newLabel();
            Label* L_done = cogen->newLabel();

            obj->cogen(cogen, ctx);
            cogen->I_checkfilter();

            // convert to XMLList
            cogen->I_coerce_a();
            cogen->I_setlocal(t_xmllist);
            cogen->I_findpropstrict(compiler->ID_XMLList);
            cogen->I_getproperty(compiler->ID_XMLList);
            cogen->I_pushnull();
            cogen->I_getlocal(t_xmllist);
            cogen->I_call(1);
            cogen->I_coerce_a();
            cogen->I_setlocal(t_xmllist);
            
            // get the length of the list and save it
            cogen->I_getlocal(t_xmllist);
            cogen->I_callproperty(compiler->ID_length, 0);
            cogen->I_coerce_a();
            cogen->I_setlocal(t_length);
            
            // create a new, empty list for the result
            cogen->I_findpropstrict(compiler->ID_XMLList);
            cogen->I_getproperty(compiler->ID_XMLList);
            cogen->I_construct(0);
            cogen->I_coerce_a();
            cogen->I_setlocal(t_result);
            cogen->I_pushbyte(0);
            cogen->I_setlocal(t_result_index);
            
            // set up loop
            cogen->I_pushbyte(0);
            cogen->I_setlocal(t_index);
            
            // iterate across the list
            cogen->I_label(L_again);
            
            // while index < length
            cogen->I_getlocal(t_index);
            cogen->I_getlocal(t_length);
            cogen->I_ifge(L_done);
            
            // item := list[index]
            cogen->I_getlocal(t_xmllist);
            cogen->I_getlocal(t_index);
            cogen->I_getproperty(compiler->MNL_public);
            cogen->I_coerce_a();
            cogen->I_setlocal(t_item);
            
            // with (item) b := <filter>
            cogen->I_getlocal(t_item);
            cogen->I_pushwith();
            filter->cogen(cogen, ctx);
            cogen->I_popscope();
            
            // if b result += item
            cogen->I_iffalse(L_skip);
            // Add it
            cogen->I_getlocal(t_result);
            cogen->I_getlocal(t_result_index);
            cogen->I_getlocal(t_item);
            cogen->I_setproperty(compiler->MNL_public);
            cogen->I_inclocal_i(t_result_index);
            
            cogen->I_label(L_skip);
            
            // index++;
            cogen->I_inclocal_i(t_index);
            cogen->I_jump(L_again);
            
            // post-loop
            cogen->I_label(L_done);
            cogen->I_getlocal(t_result);
            cogen->I_kill(t_xmllist);
            cogen->I_kill(t_length);
            cogen->I_kill(t_result);
            cogen->I_kill(t_result_index);
            cogen->I_kill(t_index);
            cogen->I_kill(t_scope);
            cogen->I_kill(t_item);
        }
        
        void DescendantsExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            obj->cogen(cogen, ctx);
            Name n(cogen, ctx, name);
            n.setup();
            cogen->I_getdescendants(n.sym);
        }
        
        void SuperExpr::cogen(Cogen* cogen, Ctx* ctx)
        {
            (void)ctx;
            Compiler* compiler = cogen->compiler;
            compiler->internalError(pos, "Unimplemented: superExpr");
        }
    }
}

#endif // VMCFG_EVAL
