/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

// Constants and bindings

// The parser parses constants and adds lists of constant definitions with
// unresolved RHS values to lists in the program and function nodes.
//
// The code generator resolves those constants.  Values that either must be constants
// (identifiers used in 'namespace', 'const', and 'use namespace' definitions)
// or can be constants (identifiers used as the namespace value in qualified
// identifiers) are looked up during code generation by searching the context.
// The 'Program' and 'Function' (and probably 'Package', when we have that)
// ribs have separate lists of constant bindings with their values.  Those value
// lists are populated in a prepass when code generation enters a Program or
// Function, since a statement can reference a later constant binding at the
// same or outer scope.

/*
enum CVTag {
    TAG_cvValue,
    TAG_cvAnonNS,
    TAG_cvNamespace
};

class ConstValue {
public:
    virtual ~ConstValue();
    virtual CVTag tag() const = 0;
};

class CVValue : public ConstValue {
public:
    CVValue(Expr* expr) : expr(expr) {}
    virtual CVTag tag() const { return TAG_cvValue; }
    Expr * const expr;
};

class CVAnonNS : public ConstValue {
public:
    virtual CVTag tag() const { return TAG_cvAnonNS; }
};

class CVNamespace : public ConstValue {
public:
    CVNamespace(Expr* expr) : expr(expr) {}
    virtual CVTag tag() const { return TAG_cvNamespace; }
    Expr * const expr;
};
*/

enum BindingKind {
    TAG_varBinding,     // also used for function (though not for class methods)
    TAG_constBinding,
    TAG_methodBinding,  // methods in classes and interfaces
    TAG_namespaceBinding,
    TAG_classBinding,
    TAG_interfaceBinding
};

enum QualifierTag {
    QUAL_none,
    QUAL_private,
    QUAL_protected,
    QUAL_public,
    QUAL_internal,
    QUAL_name
};

class Qualifier {
public:
    Qualifier()
        : tag(QUAL_none)
        , is_dynamic(0)
        , is_final(0)
        , is_native(0)
        , is_override(0)
        , is_static(0)
        , name(NULL)
        , metadata(NULL)
    {
    }
    QualifierTag tag;
    uint32_t is_dynamic;
    uint32_t is_final;
    uint32_t is_native;
    uint32_t is_override;
    uint32_t is_static;
    QualifiedName* name;
    Seq<Expr*>* metadata;
};

class Binding {
public:
    Binding(NameComponent* ns, Str* name, Type* type_name, BindingKind kind)
        : ns(ns)
        , name(name)
        , type_name(type_name)
        , kind(kind)
    {
    }

    NameComponent* const ns;
    Str* const name;
    Type* const type_name;
    const BindingKind kind;
};


// Program, class, interface, etc nodes

enum CodeType {
    CODE_Program,
    CODE_Class,
    CODE_Function
};

// Information about the body of a program or function, built up during parsing.
// This is never stored in the AST, its contents are copied into CodeBlock,
// Program, or FunctionDefn.

class BodyInfo
{
public:
    BodyInfo(Allocator* allocator)
        : bindings(allocator)
        , functionDefinitions(allocator)
        , namespaces(allocator)
        , openNamespaces(allocator)
        , stmts(allocator)
        , uses_finally(false)
        , uses_catch(false)
        , uses_goto(false)
        , uses_arguments(false)
        , uses_dxns(false)
        , uses_super(false)
        , empty_body(false)
    {
    }

    SeqBuilder<Binding*> bindings;
    SeqBuilder<FunctionDefn*> functionDefinitions;
    SeqBuilder<NamespaceDefn*> namespaces;
    SeqBuilder<Namespace*> openNamespaces;
    SeqBuilder<Stmt*> stmts;
    bool uses_finally;
    bool uses_catch;
    bool uses_goto;
    bool uses_arguments;
    bool uses_dxns;
    bool uses_super;          // True only for super /statements/, as in constructors
    bool empty_body;
};

// Information about the interface of a function, built up during parsing.
// This is never stored in the AST, its contents are copied into CodeBlock
// or FunctionDefn.
    
class SignatureInfo
{
public:
    SignatureInfo(Allocator* allocator)
        : ns(NULL)
        , name(NULL)
        , params(allocator)
        , rest_param(NULL)
        , numparams(0)
        , optional_arguments(false)
        , isVoid(false)
        , isGetter(false)
        , isSetter(false)
        , return_type_name(NULL)
    {
    }
    
    NameComponent* ns;
    Str* name;
    SeqBuilder<FunctionParam*> params;
    FunctionParam* rest_param;
    uint32_t numparams;
    bool optional_arguments;
    bool isVoid;
    bool isGetter;
    bool isSetter;
    Type* return_type_name;
};

// Base class for Program and FunctionDefn, holds information about the "body"
// of whatever we're compiling.

class CodeBlock {
public:
    CodeBlock(CodeType tag, BodyInfo& body)
        : tag(tag)
        , bindings(body.bindings.get())
        , functions(body.functionDefinitions.get())
        , namespaces(body.namespaces.get())
        , openNamespaces(body.openNamespaces.get())
        , stmts(body.stmts.get())
        , uses_finally(body.uses_finally)
        , uses_catch(body.uses_catch)
        , uses_goto(body.uses_goto)
        , uses_arguments(body.uses_arguments)
        , uses_dxns(body.uses_dxns)
        , uses_super(body.uses_super)
        , empty_body(body.empty_body)
    {
    }

    virtual ~CodeBlock();
    virtual void cogenBody(Cogen* cogen, Ctx* ctx, uint32_t activation) = 0;
    
    void cogen(Cogen* cogen, Ctx* ctx);
    
    const CodeType tag;
    Seq<Binding*>* const bindings;
    Seq<FunctionDefn*>* const functions;
    Seq<NamespaceDefn*>* const namespaces;
    Seq<Namespace*>* const openNamespaces;
    Seq<Stmt*>* stmts;                      // Not const because sometimes altered after the fact
    const bool uses_finally;
    const bool uses_catch;
    const bool uses_goto;
    const bool uses_arguments;
    const bool uses_dxns;
    const bool uses_super;                  // True only for super /statements/, as in constructors
    const bool empty_body;                  // True for all interface methods
};

class Program : public CodeBlock {
public:
    Program(BodyInfo& body, Seq<TypeDefn*>* classes)
        : CodeBlock(CODE_Program, body)
        , classes(classes)
    {
    }
    
    virtual void cogenBody(Cogen* cogen, Ctx* ctx, uint32_t activation);
    
    void cogenTypes(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, Seq<TypeDefn*>* classes);
    void cogenTypeHierarchy(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, TypeDefn* cls);
    void cogenType(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, ClassDefn* cls);
    void cogenType(Cogen* cogen, Ctx* ctx, ABCTraitsTable* global_traits, InterfaceDefn* cls);
    ABCTraitsTable* cogenClassTraits(Cogen* cogen, Ctx* ctx, Seq<FunctionDefn*>* inst_methods, Seq<Binding*>* inst_vars);
    uint32_t cogenConstructor(Cogen* cogen, Ctx* ctx, FunctionDefn* constructor);
    ClassDefn* findClass(Cogen* cogen, uint32_t pos, Str* name);
    InterfaceDefn* findInterface(Cogen* cogen, uint32_t pos, Str* name);
    int pushBaseClasses(Cogen* cogen, ClassDefn* currcls);
    uint32_t* interfaceRefs(Cogen* cogen, Seq<Str*>* ifaces, uint32_t* interface_count);

    Seq<TypeDefn*>* const classes;
};

class TypeDefn {
public:
    TypeDefn(uint32_t pos, NameComponent* ns, Str* name, bool is_interface)
        : pos(pos)
        , ns(ns)
        , name(name)
        , is_interface(is_interface)
        , is_generated(false)
        , is_searching(false)
    {
    }

    uint32_t pos;
    NameComponent* const ns;
    Str* const name;
    const bool is_interface;
    bool is_generated;
    bool is_searching;      // A mark used during class hierarchy traversal indicating that the class has already been visited
};
    
class ClassDefn : public TypeDefn {
public:
    ClassDefn(uint32_t pos,
              NameComponent* ns, Str* name, bool is_final, bool is_dynamic, Str* extends, Seq<Str*>* implements,
              Seq<FunctionDefn*>* cls_methods, Seq<Binding*>* cls_vars, Seq<NamespaceDefn*>* cls_namespaces,
              FunctionDefn* cls_init,
              Seq<FunctionDefn*>* inst_methods, Seq<Binding*>* inst_vars, FunctionDefn* inst_init);

    const bool is_final;
    const bool is_dynamic;
    Str* const extends;
    Seq<Str*>* const implements;
    Seq<FunctionDefn*>* const cls_methods;
    Seq<Binding*>* const cls_vars;
    Seq<NamespaceDefn*>* const cls_namespaces;
    FunctionDefn* const cls_init;
    Seq<FunctionDefn*>* const inst_methods;     // Does not contain the constructor
    Seq<Binding*>* const inst_vars;
    FunctionDefn* const inst_init;
};

class InterfaceDefn : public TypeDefn {
public:
    InterfaceDefn(uint32_t pos, NameComponent* ns, Str* name, Seq<Str*>* extends, Seq<FunctionDefn*>* inst_methods)
        : TypeDefn(pos, ns, name, true)
        , extends(extends)
        , inst_methods(inst_methods)
    {
    }

    Seq<Str*>* const extends;
    Seq<FunctionDefn*>* const inst_methods;
};

class FunctionParam {
public:
    FunctionParam(Str* name, Type* type_name, Expr* default_value)
        : name(name)
        , type_name(type_name)
        , default_value(default_value)
    {
    }

    Str * const name;
    Type * const type_name;
    Expr * const default_value;
};

// Somewhat mis-named, also used for function expressions (see LiteralFunction below), where name may be NULL
    
class FunctionDefn : public CodeBlock {
public:
    FunctionDefn(SignatureInfo& signature, BodyInfo& body, Seq<Stmt*>* inits=NULL)
        : CodeBlock(CODE_Function, body)
        , ns(signature.ns)
        , name(signature.name)
        , params(signature.params.get())
        , numparams(signature.numparams)
        , rest_param(signature.rest_param)
        , return_type_name(signature.return_type_name)
        , optional_arguments(signature.optional_arguments)
        , isGetter(signature.isGetter)
        , isSetter(signature.isSetter)
        , inits(inits)
    {
    }

    virtual void cogenBody(Cogen* cogen, Ctx* ctx, uint32_t activation);

    // Common code for function code generation.  Create MethodInfo and MethodBodyInfo;
    // generate code for the function; set the function's flags properly.
    void cogenGuts(Compiler* compiler, Ctx* ctx, ABCMethodInfo** info, ABCMethodBodyInfo** body);

    void cogenGuts(Compiler* compiler, Ctx* ctx, bool isMethod, ABCMethodInfo** info, ABCMethodBodyInfo** body);
    
    NameComponent* ns;                  // Namespace of function or NULL.
    Str* name;                          // Name of function or NULL.  Not const because we sometimes need to set it to NULL.
    Seq<FunctionParam*>* const params;  // List of fixed parameters
    const uint32_t numparams;           // Number of fixed parameters
    FunctionParam* rest_param;          // non-NULL iff we have a ... rest argument; name+type of that argument
    Type* return_type_name;             // may be NULL
    const bool optional_arguments;      // true iff any of the parameters have default values
    const bool isGetter;                // true iff this is a getter function
    const bool isSetter;                // true iff this is a setter function
    Seq<Stmt*>* inits;                  // initializing statements executed in the outer scope on entry
};

class NamespaceDefn {
public:
    NamespaceDefn(NameComponent* ns, Str* name, Expr* value) : ns(ns), name(name), value(value) {}
    NameComponent* const ns;
    Str* const name;
    Expr* const value;  // NULL for anonymous ones; TAG_literalString for string values; otherwise a NameExpr
};

// Tags returned from Expr::tag()

enum Tag {
    TAG_expr,                   // generic expr
    TAG_objectRef,              // instance of ObjectRef
    TAG_qualifiedName,          // instance of QualifiedName
    TAG_instantiatedTypeRef,
    TAG_literalString,          // instance of LiteralString
    TAG_literalUndefined,
    TAG_literalNull,
    TAG_literalBoolean,
    TAG_literalInt,
    TAG_literalUInt,
    TAG_literalDouble,
    TAG_literalFloat,
    TAG_literalRegExp,
    TAG_literalFunction,
    TAG_commonNamespace,
    TAG_namespaceRef,
    TAG_literalArray,
    TAG_literalVector,
    TAG_literalObject,
    TAG_literalXml,
    TAG_binaryExpr,
    TAG_unaryExpr,
    TAG_conditionalExpr,
    TAG_type,                   // generic type
    TAG_simpleType,             // type name
    TAG_instantiatedType        // instantiation of parameterized type
};

// Tags returned from NameComponent::tag()

enum NameTag {
    TAG_simpleName,             // instance of SimpleName
    TAG_wildcardName,           // instance of WildcardName
    TAG_computedName,           // instance of ComputedName
    TAG_builtinNamespace,       // instance of BuiltinNamespace
};


// Expression nodes

// Binary operators are used in the table binopMapping in eval-cogen.cpp.
// So keep these in order, and if you add any you *must* add entries to
// that table.

enum Binop {
    // 0 is not used
    OPR_as=1,
    OPR_plus,
    OPR_assign,
    OPR_init,
    OPR_comma,
    OPR_minus,
    OPR_multiply,
    OPR_divide,
    OPR_remainder,
    OPR_leftShift,
    OPR_rightShift,
    OPR_rightShiftUnsigned,
    OPR_instanceof,
    OPR_in,
    OPR_is,
    OPR_bitwiseAnd,
    OPR_bitwiseOr,
    OPR_bitwiseXor,
    OPR_logicalAnd,
    OPR_logicalOr,
    OPR_less,
    OPR_lessOrEqual,
    OPR_greater,
    OPR_greaterOrEqual,
    OPR_equal,
    OPR_notEqual,
    OPR_strictEqual,
    OPR_strictNotEqual
};

enum Unop {
    // 0 is not used
    OPR_deleteVar=1,
    OPR_delete,
    OPR_typeofVar,
    OPR_typeof,
    OPR_postIncr,
    OPR_preIncr,
    OPR_postDecr,
    OPR_preDecr,
    OPR_postIncrVar,
    OPR_preIncrVar,
    OPR_postDecrVar,
    OPR_preDecrVar,
    OPR_bitwiseNot,
    OPR_unminus,
    OPR_unplus,
    OPR_not,
    OPR_void
};

class Type {
public:
    Type(uint32_t pos=0) : pos(pos) {}
    virtual ~Type() {}    // Not really, but otherwise gcc drowns us in warnings
    virtual Tag tag() const { return TAG_type; }
    virtual void cogen(Cogen* cogen, Ctx* ctx) = 0; // Generates a reference to the type

    const uint32_t pos;
};

class SimpleType : public Type {
public:
    SimpleType(QualifiedName* name, uint32_t pos=0)
        : Type(pos)
        , name(name)
    {
    }
    
    QualifiedName* const name;
    virtual Tag tag() const { return TAG_simpleType; }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
};

class InstantiatedType : public Type {
public:
    InstantiatedType(QualifiedName* basename, Type* tparam, uint32_t pos=0)
        : Type(pos)
        , basename(basename)
        , tparam(tparam)
    {
    }
    
    QualifiedName* const basename;
    Type* const tparam;
    virtual Tag tag() const { return TAG_instantiatedType; }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
};

class Expr {
public:
    Expr(uint32_t pos=0) : pos(pos) {}
    virtual ~Expr() {}  // Not really, but otherwise gcc drowns us in warnings
    virtual void cogen(Cogen* cogen, Ctx* ctx) = 0;   // override for value computation
    virtual Tag tag() const { return TAG_expr; }
    const uint32_t pos;
};

class LiteralObject : public Expr {
public:
    LiteralObject(Seq<LiteralField*>* fields, uint32_t pos) : Expr(pos), fields(fields) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Seq<LiteralField*>* const fields;
    virtual Tag tag() const { return TAG_literalObject; }
};

class LiteralField {
public:
    LiteralField(Str* name, Expr* value) : name(name), value(value) {}
    Str* const name;
    Expr* const value;
};

class LiteralArray : public Expr {
public:
    LiteralArray(Seq<Expr*>* elements, uint32_t pos) : Expr(pos), elements(elements) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Seq<Expr*>* const elements;
    virtual Tag tag() const { return TAG_literalArray; }
};

class LiteralVector : public Expr {
public:
    LiteralVector(Type* type, Seq<Expr*>* elements, uint32_t pos) : Expr(pos), type(type), elements(elements) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Type* const type;
    Seq<Expr*>* const elements;
    virtual Tag tag() const { return TAG_literalVector; }
};

class LiteralUndefined : public Expr {
public:
    LiteralUndefined(uint32_t pos) : Expr(pos) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalUndefined; }
};

class LiteralNull : public Expr {
public:
    LiteralNull(uint32_t pos) : Expr(pos) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalNull; }
};

class LiteralInt : public Expr {
public:
    LiteralInt(int32_t value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalInt; }
    const int32_t value;
};

class LiteralUInt : public Expr {
public:
    LiteralUInt(uint32_t value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalUInt; }
    const uint32_t value;
};

class LiteralDouble : public Expr {
public:
    LiteralDouble(double value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalDouble; }
    const double value;
};

#ifdef VMCFG_FLOAT
class LiteralFloat : public Expr {
public:
    LiteralFloat(float value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalFloat; }
    const float value;
};
#endif

class LiteralBoolean : public Expr {
public:
    LiteralBoolean(bool value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalBoolean; }
    const bool value;
};

class LiteralString : public Expr {
public:
    LiteralString(Str* value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalString; }
    Str* const value;
};

class LiteralRegExp : public Expr {
public:
    LiteralRegExp(Str* value, uint32_t pos) : Expr(pos), value(value) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalRegExp; }
    Str* const value;
};

class LiteralFunction : public Expr {
public:
    LiteralFunction(FunctionDefn* function) : function(function) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalFunction; }
    FunctionDefn* const function;
};

class XmlInitializer : public Expr {
public:
    XmlInitializer(Seq<Expr*>* exprs, bool is_list, uint32_t pos)
        : Expr(pos)
        , exprs(exprs)
        , is_list(is_list)
    {
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_literalXml; }
    Seq<Expr*>* const exprs;
    const bool is_list;
};

class ThisExpr : public Expr {
public:
    virtual void cogen(Cogen* cogen, Ctx* ctx);
};

class NewExpr : public Expr {
public:
    NewExpr(Expr* fn, Seq<Expr*>* arguments) : fn(fn), arguments(arguments) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const fn;
    Seq<Expr*>* const arguments;
};

class CallExpr : public Expr {
public:
    CallExpr(Expr* fn, Seq<Expr*>* arguments, uint32_t pos) : Expr(pos), fn(fn), arguments(arguments) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const fn;
    Seq<Expr*>* const arguments;
};

class SuperExpr : public Expr {
public:
    SuperExpr(Expr* obj) : obj(obj) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const obj;
};

class ConditionalExpr : public Expr {
public:
    ConditionalExpr(Expr* e1, Expr* e2, Expr* e3) : e1(e1), e2(e2), e3(e3) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_conditionalExpr; }
    Expr* const e1;
    Expr* const e2;
    Expr* const e3;
};

class AssignExpr : public Expr {
public:
    AssignExpr(Binop op, Expr* lhs, Expr* rhs) : op(op), lhs(lhs), rhs(rhs) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    const Binop op;
    Expr* const lhs;
    Expr* const rhs;
};
    
class BinaryExpr : public Expr {
public:
    BinaryExpr(Binop op, Expr* lhs, Expr* rhs) : op(op), lhs(lhs), rhs(rhs) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_binaryExpr; }
    const Binop op;
    Expr* const lhs;
    Expr* const rhs;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Unop op, Expr* expr) : op(op), expr(expr) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_unaryExpr; }
    void incdec(Cogen* cogen, Ctx* ctx, bool pre, bool inc);
    Unop const op;
    Expr* const expr;
};

class RefLocalExpr : public Expr {
public:
    RefLocalExpr(uint32_t local) : local(local) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    const uint32_t local;
};

class FilterExpr : public Expr {
public:
    FilterExpr(Expr* obj, Expr* filter, uint32_t pos) : Expr(pos), obj(obj), filter(filter) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr * const obj;
    Expr * const filter;
};

class DescendantsExpr : public Expr {
public:
    DescendantsExpr(Expr* obj, QualifiedName* name, uint32_t pos) : Expr(pos), obj(obj), name(name) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr * const obj;
    QualifiedName * const name;
};

enum Escapement {
    ESC_none,
    ESC_elementValue,
    ESC_attributeValue
};
    
class EscapeExpr : public Expr {
public:
    EscapeExpr(Expr* expr, Escapement esc) : Expr(0), expr(expr), esc(esc) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr * const expr;
    const Escapement esc;
};

// Anything that is a name is a NameExpr.  There are three kinds:
//
//    QualifiedName for id, ns::id, ns::[E], ditto with '@' prefix
//
//    ObjectRef for obj.<QualifiedName>
//
//    InstantiatedTypeRef for Vector.<T>
//
// Notes:
//   ns::@... is not legal syntax and need not be supported


class NameExpr : public Expr {
public:
    NameExpr(uint32_t pos) : Expr(pos) {}
};

class QualifiedName : public NameExpr {
public:
    QualifiedName(NameComponent* qualifier, NameComponent* name, bool is_attr, uint32_t pos)
        : NameExpr(pos)
        , qualifier(qualifier)
        , name(name)
        , is_attr(is_attr)
    {
        AvmAssert(name != NULL);
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_qualifiedName; }
    NameComponent * const qualifier;
    NameComponent * const name;
    const bool is_attr;
};

class ObjectRef : public NameExpr {
public:
    ObjectRef(Expr* obj, QualifiedName* name, uint32_t pos)
        : NameExpr(pos)
        , obj(obj)
        , name(name)
    {
        AvmAssert(obj != NULL && name != NULL);
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_objectRef; }
    Expr * const obj;
    QualifiedName * const name;
};

class InstantiatedTypeRef : public NameExpr {
public:
    InstantiatedTypeRef(InstantiatedType* type, uint32_t pos)
        : NameExpr(pos)
        , type(type)
    {
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual Tag tag() const { return TAG_instantiatedTypeRef; }
    InstantiatedType * const type;
};
    
class NameComponent {
public:
    virtual ~NameComponent();
    virtual NameTag tag() const = 0;
};

// simple identifier
class SimpleName : public NameComponent {
public:
    SimpleName(Str* name) : name(name) { AvmAssert(name != NULL); }
    virtual NameTag tag() const { return TAG_simpleName; }
    Str* const name;
};

// the '*' identifier
class WildcardName : public NameComponent {
public:
    WildcardName() {}
    virtual NameTag tag() const { return TAG_wildcardName; }
};

// a [E] name
class ComputedName : public NameComponent {
public:
    ComputedName(Expr* expr) : expr(expr) {}
    virtual NameTag tag() const { return TAG_computedName; }
    Expr* const expr;
};

// private, protected, public, internal
class BuiltinNamespace : public NameComponent {
public:
    BuiltinNamespace(Token t) : t(t) {}
    virtual NameTag tag() const { return TAG_builtinNamespace; }
    const Token t;
};

// Statement nodes

class Stmt {
public:
    Stmt(uint32_t pos=0) : pos(pos) {}
    virtual ~Stmt() {}
    virtual void cogen(Cogen* cogen, Ctx*) = 0;
    virtual bool isLabeledStmt() { return false; }
    virtual bool isLabelSetStmt() { return false; }
    virtual bool isVardefStmt() { return false; }
    uint32_t pos;

    static void cogenProgramBody(Cogen* cogen, Seq<Stmt*>* stmts);
    static void cogenFunctionBody(Cogen* cogen, Seq<Stmt*>* stmts);
};

class EmptyStmt : public Stmt {
public:
    virtual void cogen(Cogen* cogen, Ctx* ctx);
};

class BlockStmt : public Stmt {
public:
    BlockStmt(Seq<Stmt*>* stmts) : stmts(stmts) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Seq<Stmt*>* const stmts;
};

class ExprStmt : public Stmt {
public:
    ExprStmt(uint32_t pos, Expr* expr) : Stmt(pos), expr(expr) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
};

class VardefStmt: public ExprStmt {
public:
    VardefStmt(uint32_t pos, Expr* expr) : ExprStmt(pos, expr) {}
    virtual bool isVardefStmt() { return true; }
};
    
class LabeledStmt : public Stmt {
public:
    LabeledStmt(Str* label, Stmt* stmt) : label(label), stmt(stmt), address(NULL) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    virtual bool isLabeledStmt() { return true; }
    Str* const label;
    Stmt* const stmt;
    Label* address;     // Used for "goto" processing; updated by a prepass
};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(uint32_t pos, Expr* expr) : Stmt(pos), expr(expr) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;  // or NULL
};

class BreakStmt : public Stmt {
public:
    BreakStmt(uint32_t pos, Str* label) : Stmt(pos), label(label) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Str* const label;   // or NULL
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt(uint32_t pos, Str* label) : Stmt(pos), label(label) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Str* const label;   // or NULL
};

class GotoStmt : public Stmt {
public:
    GotoStmt(uint32_t pos, Str* label) : Stmt(pos), label(label) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Str* const label;   // never NULL
};

class IfStmt : public Stmt {
public:
    IfStmt(uint32_t pos, Expr* expr, Stmt* consequent, Stmt* alternate)
        : Stmt(pos)
        , expr(expr)
        , consequent(consequent)
        , alternate(alternate)
    {
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
    Stmt* const consequent;
    Stmt* const alternate;  // or NULL
};

class LabelSetStmt : public Stmt {
public:
    LabelSetStmt(uint32_t pos=0) : Stmt(pos), labels(NULL) {}
    virtual bool isLabelSetStmt() { return true; }
    Seq<Str*>* labels;
};

class WhileStmt : public LabelSetStmt {
public:
    WhileStmt(uint32_t pos, Expr* expr, Stmt* body) : LabelSetStmt(pos), expr(expr), body(body) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
    Stmt* const body;
};

class DoWhileStmt : public LabelSetStmt {
public:
    // pos should be the position of the "while" keyword, roughly, because it's the expression
    // that will go wrong if anything
    DoWhileStmt(uint32_t pos, Expr* expr, Stmt* body) : LabelSetStmt(pos), expr(expr), body(body) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
    Stmt* const body;
};

class ForStmt : public LabelSetStmt {
public:
    ForStmt(uint32_t pos, Expr* init, Expr* test, Expr* update, Stmt* body)
        : LabelSetStmt(pos)
        , init(init)
        , test(test)
        , update(update)
        , body(body)
    {
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr * const init;
    Expr * const test;
    Expr * const update;
    Stmt * const body;
};

class ForInStmt : public LabelSetStmt {
public:
    ForInStmt(uint32_t pos, Expr* lhs, Expr* init, Expr* obj, Stmt* body, bool is_each)
        : LabelSetStmt(pos)
        , is_each(is_each)
        , lhs(lhs)
        , init(init)
        , obj(obj)
        , body(body)
    {
    }
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    const bool is_each;
    Expr * const lhs;
    Expr * const init;
    Expr * const obj;
    Stmt * const body;
};

class SwitchStmt : public Stmt {
public:
    SwitchStmt(uint32_t pos, Expr* expr, Seq<CaseClause*>* cases) : Stmt(pos), expr(expr), cases(cases) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    bool analyze(int32_t* low, int32_t* high);
    void cogenFast(Cogen* cogen, Ctx* ctx, int32_t low, int32_t high);
    void cogenSlow(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
    Seq<CaseClause*>* const cases;
};

class CaseClause {
public:
    CaseClause(uint32_t pos, Expr* expr) : pos(pos), expr(expr), stmts(NULL) {}
    uint32_t pos;
    Expr* const expr;   // NULL denotes a default clause
    Seq<Stmt*>* stmts;
};

class ThrowStmt : public Stmt {
public:
    ThrowStmt(uint32_t pos, Expr* expr) : Stmt(pos), expr(expr) {};
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
};

class TryStmt : public Stmt {
public:
    TryStmt(Seq<Stmt*>* tryblock, Seq<CatchClause*>* catches, Seq<Stmt*>* finallyblock) : tryblock(tryblock), catches(catches), finallyblock(finallyblock) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    void cogenWithFinally(Cogen* cogen, Ctx* ctx);
    void cogenNoFinally(Cogen* cogen, Ctx* ctx);
    void cgCatch(Cogen* cogen, Ctx* ctx, uint32_t code_start, uint32_t code_end, Label* Lend, CatchClause* catchClause);
    Seq<Stmt*>* const tryblock;
    Seq<CatchClause*>* const catches;
    Seq<Stmt*>* const finallyblock;
};

class CatchClause {
public:
    CatchClause(Str* name, Type* type_name, Seq<Stmt*>* stmts) : name(name), type_name(type_name), stmts(stmts) {}
    Str* const name;
    Type* const type_name;
    Seq<Stmt*>* const stmts;
};

class WithStmt : public Stmt {
public:
    WithStmt(uint32_t pos, Expr* expr, Stmt* body) : Stmt(pos), expr(expr), body(body) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
    Stmt* const body;
};

// This is for base class constructor calls; base class method calls are represented
// as ExprStmts containing ObjectRef nodes with a SuperExpr left-hand-side and a CallExpr
// right-hand-side.
class SuperStmt : public Stmt {
public:
    SuperStmt(uint32_t pos, Seq<Expr*>* arguments) : Stmt(pos), arguments(arguments) {}
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Seq<Expr*>* const arguments;
};

class DefaultXmlNamespaceStmt : public Stmt {
public:
    DefaultXmlNamespaceStmt(uint32_t pos, Expr* expr) : Stmt(pos), expr(expr) {};
    virtual void cogen(Cogen* cogen, Ctx* ctx);
    Expr* const expr;
};

// Base class for various kinds of namespace values
class Namespace {
public:
    virtual ~Namespace() {}
    virtual Tag tag() const = 0;
};

// A string-based namespace.
class CommonNamespace : public Namespace {
public:
    CommonNamespace(Str* name) : name(name) {}
    virtual Tag tag() const { return TAG_commonNamespace; }
    Str* const name;
};

// A reference to a namespace binding, must be resolved at compile time
class NamespaceRef : public Namespace {
public:
    NamespaceRef(Str* id) : id(id) {}
    virtual Tag tag() const { return TAG_namespaceRef; }
    Str* const id;
};

class Parser {
public:
    Parser(Compiler* compiler, Lexer* lexer, bool public_by_default, uint32_t first_line=1);
    
    Program* parse();
    Token onEOS(uint32_t* , TokenValue* valuep);
    
    // Utilities shared with AST node constructors
    
    // Return a value representing the internal namespace in an AST.
    BuiltinNamespace* internalNamespace();

    // Return a value representing the default namespace in an AST.  What it is,
    // public or internal, depends on "public_by_default".
    BuiltinNamespace* defaultNamespace();

private:
    Compiler * const compiler;
    Allocator * const allocator;
    const bool public_by_default;
    const uint32_t line_offset;

    // Parser attributes as bit vectors.  A bit vector of all 0s is the "normal"
    // case, the flags modify that.
    
    enum {
        EFLAG_NoIn = 1,     // Expression parsing: do not allow the use of the "in" operator
    };
    
    enum {
        SFLAG_Toplevel = 1, // Statement and directive parsing: at top level
        SFLAG_Package = 2,  // Statement and directive parsing: at package level
        SFLAG_Function = 4, // Statement and directive parsing: at function level
        SFLAG_Class = 8,
        SFLAG_Interface = 16,
        SFLAG_Block = 32,   // Statement and directive parsing: at block level inside one of the above
    };

    // Program, etc parser
    
    Program* program();
    void package();
    void directives(int flags);
    void includeDirective();
    void classDefinition(bool config, int flags, Qualifier* qual);
    void interfaceDefinition(bool config, int flags, Qualifier* qual);
    void namespaceDefinition(bool config, int flags, Qualifier* qualifier);
    void configNamespaceDefinition(int flags, bool config);
    void functionDefinition(bool config, Qualifier* qualifier, bool getters_and_setters, bool require_body);
    Stmt* variableDefinition(bool config, Qualifier* qualifier);
    FunctionDefn* functionGuts(Qualifier* qual, bool require_name, bool getters_and_setters, bool require_body);
    Expr* varBindings(uint32_t* pos, NameComponent* ns, bool is_const, bool is_static, int flags=0, uint32_t* numbindings=NULL, Expr** firstName=NULL);
    void checkSimpleAttributes(Qualifier* qual);

    Seq<Stmt*>* statementBlock(bool config=true);
    
    // Expression parser
    //
    // OPTIMIZEME: Make the parser work by operator precedence so that we have only
    // a small number of recursive levels from "commaExpression" to "nameExpression"
    // or "primaryExpression" (the common paths).
    
    Type* typeExpression();
    Expr* functionExpression();
    QualifiedName* nameExpression(bool is_attr=false);
    Expr* propertyOperator(Expr* obj);
    Expr* objectInitializer();
    Seq<LiteralField*>* fieldList();
    LiteralField* literalField() ;
    Expr* arrayInitializer();
    Seq<Expr*>* elementList();
    Expr* vectorInitializer(uint32_t pos);
    Expr* primaryExpression();
    QualifiedName* attributeIdentifier();
    QualifiedName* propertyIdentifier();
    Seq<Expr*>* argumentList();
    Expr* memberExpression();
    Expr* memberExpressionPrime(Expr* expr);
    Expr* superExpression();
    Expr* callExpression();
    Expr* callExpressionPrime(Expr* call_expr);
    Expr* newExpression (int new_count=0);
    Expr* leftHandSideExpression();
    Expr* postfixExpression();
    Expr* unaryExpression();
    Expr* multiplicativeExpression();
    Expr* additiveExpression();
    Expr* shiftExpression();
    Expr* relationalExpression(int flags);
    Expr* equalityExpression(int flags);
    Expr* bitwiseAndExpression(int flags);
    Expr* bitwiseXorExpression(int flags);
    Expr* bitwiseOrExpression(int flags);
    Expr* logicalAndExpression(int flags);
    Expr* logicalOrExpression(int flags);
    Expr* nonAssignmentExpression(int flags);
    Expr* conditionalExpression(int flags);
    Expr* assignmentExpression(int flags);
    Expr* commaExpression(int flags);
    
    Expr* exprListToCommaExpr(Seq<Expr*>* es);
    Expr* parenExpression();
    
    // Statement parser
    
    Stmt* statement(bool config=true);
    Stmt* labeledStatement();
    Stmt* returnStatement();
    Stmt* breakStatement();
    Stmt* continueStatement();
    Stmt* gotoStatement();
    Str* breakOrContinueLabel(Token t);
    Stmt* defaultXmlNamespaceStatement();
    Stmt* ifStatement();
    Stmt* whileStatement();
    Stmt* doStatement();
    Stmt* forStatement();
    Stmt* switchStatement();
    Seq<CaseClause*>* caseElements();
    Stmt* throwStatement();
    Stmt* tryStatement();
    Seq<CatchClause*>* catches();
    CatchClause* catchClause();
    Stmt* useStatement();
    Stmt* importStatement();
    Stmt* varStatement(NameComponent* ns, bool is_const, bool is_static);
    Stmt* withStatement();
    Stmt* superStatement();

    // Xml initializers
    
    Expr* xmlInitializer();
    void xmlListInitializer(XmlContext* ctx);
    void xmlMarkup(XmlContext* ctx);
    void xmlElement(XmlContext* ctx);
    void xmlTagName(XmlContext* ctx);
    void xmlAttributes(XmlContext* ctx);
    void xmlElementContent(XmlContext* ctx);
    void xmlExpression(XmlContext* ctx, Escapement esc);
    void xmlAssert(XmlContext* ctx, Token t, Escapement esc=ESC_none);
    void xmlEscape(XmlContext* ctx, const wchar* cs, const wchar* limit, bool is_attr);
    void xmlPushback(wchar c);
    void xmlAtom();
    void xmlAtomSkipSpace();

    // Sundry low-level parsers
    
    Str* identifier();
    void semicolon();
    bool noNewline();
    bool newline();
    uint32_t position();
    
    // Sundry utilities
    
    bool isAdditive(Token t);
    bool isRelational(Token t, bool in_allowed);
    bool isShift(Token t);
    bool isRelationalType(Token t);
    bool isOpAssign(Token t);
    bool isMultiplicative(Token t);
    bool isEquality(Token t);
    Unop tokenToUnaryOperator(Token t);
    Binop tokenToBinaryOperator(Token t);
    Str* doubleToStr(double d);
    double strToDouble(Str* s);
    bool isNamespaceReference(Expr* e);
    void addExprStatements(SeqBuilder<Stmt*>* stmts, Seq<Expr*>* exprs);
    Seq<Binding*>* bindingsToVars(Seq<Binding*>* bindings);
    Seq<FunctionDefn*>* filterConstructor(Str* name, Seq<FunctionDefn*>* fns, FunctionDefn** constructor);
    Seq<Stmt*>* filterStatements(Seq<Stmt*>* stmts, Seq<Stmt*>** nonDefinitionStmts);
    FunctionDefn* constructClassConstructor(uint32_t pos, Str* name, BodyInfo* old_body, Seq<Stmt*>* init_stmts, Seq<Stmt*>* static_stmts);
    FunctionDefn* constructInstanceConstructor(uint32_t pos, Str* name, Seq<Stmt*>* init_stmts, FunctionDefn* constructor);
    NameComponent* qualToNamespace(Qualifier* qual);
    
    BuiltinNamespace* const internalNS;
    BuiltinNamespace* const privateNS;
    BuiltinNamespace* const protectedNS;
    BuiltinNamespace* const publicNS;

    // Binding management
    
    enum RibType {
        RIB_Program,
        RIB_Class,
        RIB_Instance,
        RIB_Function
    };
    
    class BindingRib {
    public:
        BindingRib(Allocator* allocator, BindingRib* next, RibType tag);

        const RibType tag;
        SignatureInfo* signature;   // NULL unless tag == RIB_Function
        BodyInfo body;
        BindingRib* next;
    };
    
    BindingRib* topRib;
    SeqBuilder<TypeDefn*> classes;

    Binding* findBinding(NameComponent* ns, Str* name, BindingKind kind, BindingRib* rib=NULL);
    void pushBindingRib(RibType tag);
    void popBindingRib();
    void addBinding(Str* name, BindingKind kind);
    void addVarBinding(NameComponent* ns, Str* name, Type* type_name, BindingRib* rib=NULL);
    void addConstBinding(NameComponent* ns, Str* name, Type* type_name, BindingRib* rib=NULL);
    void addNamespaceBinding(NameComponent* ns, Str* name, Expr* expr);
    void addFunctionBinding(NameComponent* ns, FunctionDefn* fn);
    void addMethodBinding(NameComponent* ns, FunctionDefn* fn, BindingRib* rib);
    void addClass(ClassDefn* cls);
    void addInterface(InterfaceDefn* iface);
    void addOpenNamespace(Namespace* ns);
    void addUnqualifiedImport(Seq<Str*>* name); // import a.b.*; name would have length 2
    void addQualifiedImport(Seq<Str*>* name);   // import a.b.c; name would have length 3
    void setUsesFinally();
    void setUsesCatch();
    void setUsesArguments();
    void setUsesDefaultXmlNamespace();
    void setUsesGoto();
    void setUsesSuper();

    // Program configuration management

    class ConfigBinding {
    public:
        ConfigBinding(Str* ns, Str* name, Expr* value);
        Str* ns;        // Will be found among the configNamespaces
        Str* name;      // Unique within ns
        Expr* value;    // LiteralUndefined, LiteralNull, LiteralBoolean, LiteralString, LiteralInt, LiteralUInt, LiteralDouble
    };
    
    Seq<Str*>* configNamespaces;
    Seq<ConfigBinding*>* configBindings;
    
    void addConfigNamespace(Str* ns);
    void checkNoShadowingOfConfigNamespaces(uint32_t pos, Str* s);
    void addConfigBinding(Str* ns, Str* name, Expr* value);
    bool isConfigReference(Expr* e);        // e is a QualifiedName with non-NULL SimpleName parts, and the ns names a known config ns
    bool findConfigNamespace(Str* ns);
    Expr* findConfigBinding(Str* ns, Str* name);
    bool evaluateConfigReference(QualifiedName* e);
    
    // Constant evaluator, used by program configuration management.
    // Could fairly easily be generalized into its own class, parameterized
    // by an environment abstraction.

    Expr* evaluateConfigDefinition(Str* ns, Expr* e);
    uint32_t evaluateToUInt32(Expr* e);
    int32_t evaluateToInt32(Expr* e);
    double evaluateToNumber(Expr* e);
    bool evaluateToBoolean(Expr* e);
    Str* evaluateToString(Expr* e);
    int evaluateRelational(Expr* lhs, Expr* rhs);
    void failNonConstant(Expr* e);

    // Utilities used by constant evaluator, pretty general but not used elsewhere.

    Expr* boxDouble(double n);
    Expr* boxFloat(float n);
    Expr* boxUInt(uint32_t n);
    Expr* boxInt(int32_t n);
    Expr* boxBoolean(bool b);
    Expr* boxString(const char* s);
    Expr* boxString(Str* s);
    Expr* boxUndefined();

    // Lexer can be updated by "include" processing, suspended lexers are
    // pushed onto the lexerStack.
    
    class LexerState {
    public:
        Lexer* lexer;
        Token T0, T1;
        TokenValue V0, V1;
        uint32_t LP, L0, L1;
        const wchar* included_input;
        LexerState* next;
    };
    
    void pushLexerState();
    void popLexerState();
    
    LexerState* lexerStack;

    // Token queue, see implementation for extensive comments
    
    Lexer * lexer;
    
    Token T0, T1;
    TokenValue V0, V1;
    uint32_t LP;
    uint32_t L0;
    uint32_t L1;
    const wchar* included_input;

    void start();
    Token divideOperator();
    Token regexp();
    Token rightAngle();
    Token leftAngle();
    Token rightShiftOrRelationalOperator();
    Token leftShiftOrRelationalOperator();
    Token hd();
    Token hd2();
    void next();
    void eat(Token t);
    bool match(Token t);
    Str* xmlValue();
    Str* stringValue();
    Str* identValue();
    Str* regexValue();
    int32_t intValue();
    uint32_t uintValue();
    double doubleValue();
    float floatValue();
    
    // Mapping from subset of tokens to attributes and operator values
    static const struct TokenMapping {
        unsigned isOpAssign:1;
        unsigned isMultiplicative:1;
        unsigned isAdditive:1;
        unsigned isRelational:1;
        unsigned isEquality:1;
        unsigned isShift:1;
        unsigned unaryOp:8;
        unsigned binaryOp:8;
    } tokenMapping[];
};

}}
