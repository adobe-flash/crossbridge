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
        NameComponent::~NameComponent() {}
        
        Type* Parser::typeExpression()
        {
            if (match(T_Multiply))
                return NULL;
            QualifiedName* n = nameExpression(false);
            if ((n->qualifier != NULL && n->qualifier->tag() != TAG_simpleName) ||
                n->name->tag() != TAG_simpleName)
                compiler->syntaxError(n->pos, SYNTAXERR_ILLEGAL_TYPENAME);
            if (hd() != T_LeftDotAngle) {
                return ALLOC(SimpleType, (n));
            }
            else {
                next();
                Type* t2 = typeExpression();
                rightAngle();
                next();
                return ALLOC(InstantiatedType, (n, t2));
            }
        }

        QualifiedName* Parser::nameExpression(bool is_attr)
        {
            uint32_t pos = position();
            Str* name = NULL;
            NameComponent* n = NULL;
            if (match(T_Multiply))
                n = ALLOC(WildcardName, ());
            else if (match(T_Public)) {
                n = ALLOC(BuiltinNamespace, (T_Public));
            }
            else if (match(T_Protected)) {
                n = ALLOC(BuiltinNamespace, (T_Protected));
            }
            else if (match(T_Private)) {
                n = ALLOC(BuiltinNamespace, (T_Private));
            }
            else if (match(T_Internal)) {
                n = ALLOC(BuiltinNamespace, (T_Internal));
            }
            else {
                name = identifier();
                n = ALLOC(SimpleName, (name));
            }
            if (match(T_DoubleColon)) {
                if (match(T_Multiply))
                    return ALLOC(QualifiedName, (n, ALLOC(WildcardName, ()), is_attr, pos));
                if (match(T_LeftBracket)) {
                    Expr* e = commaExpression(0);
                    eat(T_RightBracket);
                    return ALLOC(QualifiedName, (n, ALLOC(ComputedName, (e)), is_attr, pos));
                }
                return ALLOC(QualifiedName, (n, ALLOC(SimpleName, (identifier())), is_attr, pos));
            }
            else {
                if (name == compiler->SYM_arguments)
                    setUsesArguments();
                return ALLOC(QualifiedName, (NULL, n, is_attr, pos));
            }
        }
        
        Expr* Parser::exprListToCommaExpr(Seq<Expr*>* es) {
            Expr* expr = es->hd;
            for ( es=es->tl ; es != NULL ; es = es->tl )
                expr = ALLOC(BinaryExpr, (OPR_comma, expr, es->hd));
            return expr;
        }
        
        Expr* Parser::objectInitializer ()
        {
            uint32_t pos = position();
            eat (T_LeftBrace);
            Seq<LiteralField*>* fields = fieldList();
            eat (T_RightBrace);
            
            return ALLOC(LiteralObject, (fields, pos));
        }
        
        Seq<LiteralField*>* Parser::fieldList ()
        {
            SeqBuilder<LiteralField*> fields(allocator);
            if (hd () != T_RightBrace) {
                do {
                    LiteralField* f = literalField();
                    if (f != NULL)
                        fields.addAtEnd(f);
                } while (match(T_Comma));
            }
            return fields.get();
        }
        
        LiteralField* Parser::literalField()
        {
            Str* name = NULL;
            QualifiedName* config = NULL;
        again:
            switch (hd ()) {
                case T_StringLiteral:
                    name = stringValue();
                    break;
                    
                case T_IntLiteral:
                    name = doubleToStr(intValue());
                    break;
                    
                case T_UIntLiteral:
                    name = doubleToStr(uintValue());
                    break;
                    
                case T_DoubleLiteral:
                    name = doubleToStr(doubleValue());
                    break;
                    
                case T_FloatLiteral:
                    name = doubleToStr((double)floatValue());
                    break;
                    
                case T_Identifier: {
                    if (hd2() != T_DoubleColon) {
                        name = identValue();
                        break;
                    }
                    // This must be a program configuration expression
                    if (config != NULL)
                        compiler->syntaxError(position(), SYNTAXERR_DUPLICATE_CONFIG);
                    Expr* e = assignmentExpression(0);
                    if (!isConfigReference(e))
                        compiler->syntaxError(position(), SYNTAXERR_CONFIG_REQUIRED);
                    config = (QualifiedName*)e;
                    goto again;
                }

                default:
                    compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_FIELDNAME);
                    break;
            }
            next();
            match(T_Colon);
            Expr* expr = assignmentExpression(0);
            if (config == NULL || evaluateConfigReference(config))
                return ALLOC(LiteralField, (name, expr));
            return NULL;
        }
        
        Expr* Parser::arrayInitializer ()
        {
            uint32_t pos = position();
            eat (T_LeftBracket);
            Seq<Expr*>* elts = elementList();
            eat (T_RightBracket);
            return ALLOC(LiteralArray, (elts, pos));
        }
        
        Seq<Expr*>* Parser::elementList()
        {
            SeqBuilder<Expr*> elts(allocator);
            Expr* elt = NULL;

            for (;;) {
                switch (hd()) {
                    case T_RightBracket:
                        goto end_loop;

                    case T_Comma:  {
                        eat(T_Comma);
                        if (elt == NULL)
                            elt = NULL;
                        elts.addAtEnd(elt);
                        elt = NULL;
                        break;
                    }
                        
                    default:
                        if (elt != NULL)
                            eat(T_Comma);
                        elt = assignmentExpression(0);
                        if (hd() != T_RightBracket && hd() != T_Comma) {
                            // This must be a program configuration expression
                            if (!isConfigReference(elt))
                                compiler->syntaxError(position(), SYNTAXERR_CONFIG_REQUIRED);
                            bool result = evaluateConfigReference((QualifiedName*)elt);
                            elt = assignmentExpression(0);
                            if (!result)
                                elt = NULL;
                        }
                        break;
                }
            }
        end_loop:
            if (elt != NULL)
                elts.addAtEnd(elt);
            
            return elts.get();
        }

        // The "new" token has been consumed and we're stopped at T_BreakLeftAngle
        Expr* Parser::vectorInitializer(uint32_t pos)
        {
            leftAngle();
            eat(T_LessThan);
            Type* type = typeExpression();
            rightAngle();
            eat(T_GreaterThan);
            SeqBuilder<Expr*> elts(allocator);
            eat (T_LeftBracket);
            // The grammar allows for a trailing comma following an expression
            if (hd() != T_RightBracket)
            {
                for (;;) {
                    elts.addAtEnd(assignmentExpression(0));
                    if (!match(T_Comma))
                        break;
                    if (hd() == T_RightBracket)
                        break;
                }
            }
            eat (T_RightBracket);
            return ALLOC(LiteralVector, (type, elts.get(), pos));
        }
        
        Expr* Parser::functionExpression()
        {
            Qualifier qual;
            return ALLOC(LiteralFunction, (functionGuts(&qual, false, false, true)));
        }
        
        Expr* Parser::primaryExpression()
        {
            if (hd() == T_BreakSlash)
                regexp();
                
            uint32_t pos = position();   // Record the source location before consuming the token
            switch (hd ()) {
                case T_Null:
                    next();
                    return ALLOC(LiteralNull, (pos));
                    
                case T_True:
                case T_False: {
                    bool flag = hd() == T_True;
                    next();
                    return ALLOC(LiteralBoolean, (flag, pos));
                }

                case T_IntLiteral: {
                    int32_t i = intValue();
                    next();
                    return ALLOC(LiteralInt, (i, pos));
                }

                case T_UIntLiteral: {
                    uint32_t u = uintValue();
                    next();
                    return ALLOC(LiteralUInt, (u, pos));
                }

                case T_DoubleLiteral: {
                    double d = doubleValue();
                    next();
                    return ALLOC(LiteralDouble, (d, pos));
                }

#ifdef VMCFG_FLOAT
                case T_FloatLiteral: {
                    float f = floatValue();
                    next();
                    return ALLOC(LiteralFloat, (f, pos));
                }
#endif

                case T_StringLiteral: {
                    Str* s = stringValue();
                    next();
                    return ALLOC(LiteralString, (s, pos));
                }

                case T_RegexpLiteral: {
                    Str* r = regexValue();
                    next();
                    return ALLOC(LiteralRegExp, (r, pos));
                }

                case T_This:
                    next();
                    return ALLOC(ThisExpr, ());

                case T_Super:
                    return superExpression();

                case T_LeftParen:
                    return parenExpression();
                    
                case T_LeftBracket:
                    return arrayInitializer ();
                    
                case T_LeftBrace:
                    return objectInitializer ();
                    
                case T_Function:
                    return functionExpression ();
                    
                case T_BreakLeftAngle:
                    return xmlInitializer();
                    
                case T_AtSign:
                    return attributeIdentifier();
                    
                default:
                    break;
            }

            return nameExpression();
        }

        QualifiedName* Parser::attributeIdentifier()
        {
            eat(T_AtSign);
            if (hd() == T_LeftBracket) {
                eat(T_LeftBracket);
                Expr* e = commaExpression(0);
                eat(T_RightBracket);
                return ALLOC(QualifiedName, (NULL, ALLOC(ComputedName, (e)), true, 0));
            }
            else
                return nameExpression(true);
        }

        QualifiedName* Parser::propertyIdentifier()
        {
            switch (hd()) {
                case T_Multiply:
                case T_AtSign:
                case T_Identifier:
                    return (QualifiedName*)primaryExpression();
                default:
                    compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_PROPNAME);
                    /*NOTREACHED*/
                    return NULL;
            }
        }

        Expr* Parser::propertyOperator(Expr* obj)
        {
            uint32_t pos = position();
            switch (hd ()) {
                case T_Dot: {
                    eat(T_Dot);
                    if (hd() == T_LeftParen)
                        return ALLOC(FilterExpr, (obj, parenExpression(), pos));
                    if (hd() == T_AtSign)
                        return ALLOC(ObjectRef, (obj, attributeIdentifier(), pos));
                    // Begin keyword hacks
                    // FIXME
                    // Allow 'namespace' to be used as a property name
                    if (hd() == T_Namespace) {
                        eat(T_Namespace);
                        return ALLOC(ObjectRef, (obj, ALLOC(QualifiedName, (NULL, ALLOC(SimpleName, (compiler->SYM_namespace)), false, pos)), pos));
                    }
                    // End keyword hacks
                    return ALLOC(ObjectRef, (obj, nameExpression(), pos));
                }
                case T_DoubleDot: {
                    eat(T_DoubleDot);
                    return ALLOC(DescendantsExpr, (obj, propertyIdentifier(), pos));
                };
                case T_LeftBracket: {
                    eat(T_LeftBracket);
                    Expr* expr = commaExpression(0);
                    eat (T_RightBracket);
                    return ALLOC(ObjectRef, (obj, ALLOC(QualifiedName, (NULL, ALLOC(ComputedName, (expr)), false, pos)), pos));
                }
                case T_LeftDotAngle: {
                    // In this case, obj must be a QualifiedName of a certain form.
                    if (obj->tag() == TAG_qualifiedName) {
                        QualifiedName* basename = (QualifiedName*)obj;
                        Cogen::checkVectorType(compiler, basename);
                        next();
                        Type* t2 = typeExpression();
                        rightAngle();
                        next();
                        return ALLOC(InstantiatedTypeRef, (ALLOC(InstantiatedType, (basename, t2)), pos));
                    }
                    compiler->syntaxError(pos, SYNTAXERR_ILLEGAL_TYPENAME);
                    /*NOTREACHED*/
                    return NULL;
                }
                default:
                    compiler->internalError(position(), "Unexpected token in propertyOperator: %d", (int)hd());
                    /*NOTREACHED*/
                    return NULL;
            }
        }
        
        Seq<Expr*>* Parser::argumentList ()
        {
            SeqBuilder<Expr*> exprs(allocator);
            
            eat(T_LeftParen);
            if (hd() != T_RightParen) {
                do {
                    exprs.addAtEnd(assignmentExpression(0));
                } while (match(T_Comma));
            }
            eat(T_RightParen);
            return exprs.get();
        }
        
        Expr* Parser::memberExpression ()
        {
            switch (hd ()) {
                case T_New: {
                    uint32_t pos = position();
                    next();
                    if (hd() == T_BreakLeftAngle)
                        return vectorInitializer(pos);
                    Expr* object_expr = memberExpression ();
                    Seq<Expr*>* argument_exprs = argumentList();
                    return memberExpressionPrime (ALLOC(NewExpr, (object_expr, argument_exprs)));
                }
                    
                default: {
                    Expr* expr = primaryExpression ();
                    return memberExpressionPrime (expr);
                }
            }
        }
        
        Expr* Parser::memberExpressionPrime (Expr* expr)
        {
            switch (hd ()) {
                case T_LeftBracket:
                case T_Dot:
                case T_DoubleDot:
                case T_LeftDotAngle:
                    return memberExpressionPrime (propertyOperator (expr));
                default:
                    return expr;
            }
        }
        
        Expr* Parser::superExpression ()
        {
            eat (T_Super);
            uint32_t pos = position();
            Seq<Expr*>* arguments = NULL;
            bool argsPresent = false;
            if (hd() == T_LeftParen) {
                argsPresent = true;
                arguments = argumentList();
            }
            if (argsPresent && (arguments == NULL || arguments->tl != NULL))
                compiler->syntaxError(pos, SYNTAXERR_ONE_ARGUMENT_REQUIRED);
            if (hd() != T_Dot && hd() != T_LeftBracket)
                compiler->syntaxError(pos, SYNTAXERR_PROPERTY_OPERATOR_REQUIRED);
            Expr* obj = argsPresent ? arguments->hd : ALLOC(ThisExpr, ());
            return propertyOperator(ALLOC(SuperExpr, (obj)));
        }

        Expr* Parser::callExpression ()
        {
            uint32_t pos = position();
            Expr* object_expr = memberExpression ();
            Seq<Expr*>* argument_exprs = argumentList();
            
            return callExpressionPrime (ALLOC(CallExpr, (object_expr, argument_exprs, pos)));
        }
        
        // shared among many
        Expr* Parser::callExpressionPrime (Expr* call_expr)
        {
            switch (hd ()) {
                case T_LeftParen:
                {
                    uint32_t pos = position();
                    Seq<Expr*>* argument_exprs = argumentList();
                    return callExpressionPrime (ALLOC(CallExpr, (call_expr, argument_exprs, pos)));
                }
                case T_LeftBracket:
                case T_Dot:
                case T_LeftDotAngle:
                    return callExpressionPrime (propertyOperator (call_expr));
                default:
                    return call_expr;
            }
        }
        
        Expr* Parser::newExpression (int new_count)
        {
            Expr* call_expression;
            
            uint32_t pos = position();
            bool is_new = match(T_New);
            if (is_new) {
                if (hd() == T_BreakLeftAngle)
                    call_expression = vectorInitializer(pos);
                else
                    call_expression = newExpression (new_count+1);
            }
            else
                call_expression = memberExpression();
                    
            if (hd() == T_LeftParen) {   // No more new exprs so this paren must start a call expr
                Seq<Expr*>* argument_exprs = argumentList();
                if (new_count > 0)
                    return ALLOC(NewExpr, (call_expression, argument_exprs));
                return callExpressionPrime (ALLOC(CallExpr, (call_expression, argument_exprs, pos)));
            }
            
            if (new_count > 0)
                return ALLOC(NewExpr, (call_expression, NULL));
            
            if (is_new)
                return memberExpressionPrime (call_expression);
            
            return call_expression;
        }
        
        Expr* Parser::leftHandSideExpression ()
        {
            Expr* oper = (hd() == T_New) ? newExpression (0) : memberExpression ();
            if (hd () == T_LeftParen) {
                uint32_t pos = position();
                Seq<Expr*>* args = argumentList();
                return callExpressionPrime(ALLOC(CallExpr, (oper, args, pos)));
            }
            return oper;
        }
        
        Expr* Parser::postfixExpression ()
        {
            Expr* expr = leftHandSideExpression ();
            if (noNewline()) {
                if (match(T_PlusPlus))
                    return ALLOC(UnaryExpr, (OPR_postIncr, expr));
                if (match(T_MinusMinus))
                    return ALLOC(UnaryExpr, (OPR_postDecr, expr));
            }
            return expr;
        }
        
        Expr* Parser::unaryExpression()
        {
            Token t;
            
            switch (t = hd ()) {
                case T_Delete:
                    next();
                    return ALLOC(UnaryExpr, (OPR_delete, postfixExpression()));
                    
                case T_PlusPlus:
                case T_MinusMinus:
                    next();
                    return ALLOC(UnaryExpr, (tokenToUnaryOperator(t), postfixExpression()));
                    
                case T_Void:
                case T_TypeOf:
                case T_Plus:
                case T_Minus:
                case T_BitwiseNot:
                case T_Not:
                    next();
                    return ALLOC(UnaryExpr, (tokenToUnaryOperator(t), unaryExpression()));
                    
                default:
                    return postfixExpression();
            }
        }
        
        Expr* Parser::multiplicativeExpression()
        {
            Expr* expr = unaryExpression();
            Token t;
            
            while (isMultiplicative(t = hd()) || t == T_BreakSlash) {
                if (t == T_BreakSlash) {
                    divideOperator();
                    if (!isMultiplicative(t = hd()))
                        break;
                }
                next();
                expr = ALLOC(BinaryExpr, (tokenToBinaryOperator(t), expr, unaryExpression()));
            }
            return expr;
        }
        
        Expr* Parser::additiveExpression()
        {
            Expr* expr = multiplicativeExpression();
            Token t;
            
            while (isAdditive(t = hd())) {
                next();
                expr = ALLOC(BinaryExpr, (tokenToBinaryOperator(t), expr, multiplicativeExpression()));
            }
            return expr;
        }
        
        Expr* Parser::shiftExpression()
        {
            Expr* expr = additiveExpression();
            Token t;
            
            while (isShift(t = hd()) || t == T_BreakLeftAngle || t == T_BreakRightAngle) {
                if (t == T_BreakLeftAngle) {
                    leftShiftOrRelationalOperator();
                    if (!isShift(t = hd()))
                        break;
                }
                else if (t == T_BreakRightAngle) {
                    rightShiftOrRelationalOperator();
                    if (!isShift(t = hd()))
                        break;
                }
                next();
                expr = ALLOC(BinaryExpr, (tokenToBinaryOperator(t), expr, additiveExpression()));
            }
            
            return expr;
        }
        
        Expr* Parser::relationalExpression(int flags)
        {
            Expr* expr = shiftExpression();
            Token t;
            bool allowIn = !(flags & EFLAG_NoIn);

            for (;;) {
                if (!(isRelational(t = hd(), allowIn) || t == T_BreakLeftAngle || t == T_BreakRightAngle))
                    break;
                if (t == T_BreakLeftAngle) {
                    leftShiftOrRelationalOperator();
                    if (!isRelational(t = hd(), allowIn))
                        break;
                }
                else if (t == T_BreakRightAngle) {
                    rightShiftOrRelationalOperator();
                    if (!isRelational(t = hd(), allowIn))
                        break;
                }
                next();
                expr = ALLOC(BinaryExpr, (tokenToBinaryOperator(t), expr, shiftExpression()));
            }
            
            return expr;
        }
        
        Expr* Parser::equalityExpression(int flags)
        {
            Expr* expr = relationalExpression(flags);
            Token t;
            
            while (isEquality(t = hd())) {
                next();
                expr = ALLOC(BinaryExpr, (tokenToBinaryOperator(t), expr, relationalExpression(flags)));
            }
            return expr;
        }
        
        Expr* Parser::bitwiseAndExpression(int flags)
        {
            Expr* expr = equalityExpression(flags);
            while (match(T_BitwiseAnd))
                expr = ALLOC(BinaryExpr, (OPR_bitwiseAnd, expr, equalityExpression(flags)));
            return expr;
        }
        
        Expr* Parser::bitwiseXorExpression(int flags)
        {
            Expr* expr = bitwiseAndExpression(flags);
            while (match(T_BitwiseXor))
                expr = ALLOC(BinaryExpr, (OPR_bitwiseXor, expr, bitwiseAndExpression(flags)));
            return expr;
        }
        
        Expr* Parser::bitwiseOrExpression(int flags)
        {
            Expr* expr = bitwiseXorExpression(flags);
            while (match(T_BitwiseOr))
                expr = ALLOC(BinaryExpr, (OPR_bitwiseOr, expr, bitwiseXorExpression(flags)));
            return expr;
        }
        
        Expr* Parser::logicalAndExpression(int flags)
        {
            Expr* expr = bitwiseOrExpression(flags);
            while (match(T_LogicalAnd))
                expr = ALLOC(BinaryExpr, (OPR_logicalAnd, expr, bitwiseOrExpression(flags)));
            return expr;
        }
        
        Expr* Parser::logicalOrExpression(int flags)
        {
            Expr* expr = logicalAndExpression(flags);
            while (match(T_LogicalOr))
                expr = ALLOC(BinaryExpr, (OPR_logicalOr, expr, logicalAndExpression(flags)));
            return expr;
        }
        
        Expr* Parser::nonAssignmentExpression(int flags)
        {
            Expr* expr = logicalOrExpression(flags);
            if (!match(T_Question))
                return expr;
            
            Expr* consequent = nonAssignmentExpression(flags);
            eat (T_Colon);
            Expr* alternate = nonAssignmentExpression(flags);
            return ALLOC(ConditionalExpr, (expr, consequent, alternate));
        }
        
        Expr* Parser::conditionalExpression(int flags)
        {
            Expr* expr = logicalOrExpression(flags);
            if (!match(T_Question))
                return expr;

            Expr* consequent = assignmentExpression(flags);
            eat (T_Colon);
            Expr* alternate = assignmentExpression(flags);
            return ALLOC(ConditionalExpr, (expr, consequent, alternate));
        }
        
        Expr* Parser::assignmentExpression(int flags)
        {
            Expr* lhs = conditionalExpression(flags);
            Token t;
            
            if (!((t = hd()) == T_Assign || isOpAssign(t) || t == T_BreakLeftAngle || t == T_BreakRightAngle))
                return lhs;

            if (t == T_BreakLeftAngle) {
                leftShiftOrRelationalOperator();
                if (!isOpAssign(t = hd()))
                    return lhs;
            }
            else if (t == T_BreakRightAngle) {
                rightShiftOrRelationalOperator();
                if (!isOpAssign(t = hd()))
                    return lhs;
            }
            
            eat(t);
            Expr* rhs = assignmentExpression (flags);
            return ALLOC(AssignExpr, (tokenToBinaryOperator(t), lhs, rhs));
        }
        
        Expr* Parser::commaExpression(int flags)
        {
            Expr* expr = assignmentExpression(flags);
            while (match(T_Comma))
                expr = ALLOC(BinaryExpr, (OPR_comma, expr, assignmentExpression(flags)));
            return expr;
        }

        Expr* Parser::parenExpression()
        {
            eat (T_LeftParen);
            Expr* expr = commaExpression(0);
            eat (T_RightParen);
            
            return expr;
        }
    }
}

#endif  // VMCFG_EVAL
