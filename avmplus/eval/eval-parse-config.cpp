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
        Parser::ConfigBinding::ConfigBinding(Str* ns, Str* name, Expr* value)
            : ns(ns)
            , name(name)
            , value(value)
        {
        }

        void Parser::addConfigNamespace(Str* ns)
        {
            configNamespaces = ALLOC(Seq<Str*>, (ns, configNamespaces));
        }

        void Parser::checkNoShadowingOfConfigNamespaces(uint32_t pos, Str* s)
        {
            Seq<Str*>* nss = configNamespaces;
            while (nss != NULL) {
                if (s == nss->hd)
                    compiler->syntaxError(pos, SYNTAXERR_CONFIG_NAMESPACE_SHADOWING);
                nss = nss->tl;
            }
        }

        void Parser::addConfigBinding(Str* ns, Str* name, Expr* value)
        {
            configBindings = ALLOC(Seq<ConfigBinding*>, (ALLOC(ConfigBinding, (ns, name, value)), configBindings));
        }

        bool Parser::findConfigNamespace(Str* ns)
        {
            Seq<Str*>* nss = configNamespaces;
            while (nss != NULL) {
                if (nss->hd == ns)
                    return true;
                nss = nss->tl;
            }
            return false;
        }
        
        Expr* Parser::findConfigBinding(Str* ns, Str* name)
        {
            Seq<ConfigBinding*>* bs = configBindings;
            while (bs != NULL) {
                if (bs->hd->ns == ns && bs->hd->name == name)
                    return bs->hd->value;
                bs = bs->tl;
            }
            return NULL;
        }
        
        bool Parser::isConfigReference(Expr* e)
        {
            if (e->tag() != TAG_qualifiedName)
                return false;
            QualifiedName* qn = (QualifiedName*)e;
            if (qn->qualifier == NULL || qn->qualifier->tag() != TAG_simpleName || qn->name->tag() != TAG_simpleName)
                return false;
            SimpleName* sn = (SimpleName*)(qn->qualifier);
            return findConfigNamespace(sn->name);
        }
        
        bool Parser::evaluateConfigReference(QualifiedName* qname)
        {
            if (qname == NULL)
                return true;

            AvmAssert(isConfigReference(qname));
            
            Str* ns = ((SimpleName*)qname->qualifier)->name;
            Str* name = ((SimpleName*)qname->name)->name;
            Expr* value = findConfigBinding(ns, name);
            if (value != NULL)
                return evaluateToBoolean(value);

            compiler->syntaxError(qname->pos, SYNTAXERR_UNBOUND_CONST_NAME);
            /*NOTREACHED*/
            return false;
        }

        // POSSIBLE EXTENSION: structured data and field access
        //
        // It would be possible to support array, object, xml, and vector initializers
        // whose initializing subexpressions are either literal values or references
        // to config variables, as well as field selections on those initializers.
        // It's possible to imagine some use cases.  We need some clean semantics for
        // when those initializers escape into non-config code - do we want a single
        // copy or multiple copies?

        Expr* Parser::evaluateConfigDefinition(Str* ns, Expr* e)
        {
            // e is evaluated in an environment containing only config bindings.
            // ns is the default namespace: it will be used to qualify any unqualified name.
            
            switch (e->tag()) {
                case TAG_literalUndefined:
                case TAG_literalString:
                case TAG_literalNull:
                case TAG_literalUInt:
                case TAG_literalInt:
                case TAG_literalDouble:
                case TAG_literalFloat:
                case TAG_literalBoolean:
                    return e;
                case TAG_simpleName: {
                    Expr* expr = findConfigBinding(ns, ((SimpleName*)e)->name);
                    if (expr == NULL)
                        compiler->syntaxError(expr->pos, SYNTAXERR_UNBOUND_CONST_NAME);
                    return expr;
                }
                case TAG_qualifiedName: {
                    QualifiedName* qname = (QualifiedName*)e;
                    if (qname->qualifier->tag() != TAG_simpleName || qname->name->tag() != TAG_simpleName)
                        compiler->syntaxError(e->pos, SYNTAXERR_UNBOUND_CONST_NAME);    // Specifically an illegal const name
                    Str* ns = ((SimpleName*)qname->qualifier)->name;
                    Str* name = ((SimpleName*)qname->name)->name;
                    Expr* value = findConfigBinding(ns, name);
                    if (value == NULL)
                        compiler->syntaxError(e->pos, SYNTAXERR_UNBOUND_CONST_NAME);
                    return value;
                }
                case TAG_binaryExpr: {
                    // CLARIFICATION: no short-circuiting
                    //
                    // We evaluate both sides of && and || in order to uncover
                    // any undefined variables lurking in non-taken branches.
                    BinaryExpr* binary = (BinaryExpr*)e;
                    Expr* lhs = evaluateConfigDefinition(ns, binary->lhs);
                    Expr* rhs = evaluateConfigDefinition(ns, binary->rhs);
                    switch (binary->op) {
                        case OPR_plus:
                            if (lhs->tag() == TAG_literalString || rhs->tag() == TAG_literalString) {
                                StringBuilder b(compiler);
                                b.append(((LiteralString*)lhs)->value);
                                b.append(((LiteralString*)rhs)->value);
                                return boxString(b.str());
                            }
                            return boxDouble(evaluateToNumber(lhs) + evaluateToNumber(rhs));
                        case OPR_minus:
                            return boxDouble(evaluateToNumber(lhs) - evaluateToNumber(rhs));
                        case OPR_multiply:
                            return boxDouble(evaluateToNumber(lhs) * evaluateToNumber(rhs));
                        case OPR_divide:
                            return boxDouble(evaluateToNumber(lhs) / evaluateToNumber(rhs));
                        case OPR_remainder:
                            return boxDouble(fmod(evaluateToNumber(lhs), evaluateToNumber(rhs)));
                        case OPR_leftShift:
                            return boxInt(evaluateToInt32(lhs) << (evaluateToUInt32(rhs) & 0x1F));
                        case OPR_rightShift:
                            return boxInt(evaluateToInt32(lhs) >> (evaluateToUInt32(rhs) & 0x1F));
                        case OPR_rightShiftUnsigned:
                            return boxUInt(evaluateToUInt32(lhs) >> (evaluateToUInt32(rhs) & 0x1F));
                        case OPR_bitwiseAnd:
                            return boxInt(evaluateToInt32(lhs) & evaluateToInt32(rhs));
                        case OPR_bitwiseOr:
                            return boxInt(evaluateToInt32(lhs) | evaluateToInt32(rhs));
                        case OPR_bitwiseXor:
                            return boxInt(evaluateToInt32(lhs) ^ evaluateToInt32(rhs));
                        case OPR_logicalAnd:
                            return boxBoolean(int(evaluateToBoolean(lhs)) + int(evaluateToBoolean(rhs)) == 2);
                        case OPR_logicalOr:
                            return boxBoolean(int(evaluateToBoolean(lhs)) + int(evaluateToBoolean(rhs)) != 0);
                        case OPR_less: {
                            int r = evaluateRelational(lhs, rhs);
                            return boxBoolean(r == -1 || r == 0 ? false : true);
                        }
                        case OPR_greater: {
                            int r = evaluateRelational(rhs, lhs);
                            return boxBoolean(r == -1 || r == 0 ? false : true);
                        }
                        case OPR_lessOrEqual: {
                            int r = evaluateRelational(rhs, lhs);
                            return boxBoolean(r == -1 || r == 1 ? false : true);
                        }
                        case OPR_greaterOrEqual: {
                            int r = evaluateRelational(lhs, rhs);
                            return boxBoolean(r == -1 || r == 1 ? false : true);
                        }
                        case OPR_equal:
                        case OPR_notEqual:
                        case OPR_strictEqual:
                        case OPR_strictNotEqual: {
                            if (lhs->tag() == TAG_literalInt || lhs->tag() == TAG_literalUInt)
                                lhs = boxDouble(evaluateToNumber(lhs));
                            if (rhs->tag() == TAG_literalInt || rhs->tag() == TAG_literalUInt)
                                rhs = boxDouble(evaluateToNumber(rhs));

                            bool equality;
                            if (binary->op == OPR_equal || binary->op == OPR_notEqual)
                                equality = binary->op == OPR_equal;
                            else
                                equality = binary->op == OPR_strictEqual;
                            
                            if (lhs->tag() != rhs->tag()) {
                                if (binary->op == OPR_equal || binary->op == OPR_notEqual) {
                                    if ((lhs->tag() == TAG_literalUndefined && rhs->tag() == TAG_literalNull) ||
                                        (lhs->tag() == TAG_literalNull && rhs->tag() == TAG_literalUndefined))
                                        return boxBoolean(true == equality);
                                    if ((lhs->tag() == TAG_literalString && rhs->tag() == TAG_literalDouble) ||
                                        (lhs->tag() == TAG_literalDouble && rhs->tag() == TAG_literalString))
                                        return boxBoolean((evaluateToNumber(lhs) == evaluateToNumber(rhs)) == equality);
                                    if (lhs->tag() == TAG_literalBoolean || rhs->tag() == TAG_literalBoolean)
                                        return boxBoolean((evaluateToBoolean(lhs) == evaluateToBoolean(rhs)) == equality);
                                }
                                return boxBoolean(false == equality);
                            }
                            if (lhs->tag() == TAG_literalUndefined || lhs->tag() == TAG_literalNull)
                                return boxBoolean(true == equality);
                            if (lhs->tag() == TAG_literalDouble)
                                return boxBoolean((evaluateToNumber(lhs) == evaluateToNumber(rhs)) == equality);
                            if (lhs->tag() == TAG_literalBoolean)
                                return boxBoolean((evaluateToBoolean(lhs) == evaluateToBoolean(rhs)) == equality);
                            if (lhs->tag() == TAG_literalString)
                                return boxBoolean((evaluateToString(lhs) == evaluateToString(rhs)) == equality);
                            failNonConstant(lhs);
                            /*NOTREACHED*/
                            break;
                        }
                        default:
                            // "as", "is", "in", ",", "="
                            compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_OP_IN_CONSTEXPR);
                            /*NOTREACHED*/
                            break;
                    }
                    /*NOTREACHED*/
                    break;
                }
                case TAG_unaryExpr: {
                    // EXTENSION: typeof
                    //
                    // Supporting "typeof" makes some sort of sense (for example, the
                    // operand of typeof can be a config constant that can take on
                    // various values, and computing the name of the type into the
                    // program can be useful).
                    //
                    // EXTENSION: void
                    //
                    // Supporting "void" probably does not make a lot of sense, but it
                    // seems benign.
                    UnaryExpr* unary = (UnaryExpr*)e;
                    Expr* opd = evaluateConfigDefinition(ns, unary->expr);
                    switch (unary->op) {
                        case OPR_typeof:
                            switch (opd->tag()) {
                                case TAG_literalUndefined: return boxString("undefined");
                                case TAG_literalString:    return boxString("string");
                                case TAG_literalNull:      return boxString("object");
                                case TAG_literalUInt:
                                case TAG_literalInt:
                                case TAG_literalDouble:    return boxString("number");
                                case TAG_literalFloat:     return boxString("number");
                                case TAG_literalBoolean:   return boxString("boolean");
                                default:
                                    failNonConstant(opd);
                                    return NULL;
                            }
                        case OPR_bitwiseNot: return boxUInt(~evaluateToUInt32(opd));
                        case OPR_unminus:    return boxDouble(-evaluateToNumber(opd));
                        case OPR_unplus:     return boxDouble(evaluateToNumber(opd));
                        case OPR_not:        return boxBoolean(!evaluateToBoolean(opd));
                        case OPR_void:       return boxUndefined();
                        default:
                            // "delete", "++", "--"
                            compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_OP_IN_CONSTEXPR);
                            /*NOTREACHED*/
                            break;
                    }
                    /*NOTREACHED*/
                    break;
                }
                case TAG_conditionalExpr: {
                    // EXTENSION: conditional operator
                    //
                    // It seems totally sensible to support "... ? ... : ...", though
                    // it's not mentioned in the conditional compilation spec.
                    //
                    // We evaluate both arms in order to uncover references to undefined
                    // configuration variables, same as for && and ||.
                    ConditionalExpr* cond = (ConditionalExpr*)e;
                    Expr* e1 = evaluateConfigDefinition(ns, cond->e1);
                    Expr* e2 = evaluateConfigDefinition(ns, cond->e2);
                    Expr* e3 = evaluateConfigDefinition(ns, cond->e3);
                    return evaluateToBoolean(e1) ? e2 : e3;
                }
                default:
                    // Property references, 'new', 'call' - lots of things
                    compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_OP_IN_CONSTEXPR);
                    /*NOTREACHED*/
                    break;
            }
            /*NOTREACHED*/
            return NULL;
        }
        
        Expr* Parser::boxDouble(double n)      { return ALLOC(LiteralDouble, (n, 0)); }
#ifdef VMCFG_FLOAT
        Expr* Parser::boxFloat(float n)        { return ALLOC(LiteralFloat, (n, 0)); }
#endif
        Expr* Parser::boxUInt(uint32_t n)      { return ALLOC(LiteralUInt, (n, 0)); }
        Expr* Parser::boxInt(int32_t n)        { return ALLOC(LiteralInt, (n, 0)); }
        Expr* Parser::boxBoolean(bool b)       { return ALLOC(LiteralBoolean, (b, 0)); }
        Expr* Parser::boxString(const char* s) { return ALLOC(LiteralString, (compiler->intern(s), 0)); }
        Expr* Parser::boxString(Str* s)        { return ALLOC(LiteralString, (s, 0)); }
        Expr* Parser::boxUndefined()           { return ALLOC(LiteralUndefined, (0)); }

        uint32_t Parser::evaluateToUInt32(Expr* e)
        {
            if (e->tag() == TAG_literalUInt)
                return ((LiteralUInt*)e)->value;
            double d = evaluateToNumber(e);
            if (d == 0 || MathUtils::isNaN(d) || MathUtils::isInfinite(d))
                return 0;
            d = (d < 0 ? -1 : 1) * floor(fabs(d));
            d = fmod(d, 4294967296.0);
            return uint32_t(d);
        }
        
        int32_t Parser::evaluateToInt32(Expr* e)
        {
            if (e->tag() == TAG_literalInt)
                return ((LiteralInt*)e)->value;
            double d = evaluateToNumber(e);
            if (d == 0 || MathUtils::isNaN(d) || MathUtils::isInfinite(d))
                return 0;
            d = (d < 0 ? -1 : 1) * floor(fabs(d));
            d = fmod(d, 4294967296.0);
            if (d >= 2147483648.0)
                return int32_t(d - 4294967296.0);
            else
                return int32_t(d);
        }

        double Parser::evaluateToNumber(Expr* e)
        {
            switch (e->tag()) {
                case TAG_literalUndefined: return MathUtils::kNaN;
                case TAG_literalNull:      return 0.0;
                case TAG_literalBoolean:   return ((LiteralBoolean*)e)->value ? 1.0 : 0.0;
                case TAG_literalDouble:    return ((LiteralDouble*)e)->value;
#ifdef VMCFG_FLOAT
                case TAG_literalFloat:     return ((LiteralFloat*)e)->value;        // FIXME?
#endif
                case TAG_literalInt:       return (double)(((LiteralInt*)e)->value);
                case TAG_literalUInt:      return (double)(((LiteralUInt*)e)->value);
                case TAG_literalString:    return strToDouble(((LiteralString*)e)->value);
                default:
                    failNonConstant(e);
                    return 0;
            }
        }
        
        bool Parser::evaluateToBoolean(Expr* e)
        {
            switch (e->tag()) {
                case TAG_literalUndefined: return false;
                case TAG_literalNull:      return false;
                case TAG_literalBoolean:   return ((LiteralBoolean*)e)->value;
                case TAG_literalDouble:    { double v = ((LiteralDouble*)e)->value; return !MathUtils::isNaN(v) && v != 0.0; }
#ifdef VMCFG_FLOAT
                case TAG_literalFloat:     { float v = ((LiteralFloat*)e)->value; return !MathUtils::isNaNf(v) && v != 0.0f; }
#endif
                case TAG_literalInt:       return ((LiteralInt*)e)->value != 0;
                case TAG_literalUInt:      return ((LiteralUInt*)e)->value != 0;
                case TAG_literalString:    return ((LiteralString*)e)->value->length > 0;
                default:
                    failNonConstant(e);
                    return 0;
            }
        }

        Str* Parser::evaluateToString(Expr* e)
        {
            switch (e->tag()) {
                case TAG_literalUndefined: return compiler->intern("undefined");
                case TAG_literalNull:      return compiler->intern("null");
                case TAG_literalBoolean:   return ((LiteralBoolean*)e)->value ? compiler->intern("true") : compiler->intern("false");
                case TAG_literalDouble:    return doubleToStr(((LiteralDouble*)e)->value);
#ifdef VMCFG_FLOAT
                case TAG_literalFloat:     return doubleToStr(((LiteralFloat*)e)->value);
#endif
                case TAG_literalInt:       return doubleToStr(((LiteralInt*)e)->value);
                case TAG_literalUInt:      return doubleToStr(((LiteralUInt*)e)->value);
                case TAG_literalString:    return ((LiteralString*)e)->value;
                default:
                    failNonConstant(e);
                    return 0;
            }
        }

        // Returns -1 for undefined, 0 for false, 1 for true.  Generally true means "x < y",
        // false means "!(x < y)" and undefined means x and y are not comparable.
    
        int Parser::evaluateRelational(Expr* lhs, Expr* rhs)
        {
            if (lhs->tag() == TAG_literalString && rhs->tag() == TAG_literalString)
                return (((LiteralString*)lhs)->value)->compareTo(((LiteralString*)rhs)->value) < 0 ? 1 : 0;

            double l = evaluateToNumber(lhs);
            double r = evaluateToNumber(rhs);
            if (MathUtils::isNaN(l) || MathUtils::isNaN(r))
                return -1;
            return l < r ? 1 : 0;
        }

        void Parser::failNonConstant(Expr* e)
        {
            compiler->internalError(position(), "Non-constant value in expression evaluation, tag=%d", int(e->tag()));
        }
    }
}

#endif  // VMCFG_EVAL
