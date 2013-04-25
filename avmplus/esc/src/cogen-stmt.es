/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Gen,
    namespace Gen;

use namespace Abc,
    namespace Asm,
    namespace Ast,
    namespace Emit,
    namespace Util;

function cgStmt(ctx, s) {
    switch type (s) {
    case (s:EmptyStmt) { }
    case (s:ExprStmt) { cgExprStmt(ctx, s) }
    case (s:ForStmt) { cgForStmt(ctx, s) }
    case (s:ForBindingStmt) { cgForBindingStmt(ctx, s) }
    case (s:ForInStmt) { cgForInStmt(ctx, s) }
    case (s:ForInBindingStmt) { cgForInBindingStmt(ctx, s) }
    case (s:ThrowStmt) { cgThrowStmt(ctx, s) }
    case (s:ReturnStmt) { cgReturnStmt(ctx, s) }
    case (s:BreakStmt) { cgBreakStmt(ctx,s) }
    case (s:ContinueStmt) { cgContinueStmt(ctx,s) }
    case (s:BlockStmt) { cgBlockStmt(ctx,s) }
    case (s:LabeledStmt) { cgLabeledStmt(ctx,s) }
    case (s:LetBlockStmt) { cgLetBlockStmt(ctx,s) }
    case (s:WhileStmt) { cgWhileStmt(ctx, s) }
    case (s:DoWhileStmt) { cgDoWhileStmt(ctx, s) }
    case (s:IfStmt) { cgIfStmt(ctx, s) }
    case (s:WithStmt) { cgWithStmt(ctx, s) }
    case (s:TryStmt) { cgTryStmt(ctx, s) }
    case (s:SuperStmt) { cgSuperStmt(ctx, s) }
    case (s:SwitchStmt) { cgSwitchStmt(ctx, s) }
    case (s:SwitchTypeStmt) { cgSwitchTypeStmt(ctx, s) }
    case (s:*) { 
        Gen::internalError(ctx, "Unimplemented statement type " + s);
    }
    }
}

function cgExprStmt(ctx, s) {
    let {asm, stk} = ctx;

    cgExpr(ctx, s.expr);
        
    // Capture the result or discard it.
    //
    // Statement result capture (E262-3 Ch 12):
    //  - if capture is desired then there is a local to receive the value
    //  - exprStmt updates the local with the value instead of popping it
    //  - catch and finally blocks do not affect the captured value

    while (stk != null && stk.tag != "function" && stk.tag != "script" && stk.tag != "finally" && stk.tag != "catch")
        stk = stk.link;
    if (stk != null /*&& stk.tag == "function"*/ && stk.capture_reg) {
        asm.I_coerce_a();
        asm.I_setlocal(stk.capture_reg);
    }
    else
        asm.I_pop();
}

function cgBlockStmt(ctx, {block}) {
    cgBlock(ctx, block);
}

function cgLabeledStmt(ctx, {label, stmt}) {
    let {asm} = ctx;
    let L0 = asm.newLabel();
    cgStmt(pushLabel(ctx, label, L0), stmt);
    asm.I_label(L0);
}

function cgLetBlockStmt(ctx, s) {
    // FIXME
    Gen::internalError(ctx, "Unimplemented: LetBlockStmt");
}

function cgIfStmt(ctx, {test, consequent, alternate}) {
    let {asm} = ctx;
    cgExpr(ctx, test);
    let L0 = asm.I_iffalse(undefined);
    cgStmt(ctx, consequent);
    if (alternate != null) {
        let L1 = asm.I_jump(undefined);
        asm.I_label(L0);
        cgStmt(ctx, alternate);
        asm.I_label(L1);
    }
    else
        asm.I_label(L0);
}

// Probable AST bug: should be no fixtures here, you can't define
// vars in the WHILE head.

function cgWhileStmt(ctx, {stmt, labels, expr}) {
    let {asm}  = ctx;
    let Lbreak = asm.newLabel();
    let Lcont  = asm.I_jump(undefined);
    let Ltop   = asm.I_label(undefined);
    cgStmt(pushBreak(pushContinue(ctx, labels, Lcont), Lbreak), stmt);
    asm.I_label(Lcont);
    cgExpr(ctx, expr);
    asm.I_iftrue(Ltop);
    asm.I_label(Lbreak);
}

// Probable AST bug: should be no fixtures here, you can't define
// vars in the DO-WHILE head.

function cgDoWhileStmt(ctx, {stmt, labels, expr}) {
    let {asm}  = ctx;
    let Lbreak = asm.newLabel();
    let Lcont  = asm.newLabel();
    let Ltop   = asm.I_label(undefined);
    cgStmt(pushBreak(pushContinue(ctx, labels, Lcont), Lbreak), stmt);
    asm.I_label(Lcont);
    cgExpr(ctx, expr);
    asm.I_iftrue(Ltop);
    asm.I_label(Lbreak);
}

function cgForStmt(ctx, {init, cond, incr, stmt, labels}) {
    let {asm} = ctx;
    let Lbreak = asm.newLabel();
    let Lcont = asm.newLabel();
    if (init != null) {
        cgExpr(ctx, init);
        asm.I_pop();
    }
    let Ltop = asm.I_label(undefined);
    if (cond != null) {
        cgExpr(ctx, cond);
        asm.I_iffalse(Lbreak);
    }
    cgStmt(pushBreak(pushContinue(ctx, labels, Lcont), Lbreak), stmt);
    asm.I_label(Lcont);
    if (incr != null) {
        cgExpr(ctx, incr);
        asm.I_pop();
    }
    asm.I_jump(Ltop);
    asm.I_label(Lbreak);
}

function cgForBindingStmt(ctx, s) {
    cgHead(ctx, s.head);
    cgForStmt(ctx, s);
}

function cgForInStmt(ctx, {assignment, tmp, obj, stmt, is_each, labels}) {
    let {asm, emitter} = ctx;

    let Lbreak = asm.newLabel();
    let Lcont = asm.newLabel();

    let T_obj = asm.getTemp();
    let T_idx = asm.getTemp();
    let T_val = asm.getTemp();
    tmp.n = T_val;
    cgExpr(ctx, obj);
    asm.I_coerce_a(); // satisfy verifier
    asm.I_setlocal(T_obj);
    asm.I_pushbyte(0);
    asm.I_setlocal(T_idx);

    let Ltop = asm.I_label(undefined);

    asm.I_hasnext2(T_obj, T_idx);
    asm.I_iffalse(Lbreak);
    asm.I_getlocal(T_obj);
    asm.I_getlocal(T_idx);
        
    if(is_each)
        asm.I_nextvalue();
    else 
        asm.I_nextname();
    asm.I_setlocal(T_val);
    cgExpr(ctx, assignment);
    asm.I_pop();

    cgStmt(pushBreak(pushContinue(ctx, labels, Lcont), Lbreak), stmt);
    asm.I_label(Lcont);
    asm.I_jump(Ltop);

    asm.I_label(Lbreak);
    asm.killTemp(T_val);
    asm.killTemp(T_idx);
    asm.killTemp(T_obj);
}

function cgForInBindingStmt(ctx, s) {
    let {asm, emitter} = ctx;
    let {head, init} = s;
    cgHead(ctx, head);
    cgExpr(ctx, init);
    asm.I_pop();
    cgForInStmt(ctx, s);
}

function cgBreakStmt(ctx, {ident}) {
    function hit (node) {
        return node.tag == "break" && (ident == null || ident == node.label);
    }
    unstructuredControlFlow(ctx,
                            hit,
                            true,
                            (ident == null ? "No 'break' allowed here" : "'break' to undefined label " + ident.text));
}

function cgContinueStmt(ctx, {ident}) {
    function hit(node) {
        return node.tag == "continue" && (ident == null || memberOf(ident, node.labels));
    }
    unstructuredControlFlow(ctx,
                            hit,
                            true,
                            (ident == null ? "No 'continue' allowed here" : "'continue' to undefined label " + ident.text));
}

function cgThrowStmt(ctx, {expr}) {
    let {asm} = ctx;
    cgExpr(ctx, expr);
    asm.I_throw();
}

function cgReturnStmt(ctx, {expr}) {
    let {asm} = ctx;
    let t = null;
    if (expr != null) {
        cgExpr(ctx, expr);
        t = asm.getTemp();
        asm.I_setlocal(t);
    }
    function hit(node){
        return node.tag == "function";
    }
    unstructuredControlFlow(ctx,
                            hit,
                            false,
                            "No 'return' allowed here.");
    if (expr == null)
        asm.I_returnvoid();
    else {
        asm.I_getlocal(t);
        asm.killTemp(t);
        asm.I_returnvalue();
    }
}

function cgSuperStmt(ctx, s) {
    // See comments in cgCtor in cogen.es, about how to do this.
    //
    // The logic right here will not be very complicated:
    //    - check the local supercall flag, fail if set
    //    - call the super constructor
    //    - set the local supercall flag
    Gen::internalError(ctx, "Unimplemented: SuperStmt");
}

// There are many optimization possibilities for 'switch'.
// Notably, if the case expressions are all constants (or there
// are sequences of cases with constant expressions) then binary
// switching or LOOKUPSWITCH can be used.
//
// LOOKUPSWITCH may be used if the case expressions are constant
// int values or single-character strings (we can extract the
// character value and switch on that), with a run-time guard on
// the type of the dispatch value if there is no compile-time type
// information.
//
// For now, we generate LOOKUPSWITCH for "dense" int switches
// above a certain size -- dense meaning at least one third of the
// case values in the range of all the defined case values are
// actually present in the switch, and the size cutoff being four
// labelled cases or more.  The function analyzeSwitch() performs
// a simple analysis and determines whether the optimization should
// kick in or not.
//
// FIXME: handle more interesting cases:
//  - single-character string values
//  - binary search for switches with all constant-value cases
//  - mixed sparse-dense switches
//  - ...

// FIXME: this now carries a head with local bindings.

function cgSwitchStmt(ctx, s) {
    let fastswitch = analyzeSwitch(ctx, s);
    if (!fastswitch)
        cgSwitchStmtSlow(ctx,s);
    else {
        let [low,high,has_default] = fastswitch;
        cgSwitchStmtFast(ctx, s, low, high, has_default);
    }
}

function analyzeSwitch(ctx, s) {
    let cases = s.cases;
    let low = Infinity;
    let high = -Infinity;
    let count = 0;
    let has_default = false;
    for ( let i=0, limit=cases.length ; i < limit ; i++ ) {
        let e = cases[i].expr;
        if (e == null)
            has_default = true;
        else if (e is LiteralInt) {
            low = Math.min(low, e.intValue);
            high = Math.max(high, e.intValue);
            count++;
        }
        else
            return false;
    }
    if (count < 4)
        return false;
    if (count * 3 < ((high - low) + 1))
        return false;
    return [low,high,has_default];
}

function cgSwitchStmtFast(ctx, s, low, high, has_default) {
    let {expr, cases} = s;
    let {asm, cp} = ctx;
    let t = asm.getTemp();

    let Ldef = asm.newLabel();
    let Lcases = new Array(high-low+1);
    let Lbreak = asm.newLabel();
    let nctx = pushBreak(ctx, Lbreak);
    let ldef_emitted = false;

    cgExpr(ctx, expr);                    // switch value
    asm.I_pushint(cp.int32(low));         // offset
    asm.I_subtract();                     // bias it
    asm.I_dup();
    asm.I_convert_i();                    // convert to int
    asm.I_dup();
    asm.I_setlocal(t);                    //   and save
    asm.I_equals();                       // if computed value and int value are 
    asm.I_getlocal(t);                    // otherwise dispatch
    asm.killTemp(t);
    asm.I_swap();
    asm.I_iffalse(Ldef);                  //   not the same then default case

    let Ldefault = asm.I_lookupswitch(undefined, Lcases);

    // Make a prepass to find all the case labels that do not have a
    // case (except maybe the default case).  If Lcases[i] is not
    // handled then Lhandled[i] will be false.

    let Lhandled = new Array(Lcases.length);
    for ( let i=0, limit=cases.length ; i < limit ; i++ ) {
        let c = cases[i];
        let e = c.expr;

        if (e != null) {
            assert(e is LiteralInt);
            Lhandled[e.intValue - low] = true;
        }
    }

    // Now emit code for all the cases.  If there is a default
    // case then all unhandled labels are emitted there.

    for ( let i=0, limit=cases.length ; i < limit ; i++ ) {
        let c = cases[i];
        let e = c.expr;

        if (e == null) {
            asm.I_label(Ldef);
            asm.I_pop();
            asm.I_label(Ldefault);
            ldef_emitted = true;
            for ( let j=0, jlimit=Lhandled.length ; j < jlimit ; j++ )
                if (!Lhandled[j])
                    asm.I_label(Lcases[j]);
        }
        else {
            assert(e is LiteralInt);

            // There might be duplicate case selector values, but only the first one counts.
            if (Lcases[e.intValue - low] !== false) {
                asm.I_label(Lcases[e.intValue - low]);
                Lcases[e.intValue - low] = false;
            }
        }

        let stmts = c.stmts;
        for ( let j=0, jlimit=stmts.length ; j < jlimit ; j++ )
            cgStmt(nctx, stmts[j] );
    }

    // If there was not a default case then map unhandled case
    // values to this point.

    if (!has_default) {
        for ( let j=0, jlimit=Lhandled.length ; j < jlimit ; j++ )
            if (!Lhandled[j])
                asm.I_label(Lcases[j]);
    }

    if (!ldef_emitted) {
        asm.I_label(Ldef);
        asm.I_pop();
    }
    asm.I_label(Lbreak);
}

function cgSwitchStmtSlow(ctx, {expr,cases}) {
    let {asm} = ctx;
    cgExpr(ctx, expr);
    let t = asm.getTemp();
    asm.I_coerce_a();
    asm.I_setlocal(t);
    let Ldefault = null;
    let Lnext = null;
    let Lfall = null;
    let Lbreak = asm.newLabel();
    let nctx = pushBreak(ctx, Lbreak);
    for ( let i=0, limit=cases.length ; i < limit ; i++ ) {
        let c = cases[i];

        if (c.expr == null) {
            assert (Ldefault==null);
            Ldefault = asm.I_label(undefined);    // label default pos
        }
        else {
            if (Lnext !== null) {
                asm.I_label(Lnext);               // label next pos
                Lnext = null;
            }
            cgExpr(nctx, c.expr);                 // check for match
            asm.I_getlocal(t);
            asm.I_strictequals();
            Lnext = asm.I_iffalse(undefined);     // if no match jump to next label
        }

        if (Lfall !== null) {                     // label fall through pos
            asm.I_label(Lfall);
            Lfall = null;
        }

        let stmts = c.stmts;
        for ( let j=0, jlimit=stmts.length ; j < jlimit ; j++ )
            cgStmt(nctx, stmts[j] );

        Lfall = asm.I_jump(undefined);            // fall through
    }
    if (Lnext !== null)
        asm.I_label(Lnext);
    if (Ldefault !== null)
        asm.I_jump(Ldefault);
    if (Lfall !== null)
        asm.I_label(Lfall);
    asm.I_label(Lbreak);
    asm.killTemp(t);
}

function cgSwitchTypeStmt(ctx, {expr,cases}) {
    let {asm} = ctx;
    let t = new Ast::GetCogenTemp();
    t.n = asm.getTemp();
    cgExpr(ctx, expr);
    asm.I_setlocal(t.n);
    let b = new Ast::Block(new Ast::Head([],[]), [new Ast::ThrowStmt(t)]);
    let newcases = [];
    let hasDefault = false;

    for( let i = 0, limit=cases.length ; i < limit ; i++ ) {
        newcases.push(cases[i]);
        let {data} = cases[i].param.fixtures[0];
        if (Ast::isAnyType(data.ty))
            hasDefault = true;
    }

    // Add a catch all case so we don't end up throwing whatever the switch type expr was
    if (!hasDefault) {
        newcases.push(new Ast::Catch(new Ast::Head([ new Ast::Fixture(new Ast::PropName(new Ast::Name(Ast::publicNS, Token::sym_x)),
                                                                      new Ast::ValFixture(Ast::anyType, false)) ],
                                                   []),
                                     new Ast::Block(new Ast::Head([],[]), [])));
    }
    cgTryStmt(ctx, {block:b, catches:newcases, finallyBlock:null} );
    asm.killTemp(t.n);
}
    
function cgWithStmt(ctx, {expr,stmt}) {
    let {asm} = ctx;
    let scopereg = asm.getTemp();

    cgExpr(ctx, expr);
    asm.I_dup();
    asm.I_setlocal(scopereg);
    asm.I_pushwith();
    cgStmt(pushWith(ctx, scopereg), stmt);
    asm.I_popscope();
    asm.killTemp(scopereg);
}
    
function cgTryStmt(ctx, s) {
    if (s.finallyBlock != null)
        cgTryStmtWithFinally(ctx, s);
    else
        cgTryStmtNoFinally(ctx, s);
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
//   the finally block (in inside-out if there are several)
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

function cgTryStmtWithFinally(ctx, {block, catches, finallyBlock}) {
    let {asm, emitter, target, cp} = ctx;

    let returnreg = asm.getTemp();
    let Lfinally = asm.newLabel();
    let newctx = pushFinally(ctx, Lfinally, returnreg);
    let rib = newctx.stk;

    let myreturn = rib.nextReturn++;
    rib.returnAddresses[myreturn] = null; // aka "Lreturn"; will be initialized below

    let Lend = asm.newLabel();
    let myend = rib.nextReturn++;
    rib.returnAddresses[myend] = Lend;

    let code_start = asm.length;
    cgTryStmtNoFinally(newctx, s);
    let code_end = asm.length;

    // Fallthrough from try-catch: visit the finally block.  This
    // code must not be in the scope of the generated exception
    // handler.

    asm.I_pushint(cp.int32(myend));
    asm.I_setlocal(returnreg);
    asm.I_jump(Lfinally);                    // control continues at Lend below

    // Generated catch block to handle throws out of try-catch:
    // capture the exception, visit the finally block with return
    // to Lreturn, then re-throw the exception at Lreturn.
    //
    // Use a lightweight exception handler; always store the value
    // in a register.

    let savedExn = asm.getTemp();
    let catch_idx = target.addException(new ABCException(code_start, code_end, asm.length, 0, 0));

    asm.startCatch();           // push 1 item
    asm.I_setlocal(savedExn);   // pop and save it

    restoreScopes(ctx);         // finally block needs correct scopes

    asm.I_pushint(cp.int32(myreturn));
    asm.I_setlocal(returnreg);
    asm.I_jump(Lfinally);                                    // control continues at Lreturn
    rib.returnAddresses[myreturn] = asm.I_label(undefined);  // "Lreturn" is here
    asm.I_getlocal(savedExn);
    asm.killTemp(savedExn);
    asm.I_throw();

    // Finally block

    asm.I_label(Lfinally);
    cgBlock(ctx, finallyBlock);

    // The return-from-subroutine code at the end of the finally block

    let visitors = newctx.stk;
    let {nextReturn:numvisits, returnAddresses:visitors} = rib;
    let Lcases = new Array(numvisits);
    asm.I_getlocal(returnreg);
    asm.I_convert_i();
    let Ldefault = asm.I_lookupswitch(undefined, Lcases);
    asm.I_label(Ldefault); // Default case is never hit.
    for ( let i=0 ; i < numvisits ; i++ ) {
        asm.I_label(Lcases[i]);
        asm.I_jump(visitors[i]);
    }

    asm.I_label(Lend);
    asm.killTemp(returnreg);
}

function cgTryStmtNoFinally(ctx, {block, catches}) {
    let {asm} = ctx;
    let code_start = asm.length;
    cgBlock(ctx, block);
    let code_end = asm.length;
        
    let Lend = asm.newLabel();
    asm.I_jump(Lend);

    for( let i = 0, limit=catches.length ; i < limit ; ++i )
        cgCatch(ctx, code_start, code_end, Lend, catches[i]);
        
    asm.I_label(Lend);
}
    
function cgCatch(ctx, code_start, code_end, Lend, {param, block} ) {
    let {asm, emitter, target} = ctx;
        
    if( param.fixtures.length != 1 )
        Gen::internalError(ctx, "Catch block should have 1 fixture");
        
    let {name:propname, data:fix} = param.fixtures[0];
        
    let param_name = emitter.fixtureNameToName(propname);
    let param_type = emitter.realTypeName(fix.ty);
        
    let catch_idx = target.addException(new ABCException(code_start, code_end, asm.length, param_type, param_name));

    asm.startCatch();

    let t = asm.getTemp();
    restoreScopes(ctx);
    let catch_ctx = pushCatch(ctx,t);

    asm.I_newcatch(catch_idx);
    asm.I_dup();
    asm.I_setlocal(t);  // Store catch scope in register so it can be restored later
    asm.I_dup();
    asm.I_pushscope();
        
    // Store the exception object in the catch scope.
    asm.I_swap();
    asm.I_setproperty(param_name);

    // catch block body
    cgBlock(catch_ctx, block);
        
    asm.killTemp(t);
        
    asm.I_popscope();
    asm.I_jump(Lend);
}
