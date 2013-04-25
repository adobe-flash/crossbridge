/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* FIXME: handling 'super'.
 *
 * These work now:
 *
 * super.m(...)
 *   calls the super class's "m" with "this" as the receiver:
 *   'callsuper' instruction
 *
 * super(...)
 *   calls the super class's constructor on the arguments:
 *   'constructsuper' instruction
 *   only legal in a constructor, the parser annotates the Ctor structure with the args,
 *   don't need to handle this as an expression
 *
 *
 * These have yet to be handled:
 *
 * super.x
 *   picks up the 'x' member from the super class:
 *   'getsuper' instruction
 *
 * super(o).m()
 *   calls the super class's "m" with "o" as the receiver (o must be of a reasonable type):
 *   'callsuper' instruction
 */

use default namespace Gen,
    namespace Gen;

use namespace Abc,
    namespace Asm,
    namespace Ast,
    namespace Emit,
    namespace Util;

internal var lastline = -1;

function cgExpr(ctx, e) {
    if (emit_debug && e.pos > 0 && e.pos != lastline) {
        ctx.asm.I_debugline(e.pos);
        lastline = e.pos;
    }
    switch type (e) {
    case (e:TernaryExpr) { cgTernaryExpr(ctx, e) }
    case (e:BinaryExpr) { cgBinaryExpr(ctx, e) }
    case (e:BinaryTypeExpr) { cgBinaryTypeExpr(ctx, e) }
    case (e:UnaryExpr) { cgUnaryExpr(ctx, e) }
    case (e:TypeOpExpr) { cgTypeOpExpr(ctx, e) }
    case (e:ThisExpr) { cgThisExpr(ctx, e) }
    case (e:YieldExpr) { cgYieldExpr(ctx, e) }
    case (e:SuperExpr) { 
        Gen::syntaxError(ctx, "A 'super' expression can't appear here");
    }
    case (e:LiteralExpr) { cgLiteralExpr(ctx, e) }
    case (e:CallExpr) { cgCallExpr(ctx, e) }
    case (e:ApplyTypeExpr) { cgApplyTypeExpr(ctx, e) }
    case (e:LetExpr) { cgLetExpr(ctx, e) }
    case (e:DynamicOverrideExpr) { cgDynamicOverrideExpr(ctx, e) }
    case (e:NewExpr) { cgNewExpr(ctx, e) }
    case (e:ObjectRef) { cgObjectRef(ctx, e) }
    case (e:IdentExpr) { cgIdentExprNode(ctx, e) }
    case (e:SetExpr) { cgSetExpr(ctx, e) }
    case (e:InitExpr) { cgInitExpr(ctx, e) }
    case (e:GetTemp) { cgGetTempExpr(ctx, e) }
    case (e:GetParam) { cgGetParamExpr(ctx, e) }
    case (e:GetCogenTemp) { cgGetCogenTemp(ctx, e) }
    case (e:EvalScopeInitExpr) { cgEvalScopeInitExpr(ctx,e) }
    case (e:*) { 
        Gen::internalError(ctx, "Unimplemented expression type " + e);
    }
    }
}

function cgTernaryExpr(ctx, { e1: test, e2: consequent, e3: alternate }) {
    let {asm} = ctx;
    cgExpr(ctx, test);
    let L0 = asm.I_iffalse(undefined);
    cgExpr(ctx, consequent);
    asm.I_coerce_a();
    let L1 = asm.I_jump(undefined);
    asm.I_label(L0);
    cgExpr(ctx, alternate);
    asm.I_coerce_a();
    asm.I_label(L1);
}

function cgBinaryExpr(ctx, e) {
    let {asm} = ctx;
    if (e.op == logicalAndOp) {
        cgExpr(ctx, e.e1);
        asm.I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
        asm.I_dup();
        asm.I_convert_b();
        let L0 = asm.I_iffalse(undefined);
        asm.I_pop();
        cgExpr(ctx, e.e2);
        asm.I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
        asm.I_label(L0);
    }
    else if (e.op == logicalOrOp) {
        cgExpr(ctx, e.e1);
        asm.I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
        asm.I_dup();
        asm.I_convert_b();
        let L0 = asm.I_iftrue(undefined);
        asm.I_pop();
        cgExpr(ctx, e.e2);
        asm.I_coerce_a();  // wrong, should coerce to LUB of lhs and rhs
        asm.I_label(L0);
    }
    else if (e.op == commaOp) {
        cgExpr(ctx, e.e1);
        asm.I_pop();
        cgExpr(ctx, e.e2);
    }
    else {
        cgExpr(ctx, e.e1);
        cgExpr(ctx, e.e2);
        switch (e.op) {
        case plusOp:               asm.I_add(); break;
        case minusOp:              asm.I_subtract(); break;
        case timesOp:              asm.I_multiply(); break;
        case divideOp:             asm.I_divide(); break;
        case remainderOp:          asm.I_modulo(); break;
        case leftShiftOp:          asm.I_lshift(); break;
        case rightShiftOp:         asm.I_rshift(); break;
        case rightShiftUnsignedOp: asm.I_urshift(); break;
        case bitwiseAndOp:         asm.I_bitand(); break;
        case bitwiseOrOp:          asm.I_bitor(); break;
        case bitwiseXorOp:         asm.I_bitxor(); break;
        case instanceOfOp:         asm.I_instanceof(); break;
        case inOp:                 asm.I_in(); break;
        case equalOp:              asm.I_equals(); break;
        case notEqualOp:           asm.I_equals(); asm.I_not(); break;
        case strictEqualOp:        asm.I_strictequals(); break;
        case strictNotEqualOp:     asm.I_strictequals(); asm.I_not(); break;
        case lessOp:               asm.I_lessthan(); break;
        case lessOrEqualOp:        asm.I_lessequals(); break;
        case greaterOp:            asm.I_greaterthan(); break;
        case greaterOrEqualOp:     asm.I_greaterequals(); break;
        default:                   Gen::internalError(ctx, "Unimplemented binary operator " + e);
        }
    }
}

internal var id_TypeError = new Ast::Identifier(Token::sym_TypeError, Ast::publicNSSL);

function cgBinaryTypeExpr(ctx, {op, e1, e2}) {
    let {asm, cp} = ctx;
    switch (op) {
    case castOp: {
        // ES4 'cast'.
        //
        // OPTIMIZEME.  This would benefit from an "OP_coercelate"
        // opcode for brevity and less control flow but the code
        // below should be correct as far as the language and
        // verifier are concerned.
        //
        // OPTIMIZEME.  Early-bind the right hand side if possible
        // and use simpler code here (probably just a single
        // OP_coerce instruction if that does not invoke any user
        // defined converters).
        cgExpr(ctx, e1);
        asm.I_dup();
        cgTypeExprHelper(ctx, e2);
        asm.I_istypelate();
        let L0 = asm.I_iftrue(undefined);
        asm.I_pop();
        // FIXME: should lookup directly in global obj or be unforgeable name.
        asm.I_findproperty(cgIdentExpr(ctx, id_TypeError));
        asm.I_pushstring(cp.stringUtf8("Cast failed."));
        asm.I_constructprop(cgIdentExpr(ctx, id_TypeError), 1);
        asm.I_throw();
        asm.I_label(L0);
        asm.I_coerce_a();
        break;
    }
    case isOp: { 
        // ES4 'is'.
        //
        // OPTIMIZEME.  Early-bind the right hand side if possible
        // and use simpler code here (probably just a single
        // OP_istype instruction).
        cgExpr(ctx, e1);
        cgTypeExprHelper(ctx, e2);
        asm.I_istypelate();
        break;
    }
    default:
        Gen::internalError(ctx, "Unimplemented binary type operator " + op);
    }
}

function cgTypeOpExpr(ctx, {ex}) {
    cgTypeExprHelper(ctx, ex);
}

function cgTypeExprHelper(ctx, ty) {
    let {asm, emitter} = ctx;
    switch type (ty) {
    case (ty:TypeName) {
        asm.I_findpropstrict(cgIdentExpr(ctx, ty.ident));
        asm.I_getproperty(cgIdentExpr(ctx, ty.ident));
    }
    case (ty:*) {
        /* FIXME */
        Gen::internalError(ctx, "Unimplemented type expression type " + ty);
    }
    }
}

function cgUnaryExpr(ctx, e) {
    let {asm, emitter} = ctx;

    switch (e.op & strictMask) {
    case deleteOp: {
        // FIXME: delete operator strict mode
        switch type (e.e1) {
        case (lr: IdentExpr) {
            let bind = getIdentBinding(ctx, lr);
            if( bind === Ast::nobind ) {
	            asm.I_findproperty(cgIdentExpr(ctx, lr));
	            asm.I_deleteproperty(cgIdentExpr(ctx, lr));
        	}
            else
                asm.I_pushtrue();  // can't delete a fixed property
        }
        case (or: ObjectRef) {
            cgExpr(ctx, or.base);
            asm.I_deleteproperty(cgIdentExpr(ctx, or.ident));
        }
        case (e1:*) {
            cgExpr(ctx, e1);
            asm.I_pop();
            asm.I_pushtrue();
        }
        }
        break;
    }

    case voidOp:
        cgExpr(ctx, e.e1);
        asm.I_pop();
        asm.I_pushundefined();
        break;

    case typeOfOp:
        if (e.e1 is IdentExpr) {
            cgFindProp(ctx, e.e1);
            cgGetProp(ctx, e.e1);
        }
        else 
            cgExpr(ctx, e.e1);
        asm.I_typeof();
        break;

    case preIncrOp: 
        incdec(true, true); 
        break;

    case preDecrOp: 
        incdec(true, false); 
        break;

    case postIncrOp: 
        incdec(false, true); 
        break;

    case postDecrOp: 
        incdec(false, false); 
        break;

    case unaryPlusOp:
        cgExpr(ctx, e.e1);
        asm.I_convert_d();
        break;

    case unaryMinusOp:
        cgExpr(ctx, e.e1);
        asm.I_negate();
        break;

    case bitwiseNotOp:
        cgExpr(ctx, e.e1);
        asm.I_bitnot();
        break;

    case logicalNotOp:
        cgExpr(ctx, e.e1);
        asm.I_not();
        break;

    default:
        Gen::internalError(ctx, "Unimplemented unary operation " + op);
    }

    function incdec(pre, inc) {
        switch type (e.e1) {
        case (lr: IdentExpr) {
            if (cgFindPropStrict(ctx, lr))  // Ugly-ish.  Object will be used by cgSetProp later.
                asm.I_dup();
            cgGetProp(ctx, e.e1);
        }
        case (or:ObjectRef) {
            cgExpr(ctx, or.base);
            asm.I_dup();
            cgGetProp(ctx, e.e1.ident);
        }
        case (x:*) { 
            Gen::syntaxError(ctx, "Expression is not an lvalue: " + e.e1);
        }
        }
        let t = asm.getTemp();
        if (pre) {
            if (inc)
                asm.I_increment();
            else
                asm.I_decrement();
            asm.I_dup();
            asm.I_setlocal(t);
        }
        else {
            // Postfix ops return value after conversion to number.
            asm.I_convert_d();
            asm.I_dup();
            asm.I_setlocal(t);
            if (inc)
                asm.I_increment();
            else
                asm.I_decrement();
        }
        switch type (e.e1) {
        case (lr: IdentExpr) {
            cgSetProp(ctx, lr);
        }
        case (or: ObjectRef) {
            cgSetProp(ctx, or.ident);
        }
        }
        asm.I_getlocal(t);
        asm.killTemp(t);
    }
}

function cgThisExpr({asm}, e) {
    asm.I_getlocal(0);
}

function cgYieldExpr(ctx, e) {
    // FIXME
    Gen::internalError(ctx, "Unimplemented 'yield' expression");
}

function cgCallExpr(ctx, e) {
    let {asm, emitter} = ctx;
    let nargs = e.args.length;
    let evalTmp = 0;
    let isEval = false;
    let isSuperCall = false;
    let name;

    if (e.spread != null)
        Gen::internalError(ctx, "Spread expression not implemented.");

    switch type (e.expr) {
    case (or: ObjectRef) {
        if (or.base is SuperExpr) {
            assert(or.base.ex == null); // If not then super(o) form, don't know what that is yet.  --lars
            asm.I_getlocal(0);
            isSuperCall = true;
        }
        else
            cgExpr(ctx, or.base);
    }
    case (lr: IdentExpr) {
        var bind = getIdentBinding(ctx, lr);
        if( bind is RegBind ) {
            asm.I_getlocal(bind.reg);            
            asm.I_pushnull(); // Should we be using something else for this?
        }
        else {
            cgFindPropStrict(ctx, lr);
        }
        if (lr is Identifier && lr.ident == Token::sym_eval) {
            isEval = true;
            evalTmp = asm.getTemp();   // save the
            asm.I_dup();               //   object
            asm.I_setlocal(evalTmp);   //     for later
        }
    }
    case (x:*) {
        cgExpr(ctx, e.expr);
        asm.I_pushnull();
    }
    }

    if (e.expr is ObjectRef) {
        // Runtime/late parts appear on stack before arguments!
        name = cgIdentExpr(ctx, e.expr.ident);
    }

    for ( let i=0 ; i < nargs ; i++ )
        cgExpr(ctx, e.args[i]);

    let L_skipcall;

    if (isEval) {
        // Code performs 'eval', cleans the stack, and jumps to L0
        // if the eval operator kicks in, otherwise falls through
        // to the regular call code below.
        L_skipcall = cgEvalPrefix(ctx, evalTmp, nargs, e.strict);
    }

    switch type (e.expr) {
    case (or: ObjectRef) {
        if (isSuperCall)
            asm.I_callsuper(name, nargs);
        else
            asm.I_callproperty(name, nargs);
    }
    case (lr: IdentExpr) {
        // This is not right if the function is bound by "with".  In that
        // case, I_callproperty would be more right.  That's the outlier, though.
        var bind = getIdentBinding(ctx, lr);
        if( bind is SlotBind || bind == Ast::nobind )
            asm.I_callproplex(cgIdentExpr(ctx, lr), nargs);
        else if ( bind is RegBind ) {
            asm.I_call(nargs);
        }
    }
    case (x:*) {
        asm.I_call(nargs);
    }
    }

    if (isEval) {
        asm.I_label(L_skipcall);
        asm.killTemp(evalTmp);
    }
}

// First check that 'eval' really gets us the global eval binding.
//
// Assuming it really is the eval operator:
//
// Since the form of the call is eval(...) we know there are no
// late-bound name components on the evaluation stack, just the
// receiver object (which is the global object).  Clean up the
// stack by consing up an array of all the arguments.
//
// Call ESC::eval() on the argument array, a descriptor of the
// scopes (a string), and an array of scopes, outermost-first.
// The descriptor and the scope array have the same length.  A
// letter in the descriptor is "s" if the object is a regular
// scope object (to be pushed by "pushscope") and "w" if it is is
// a "with" object (to be pushed by "pushwith").
//
// ESC::evalCompiler() returns the result of the evaluation.

function cgEvalPrefix(ctx, evalTmp, nargs, strict) {
    let {asm} = ctx;
    let id_ESC = new Ast::Identifier(Token::sym_ESC, Ast::publicNSSL);
    let id_evaluateInScopeArray = new Ast::QualifiedIdentifier(id_ESC, Token::sym_evaluateInScopeArray);

    // Check it: Is this *really* the eval operator?

    let L_normalcall = undefined;

    /* FIXME: The following sanity tests only work when getglobalscope returns the object 
       that actually holds the global variables.  Bugzilla 417342.

       // The container must be === to the global object
       asm.I_getlocal(evalTmp);
       asm.I_getglobalscope();
       asm.I_ifstrictne(L_normalcall);

       // Property must be === to the predefined eval function, stored in the constant ESC::eval
       asm.I_getlocal(evalTmp);
       asm.I_getproperty(cgIdentExpr(ctx, lr.ident));
       asm.I_getglobalscope();
       asm.i_getproperty(cgIdentExpr(ctx, new Ast::QualifiedIdentifier(id_ESC,
       new Ast::Identifier("eval", [[Ast::NoNS]]))));
       asm.I_ifstrictne(L_normalcall);
    */

    asm.I_newarray(nargs);             // collapse the arguments
    asm.I_setlocal(evalTmp);           // save it...
    asm.I_pop();                       // get rid of the object
    asm.I_findpropstrict(cgIdentExpr(ctx, id_evaluateInScopeArray));
    asm.I_getlocal(evalTmp);           // argument array

    // pushScopes() generates code that leaves an array of scope
    // objects on the stack, followed by the scope descriptor
    // string.

    let numscopes = pushScopes(ctx);
    if (strict)
        asm.I_pushtrue();
    else
        asm.I_pushfalse();
    asm.I_callproplex(cgIdentExpr(ctx, id_evaluateInScopeArray), 4);

    L_skipcall = asm.I_jump(undefined);

    if (L_normalcall != undefined)
        asm.I_label(L_normalcall);

    return L_skipcall;

    function pushScopes(ctx) {

        let scopedesc = "";

        function rec(stk, inner) {
            let level;

            // Do not capture the global scope
            if (stk.tag == "script")
                return 1;

            if (stk.tag == "function") {
                level = rec(stk.link, false);
                if (inner)
                    level = 0;
            }
            else
                level = rec(stk.link, inner);

            // FIXME: There may be two scope objects per ctx rib,
            // because push_this may be true too.
            push = false;
            switch (stk.tag) {
            case "function":
            case "catch":
                scopedesc = "s" + scopedesc;
                push = true;
                break;
            case "with":
                scopedesc = "w" + scopedesc;
                push = true;
                break;
            case "class":
            case "instance":
            case "break":
            case "continue":
            case "finally":
                break;
            case "let":
            default:
                Gen::internalError(ctx, "Unknown context tag: " + stk.tag);
            }

            if (push) {
                if (inner)
                    asm.I_getscopeobject(level);
                else
                    asm.I_getouterscope(level);
                return level + 1;
            }
            return level;
        }

        rec(ctx.stk, true);

        let numscopes = scopedesc.length;

        ctx.asm.I_newarray(numscopes);
        ctx.asm.I_pushstring(ctx.cp.stringUtf8(scopedesc));

        return numscopes;
    }
}

function cgEvalScopeInitExpr(ctx, {index, how}) {
    let {asm} = ctx;
    asm.I_getlocal(1);
    asm.I_getproperty(cgIdentExpr(ctx, new Ast::Identifier(String(index), Ast::publicNSSL)));
    if (how == "w")
        asm.I_pushwith();
    else
        asm.I_pushscope();
    asm.I_pushundefined();
}

function cgApplyTypeExpr(ctx, e) {
    // FIXME
    Gen::internalError(ctx, "Unimplemented type application expression");
}

function cgLetExpr(ctx, {head, expr}) {
    cgHead(ctx, head);
    cgExpr(ctx, expr);
}

function cgDynamicOverrideExpr(ctx, {names, exprs, body}) {
    assert(names.length == exprs.length);

    // create code to secret current values away in temps
    // set up a simplified try-finally.
    // Duplicate the restoring code: the catch block restores
    // and re-throws (but does not restore the scope chain?? It
    // may need to, since variables to restore could be bound
    // in various objects in the local scope chain.)
    // The fallthrough code restores and continues.

    // body

    // finally handler
    // create code to restore values from temps
    
    // FIXME: this code is only right if exceptions are not
    // thrown.
    //
    // FIXME: this code evaluates subexpressions of the names several
    // times.

    let {asm} = ctx;
    let limit = names.length;
    let temps = [];
    let temps2 = [];

    // Save existing values
    for ( let i=0 ; i < limit ; i++ ) {
        temps[i] = asm.getTemp();
        asm.I_findpropstrict(cgIdentExpr(ctx, names[i]));
        asm.I_getproperty(cgIdentExpr(ctx, names[i]));
        asm.I_setlocal(temps[i]);
    }
    // Compute new values
    for ( let i=0 ; i < limit ; i++ ) {
        temps2[i] = asm.getTemp();
        cgExpr(ctx, exprs[i]);
        asm.I_setlocal(temps2[i]);
    }
    // Assign new values
    for ( let i=0 ; i < limit ; i++ ) {
        asm.I_findpropstrict(cgIdentExpr(ctx, names[i]));
        asm.I_getlocal(temps2[i]);
        asm.I_setproperty(cgIdentExpr(ctx, names[i]));
        asm.killTemp(temps2[i]);
    }
    // Evaluate body
    cgExpr(ctx, body);
    // Restore old values
    for ( let i=0 ; i < limit ; i++ ) {
        asm.I_findpropstrict(cgIdentExpr(ctx, names[i]));
        asm.I_getlocal(temps[i]);
        asm.I_setproperty(cgIdentExpr(ctx, names[i]));
        asm.killTemp(temps[i]);
    }
    // Result of body is left on the stack.
}
  

function cgNewExpr(ctx, {expr, args, spread}) {
    let {asm} = ctx;

    if (spread != null)
        Gen::internalError(ctx, "Spread expression not implemented");

    cgExpr(ctx, expr);
    for ( let i=0, limit=args.length ; i < limit ; i++ )
        cgExpr(ctx, args[i]);
    asm.I_construct(args.length);
}

function cgObjectRef(ctx, {base, ident}) {
    let {asm} = ctx;
    cgExpr(ctx, base);
    asm.I_getproperty(cgIdentExpr(ctx, ident));
}

// Not to be confused with the subroutine cgIdentExpr below.  That one
// ought to be renamed, not this one.

function cgIdentExprNode(ctx, ident) {
    cgFindPropStrict(ctx, ident);
    cgGetProp(ctx, ident);
}

function cgSetExpr(ctx, e) {
    switch type (e.le) {
    case (objref: Ast::ObjectRef) { cgSetObjectRefExpr(ctx, e) }
    case (ident: Ast::IdentExpr)  { cgSetIdentExpr(ctx, e) }
    case (x: *)                   { Gen::syntaxError(ctx, "Illegal lvalue " + x) }
    }
}

function cgSetIdentExpr(ctx, {op, le, re}) {
    let {asm, emitter} = ctx;
    let opr = op & strictMask;
    let t = asm.getTemp();
        
    if (opr == assignOp) {
        cgFindProp(ctx, le);
        cgExpr(ctx, re);
    }
    else {
        if (cgFindPropStrict(ctx, le))
            asm.I_dup();
        cgGetProp(ctx, le);
        cgOperate(ctx, re, opr, (op & strictFlag) != 0);
    }

    asm.I_dup();
    asm.I_setlocal(t);
    cgSetProp(ctx, le);
    asm.I_getlocal(t);
    asm.killTemp(t);
}

function cgSetObjectRefExpr(ctx, {op, le, re}) {
    let {asm, emitter} = ctx;
    let opr = op & strictMask;
    let t = asm.getTemp();

    cgExpr(ctx, le.base);
    if( le.ident is Identifier )
        le.ident.binding = Ast::nobind;

    if (opr == assignOp) {
        let name = cgIdentExpr(ctx, le.ident);   // order matters if it's a ComputedName
        cgExpr(ctx, re);
        asm.I_dup();
        asm.I_setlocal(t);
        asm.I_setproperty(name);
    }
    else {
        let subtmp = null;     // stores indexing expression value
        let subname = null;    // multiname to store under
        asm.I_dup();           // object expr

        if (le.ident is ComputedName) {
            subtmp = asm.getTemp();
            cgExpr(ctx, le.ident.expr);
            asm.I_dup();
            asm.I_setlocal(subtmp);
            subname = emitter.multinameL(Ast::publicNSSL, false);
            asm.I_getproperty(subname);
        }
        else
            cgGetProp(ctx, le.ident);

        cgOperate(ctx, re, opr, (op & strictFlag) != 0);

        asm.I_dup();
        asm.I_setlocal(t);
        if (le.ident is ComputedName) {
            asm.I_getlocal(subtmp);
            asm.I_swap();
            asm.I_setproperty(subname);
            asm.killTemp(subtmp);
        }
        else
            cgSetProp(ctx, le.ident);
    }
    asm.I_getlocal(t);
    asm.killTemp(t);
}

function cgOperate(ctx, expr, op, is_strict) {
    // FIXME: assignment operators strict mode

    // the left-hand expression is on the stack.
    let {asm} = ctx;
    if (op == assignLogicalAndOp || op == assignLogicalOrOp) {
        asm.I_dup();
        asm.I_convert_b();
        let L0 = (op == assignLogicalAndOp) ? asm.I_iffalse(undefined) : asm.I_iftrue(undefined);
        asm.I_pop();
        cgExpr(ctx, expr);
        asm.I_coerce_a();
        asm.I_label(L0);
    }
    else {
        cgExpr(ctx, expr);
        switch (op) {
        case assignPlusOp:                asm.I_add(); break;
        case assignMinusOp:               asm.I_subtract(); break;
        case assignTimesOp:               asm.I_multiply(); break;
        case assignDivideOp:              asm.I_divide(); break;
        case assignRemainderOp:           asm.I_modulo(); break;
        case assignLeftShiftOp:           asm.I_lshift(); break;
        case assignRightShiftOp:          asm.I_rshift(); break;
        case assignRightShiftUnsignedOp:  asm.I_urshift(); break;
        case assignBitwiseAndOp:          asm.I_bitand(); break;
        case assignBitwiseOrOp:           asm.I_bitor(); break;
        case assignBitwiseXorOp:          asm.I_bitxor(); break;
        default:                          Gen::internalError(ctx, "ASSIGNOP not supported " + op);
        }
    }
}

function cgInitExpr(ctx, e) {
    let {asm} = ctx;
    let baseOnStk = false;
    if (e.target == instanceInit) {
        // Load this on the stack
        asm.I_getlocal(0);
        baseOnStk = true;
    }
    cgInits(ctx, e.inits, baseOnStk);
    asm.I_pushundefined(); // exprs need to leave something on the stack
    // FIXME: should this be the value of the last init?
}

function cgLiteralExpr(ctx, e) {

    function cgArrayInitializer(ctx, {exprs, spread}) {
        let {asm} = ctx;
        let i = 0;
        let limit = exprs.length

        if (spread != null)
            Gen::internalError(ctx, "Spread expression in array initializer not implemented.");

        // Use newarray to construct the dense prefix
        for ( ; i < limit ; i++ ) {
            let e = exprs[i];
            if (e is Ast::LiteralUndefined)
                break;
            cgExpr(ctx, e);
        }
        asm.I_newarray(i);

        // Then init the other defined slots one by one
        if (i < limit) {
            let last_was_undefined = false;
            for ( ; i < limit ; i++ ) {
                let e = exprs[i];
                if (!(e is Ast::LiteralUndefined)) {
                    asm.I_dup();
                    cgExpr(ctx, e);
                    asm.I_setproperty(cgIdentExpr(ctx, new Ast::Identifier(Token::intern(i), Ast::publicNSSL)));
                    last_was_undefined = false;
                }
                else
                    last_was_undefined = true;
            }
            if (last_was_undefined) {
                asm.I_dup();
                asm.I_pushint(ctx.cp.int32(limit));
                asm.I_setproperty(cgIdentExpr(ctx, new Ast::Identifier(Token::sym_length, Ast::publicNSSL)));
            }
        }
    }

    function cgObjectInitializer(ctx, {fields:fields}) {
        let {asm, emitter} = ctx;
        asm.I_findpropstrict(ctx.emitter.Object_name);
        asm.I_constructprop(ctx.emitter.Object_name, 0);
        let t = asm.getTemp();
        asm.I_setlocal(t);
        for ( let i=0, limit=fields.length ; i < limit ; i++ ) {
            switch type (fields[i]) {
            case (lf: Ast::LiteralField) {
                // SYNTACTIC CONDITION.  If the object initializer is
                // used to produce a value (it's not used for
                // destructuring) then the ": expr" part is not
                // optional.

                if (lf.expr == null) 
                    Gen::syntaxError(ctx, "Missing field value in object initializer: " + lf.ident);

                asm.I_getlocal(t);
                cgExpr(ctx, lf.expr);
                asm.I_setproperty(cgIdentExpr(ctx, lf.ident));

            }
            case (vf: Ast::VirtualField) {
                Gen::internalError(ctx, "VirtualField support missing.");
            }
            case (pf: Ast::ProtoField) {
                Gen::internalError(ctx, "ProtoField support missing.");
            }
            }
        }
        //asm.I_newobject(fields.length);
        asm.I_getlocal(t);
        asm.killTemp(t);
    }

    function cgRegExpLiteral(ctx, re) {
        let {asm, cp} = ctx;
        let src = re.src.text;
        // src is "/.../flags"
        //
        // Note, ES4 semantics: recreate RE object every time.
        // FIXME: re-compiles the RE every time.
        let p = src.lastIndexOf('/');
        // FIXME: We don't want findpropstrict because it can be used to spoof RegExp.
        // But getglobalscope produces an object that does not have "RegExp" bound.
        asm.I_findpropstrict(ctx.emitter.RegExp_name);
        asm.I_pushstring(cp.stringUtf8(src.substring(1,p)));
        asm.I_pushstring(cp.stringUtf8(src.substring(p+1)));
        asm.I_constructprop(ctx.emitter.RegExp_name, 2);
    }

    let {asm, emitter} = ctx;
    switch type (e) {
    case (e:LiteralNull) { asm.I_pushnull() }
    case (e:LiteralUndefined) { asm.I_pushundefined() }
    case (e:LiteralInt) { 
        let val = e.intValue;
        if (val >= -128 && val < 128)
            asm.I_pushbyte(val & 0xFF);  // pushbyte sign-extends
        else
            asm.I_pushint(ctx.cp.int32(val));
    }
    case (e:LiteralUInt) { 
        asm.I_pushuint(ctx.cp.uint32(e.uintValue));
    }
    case (e:LiteralDouble) { 
        let val = e.doubleValue;
        if (isNaN(val))
            asm.I_pushnan();
        else
            asm.I_pushdouble(ctx.cp.float64(val));
    }
    case (e:LiteralDecimal) { 
        // FIXME: proper decimal support when the AVM can handle it!
        asm.I_pushdouble(ctx.cp.float64(parseFloat(e.decimalValue)));
    }
    case (e:LiteralString) {
        asm.I_pushstring(ctx.cp.symbolUtf8(e.strValue));
    }
    case (e:LiteralBoolean) {
        if (e.booleanValue)
            asm.I_pushtrue();
        else
            asm.I_pushfalse();
    }
    case (e:LiteralFunction) { 
        if (e.func.name != null) {
            // FIXME: correct for ES3 but not for ES4
            let t = asm.getTemp();
            asm.I_newobject(0);
            asm.I_dup();
            asm.I_setlocal(t);
            asm.I_pushwith();
            asm.I_newfunction(cgFunc(ctx, e.func));
            asm.I_dup();
            asm.I_getlocal(t);
            asm.I_swap();
            asm.I_setproperty(emitter.nameFromIdent(e.func.name.ident));
            asm.killTemp(t);
        }
        else
            asm.I_newfunction(cgFunc(ctx, e.func));
    }
    case (e:LiteralArray) { cgArrayInitializer(ctx, e) }
    case (e:LiteralObject) { cgObjectInitializer(ctx, e) }
    case (e:LiteralRegExp) { cgRegExpLiteral(ctx, e) }
    // case (e:LiteralNamespace) { cgNamespaceLiteral(ctx, e) }
    case (e:*) { 
        Gen::internalError(ctx, "Unimplemented LiteralExpr " + e);
    }
    }
}

function cgGetTempExpr(ctx, {n}) {
    let {asm, emitter} = ctx;
    let id = new Ast::Identifier(Token::intern("$t" + n), Ast::publicNSSL);
    cgFindPropStrict(ctx, id);
    cgGetProp(ctx, id);
}

function cgGetParamExpr({asm}, {n}) {
    asm.I_getlocal(n + 1);  //account for 'this'
}

function cgGetCogenTemp({asm}, {n}) {
    asm.I_getlocal(n);
}
    
function cgIdentExpr(ctx, e) {
    let {asm, emitter} = ctx;
    switch type(e) {
    case (id:Identifier) {
        return emitter.multiname(id,false);
    }
    case (ei:ComputedName) {
        cgExpr(ctx, ei.expr);
        return emitter.multinameL(Ast::publicNSSL,false);
    }
    case (qi:QualifiedIdentifier) { 
        switch type(qi.qual) {
        case( lr: Identifier ) {
            // Hack to deal with namespaces for now...
            // later we will have to implement a namespace lookup to resolve qualified typenames
            return emitter.qname(new Ast::Name(new Ast::UnforgeableNamespace(lr.ident), qi.ident), false);
        }
        case (lr: ForgeableNamespace) {
            return emitter.qname(new Ast::Name(lr, qi.ident), false);
        }
        case (lr: UnforgeableNamespace) {
            return emitter.qname(new Ast::Name(lr, qi.ident), false);
        }
        case( e:* ) {
            /// cgExpr(ctx, qi.qual);
            /// return emitter.rtqname(qi);
            Gen::internalError(ctx, "Unsupported form of qualified identifier " + qi);
        }
        }
    }
    case (x: PropName) {
        return emitter.fixtureNameToName(x);
    }
    case (x: TempName) {
        return emitter.fixtureNameToName(x);
    }
    case (x:*) { 
        Gen::internalError(ctx, "Unimplemented cgIdentExpr " + e);
    }
    }
}
    
function getIdentBinding(ctx, id: IdentExpr) {
    if (id is Identifier) {
        if( id.binding === undefined )
            id.binding = findBinding(ctx, id.ident, id.nss);
        return id.binding;
    }
    return Ast::nobind;
}

// Returns true if the result of the findprop is left on the stack
function cgFindPropStrict(ctx, id:IdentExpr)
    cgFindProp(ctx, id, true);

// Returns true if the result of the findprop is left on the stack
function cgFindProp(ctx, id:IdentExpr, is_strict = false) {
    let asm = ctx.asm;

    switch type ( getIdentBinding(ctx, id) ) {
    case ( rb : RegBind ) {
        // Do nothing, we'll get/set the value later with a register
        return false;
    }
    case ( sb : SlotBind ) {
        // Load the scope the slot is in
        asm.I_getlocal(sb.scope);
    }
    case ( e : * ) {
        if( is_strict )
            asm.I_findpropstrict(cgIdentExpr(ctx, id));
        else
            asm.I_findproperty(cgIdentExpr(ctx, id));
    }
    }
    return true;
}

function cgGetProp(ctx, id) {
    let asm = ctx.asm;

    switch type ( getIdentBinding(ctx, id) ) {
        case ( rb : RegBind ) {
            // Load the register
            asm.I_getlocal(rb.reg);
        }
        case ( sb : SlotBind ) {
            // Load the scope the slot is in
            if( sb.slot == -1)
                asm.I_getproperty(cgIdentExpr(ctx, id));
            else
                asm.I_getslot(sb.slot);
        }
        case ( e : * ) {
            asm.I_getproperty(cgIdentExpr(ctx, id));
        }
    }
}

function cgSetProp(ctx, id) {
    let asm = ctx.asm;

    switch type ( getIdentBinding(ctx, id) ) {
        case ( rb : RegBind ) {
            // Set the register
            if( rb.type_index != 0 )
                asm.I_coerce(rb.type_index);
            else
                asm.I_coerce_a();
            asm.I_setlocal(rb.reg);
        }
        case ( sb : SlotBind ) {
            // Load the scope the slot is in
            if( sb.slot == -1)
                asm.I_setproperty(cgIdentExpr(ctx, id));
            else
                asm.I_setslot(sb.slot);
        }
        case ( e : * ) {
            asm.I_setproperty(cgIdentExpr(ctx, id));
        }
    }
}
