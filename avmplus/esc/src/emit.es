/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Rough sketch:

   The emitter provides abstractions for common code generation
   patterns, and some arbitrary amount of utilities to help in code
   generation.  The client starts by creating an emitter, then
   creating scripts on that emitter and classes and other traits on
   those scripts, methods on the classes, and so on.  Boilerplate code
   is inserted for you, so code is generated for class creation when
   you create a class on a script.

   These sketches are particularly rough right now because the needs
   of the code generator (verify.es) are not known precisely yet.  But
   I expect that there will be quite a bit of code in here, and it
   will encapsulate a lot of useful knowledge about how things are
   done on the AVM.

   One thing I'm apparently unresolved on here is when to create structures
   in the ABCFile; right now there's a mix of late and early.  Since the
   abcfile is not laid out until it is finalized this matters little, except
   for the sake of clarity.

   Sometimes this OO setup does not seem natural, other times it simplifies...
*/

use default namespace Emit,
    namespace Emit;

use namespace Abc,
    namespace Asm,
    namespace Ast, // goes away
    namespace Util;

class ABCEmitter
{
    var file, constants;
    /*private*/ var scripts = [];

    function ABCEmitter() {
        file = new ABCFile;
        constants = new ABCConstantPool;
        file.addConstants(constants);
        Object_name = nameFromIdent(Token::sym_Object);
        Array_name = nameFromIdent(Token::sym_Array);
        RegExp_name = nameFromIdent(Token::sym_RegExp);
    }

    function newScript(): Script {
        var s = new Script(this);
        scripts.push(s);
        return s;
    }

    function finalize() {
        function f(s) { s.finalize() }
        forEach(f, scripts);
        return file;
    }

    var Object_name;
    var Array_name;
    var RegExp_name;
    var meta_construct_name;

    /* AVM information.
     *
     * The "public public" namespace on the AVM, the one we wish to
     * map to ES4 "public", is a CONSTANT_PackageNamespace with a name
     * that is the empty string.
     *
     * The compiler inserts two definitions at the top of the file:
     *
     *   <magic> namespace internal = <magic>
     *   internal namespace public = <the real public>
     */
    function namespace( ns: Ast::Namespace) {
        switch type ( ns ) {
        case (pn: Ast::PrivateNamespace) {
            return constants.namespace(CONSTANT_PrivateNamespace, constants.symbolUtf8(pn.name));
        }
        case (pn: Ast::ProtectedNamespace) {
            return constants.namespace(CONSTANT_ProtectedNamespace, constants.symbolUtf8(pn.name));
        }
        case (pn: Ast::PublicNamespace) {
            return constants.namespace(CONSTANT_Namespace, constants.symbolUtf8(pn.name));
        }
        case (int_ns: Ast::InternalNamespace) {
            return constants.namespace(CONSTANT_PackageInternalNS, constants.symbolUtf8(int_ns.name));
        }
        case (un: Ast::ForgeableNamespace) {
            /// return constants.namespace(CONSTANT_ExplicitNamespace, constants.stringUtf8(pn.name));
            return constants.namespace(CONSTANT_Namespace, constants.symbolUtf8(un.name));
        }
        case (an: Ast::UnforgeableNamespace) {
            /// return constants.namespace(CONSTANT_PackageInternalNS, constants.stringUtf8(an.name));
            return constants.namespace(CONSTANT_Namespace, constants.symbolUtf8(an.name));
        }
        case (x:*) {
            internalError("", 0, "Unimplemented namespace " + ns);
        }
        }
    }

    // The hit ratio of this cache is normally above 95%.  It speeds
    // up the back end by more than a factor of two.  128 is pretty
    // random; a smaller number might work just as well.
    //
    // The reason it works so well is that the flattening of the
    // namespace sets together with hashing them and looking them up
    // to eliminate duplicates in the constant pool is quite
    // expensive.  Here we filter identical NamespaceSetLists so that
    // the constant pool doesn't have to work so hard.

    internal var cached_nssl = new Array(128);
    internal var cached_id = new Array(128);

    function flattenNamespaceSet(nssl: Ast::NamespaceSetList) {
        var new_nss = [];
        for ( ; nssl != null ; nssl = nssl.link )
            for ( let nss = nssl.nsset ; nss != null ; nss = nss.link )
                new_nss.push(this.namespace(nss.ns));
        return new_nss;
    }

    function namespaceSetList(nssl) {
        let h = nssl.hash & 127;
        if (nssl !== cached_nssl[h]) {
            cached_nssl[h] = nssl;
            cached_id[h] = constants.namespaceset(flattenNamespaceSet(nssl));
        }
        return cached_id[h];
    }

    function multiname(mname, is_attr) {
        let {nss, ident} = mname;
        return constants.Multiname(namespaceSetList(nss), constants.symbolUtf8(ident), is_attr);
    }

    function qname(qn, is_attr) {
        let {ns, id} = qn;
        return constants.QName(this.namespace(ns), constants.symbolUtf8(id), is_attr);
    }

    function nameFromIdent(id) {
        return constants.QName(constants.namespace(CONSTANT_PackageNamespace, constants.symbolUtf8(Token::sym_EMPTY)),
                               constants.symbolUtf8(id),false);
    }

    function multinameL(nss, is_attr)
        constants.MultinameL(namespaceSetList(nss), is_attr);

    // This is a limited version of cgIdentExpr -- several pieces are
    // just copies -- and all uses of this function should probably be replaced
    // by uses of the other.

    function nameFromIdentExpr(e) {
        switch type (e) {
        case (id: Ast::Identifier) { 
            return multiname(id,false);
        }
        case (qi: Ast::QualifiedIdentifier) { 
            switch type(qi.qual) {
            case( lr: Ast::Identifier ) {
                // FIXME: Hack to deal with namespaces for now.
                // later we will have to implement a namespace lookup to resolve qualified typenames
                return qname(new Ast::Name(new Ast::UnforgeableNamespace(lr.ident), qi.ident), false);
            }
            case (lr: Ast::ForgeableNamespace) {
                return qname(new Ast::Name(lr, qi.ident), false);
            }
            case (lr: Ast::UnforgeableNamespace) {
                return qname(new Ast::Name(lr, qi.ident), false);
            }
            case( e:* ) {
                internalError("", 0, "Unimplemented: nameFromIdentExpr " + e);
            }
            }
            return multiname(id,false);
        }
        case (x:*) { 
            internalError("", 0, "Unimplemented: nameFromIdentExpr " + e);
        }
        }
    }

    function rtqname({ident:ident}, is_attr)
        constants.RTQName(constants.symbolUtf8(ident), is_attr);

    function rtqnameL(is_attr)
        constants.RTQNameL(is_attr);

    function typeFromTypeExpr(t) {
        // not dealing with types for now
        switch type (t) {
        case (tn: Ast::TypeName) {
            switch type( tn.ident ){
            case(i: Ast::Identifier) {
                let name = i.ident;
                if( name==Token::sym_String || name==Token::sym_Number ||
                    name==Token::sym_Boolean || name==Token::sym_int ||
                    name==Token::sym_uint || name==Token::sym_Object ||
                    name==Token::sym_Array || name==Token::sym_Class ||
                    name==Token::sym_Function) {
                    return nameFromIdent(name);
                }
                else if( name==Token::sym_string ) {
                    return nameFromIdent(Token::sym_String);
                }
                else if( name==Token::sym_boolean ) {
                    return nameFromIdent(Token::sym_Boolean);
                }
                else {
                    //print ("warning: unknown type name " + t + ", using Object");
                    return nameFromIdent(Token::sym_Object);
                }
            }
            }
        }
        case (x:*) { 
            // print ("warning: Unimplemented: typeFromTypeExpr " + t + ", using *");
        }
        }
        return 0;
    }

    // Use this only for places that need a QName, only works with basic class names
    // as Tamarin doesn't support 
    function realTypeName(t) {
        // not dealing with types for now
        switch type (t) {
        case (tn: Ast::TypeName) {
            return nameFromIdentExpr(tn.ident);
        }
        case (st: Ast::SpecialType) {
            return 0;
        }
        case (x:*) { 
            internalError("", 0, "Unimplemented: realTypeName " + t + ", using *");
        }
        }
        return 0;
    }

    function fixtureNameToName(fn) {
        switch type (fn) {
        case (pn: Ast::PropName) {
            return qname(pn.name, false);
        }
        case (tn: Ast::TempName) {
            return qname (new Ast::Name(Ast::publicNS, Token::intern("$t"+tn.index)),false);  // FIXME allocate and access actual temps
        }
        case (x:*) { 
            internalError("", 0, "Not a valid fixture name " + x);
        }
        }
    }
        
    function fixtureTypeToType(fix) {
        switch type (fix) {
        case (vf: Ast::ValFixture) {
            return vf.ty != null ? typeFromTypeExpr(vf.ty) : 0 ;
        }
        case (mf: Ast::MethodFixture) {
            return 0;
        }
        case(x:*) { 
            internalError("", 0, "Unimplemented: fixtureTypeToType " + x);
        }
        }
    }
        
    function defaultLiteralExpr(lit)
    {
        switch type (lit) {
        case(ln: Ast::LiteralNull) {
            return {val:CONSTANT_Null, kind:CONSTANT_Null}
        }
        case(lu: Ast::LiteralUndefined) {
            return {val:0, kind:0}
        }
        case(ld: Ast::LiteralDouble) {
            let val = constants.float64(ld.doubleValue);
            return {val:val, kind:CONSTANT_Double};
        }
        case(ld: Ast::LiteralDecimal) {
            // FIXME: emit a decimal constant here when we support decimal.
            let val = constants.float64(ld.decimalValue);
            return {val:val, kind:CONSTANT_Double};
        }
        case(li: Ast::LiteralInt) {
            let val = constants.int32(li.intValue);
            return {val:val, kind:CONSTANT_Integer};
        }
        case(lu: Ast::LiteralUInt) {
            let val = constants.uint32(lu.uintValue);
            return {val:val, kind:CONSTANT_UInt};
        }
        case(lb: Ast::LiteralBoolean) {
            let val = (lb.booleanValue ? CONSTANT_True : CONSTANT_False);
            return {val:val, kind:val};
        }
        case(ls: Ast::LiteralString) {
            let val = constants.symbolUtf8(ls.strValue);
            return {val:val, kind:CONSTANT_Utf8};
        }
        case(ln: Ast::LiteralNamespace) {
            let val = constants.namespace(ln.namespaceValue);
            return {val:val, kind:CONSTANT_Namespace};
        }
        case(x:*) {
            syntaxError("", 0, "Default expression must be a constant value " + x); // FIXME: source pos
        }
        }
    }

    function defaultExpr(expr) {
        // FIXME: This outlaws ~0, -1, and so on.  ES4 default expression is a general expr.
        switch type (expr) {
        case(le: LiteralExpr) {
            return defaultLiteralExpr(le);
        }
        case(i: Ast::Identifier) {
            if( i.ident == Token::sym_undefined ) {
                // Handle defualt expr of (... arg = undefined ...)
                return defaultLiteralExpr(new Ast::LiteralUndefined());
            }
        }
        }
        syntaxError("", 0, "Default expression must be a constant value " + expr); // FIXME: source pos
    }
}

// Optimization?  A brute-force hints table that maps both name and
// (name ^ kind) to true, allowing us to avoid searching the traits
// table if the hints table does not have an entry for whatever we're
// looking for, reduces the amount of searching effectively.  But it
// does not improve running times very much, probably because most
// traits sets are small.  (In ESC the largest number of traits in a
// scope is in the assembler, but compiling the assembler with that
// kind of hints structure slows code generation down.)

class TraitsTable 
{
    var traits = [];

    // Here we probably want: newVar, newConst, ... instead?

    function addTrait(t)
        traits.push(t);

    function hasTrait(name, kind) {
        for (let i=0, limit=traits.length ; i < limit ; i++) {
            let t = traits[i];
            if(t.name == name && ((t.kind&15)==kind))
                return true;
        }
        return false;
    }

    function probeTrait(name) {
        for (let i=0, limit=traits.length ; i < limit ; i++) {
            let t = traits[i];
            if(t.name == name)
                return [true, t.kind & 15];
        }
        return [false, 0];
    }
}

class Script extends TraitsTable
{
    var e, init;

    function Script(e:ABCEmitter) {
        this.e = e;
        this.init = new Method(e,[], 0, true, new Ast::FuncAttr(null));
    }

    function newClass(name, basename, interfaces, flags, protectedns=null) {
        return new Emit::Class(this, name, basename, interfaces, flags, protectedns);
    }

    function newInterface(ifacename, methname, interfaces) {
        return new Emit::Interface(this, ifacename, methname, interfaces);
    }

    function addException(e) {
        return init.addException(e);
    }

    function finalize() {
        let id = init.finalize();
        let si = new ABCScriptInfo(id);
        for ( let i=0, limit=traits.length  ; i < limit ; i++ )
            si.addTrait(traits[i]);
        e.file.addScript(si);
    }
}
    
class Class extends TraitsTable
{
    var s, name, basename, instance=null, cinit, interfaces, flags, protectedns;

    function Class(script, name, basename, interfaces, flags, protectedns=null) {
        this.s = script;
        this.name = name;
        this.basename = basename;
        this.interfaces = interfaces;
        this.flags = flags;
        this.protectedns = protectedns;

        var asm = script.init;
    }

    function getCInit() {
        if(cinit == null )
            cinit = new Method(s.e, [], s.e.constants.stringUtf8("$cinit"), true, new Ast::FuncAttr(null));
        return cinit;
    }

    function getInstance() {
        if( this.instance == null )
            this.instance = new Instance(s, name, basename, interfaces, flags, protectedns);
            
        return this.instance;
    }
        
    function finalize() {
        let instidx = instance.finalize();
        let clsinfo = new ABCClassInfo();
        clsinfo.setCInit(getCInit().finalize());
        for(let i = 0, limit=traits.length ; i < limit ; ++i)
            clsinfo.addTrait(traits[i]);
            
        let clsidx = s.e.file.addClass(clsinfo);
            
        assert(clsidx == instidx);

        return clsidx;
    }
}
    
// The documentation has issues here.
//
// The way ASC generates code:
//   - the flags are ClassInterface|ClassSealed 
//   - the class init has a body that just executes "returnvoid"
//   - there is a method_info entry for the instance initializer 
//     but no corresponding method_body
//   - logic in cogen is responsible for generating global
//     code that performs newclass/initproperty

class Interface extends TraitsTable
{
    var script, ifacename, methname, interfaces;

    function Interface(script, ifacename, methname, interfaces) 
        : script=script
        , ifacename=ifacename
        , methname=methname
        , interfaces=interfaces 
    {
        assert(methname is Number);
    }

    function finalize() {
        var clsinfo = new ABCClassInfo();
        var methname_idx = script.e.constants.stringUtf8(String(methname));

        var iinit = new Instance(script, ifacename, 0, interfaces, CONSTANT_ClassInterface|CONSTANT_ClassSealed);
        var cinit = (new Method(script.e, [], methname_idx, false, new Ast::FuncAttr(null))).finalize();
        clsinfo.setCInit(cinit);
        for(let i = 0, limit=traits.length ; i < limit ; ++i)
            clsinfo.addTrait(traits[i]);

        var clsidx = script.e.file.addClass(clsinfo);
            
        var iinitm = new Method(script.e, [], methname_idx, false, new Ast::FuncAttr(null), true);
        iinit.setIInit(iinitm.finalize());
        iinit.finalize();

        return clsidx;

    }
}

class Instance extends TraitsTable
{
    var s, name, basename, flags, interfaces, iinit, protectedns;
        
    function Instance(s:Script, name, basename, interfaces, flags, protectedns=null) 
        : s=s
        , name=name
        , basename=basename 
        , interfaces=interfaces
        , flags=flags
        , protectedns=protectedns
    {
    }
        
    function setIInit(method) {
        iinit = method;
    }
        
    function finalize() {
        if (protectedns != null) {
            flags |= CONSTANT_ClassProtectedNs;
            pnsid = s.e.namespace(protectedns);
        }
        else
            pnsid = 0;
        var instinfo = new ABCInstanceInfo(name, basename, flags, pnsid, interfaces);
            
        instinfo.setIInit(iinit);
            
        for(let i = 0, limit=traits.length ; i < limit ; i++)
            instinfo.addTrait(traits[i]);
            
        return s.e.file.addInstance(instinfo);
    }
}

class Method extends TraitsTable // extends AVM2Assembler
{
    var e, formals, name, asm, finalized=false, defaults = null, exceptions=[], attr=null, bodyless;

    function Method(e:ABCEmitter, formals:Array, name, standardPrologue, attr, bodyless=false) {
        assert( name is Number && name < 1073741824);
        //super(e.constants, formals.length);
        this.formals = formals;
        this.e = e;
        this.name = name;
        this.attr = attr;
        this.bodyless = bodyless;

        if (!bodyless && !attr.is_native) {
            asm = new AVM2Assembler(e.constants, formals.length - (attr.uses_rest ? 1 : 0), attr);
            // Standard prologue -- but is this always right?
            // ctors don't need this - have a more complicated prologue
            if(standardPrologue) {
                asm.I_getlocal_0();
                asm.I_pushscope();
            }
        }
    }

    function setDefaults(d) {
        defaults = d;
    }

    function addException(e) {
        return exceptions.push(e)-1;
    }

    function finalize() {
        if (finalized)
            return;
        finalized = true;

        var flags = 0;

        if (!bodyless && !attr.is_native) {
            // Standard epilogue for lazy clients.
            asm.I_returnvoid();
            flags = asm.flags;
        } 
        else if (attr.is_native)
            flags = METHOD_Native;

        var meth = e.file.addMethod(new ABCMethodInfo(name, formals, 0, flags, defaults, null));
        if (!bodyless && !attr.is_native) {
            var body = new ABCMethodBodyInfo(meth);
            body.setMaxStack(asm.maxStack);
            body.setLocalCount(asm.maxLocal);
            body.setInitScopeDepth(0);
            body.setMaxScopeDepth(asm.maxScope);
            body.setCode(asm.finalize());
            for ( var i=0, limit=traits.length ; i < limit ; i++ )
                body.addTrait(traits[i]);
            
            for ( var i=0, limit=exceptions.length ; i < limit ; i++ )
                body.addException(exceptions[i]);
            
            e.file.addMethodBody(body);
        }

        return meth;
    }
}
