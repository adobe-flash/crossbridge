/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Ast,
    namespace Ast;

interface Serializable {
    function serialize(s);
}

class ASTNode {
    public function toString()
        (new Serializer(true)).serialize(this);
}

// BASIC TYPES

type IDENT = Token::Tok; // hm, how does this serialize??

class Head extends ASTNode implements Serializable {
    const fixtures: [...Fixture];
    const exprs: [...Expr];
    const cache = null;  // used by the definer, for the moment
    function Head (fixtures,exprs)
        : fixtures=fixtures
        , exprs=exprs {}

    function serialize(s)
        s.sClass(this, "Head", "fixtures", "exprs");
}

class Fixture extends ASTNode implements Serializable {
    const name: FixtureName;
    const data: FixtureData;
    function Fixture(name, data) : name=name, data=data {}

    function serialize(s)
        s.sClass(this, "Fixture", "name", "data");
}

interface FixtureName {
}

// FIXME: conflating "temps" and "positional parameters" seems wrong
// and is an unnecessary Tamarin dependency.  The parser uses TempName
// for "positional parameter" but will want to start using something
// for true unforgeable temporaries.

class TempName extends ASTNode implements FixtureName, Serializable {
    const index : int;
    function TempName (index) : index=index {}

    function serialize(s)
        s.sClass(this, "TempName", "index");
}

// FIXME: does this serve any purpose at all?  Strikes me as pure
// bureaucracy.  (It does allow FixtureName to be attached, but it
// seems that the real problem -- see comment above -- is that naming
// in general is not all that coherent atm.

class PropName extends ASTNode implements FixtureName, Serializable {
    const name: Name;
    function PropName(name) : name=name {}

    function serialize(s)
        s.sClass(this, "PropName", "name");
}

class InitBinding extends ASTNode implements Serializable {
    const name: FixtureName;
    const expr: Expr;
    function InitBinding(name, expr) : name=name, expr=expr {}

    function serialize(s)
        s.sClass(this, "InitBinding", "name", "expr");
}

class Name extends ASTNode implements Serializable {
    const ns;
    const id;
    function Name(ns, id) : ns=ns, id=id {}
     
    function serialize(s)
        s.sClass(this, "Name", "ns", "id");
}

// Must be qualified everywhere it's used as 'Ast::Namespace' because
// Tamarin does not (yet) have prioritized name lookup.

interface Namespace {
}

function nsEquals(ns1, ns2) {
    if (ns1 == ns2)
        return true;
    if (ns1.name != ns2.name)
        return false;
    switch type (ns1) {
    case (x: UnforgeableNamespace) { return ns2 is UnforgeableNamespace; }
    case (x: ForgeableNamespace) { return ns2 is UnforgeableNamespace; }
    case (x: PrivateNamespace) { return ns2 is PrivateNamespace; }
    case (x: ProtectedNamespace) { return ns2 is ProtectedNamespace; }
    case (x: PublicNamespace) { return ns2 is PublicNamespace; }
    case (x: InternalNamespace) { return ns2 is InternalNamespace; }
    case (x: *) { return ns1.hash() == ns2.hash(); }
    }
}

class PrivateNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function PrivateNamespace (name)
        : name = name { }
    function hash () { return "private " + name; }

    function serialize(s)
        s.sClass(this, "PrivateNamespace", "name");
}

class ProtectedNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function ProtectedNamespace (name)
        : name = name { }
    function hash () { return "protected " + name; }

    function serialize(s)
        s.sClass(this, "ProtectedNamespace", "name");
}

class PublicNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function PublicNamespace (name)
        : name = name { }
    function hash () { return "public " + name; }

    function serialize(s)
        s.sClass(this, "PublicNamespace", "name");
}

class InternalNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function InternalNamespace (name)
        : name = name { }
    function hash () { return "internal " + name; }

    function serialize(s)
        s.sClass(this, "InternalNamespace", "name");
}

class ForgeableNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function ForgeableNamespace (name)
        : name = name { }
    function hash () { return "forgeable " + name; }

    function serialize(s)
        s.sClass(this, "ForgeableNamespace", "name");
}

class UnforgeableNamespace extends ASTNode implements Ast::Namespace, Serializable {
    const name : IDENT;
    function UnforgeableNamespace (name)
        : name = name { }
    function hash () { return "unforgeable " + name; }

    function serialize(s)
        s.sClass(this, "UnforgeableNamespace", "name");
}

internal var nshash = 0;  // Used as an object ID for namespace nodes in the back end

// The correctness of optimizations in the back-end depends on
// NamespaceSet and NamespaceSetList being functional data structures.

final class NamespaceSet extends ASTNode implements Serializable {
    const ns;
    const link: NamespaceSet;
    const hash = nshash++;
    function NamespaceSet(ns, link) : ns=ns, link=link {}

    function serialize(s)
        s.sClass(this, "NamespaceSet", "ns", "link");
}

final class NamespaceSetList extends ASTNode implements Serializable {
    const nsset: NamespaceSet;
    const link : NamespaceSetList;
    const hash = nshash++;
    function NamespaceSetList(nsset, link) : nsset=nsset, link=link {}

    function serialize(s)
        s.sClass(this, "NamespaceSetList", "nsset", "link");

    function pushScope()
        new NamespaceSetList(null, this);

    function pushNamespace(ns)
        new NamespaceSetList(new NamespaceSet(ns, nsset), this.link);
}

const publicNS = new Ast::PublicNamespace(Token::sym_EMPTY);
const publicNSSL = new Ast::NamespaceSetList(new Ast::NamespaceSet(publicNS, null), null);

// Binary type operators

type BINTYOP = int;

const castOp = 0;
const isOp = 1;
const likeOp = 2;

// Binary arithmetic and logical operators

type BINOP = int;

const plusOp = 0;
const minusOp = 1;
const timesOp = 2;
const divideOp = 3;
const remainderOp = 4;
const leftShiftOp = 5;
const rightShiftOp = 6;
const rightShiftUnsignedOp = 7;
const bitwiseAndOp = 8;
const bitwiseOrOp = 9;
const bitwiseXorOp = 10;
const logicalAndOp = 11;
const logicalOrOp = 12;
const instanceOfOp = 13;
const inOp = 14;
const equalOp = 15;
const notEqualOp = 16;
const strictEqualOp = 17;
const strictNotEqualOp = 18;
const lessOp = 19;
const lessOrEqualOp = 20;
const greaterOp = 21;
const greaterOrEqualOp = 22;
const commaOp = 23;

// Assignment operators

type ASSIGNOP = int;

const assignOp = 0;
const assignPlusOp = 1;
const assignMinusOp = 2;
const assignTimesOp = 3;
const assignDivideOp = 4;
const assignRemainderOp = 5;
const assignLeftShiftOp = 6;
const assignRightShiftOp = 7;
const assignRightShiftUnsignedOp = 8;
const assignBitwiseAndOp = 9;
const assignBitwiseOrOp = 10;
const assignBitwiseXorOp = 11;
const assignLogicalAndOp = 12;
const assignLogicalOrOp = 13;

// Unary arithmetic and logical operators

type UNOP = int;

const deleteOp = 0;
const voidOp = 1;
const typeOfOp = 2;
const preIncrOp = 3;
const preDecrOp = 4;
const postIncrOp = 5;
const postDecrOp = 6;
const unaryPlusOp = 7;
const unaryMinusOp = 8;
const bitwiseNotOp = 9;
const logicalNotOp = 10;
const typeOp = 11;
const spreadOp = 12;

// The strictFlag is set on ops that generated in strict mode.

const strictFlag = 64;
const strictMask = strictFlag-1;

class Expr extends ASTNode {
    const pos: int;
    function Expr(pos=0) : pos=pos {}
}

class TernaryExpr extends Expr implements Serializable {
    const e1 : Expr;
    const e2 : Expr;
    const e3 : Expr;
    function TernaryExpr (e1,e2,e3) : e1=e1, e2=e2, e3=e3 {}

    function serialize(s)
        s.sClass(this, "TernaryExpr", "e1", "e2", "e3");
}

class BinaryExpr extends Expr implements Serializable {
    const op : BINOP;
    const e1 : Expr;
    const e2 : Expr;
    function BinaryExpr (op,e1,e2) : op=op, e1=e1, e2=e2 {}

    function serialize(s)
        s.sClass(this, "BinaryExpr", "op", "e1", "e2");
}

class BinaryTypeExpr extends Expr implements Serializable {
    const op : BINTYOP;
    const e1 : Expr;
    const e2 : TypeExpr;
    function BinaryTypeExpr (op,e1,e2) : op=op, e1=e1, e2=e2 {}

    function serialize(s)
        s.sClass(this, "BinaryTypeExpr", "op", "e1", "e2");
}

class UnaryExpr extends Expr implements Serializable {
    const op : UNOP;
    const e1 : Expr;
    function UnaryExpr (op,e1) : op=op, e1=e1 {}

    function serialize(s)
        s.sClass(this, "UnaryExpr", "op", "e1");
}

// FIXME: ex => expr

class TypeOpExpr extends Expr implements Serializable {
    const ex : TypeExpr;
    function TypeOpExpr (ex) : ex=ex {}

    function serialize(s)
        s.sClass(this, "TypeOpExpr", "ex");
}

class ThisExpr extends Expr implements Serializable {
    const strict: Boolean;
    function ThisExpr(strict) : strict=strict {}

    function serialize(s)
        s.sClass(this, "ThisExpr", "strict");
}

class ThisGeneratorExpr extends Expr implements Serializable {
    function serialize(s)
        s.sClass(this, "ThisGeneratorExpr");
}

class ThisFunctionExpr extends Expr implements Serializable {
    function serialize(s)
        s.sClass(this, "ThisFunctionExpr");
}

// FIXME: ex => expr
class YieldExpr extends Expr implements Serializable {
    const ex : ? Expr;
    function YieldExpr (ex=null) : ex=ex {}

    function serialize(s)
        s.sClass(this, "YieldExpr", "ex");
}

// FIXME: ex => expr
class SuperExpr extends Expr implements Serializable {
    const ex : ? Expr;
    function SuperExpr (ex=null) : ex=ex {}

    function serialize(s)
        s.sClass(this, "SuperExpr", "ex");
}

class CallExpr extends Expr implements Serializable {
    const expr : Expr;
    const args : [...Expr];
    const spread: ? Expr;
    const strict: Boolean;
    function CallExpr (expr,args,spread,pos=0,strict=false)
        : expr=expr
        , args=args
        , spread=spread
        , strict=strict
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "CallExpr", "expr", "args", "spread", "pos", "strict");
}

class ApplyTypeExpr extends Expr implements Serializable {
    const expr : Expr;
    const args : [...TypeExpr];
    function ApplyTypeExpr (expr,args)
        : expr=expr
        , args=args {}

    function serialize(s)
        s.sClass(this, "ApplyTypeExpr", "expr", "args");
}

class LetExpr extends Expr implements Serializable {
    const head : Head;
    const expr : Expr;
    function LetExpr (head,expr)
        : head = head
        , expr = expr {}

    function serialize(s)
        s.sClass(this, "LetExpr", "head", "expr");
}

class DynamicOverrideExpr extends Expr implements Serializable {
    const names: [...IdentExpr];
    const exprs: [...Expr];
    const body : Expr;
    function DynamicOverrideExpr (names, exprs, body)
        : names=names
        , exprs=exprs
        , body=body {}

    function serialize(s)
        s.sClass(this, "DynamicOverrideExpr", "names", "exprs", "body");
}

class NewExpr extends Expr implements Serializable {
    const expr : Expr;
    const args : [...Expr];
    const spread: ? Expr;
    function NewExpr (expr,args,spread)
        : expr = expr
        , args = args
        , spread = spread {}

    function serialize(s)
        s.sClass(this, "NewExpr", "expr", "args", "spread");
}

class ObjectRef extends Expr implements Serializable {
    const base  : Expr;
    const ident : (IdentExpr | ComputedName);
    function ObjectRef (base,ident,pos=0)
        : base = base
        , ident = ident
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "ObjectRef", "base", "ident", "pos");
}

// This is used to encode obj[E] because that whole expression is
// encoded as an ObjectRef; the "ident" in ObjectRef (the 'E' above)
// turns out to be a ComputedName.  Note that E cannot be a slice
// expression; the pattern obj[E1:E2:E3] is translated into a call
// expression.

class ComputedName extends Expr implements Serializable {
    const expr: Expr;
    function ComputedName (expr)
        : expr=expr { }

    function serialize(s)
        s.sClass(this, "ComputedName", "expr");
}

// FIXME: le? re?
class SetExpr extends Expr implements Serializable {
    const op : ASSIGNOP;
    const le : Expr;
    const re : Expr;
    function SetExpr (op,le,re,pos=0)
        : op=op
        , le=le
        , re=re 
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "SetExpr", "op", "le", "re");
}

class EvalScopeInitExpr extends Expr implements Serializable {
    const index: int;
    const how: String;
    function EvalScopeInitExpr(index, how)
        : index=index
        , how=how
    {}

    function serialize(s)
        s.sClass(this, "EvalScopeInitExpr", "index", "how");
}

interface Comprehension {
}

class ComprehendIf extends Expr implements Comprehension, Serializable {
    const condition: Expr;
    const subclause: ? Expr;
    function ComprehendIf(condition, subclause) 
        : condition=condition
        , subclause=subclause {}
        
    function serialize(s)
        s.sClass(this, "ComprehendIf", "condition", "subclause");
}

class ComprehendLet extends Expr implements Comprehension, Serializable {
    const head: Head;
    const subclause: ? Expr;
    function ComprehendLet(head, subclause) 
        : head=head
        , subclause=subclause {}
        
    function serialize(s)
        s.sClass(this, "ComprehendLet", "head", "subclause");
}

class ComprehendFor extends Expr implements Comprehension, Serializable {
    const is_each: Boolean;
    const head: Head;
    const iterator: Expr;
    const subclause: ? Expr;
    function ComprehendFor(is_each, head, iterator, subclause) 
        : is_each=is_each
        , head=head
        , iterator=iterator
        , subclause=subclause {}
        
    function serialize(s)
        s.sClass(this, "ComprehendFor", "is_each", "head", "iterator", "subclause");
}

interface Bind {}
    
// Couldn't resolve to any scope/reg
class NoBind implements Bind {
}

const nobind = new NoBind;

class RegBind implements Bind {
    const reg;
    const type_index;
    function RegBind(reg, type_index)
        : reg = reg
        , type_index = type_index { }
}
 
class SlotBind implements Bind {
    const slot;   // slot id
    const scope;  // Register the scope is in.  Could scope ever not be in a register?
    function SlotBind(slot, scope)
        : slot = slot
        , scope = scope { }
}

// FIXME: better as an ENUM, but then that needs to be serializable.

type INIT_TARGET = int;

const varInit = 0;
const letInit = 1;
const prototypeInit = 2;
const instanceInit = 3;

class InitExpr extends Expr implements Serializable {
    const target : INIT_TARGET;
    const head : Head;               // for desugaring temporaries
    const inits : [...InitBinding];
    function InitExpr (target, head, inits)
        : target = target
        , head = head
        , inits = inits {}

    function serialize(s)
        s.sClass(this, "InitExpr", "target", "head", "inits");
}

class GetTemp extends Expr implements Serializable {
    const n : int;
    function GetTemp (n)
        : n = n {}

    function serialize(s)
        s.sClass(this, "GetTemp", "n");
}

class GetParam extends Expr implements Serializable {
    const n : int;
    function GetParam (n) 
        : n = n {}

    function serialize(s)
        s.sClass(this, "GetParam", "n");
}

class GetCogenTemp extends Expr implements Serializable {
    const n;   // set by the code generator
    function GetCogenTemp() {}

    function serialize(s)
        s.sClass(this, "GetCogenTemp");
}

interface IdentExpr {
}

// Values in nss are unresolved expressions until definition time, but
// the parser also places literal namespace values in the list when it
// needs to.

class Identifier extends Expr implements IdentExpr, Serializable {
    const ident : IDENT;
    const nss: [...(Ast::Namespace | Expr)];
    var binding;
    function Identifier (ident,nss,pos=0)
        : ident = ident
        , nss = nss
        , binding = undefined
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "Identifier", "ident", "nss");
}

class QualifiedIdentifier extends Expr implements IdentExpr, Serializable {
    const qual  : (Ast::Namespace | Expr);
    const ident : IDENT;
    function QualifiedIdentifier (qual,ident,pos=0)
        : qual=qual
        , ident=ident
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "QualifiedIdentifier", "qual", "ident");
}

interface LiteralExpr {
}

class LiteralNull extends Expr implements LiteralExpr, Serializable {
    function LiteralNull(pos=0)
        : super(pos) { }
    
    function serialize(s)
        s.sClass(this, "LiteralNull");
}

class LiteralUndefined extends Expr implements LiteralExpr, Serializable {
    function LiteralUndefined(pos=0)
        : super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralUndefined");
}

class LiteralDouble extends Expr implements LiteralExpr, Serializable {
    const doubleValue : Number;
    function LiteralDouble (doubleValue, pos=0)
        : doubleValue=doubleValue
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralDouble", "doubleValue");
}

class LiteralDecimal extends Expr implements LiteralExpr, Serializable {
    const decimalValue : decimal;
    function LiteralDecimal (decimalValue, pos=0)
        : decimalValue = decimalValue
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralDouble", "decimalValue");
}

class LiteralInt extends Expr implements LiteralExpr, Serializable {
    const intValue : int;
    function LiteralInt(intValue, pos=0) 
        : intValue=intValue
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralInt", "intValue");
}

class LiteralUInt extends Expr implements LiteralExpr, Serializable {
    const uintValue : uint;
    function LiteralUInt(uintValue, pos=0) 
        : uintValue=uintValue
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralUInt", "uintValue");
}

class LiteralBoolean extends Expr implements LiteralExpr, Serializable {
    const booleanValue : Boolean;
    function LiteralBoolean(booleanValue, pos=0) 
        : booleanValue=booleanValue
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralBoolean", "booleanValue");
}

class LiteralString extends Expr implements LiteralExpr, Serializable {
    const strValue : Token::Tok;
    function LiteralString (strValue, pos=0)
        : strValue = strValue
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralString", "strValue");
}

class LiteralArray extends Expr implements LiteralExpr, Serializable {
    const exprs : [...Expr];
    const spread : ? Expr;
    const ty : TypeExpr;
    function LiteralArray (exprs, spread, ty, pos=0)
        : exprs = exprs
        , spread = spread
        , ty = ty
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralArray", "exprs", "ty");
}

class LiteralComprehension extends Expr implements LiteralExpr, Serializable {
    const expr : Expr;
    const comprehension : Comprehension;
    const ty : TypeExpr;
    function LiteralComprehension (expr, comprehension, ty, pos=0)
        : expr = expr
        , comprehension = comprehension
        , ty = ty
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralComprehension", "expr", "comprehension", "ty");
}

class LiteralNamespace extends Expr implements LiteralExpr, Serializable {
    const namespaceValue : Ast::Namespace;
    function LiteralNamespace (namespaceValue, pos=0)
        : namespaceValue = namespaceValue 
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralNamespace", "namespaceValue");
}

class LiteralObject extends Expr implements LiteralExpr, Serializable {
    const fields : [...LiteralField];
    const ty : TypeExpr;
    function LiteralObject (fields, ty, pos=0)
        : fields = fields
        , ty = ty 
        , super(pos) { }

    function serialize(s)
        s.sClass(this, "LiteralObject", "fields", "ty");
}
    
class LiteralField extends ASTNode implements Serializable {
    const kind:  VAR_DEFN_TAG;
    const ident: IdentExpr;
    const expr:  ? Expr;
    function LiteralField (kind,ident,expr)
        : kind = kind
        , ident = ident
        , expr = expr {}

    function serialize(s)
        s.sClass(this, "LiteralField", "kind", "ident", "expr");
}

class ProtoField extends ASTNode implements Serializable {
    const expr : Expr;
    function ProtoField(expr) : expr=expr {}

    function serialize(s)
        s.sClass(this, "ProtoField", "expr");
}

class VirtualField extends ASTNode implements Serializable {
    const tag  : VAR_DEFN_TAG;
    const name : IdentExpr;
    const kind : FUNC_NAME_KIND;
    const func : Func;
    function VirtualField(tag, name, kind, func)
        : tag=tag
        , name=name
        , kind=kind
        , func=func {}

    function serialize(s)
        s.sClass(this, "VirtualField", "tag", "name", "kind", "func");

}

class LiteralFunction extends Expr implements LiteralExpr, Serializable {
    const func : Func;
    function LiteralFunction (func, pos=0)
        : func = func
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralFunction", "func");
}

class LiteralRegExp extends Expr implements LiteralExpr, Serializable {
    const src : Token::Tok;
    function LiteralRegExp(src, pos=0)
        : src=src
        , super(pos) {}

    function serialize(s)
        s.sClass(this, "LiteralRegExp", "src");
}

type VAR_DEFN_TAG = int;

const noTag = 0;     // dynamic property
const constTag = 1;  // 'const' fixture or parameter
const varTag = 2;    // 'var' fixture or parameter

/*  Not used
class VariableDefn extends ASTNode implements Serializable {
    const ns: Namespace;
    const isStatic: Boolean;
    const isPrototype: Boolean;
    const kind: VAR_DEFN_TAG;
    const bindings: BINDING_INITS;
    function VariableDefn (ns,isStatic,isPrototype,kind,bindings)
        : ns = ns
        , isStatic = isStatic
        , isPrototype = isPrototype
        , kind = kind
        , bindings = bindings {}

    function serialize(s)
        s.sClass(this, "VariableDefn", "ns", "isStatic", "isPrototype", "kind", "bindings");
}
*/

class Cls extends ASTNode implements Serializable {
    const name: Name;
    const typeParams: [...TypeExpr];
    const nonnullable: Boolean;
    const baseName: IdentExpr;
    const interfaceNames: [...IdentExpr];
    const protectedns;
    const constructor : Ctor;
    const classHead: Head;
    const instanceHead: Head;
    const classType: ObjectType;
    const instanceType: InstanceType;
    const classBody: [...Stmt];
    const isDynamic;
    const isFinal;
    function Cls (name,typeParams,nonnullable,baseName,interfaceNames,protectedns,
                  constructor,classHead,instanceHead,classType,instanceType,classBody,
                  isDynamic,isFinal)
        : name = name
        , typeParams = typeParams
        , nonnullable = nonnullable
        , baseName = baseName
        , interfaceNames = interfaceNames
        , protectedns = protectedns
        , constructor = constructor
        , classHead = classHead
        , instanceHead = instanceHead
        , classType = classType
        , instanceType = instanceType
        , classBody = classBody
        , isDynamic = isDynamic
        , isFinal = isFinal
    {}

    function serialize(s)
        s.sClass(this, "Cls", 
                 "name", "typeParams", "nonnullable", "baseName", "interfaceNames", "protectedns", 
                 "constructor", "classHead", "instanceHead", "classType", "instanceType", "classBody",
                 "isDynamic", "isFinal");
}

class Interface extends ASTNode implements Serializable {
    const name: Name;
    const typeParams: [...TypeExpr];
    const interfaceNames: [...IdentExpr];
    const instanceHead: Head;
    function Interface (name,typeParams,interfaceNames,instanceHead)
        : name = name
        , typeParams = typeParams
        , interfaceNames = interfaceNames
        , instanceHead = instanceHead
    {}

    function serialize(s)
        s.sClass(this, "Interface", "typeParams", "name", "interfaceNames", "instanceHead");
}

// Functions

// FIXME: 'TAG' is better here, see above uses.
type FUNC_NAME_KIND = int;

const ordinaryFunction = 0;
const getterFunction = 1;
const setterFunction = 2;
const staticInitFunction = 3;

class FuncName extends ASTNode implements Serializable {
    const kind: FUNC_NAME_KIND;
    const ident: IDENT;
    function FuncName(kind, ident) : kind=kind, ident=ident {}

    function serialize(s)
        s.sClass(this, "FuncName", "kind", "ident");
}

class FuncAttr extends ASTNode implements Serializable {
    /* Outer function, or null if the function is at the global
       level (including for class methods). */
    const parent: FuncAttr;

    /* Nested functions and function expressions, empty for leaf functions */
    const children;

    /* True iff identifier "arguments" lexically referenced in function body.
       Note that the parameter list is excluded. */
    var uses_arguments = false;
        
    /* True iff identifier expression "eval" is lexically referenced 
       in the function body as the operator in a call expression. */
    var uses_eval = false;

    /* True iff ...<id> appears in the parameter list. */
    var uses_rest = false;
        
    /* True iff the body has a "with" statement */
    var uses_with = false;

    /* True iff the body has a "try" statement with a "catch" clause */
    var uses_catch = false;

    /* True iff the body has a "try" statement with a "finally" clause */
    var uses_finally = false;

    /* True iff the body has a "yield" statement or expression */
    var uses_yield = false;

    /* True iff this is a constructor whose body or settings has an
       explicit "super" call */
    var uses_super = false;

    /* True iff this function is native */
    var is_native = false;

    /* True iff the function must capture its statement result value and return it if 
       control falls off the end */
    var capture_result = false;

    /* Synthesized: true iff activation object must be reified for any reason */
    var reify_activation = false;

    function FuncAttr(...rest) {
        if (rest.length == 1) {
            this.parent = rest[0];
            this.children = [];
        }
        else {
            // Unserialization.  An ad hoc pass over the AST will (hopefully) take 
            // care of patching up parent/children.
            [uses_arguments, uses_eval, uses_rest, uses_with, uses_catch, 
             uses_finally, uses_yield, uses_super, is_native, capture_result, 
             reify_activation] = rest;
        }
    }

    function serialize(s)
        s.sClass(this, "FuncAttr", "uses_arguments", "uses_eval", "uses_rest", "uses_with",
                 "uses_catch", "uses_finally", "uses_yield", "uses_super", "is_native", 
                 "capture_result", "reify_activation");
}

class Func extends ASTNode implements Serializable {
    const name; //: FUNC_NAME;
    const body: [...Stmt];
    const params: Head;
    const numparams: int;
    const vars: Head;
    const defaults: [...Expr];
    const ty: TypeExpr;
    const attr: FuncAttr;
    const strict: Boolean;
    const pos;
    const filename;
    function Func (name,body,params,numparams,vars,defaults,ty,attr,strict,pos=0,filename=null)
        : name = name
        , body = body
        , params = params
        , numparams = numparams
        , vars = vars
        , defaults = defaults
        , ty = ty
        , attr = attr 
        , strict = strict
        , pos = pos
        , filename = filename {}

    function serialize(s)
        s.sClass(this, "Func", "name", "body", "params", "numparams", "vars", "defaults", "ty", "attr", "strict", "pos", "filename");
}

class Ctor extends ASTNode implements Serializable {
    const settings : [...Expr];
    const superArgs : ? [...Expr];   // Will be null if there is no 'super' call in the settings
    const superSpread: ? Expr
    const func : Func;
    function Ctor (settings,superArgs,superSpread,func)
        : settings = settings
        , superArgs = superArgs
        , superSpread = superSpread
        , func = func {}

    function serialize(s)
        s.sClass(this, "Ctor", "settings", "superArgs", "superSpread", "func");
}

interface BindingIdent {
}

class TempIdent extends ASTNode implements BindingIdent, Serializable {
    const index : int;
    function TempIdent (index)
        : index = index {}

    function serialize(s)
        s.sClass(this, "TempIdent", "index");
}

class ParamIdent extends ASTNode implements BindingIdent, Serializable {
    const index : int;
    function ParamIdent (index)
        : index = index {}

    function serialize(s)
        s.sClass(this, "ParamIdent", "index");
}

class PropIdent extends ASTNode implements BindingIdent, Serializable {
    const ident : IDENT;
    function PropIdent (ident)
        : ident = ident { }
 
    function serialize(s)
        s.sClass(this, "PropIdent", "ident");
}

interface FixtureData {
}

class NamespaceFixture extends ASTNode implements FixtureData, Serializable {
    const ns : Ast::Namespace;
    function NamespaceFixture (ns)
        : ns = ns {}

    function serialize(s)
        s.sClass(this, "NamespaceFixture", "ns");
}

internal class FixtureFwd extends ASTNode implements FixtureData {
    const params     : [...IDENT];
    const nonnullable: Boolean;
    function FixtureFwd (params, nonnullable) 
        : params=params
        , nonnullable=nonnullable {}
}

class ClassFixtureFwd extends FixtureFwd implements Serializable {
    function ClassFixtureFwd (params, nonnullable) : super(params, nonnullable) {}

    function serialize(s)
        s.sClass(this, "ClassFixtureFwd", "params", "nonnullable");
}

class ClassFixture extends ASTNode implements FixtureData, Serializable {
    const cls : Cls;
    function ClassFixture (cls)
        : cls = cls {}

    function serialize(s)
        s.sClass(this, "ClassFixture", "cls");
}

class InterfaceFixtureFwd extends FixtureFwd implements Serializable {
    function InterfaceFixtureFwd (params) : super(params, false) {}

    function serialize(s)
        s.sClass(this, "InterfaceFixtureFwd", "params");
}

class InterfaceFixture extends ASTNode implements FixtureData, Serializable {
    const iface : Interface;
    function InterfaceFixture (iface)
        : iface = iface {}

    function serialize(s)
        s.sClass(this, "InterfaceFixture", "iface");
}

/*
class TypeVarFixture extends ASTNode implements FixtureData, Serializable {
    function serialize(s)
        s.sClass(this, "TypeVarFixture");
}
*/

class TypeFixtureFwd extends FixtureFwd implements Serializable {
    function TypeFixtureFwd (params, nonnullable) : super(params, nonnullable) {}

    function serialize(s)
        s.sClass(this, "TypeFixtureFwd", "params", "nonnullable");
}

class TypeFixture extends ASTNode implements FixtureData, Serializable {
    const params     : [...IDENT];
    const nonnullable: Boolean;
    const ty         : TypeExpr;
    function TypeFixture (params, nonnullable, ty)
        : params=params
        , nonnullable=nonnullable
        , ty = ty {}

    function serialize(s)
        s.sClass(this, "TypeFixture", "params", "nonnullable", "ty");
}

class MethodFixture extends ASTNode implements FixtureData, Serializable {
    const func : Func;
    const ty : TypeExpr;
    const isReadOnly : Boolean;
    const isOverride : Boolean;
    const isFinal : Boolean;
    function MethodFixture(func, ty, isReadOnly, isOverride, isFinal) 
        : func = func
        , ty = ty
        , isReadOnly = isReadOnly
        , isOverride = isOverride
        , isFinal = isFinal { }

    function serialize(s)
        s.sClass(this, "MethodFixture", "func", "ty", "isReadOnly", "isOverride", "isFinal");
}

class ValFixture extends ASTNode implements FixtureData, Serializable {
    const ty : TypeExpr;
    const isReadOnly : Boolean;
    function ValFixture(ty, isReadOnly) 
        : ty=ty
        , isReadOnly=isReadOnly {}

    function serialize(s)
        s.sClass(this, "ValFixture", "ty", "isReadOnly");
}

class VirtualValFixture extends ASTNode implements FixtureData, Serializable {
    const ty : TypeExpr;
    const getter : ? Func;
    const setter : ? Func;

    function VirtualValFixture(ty, getter, setter)
        : ty=ty
        , getter=getter
        , setter=setter {}

    function serialize(s)
        s.sClass(this, "VirtualValFixture", "ty", "getter", "setter");
}

interface TypeExpr {
}

type SPECIAL_TYPE_KIND = int;

class SpecialType extends ASTNode implements TypeExpr, Serializable {
    const kind : SPECIAL_TYPE_KIND;
    function SpecialType(kind) : kind=kind {}

    function serialize(s)
        s.sConstant("Ast::specialTypes[" + kind + "]");
}

const specialTypes = [new SpecialType(0), 
                      new SpecialType(1), 
                      new SpecialType(2), 
                      new SpecialType(3)];

const [anyType, nullType, undefinedType, voidType] = specialTypes;

// These may not be required any more, serialization and
// deserialization preserves identity of the special type objects.

function isAnyType(t:TypeExpr): Boolean
    t is SpecialType && t.kind == 0;

function isNullType(t:TypeExpr): Boolean
    t is SpecialType && t.kind == 1;

function isUndefinedType(t:TypeExpr): Boolean
    t is SpecialType && t.kind == 2;

function isVoidType(t:TypeExpr): Boolean
    t is SpecialType && t.kind == 3;

class UnionType extends ASTNode implements TypeExpr, Serializable {
    const types : [...TypeExpr];
    function UnionType (types)
        : types = types { }

    function serialize(s)
        s.sClass(this, "UnionType", "types");
}

// "spread" may not be the best choice of name here.

class ArrayType extends ASTNode implements TypeExpr, Serializable {
    const types : [...TypeExpr];
    const spread: ? TypeExpr;
    function ArrayType (types, spread)
        : types = types
        , spread = spread {}

    function serialize(s)
        s.sClass(this, "ArrayType", "types", "spread");
}

// FIXME: Really redundant, except it introduces the tag TypeExpr
// which might also be moved to Identifier and QualifiedIdentifier.
// But keep for now until the type language settles down.

class TypeName extends ASTNode implements TypeExpr, Serializable {
    const ident : IdentExpr;
    function TypeName (ident)
        : ident = ident {}

    function serialize(s)
        s.sClass(this, "TypeName", "ident");
}

class ElementTypeRef extends ASTNode implements TypeExpr, Serializable {
    const base : TypeExpr;
    const index : int;
    function ElementTypeRef (base,index)
        : base = base
        , index = index { }

    function serialize(s)
        s.sClass(this, "ElementTypeRef", "base", "index");
}

class FieldTypeRef extends ASTNode implements TypeExpr, Serializable {
    const base : TypeExpr;
    const ident : IdentExpr;
    function FieldTypeRef (base,ident)
        : base = base
        , ident = ident { }

    function serialize(s)
        s.sClass(this, "FieldTypeRef", "base", "ident");
}

class FunctionType extends ASTNode implements TypeExpr, Serializable {
    const typeParams: [...IDENT];
    const thisType  : ? TypeExpr;
    const paramTypes: [...{ty:TypeExpr, optional:Boolean}];
    const hasRest   : Boolean;
    const returnType: ? TypeExpr;
    function FunctionType(typeParams, thisType, paramTypes, hasRest, returnType) 
        : typeParams = typeParams
        , thisType = thisType
        , paramTypes = paramTypes
        , hasRest = hasRest
        , returnType = returnType {}

    function serialize(s) 
        s.sClass(this, "FunctionType", "typeParams", "thisType", "paramTypes", "hasRest", "returnType");
}

class ObjectType extends ASTNode implements Serializable {
    const fields : [...FieldType];
    function ObjectType (fields)
        : fields = fields { }

    function serialize(s)
        s.sClass(this, "ObjectType", "fields");
}

class FieldType extends ASTNode implements Serializable {
    const ident: IDENT;
    const ty: TypeExpr;
    function FieldType (ident,ty)
        : ident = ident
        , ty = ty {}

    function serialize(s)
        s.sClass(this, "FieldType", "ident", "ty");
}

class AppType extends ASTNode implements TypeExpr, Serializable {
    const base : TypeExpr;
    const args : [...TypeExpr];
    function AppType (base,args)
        : base = base
        , args = args { }

    function serialize(s)
        s.sClass(this, "AppType", "base", "args");
}

class NullableType extends ASTNode implements TypeExpr, Serializable {
    const ty : TypeExpr;
    const isNullable : Boolean;
    function NullableType (ty,isNullable)
        : ty = ty
        , isNullable = isNullable { }

    function serialize(s)
        s.sClass(this, "NullableType", "ty", "isNullable");
}

class InstanceType extends ASTNode implements TypeExpr, Serializable {
    const name : Name;
    const typeParams : [...IDENT];
    const ty : TypeExpr;
    const isDynamic : Boolean;
    function InstanceType(name, typeParams, ty, isDynamic)
        : name=name
        , typeParams=typeParams
        , ty=ty
        , isDynamic=isDynamic
    { }

    function serialize(s)
        s.sClass(this, "InstanceType", "name", "typeParams", "ty", "isDynamic");
}

class LikeType extends ASTNode implements Serializable {
    const ty: TypeExpr;
    function LikeType(ty) : ty=ty {}

    function serialize(s)
        s.sClass(this, "LikeType", "ty");
}

interface Stmt {
}

interface LabelSet {
    // Want to express (but can't): has 'labels': Array The best thing
    // we can do is either specify a push method, or a getter method
    // for 'labels'.
}

class EmptyStmt extends ASTNode implements Stmt, Serializable {
    function serialize(s)
        s.sClass(this, "EmptyStmt");
}

class ExprStmt extends ASTNode implements Stmt, Serializable {
    const expr : Expr;
    function ExprStmt (expr)
        : expr = expr {}

    function serialize(s)
        s.sClass(this, "ExprStmt", "expr");
}

internal class ForInStmtCore extends ASTNode implements Stmt, LabelSet {
    const assignment: Expr;
    const tmp: Expr;
    const obj  : Expr;
    const stmt : Stmt;
    const is_each : boolean;
    const labels: [...IDENT];
    function ForInStmtCore (assignment,tmp,obj,stmt,is_each=false,labels=null)
        : assignment = assignment
        , tmp = tmp
        , obj = obj
        , stmt = stmt
        , is_each = is_each
        , labels = labels == null ? ([] : [...IDENT]) : labels {}
}

class ForInStmt extends ForInStmtCore implements Serializable {
    function ForInStmt (assignment,tmp,obj,stmt,is_each=false,labels=null)
        : super(assignment, tmp, obj, stmt, is_each, labels) {}

    function serialize(s)
        s.sClass(this, "ForInStmt", "assignment", "tmp", "obj", "stmt", "is_each", "labels");
}

class ForInBindingStmt extends ForInStmtCore implements Serializable {
    const head: Head;
    const init: ? Expr;

    function ForInBindingStmt (head, assignment, tmp, init, obj, stmt,is_each=false,labels=null)
        : head = head
        , init = init
        , super(assignment, tmp, obj, stmt, is_each, labels) {}

    function serialize(s)
        s.sClass(this, "ForInBindingStmt", "head", "assignment", "tmp", "init", "obj", "stmt", "is_each", "labels");
}

class ThrowStmt extends ASTNode implements Stmt, Serializable {
    const expr : Expr;
    function ThrowStmt (expr)
        : expr = expr { }

    function serialize(s)
        s.sClass(this, "ThrowStmt", "expr");
}

class ReturnStmt extends ASTNode implements Stmt, Serializable {
    const expr : ? Expr;
    function ReturnStmt(expr) 
        : expr = expr { }

    function serialize(s)
        s.sClass(this, "ReturnStmt", "expr");
}

class BreakStmt extends ASTNode implements Stmt, Serializable {
    const ident : ? IDENT;
    function BreakStmt (ident)
        : ident = ident { }

    function serialize(s)
        s.sClass(this, "BreakStmt", "ident");
}

class ContinueStmt extends ASTNode implements Stmt, Serializable {
    const ident : ? IDENT;
    function ContinueStmt (ident)
        : ident = ident { }

    function serialize(s)
        s.sClass(this, "ContinueStmt", "ident");
}

class BlockStmt extends ASTNode implements Stmt, Serializable {
    const block : Block;
    function BlockStmt (block)
        : block = block {}

    function serialize(s)
        s.sClass(this, "BlockStmt", "block");
}

class LabeledStmt extends ASTNode implements Stmt, Serializable {
    const label : IDENT;
    const stmt : Stmt;
    function LabeledStmt (label,stmt)
        : label = label
        , stmt = stmt { }

    function serialize(s)
        s.sClass(this, "LabeledStmt", "label", "stmt");
}

class LetBlockStmt extends ASTNode implements Stmt, Serializable {
    const outer_head : Head;
    const inner_head : Head;
    const body : [...Stmt];
    function LetBlockStmt (outer_head,inner_head,body)
        : outer_head = outer_head
        , inner_head = inner_head
        , body = body{}

    function serialize(s)
        s.sClass(this, "LetBlockStmt", "outer_head", "inner_head", "body");
}

class WhileStmt extends ASTNode implements Stmt, Serializable, LabelSet {
    const expr : Expr;
    const stmt : Stmt;
    const labels: [...IDENT];
    function WhileStmt (expr,stmt,labels=null)
        : expr = expr
        , stmt = stmt
        , labels = labels == null ? ([] : [...IDENT]) : labels {}

    function serialize(s)
        s.sClass(this, "WhileStmt", "expr", "stmt", "labels");
}

class DoWhileStmt extends ASTNode implements Stmt, Serializable, LabelSet {
    const expr : Expr;
    const stmt : Stmt;
    const labels : [...IDENT];
    function DoWhileStmt (expr,stmt,labels=null)
        : expr = expr
        , stmt = stmt 
        , labels = labels == null ? ([] : [...IDENT]) : labels {}

    function serialize(s)
        s.sClass(this, "DoWhileStmt", "expr", "stmt", "labels");
}

internal class ForStmtCore extends ASTNode implements Stmt, LabelSet {
    const init : ? Expr;
    const cond : ? Expr;
    const incr : ? Expr;
    const stmt : Stmt;
    const labels : [...IDENT];
    function ForStmtCore (init,cond,incr,stmt,labels=null)
        : init = init
        , cond = cond
        , incr = incr
        , stmt = stmt
        , labels = labels == null ? ([] : [...IDENT]) : labels {}
}

class ForStmt extends ForStmtCore implements Serializable {
    function ForStmt (init,cond,incr,stmt,labels=null)
        : super(init, cond, incr, stmt, labels) {}

    function serialize(s)
        s.sClass(this, "ForStmt", "init", "cond", "incr", "stmt", "labels");
}

class ForBindingStmt extends ForStmtCore implements Serializable {
    const head : Head;
    function ForBindingStmt (head,init,cond,incr,stmt,labels=null)
        : head = head
        , super(init, cond, incr, stmt, labels) {}

    function serialize(s)
        s.sClass(this, "ForBindingStmt", "head", "init", "cond", "incr", "stmt", "labels");
}

class IfStmt extends ASTNode implements Stmt, Serializable {
    const test       : Expr;
    const consequent : Stmt;
    const alternate  : ? Stmt;
    function IfStmt (test, consequent, alternate)
        : test=test
        , consequent=consequent
        , alternate=alternate { }

    function serialize(s)
        s.sClass(this, "IfStmt", "test", "consequent", "alternate");
}

class SwitchStmt extends ASTNode implements Stmt, Serializable {
    const head  : Head;
    const expr  : Expr;
    const cases : [...Case];
    function SwitchStmt (head, expr, cases)
        : head = head
        , expr = expr
        , cases = cases { }

    function serialize(s)
        s.sClass(this, "SwitchStmt", "head", "expr", "cases");
}

class Case extends ASTNode implements Stmt, Serializable {
    const expr : ? Expr;  // null for default
    const stmts : [...Stmt];
    function Case (expr,stmts)
        : expr = expr
        , stmts = stmts { }

    function serialize(s)
        s.sClass(this, "Case", "expr", "stmts");
}

class WithStmt extends ASTNode implements Stmt, Serializable {
    const expr : Expr;
    const stmt : Stmt;
    function WithStmt (expr,stmt)
        : expr = expr
        , stmt = stmt { }

    function serialize(s)
        s.sClass(this, "WithStmt", "expr", "stmt");
}

class TryStmt extends ASTNode implements Stmt, Serializable {
    const block        : Block;
    const catches      : [...Catch];
    const finallyBlock : ? Block;
    function TryStmt (block,catches,finallyBlock)
        : block = block
        , catches = catches
        , finallyBlock = finallyBlock { }

    function serialize(s)
        s.sClass(this, "TryStmt", "block", "catches", "finallyBlock");
}

class SwitchTypeStmt extends ASTNode implements Stmt, Serializable {
    const expr: Expr;
    const cases: [...Catch];
    function SwitchTypeStmt (expr,cases)
        : expr = expr
        , cases = cases { }

    function serialize(s)
        s.sClass(this, "SwitchTypeStmt", "expr", "cases");
}

class Catch extends ASTNode implements Serializable {
    const param: Head;
    const block: Block;
    function Catch (param,block)
        : param = param
        , block = block { }

    function serialize(s)
        s.sClass(this, "Catch", "param", "block");
}

class SuperStmt extends ASTNode implements Stmt, Serializable {
    const args  : [...Expr];
    const spread: ? Expr;
    function SuperStmt(args, spread) 
        : args=args
        , spread=spread {}

    function serialize(s)
        s.sClass(this, "SuperStmt", "args", "spread");
}

class Block extends ASTNode implements Serializable {
    const head  : ? Head;
    const stmts : [...Stmt];
    function Block (head,stmts)
        : head = head
        , stmts = stmts { }

    function serialize(s) 
        s.sClass(this, "Block", "head", "stmts");
}

/*
 * PROGRAM
 */

class Program extends ASTNode implements Serializable {
    const body : [...Stmt];
    const head : Head;
    const file : ? String;
    const attr : FuncAttr;
    function Program (body, head, attr, file=null)
        : body = body
        , head = head
        , attr = attr
        , file = file {}

    function serialize(s)
        s.sClass(this, "Program", "body", "head", "attr", "file");
}

/* Helper tables */

const tokenToOperator = [];

// Binary
tokenToOperator[Token::Equal] = Ast::equalOp;
tokenToOperator[Token::NotEqual] = Ast::notEqualOp;
tokenToOperator[Token::StrictEqual] = Ast::strictEqualOp;
tokenToOperator[Token::StrictNotEqual] = Ast::strictNotEqualOp;
tokenToOperator[Token::LessThan] = Ast::lessOp;
tokenToOperator[Token::GreaterThan] = Ast::greaterOp;
tokenToOperator[Token::LessThanOrEqual] = Ast::lessOrEqualOp;
tokenToOperator[Token::GreaterThanOrEqual] = Ast::greaterOrEqualOp;
tokenToOperator[Token::In] = Ast::inOp;
tokenToOperator[Token::InstanceOf] = Ast::instanceOfOp;
tokenToOperator[Token::Is] = Ast::isOp;
tokenToOperator[Token::Cast] = Ast::castOp;
tokenToOperator[Token::Like] = Ast::likeOp;
tokenToOperator[Token::LeftShift] = Ast::leftShiftOp;
tokenToOperator[Token::RightShift] = Ast::rightShiftOp;
tokenToOperator[Token::UnsignedRightShift] = Ast::rightShiftUnsignedOp;
tokenToOperator[Token::Plus] = Ast::plusOp;
tokenToOperator[Token::Minus] = Ast::minusOp;
tokenToOperator[Token::Mult] = Ast::timesOp;
tokenToOperator[Token::Div] = Ast::divideOp;
tokenToOperator[Token::Remainder] = Ast::remainderOp;
tokenToOperator[Token::Delete] = Ast::deleteOp;
tokenToOperator[Token::Assign] = Ast::assignOp;
tokenToOperator[Token::PlusAssign] = Ast::assignPlusOp;
tokenToOperator[Token::MinusAssign] = Ast::assignMinusOp;
tokenToOperator[Token::MultAssign] = Ast::assignTimesOp;
tokenToOperator[Token::DivAssign] = Ast::assignDivideOp;
tokenToOperator[Token::RemainderAssign] = Ast::assignRemainderOp;
tokenToOperator[Token::BitwiseAndAssign] = Ast::assignBitwiseAndOp;
tokenToOperator[Token::BitwiseOrAssign] = Ast::assignBitwiseOrOp;
tokenToOperator[Token::BitwiseXorAssign] = Ast::assignBitwiseXorOp;
tokenToOperator[Token::LeftShiftAssign] = Ast::assignLeftShiftOp;
tokenToOperator[Token::RightShiftAssign] = Ast::assignRightShiftOp;
tokenToOperator[Token::UnsignedRightShiftAssign] = Ast::assignRightShiftUnsignedOp;
tokenToOperator[Token::LogicalAndAssign] = Ast::assignLogicalAndOp;
tokenToOperator[Token::LogicalOrAssign] = Ast::assignLogicalOrOp;

// Unary.
tokenToOperator[Token::Delete + 1000] = Ast::deleteOp;
tokenToOperator[Token::PlusPlus + 1000] = Ast::preIncrOp;
tokenToOperator[Token::MinusMinus + 1000] = Ast::preDecrOp;
tokenToOperator[Token::Void + 1000] = Ast::voidOp;
tokenToOperator[Token::TypeOf + 1000] = Ast::typeOfOp;
tokenToOperator[Token::Plus + 1000] = Ast::unaryPlusOp;
tokenToOperator[Token::Minus + 1000] = Ast::unaryMinusOp;
tokenToOperator[Token::BitwiseNot + 1000] = Ast::bitwiseNotOp;
tokenToOperator[Token::Not + 1000] = Ast::logicalNotOp;

//////////////////////////////////////////////////////////////////////
//
// Serialization

class Serializer {
    use default namespace internal, 
        namespace internal;

    var compact;

    public function Serializer(compact=false) : compact=compact {}

    public function serialize(obj) {
        if (obj is Serializable)
            return obj.serialize(this);

        if (obj is Array) 
            return serializeArray(obj, true);

        if (obj is Number || obj is Boolean || obj is int || obj is uint || obj === null || obj === undefined)
            return String(obj);

        if (obj is String)
            return "'" + sanitize(obj) + "'";

        //throw new Error("Unserializable datum " + obj);
        return "[[" + obj + "]]";
    }

    function serializeArray(obj, linebreak) {
        let s = "[";
        let separator = !compact && linebreak ? "\n," : ",";
        let lastWasOK = true;
        for ( let i=0, limit=obj.length ; i < limit ; i++ ) {
            lastWasOK = false;
            if (i > 0)
                s += separator;
            if (obj.hasOwnProperty(i)) {
                lastWasOK = true;
                s += serialize(obj[i]);
            }
        }
        if (!lastWasOK)
            s += separator;
        s += "]";
        return s;
    }

    function sanitize(s) {
        let r = "";
        let i = 0;
        let l = s.length;
        outer:
        while (i < l) {
            let start = i;
            while (i < l) {
                let c = s.charCodeAt(i);
                if (c < 32 || 
                    c == Char::BackSlash || 
                    c == Char::SingleQuote || 
                    c == Char::DoubleQuote ||
                    c == Char::UnicodePS ||
                    c == Char::UnicodeLS) {
                    r += s.substring(start, i);
                    s += uescape(c);
                    i++;
                    continue outer;
                }
                i++;
            }
            r += s.substring(start, i);
        }
        return r;
    }

    function uescape(c)
        "\\u" + (c+0x10000).toString(16).substring(1);

    public function sClass(obj, ...rest) {
        // The 'ast_layout' is reduced to the class name the day 
        // Tamarin performs enumeration/itemization in insertion 
        // order.  If size concerns us (not likely) then we can
        // compress by maintaining an indexed dictionary of
        // layout arrays, which is emitted as part of the output.

        let sep0 = compact ? "" : "\n, ";
        let sep1 = compact ? "," : "\n, ";
        let s = "";
        if (compact)
            s += "{/*" + rest[0] + "*/ ";
        else
            s += "{ 'ast_layout': " + serializeArray(rest, false);
        for ( let i=1, limit=rest.length ; i < limit ; i++ ) {
            s += (i > 1 ? sep1 : sep0) + "'" + rest[i] + "': ";
            s += serialize(obj[rest[i]]);
        }
        s += "}";
        return s;
    }

    public function sConstant(expr)
        "{ 'ast_constant': '" + expr + "' }";
}

// FIXME I:
// The unserializer uses 'eval' for three things:
//   - to decode the JSON input (dangerous)
//   - to create constructor functions for classes
//   - to look up global constant properties
//
// The latter two could be gotten rid of by using 
//
//    new Ast::[tag](...desc.map(decode))
//
// and
//
//    Ast::[constName]
//
// respectively (if the serializer cooperates; right now the
// constName expression is not a name so that wouldn't work), 
// but for that we need bracket syntax and splat to work.
//
// FIXME II:
// The function 'globalEval' should be removed and invocations
// of it should be replaced simply by 'global.eval', but that
// requires a working notion of 'global' in tamarin.

class Unserializer {
    use default namespace internal,
        namespace internal;

    public function unserializeText(s) 
        decode(globalEval("(" + s + ")"));

    public function unserializeObject(obj) 
        decode(obj);

    function decode(x) {
        if (x is Boolean || x is Number || x is int || x is uint || x is String)
            return x;

        if (x is Array) {
            for ( let i=0, limit=x.length ; i < limit ; i++ )
                if (x.hasOwnProperty(i))
                    x[i] = decode(x[i]);
            return x;
        }

        if (x.hasOwnProperty("ast_layout")) {
            let desc = x.ast_layout;
            let tag = desc[0];
            desc.shift(1);
            return (getConstructor(tag, desc.length)).apply(null, Util::map(function (n) { return decode(x[n]) }, desc));
        }

        if (x.hasOwnProperty("ast_constant"))
            return globalEval(x.ast_constant);

        let s = "{ ";
        for ( let n in x )
            if (x.hasOwnProperty(n))
                s += "'" + n + "': " + x[n];
        s += "}";
        throw new Error("Unknown datum type: object without a tag: " + s);
    }

    // A map from name to function that constructs instance of name.
    var constructors = {};

    function getConstructor(name, arity) {
        if (!constructors.hasOwnProperty(name)) {
            // FIXME: An array comprehension would be pretty here...
            let args = [];
            for ( let i=0 ; i < arity ; i++ )
                args.push("_p" + i);
            constructors[name] = globalEval("(function (" + args + ") { return new Ast::" + name + "(" + args + ") })");
        }
        return constructors[name];
    }

    function globalEval(s)
        ESC::evaluateInScopeArray([s], [], "");
}
