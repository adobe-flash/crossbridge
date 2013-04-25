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
        
        /*
         static const struct TokenMapping {
             unsigned isOpAssign:1;
             unsigned isMultiplicative:1;
             unsigned isAdditive:1;
             unsigned isRelational:1;
             unsigned isEquality:1;
             unsigned isShift:1;
             unsigned unaryOp:8;
             unsigned binaryOp:8;
         };
         */
        const Parser::TokenMapping Parser::tokenMapping[] = {
        { 0, 0, 0, 1, 0, 0, 0,              OPR_as },           // T_As
        { 0, 0, 0, 0, 0, 0, 0,              OPR_assign },       // T_Assign,
        { 0, 0, 0, 0, 0, 0, 0,              OPR_bitwiseAnd },   // T_BitwiseAnd,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_bitwiseAnd },   // T_BitwiseAndAssign,
        { 0, 0, 0, 0, 0, 0, OPR_bitwiseNot, 0 },                // T_BitwiseNot,
        { 0, 0, 0, 0, 0, 0, 0,              OPR_bitwiseOr },    // T_BitwiseOr,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_bitwiseOr },    // T_BitwiseOrAssign,
        { 0, 0, 0, 0, 0, 0, 0,              OPR_bitwiseXor },   // T_BitwiseXor,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_bitwiseXor },   // T_BitwiseXorAssign,
        { 0, 0, 0, 0, 0, 0, OPR_delete,     0 },                // T_Delete,
        { 0, 1, 0, 0, 0, 0, 0,              OPR_divide },       // T_Divide,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_divide },       // T_DivideAssign,
        { 0, 0, 0, 0, 1, 0, 0,              OPR_equal },        // T_Equal,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_greater },      // T_GreaterThan,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_greaterOrEqual }, // T_GreaterThanOrEqual,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_in },           // T_In,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_instanceof },   // T_InstanceOf,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_is },           // T_Is,
        { 0, 0, 0, 0, 0, 1, 0,              OPR_leftShift },    // T_LeftShift,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_leftShift },    // T_LeftShiftAssign,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_less },         // T_LessThan,
        { 0, 0, 0, 1, 0, 0, 0,              OPR_lessOrEqual },  // T_LessThanOrEqual,
        { 0, 0, 0, 0, 0, 0, 0,              OPR_logicalAnd },   // T_LogicalAnd,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_logicalAnd },   // T_LogicalAndAssign,
        { 0, 0, 0, 0, 0, 0, 0,              OPR_logicalOr },    // T_LogicalOr,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_logicalOr },    // T_LogicalOrAssign,
        { 0, 0, 1, 0, 0, 0, OPR_unminus,    OPR_minus },        // T_Minus,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_minus },        // T_MinusAssign,
        { 0, 0, 0, 0, 0, 0, OPR_preDecr,    OPR_postDecr },     // T_MinusMinus, note binop slot encodes postfix form
        { 0, 1, 0, 0, 0, 0, 0,              OPR_multiply },     // T_Multiply,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_multiply },     // T_MultiplyAssign,
        { 0, 0, 0, 0, 0, 0, OPR_not,        0 },                // T_Not,
        { 0, 0, 0, 0, 1, 0, 0,              OPR_notEqual },     // T_NotEqual,
        { 0, 0, 1, 0, 0, 0, OPR_unplus,     OPR_plus },         // T_Plus,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_plus },         // T_PlusAssign,
        { 0, 0, 0, 0, 0, 0, OPR_preIncr,    OPR_postIncr },     // T_PlusPlus, note binop slot encodes postfix form
        { 0, 1, 0, 0, 0, 0, 0,              OPR_remainder },    // T_Remainder,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_remainder },    // T_RemainderAssign,
        { 0, 0, 0, 0, 0, 1, 0,              OPR_rightShift },   // T_RightShift,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_rightShift },   // T_RightShiftAssign,
        { 0, 0, 0, 0, 1, 0, 0,              OPR_strictEqual },  // T_StrictEqual,
        { 0, 0, 0, 0, 1, 0, 0,              OPR_strictNotEqual }, // T_StrictNotEqual,
        { 0, 0, 0, 0, 0, 0, OPR_typeof,     0 },                // T_TypeOf,
        { 0, 0, 0, 0, 1, 0, 0,              OPR_rightShiftUnsigned }, // T_UnsignedRightShift,
        { 1, 0, 0, 0, 0, 0, 0,              OPR_rightShiftUnsigned }, // T_UnsignedRightShiftAssign,
        { 0, 0, 0, 0, 0, 0, OPR_void,       0 }                 // T_Void,
        };

        Parser::BindingRib::BindingRib(Allocator* allocator, BindingRib* next, RibType tag)
            : tag(tag)
            , signature(NULL)
            , body(allocator)
            , next(next)
        {
        }

        Parser::Parser(Compiler* compiler, Lexer* lexer, bool public_by_default, uint32_t first_line)
            : compiler(compiler)
            , allocator(compiler->allocator)
            , public_by_default(public_by_default)
            , line_offset(first_line-1)
            , internalNS(ALLOC(BuiltinNamespace, (T_Internal)))
            , privateNS(ALLOC(BuiltinNamespace, (T_Private)))
            , protectedNS(ALLOC(BuiltinNamespace, (T_Protected)))
            , publicNS(ALLOC(BuiltinNamespace, (T_Public)))
            , topRib(NULL)
            , classes(allocator)
            , configNamespaces(NULL)
            , configBindings(NULL)
            , lexerStack(NULL)
            , lexer(lexer)
            , T0(T_LAST)
            , T1(T_LAST)
            , LP(0)
            , L0(0)
            , L1(0)
            , included_input(NULL)
        {
        }
        
        Program* Parser::parse()
        {
            start();
            return program();
        }
        
        /*
         * <program> ::= <package>* <directive>*
         */
        Program* Parser::program()
        {
            addConfigNamespace(compiler->SYM_CONFIG);
            pushBindingRib(RIB_Program);
            addNamespaceBinding(defaultNamespace(), compiler->SYM_AS3, ALLOC(LiteralString, (compiler->intern("http://adobe.com/AS3/2006/builtin"), 0)));
            while (hd() == T_Package)
                package();
            directives(SFLAG_Toplevel);
            Program* prog = ALLOC(Program, (topRib->body, classes.get()));
            popBindingRib();
            return prog;
        }
        
        // FIXME: must open the package namespaces inside the package
        // FIXME: must record the package name as a compound name prefix inside the package
        // FIXME: must record that we're inside a package so that namespace qualifiers can be checked
        void Parser::package()
        {
            SeqBuilder<Str*> name(allocator);
            eat(T_Package);
            while (hd() == T_Identifier) {
                name.addAtEnd(identValue());
                eat(T_Identifier);
                if (!match(T_Dot))
                    break;
            }
            eat(T_LeftBrace);
            directives(SFLAG_Package);
            eat(T_RightBrace);
        }
        
        void Parser::directives(int flags)
        {
            SeqBuilder<Stmt*>* instanceStmts = &(topRib->body.stmts);
            SeqBuilder<Stmt*>* classStmts = (flags & SFLAG_Class) ? &(topRib->next->body.stmts) : NULL;

            // Directive parser machine state.

            SeqBuilder<Expr*> metadatas(allocator); // Metadata expressions, in order
            QualifiedName* configname;              // Non-NULL if we have a config name, will also appear in metadatas
            Qualifier qual;                         // ns / native / static / prototype / dynamic / override / committed metadata
            bool metadata;                          // We're still looking at metadata and have not moved to qualifiers
            bool committed;                         // We're committed to parsing a directive

            // Working storage

            Expr* e;                                // Pending expression on entry to expr_statement

        next_directive:
            metadatas.clear();
            configname = NULL;
            qual = Qualifier();
            metadata = true;
            committed = false;
            e = NULL;

            switch (hd()) {
                //case T_Import:     goto import_directive;
                case T_Include:    goto include_directive;
                //case T_Use:        goto use_directive;
                //case T_Package:    goto package_directive;
                case T_Identifier: {
                    if (identValue() == compiler->SYM_config && hd2() == T_Namespace && L0 == L1)
                        goto config_namespace_directive;
                    goto continue_directive;
                }
                default:           goto continue_directive;
            }

        continue_directive:
            if (hd() == T_RightBrace || hd() == T_EOS) {
                if (committed)
                    compiler->syntaxError(position(), SYNTAXERR_DIRECTIVE_REQUIRED);
                addExprStatements((flags & SFLAG_Class) ? classStmts : instanceStmts, metadatas.get());
                goto finish;
            }

            switch (hd())
            {
                case T_LeftBracket:    goto array_metadata;
                case T_BreakLeftAngle: goto xml_metadata;
                case T_Private:        goto special_namespace_qualifier;
                case T_Protected:      goto special_namespace_qualifier;
                case T_Public:         goto special_namespace_qualifier;
                case T_Internal:       goto special_namespace_qualifier;
                case T_Dynamic:        goto dynamic_qualifier;
                case T_Final:          goto final_qualifier;
                case T_Native:         goto native_qualifier;
                case T_Override:       goto override_qualifier;
                case T_Static:         goto static_qualifier;
                case T_Namespace:      goto namespace_directive;
                case T_Const:          goto const_or_var_directive;
                case T_Var:            goto const_or_var_directive;
                case T_Function:       goto function_directive;
                case T_Class:          goto class_directive;
                case T_Interface:      goto interface_directive;
                case T_Identifier:     goto configname_or_namespacename_or_statement;
                default:               goto statement;
            }
            
        xml_metadata: {
            // Could be xml-literal metadata or expression statement.
            //
            // Must parse full expression here in case it's not metadata, eg,
            //   <foo></foo> + "bar"
            // is a valid statement.
            if (!metadata)
                compiler->syntaxError(position(), SYNTAXERR_METADATA_NOT_ALLOWED);
            e = commaExpression(0);
            if (e->tag() == TAG_literalXml) {
                committed = true;
                metadatas.addAtEnd(e);
                e = NULL;
                goto continue_directive;
            }

            // Not metadata after all
            goto expr_statement;
        }

        array_metadata: {
            // Could be array-literal metadata or expression statement.
            //
            // Must parse full expression here in case it's not metadata, eg,
            //   [native(a,b,c)] + " = " + x
            // or
            //   [a,b,c] = 10
            // are valid statements (or will be).
            if (!metadata)
                compiler->syntaxError(position(), SYNTAXERR_METADATA_NOT_ALLOWED);
            e = commaExpression(0);
            if (e->tag() == TAG_literalArray) {
                committed = true;
                metadatas.addAtEnd(e);
                e = NULL;
                goto continue_directive;
            }

            // Not metadata after all
            goto expr_statement;
        }

        configname_or_namespacename_or_statement: {
            // Here we're looking at some identifier.  It can be the start of: a config name
            // reference (always of the form NS::v), a namespace reference (anything from a
            // simple unqualified name through a string of namespace qualifiers and finally a
            // name), a labeled statement, or an expression statement.  The special names
            // private, public, protected, and internal do not appear.
            //
            // If it's followed by a colon then it's a label.  Otherwise we must parse it as
            // a full comma expression and then start disambiguating.
            //
            // In the AST, a simple unqualified name is a QualifiedName with a NULL qualifier
            // and a SimpleName for a name.
            
            if (hd2() == T_Colon)
                goto statement; // labeled statement
            e = commaExpression(0);
            if (e->tag() != TAG_qualifiedName)
                goto expr_statement;
            if (newline() || hd() == T_Semicolon || hd() == T_RightBrace)
                goto configname_or_statement;
            if (isConfigReference(e))
                goto configname;
            goto namespace_or_statement;

        configname_or_statement:
            if (!isConfigReference(e))
                goto expr_statement;
            goto configname;

        configname:
            if (!metadata)
                compiler->syntaxError(position(), SYNTAXERR_DIRECTIVE_REQUIRED);
            if (configname != NULL) {
                // Flush up to and including previous configname
                while (!metadatas.isEmpty()) {
                    Expr* p = metadatas.dequeue();
                    if (p == NULL)
                        break;  // Oops - really an error
                    ((flags & SFLAG_Class) ? classStmts : instanceStmts)->addAtEnd(ALLOC(ExprStmt, (e->pos, e)));
                    if (e == configname)
                        break;
                }
            }
            configname = (QualifiedName*)e;
            metadatas.addAtEnd(e);
            e = NULL;
            if (!newline())
                committed = true;
            goto continue_directive;

        expr_statement:
            // When we get here there must be an expr in e that is
            // an expression statement.  We must not be committed,
            // but there can be metadatas - those are also expression
            // statements that were newline-terminated.  We have not
            // consumed any semicolon at the end of the current
            // expression statement.
            AvmAssert(e != NULL);
            if (committed)
                compiler->syntaxError(position(), SYNTAXERR_DIRECTIVE_REQUIRED);
            semicolon();
            metadatas.addAtEnd(e);
            addExprStatements((flags & SFLAG_Class) ? classStmts : instanceStmts, metadatas.get());
            goto next_directive;

        namespace_or_statement:
            AvmAssert(!newline());
            if (!isNamespaceReference(e))
                goto expr_statement;
            if (qual.tag != QUAL_none)
                compiler->syntaxError(position(), SYNTAXERR_DUPLICATE_QUALIFIER);
            qual.tag = QUAL_name;
            qual.name = (QualifiedName*)e;
            committed = true;
            metadata = false;
            e = NULL;
            goto continue_directive;
        }

        special_namespace_qualifier: {
            // The namespace qualifiers public, private, protected, and internal can be used
            // either by themselves as qualifiers or as part of a qualified name, eg,
            // 'internal::foo'.  In the latter case, we also risk looking at an expression
            // statement, eg, 'internal::foo() + fnord'.  However, the namespace qualifiers
            // are not themselves valid expressions, so we can't blithely parse an expression
            // here.  We must instead look ahead: if we're seeing '::' next then we can parse
            // an expression and then disambiguate between a qualified namespace name or
            // an expression statement, otherwise we take the keyword to be a qualifier.
            //
            // Configuration names do not appear here because private, public, protected, and
            // internal never denote a configuration namespace.
            
            Token tok = hd();
            QualifierTag t = QUAL_none;
            if (hd2() != T_DoubleColon) {
                next();
                switch (tok) {
                    case T_Private:   t = QUAL_private; break;
                    case T_Public:    t = QUAL_public; break;
                    case T_Protected: t = QUAL_protected; break;
                    case T_Internal:  t = QUAL_internal; break;
                }
                goto consumed_special_namespace_qualifier;
            }

            {
                Expr* e = commaExpression(0);
                if (e->tag() != TAG_qualifiedName || newline()) {
                    if (committed)
                        compiler->syntaxError(position(), SYNTAXERR_DIRECTIVE_REQUIRED);
                    // Expression statement
                    semicolon();
                    metadatas.addAtEnd(e);
                    addExprStatements((flags & SFLAG_Class) ? classStmts : instanceStmts, metadatas.get());
                    goto next_directive;
                }

                t = QUAL_name;
                qual.name = (QualifiedName*)e;
                goto consumed_special_namespace_qualifier;
            }

        consumed_special_namespace_qualifier:
            if (qual.tag != QUAL_none)
                compiler->syntaxError(position(), SYNTAXERR_KWD_NOT_ALLOWED, "private");
            qual.tag = t;
            committed = true;
            metadata = false;
            goto continue_directive;
        }

        dynamic_qualifier: {
            qual.is_dynamic++;
            goto consume_simple_qualifier;
        }

        final_qualifier: {
            qual.is_final++;
            goto consume_simple_qualifier;
        }

        native_qualifier: {
            qual.is_native++;
            goto consume_simple_qualifier;
        }
            
        override_qualifier: {
            qual.is_override++;
            goto consume_simple_qualifier;
        }

        static_qualifier: {
            qual.is_static++;
            goto consume_simple_qualifier;
        }

        consume_simple_qualifier: {
            next();
            committed = true;
            metadata = false;
            goto continue_directive;
        }

        config_namespace_directive: {
            // No attributes or metadata will have been parsed
            eat(T_Identifier);
            eat(T_Namespace);
            if (!(flags & SFLAG_Toplevel))
                compiler->syntaxError(position(), SYNTAXERR_CONFIG_NAMESPACE_NOT_ALLOWED);
            configNamespaceDefinition(flags, evaluateConfigReference(configname));
            goto next_directive;
        }
            
        include_directive: {
            // No attributes or metadata will have been parsed
            includeDirective();
            goto next_directive;
        }

        namespace_directive: {
            checkSimpleAttributes(&qual);
            if (!(flags & (SFLAG_Function|SFLAG_Toplevel|SFLAG_Package|SFLAG_Class)))
                compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_NAMESPACE);
            if (qual.is_dynamic || qual.is_final || qual.is_native || qual.is_override || qual.is_static)
                compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_NAMESPACE);
            namespaceDefinition(evaluateConfigReference(configname), flags, &qual);
            goto next_directive;
        }

        const_or_var_directive: {
            checkSimpleAttributes(&qual);
            if (flags & SFLAG_Interface)
                compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_IN_INTERFACE);
            if (qual.is_dynamic || qual.is_final || qual.is_native || qual.is_override ||
                (qual.is_static && !(flags & SFLAG_Class)) ||
                (qual.tag != QUAL_none && !(flags & (SFLAG_Toplevel|SFLAG_Package|SFLAG_Class))))
                compiler->syntaxError(position(), SYNTAXERR_QUALIFIER_NOT_ALLOWED);
            // FIXME: if const, and at the top level, and the namespace is a config namespace, evaluate and define a config const
            ((flags & SFLAG_Class) && qual.is_static ? classStmts : instanceStmts)->addAtEnd(variableDefinition(evaluateConfigReference(configname), &qual));
            goto next_directive;
        }

        function_directive: {
            checkSimpleAttributes(&qual);
            if (qual.is_dynamic ||
                ((qual.is_final || qual.is_override || qual.is_static) && !(flags & SFLAG_Class)) ||
                (qual.tag != QUAL_none && !(flags & (SFLAG_Toplevel|SFLAG_Package|SFLAG_Class))))
                compiler->syntaxError(position(), SYNTAXERR_QUALIFIER_NOT_ALLOWED);
            functionDefinition(evaluateConfigReference(configname), &qual, (flags & SFLAG_Class) != 0, (flags & SFLAG_Interface) == 0);
            goto next_directive;
        }

        class_directive: {
            checkSimpleAttributes(&qual);
            if (!(flags & (SFLAG_Toplevel|SFLAG_Package)))
                compiler->syntaxError(position(), SYNTAXERR_CLASS_NOT_ALLOWED);
            if (qual.is_native || qual.is_override || qual.is_static)
                compiler->syntaxError(position(), SYNTAXERR_QUALIFIER_NOT_ALLOWED);
            classDefinition(evaluateConfigReference(configname), flags, &qual);
            goto next_directive;
        }

        interface_directive: {
            checkSimpleAttributes(&qual);
            if (!(flags & (SFLAG_Toplevel|SFLAG_Package)))
                compiler->syntaxError(position(), SYNTAXERR_INTERFACE_NOT_ALLOWED);
            if (qual.is_dynamic || qual.is_final || qual.is_native || qual.is_override || qual.is_static ||
                (qual.tag != QUAL_none && qual.tag != QUAL_public))
                compiler->syntaxError(position(), SYNTAXERR_QUALIFIER_NOT_ALLOWED);
            interfaceDefinition(evaluateConfigReference(configname), flags, &qual);
            goto next_directive;
        }

        statement: {
            AvmAssert(e == NULL);
            if (committed)
                compiler->syntaxError(position(), SYNTAXERR_DIRECTIVE_REQUIRED);
            addExprStatements((flags & SFLAG_Class) ? classStmts : instanceStmts, metadatas.get());
            if (flags & SFLAG_Interface)
                compiler->syntaxError(position(), SYNTAXERR_STMT_IN_INTERFACE);
            // FIXME: is this test right?
            if (qual.tag != QUAL_none)
                compiler->syntaxError(position(), SYNTAXERR_ILLEGAL_STMT);
            if (configname != NULL && hd() != T_LeftBrace)
                compiler->syntaxError(position(), SYNTAXERR_CONFIG_PROHIBITED);
            ((flags & SFLAG_Class) ? classStmts : instanceStmts)->addAtEnd(statement(evaluateConfigReference(configname)));
            goto next_directive;
        }

        finish: ;
        }

        void Parser::checkSimpleAttributes(Qualifier* qual)
        {
            // FIXME: if the defining keyword is not const, and a namespace is present,
            // then that namespace must not be a config namespace.
            if ((qual->is_dynamic > 1 || qual->is_final > 1 || qual->is_native > 1 || qual->is_override > 1 || qual->is_static > 1) ||
                (qual->is_static + qual->is_final > 1) ||
                (qual->is_static + qual->is_override > 1) ||
                (qual->is_static + qual->is_dynamic > 1))
                compiler->syntaxError(position(), SYNTAXERR_QUALIFIER_NOT_ALLOWED);
        }
        
        // FIXME: This is incorrect because QualifiedName currently does not admit a::b::c
        bool Parser::isNamespaceReference(Expr* e)
        {
            if (e->tag() != TAG_qualifiedName)
                return false;
            QualifiedName* qn = (QualifiedName*)e;
            if (qn->name->tag() != TAG_simpleName)
                return false;
            if (qn->qualifier != NULL && qn->qualifier->tag() != TAG_simpleName)
                return false;
            return true;
        }

        void Parser::addExprStatements(SeqBuilder<Stmt*>* stmts, Seq<Expr*>* exprs)
        {
            while (exprs != NULL) {
                Expr* e = exprs->hd;
                exprs = exprs->tl;
                stmts->addAtEnd(ALLOC(ExprStmt, (e->pos, e)));
            }
        }

        // Qualifiers are known to be appropriate for 'class'.

        void Parser::classDefinition(bool config, int /*flags*/, Qualifier* qual)
        {
            (void)config;
            eat(T_Class);
            uint32_t pos = position();
            Str* name = identifier();
            checkNoShadowingOfConfigNamespaces(pos, name);
            Str* extends = NULL;
            SeqBuilder<Str*> implements(allocator);
            if (hd() == T_Identifier && identValue() == compiler->SYM_extends) {
                next();
                extends = identifier();
            }
            if (hd() == T_Identifier && identValue() == compiler->SYM_implements) {
                next();
                do {
                    implements.addAtEnd(identifier());
                } while (match(T_Comma));
            }
            eat(T_LeftBrace);
            pushBindingRib(RIB_Class);
            pushBindingRib(RIB_Instance);
            directives(SFLAG_Class);
            
            // The initializing statements of the class constructor are hoisted
            // and become cls_init1, these are executed in an outer scope.  Any other
            // statements are part of the constructor and executed after the super stmt,
            // and are within the constructor body.  This means that during parsing we
            // must distinguish between initializing stores and other statements.  We hack:
            // just filter the statements after the fact, variable definition stmts are
            // distinguishable from other statements.
            //
            // FIXME: the hack is somehow wrong, consider
            //
            //   class C {
            //      L1: var c = v;
            //   }
            //
            // where "c = v" is part of the instance initializer.  Now put it in a (static) loop.
            // It should not be executed by that loop, but it is.

            BodyInfo* cls = &topRib->next->body;
            Seq<FunctionDefn*>* cls_methods = cls->functionDefinitions.get();
            Seq<Binding*>* cls_vars = bindingsToVars(cls->bindings.get());
            Seq<NamespaceDefn*>* cls_namespaces = cls->namespaces.get();
            Seq<Stmt*>* static_stmts = NULL;
            Seq<Stmt*>* cls_init_stmts = filterStatements(cls->stmts.get(), &static_stmts);
            FunctionDefn* cls_init = constructClassConstructor(pos, name, cls, cls_init_stmts, static_stmts);

            // For the instance constructor only the initializing stores appear in the
            // statements list; this list becomes init1, whereas the constructor (user-provided
            // or hand-written) becomes init2.

            BodyInfo* instance = &topRib->body;
            FunctionDefn* constructor = NULL;
            Seq<FunctionDefn*>* inst_methods = filterConstructor(name, instance->functionDefinitions.get(), &constructor);
            Seq<Binding*>* inst_vars = bindingsToVars(instance->bindings.get());
            Seq<Stmt*>* inst_init_stmts = instance->stmts.get();
            FunctionDefn* inst_init = constructInstanceConstructor(pos, name, inst_init_stmts, constructor);
            
            ClassDefn* clsobj = ALLOC(ClassDefn, (pos,
                                                  qualToNamespace(qual), name,
                                                  qual->is_final > 0, qual->is_dynamic > 0,
                                                  extends, implements.get(),
                                                  cls_methods, cls_vars, cls_namespaces, cls_init,
                                                  inst_methods, inst_vars, inst_init));

            popBindingRib();
            popBindingRib();
            eat(T_RightBrace);
            addClass(clsobj);
        }
        
        ClassDefn::ClassDefn(uint32_t pos,
                             NameComponent* ns, Str* name, bool is_final, bool is_dynamic, Str* extends, Seq<Str*>* implements,
                             Seq<FunctionDefn*>* cls_methods, Seq<Binding*>* cls_vars, Seq<NamespaceDefn*>* cls_namespaces,
                             FunctionDefn* cls_init,
                             Seq<FunctionDefn*>* inst_methods, Seq<Binding*>* inst_vars,
                             FunctionDefn* inst_init)
            : TypeDefn(pos, ns, name, false)
            , is_final(is_final)
            , is_dynamic(is_dynamic)
            , extends(extends)
            , implements(implements)
            , cls_methods(cls_methods)
            , cls_vars(cls_vars)
            , cls_namespaces(cls_namespaces)
            , cls_init(cls_init)
            , inst_methods(inst_methods)
            , inst_vars(inst_vars)
            , inst_init(inst_init)
        {
        }

        FunctionDefn* Parser::constructClassConstructor(uint32_t pos, Str* name, BodyInfo* old_body, Seq<Stmt*>* init_stmts, Seq<Stmt*>* stmts)
        {
            SignatureInfo signature(allocator);
            BodyInfo body(allocator);
            StringBuilder fnname(compiler);

            fnname.append(name);
            fnname.append("$cinit");

            signature.ns = publicNS;
            signature.name = fnname.str();

            body.uses_finally = old_body->uses_finally;
            body.uses_catch = old_body->uses_catch;
            body.uses_goto = old_body->uses_goto;
            body.uses_dxns = old_body->uses_dxns;
            
            while (stmts != NULL) {
                body.stmts.addAtEnd(stmts->hd);
                stmts = stmts->tl;
            }
            body.stmts.addAtEnd(ALLOC(SuperStmt, (pos, NULL)));

            return ALLOC(FunctionDefn, (signature, body, init_stmts));
        }

        // For the instance init we want a function derived from the real constructor method,
        // it is called CLASSNAME$iinit.  If there's no constructor method one is generated.
        // Initializer statements are inserted at the beginning of the constructor body; these
        // statements are the statements in the "inst" body.  Finally, if there's no super
        // call in the constructor method then one gets inserted at the beginning.
        //
        // The spec says:
        //  - initializers are evaluated *in the instance scope*, not in the scope of the
        //    constructor.  Thus they cannot simply be added to the start of the constructor
        //    body, somehow the code generator needs to be in on the joke.
        //  - super is called at the beginning of the constructor body, after var init,
        //    if not present.

        FunctionDefn* Parser::constructInstanceConstructor(uint32_t pos, Str* name, Seq<Stmt*>* init_stmts, FunctionDefn* constructor)
        {
            if (constructor) {
                if (!constructor->uses_super)
                    constructor->stmts = ALLOC(Seq<Stmt*>, (ALLOC(SuperStmt, (pos, NULL)), constructor->stmts));
                constructor->inits = init_stmts;
                return constructor;
            }
            else {
                SignatureInfo signature(allocator);
                BodyInfo body(allocator);
                StringBuilder fnname(compiler);
                
                fnname.append(name);
                fnname.append("$iinit");
                
                signature.ns = publicNS;
                signature.name = fnname.str();

                body.stmts.addAtEnd(ALLOC(SuperStmt, (pos, NULL)));

                return ALLOC(FunctionDefn, (signature, body, init_stmts));
            }
        }
        
        Seq<Stmt*>* Parser::filterStatements(Seq<Stmt*>* stmts, Seq<Stmt*>** nonDefinitionStmts)
        {
            SeqBuilder<Stmt*> defStmts(allocator);
            SeqBuilder<Stmt*> nodefStmts(allocator);
            
            while (stmts != NULL) {
                if (stmts->hd->isVardefStmt())
                    defStmts.addAtEnd(stmts->hd);
                else
                    nodefStmts.addAtEnd(stmts->hd);
                stmts = stmts->tl;
            }
            *nonDefinitionStmts = nodefStmts.get();
            return defStmts.get();
        }

        Seq<FunctionDefn*>* Parser::filterConstructor(Str* name, Seq<FunctionDefn*>* fns, FunctionDefn** constructor)
        {
            SeqBuilder<FunctionDefn*> out(allocator);
            *constructor = NULL;
            while (fns != NULL) {
                if (fns->hd->name == name) {
                    if (*constructor != NULL)
                        compiler->internalError(0, "Multiple constructors in list??");
                    *constructor = fns->hd;
                }
                else
                    out.addAtEnd(fns->hd);
                fns = fns->tl;
            }
            return out.get();
        }

        Seq<Binding*>* Parser::bindingsToVars(Seq<Binding*>* bindings)
        {
            SeqBuilder<Binding*> l(allocator);
            while (bindings != NULL) {
                if (bindings->hd->kind == TAG_varBinding || bindings->hd->kind == TAG_constBinding)
                    l.addAtEnd(bindings->hd);
                bindings = bindings->tl;
            }
            return l.get();
        }

        void Parser::interfaceDefinition(bool config, int /*flags*/, Qualifier* qual)
        {
            (void)config;
            eat(T_Interface);
            uint32_t pos = position();
            Str* name = identifier();
            checkNoShadowingOfConfigNamespaces(pos, name);
            SeqBuilder<Str*> extends(allocator);
            if (hd() == T_Identifier && identValue() == compiler->SYM_extends) {
                next();
                do {
                    extends.addAtEnd(identifier());
                } while (match(T_Comma));
            }
            eat(T_LeftBrace);
            pushBindingRib(RIB_Instance);
            directives(SFLAG_Interface);
            
            BodyInfo* instance = &topRib->body;
            FunctionDefn* constructor = NULL;
            Seq<FunctionDefn*>* inst_methods = filterConstructor(name, instance->functionDefinitions.get(), &constructor);
            if (constructor != NULL)
                compiler->syntaxError(pos, SYNTAXERR_REDUNDANT_METHOD);
            
            InterfaceDefn* iface = ALLOC(InterfaceDefn, (pos, qualToNamespace(qual), name, extends.get(), inst_methods));

            popBindingRib();
            eat(T_RightBrace);
            addInterface(iface);
        }
        
        // Namespaces are:
        //  - allowed at the top level of classes
        //  - allowed in functions and blocks
        //  - hoisted to the function level and scoped to the entire function
        //  - initialized on entry to the function (so defining them in blocks is pretty silly)

        void Parser::namespaceDefinition(bool config, int flags, Qualifier* qual)
        {
            (void)flags;
            uint32_t pos = position();
            eat(T_Namespace);
            Str* name = identifier();
            checkNoShadowingOfConfigNamespaces(pos, name);
            Expr* value = NULL;
            if (match(T_Assign)) {
                if (hd() != T_Identifier && hd() != T_StringLiteral)
                    compiler->syntaxError(pos, SYNTAXERR_ILLEGAL_NAMESPACE);
                value = primaryExpression();
            }
            semicolon();
            if (config)
                addNamespaceBinding(qualToNamespace(qual), name, value);
        }

        void Parser::configNamespaceDefinition(int flags, bool config)
        {
            (void)flags;
            (void)config;
        }

        void Parser::includeDirective()
        {
            eat(T_Include);
            Str* newFile = stringValue();
            uint32_t pos = position();
            eat(T_StringLiteral);
            semicolon();
            if (!compiler->origin_is_file)
                compiler->syntaxError(pos, SYNTAXERR_INCLUDE_ORIGIN);
            
            // The current lexer state - including the state variables that are a part of the
            // parser object - gets pushed onto a stack, a new lexer is created for the new
            // input, and is installed.  Then we return, the caller must continue parsing as
            // if nothing had happened.  When the current lexer sees EOS it calls onEOS()
            // on the parser, which pops the lexer stack and generates a token from the popped
            // lexer.  The normal token processing path does not slow down at all and the
            // machinery is almost entirely transparent to the parser.
            
            pushLexerState();
            T0 = T1 = T_LAST;
            LP = L0 = L1 = 0;

            uint32_t inputlen;
            const wchar* input = compiler->context->readFileForEval(compiler->filename, newFile->s, &inputlen);
            if (input == NULL)
                compiler->syntaxError(pos, SYNTAXERR_INCLUDE_INACCESSIBLE);
            
            included_input = input;     // freed in onEOS below
#ifdef DEBUG
            bool doTrace = lexer->getTrace();
#endif
            lexer = ALLOC(Lexer, (compiler, input, inputlen));
#ifdef DEBUG
            if (doTrace) lexer->trace();
#endif
            start();
        }
    
        Token Parser::onEOS(uint32_t* linep, TokenValue* valuep)
        {
            if (lexerStack == NULL)
                return T_EOS;
            
            compiler->context->freeInput(included_input);
            popLexerState();

            if (T0 == T_LAST)
                next();
            *linep = L0;
            *valuep = V0;
            return T0;
        }

        void Parser::pushLexerState()
        {
            LexerState* s = ALLOC(LexerState, ());
            s->lexer = lexer;
            s->next = lexerStack;
            s->T0 = T0;
            s->T1 = T1;
            s->V0 = V0;
            s->LP = LP;
            s->L0 = L0;
            s->L1 = L1;
            s->included_input = included_input;
            lexerStack = s;
        }
        
        void Parser::popLexerState()
        {
            T0 = lexerStack->T0;
            T1 = lexerStack->T1;
            V0 = lexerStack->V0;
            V1 = lexerStack->V1;
            LP = lexerStack->LP;
            L0 = lexerStack->L0;
            L1 = lexerStack->L1;
            included_input = lexerStack->included_input;
            lexer = lexerStack->lexer;
            lexerStack = lexerStack->next;
        }
        
        void Parser::pushBindingRib(RibType tag)
        {
            topRib = ALLOC(BindingRib, (allocator, topRib, tag));
        }
        
        void Parser::popBindingRib()
        {
            topRib = topRib->next;
        }

        // Only makes sense in the context of a scope; compares name components only
        static bool sameName(NameComponent* n1, NameComponent* n2)
        {
            if (n1 == NULL && n2 == NULL)
                return true;
            if (n1 == NULL || n2 == NULL)
                return false;
            if (n1->tag() != n2->tag())
                return false;
            switch (n1->tag()) {
                case TAG_simpleName:
                    return ((SimpleName*)n1)->name == ((SimpleName*)n2)->name;  // Str is interned, so pointer comparison works
                case TAG_wildcardName:
                    return true;
                case TAG_computedName:
                    return false;
                case TAG_builtinNamespace:
                    return ((BuiltinNamespace*)n1)->t == ((BuiltinNamespace*)n2)->t;    // tokens are integer values
                default:
                    return false;
            }
        }

        static bool sameName(QualifiedName* n1, QualifiedName* n2)
        {
            // ActionScript allows eg 'var n:Number; var n;".
            if (n1 == NULL || n2 == NULL)
                return true;
            return sameName(n1->qualifier, n2->qualifier) && sameName(n1->name, n2->name);
        }

        static bool sameType(Type* t1, Type* t2)
        {
            // Iffy, why NULL?
            if (t1 == NULL || t2 == NULL)
                return true;
            if (t1->tag() != t2->tag())
                return false;
            switch (t1->tag())
            {
                case TAG_simpleType: {
                    return sameName(((SimpleType*)t1)->name, ((SimpleType*)t2)->name);
                case TAG_instantiatedType: {
                    InstantiatedType* it1 = (InstantiatedType*)t1;
                    InstantiatedType* it2 = (InstantiatedType*)t2;
                    return sameName(it1->basename, it2->basename) && sameType(it1->tparam, it2->tparam);
                }
                default:
                    AvmAssert(!"Should not happen");
                    return false;
                }
            }
        }
        
        Binding* Parser::findBinding(NameComponent* ns, Str* name, BindingKind kind, BindingRib* rib)
        {
            if (rib == NULL)
                rib = topRib;
            for ( Seq<Binding*>* bindings = rib->body.bindings.get() ; bindings != NULL ; bindings = bindings->tl ) {
                if (sameName(bindings->hd->ns, ns) && bindings->hd->name == name) {
                    if (bindings->hd->kind != kind)
                        compiler->syntaxError(0, SYNTAXERR_REDEFINITION);
                    return bindings->hd;
                }
            }
            return NULL;
        }
        
        void Parser::addClass(ClassDefn* cls)
        {
            Binding* b = findBinding(cls->ns, cls->name, TAG_classBinding);
            if (!b)
                topRib->body.bindings.addAtEnd(ALLOC(Binding, (cls->ns, cls->name, NULL, TAG_classBinding)));
            else
                compiler->syntaxError(0, SYNTAXERR_REDEFINITION_TYPE);
            classes.addAtEnd(cls);
        }
        
        void Parser::addInterface(InterfaceDefn* iface)
        {
            Binding* b = findBinding(iface->ns, iface->name, TAG_interfaceBinding);
            if (!b)
                topRib->body.bindings.addAtEnd(ALLOC(Binding, (iface->ns, iface->name, NULL, TAG_interfaceBinding)));
            else
                compiler->syntaxError(0, SYNTAXERR_REDEFINITION_TYPE);
            classes.addAtEnd(iface);
        }
        
        void Parser::addFunctionBinding(NameComponent* ns, FunctionDefn* fn)
        {
            addVarBinding(ns, fn->name, NULL);
            topRib->body.functionDefinitions.addAtEnd(fn);
        }

        void Parser::addVarBinding(NameComponent* ns, Str* name, Type* type_name, BindingRib* rib)
        {
            if (rib == NULL)
                rib = topRib;
            Binding* b = findBinding(ns, name, TAG_varBinding, rib);
            if (!b)
                rib->body.bindings.addAtEnd(ALLOC(Binding, (ns, name, type_name, TAG_varBinding)));
            else if (!sameType(b->type_name, type_name))
                compiler->syntaxError(0, SYNTAXERR_REDEFINITION_TYPE);
        }
        
        void Parser::addConstBinding(NameComponent* ns, Str* name, Type* type_name, BindingRib* rib)
        {
            if (rib == NULL)
                rib = topRib;
            if (findBinding(ns, name, TAG_constBinding, rib))
                compiler->syntaxError(0, SYNTAXERR_REDUNDANT_CONST);
            rib->body.bindings.addAtEnd(ALLOC(Binding, (ns, name, type_name, TAG_constBinding)));
        }
        
        void Parser::addMethodBinding(NameComponent* ns, FunctionDefn* fn, BindingRib* rib)
        {
            if (findBinding(ns, fn->name, TAG_methodBinding, rib))
                compiler->syntaxError(0, SYNTAXERR_REDUNDANT_METHOD);
            rib->body.bindings.addAtEnd(ALLOC(Binding, (ns, fn->name, NULL, TAG_methodBinding)));
            rib->body.functionDefinitions.addAtEnd(fn);
        }
        
        void Parser::addNamespaceBinding(NameComponent* ns, Str* name, Expr* expr)
        {
            if (findBinding(ns, name, TAG_namespaceBinding))
                compiler->syntaxError(0, SYNTAXERR_REDUNDANT_NAMESPACE);
            topRib->body.bindings.addAtEnd(ALLOC(Binding, (ns, name, NULL, TAG_namespaceBinding)));  // FIXME: type for 'Namespace'
            topRib->body.namespaces.addAtEnd(ALLOC(NamespaceDefn, (ns, name, expr)));
        }

        void Parser::addQualifiedImport(Seq<Str*>* name)
        {
            (void)name;
            // Nothing we care about at this time
        }
        
        void Parser::addUnqualifiedImport(Seq<Str*>* name)
        {
            (void)name;
            // Nothing we care about at this time
        }
        
        void Parser::addOpenNamespace(Namespace* ns)
        {
            topRib->body.openNamespaces.addAtEnd(ns);
        }

        void Parser::setUsesFinally()
        {
            // FIXME: Far from ideal
            if (topRib->tag == RIB_Instance && topRib->next != NULL && topRib->next->tag == RIB_Class)
                topRib->next->body.uses_finally = true;
            else
                topRib->body.uses_finally = true;
        }
        
        void Parser::setUsesCatch()
        {
            // FIXME: Far from ideal
            if (topRib->tag == RIB_Instance && topRib->next != NULL && topRib->next->tag == RIB_Class)
                topRib->next->body.uses_catch = true;
            else
                topRib->body.uses_catch = true;
        }

        void Parser::setUsesGoto()
        {
            // FIXME: Far from ideal
            if (topRib->tag == RIB_Instance && topRib->next != NULL && topRib->next->tag == RIB_Class)
                topRib->next->body.uses_goto = true;
            else
                topRib->body.uses_goto = true;
        }

        void Parser::setUsesSuper()
        {
            topRib->body.uses_super = true;
        }

        void Parser::functionDefinition(bool config, Qualifier* qual, bool getters_and_setters, bool require_body)
        {
            (void)config;
            uint32_t pos = position();
            FunctionDefn* fn = functionGuts(qual, true, getters_and_setters, require_body);
            // FIXME: if an interface method it's always public, plus functionGuts() will have computed a namespace
            // in signature.ns and we should use that.
            NameComponent* ns = qualToNamespace(qual);
            checkNoShadowingOfConfigNamespaces(pos, fn->name);
            if (topRib->tag == RIB_Instance) {
                // Class or interface.  If interface then is_static will never be true because it's
                // filtered during attribute parsing.
                if (qual->is_static) {
                    AvmAssert(topRib->next->tag == RIB_Class);
                    addMethodBinding(ns, fn, topRib->next);
                }
                else
                    addMethodBinding(ns, fn, topRib);
            }
            else
                addFunctionBinding(ns, fn);
        }

        Stmt* Parser::variableDefinition(bool config, Qualifier* qual)
        {
            // FIXME: do not ignore the config value
            // FIXME: discards qualifiers on variable definitions
            // FIXME: check that the name we're defining is not shadowing a config NS
            (void)config;
            Stmt* stmt = varStatement(qualToNamespace(qual), hd() == T_Const, qual->is_static > 0);
            semicolon();
            return stmt;
        }

        FunctionDefn* Parser::functionGuts(Qualifier* qual, bool require_name, bool getters_and_setters, bool require_body)
        {
            if (qual->is_native)
                compiler->syntaxError(position(), SYNTAXERR_NATIVE_NOT_SUPPORTED);

            eat(T_Function);
            uint32_t pos = position();
            SignatureInfo signature(allocator);

            if (require_name || hd() == T_Identifier)
                signature.name = identifier();
            if (signature.name != NULL && hd() == T_Identifier && getters_and_setters) {
                signature.isGetter = (signature.name == compiler->SYM_get);
                signature.isSetter = (signature.name == compiler->SYM_set);
                signature.name = identifier();
            }
            if (signature.name != NULL)
                signature.ns = qualToNamespace(qual);

            pushBindingRib(RIB_Function);
            eat(T_LeftParen);
            if (hd() != T_RightParen) {
                for (;;)
                {
                    if (hd() == T_TripleDot) {
                        Str* rest_name = NULL;
                        Type* rest_type_name = NULL;
                        eat(T_TripleDot);
                        rest_name = identifier();
                        if (match(T_Colon))
                            rest_type_name = typeExpression();
                        addVarBinding(defaultNamespace(), rest_name, rest_type_name);
                        signature.rest_param = ALLOC(FunctionParam, (rest_name, rest_type_name, NULL));
                        break;
                    }
                    ++signature.numparams;
                    Str* param_name = identifier();
                    Type* param_type_name = NULL;
                    Expr* param_default_value = NULL;
                    if (match(T_Colon))
                        param_type_name = typeExpression();
                    if (match(T_Assign)) {
                        signature.optional_arguments = true;
                        param_default_value = assignmentExpression(0);
                    }
                    else if (signature.optional_arguments)
                        compiler->syntaxError(pos, SYNTAXERR_DEFAULT_VALUE_REQD);
                    addVarBinding(defaultNamespace(), param_name, param_type_name);
                    signature.params.addAtEnd(ALLOC(FunctionParam, (param_name, param_type_name, param_default_value)));
                    if (hd() == T_RightParen)
                        break;
                    eat(T_Comma);
                }
            }
            eat(T_RightParen);
            topRib->signature = &signature;
            if (match(T_Colon)) {
                if (match(T_Void))
                    signature.isVoid = true;
                else
                    signature.return_type_name = typeExpression();
            }
            if (require_body)
            {
                eat(T_LeftBrace);
                directives(SFLAG_Function);
                eat(T_RightBrace);
                // Rest takes precedence over 'arguments'
                if (topRib->body.uses_arguments) {
                    if (signature.rest_param == NULL)
                        addVarBinding(defaultNamespace(), compiler->SYM_arguments, NULL);
                    else
                        topRib->body.uses_arguments = false;
                }
            }
            else {
                topRib->body.empty_body = true;
                semicolon();
            }

            BindingRib* theRib = topRib;
            popBindingRib();
            return ALLOC(FunctionDefn, (signature, theRib->body));
        }

        BuiltinNamespace* Parser::internalNamespace()
        {
            return internalNS;
        }

        BuiltinNamespace* Parser::defaultNamespace()
        {
            if (public_by_default)
                return publicNS;
            return internalNS;
        }
        
        NameComponent* Parser::qualToNamespace(Qualifier* qual)
        {
            switch (qual->tag) {
                case QUAL_none:
                    return defaultNamespace();
                case QUAL_internal:
                    return internalNS;
                case QUAL_private:
                    return privateNS;
                case QUAL_protected:
                    return protectedNS;
                case QUAL_public:
                    return publicNS;
                case QUAL_name:
                    compiler->internalError(position(), "Defintions in user-defined namespaces not implemented");
                    /*NOTREACHED*/
                default:
                    return NULL;
            }
        }

        // Token queue abstractions
        // BEGIN
        
        // Token queue
        //
        // T0 is the current token, L0 its line number
        // T1 is the next token, L1 its line number
        // LP is the line number of the previous token
        //
        // The line number of a token is the 1-based line number of
        // the last character of the token.
        //
        // Invariants:
        //   T0 is LAST only before start() and when the stream is exhausted.
        //   T1 is LAST whenever it's invalid
        //   L1 is invalid iff T1 is invalid
        //   LP, L0, and L1 are 0 if there is "no information"
        //   The scanner never returns Eol
        //
        // Every token passes through this interface so we try to save time
        // when possible.
        //
        // Current optimizations:
        //   * avoid function calls
        //     - Lexer::lex() is shallow
        //     - Token::tokenKind and Token::tokenText have been in-lined
        //   * avoid allocations
        //     - Communicate with lexer via multiple variables rather than
        //       using multiple return values
        
        void Parser::start()
        {
            T0 = lexer->lex(&L0, &V0);
        }
        
        Token Parser::divideOperator()
        {
            AvmAssert( T0 == T_BreakSlash && T1 == T_LAST );
            T0 = lexer->divideOperator(&L0);
            return hd();
        }
        
        Token Parser::regexp()
        {
            AvmAssert( T0 == T_BreakSlash && T1 == T_LAST );
            T0 = lexer->regexp(&L0, &V0);
            return hd();
        }
        
        Token Parser::rightAngle()
        {
            AvmAssert( T0 == T_BreakRightAngle && T1 == T_LAST );
            T0 = lexer->rightAngle(&L0);
            return hd();
        }
        
        Token Parser::leftAngle()
        {
            AvmAssert( T0 == T_BreakLeftAngle && T1 == T_LAST );
            T0 = lexer->leftAngle(&L0);
            return hd();
        }

        Token Parser::leftShiftOrRelationalOperator()
        {
            AvmAssert( T0 == T_BreakLeftAngle && T1 == T_LAST );
            T0 = lexer->leftShiftOrRelationalOperator(&L0);
            return hd();
        }
        
        Token Parser::rightShiftOrRelationalOperator()
        {
            AvmAssert( T0 == T_BreakRightAngle && T1 == T_LAST );
            T0 = lexer->rightShiftOrRelationalOperator(&L0);
            return hd();
        }
        
        Token Parser::hd2()
        {
            if (T1 == T_LAST)
                T1 = lexer->lex(&L1, &V1);
            return T1;
        }
        
        void Parser::next()
        {
            LP = L0;
            T0 = T1;
            L0 = L1;
            V0 = V1;
            T1 = T_LAST;
            if (T0 == T_LAST)
                T0 = lexer->lex(&L0, &V0);
        }
        
        // Token queue abstractions
        // END
        
        // If the current token is not tc then throw an error.
        // Otherwise consume the token.
        
        void Parser::eat(Token tc)
        {
            Token tk = hd();
            if (tk != tc) {
#ifdef DEBUG
                compiler->syntaxError(position(), SYNTAXERR_WRONG_TOKEN, tc, tk);
#else
                compiler->syntaxError(position(), SYNTAXERR_WRONG_TOKEN);   // FIXME: not adequate
#endif
            }
            next();
        }
        
        bool Parser::match (Token tc)
        {
            Token tk = hd();
            if (tk != tc)
                return false;
            next();
            return true;
        }
        
        Str* Parser::identifier()
        {
            if (hd() != T_Identifier)
                compiler->syntaxError(position(), SYNTAXERR_EXPECTED_IDENT);
            Str* name = V0.s;
            next();
            return name;
        }
        
        Str* Parser::doubleToStr(double d)
        {
            char buf[380];
            compiler->context->doubleToString(d, buf, sizeof(buf));
            return compiler->intern(buf);
        }
        
        // FIXME: this duplicates Lexer::parseDouble()
        double Parser::strToDouble(Str* s)
        {
            double n;
            StringBuilder sb(compiler);
            sb.append(s);
            DEBUG_ONLY(bool flag =) compiler->context->stringToDouble(sb.chardata(), &n);
            AvmAssert(flag);
            return n;
        }

        CodeBlock::~CodeBlock()
        {
        }
    }
}

#endif // VMCFG_EVAL
