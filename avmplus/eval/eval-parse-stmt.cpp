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
        Stmt* Parser::statement(bool config)
        {
            AvmAssert(config == true || hd() == T_LeftBrace);
            switch (hd()) {
                case T_Semicolon: {
                    Stmt* stmt = ALLOC(EmptyStmt, ());
                    next();
                    return stmt;
                }
                    
                case T_LeftBrace:
                    return ALLOC(BlockStmt, (statementBlock(config)));
                    
                case T_Break: {
                    Stmt* stmt = breakStatement();
                    semicolon();
                    return stmt;
                }
                    
                case T_Continue: {
                    Stmt* stmt = continueStatement();
                    semicolon();
                    return stmt;
                }
                
                case T_Default: {
                    next();
                    if (hd() == T_Identifier && identValue() == compiler->SYM_xml) {
                        Stmt* stmt = defaultXmlNamespaceStatement();
                        semicolon();
                        return stmt;
                    }
                    else {
                        compiler->syntaxError(position(), SYNTAXERR_DEFAULT_NOT_EXPECTED);
                        /*NOTREACHED*/
                        return NULL;
                    }
                }

                case T_Do: {
                    Stmt* stmt = doStatement();
                    semicolon();
                    return stmt;
                }
                    
                case T_Goto: {
                    Stmt* stmt = gotoStatement();
                    semicolon();
                    return stmt;
                }

                case T_For:
                    return forStatement();
                    
                case T_If:
                    return ifStatement();
                    
                case T_Import:
                    return importStatement();

                case T_Return: {
                    Stmt* stmt = returnStatement();
                    semicolon();
                    return stmt;
                }
                    
                case T_Switch:
                    return switchStatement();
                    
                case T_Throw: {
                    Stmt* stmt = throwStatement();
                    semicolon();
                    return stmt;
                }
                    
                case T_Try:
                    return tryStatement();
                    
                case T_Use: {
                    Stmt* stmt = useStatement();
                    semicolon();
                    return stmt;
                }

                case T_Var:
                case T_Const: {
                    Stmt* stmt = varStatement(defaultNamespace(), hd() == T_Const, false);
                    semicolon();
                    return stmt;
                }

                case T_While:
                    return whileStatement();
                    
                case T_With:
                    return withStatement ();
                    
                case T_Function:
                    if (compiler->local_functions) {
                        // FIXME: should block-internal function definition be initialized on block entry?
                        uint32_t pos = position();
                        Qualifier qual;
                        FunctionDefn* fn = functionGuts(&qual, true, false, true);
                        Str* name = fn->name;
                        fn->name = NULL;
                        addVarBinding(defaultNamespace(), name, NULL);
                        return ALLOC(ExprStmt, (pos,
                                                ALLOC(AssignExpr,
                                                      (OPR_assign,
                                                       ALLOC(QualifiedName, (NULL, ALLOC(SimpleName, (name)), false, pos)),
                                                       ALLOC(LiteralFunction, (fn))))));
                    }
                    else {
                        compiler->syntaxError(position(), SYNTAXERR_NO_FUNCTIONS_IN_BLOCKS);
                        /*NOTREACHED*/
                        return NULL;
                    }
                    
                case T_Super:
                    return superStatement();
                    
                default: {
                    if (hd() == T_Identifier && hd2() == T_Colon)
                        return labeledStatement();
                    uint32_t pos = position();
                    Stmt* stmt = ALLOC(ExprStmt, (pos, commaExpression(0)));
                    semicolon();
                    return stmt;
                }
            }
        }
        
        void Parser::semicolon()
        {
            switch (hd ()) {
                case T_Semicolon:
                    next();
                    return;
                case T_EOS:
                case T_RightBrace:
                    // Inserting it
                    return;
                default:
                    if (!newline ())
                        compiler->syntaxError(position(), SYNTAXERR_SEMICOLON_OR_NEWLINE);
                    // Inserting it
                    return;
            }
        }
        
        bool Parser::noNewline()
        {
            switch (hd ()) {
                case T_EOS:
                case T_Semicolon:
                case T_RightBrace:
                    return false;
                default:
                    if (newline())
                        return false;
                    return true;
            }
        }
        
        Seq<Stmt*>* Parser::statementBlock(bool config)
        {
            SeqBuilder<Stmt*> stmts(allocator);
            eat (T_LeftBrace);
            while (hd() != T_RightBrace)
                stmts.addAtEnd(statement(config));
            eat (T_RightBrace);
            return stmts.get();
        }
        
        // updates bindings by side effect, returns a single expression
        // statement for the initialization.
        
        Stmt* Parser::varStatement(NameComponent* ns, bool is_const, bool is_static)
        {
            uint32_t pos = 0;
            Expr* inits = varBindings(&pos, ns, is_const, is_static);
            return ALLOC(VardefStmt, (pos, inits));
        }
        
        // updates bindings by side effect, returns a single expression for
        // the initialization, a LiteralUndefined node if there was no useful
        // initialization.
        
        Expr* Parser::varBindings(uint32_t* pos, NameComponent* ns, bool is_const, bool is_static, int flags, uint32_t* numbindings, Expr** firstName)
        {
            AvmAssert( !is_const || firstName == NULL );

            Expr* inits = NULL;
            eat(is_const ? T_Const : T_Var);
            *pos = position();
            if (numbindings)
                *numbindings = 0;
            if (firstName)
                *firstName = NULL;
            for (;;) {
                Str* name = identifier();
                Type* type_name = NULL;
                if (match(T_Colon))
                    type_name = typeExpression();
                if (numbindings)
                    *numbindings += 1;
                if (!is_const)
                    addVarBinding(ns, name, type_name, is_static ? topRib->next : topRib);
                /*
                if (is_const && hd() != T_Assign)
                    compiler->syntaxError(*pos, SYNTAXERR_CONST_INIT_REQD);
                */
                if (match(T_Assign)) {
                    Expr* init = assignmentExpression(flags);
                    Expr* lhs = ALLOC(QualifiedName, (ns, ALLOC(SimpleName, (name)), false, *pos));
                    if (is_const)
                        addConstBinding(ns, name, type_name, is_static ? topRib->next : topRib);
                    Expr* assign = ALLOC(AssignExpr, (is_const ? OPR_init : OPR_assign, lhs, init));
                    if (firstName && *firstName == NULL)
                        *firstName = lhs;
                    if (inits == NULL)
                        inits = assign;
                    else
                        inits = ALLOC(BinaryExpr, (OPR_comma, inits, assign));
                }
                else if (firstName && *firstName == NULL) {
                    *firstName = ALLOC(QualifiedName, (ns, ALLOC(SimpleName, (name)), false, *pos));
                }

                if (!match(T_Comma))
                    break;
            }
            return inits ? inits : ALLOC(LiteralUndefined, (*pos));
        }
        
        Stmt* Parser::useStatement()
        {
            uint32_t pos = position();
            eat(T_Use);
            if (!match(T_Namespace))
                compiler->syntaxError(pos, SYNTAXERR_ILLEGAL_USE);
            Str* ns = identifier();
            addOpenNamespace(ALLOC(NamespaceRef, (ns)));
            return ALLOC(EmptyStmt, ());
        }
        
        // <import> ::= "import" <ident> { "." <ident> }* { "." "*" }
        Stmt* Parser::importStatement()
        {
            SeqBuilder<Str*> name(allocator);
            StringBuilder id(compiler);
            const bool hackaround = true;   // treat import x.y.z as import x.y.*
            eat(T_Import);
            if (hd() == T_Identifier) {
                name.addAtEnd(identValue());
                id.append(identValue());
            }
            eat(T_Identifier);
            bool qualified = true;
            while (match(T_Dot)) {
                if (hd() == T_Multiply) {
                    match(T_Multiply);
                    qualified = false;
                    break;
                }
                if (hackaround)
                {
                    if (hd() == T_Identifier && hd2() == T_Dot) {
                        id.append(".");
                        name.addAtEnd(identValue());
                        id.append(identValue());
                    }
                    eat(T_Identifier);
                }
                else
                {
                    id.append(".");
                    if (hd() == T_Identifier) {
                        name.addAtEnd(identValue());
                        id.append(identValue());
                    }
                    eat(T_Identifier);
                }
            }
            Seq<Str*>* n = name.get();
            if (qualified) {
                compiler->internalWarning(position(), "Unimplemented: Qualified import not supported yet, using an unqualified import instead");
                addOpenNamespace(ALLOC(CommonNamespace, (id.str())));
                addQualifiedImport(n);
            }
            else {
                addOpenNamespace(ALLOC(CommonNamespace, (id.str())));
                addUnqualifiedImport(n);
            }
            return ALLOC(EmptyStmt, ());
        }
        
        Stmt* Parser::labeledStatement()
        {
            Str* label = identifier();
            eat(T_Colon);
            
            Stmt* stmt = statement();
            
            Stmt* s = stmt;
            while (s->isLabeledStmt())
                s = ((LabeledStmt*)s)->stmt;
            if (s->isLabelSetStmt()) {
                LabelSetStmt* ls = (LabelSetStmt*)s;
                ls->labels = ALLOC(Seq<Str*>, (label, ls->labels));
            }
            
            return ALLOC(LabeledStmt, (label, stmt));
        }
        
        Stmt* Parser::returnStatement()
        {
            eat (T_Return);
            uint32_t pos = position();
            if (topRib->tag != RIB_Function)
                compiler->syntaxError(pos, SYNTAXERR_RETURN_OUTSIDE_FN);
            Expr* expr = NULL;
            if (noNewline()) {
                if (topRib->signature->isVoid)
                    compiler->syntaxError(pos, SYNTAXERR_VOIDFN_RETURNS_VALUE);
                expr = commaExpression(0);
            }
            return ALLOC(ReturnStmt, (pos, expr));
        }
        
        Stmt* Parser::breakStatement()
        {
            uint32_t pos = position();
            return ALLOC(BreakStmt, (pos, breakOrContinueLabel(T_Break)));
        }
        
        Stmt* Parser::continueStatement()
        {
            uint32_t pos = position();
            return ALLOC(ContinueStmt, (pos, breakOrContinueLabel(T_Continue)));
        }
        
        Str* Parser::breakOrContinueLabel(Token tok)
        {
            eat(tok);
            return noNewline() ? identifier() : NULL;
        }

        Stmt* Parser::gotoStatement()
        {
            uint32_t pos = position();
            eat(T_Goto);
            Str* label = identifier();
            setUsesGoto();
            return ALLOC(GotoStmt, (pos, label));
        }

        // 'default' has been consumed, hd() is the identifier 'xml'
        Stmt* Parser::defaultXmlNamespaceStatement()
        {
            uint32_t pos = position();
            if(hd() != T_Identifier || identValue() != compiler->SYM_xml)
                goto failure;
            eat(T_Identifier);
            if(hd() != T_Namespace)
                goto failure;
            eat(T_Namespace);
            eat(T_Assign);
            setUsesDefaultXmlNamespace();
            return ALLOC(DefaultXmlNamespaceStmt, (pos, commaExpression(0)));
        failure:
            compiler->syntaxError(pos, SYNTAXERR_EXPECT_DXNS);
            /*NOTREACHED*/
            return NULL;
        }
        
        Stmt* Parser::ifStatement()
        {
            eat(T_If);
            uint32_t pos = position();
            Expr* test = parenExpression();
            Stmt* consequent = statement();
            Stmt* alternate = NULL;
            if (match(T_Else))
                alternate = statement();
            
            return ALLOC(IfStmt, (pos, test, consequent, alternate));
        }
        
        Stmt* Parser::whileStatement()
        {
            eat(T_While);
            uint32_t pos = position();
            Expr* expr = parenExpression();
            Stmt* body = statement();
            
            return ALLOC(WhileStmt, (pos, expr, body));
        }
        
        Stmt* Parser::doStatement()
        {
            eat(T_Do);
            Stmt* body = statement();
            eat(T_While);
            uint32_t pos = position();
            Expr* expr = parenExpression ();

            return ALLOC(DoWhileStmt, (pos, expr, body));
        }
        
        Stmt* Parser::forStatement()
        {
            Expr* init=NULL;
            Expr* lhs=NULL;
            uint32_t numbindings = 0;
            bool is_each = false;

            eat (T_For);
            if (hd() == T_Identifier && identValue() == compiler->SYM_each) {
                is_each = true;
                eat(T_Identifier);
            }
            uint32_t pos = position();
            eat (T_LeftParen);
            
            if (hd() == T_Var)
            {
                uint32_t dummy = 0;
                init = varBindings(&dummy, defaultNamespace(), false, false, EFLAG_NoIn, &numbindings, &lhs);
            }
            else if (hd() == T_Semicolon)
                ;
            else
                lhs = init = commaExpression(EFLAG_NoIn);
            
            if (match(T_In)) {
                if (numbindings > 1)
                    compiler->syntaxError(pos, SYNTAXERR_FOR_IN_ONEBINDING);

                Expr* objexpr = commaExpression(0);
                eat (T_RightParen);
                Stmt* body = statement();
                
                AvmAssert( lhs != NULL );
                return ALLOC(ForInStmt, (pos, lhs, init, objexpr, body, is_each));
            }
            else {
                if (is_each)
                    compiler->syntaxError(pos, SYNTAXERR_FOR_EACH_REQS_IN);

                eat(T_Semicolon);
                Expr* test = hd() == T_Semicolon ? NULL : commaExpression(0);
                eat(T_Semicolon);
                Expr* update = hd() == T_RightParen ? NULL : commaExpression(0);
                eat(T_RightParen);
                Stmt* body = statement ();

                return ALLOC(ForStmt, (pos, init, test, update, body));
            }
        }
        
        Stmt* Parser::switchStatement()
        {
            eat (T_Switch);
            uint32_t pos = position();
            Expr* expr = parenExpression ();

            eat (T_LeftBrace);
            Seq<CaseClause*>* cases = NULL;
            if (hd() == T_Case || hd() == T_Default)
                cases = caseElements();
            eat(T_RightBrace);

            return ALLOC(SwitchStmt, (pos, expr, cases));
        }
        
        Seq<CaseClause*>* Parser::caseElements()
        {
            SeqBuilder<CaseClause*> cases(allocator);
            bool hasDefault = false;
            CaseClause* last = NULL;
            
            for (;;) {
                switch (hd ()) {
                    case T_RightBrace:
                        return cases.get();
                        
                    case T_Default: {
                        if (hd2() != T_Colon)
                            goto just_a_statement;  // default xml namespace
                        eat(T_Default);
                        eat(T_Colon);
                        if (hasDefault)
                            compiler->syntaxError(position(), SYNTAXERR_DUPLICATE_DEFAULT);
                        hasDefault = true;
                        cases.addAtEnd(last = ALLOC(CaseClause, (0, NULL)));
                        break;
                    }
                        
                    case T_Case: {
                        eat(T_Case);
                        uint32_t pos = position();
                        Expr* expr = commaExpression(0);
                        eat(T_Colon);
                        cases.addAtEnd(last = ALLOC(CaseClause, (pos, expr)));
                    }
                    /*FALLTHROUGH*/
                    just_a_statement:
                    default: {
                        if (last == NULL)
                            compiler->syntaxError(position(), SYNTAXERR_EXPECT_CASE_OR_DEFAULT);
                        AvmAssert(last->stmts == NULL);
                        SeqBuilder<Stmt*> stmts(allocator);
                        while (hd() != T_RightBrace && hd() != T_Case && hd() != T_Default)
                            stmts.addAtEnd(statement());
                        last->stmts = stmts.get();
                        break;
                    }
                }
            }
        }
        
        Stmt* Parser::throwStatement()
        {
            eat (T_Throw);
            uint32_t pos = position();
            return ALLOC(ThrowStmt, (pos, commaExpression(0)));
        }
        
        Stmt* Parser::tryStatement()
        {
            eat (T_Try);

            Seq<Stmt*>* tryblock = statementBlock();
            Seq<CatchClause*>* catchblocks = catches();
            Seq<Stmt*>* finallyblock = NULL;

            if (match(T_Finally)) {
                setUsesFinally();
                finallyblock = statementBlock();
            }
 
            return ALLOC(TryStmt, (tryblock, catchblocks,finallyblock));
        }
        
        Seq<CatchClause*>* Parser::catches()
        {
            SeqBuilder<CatchClause*> catches(allocator);

            // Sort of silly that this allows multiple catches yet catchClause() does not
            // allow the clauses to discriminate by type.

            while (match(T_Catch)) {
                setUsesCatch();
                catches.addAtEnd(catchClause());
            }

            return catches.get();
        }
        
        CatchClause* Parser::catchClause()
        {
            eat (T_LeftParen);
            Str* catchvar_name = identifier();
            Type* catchvar_type_name = NULL;
            if (match(T_Colon))
                catchvar_type_name = typeExpression();
            eat (T_RightParen);
            Seq<Stmt*>* catchblock = statementBlock();
            return ALLOC(CatchClause, (catchvar_name, catchvar_type_name, catchblock));
        }
        
        Stmt* Parser::withStatement()
        {
            eat (T_With);
            uint32_t pos = position();
            Expr* expr = parenExpression();
            Stmt* body = statement();
            return ALLOC(WithStmt, (pos, expr, body));
        }

        // This also parses super expressions when they appear in the statement position.
        Stmt* Parser::superStatement()
        {
            eat (T_Super);
            uint32_t pos = position();
            Seq<Expr*>* arguments = NULL;
            bool argsPresent = false;
            if (hd() == T_LeftParen) {
                argsPresent = true;
                arguments = argumentList();
            }
            // FIXME: We want to abstract "check for property operator" here.
            if (argsPresent && hd() != T_Dot && hd() != T_DoubleDot && hd() != T_LeftDotAngle && hd() != T_LeftBracket) {
                setUsesSuper();
                return ALLOC(SuperStmt, (pos, arguments));
            }
            if (argsPresent && (arguments == NULL || arguments->tl != NULL))
                compiler->syntaxError(pos, SYNTAXERR_ONE_ARGUMENT_REQUIRED);
            Expr* obj = argsPresent ? arguments->hd : ALLOC(ThisExpr, ());
            return ALLOC(ExprStmt, (pos, propertyOperator(ALLOC(SuperExpr, (obj)))));
        }
    }
}

#endif // VMCFG_EVAL
