/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Protocols:
   
   - the 'this' receiver object is passed as the first argument to any call, 
     the value may be null
   - local(0) contains the current 'this' object inside any function body,
     its value is always an object
   - inside class instance methods, the 'this' object is on the scope stack 
     outside the activation object
   - inside constructors, the 'this' object is on the scope stack in the 
     constructor body, also outside the activation object
   - inside other functions, the 'this' object is not on the scope stack
   - inside script top levels the 'this' object is on the scope stack; it
     should be the global object.
   - inside class initializers the 'this' object is on the scope stack; it
     is presumably the class object (but this remains to be verified).
*/

use default namespace Gen,
    namespace Gen;

use namespace Abc,
    namespace Asm,
    namespace Ast,
    namespace Emit,
    namespace Util;

/* A context is a structure with the following fields.
 *
 * In the object:
 *
 *    asm      -- the current function's assembler
 *    stk      -- the current function's binding stack (labels, ribs)
 *    target   -- the current trait target
 *
 * Invariant fields in CTX.prototype:
 *
 *    emitter  -- the unique emitter
 *    script   -- the only script we care about in that emitter
 *    cp       -- the emitter's constant pool
 *    filename -- the source file name (for error messages)
 *
 * Default fields in CTX.prototype that may be overridden in the object:
 *
 *    scope_reg -- register holding the activation/scope object (default 0 -- no register)
 *    push_this -- true iff reg0 should be pushed as first scope obj (default false)
 *    
 * FIXME, there are probably at least two targets: one for LET, another
 * for VAR/CONST/FUNCTION.
 *
 * Never construct a CTX by hand, always go through the push* functions
 * near the end of this file.
 */
dynamic class CTX 
{
    const asm, stk, target;

    function CTX (asm, stk, target) 
        : asm = asm
        , stk = stk
        , target = target
    { }
}

// Emit debug info or not, re-initialized by cg() below
var emit_debug = true;

// Turn on early binding or not
var early_binding = true;

Gen function internalError(ctx, msg) {
    Util::internalError(ctx.filename, ctx.lineno, msg);
}

Gen function syntaxError(ctx, msg) {
    Util::syntaxError(ctx.filename, ctx.lineno, msg);
}

/* Returns an ABCFile structure */
function cg(tree: PROGRAM) {
    emit_debug = ESC::flags.debugging;

    let e = new ABCEmitter;
    let s = e.newScript();

    CTX.prototype.emitter = e;
    CTX.prototype.script = s;
    CTX.prototype.cp = e.constants;
    CTX.prototype.filename = tree.file;
    CTX.prototype.scope_reg = 0;

    let asm = s.init.asm;
    let capture_reg = asm.getTemp();
    cgProgram(pushProgram(s, capture_reg), tree);
    asm.I_getlocal(capture_reg);
    asm.I_returnvalue();
    //print("iterations=" + (hits + misses) + "; hit ratio=" + hits/(hits + misses));
    //hits=misses=0;
    return e.finalize();
}

/* Support for "eval".  Compile 'tree' to a special script.
 *
 * The script is compiled as a top-level function with the name
 * given by "name", and the script stores it into ESC::eval_hook,
 * where the caller will find it (that's awful style, but it works
 * around a problem with the way name lookup is done in Tamarin,
 * namely, that a file loading a script can't seem to access names
 * bound by that script).
 *
 * Anyhow the new function will be passed an array of scope
 * objects, which it must push onto its scope stack (scopedesc
 * details how many elements, and how to push them).
 *
 * Upon entry, the function must bind the var and function
 * fixtures in 'tree' in the innermost scope object; these
 * properties are deletable.  Then it must run the program.  (No
 * other fixture types are allowed.)
 *
 * The function can't contain a "return" statement (because it was
 * parsed as a <Program>) but the statement result value is
 * captured and is return if the script terminates normally.
 */

function cgEval(tree: PROGRAM, name: Token::Tok, scopedesc: String) {

    function evalInits() {
        let d = [];
        for ( let i=0 ; i < scopedesc.length ; i++ )
            d.push(new Ast::EvalScopeInitExpr(i, scopedesc.charAt(i)));
        return d;
    }

    let attr = new Ast::FuncAttr(null);
    attr.capture_result = true;

    tree.head.fixtures.push
        ( new Ast::Fixture
          ( new Ast::PropName(new Ast::Name(Ast::publicNS, name)),
            new Ast::MethodFixture
            ( new Ast::Func
              ( new Ast::FuncName(Ast::ordinaryFunction, name),
                tree.body,
                new Ast::Head( [ new Ast::Fixture( new Ast::TempName(100000),
                                                   new Ast::ValFixture(Ast::anyType,false)) ],
                               evalInits() ),
                1,
                new Ast::Head([],[]),
                [],
                Ast::anyType,
                attr,
                true),
              Ast::anyType,
              false,
              false,
              false )) );

    return cg( new Ast::Program
               ( [ new Ast::ExprStmt
                   ( new Ast::SetExpr
                     ( Ast::assignOp,
                       new Ast::QualifiedIdentifier( new Ast::Identifier(Token::sym_ESC, Ast::publicNSSL), Token::sym_eval_hook ),
                       new Ast::Identifier(name, Ast::publicNSSL))) ],
                 tree.head,
                 tree.attr,
                 tree.file ));
}

function cgDebugFile(ctx) {
    let {asm, cp} = ctx;
    if( emit_debug && ctx.filename != null )
        asm.I_debugfile(cp.stringUtf8(ctx.filename));
}

function cgProgram(ctx, prog) {
    cgDebugFile(ctx);
    if (prog.head.fixtures != null)
        cgFixtures(ctx, prog.head.fixtures);
    cgStmts(ctx, prog.body);
}

function cgGetFixtureNameParts(ctx, fxname) {
    switch type (fxname) {
    case (pn:PropName) {
        return [pn.name.id, pn.name.ns];
    }
    case (tn:TempName) {
        return [Token::intern("$t"+tn.index), Ast::publicNS]
    }
    case (x:*) { 
        Gen::internalError(ctx, "Not a valid fixture name " + x); // FIXME source pos
    }
    }
}

function cgFixtures(ctx, fixtures) {
    let {target, asm, emitter} = ctx;
    let {slots, use_regs, scope_reg} = getVariableDefinitionScope(ctx);
    
    if( use_regs ) {
        for ( let i=0, limit=fixtures.length ; i < limit ; i++ ) {
            let {name:fxname, data:fx} = fixtures[i];
            let [name, ns] = cgGetFixtureNameParts(ctx, fxname);
            if( slots.getBinding(name, ns) != null )
                continue;  // This name already allocated
            let reg = asm.getTemp();
            let type_index = 0;
            if( fx is ValFixture )
                type_index = emitter.typeFromTypeExpr(fx.ty);
            slots.putBinding(name, ns, new RegBind(reg, type_index));
            
            asm.I_pushundefined();
            // init with the right type 
            // TODO: optimize so we don't emit this when it's not needed to satisy the verifier
            if( type_index != 0 )
                asm.I_coerce(type_index);
            else
                asm.I_coerce_a();
            
            asm.I_setlocal(reg);
        }
    }
    else {
        for ( let i=0, limit=fixtures.length ; i < limit ; i++ ) {
            let slot_id = -1;
            let {name:fxname, data:fx} = fixtures[i];
            let name = emitter.fixtureNameToName(fxname);

            if (fx is ValFixture) {
                if (checkTrait(fxname, name, TRAIT_Slot))
                    slot_id = target.addTrait(new ABCSlotTrait(name, 0, false, 0, emitter.typeFromTypeExpr(fx.ty), 0, 0));
                // FIXME when we have more general support for type annos
            }
            else if (fx is MethodFixture) {
                let methidx = cgFunc(ctx, fx.func);
                if (target is Method || target is Script) {
                    // Normal function definition
                    if (checkTrait(fxname, name, TRAIT_Slot))
                        slot_id = target.addTrait(new ABCSlotTrait(name, 0, false, 0, 0, 0, 0));
                    asm.I_findpropstrict(name);
                    asm.I_newfunction(methidx);
                    asm.I_setproperty(name);
                }
                else {
                    // target.addTrait(new ABCOtherTrait(name, 0, TRAIT_Method, 0, methidx));
                    let trait_kind;
                    if (fx.func.name.kind == getterFunction) {
                        trait_kind = TRAIT_Getter;
                        if (target.hasTrait(name, trait_kind))
                            dupTrait(fxname, trait_kind, trait_kind);
                    }
                    else if (fx.func.name.kind == setterFunction) {
                        trait_kind = TRAIT_Setter;
                        if (target.hasTrait(name, trait_kind))
                            dupTrait(fxname, trait_kind, trait_kind);
                    }
                    else {
                        trait_kind = TRAIT_Method;
                        checkTrait(fxname, name, trait_kind, true);
                    }
                    let methattrs = 0;
                    if (fx.isOverride)
                        methattrs |= ATTR_Override;
                    if (fx.isFinal)
                        methattrs |= ATTR_Final;
                    slot_id = target.addTrait(new ABCOtherTrait(name, methattrs, trait_kind, 0, methidx));
                }
            }
            else if (fx is ClassFixture) {
                checkTrait(fxname, name, TRAIT_Slot, true);
                let clsidx = cgClass(ctx, fx.cls);
                slot_id = target.addTrait(new ABCOtherTrait(name, 0, TRAIT_Class, 0, clsidx));
            }
            else if (fx is InterfaceFixture) {
                checkTrait(fxname, name, TRAIT_Slot, true);
                let ifaceidx = cgInterface(ctx, fx.iface);
                slot_id = target.addTrait(new ABCOtherTrait(name, 0, TRAIT_Class, 0, ifaceidx));
            }
            else if (fx is NamespaceFixture) {
                checkTrait(fxname, name, TRAIT_Slot, true);
                slot_id = target.addTrait(new ABCSlotTrait(name, 0, true, 0, 
                                                 emitter.qname(new Ast::Name(Ast::publicNS, Token::sym_Namespace),false), 
                                                 emitter.namespace(fx.ns), CONSTANT_Namespace));
            }
            else if (fx is TypeFixture) {
                //print ("warning: ignoring type fixture");
            }
            else {
                Gen::internalError(ctx, "Unhandled fixture type " + fx);
            }
            if(slot_id != -1 && slots ){
                //print("adding slot for ", fxname.name.id, "to", ctx.stk.tag, "slot_id ", slot_id);
                slots.putBinding(fxname.name.id, fxname.name.ns, new Ast::SlotBind(slot_id, scope_reg));
            }
        }
    }

    function checkTrait(fxname, name, kind, unique=false) {
        let [has_trait, trait_kind] = target.probeTrait(name);
        if (has_trait && (unique || trait_kind != kind))
            dupTrait(fxname, kind, trait_kind);
        return !has_trait;
    }

    function dupTrait(fxname, newkind, oldkind) {
        if (newkind != oldkind)
            Gen::syntaxError(ctx, "Incompatible fixture : name=" + fxname +
                             ", newkind=" + newkind + ", oldkind=" + oldkind );
        else
            Gen::syntaxError(ctx, "Duplicate fixture : name=" + fxname);
    }
}

function cgBlock(ctx, b) {
    // FIXME -- more here
    cgHead(ctx, b.head);
    cgStmts(ctx, b.stmts);
}

function cgStmts(ctx, stmts) {
    for ( let i=0, limit=stmts.length ; i < limit ; i++ )
        cgStmt(ctx, stmts[i]);
}

function extractNamedFixtures(fixtures) {
    let extracted = [];
    for(let i=0, limit=fixtures.length ; i < limit ; ++i)
        if (fixtures[i].name is PropName)
            extracted.push(fixtures[i]);
    return extracted;
}
    
function extractUnNamedFixtures(fixtures) {
    let extracted = [];
    for(let i=0, limit=fixtures.length; i < limit ; ++i) 
        if (fixtures[i].name is TempName)
            extracted.push(fixtures[i]);
    return extracted;
}

function cgTypeExpr(ctx, t) {
    if (t is Ast::TypeName)
        return cgIdentExpr(ctx, t.ident);
    Gen::internalError(this, "Can't deal with type applications in type expressions yet: " + t);
}

function cgClass(ctx, c) {
    let {asm, emitter, script} = ctx;
        
    let classname = emitter.qname(c.name,false);
    let basename = cgTypeExpr(ctx, c.baseName);
    let interfacenames = Util::map((function (n) cgTypeExpr(ctx, n)), c.interfaceNames);

    let flags = 0;
    if (!(c.isDynamic))
        flags |= CONSTANT_ClassSealed;
    if (c.isFinal)
        flags |= CONSTANT_ClassFinal;
    let cls = script.newClass(classname, basename, interfacenames, flags, c.protectedns);
        
    let class_ctx = pushClass(ctx, cls);

    // Static fixtures
    cgFixtures(class_ctx, c.classHead.fixtures);       // slots and methods and types and namespaces
    assert(c.classHead.exprs.length == 0);             // the initializers are all in the class body

    let inst = cls.getInstance();
        
    // Context for the instance
    let instance_ctx = pushInstance(ctx, inst);

    // do instance slots
    // FIXME instanceHead and instanceInits should be unified
    cgFixtures(instance_ctx, c.instanceHead.fixtures); // slots and methods

    inst.setIInit(cgCtor(instance_ctx, c.constructor, {fixtures:[], exprs:c.instanceHead.exprs}));

    let clsidx = cls.finalize();

    asm.I_findpropstrict(basename);
    asm.I_getproperty(basename);
    asm.I_dup();
    asm.I_pushscope();
    asm.I_newclass(clsidx);  // class object on tos
    asm.I_popscope();

    // cinit - init static fixtures
    // This doesn't look quite right but it does work -- for now
    asm.I_dup();
    asm.I_pushscope();
    let cinit = cls.getCInit();
    let cinit_ctx = pushCInit(ctx, cinit);  // shares asm with ctx
    cgDebugFile(cinit_ctx);
    cgStmts(cinit_ctx, c.classBody);
    asm.I_popscope();

    asm.I_getglobalscope();
    asm.I_swap();
    asm.I_initproperty(classname);

    return clsidx;
}

function cgInterface(ctx, c) {
    let {asm, emitter, script} = ctx;
        
    let ifacename = emitter.qname(c.name,false);
    let interfacenames = Util::map((function (n) cgTypeExpr(ctx,n)), c.interfaceNames);

    let iface = script.newInterface(ifacename, ctx.cp.symbolUtf8(c.name.id), interfacenames);
        
    let ifaceidx = iface.finalize();

    asm.I_getglobalscope();
    asm.I_pushnull();
    asm.I_newclass(ifaceidx);
    asm.I_initproperty(ifacename);

    return ifaceidx;
}
    
/*  
 *  Generate code for a ctor.
 */
function cgCtor(ctx, c, instanceInits) {
    let f = c.func;
    let formals_type = extractFormalTypes(ctx, f);
    let method = new Method(ctx.script.e, formals_type, ctx.cp.stringUtf8("$construct"), false, f.attr);

    let defaults = extractDefaultValues(ctx, f);
    if( defaults.length > 0 )
        method.setDefaults(defaults);
        
    if (!f.attr.is_native) {
        let asm = method.asm;
        let t = asm.getTemp();
        // FIXME: record that scopes must be restored here!
        let ctor_ctx = pushIInit(ctx, t, method);
       
        cgDebugFile(ctor_ctx);
        asm.I_getlocal(0);
        asm.I_dup();
        // Should this be instanceInits.inits only?
        asm.I_pushscope();  // This isn't quite right...
        for( let i = 0; i < instanceInits.length; i++ ) {
            cgExpr(ctor_ctx, instanceInits[i]);
            asm.I_pop();
        }
        cgHead(ctor_ctx, instanceInits);
        asm.I_popscope();

        // Create the activation object, and initialize params
        asm.I_newactivation();
        asm.I_dup();
        asm.I_setlocal(t);
        asm.I_dup();
        asm.I_pushwith();

        setupArguments(ctor_ctx, f);

        cgHead(ctor_ctx, f.params);

        for ( let i=0, limit=c.settings.length ; i < limit ; i++ ) {
            cgExpr(ctor_ctx, c.settings[i]);
            asm.I_pop();
        }

        // FIXME: the code below is now wrong.
        //
        // The logic for the super invocation needs to be as follows.
        //
        // If there is an explicit call to super (a SuperStmt or a
        // call from the settings) then:
        //
        //  - the function will have a local temp (the 'supercall
        //    flag') whose initial value is false.
        //
        //  - A super() call in the settings sets the flag after the
        //    super constructor returns.
        //
        //  - The SuperStmt checks the flag, which must be cleared or an 
        //    error will be thrown, and sets it after invoking the 
        //    constructor.  (See also comments in cogen-stmt.es.)
        //
        //  - When the constructor returns normally the flag is checked,
        //    and if it is cleared an error is thrown.
        //
        // If there is not an explicit call to super then a call to
        // the super constructor (with no arguments) is inserted at every
        // point of normal return from the constructor.
        //
        // If there is a call to super() from the settings then
        // superArgs is non-null.  If there is a SuperStmt in the body
        // then the function attribute uses_super is true.

        // Eval super args, and call super ctor
        asm.I_getlocal(0);
        let nargs = 0;
        if (c.superArgs != null) {
            nargs = c.superArgs.length;
            for ( let i=0 ; i < nargs ; i++ )
                cgExpr(ctor_ctx, c.superArgs[i]);
        }
        asm.I_constructsuper(nargs);
        
        asm.I_popscope();
        asm.I_getlocal(0);
        asm.I_pushscope();  //'this'
        asm.I_pushscope();  //'activation'
        
        cgHead(ctor_ctx, f.vars);

        cgStmts(ctor_ctx, f.body);
        
        asm.I_kill(t);
    }

    return method.finalize();
}

function extractFormalTypes({emitter}, f:Func)
    Util::map((function ({data}) emitter.fixtureTypeToType(data)), 
              extractUnNamedFixtures(f.params.fixtures));
        
function extractDefaultValues({emitter}, f:Func)
    Util::map((function (x) emitter.defaultExpr(x)), f.defaults);

/* Create a method trait in the ABCFile
 * Generate code for the function
 * Return the function index
 */
 
function cgFunc(ctx0, f:Func) {
    let {emitter, script, cp} = ctx0;
    let use_regs = early_binding && !f.attr.reify_activation;
    let fntype = ctx0.stk != null && (ctx0.stk.tag == "instance" || ctx0.stk.tag == "class")? "method" : "function";  // brittle as hell
    let formals_types = extractFormalTypes({emitter:emitter, script:script}, f);
    let name = null;
    if (f.name == null) {
        if (f.pos != 0 || f.filename != null)
            name = Token::intern("<anonymous " + (f.filename ? f.filename : "(unknown)") + ":" + f.pos + ">");
    }
    else
        name = f.name.ident;
    let method = new Method(emitter, formals_types, cp.symbolUtf8(name), fntype != "function", f.attr);

    let defaults = extractDefaultValues({emitter:emitter, script:script}, f);
    if( defaults.length > 0 )
        method.setDefaults(defaults);
        
    if (!f.attr.is_native) {
        let asm = method.asm;

        /* Create a new rib and populate it with the values of all the
         * formals.  Add slot traits for all the formals so that the
         * rib have all the necessary names.  Later code generation
         * will add properties for all local (hoisted) VAR, CONST, and
         * FUNCTION bindings, and they will be added to the rib too,
         * but not initialized here.  (That may have to change, for
         * FUNCTION bindings at least.)
         *
         * FIXME: if a local VAR shadows a formal, there's more
         * elaborate behavior here, and the compiler must perform some
         * analysis and avoid the shadowed formal here.
         */
            
        let scope_reg = -1; 
        if(!use_regs)
            scope_reg = asm.getTemp();
            
        let capture_reg = 0;

        if (f.attr.capture_result)
            capture_reg = asm.getTemp();

        let fnctx = pushFunction(ctx0, fntype, scope_reg, capture_reg, (fntype != "function"), use_regs, method);
        cgDebugFile(fnctx);
            
        if( !use_regs )
        {
            asm.I_newactivation();
            asm.I_dup();
            asm.I_setlocal(scope_reg);
            asm.I_pushscope();
        }
            
        setupArguments(fnctx, f);

        if( !use_regs )
            cgHead(fnctx, f.params);
        else 
            cgOptimizedParams(fnctx, f);
        cgHead(fnctx, f.vars);

        /* Generate code for the body.  If there is no return statement in the
         * code then the default behavior of the emitter is to add a returnvoid
         * at the end, so there's nothing to worry about here.
         */
        cgStmts(fnctx, f.body);
        if( !use_regs )
            asm.killTemp(scope_reg);
        if (capture_reg) {
            asm.I_getlocal(capture_reg);
            asm.I_returnvalue();
        }
    }
    return method.finalize();
}

function cgOptimizedParams(fnctx, f:Func) {
    let {asm, emitter} = fnctx;
    let params = f.params.fixtures;
    let named_params = [];
    param_count = 0;

    // Need to generate correct registers for parameters.  This gets tricky with destructuring in parameters
    // such as {a, b, c} - it comes in as 1 argument, so we must alloc and assign the correct values
    // for a, b, c.  The temp will already be allocated, so we have to alloc registers for a, b, c
    // This code depends on the fixtures coming in with the pattern:
    // PropName, TempName, PropName, TempName...  Regular params - no destructuring 
    // PropName, PropName, PropName, TempName...  Destructuring - 3 names will be extracted from the temp
    // This is what the fixtures for {a, b, c} look like.

    for( let i = 0, limit = params.length; i < limit; ++i ){
        let p = params[i];
        if( p.name is PropName )
            named_params.push(p);
        else if ( p.name is TempName ) {
            param_count++;
            if( named_params.length == 1 ) {
                let {name:fxname, data:fx} = named_params[0];
                let [name_part, ns_part] = cgGetFixtureNameParts(fnctx, fxname);
                let reg = param_count;
                let type_index = 0;
                if( fx is ValFixture )
                    type_index = emitter.typeFromTypeExpr(fx.ty);
                fnctx.stk.slots.putBinding(name_part, ns_part, new RegBind(reg, type_index));
            }
            else if( named_params.length > 1 ){
                // Some sort of destructuring assignment
                // need to alloc temps for the named params
                for( let q = 0; q < named_params.length; ++q ) {
                    let {name:fxname, data:fx} = named_params[q];
                    let [name_part, ns_part] = cgGetFixtureNameParts(fnctx, fxname);
                    let reg = asm.getTemp();
                    let type_index = 0;
                    if( fx is ValFixture )
                        type_index = emitter.typeFromTypeExpr(fx.ty);
                    fnctx.stk.slots.putBinding(name_part, ns_part, new RegBind(reg, type_index));
                }
                let [name_part, ns_part] = cgGetFixtureNameParts(fnctx, p.name);
                //trace("Adding binding for " + name_part + " at reg " + (param_count));
                fnctx.stk.slots.putBinding(name_part, ns_part, new RegBind(param_count, 0));
            }
            named_params = [];
        }
        else
            Gen::internalError(fnctx, "Expected PropName or TempName, got " + p.name);
    }
    for ( let i=0 ; i < f.params.exprs.length ; i++ ) {
        if( !redundantInitialization(fnctx, f.params.exprs[i]) ) {
            cgExpr(fnctx, f.params.exprs[i]);
            asm.I_pop();
        }
    }
}

function redundantInitialization(ctx, e){
    let ret = false;
    if(e is InitExpr && e.inits.length==1) {
        let {name, expr} = e.inits[0];
        if( name is PropName && expr is GetParam )
            // This is just setting the param to itself if locals are using registers
            ret = true;
    }
    return ret;
}

function setupArguments(ctx, f) {
    if (f.attr.uses_arguments) {
        // Create a fixture for "arguments", cgFixture ignores it if it's defined already.
        //
        // FIXME: what if the existing fixture is typed and won't admit the arguments object?
        // The rule for ES4 should probably be that if there is a typed "arguments" defined,
        // then no assignment will be generated here.
        //
        // Then initialize it.  It must be done first according to E262-3.

        cgFixtures(ctx, [new Ast::Fixture(new PropName(new Ast::Name(Ast::publicNS, Token::sym_arguments)), 
                                          new ValFixture(Ast::anyType, false))]);
        cgExpr(ctx, new Ast::SetExpr(Ast::assignOp, 
                                     new Ast::Identifier(Token::sym_arguments, Ast::publicNSSL), 
                                     new Ast::GetParam(f.numparams)));
        ctx.asm.I_pop();
    }
}
    
function cgHead(ctx, head) {
    let {asm, emitter, target} = ctx;
        
    let named_fixtures = extractNamedFixtures(head.fixtures);
    cgFixtures(ctx, named_fixtures);
    for ( let i=0, limit=head.exprs.length ; i < limit ; i++ ) {
        cgExpr(ctx, head.exprs[i]);
        asm.I_pop();
    }
}

function cgInits(ctx, inits, baseOnStk) {
    let {asm, emitter} = ctx;

    for( let i=0, limit=inits.length ; i < limit ; ++i ) {
        let {name, expr} = inits[i];
        let [n, ns] = cgGetFixtureNameParts(ctx, name);
                
        let bind = findBinding(ctx, n, new Ast::NamespaceSetList(new Ast::NamespaceSet(ns, null), null));

        if( baseOnStk ) {
            if(i < limit-1)
                asm.I_dup();
        }
        else {
            if( bind is SlotBind ){
                // Load the scope the slot is in
                asm.I_getlocal(bind.scope);
            }
            else if ( !(bind is RegBind) ) {
                asm.I_findproperty(emitter.fixtureNameToName(name));
            }
        }
            
        cgExpr(ctx, expr);
        if( bind is SlotBind ) {
            // Load the scope the slot is in
            if( bind.slot == -1)
                asm.I_setproperty(emitter.fixtureNameToName(name));
            else
                asm.I_setslot(bind.slot);
        }
        else if ( bind is RegBind ) {

            if( bind.type_index != 0 )
                asm.I_coerce(bind.type_index);
            else
                asm.I_coerce_a();

            // Set the register
            asm.I_setlocal(bind.reg);
        }
        else {
            asm.I_setproperty(emitter.fixtureNameToName(name));
        }
    }
    if( limit == 0 && baseOnStk )
        asm.I_pop();
}
    
function getVariableDefinitionScope(ctx) {
    var stk = ctx.stk;
    while( stk != null ) {
        if( stk.slots != undefined ) {
            return stk;
        }
        stk = stk.link;
    }
    return {slots:null, use_regs:false};
}

function findBinding({stk, asm}, name: IDENT, nssl: Ast::NamespaceSetList) {
    if (!early_binding)
        return Ast::nobind;

    while(stk != null) {
        let slots = stk.slots;
        let bind = null;
        if(stk.slots) {
            //print(stk.slots);
            for ( let tmp_nssl=nssl ; tmp_nssl != null ; tmp_nssl = tmp_nssl.link ) {
                for ( let nss = tmp_nssl.nsset; nss != null ; nss = nss.link ) {
                    let ns = slots.getBinding(name, nss.ns);
                    if (ns != null) {
                        if( bind == null )
                            bind = ns;
                        else if (ns != bind)
                            bind = Ast::nobind; // Ambiguous binding, punt for now, will show up at runtime
                    }
                }
            }
            if (bind != null) {
                //print("Found binding for " + name + " " + nssl + ": " + bind);
                return bind;
            }
        }
        if( stk.tag =="instance" ||
            (stk.tag == "function" && !stk.push_this) ||
            stk.tag=="with" || 
            stk.tag=="catch")
            break;  // Can't go past an instance, or a function without a 'this'
        stk = stk.link;
    }
    //print("Found NO binding for " + name + " " + nssl);
    return Ast::nobind;
}

// Handles scopes and finally handlers and returns a label, if appropriate, to
// branch to.  "tag" is one of "function", "break", "continue"

function unstructuredControlFlow(ctx, hit, jump, msg) {
    let {stk, asm} = ctx;
    while (stk != null) {
        if (hit(stk)) {
            if (jump)
                asm.I_jump(stk.branchTarget);
            return;
        }
        else {
            if(stk.scope_reg) {
                asm.I_popscope();
            }
            if (stk.tag == "finally") {
                /* The verifier can't deal with all these combinations, it appears to
                   be a limitation of how it does control flow analysis.  So throw
                   a SyntaxError here until the verifier can be fixed.
                   let myreturn = stk.nextReturn++;
                   asm.I_pushint(ctx.cp.int32(myreturn));
                   asm.I_setlocal(stk.returnreg);
                   asm.I_jump(stk.label);
                   stk.returnAddresses[myreturn] = asm.I_label(undefined);
                */
                Gen::internalError(ctx, "Limitation: Can't generate code for break/continue/return past 'finally' block.");
            }
        }
        stk = stk.link;
    }
    Gen::syntaxError(ctx, msg);
}

function restoreScopes(ctx) {
    let {stk, asm} = ctx;
    loop(stk);

    function loop(stk) {
        if (stk == null)
            return;
        if (stk.tag != "function")
            loop(stk.link);
        if (stk.push_this) {
            // function, script
            asm.I_getlocal(0);
            asm.I_pushscope();
        }
        if (stk.scope_reg) {
            asm.I_getlocal(stk.scope_reg);
            if (stk.tag == "with")
                asm.I_pushwith();
            else
                asm.I_pushscope();
        }
    }
}

// The following return extended contexts

function pushProgram(script, capture_reg)
    new CTX( script.init.asm, 
             { tag: "script", push_this: true, capture_reg: capture_reg, link: null }, 
             script );

function pushClass({asm, stk}, cls)
    new CTX( asm,
             { tag:"class", link: stk }, 
             cls );

function pushCInit({asm, stk}, cinit)
    new CTX( asm,
             { tag: "function", ty: "cinit", link: stk }, 
             cinit );

function pushInstance({asm, stk}, inst)
    new CTX( asm, 
             { tag:"instance", target: inst, link: stk },
             inst );

function pushIInit({stk}, scope_reg, iinit)
    new CTX( iinit.asm,
             { tag:"function", ty: "iinit", scope_reg: scope_reg, link: stk },
             iinit );

function pushFunction({stk}, function_type, scope_reg, capture_reg, push_this, use_regs, func)
    new CTX( func.asm, 
             { tag: "function", 
               ty: function_type, 
               scope_reg: scope_reg, 
               capture_reg: capture_reg,
               push_this: push_this,
               use_regs: use_regs,
               link: stk,
               slots: new Util::Names() },
               func );

function pushBreak({asm, stk, target}, branchTarget)
    new CTX( asm, 
             { tag:"break", label: null, branchTarget: branchTarget, link: stk }, 
             target );

function pushLabel({asm, stk, target}, label, branchTarget)
    new CTX( asm, 
             { tag:"break", label: label, branchTarget: branchTarget, link: stk }, 
             target );

function pushContinue({asm, stk, target}, labels, branchTarget)
    new CTX( asm, 
             { tag:"continue", labels: labels, branchTarget: branchTarget, link: stk },
             target );

function pushWith({asm, stk, target}, scope_reg)
    new CTX( asm,
             { tag:"with", scope_reg: scope_reg, link: stk },
             target );

function pushLet(ctx /*more*/) {
}

function pushCatch({asm, stk, target}, scope_reg )
    new CTX( asm, 
             { tag: "catch", scope_reg: scope_reg, link: stk },
             target );

function pushFinally({asm, stk, target}, label, returnreg)
    new CTX( asm,
             { tag: "finally", label: label, returnreg: returnreg, returnAddresses: new Array(), nextReturn: 0, link: stk },
             target );
