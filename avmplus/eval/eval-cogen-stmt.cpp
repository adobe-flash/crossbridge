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

        static bool matchLabel(Ctx* ctx, Str* label, void** result=NULL)
        {
            if (label == NULL)
                return true;
            switch (ctx->tag) {
                case CTX_Continue: {
                    Seq<Str*>* labels = ((ContinueCtx*)ctx)->label_names;
                    while (labels != NULL) {
                        if (label == labels->hd)
                            return true;
                        labels = labels->tl;
                    }
                    return false;
                }
                case CTX_Goto: {
                    Seq<GotoLabel*>* labels = ((GotoCtx*)ctx)->label_names;
                    while (labels != NULL) {
                        if (label == labels->hd->label_name) {
                            if (result != NULL)
                                *result = labels->hd;
                            return true;
                        }
                        labels = labels->tl;
                    }
                    return false;
                }
                case CTX_Break:
                    return label == ((BreakCtx*)ctx)->label_name;
                default:
                    return false;
            }
        }

        void Cogen::unstructuredControlFlow(Ctx* ctx, bool (hit)(Ctx*,void*), void* package, bool jump, SyntaxError msg, uint32_t pos)
        {
            while (ctx != NULL) {
                if (hit(ctx, package)) {
                    if (jump)
                        I_jump(((ControlFlowCtx*)ctx)->label);
                    return;
                }
                
                if(ctx->tag == CTX_With || ctx->tag == CTX_Catch) {
                    ScopeCtx* ctx1 = (ScopeCtx*)ctx;
                    I_popscope();
                    I_kill(ctx1->scope_reg);
                }
                
                if (ctx->tag == CTX_Finally) {
                    FinallyCtx* ctx2 = (FinallyCtx*)ctx;
                    Label* Lreturn = newLabel();
                    uint32_t myReturnLabel = ctx2->addReturnLabel(Lreturn);
                    I_pushuint(emitUInt(myReturnLabel));
                    I_coerce_a();
                    I_setlocal(ctx2->returnreg);
                    I_jump(ctx2->Lfinally);
                    I_label(Lreturn);
                }
                ctx = ctx->next;
            }
            compiler->syntaxError(pos, msg);
        }

        static void restoreScopes(Cogen* cogen, Ctx* ctx)
        {
            if (ctx == NULL)
                return;
            if (ctx->tag != CTX_Function && ctx->tag != CTX_ClassMethod)
                restoreScopes(cogen, ctx->next);
            if (ctx->mustPushThis()) {
                cogen->I_getlocal(0);
                cogen->I_pushscope();
            }
            if (ctx->mustPushScopeReg()) {
                ScopeCtx* ctx1 = (ScopeCtx*)ctx;
                cogen->I_getlocal(ctx1->scope_reg);
                if (ctx1->tag == CTX_With)
                    cogen->I_pushwith();
                else
                    cogen->I_pushscope();
            }
        }
        
        // Syntax analysis and code generation for goto are intertwined.  That may or may not be
        // a great idea, but here's how it works:
        //
        // When we enter a Program body or Function body we create an initial set of legal goto targets,
        // this set is empty and provides the base case for the analysis that occurs within the body.
        // There will be a list of these sets chained through the goto contexts on the stack.
        //
        // When we get to a statement list (Block, Cases, Group, Program body, Function body) we walk
        // the statement list and look for labeled statements in the list (never within statements in
        // the list).  The labels on these statements will comprise the set of legal "top-level"
        // (non-nesting) labels for that statement list.  We create a new goto context for the set,
        // and we enter the labels into that context.  Each entry in the set has the label name,
        // a Label pointer, and a flag saying whether the entry is ambiguous.  It is ambiguous if the
        // label name is also found in one of the sets we inherit from nesting contexts (up to a program
        // or function body).  The labeled statements we find are updated: we store the Label pointer
        // in each of their "address" members.  This pointer will serve as a flag when code is generated
        // for the labeled statement (next paragraph).
        //
        // When we process a labeled statement, one of two things happens.  If the "address" member
        // is non-null then this is a top-level labeled statement and we do not create a new goto context,
        // as one already exists.  Otherwise, we create a new goto context containing just the label for
        // this labeled statement; the entry may again be ambiguous.  We create an "address" value too.
        // In either case, we emit the label stored in the address node.
        //
        // When we process a goto statement, we search the goto stack for the context node that contains
        // the label.  If the label is not found, we signal an error.  If the label is found but marked
        // ambiguous, we signal an error.  Otherwise we emit a nonlocal jump to the node (without control
        // transfer) and then jump to the target address, which may not be emitted yet (but backpatching
        // will take care of that).
        //
        // Goto contexts are chained through the next_goto field.  If a goto context is not found on
        // the stack when we want to process a statement list then the function or program body does
        // not use "goto" and the analysis can be skipped entirely.  There's a small cost to pay
        // to discover that, but in general the overhead of "goto" on programs not using it is tiny,
        // as context lists are normally quite short.
        //
        // (The cost of checking for duplicates is quadratic in the number of labels in a function, roughly:
        // hardly ideal.  A hashtable of all known labels in the function with occurence counts could be
        // created in linear time in the parser, and if the occurence count on a label is 1 we would never
        // need to search.  Such a table could probably even be created on-line in the code generator.)
        
        // Search for the nearest "goto" context starting with c.
 
        static GotoCtx* lookupGoto(Ctx* c)
        {
            while ( c->tag != CTX_Program && c->tag != CTX_Function && c->tag != CTX_ClassMethod && c->tag != CTX_Goto )
                c = c->next;
            if (c->tag == CTX_Goto)
                return (GotoCtx*)c;
            return NULL;
        }
 
        static bool searchForDuplicateLabel(Str* label, Seq<GotoLabel*>* labels)
        {
            while (labels != NULL && labels->hd->label_name != label)
                labels = labels->tl;
            return labels != NULL;
        }

        static bool searchForDuplicateLabel(Str* label, Seq<GotoLabel*>* inner_labels, GotoCtx* outer_goto)
        {
            if (inner_labels && searchForDuplicateLabel(label, inner_labels))
                return true;

            while (outer_goto != NULL) {
                if (searchForDuplicateLabel(label, outer_goto->label_names))
                    return true;
                outer_goto = outer_goto->next_goto;
            }

            return false;
        }

        // If there are any labeled statements in the statement list then populate goto_ctx and return it,
        // otherwise return outer_ctx.

        static Ctx* maybePopulateGotoCtx(Cogen* cogen, Seq<Stmt*>* stmts, GotoCtx* goto_ctx, Ctx* outer_ctx)
        {
            GotoCtx* outer_goto = lookupGoto(outer_ctx);
            if (outer_goto == NULL)
                return outer_ctx;

            Allocator* allocator = cogen->allocator;
            Seq<GotoLabel*>* labels = NULL;
            for ( Seq<Stmt*>* ss = stmts ; ss != NULL ; ss = ss->tl ) {
                Stmt* s = ss->hd;
                while (s->isLabeledStmt()) {
                    LabeledStmt* ls = (LabeledStmt*)s;
                    bool ambiguous = searchForDuplicateLabel(ls->label, labels, outer_goto);
                    Label* address = cogen->newLabel();
                    labels = ALLOC(Seq<GotoLabel*>, (ALLOC(GotoLabel, (ls->label, address, ambiguous)), labels));
                    ls->address = address;
                    s = ls->stmt;
                }
            }

            goto_ctx->label_names = labels;
            goto_ctx->next_goto = outer_goto;
            return goto_ctx;
        }

        // If there are any labeled statements in the statement lists within the case clauses then populate
        // goto_ctx and return it, otherwise return outer_ctx.
        
        static Ctx* maybePopulateGotoCtx(Cogen* cogen, Seq<CaseClause*>* cases, GotoCtx* goto_ctx, Ctx* outer_ctx)
        {
            GotoCtx* outer_goto = lookupGoto(outer_ctx);
            if (outer_goto == NULL)
                return outer_ctx;

            Allocator* allocator = cogen->allocator;
            Seq<GotoLabel*>* labels = NULL;
            for ( Seq<CaseClause*>* cs = cases ; cs != NULL ; cs = cs->tl ) {
                for ( Seq<Stmt*>* ss = cs->hd->stmts ; ss != NULL ; ss = ss->tl ) {
                    Stmt* s = ss->hd;
                    while (s->isLabeledStmt()) {
                        LabeledStmt* ls = (LabeledStmt*)s;
                        bool ambiguous = searchForDuplicateLabel(ls->label, labels, outer_goto);
                        Label* address = cogen->newLabel();
                        labels = ALLOC(Seq<GotoLabel*>, (ALLOC(GotoLabel, (ls->label, address, ambiguous)), labels));
                        ls->address = address;
                        s = ls->stmt;
                    }
                }
            }

            goto_ctx->label_names = labels;
            goto_ctx->next_goto = outer_goto;
            return goto_ctx;
        }

        static Ctx* maybePopulateGotoCtx(Cogen* cogen, LabeledStmt* stmt, GotoCtx* goto_ctx, Ctx* outer_ctx)
        {
            GotoCtx* outer_goto = lookupGoto(outer_ctx);
            if (outer_goto == NULL)
                return outer_ctx;
            
            bool ambiguous = searchForDuplicateLabel(stmt->label, NULL, outer_goto);
            stmt->address = cogen->newLabel();
            
            Allocator* allocator = cogen->allocator;
            goto_ctx->label_names = ALLOC(Seq<GotoLabel*>, (ALLOC(GotoLabel, (stmt->label, stmt->address, ambiguous))));
            goto_ctx->next_goto = outer_goto;
            return goto_ctx;
        }

        // Process labeled statements and generate code for the statements.  This must not be used
        // to generate code for case bodies in switch, but is fine for blocks and function and program
        // bodies.

        static void cogenStatements(Cogen* cogen, Seq<Stmt*>* stmts, Ctx* ctx)
        {
            GotoCtx ctx2(ctx);
            Ctx* ctx3 = maybePopulateGotoCtx(cogen, stmts, &ctx2, ctx);

            for ( ; stmts != NULL ; stmts = stmts->tl )
                stmts->hd->cogen(cogen, ctx3);
        }

        // Statement code generators
 
        void Program::cogenBody(Cogen* cogen, Ctx* ctx, uint32_t activation_reg)
        {
            (void)activation_reg;
            AvmAssert(activation_reg == 0);     // Because programs don't have activation records
            AvmAssert(ctx->tag == CTX_Program);
            uint32_t capture_reg = ((ProgramCtx*)ctx)->capture_reg;
            cogen->I_pushundefined();
            cogen->I_coerce_a();
            cogen->I_setlocal(capture_reg);
            GotoCtx ctx2(ctx);  // An empty set of labels
            cogenStatements(cogen, stmts, uses_goto ? &ctx2 : ctx);
            cogen->I_getlocal(capture_reg);
            cogen->I_returnvalue();
        }

        void FunctionDefn::cogenBody(Cogen* cogen, Ctx* ctx, uint32_t activation_reg)
        {
            // Inherit open namespaces
            VarScopeCtx* vs = ctx->findVarScope();
            Seq<Namespace*>* openNamespaces = vs->openNamespaces;
            uint32_t nsset = vs->nsset;
            if (this->openNamespaces != NULL) {
                Allocator* allocator = cogen->allocator;
                Seq<Namespace*>* ons = this->openNamespaces;
                while (ons != NULL) {
                    openNamespaces = ALLOC(Seq<Namespace*>, (ons->hd, openNamespaces));
                    ons = ons->tl;
                }
                nsset = cogen->buildNssetWithPublic(openNamespaces);
            }
            FunctionCtx ctx0(cogen->allocator, nsset, openNamespaces, ctx);
            ActivationCtx ctx1(activation_reg, &ctx0);
            Ctx* ctx2 = activation_reg == 0 ? (Ctx*)&ctx0 : (Ctx*)&ctx1;
            GotoCtx ctx3(ctx2); // An empty set of labels
            Ctx* ctx4 = uses_goto ? &ctx3 : ctx2;
            cogenStatements(cogen, stmts, ctx4);
            cogen->I_returnvoid();
        }
        
        void EmptyStmt::cogen(Cogen*, Ctx*)
        {
            // nothing
        }
        
        void BlockStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogenStatements(cogen, stmts, ctx);
        }
        
        void LabeledStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* L0 = cogen->newLabel();
            BreakCtx ctx1(L0, ctx, label);
            GotoCtx gctx(&ctx1);
            Ctx* ctx2 = &ctx1;
            if (!address)
                ctx2 = maybePopulateGotoCtx(cogen, this, &gctx, &ctx1);   // may update address
            if (address)
                cogen->I_label(address);
            stmt->cogen(cogen, ctx2);
            cogen->I_label(L0);
        }

        void ExprStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            Ctx* c;
            for ( c = ctx ; c->tag != CTX_Program && c->tag != CTX_Function && c->tag != CTX_ClassMethod ; c = c->next )
                ;
            if (c->tag == CTX_Program) {
                cogen->I_coerce_a();
                cogen->I_setlocal(((ProgramCtx*)c)->capture_reg);
            }
            else
                cogen->I_pop();
        }

        void IfStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* L1 = cogen->newLabel();
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_iffalse(L1);
            consequent->cogen(cogen, ctx);
            if (alternate != NULL) {
                Label* L2 = cogen->newLabel();
                cogen->I_jump(L2);
                cogen->I_label(L1);
                alternate->cogen(cogen, ctx);
                cogen->I_label(L2);
            }
            else
                cogen->I_label(L1);
        }

        void WhileStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* Lbreak = cogen->newLabel();
            Label* Lcont  = cogen->newLabel();
            cogen->I_label(Lcont);
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_iffalse(Lbreak);
            BreakCtx ctx1(Lbreak, ctx);
            ContinueCtx ctx2(Lcont, labels, &ctx1);
            body->cogen(cogen, &ctx2);
            cogen->I_jump(Lcont);
            cogen->I_label(Lbreak);
        }

        void DoWhileStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* Lbreak = cogen->newLabel();
            Label* Lcont  = cogen->newLabel();
            Label* Ltop   = cogen->newLabel();
            cogen->I_label(Ltop);
            BreakCtx ctx1(Lbreak, ctx);
            ContinueCtx ctx2(Lcont, labels, &ctx1);
            body->cogen(cogen, &ctx2);
            cogen->I_label(Lcont);
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_iftrue(Ltop);
            cogen->I_label(Lbreak);
        }

        void ForStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Label* Lbreak = cogen->newLabel();
            Label* Lcont = cogen->newLabel();
            Label* Ltop = cogen->newLabel();
            if (init != NULL) {
                init->cogen(cogen, ctx);
                cogen->I_pop();
            }
            cogen->I_label(Ltop);
            if (test != NULL) {
                test->cogen(cogen, ctx);
                cogen->I_iffalse(Lbreak);
            }
            BreakCtx ctx1(Lbreak, ctx);
            ContinueCtx ctx2(Lcont, labels, &ctx1);
            body->cogen(cogen, &ctx2);
            cogen->I_label(Lcont);
            if (update != NULL) {
                update->cogen(cogen, ctx);
                cogen->I_pop();
            }
            cogen->I_jump(Ltop);
            cogen->I_label(Lbreak);
        }
        
        // Instruction set bug workaround.
        //
        // In hasnext2, the second argument reg must be known to be 'int', but this
        // means that if we want to kill it below the loop then all paths past that
        // kill (from statements enclosing this statement) must have a kill for the
        // register too.  This is just needless complexity.  Yet if there is not a
        // kill at the end of the loop, any enclosing loop will get into trouble
        // with the verifier because the types along the backward edge to the top of
        // the loop will not match the initial values into that loop.
        //
        // To work around this we use two index registers here, and one has a very short
        // lifetime, just across the hasnext2 instruction, to keep the verifier happy.

        void ForInStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            Allocator* allocator = cogen->allocator;
            Label* Lbreak = cogen->newLabel();
            Label* Lcont = cogen->newLabel();
            Label* Ltop = cogen->newLabel();
            
            uint32_t T_obj = cogen->getTemp();
            uint32_t T_idx = cogen->getTemp();
            uint32_t T_idx2 = cogen->getTemp();
            uint32_t T_val = cogen->getTemp();
            
            if (init != lhs) {
                init->cogen(cogen, ctx);
                cogen->I_pop();
            }
            obj->cogen(cogen, ctx);

            cogen->I_coerce_a();
            cogen->I_setlocal(T_obj);
            cogen->I_pushbyte(0);
            cogen->I_coerce_a();
            cogen->I_setlocal(T_idx);
            
            cogen->I_label(Ltop);
            
            cogen->I_getlocal(T_idx);
            cogen->I_coerce_i();
            cogen->I_setlocal(T_idx2);
            cogen->I_hasnext2(T_obj, T_idx2);
            cogen->I_getlocal(T_idx2);
            cogen->I_coerce_a();
            cogen->I_setlocal(T_idx);
            cogen->I_kill(T_idx2);
            cogen->I_iffalse(Lbreak);
            cogen->I_getlocal(T_obj);
            cogen->I_getlocal(T_idx);
            cogen->I_coerce_i();

            if (is_each)
                cogen->I_nextvalue();
            else
                cogen->I_nextname();
            cogen->I_setlocal(T_val);
            (ALLOC(AssignExpr, (OPR_assign, lhs, ALLOC(RefLocalExpr, (T_val)))))->cogen(cogen, ctx);
            cogen->I_pop();
            
            BreakCtx ctx1(Lbreak, ctx);
            ContinueCtx ctx2(Lcont, labels, &ctx1);
            body->cogen(cogen, &ctx2);
            
            cogen->I_label(Lcont);
            cogen->I_jump(Ltop);
            
            cogen->I_label(Lbreak);
            cogen->I_kill(T_val);
            cogen->I_kill(T_idx);
            cogen->I_kill(T_obj);
        }
        
        static bool hitBreak(Ctx* ctx, void* package)
        {
            return ctx->tag == CTX_Break && matchLabel(ctx, (Str*)package);
        }
        
        void BreakStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogen->unstructuredControlFlow(ctx,
                                           hitBreak,
                                           (void*)label,
                                           true,
                                           (label == NULL ? SYNTAXERR_ILLEGAL_BREAK : SYNTAXERR_BREAK_LABEL_UNDEF),
                                           pos);
        }

        static bool hitContinue(Ctx* ctx, void* package)
        {
            return ctx->tag == CTX_Continue && matchLabel(ctx, (Str*)package);
        }
            
        void ContinueStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogen->unstructuredControlFlow(ctx,
                                           hitContinue,
                                           (void*)label,
                                           true,
                                           (label == NULL ? SYNTAXERR_ILLEGAL_CONTINUE : SYNTAXERR_CONTINUE_LABEL_UNDEF),
                                           pos);
        }

        struct GotoPackage
        {
            Str* label;
            GotoLabel* infoloc;
        };

        static bool hitGoto(Ctx* ctx, void* _package)
        {
            GotoPackage* package = (GotoPackage*)_package;
            return ctx->tag == CTX_Goto && matchLabel(ctx, package->label, (void**)&(package->infoloc));
        }

        void GotoStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            GotoPackage package;
            package.label = label;
            package.infoloc = NULL;
            cogen->unstructuredControlFlow(ctx,
                                           hitGoto,
                                           (void*)&package,
                                           false,
                                           SYNTAXERR_GOTO_LABEL_UNDEFINED,
                                           pos);
            if (package.infoloc->ambiguous)
                cogen->compiler->syntaxError(pos, SYNTAXERR_GOTO_LABEL_AMBIGUOUS);
            cogen->I_jump(package.infoloc->address);
        }

        void ThrowStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_throw();
        }

        static bool hitFunction(Ctx* ctx, void*)
        {
            return ctx->tag == CTX_Function;
        }
        
        void ReturnStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            uint32_t tmp = 0;
            
            if (expr != NULL) {
                cogen->I_debugline(pos);
                expr->cogen(cogen, ctx);
                tmp = cogen->getTemp();
                cogen->I_coerce_a();
                cogen->I_setlocal(tmp);
            }
            
            cogen->unstructuredControlFlow(ctx,
                                           hitFunction,
                                           NULL,
                                           false,
                                           SYNTAXERR_RETURN_OUTSIDE_FN);
            
            if (expr == NULL)
                cogen->I_returnvoid();
            else {
                cogen->I_getlocal(tmp);
                cogen->I_kill(tmp);
                cogen->I_returnvalue();
            }
        }
        
        void WithStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            uint32_t scopereg = cogen->getTemp();
            
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_dup();
            cogen->I_setlocal(scopereg);
            cogen->I_pushwith();
            WithCtx ctx1(scopereg, ctx);
            body->cogen(cogen, &ctx1);
            cogen->I_popscope();
            cogen->I_kill(scopereg);
        }

        // OPTIMIZEME: we can do better here for switches that are sparse overall (cover a large range)
        // but which have significant dense segments.  Consider a scanner that handles unicode: it may
        // have a lot of cases for values in the ASCII range and then a few cases to handle unicode
        // outliers, like unicode space characters.  It will fail the 'fast' test but would benefit
        // from being rewritten as a dense switch whose default case switches further on the outlying
        // values.

        void SwitchStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            int32_t low, high;
            if (analyze(&low, &high))
                cogenFast(cogen, ctx, low, high);
            else
                cogenSlow(cogen, ctx);
        }

        // Trigger lookupswitch if
        //  - all cases are integer constants
        //  - there are at least 4 cases
        //  - all cases in U30 range when biased by low
        //  - at least 1/3 of the values in the switch range are present

        bool SwitchStmt::analyze(int32_t* low, int32_t* high)
        {
            uint32_t count = 0;
            *low = 0x7FFFFFFF;
            *high = (-0x7FFFFFFF - 1);
            
            for ( Seq<CaseClause*>* cases = this->cases ; cases != NULL ; cases = cases->tl ) {
                Expr* e = cases->hd->expr;
                if (e == NULL)
                    ;
                else if (e->tag() == TAG_literalInt) {
                    int32_t v = ((LiteralInt*)e)->value;
                    *low = min(*low, v);
                    *high = max(*high, v);
                    count++;
                }
                else
                    return false;
            }
            const uint32_t ncases = uint32_t(*high - *low + 1);
            return count >= 4 && ncases < (1<<30) && count * 3 >= ncases;
        }
        
        void SwitchStmt::cogenFast(Cogen* cogen, Ctx* ctx, int32_t low, int32_t high)
        {
            AvmAssert( high > low );
            AvmAssert( high - low + 1 < (1 << 30) );

            // Lcase[i] has the label for value (low+i)
            // Ldefault is the default case (whether or not there is a default in the switch)

            Compiler* compiler = cogen->compiler;
            uint32_t tmp = cogen->getTemp();
            const uint32_t ncases = high - low + 1;
            Label** Lcase;
#ifdef AVMC_STANDALONE
            Lcase = (Label**)alloca(sizeof(Label*) * ncases);
#else
            MMgc::GC::AllocaAutoPtr _Lcase;
            Lcase = (Label**)avmStackAllocArray(compiler->context->core, _Lcase, ncases, sizeof(Label*));
#endif
            Label* Ldefault = cogen->newLabel();
            Label* Lbreak = cogen->newLabel();
            BreakCtx nctx(Lbreak, ctx);
            
            for ( uint32_t i=0 ; i < ncases ; i++ )
                Lcase[i] = Ldefault;
            
            for ( Seq<CaseClause*>* cases = this->cases ; cases != NULL ; cases = cases->tl ) {
                Expr* e = cases->hd->expr;
                if (e != NULL) {
                    AvmAssert( e->tag() == TAG_literalInt);
                    int32_t v = ((LiteralInt*)e)->value - low;
                    // A value may be duplicated in the switch; generate only one label, and
                    // observe that the label is only emitted for the first (sequential case),
                    // the others are unreachable except by fallthrough.
                    if (Lcase[v] == Ldefault)
                        Lcase[v] = cogen->newLabel();
                }
            }

            expr->cogen(cogen, ctx);                             // switch value
            cogen->I_coerce_a();
            cogen->I_setlocal(tmp);

            // Case clauses are triggered by strict equality, so if the type of the
            // dispatch value is not int then we definitely won't hit any of the
            // clauses.  Also, test for 'int' tests for integer values in the 'int'
            // range that are represented as Number, which is what we want here.
            
            // Test applicability of the dispatch value.  In the absence of static type
            // information this is fairly painful, we must not have observable side
            // effects (eg valueOf conversion) or overflow & truncate.  The rules are:
            //
            //    if (low != 0) then
            //        if (value is not Number) then skip to default
            //    subtract low from value
            //    if (value is not int) then skip to default
            //
            // OPTIMIZEME: avoid type tests if the type is known and if we've verified that
            // the JIT does not perform that optimization.  (It might, but it would have to
            // track the type from the original expression through tmp, and ignore all the
            // coerce_a instructions, so it's unlikely that it's currently performing it.)

            if (low != 0) {
                cogen->I_getlocal(tmp);
                cogen->I_istype(compiler->ID_Number);
                cogen->I_iffalse(Ldefault);

                cogen->I_getlocal(tmp);
                cogen->I_pushint(cogen->emitInt(low));
                cogen->I_subtract();
                cogen->I_coerce_a();
                cogen->I_setlocal(tmp);
            }
            
            cogen->I_getlocal(tmp);
            cogen->I_istype(compiler->ID_int);
            cogen->I_iffalse(Ldefault);

            cogen->I_getlocal(tmp);
            cogen->I_coerce_i();        // not redundant, the representation could have been Number
            cogen->I_lookupswitch(Ldefault, Lcase, ncases);
            
            GotoCtx gctx(&nctx);
            Ctx* actualctx = maybePopulateGotoCtx(cogen, this->cases, &gctx, &nctx);

            for ( Seq<CaseClause*>* cases = this->cases ; cases != NULL ; cases = cases->tl ) {
                CaseClause* c = cases->hd;
                Expr* e = c->expr;

                if (e == NULL) {
                    AvmAssert(Ldefault != NULL);
                    cogen->I_label(Ldefault);
                    Ldefault = NULL;
                }
                else {
                    AvmAssert(e->tag() == TAG_literalInt);
                    int32_t v = ((LiteralInt*)e)->value - low;

                    // There might be duplicate case selector values, but only the first one counts.
                    if (Lcase[v] != NULL) {
                        cogen->I_label(Lcase[v]);
                        Lcase[v] = NULL;
                    }
                }
                
                for ( Seq<Stmt*>* stmts = c->stmts ; stmts != NULL ; stmts = stmts->tl )
                    stmts->hd->cogen(cogen, actualctx);
            }
            
            if (Ldefault != NULL)
                cogen->I_label(Ldefault);
            
            cogen->I_label(Lbreak);
            cogen->I_kill(tmp);
        }
        
        void SwitchStmt::cogenSlow(Cogen* cogen, Ctx* ctx)
        {
            uint32_t tmp = cogen->getTemp();
            
            Label* Ldefault = NULL;
            Label* Lnext = cogen->newLabel();
            Label* Lfall = NULL;
            Label* Lbreak = cogen->newLabel();
            
            cogen->I_debugline(pos);
            expr->cogen(cogen, ctx);
            cogen->I_coerce_a();
            cogen->I_setlocal(tmp);
            cogen->I_jump(Lnext);

            BreakCtx ctx1(Lbreak, ctx);
            
            GotoCtx gctx(&ctx1);
            Ctx* actualctx = maybePopulateGotoCtx(cogen, cases, &gctx, &ctx1);
            
            for ( Seq<CaseClause*>* cases=this->cases ; cases != NULL ; cases = cases->tl ) {
                CaseClause* c = cases->hd;
                
                if (c->expr == NULL) {
                    AvmAssert(Ldefault == NULL);
                    Ldefault = cogen->newLabel();
                    cogen->I_label(Ldefault);               // label default pos
                }
                else {
                    if (Lnext != NULL) {
                        cogen->I_label(Lnext);              // label next pos
                        Lnext = NULL;
                    }
                    cogen->I_debugline(c->pos);
                    c->expr->cogen(cogen, ctx);                  // check for match
                    cogen->I_getlocal(tmp);
                    cogen->I_strictequals();
                    Lnext = cogen->newLabel();
                    cogen->I_iffalse(Lnext);
                }
                
                if (Lfall != NULL) {                    // label fall through pos
                    cogen->I_label(Lfall);
                    Lfall = NULL;
                }
                
                for ( Seq<Stmt*>* stmts = c->stmts ; stmts != NULL ; stmts = stmts->tl )
                    stmts->hd->cogen(cogen, actualctx);
                
                Lfall = cogen->newLabel();
                cogen->I_jump(Lfall);           // fall through
            }
            
            if (Lnext != NULL)
                cogen->I_label(Lnext);
            if (Ldefault != NULL)
                cogen->I_jump(Ldefault);
            if (Lfall != NULL)
                cogen->I_label(Lfall);
            cogen->I_label(Lbreak);
            cogen->I_kill(tmp);
        }
        
        void TryStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            if (finallyblock != NULL)
                cogenWithFinally(cogen, ctx);
            else
                cogenNoFinally(cogen, ctx);
        }
        
        // If there's a finally block then:
        //
        // - there is a generated catch around the try-catch complex with a handler that
        //   handles any exception type
        // - the handler in that block must visit the finally code and then re-throw if
        //   the finally code returns normally
        // - code in the try block or the catch block(s) is compiled with a ctx that
        //   records the fact that there is a finally block, so that exits to the outside of
        //   the try/catch block by means of break/continue (labelled or not) must visit
        //   the finally block (in inside-out order if there are several)
        // - break, continue, and return must look for finally blocks
        //
        // Visiting the finally block may thus be done from various places.  To avoid
        // code bloat it is generated out-of-line.  Visiting is done by setting a register
        // to the "return" address, then jumping to the finally code, which ends with a
        // switch statement that jumps back to all the possible return points.
        //
        // Each finally block gets its own register, it's recorded in the ctx rib.
        //
        // The code for the finally block's "switch" can't be generated until we've seen
        // all the code that can visit it (represented as a list of id/labels in the ctx rib).
        //
        // There is a counter in the ctx, and id's for the switch are generated from it.
        // Its initial value is 0.  lookupswitch can be used.
        
        void TryStmt::cogenWithFinally(Cogen* cogen, Ctx* ctx)
        {
            uint32_t returnreg = cogen->getTemp();  // the local that records the index of the label this finally block should return to
            Label* Lfinally = cogen->newLabel();    // the address of the finally block body
            
            FinallyCtx fctx(cogen->allocator, Lfinally, returnreg, ctx);
            
            // Lreturn is the address in this finally block that visited, outer finally blocks return to
            // after executing to completion.
            Label* Lreturn = cogen->newLabel();
            uint32_t myreturn = fctx.addReturnLabel(Lreturn);

            // Lend is the address to branch to to get out of the finally block after entering the
            // finally block normally and executing it without throwing any exceptions.
            Label* Lend = cogen->newLabel();
            uint32_t myend = fctx.addReturnLabel(Lend);
            
            uint32_t code_start = cogen->getCodeLength();
            cogenNoFinally(cogen, &fctx);
            uint32_t code_end = cogen->getCodeLength();

            // Fallthrough from try-catch: visit the finally block.  This code must not be in the
            // scope of the generated exception handler.
            
            cogen->I_pushuint(cogen->emitUInt(myend));  // return to Lend
            cogen->I_coerce_a();
            cogen->I_setlocal(returnreg);
            cogen->I_jump(Lfinally);                    // control continues at Lend below
            
            // Generated catch block to handle throws out of try-catch:
            // capture the exception, visit the finally block with return
            // to Lreturn, then re-throw the exception at Lreturn.
            //
            // Use a lightweight exception handler; always store the value
            // in a register.
            
            uint32_t savedExn = cogen->getTemp();
            /*uint32_t catch_idx =*/ cogen->emitException(code_start, code_end, cogen->getCodeLength(), 0, 0);

            cogen->startCatch();                // push 1 item
            cogen->I_setlocal(savedExn);        // pop and save it
            
            restoreScopes(cogen, ctx);          // finally block needs correct scopes
            
            cogen->I_pushuint(cogen->emitUInt(myreturn));
            cogen->I_coerce_a();
            cogen->I_setlocal(returnreg);
            cogen->I_jump(Lfinally);            // control continues at Lreturn directly below
            cogen->I_label(Lreturn);
            cogen->I_getlocal(savedExn);
            cogen->I_kill(savedExn);
            cogen->I_throw();
            
            // Finally block
            
            cogen->I_label(Lfinally);
            cogenStatements(cogen, this->finallyblock, ctx);

            // The return-from-subroutine code at the end of the finally block
            // From the above it may seem that there are at most two labels in the list,
            // but this is not so: unstructuredControlFlow can place more labels into it.

            Seq<Label*>* labels;
            uint32_t i;
            for ( i=0, labels = fctx.returnLabels.get() ; labels != NULL ; i++, labels = labels->tl ) {
                cogen->I_getlocal(returnreg);
                cogen->I_pushuint(cogen->emitUInt(i));
                cogen->I_coerce_a();
                cogen->I_ifeq(labels->hd);
            }
                
            cogen->I_label(Lend);
            cogen->I_kill(returnreg);
        }
        
        void TryStmt::cogenNoFinally(Cogen* cogen, Ctx* ctx)
        {
            uint32_t code_start = cogen->getCodeLength();
            cogenStatements(cogen, this->tryblock, ctx);
            uint32_t code_end = cogen->getCodeLength();
            
            Label* Lend = cogen->newLabel();
            cogen->I_jump(Lend);

            for( Seq<CatchClause*>* catches = this->catches ; catches != NULL ; catches = catches->tl )
                cgCatch(cogen, ctx, code_start, code_end, Lend, catches->hd);
            
            cogen->I_label(Lend);
        }
        
        void TryStmt::cgCatch(Cogen* cogen, Ctx* ctx, uint32_t code_start, uint32_t code_end, Label* Lend, CatchClause* catchClause)
        {
            Compiler* compiler = cogen->compiler;
            uint32_t catch_idx = cogen->emitException(code_start,
                                                      code_end,
                                                      cogen->getCodeLength(),
                                                      cogen->emitTypeName(compiler, catchClause->type_name),
                                                      cogen->abc->addQName(cogen->emitNamespace(compiler->parser.defaultNamespace()),
                                                                           cogen->emitString(catchClause->name)));
            
            cogen->startCatch();
            
            uint32_t t = cogen->getTemp();
            restoreScopes(cogen, ctx);
            CatchCtx ctx1(t, ctx);
            
            cogen->I_newcatch(catch_idx);
            cogen->I_dup();
            cogen->I_setlocal(t);   // Store catch scope in register so it can be restored later
            cogen->I_dup();
            cogen->I_pushscope();
            
            // Store the exception object in the catch scope.
            cogen->I_swap();
            cogen->I_setproperty(cogen->abc->addQName(cogen->emitNamespace(compiler->parser.defaultNamespace()),
                                                      cogen->emitString(catchClause->name)));
            
            // catch block body
            cogenStatements(cogen, catchClause->stmts, &ctx1);
            
            cogen->I_kill(t);
            
            cogen->I_popscope();
            cogen->I_jump(Lend);
        }
        
        void DefaultXmlNamespaceStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            expr->cogen(cogen, ctx);
            cogen->I_dxnslate();
        }
        
        void SuperStmt::cogen(Cogen* cogen, Ctx* ctx)
        {
            cogen->I_getlocal(0);
            cogen->I_constructsuper(cogen->arguments(arguments, ctx));
        }
    }
}

#endif // VMCFG_EVAL
