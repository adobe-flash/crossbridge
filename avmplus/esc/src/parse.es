/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Parse,
    namespace Parse;

class Pattern {
}

class FieldPattern extends Pattern {
    const ident: Ast::IdentExpr;
    const ptrn: Pattern;
    function FieldPattern (ident,ptrn)
        : ident = ident
        , ptrn = ptrn { }
}

class ObjectPattern extends Pattern {
    const ptrns : [...FieldPattern];
    function ObjectPattern (ptrns)
        : ptrns = ptrns { }
}

class ArrayPattern extends Pattern {
    const ptrns : [...Pattern];
    const spread: ? Pattern;
    function ArrayPattern (ptrns, spread)
        : ptrns = ptrns 
        , spread = spread { }
}

class SimplePattern extends Pattern {
    const expr : Ast::Expr;
    function SimplePattern (expr)
        : expr = expr { }
}

class IdentifierPattern extends Pattern {
    const ident : Ast::IDENT;
    function IdentifierPattern (ident)
        : ident = ident { }
}

class ALPHA extends Util::ENUM { function ALPHA(s) : super(s) {} }
const noColon = new ALPHA("noColon");
const allowColon = new ALPHA("allowColon");

class BETA extends Util::ENUM { function BETA(s) : super(s) {} }
const noIn = new BETA("noIn");
const allowIn = new BETA("allowIn");

class GAMMA extends Util::ENUM { function GAMMA(s) : super(s) {} }
const noExpr = new GAMMA("noExpr");
const allowExpr = new GAMMA("allowExpr");

class TAU extends Util::ENUM { function TAU(s) : super(s) {} }
const globalBlk = new TAU("globalBlk");
const classBlk = new TAU("classBlk");
const interfaceBlk = new TAU("interfaceBlk");
const localBlk = new TAU("localBlk");
const statementBlk = new TAU("statementBlk");
const constructorBlk = new TAU("constructorBlk");

class OMEGA extends Util::ENUM { function OMEGA(s) : super(s) {} }
const fullStmt = new OMEGA("fullStmt");
const abbrevStmt = new OMEGA("abbrevStmt");

type NamespaceExpr = (Ast::IdentExpr | Ast::LiteralString);

function extractInfo(ctx) {
    let filename = "";
    let position = 0;
    if (ctx is Definer)
        ctx = ctx.parser;
    if (ctx is Parser) {
        filename = ctx.scan.filename;
        position = ctx.position();
    }
    return [filename, position];
}

function syntaxError(ctx, msg) {
    let [filename, position] = extractInfo(ctx);
    Util::syntaxError(filename, position, msg);
}

function internalError(ctx, msg) {
    let [filename, position] = extractInfo(ctx);
    Util::internalError(filename, position, msg);
}

function warning(ctx, msg) {
    let [filename, position] = extractInfo(ctx);
    Util::warning(filename, position, msg);
}

final class Parser
{
    const cx      : Definer;
    const scan    : Scanner;
    const filename: String;
    const start_line;

    function Parser(src, topFixtures, filename="", start_line=1) 
        : cx = new Definer(this, topFixtures)
        , scan = new Lex::Scanner(src, filename)
        , filename = filename
        , start_line = start_line-1
    { }

    var currentClassName: Token::Tok = Token::sym_EMPTY;
    var alpha           : ALPHA = allowColon;
    var beta            : BETA = allowIn;
    var gamma           : GAMMA = allowExpr;
    var tau             : TAU = globalBlk;
    var omega           : OMEGA = fullStmt;

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
    //   T0 is NONE only before start() and when the stream is exhausted.
    //   T1 is NONE whenever it's invalid
    //   L1 is invalid iff T1 is invalid
    //   LP, L0, and L1 are 0 if there is "no information"
    //   The scanner never returns Eol
    //
    // Every token passes through this interface so we try to save time
    // when possible.
    //
    // Current optimizations:
    //   * avoid function calls
    //     - scan.start() is shallow
    //     - Token::tokenKind and Token::tokenText have been in-lined
    //   * avoid allocations
    //     - Communicate with lexer via multiple variables rather than 
    //       using multiple return values
    //
    // Future optimizations (maybe):
    //   - With static typing to "uint" it may be possible to avoid write 
    //     barriers on assignments to T0, T1, LP, L0, and L1
    //   - With tokenStore typed as Vector.<uint> it may be possible to
    //     avoid range checks and checks for holes
    //
    // What we need the compiler to do:
    //   - Early binding to instance vars and methods is important
    //   - We would probably benefit from the compiler inlining hd
    //     everywhere; we can declare it "final" or just make Parser
    //     itself "final"

    var T0=Token::NONE, T1=Token::NONE, LP=0, L0=0, L1=0;
    const tokenStore = Token::tokenStore;

    function start() {
        T0 = scan.start();
        L0 = scan.lnCoord;
    }

    function div() {
        Util::assert( T0 === Token::BREAK_SLASH && T1 === Token::NONE );
        T0 = scan.div();
        L0 = scan.lnCoord;
        return hd();
    }

    function regexp() {
        Util::assert( T0 === Token::BREAK_SLASH && T1 === Token::NONE );
        T0 = scan.regexp();
        L0 = scan.lnCoord;
        return hd();
    }

    function brocket() {
        Util::assert( T0 === Token::BREAK_RBROCKET && T1 === Token::NONE );
        T0 = scan.brocket();
        L0 = scan.lnCoord;
        return hd();
    }

    function shiftOrRelational() {
        Util::assert( T0 === Token::BREAK_RBROCKET && T1 === Token::NONE );
        T0 = scan.shiftOrRelational();
        L0 = scan.lnCoord;
        return hd();
    }

    function hd ()
        tokenStore[T0].kind;

    function lexeme ()
        tokenStore[T0];

    function hd2 () {
        if (T1 === Token::NONE) {
            T1 = scan.start();
            L1 = scan.lnCoord;
        }
        return tokenStore[T1].kind;
    }

    function lexeme2 () {
        hd2 ();
        return tokenStore[T1];
    }

    function matchToken (t) {
        if (T0 == t) {
            next();
            return true;
        }
        return false;
    }

    function next () {
        LP = L0;
        T0 = T1;
        L0 = L1;
        T1 = Token::NONE;
        if (T0 === Token::NONE) {
            T0 = scan.start();
            L0 = scan.lnCoord;
        }
    }

    function replace (t) {
        T0 = t;
    }

    function newline ()
        LP < L0;

    function position ()
        L0 + start_line;

    function tokenText(t)
        tokenStore[t].text;

    // Token queue abstractions
    // END

    // If the current token is not tc then throw an error.
    // Otherwise consume the token.

    function eat (tc) {
        let tk = hd ();
        if (tk !== tc) {
            let desc = tokenText(tk);
            if (Token::hasLexeme(tk))
                desc += " '" + tokenText(T0) + "'";
            Parse::syntaxError(this, "Expecting " + tokenText(tc) + " found " + desc);
        }
        next ();
    }

    function match (tc) {
        let tk = hd ();
        if (tk !== tc)
            return false;
        next ();
        return true;
    }

    function makeIdentifier(id : Token::Tok, nss, pos=0) : Ast::Identifier {
        Util::assert( id is Token::Tok );
        let ident = new Ast::Identifier(id, nss, pos);
        if (id === Token::sym_arguments) 
            cx.topFunction().uses_arguments = true;
        else if (id === Token::sym_eval)
            cx.topFunction().uses_eval = true;
        return ident;
    }

    /*

    Notation

    []             list
    (fl,el)        head
    fl             fixture list
    el             expr list
    il             init list
    sl             stmt list
    it             init target = VAR, LET (default=LET)
    ie             init expr
    se             set expr

    initexpr       init it (fl,el) il
    letexpr        let (fl,el) el
    block          (fl,el) sl

      

    Bindings

    var x = y      [x], set x=y
    var [x] = y    [x], let ([t0],[t0=y]) set x=t0[0]

    let (x=y) ...  let ([x], [x=y]) ...
    let x=y        let ([x]) set x=y

    Assignments

    x = y          [],  set x=y
    [x] = y        [],  let ([t0],[t0=y]) set x=t0[0]

    Blocks

    { }            () {}
    {stmt}         () {stmt}
    {let x}        ([x],[x=undef]) {}       is undef the right val?
    let (x) {}     ([x],[x=undef]) {}       what about reping uninit?

    Mixture

    { var x = y; let z = y }  =>
    ([x],[]) { blkstmt ([z],[]) { init VAR () x=y; init LET () z=y } }


    assignment, create a let for each aggregate, a temp for
    each level of nesting

    x = y              set x=y
    [x] = y            let (t0=y) set x=t0[0]
    [[x]] = y          let (t0=y) let (t1=t0[0]) set x=t1[0]
    [[x],[x]] = y      let (t0=y) let (t1=t0[0]) set x=t1[0]
    , let (t1=t0[1]) set x=t1[0]
        
    initialization, create an init rather than a set for the
    leaf nodes

    var x = v            let (t0=v) init () [x=t0]
    var [x] = v          let (t0=v) init () [x=t0[0]]
    var [x,[y,z]] = v    let (t0=v) init () [x=t0[0]]
                                  , let (t1=t0[1]) init () [y=t1[0]
                                                 , z=t1[1]]

    var [x,[y,[z]]] = v  let (t0=v) init () [x=t0[0]]
    , let (t1=t0[1]) init () [y=t1[0]
    , let (t2=t1[0]) init () [z=t2[0]]

    for initialization, we need to know the namespace and the target 
    so we make INITS to go into the InitExpr inside the LetExpr

    let x = y          init x=y

    flattening.

    var [x,[y,z]] = v  let (t0=v) init () [x=t0[0]]
    , let (t1=t0[1]) init () [y=t1[0], z=t1[0]]

    t0=v
    x=t0[0]
    t1=t0[1]
    y=t1[0]
    z=t1[1]
    head = {[t0,x,t1,y,z],

    flattening doesn't work because it mixes named and temporary
    fixtures

    lets and params have the same problem. both allow destructuring
    patterns that can expand into a nested expression.

    let ([x,[y,z]]=v) ...

    top heads only have named fixtures. sub heads only have temporaries.
    temporaries are always immediately initialized. a head is a list of
    fixtures and a list of expressions. the expressions get evaluated
    in the scope outside the head.

    settings is a sub head. it has temporary fixtures and init exprs that
    target instance variables

    */

    function desugarAssignmentPattern (p: Pattern, t: Ast::TypeExpr, e: Ast::Expr, op: Ast::ASSIGNOP) : [[Ast::Fixture], Ast::Expr]
        desugarPattern (p,t,e,null,null,null,op);

    function desugarBindingPattern (p: Pattern, t: Ast::TypeExpr, e: ? Ast::Expr,
                                    ns: ? Ast::Namespace, it: ? Ast::INIT_TARGET, ro: ? Boolean)
        : [[Ast::Fixture], Ast::Expr]
        desugarPattern (p,t,e,ns,it,ro,null);

    function desugarPattern (p: Pattern, 
                             t: Ast::TypeExpr, 
                             e: ? Ast::Expr,
                             ns: Ast::Namespace, 
                             it: ? Ast::INIT_TARGET, 
                             ro: ? Boolean, 
                             op: ? Ast::ASSIGNOP) : [[Ast::Fixture], Ast::Expr] 
    {
        return desugarSubPattern (p,t,e,0);

        function identExprFromExpr (e: Ast::Expr) : Ast::IdentExpr {
            if (!(e is Ast::IdentExpr))
                Parse::syntaxError(this, "Invalid initializer left-hand-side " + e);
            return e;
        }

        function desugarSubPattern (p: Pattern, t: Ast::TypeExpr, e: ? Ast::Expr, n: int) : [[Ast::Fixture], Ast::Expr] {
            let fxtrs, exprs, expr;
            switch type (p) {
            case (p:IdentifierPattern) {
                let nm = new Ast::PropName (new Ast::Name(ns, p.ident));
                let fx = new Ast::ValFixture (t, ro);
                fxtrs = [new Ast::Fixture(nm, fx)];
                let inits = (e !== null) ? ([new Ast::InitBinding(nm, e)]) : []; // FIXME (fix what??)
                expr = new Ast::InitExpr (it, new Ast::Head ([],[]), inits);
            }
            case (p:SimplePattern) {
                if (e === null) 
                    Parse::syntaxError(this, "Simple pattern without initializer");
                fxtrs = [];
                if (it != null) { // we have an init target so must be an init
                    let ie = identExprFromExpr (p.expr);
                    let nm = cx.resolveIdentExpr (ie,it);
                    expr = new Ast::InitExpr (it, new Ast::Head ([],[]), [new Ast::InitBinding(nm,e)]);
                }
                else {
                    expr = new Ast::SetExpr (op, p.expr, e);
                }
            }
            //case (p: (ArrayPattern|ObjectPattern)) {
            case (p: *) {
                let tn = new Ast::TempName (n);
                fxtrs = [];
                exprs = [];
                if (p is ArrayPattern && p.spread != null)
                    Parse::internalError(this, "Unimplemented spread in ArrayPattern");
                let ptrns = p.ptrns;
                for (let i=0, limit=ptrns.length ; i < limit ; ++i) {
                    let sub = ptrns[i];
                    let typ, exp, ptn;
                    /// switch type (sub) {
                    /// case (sub: FieldPattern) {
                    if (sub is FieldPattern) {
                        typ = new Ast::FieldTypeRef (t, sub.ident);
                        exp = new Ast::ObjectRef (new Ast::GetTemp (n), sub.ident);
                        ptn = sub.ptrn;
                    }
                    /// case (pat: *) {
                    else {
                        typ = new Ast::ElementTypeRef (t,i);
                        exp = new Ast::ObjectRef (new Ast::GetTemp (n), makeIdentifier( Token::intern(i), Ast::publicNSSL ));
                        // FIXME what is the ns of a temp and how do we refer it
                        ptn = sub;
                    }
                    /// }

                    let [fx,ex] = desugarSubPattern (ptn,typ,exp,n+1);
                    Util::pushOnto(fxtrs, fx);
                    exprs.push(ex);
                }
                let head = new Ast::Head ([new Ast::Fixture(tn, new Ast::ValFixture (Ast::anyType,false))],
                                          [new Ast::InitExpr (Ast::letInit, new Ast::Head([],[]), [new Ast::InitBinding(tn,e)])]);
                expr = new Ast::LetExpr (head, exprListToCommaExpr(exprs));
            }
            }
            return [fxtrs,expr];
        }
    }

    // Parse rountines

    function identifier () : Ast::IDENT {
        if (hd() == Token::Identifier)
            return let (str = lexeme()) next(), str;
        Parse::syntaxError(this, "Expecting identifier, found " + lexeme().text);
    }

    function propertyIdentifier(): Ast::IDENT {
        if (hd() == Token::Identifier || Token::isReserved(hd()))
            return let (str = lexeme()) next(), str;
        Parse::syntaxError(this, "Expecting identifier or reserved word, found " + lexeme().text);
    }
  
    // Returns IdentExpr or LiteralString, consumer should cope.
    function namespaceExpression() {
        if (hd() == Token::StringLiteral)
            return let (expr = new Ast::LiteralString (lexeme(), position())) next(), expr;
        let name = nameExpression();
        if (!(name is Ast::IdentExpr))
            Parse::syntaxError(this, "Qualified name or identifier required, found " + name);
        return name;
    }

    function nameExpression(): Ast::IdentExpr {
        let name;

        let pos = position();
        if (hd() == Token::StringLiteral)
            name = let (s = new Ast::LiteralString(lexeme(), pos)) next(), s;
        else if (hd() == Token::Identifier) 
            name = makeIdentifier(identifier(), cx.getOpenNamespaces(), pos);
        else
            Parse::syntaxError(this, "String or identifier required, found " + lexeme().text);

        // Excludes slice expression shorthands.  Messy!
        while (hd() == Token::DoubleColon && hd2() != Token::RightBracket) {
            eat(Token::DoubleColon);
            // Technically the resolution here is necessary, and when we clean up
            // namespaces we can't do without.  Right now, it slows down parsing
            // by a factor of 2, but namespace search can be made more clever
            // to fix that.
            pos = position();
            name = new Ast::QualifiedIdentifier(cx.resolveNamespaceExpr(name), propertyIdentifier(), pos);
        }

        return name;
    }

    function parenExpression () : Ast::Expr {
        eat (Token::LeftParen);
        let expr = fullCommaExpression ();
        eat (Token::RightParen);

        return expr;
    }

    function exprListToCommaExpr(es) {
        let expr = es[0];
        for ( let i=1, limit=es.length ; i < limit ; i++ )
            expr = new Ast::BinaryExpr(Ast::commaOp, expr, es[i]);
        return expr;
    }

    function initializerAttribute() {
        let var_prefix = false;
        let const_prefix = false;

        if (match(Token::Var))
            var_prefix = true;
        else if (match(Token::Const))
            const_prefix = true;
        return [var_prefix, const_prefix];
    }

    function objectInitializer () : Ast::TypeExpr {
        let [var_prefix, const_prefix] = initializerAttribute();
        let pos = position();             // Record source location of initial left brace
        eat (Token::LeftBrace);
        let fields = fieldList (var_prefix, const_prefix);
        eat (Token::RightBrace);

        // FIXME -- implement.
        //
        // SYNTACTIC CONDITION.  There cannot be a plain field for
        // which there is also a getter or setter.
        //
        // SYNTACTIC CONDITION.  Fixture fields cannot be duplicated.
        //
        // SYNTACTIC CONDITION.  The __proto__ field cannot be
        // duplicated.
        //
        // SYNTACTIC CONDITION.  Fields defined by getters or setters
        // can have at most one getter and at most one setter.

        if (cx.getStrict()) {
            // FIXME -- implement.
            // 
            // SYNTACTIC CONDITION.  In strict code, there can be no
            // duplicate field names at all.
        }

        let texpr = alpha==allowColon && match(Token::Colon) ? typeExpression () : Ast::anyType;

        return new Ast::LiteralObject (fields, texpr, pos);
    }

    function fieldList (var_prefix, const_prefix) : [(Ast::LiteralField | Ast::ProtoField | Ast::VirtualField)] {
        let fields = [] : [(Ast::LiteralField | Ast::ProtoField | Ast::VirtualField)];
        if (hd () != Token::RightBrace) {
            do
                fields.push (literalField(var_prefix, const_prefix));
            while (match(Token::Comma));
        }
        return fields;
    }

    function literalField (var_prefix, const_prefix) : Ast::FieldType {
        let [var_prefix2, const_prefix2] = initializerAttribute();

        // SYNTACTIC CONDITION.  If there is a var or const prefix to
        // the structure as a whole then there can't be one here.

        if (var_prefix || const_prefix) {
            if (var_prefix2 || const_prefix2)
                Parse::syntaxError(this, "Redundant field prefix '" + (var_prefix2 ? "var" : "const") + "'");
        }
        else
            [var_prefix,const_prefix] = [var_prefix2,const_prefix2];

        let tag = Ast::noTag;
        if (var_prefix) tag = Ast::varTag;
        if (const_prefix) tag = Ast::constTag;

        if (match(Token::__Proto__)) {
            match(Token::Colon);
            let expr = fullAssignmentExpression();
            return new Ast::ProtoField(expr);
        }

        let fn = fieldName();

        if (fn is Ast::Identifier) {
            if (fn.ident == Token::sym_get) {
                let getter = getterOrSetterFieldMaybe(tag, Ast::getterFunction);
                if (getter)
                    return getter;
            }
            else if (fn.ident == Token::sym_set) {
                let setter = getterOrSetterFieldMaybe(tag, Ast::setterFunction);
                if (setter)
                    return setter;
            }
        }

        // SYNTACTIC CONDITION.  The ": expr" suffix is optional
        // because this parsing routine is used also for the left hand
        // side of a destructuring assignment (it's not possible to
        // know what we're looking at until we see the assignment
        // operator following the right brace).  There is a check in
        // cogen-expr.es that tests whether the expression is null,
        // and which signals a SyntaxError if it is.  That check will
        // probably move into the definer when the definer becomes
        // available.

        let expr = match(Token::Colon) ? fullAssignmentExpression () : null;

        return new Ast::LiteralField (tag, fn, expr);
    }

    function getterOrSetterFieldMaybe(tag, kind) {
        if (hd() == Token::Colon || hd() == Token::Comma || hd() == Token::RightBrace)
            return null;

        let name = fieldName();
        let fn = functionExpression(kind);

        return new Ast::VirtualField(tag, name, kind, fn);
    }

    function fieldName () : Ast::IdentExpr {
        switch (hd ()) {
        case Token::StringLiteral:
            if (hd2() == Token::DoubleColon)
                break;
            return let (name = new Ast::Identifier(lexeme(), Ast::publicNSSL)) next(), name;

        case Token::IntLiteral:
        case Token::UIntLiteral:
            return let (name = new Ast::Identifier(Token::intern(parseInt(lexeme().text)), Ast::publicNSSL)) next(), name;

        case Token::DoubleLiteral:
            return let (name = new Ast::Identifier(Token::intern(parseFloat(lexeme().text)), Ast::publicNSSL)) next(), name;

        case Token::DecimalLiteral:
            return let (name = new Ast::Identifier(Token::intern(new decimal(lexeme().text)), Ast::publicNSSL)) next(), name;

        default:
            if (Token::isReserved(hd ()))
                return let (name = new Ast::Identifier(lexeme(), Ast::publicNSSL)) next(), name;
            break;
        }
        return nameExpression();
    }

    function arrayInitializer () : Ast::Expr {
        let [var_prefix, const_prefix] = initializerAttribute();
        let pos = position();   // Record source location of initial left bracket
        eat (Token::LeftBracket);
        let [elts, spread] = elementList ();

        if (elts.length == 1 && spread == null && (hd() == Token::For || hd() == Token::Let || hd() == Token::If)) {

            // FIXME -- implement.
            //
            // SYNTACTIC CONDITION: no name should be bound by more than one
            // comprehension clause.

            let comp = comprehensionExpression();
            eat (Token::RightBracket);
            let t = alpha==allowColon && match(Token::Colon) ? typeExpression () : Ast::anyType;
            return new Ast::LiteralComprehension(elts[0], comp, t, pos);
        }

        eat (Token::RightBracket);
        let t = alpha==allowColon && match(Token::Colon) ? typeExpression () : Ast::anyType;
        return new Ast::LiteralArray (elts, spread, t, pos);
    }

    function comprehensionExpression() {
        switch (hd()) {
        case Token::RightBracket:
            return null;
        case Token::For: {
            eat(Token::For);
            let is_each = matchToken(Token::id_each);
            eat(Token::LeftParen);
            let [pattern, annotation] = dynamic override (beta=noIn) typedPattern();
            let [fixtures,init] = desugarBindingPattern (pattern, annotation, null, cx.getDefaultNamespace(), null, false);
            eat(Token::In);
            let iterator = fullCommaExpression();
            eat(Token::RightParen);
            let subclause = comprehensionExpression();
            return new Ast::ComprehendFor(is_each, new Ast::Head(fixtures, init), iterator, subclause);
        }
        case Token::Let: {
            eat(Token::Let);
            eat(Token::LeftParen);
            let [fixtures, inits] = letBindingList();
            eat(Token::RightParen);
            let subclause = comprehensionExpression();
            return new Ast::ComprehendLet(new Ast::Head(fixtures, inits), subclause);
        }
        case Token::If: {
            eat(Token::If);
            let cond = parenExpression();
            let subclause = comprehensionExpression();
            return new Ast::ComprehendIf(cond, subclause);
        }
        default:
            Parse::syntaxError(this, "Expected 'for', 'let', 'if', or ']'; found " + lexeme().text);
        }
    }

    function elementList ()
        arrayElements(fullAssignmentExpression);

    function arrayElements(parser) {
        let spread = null;
        let elts = [];
        let elt = null;

        loop:
        for (;;) {
            switch (hd()) {
            case Token::RightBracket:
            case Token::For:
            case Token::Let:
            case Token::If:
                break loop;
            case Token::TripleDot:
                if (elt != null)
                    eat(Token::Comma);  // Generates the correct error
                eat(Token::TripleDot);
                spread = parser();
                break loop;
            case Token::Comma:
                let pos = position();
                eat(Token::Comma);
                if (elt == null)
                    elt = new Ast::LiteralUndefined(pos);
                elts.push(elt);
                elt = null;
                break;
            default:
                if (elt != null)
                    eat(Token::Comma);
                elt = parser();
                break;
            }
        }
        if (elt != null)
            elts.push(elt);

        return [elts, spread];
    }

    function primaryExpression() : Ast::Expr {
        if (hd() == Token::BREAK_SLASH)
            regexp();

        let pos = position();   // Record the source location before consuming the token
        switch (hd ()) {
        case Token::Null:
            return let (expr = new Ast::LiteralNull(pos)) next(), expr;

        case Token::True:
        case Token::False:
            return let (expr = new Ast::LiteralBoolean(hd() == Token::True, pos)) next(), expr;

        case Token::IntLiteral:
            return let (expr = new Ast::LiteralInt(parseInt(lexeme().text), pos)) next(), expr;

        case Token::UIntLiteral:
            return let (expr = new Ast::LiteralUInt(parseInt(lexeme().text), pos)) next(), expr;

        case Token::DoubleLiteral:
            return let (expr = new Ast::LiteralDouble(parseFloat(lexeme().text), pos)) next(), expr;

        case Token::DecimalLiteral:
            return let (expr = new Ast::LiteralDecimal(new decimal(lexeme().text), pos)) next(), expr;

        case Token::StringLiteral:
            if (hd2() == Token::DoubleColon)
                break;
            return let (expr = new Ast::LiteralString (lexeme(), pos)) next(), expr;

        case Token::RegexpLiteral:
            return let (expr = new Ast::LiteralRegExp(lexeme(), pos)) next(), expr;

        case Token::This:
            next();
            if (noNewline()) {
                if (matchToken(Token::id_generator))
                    return new Ast::ThisGeneratorExpr();
                if (match(Token::Function))
                    return new Ast::ThisFunctionExpr();
            }
            return new Ast::ThisExpr(cx.getStrict());

        case Token::LeftParen: {
            let e = parenExpression();
            // Excludes slice expression shorthands.  Messy!
            if (hd() == Token::DoubleColon && hd2() != Token::RightBracket) {
                eat(Token::DoubleColon);
                return new Ast::QualifiedIdentifier(e, propertyIdentifier());
            }
            return e;
        }

        case Token::Var:
        case Token::Const:
            if (hd2() == Token::LeftBracket)
                return arrayInitializer();
            if (hd2() == Token::LeftBrace)
                return objectInitializer();
            Parse::internalError(this, "Unexpected keyword here: " + lexeme().text);

        case Token::Let:
            return letExpression();

        case Token::LeftBracket:
            return arrayInitializer ();

        case Token::LeftBrace:
            return objectInitializer ();

        case Token::Function:
            return functionExpression (Ast::ordinaryFunction);

        default:
            break;
        }
        return nameExpression();
    }

    function superExpression() {
        eat(Token::Super);
        let expr = null;
        if (hd() == Token::LeftParen)
            expr = parenExpression();
        return new Ast::SuperExpr(expr);
    }

    // FIXME: presumably this belongs somewhere more central, but
    // right now it's only used for slice.

    const intrinsic_ns = new Ast::ForgeableNamespace(Token::intern("http://adobe.com/AS3/2006/builtin"));

    function slice(obj, pos, e1, e2, e3) {
        let args = [];
        if (e1)
            args.push(e1);
        else
            args.push(new Ast::LiteralInt(0));
        if (e2)
            args.push(e2);
        else if (e3)
            args.push(new Ast::LiteralUndefined);
        if (e3)
            args.push(e3);
        return new Ast::CallExpr( new Ast::ObjectRef( obj, 
                                                      new Ast::QualifiedIdentifier(intrinsic_ns, Token::sym_slice),
                                                      pos ),
                                  args,
                                  null,
                                  cx.getStrict() );
    }

    function brackets(obj: Ast::Expr, pos) {
        eat(Token::LeftBracket);

        if (match(Token::DoubleColon)) {
            let expr3 = optionalBracketExpr();
            eat (Token::RightBracket);
            return slice( obj, pos, null, null, expr3 );
        }

        let expr1 = optionalBracketExpr();

        if (match(Token::DoubleColon)) {
            eat(Token::RightBracket);
            return slice( obj, pos, expr1, null, null );
        }

        if (match(Token::Colon)) {
            let expr2 = optionalBracketExpr();
            let expr3 = null;
            if (match(Token::Colon))
                expr3 = optionalBracketExpr();
            eat(Token::RightBracket);
            return slice( obj, pos, expr1, expr2, expr3 );
        }

        if (expr1 == null)
            Parse::syntaxError(this, "Expression required inside the brackets.");

        eat (Token::RightBracket);
        return new Ast::ObjectRef (obj, new Ast::ComputedName(expr1), pos);
    }

    function optionalBracketExpr() {
        if (hd() != Token::Colon && hd() != Token::RightBracket)
            return dynamic override (alpha = noColon) commaExpression ();
        return null;
    }

    function propertyOperator (obj: Ast::Expr) : [Ast::Expr] {
        switch (hd ()) {
        case Token::Dot: {
            eat(Token::Dot);
            let pos = position();
            if (Token::isReserved(hd()))
                return new Ast::ObjectRef (obj, makeIdentifier(propertyIdentifier(), cx.getOpenNamespaces(), pos));
            if (hd() == Token::StringLiteral || hd() == Token::Identifier)
                return new Ast::ObjectRef (obj, nameExpression(), pos);
            Parse::syntaxError(this, "Name expression required here.");
        }
        case Token::LeftBracket: {
            let pos = position();
            return brackets(obj, pos);
        }
        case Token::LeftDotAngle: {
            let ts = typeApplication();
            return new Ast::ApplyTypeExpr(obj, ts);
        }
        default:
            Parse::internalError("propertyOperator: " + hd());
        }
    }

    function typeApplication() {
        eat(Token::LeftDotAngle);
        let ts = typeExpressionList();
        if (hd() == Token::BREAK_RBROCKET)
            brocket();
        eat(Token::GreaterThan);
        return ts;
    }

    function argumentList () {
        let args = [];
        let spread = null;

        eat(Token::LeftParen);
        if (hd() != Token::RightParen) {
            do {
                if (match(Token::TripleDot)) {
                    spread = fullAssignmentExpression();
                    break;
                }
                args.push(fullAssignmentExpression());
            } while (match(Token::Comma));
        }
        eat(Token::RightParen);

        return [args, spread];
    }

    function memberExpression () : Ast::Expr {
        switch (hd ()) {
        case Token::New:
            next();
            let object_expr = memberExpression ();
            let [argument_exprs, spread_expr] = argumentList ();
            return memberExpressionPrime (new Ast::NewExpr (object_expr, argument_exprs, spread_expr));
        case Token::Super:
            let super_expr = superExpression ();
            let property_expr = propertyOperator (super_expr);
            return memberExpressionPrime (property_expr);
        default:
            let expr = primaryExpression ();
            return memberExpressionPrime (expr);
        }
    }

    function memberExpressionPrime (expr: Ast::Expr) : Ast::Expr {
        switch (hd ()) {
        case Token::LeftBracket:
        case Token::Dot:
        case Token::LeftDotAngle:
            return memberExpressionPrime (propertyOperator (expr));
        default:
            return expr;
        }
    }

    function callExpression () : Ast::Expr {
        let pos = position();
        let object_expr = memberExpression ();
        let [argument_exprs,spread_expr] = argumentList ();

        return callExpressionPrime (new Ast::CallExpr (object_expr, argument_exprs, spread_expr, pos, cx.getStrict));
    }

    // shared among many
    function callExpressionPrime (call_expr: Ast::Expr) : Ast::Expr {
        switch (hd ()) {
        case Token::LeftParen:
            let pos = position();
            let [argument_exprs,spread_expr] = argumentList ();
            return callExpressionPrime (new Ast::CallExpr (call_expr, argument_exprs, spread_expr, pos, cx.getStrict()));
        case Token::LeftBracket:
        case Token::Dot:
            return callExpressionPrime (propertyOperator (call_expr));
        default:
            return call_expr;
        }
    }

    function newExpression (new_count=0) : Ast::Expr {
        let call_expression, is_new;

        if (is_new = match(Token::New))
            call_expression = newExpression (new_count+1);
        else
            call_expression = memberExpression();

        if (hd() == Token::LeftParen) {   // No more new exprs so this paren must start a call expr
            let pos = position();
            let [argument_exprs, spread_expr] = argumentList ();
            if (new_count > 0)
                return new Ast::NewExpr (call_expression, argument_exprs, spread_expr);
            return callExpressionPrime (new Ast::CallExpr (call_expression, argument_exprs, spread_expr, pos, cx.getStrict()));
        }

        if (new_count > 0)
            return new Ast::NewExpr (call_expression, [], null);

        if (is_new)
            return memberExpressionPrime (call_expression);
        return call_expression;
    }

    function leftHandSideExpression () : Ast::Expr {
        let operator = (hd() == Token::New) ? newExpression (0) : memberExpression ();
        if (hd () == Token::LeftParen) {
            let pos = position();
            let [args, spread] = argumentList ();
            return callExpressionPrime (new Ast::CallExpr (operator, args, spread, pos, cx.getStrict()));
        }
        return operator;
    }

    function postfixExpression () : Ast::Expr {
        let expr = leftHandSideExpression ();
        if (noNewline()) {
            if (match(Token::PlusPlus))
                return new Ast::UnaryExpr (Ast::postIncrOp, expr);
            if (match(Token::MinusMinus))
                return new Ast::UnaryExpr (Ast::postDecrOp, expr);
        }

        return expr;
    }

    function unaryExpression () : Ast::Expr {
        let t;

        switch (t = hd ()) {
        case Token::Delete:
            next();
            return new Ast::UnaryExpr(Ast::deleteOp | (cx.getStrict() ? Ast::strictFlag : 0), postfixExpression ());

        case Token::PlusPlus:
        case Token::MinusMinus:
            next();
            return new Ast::UnaryExpr(Ast::tokenToOperator[t + 1000], postfixExpression ());

        case Token::Void:
        case Token::TypeOf:
        case Token::Plus:
        case Token::Minus:
        case Token::BitwiseNot:
        case Token::Not:
            next();
            return new Ast::UnaryExpr(Ast::tokenToOperator[t + 1000], unaryExpression ());

        default:
            return postfixExpression();
        }
    }

    function multiplicativeExpression () : Ast::Expr {
        let expr = unaryExpression ();
        let t;

        while (Token::isMultiplicative(t = hd()) || t == Token::BREAK_SLASH) {
            if (t == Token::BREAK_SLASH) {
                div();
                if (!Token::isMultiplicative(t = hd()))
                    break;
            }
            next();
            expr = new Ast::BinaryExpr(Ast::tokenToOperator[t], expr, unaryExpression());
        }

        return expr;
    }

    function additiveExpression () : Ast::Expr {
        let expr = multiplicativeExpression ();
        let t;

        while (Token::isAdditive(t = hd())) {
            next();
            expr = new Ast::BinaryExpr(Ast::tokenToOperator[t], expr, multiplicativeExpression());
        }
        return expr;
    }

    function shiftExpression () : Ast::Expr {
        let expr = additiveExpression ();
        let t;

        while (Token::isShift(t = hd()) || t == Token::BREAK_RBROCKET) {
            if (t == Token::BREAK_RBROCKET) {
                shiftOrRelational();
                if (!Token::isShift(t = hd()))
                    break;
            }
            next();
            expr = new Ast::BinaryExpr(Ast::tokenToOperator[t], expr, additiveExpression());
        }

        return expr;
    }

    // FIXME: Why would we distinguish between BinaryTypeExpr and
    // BinaryExpr here, as long as the operator gives it away
    // anyhow?  (Note we must still dispatch on the subparser).

    function relationalExpression () : Ast::Expr {
        let expr = shiftExpression ();
        let t;

        while (Token::isRelational(t = hd(), beta != noIn) || t == Token::BREAK_RBROCKET) {
            if (t == Token::BREAK_RBROCKET) {
                shiftOrRelational();
                if (!Token::isRelational(t = hd(), beta != noIn))
                    break;
            }
            next();
            if (Token::isRelationalType(t))
                expr = new Ast::BinaryTypeExpr(Ast::tokenToOperator[t], expr, typeExpression());
            else 
                expr = new Ast::BinaryExpr(Ast::tokenToOperator[t], expr, shiftExpression());
        }

        return expr;
    }

    function equalityExpression () : Ast::Expr {
        let expr = relationalExpression ();
        let t;

        while (Token::isEquality(t = hd())) {
            next();
            expr = new Ast::BinaryExpr (Ast::tokenToOperator[t], expr, relationalExpression ());
        }

        return expr;
    }

    function bitwiseAndExpression () : Ast::Expr {
        let expr = equalityExpression ();

        while (match(Token::BitwiseAnd))
            expr = new Ast::BinaryExpr (Ast::bitwiseAndOp, expr, equalityExpression ());

        return expr;
    }

    function bitwiseXorExpression () : Ast::Expr {
        let expr = bitwiseAndExpression ();

        while (match(Token::BitwiseXor))
            expr = new Ast::BinaryExpr (Ast::bitwiseXorOp, expr, bitwiseAndExpression ());

        return expr;
    }

    function bitwiseOrExpression () : Ast::Expr {
        let expr = bitwiseXorExpression ();

        while (match(Token::BitwiseOr)) 
            expr = new Ast::BinaryExpr (Ast::bitwiseOrOp, expr, bitwiseXorExpression ());

        return expr;
    }

    function logicalAndExpression () : Ast::Expr {
        let expr = bitwiseOrExpression ();

        while (match(Token::LogicalAnd)) 
            expr = new Ast::BinaryExpr (Ast::logicalAndOp, expr, bitwiseOrExpression ());

        return expr;
    }

    function logicalOrExpression () : Ast::Expr {
        let expr = logicalAndExpression ();

        while (match(Token::LogicalOr)) 
            expr = new Ast::BinaryExpr (Ast::logicalOrOp, expr, logicalAndExpression ());

        return expr;
    }

    // See also letBlockStmtOrLetExprStmt() 
    function letExpression() {
        eat(Token::Let);
        eat(Token::LeftParen);
        let [fixtures, inits] = letBindingList();
        eat(Token::RightParen);
        let expr = commaExpression();
        return new Ast::LetExpr(new Ast::Head(fixtures, inits), expr);
    }

    function letBindingList() {
        if (hd() == Token::RightParen)
            return [[], []];
        return variableBindingList(Ast::publicNS, Ast::letInit, false);
    }

    function yieldExpression() {
        cx.topFunction().uses_yield = true;
        eat(Token::Yield);
        let expr = null;
        if (noNewline())
            expr = assignmentExpression();
        return new Ast::YieldExpr(expr);
    }

    // Experimental extension: dynamically scoped binding.
    //
    // The values of the variables in the head are read and saved; the
    // expressions are computed left-to-right yielding new values,
    // then assigned left-to-right to the variables; the body
    // expression is evaluated; the saved values are assigned
    // left-to-right to the variables; and the result of the body
    // expression is returned.  If the body expression throws an
    // exception then the original values are restored to the
    // variables but the exception is propagagated out of the
    // expression.
    //
    // If a variable name has computable parts (eg o.x) then the
    // computable parts are computed only once, but each variable is
    // considered independently, that is, "o" is evaluated twice in
    // the list (o.x=1, o.y=2).
    // 
    // If the expressions throw, or the reads or writes throw (because
    // they are getter or setter call that throw, or because the code
    // is strict and a property is read-only or missing, or for any
    // other reason), then the dynamic override expression as a whole
    // throws that exception, and the variables are left in the
    // intermediate state they were in when the exception was thrown.
    // That is, only the body of the form is protected by an exception
    // handler.
    //
    //   "dynamic" "override" "(" NameExpression "=" AssignmentExpr "," ... ")" CommaExpression
    //
    // For example,
    //
    //   dynamic override (ns::x = E1, y = E2) ...
    //
    // (The ESC parser uses this form extensively for the grammar
    // parameters; grep this file.)
    //
    // The syntax "dynamic override" is vile but was chosen because
    //
    //   * "let dynamic" isn't quite right -- all other let forms are
    //     lexical scoping forms and bind only simple names, neither of
    //     which is the case here
    //
    //   * "dynamic" or "override" by itself followed by leftparen is
    //     syntactically ambiguous at the statement level (the leftparen
    //     could introduce a namespace expression)
    //
    // FIXME: general patterns should be allowed on the lhs of the "=".
    //
    // FIXME: there are use cases for a statement form, mirroring
    // let-block statements:
    //
    //        dynamic override (...) {
    //          ...
    //        }

    function dynamicOverrideExpression() {
        if (!ESC::flags.ext_dynamic_override)
            Parse::syntaxError(this, "'dynamic override' not enabled.");
        eat(Token::Dynamic);
        eat(Token::Override);
        eat(Token::LeftParen);
        let names = [];
        let exprs = [];
        if (hd() != Token::RightParen) {
            do {
                names.push(nameExpression());
                eat(Token::Assign);
                exprs.push(fullAssignmentExpression());
            } while (match(Token::Comma));
        }
        eat(Token::RightParen);
        let body = commaExpression();
        return new Ast::DynamicOverrideExpr(names, exprs, body);
    }

    function nonAssignmentExpression () : Ast::Expr {
        switch (hd ()) {
        case Token::Yield:
            return yieldExpression ();

        case Token::Dynamic:
            return dynamicOverrideExpression ();

        case Token::Type:
            next();
            return new Ast::TypeOpExpr(typeExpression ());

        default: {
            let expr = logicalOrExpression ();
            if (match(Token::QuestionMark)) {
                let consequent = dynamic override (alpha=noColon) nonAssignmentExpression ();
                eat (Token::Colon);
                let alternate = nonAssignmentExpression ();
                return new Ast::TernaryExpr (expr, consequent, alternate);
            }
            return expr;
        }
        }
    }

    function conditionalExpression () : Ast::Expr {
        switch (hd ()) {
        case Token::Yield:
            return yieldExpression ();

        case Token::Dynamic:
            return dynamicOverrideExpression ();

        case Token::Type:
            next();
            return new Ast::TypeOpExpr(typeExpression ());

        default: {
            let expr = logicalOrExpression ();
            if (match(Token::QuestionMark)) {
                let consequent = dynamic override (alpha=noColon) assignmentExpression (); 
                eat (Token::Colon);
                let alternate = assignmentExpression ();
                return new Ast::TernaryExpr (expr, consequent, alternate);
            }
            return expr;
        }
        }
    }

    function fullAssignmentExpression(): Ast::Expr
        dynamic override (alpha=allowColon, beta=allowIn) assignmentExpression();

    function assignmentExpression () : Ast::Expr {
        let lhs = conditionalExpression ();
        let t;

    opconsume:
        if ((t = hd()) == Token::Assign || Token::isOpAssign(t) || t == Token::BREAK_RBROCKET) {
            if (t == Token::BREAK_RBROCKET) {
                shiftOrRelational();
                if (!Token::isOpAssign(t = hd()))
                    break opconsume;
            }
            next();
            let pat = patternFromExpr (lhs);
            if (t != Token::Assign && !(pat is SimplePattern))
                Parse::internalError(this, "The left-hand-side of op= must be a name or property reference.");
            let rhs = assignmentExpression ();
            let [_, expr] = desugarAssignmentPattern (pat,Ast::anyType,rhs,Ast::tokenToOperator[t] | (cx.getStrict() ? Ast::strictFlag : 0));
            return expr;
        }
        return lhs;
    }

    // FIXME: This looks partial.  This code does not allow for holes
    // in the array patterns, but they are legal.

    function patternFromExpr (e: Ast::Expr) {
        if (e is Ast::IdentExpr || e is Ast::ObjectRef)
            return new SimplePattern (e);

        if (e is Ast::LiteralArray)
            return new ArrayPattern( Util::map(patternFromExpr, e.exprs), e.spread ? patternFromExpr(e.spread) : null );

        if (e is Ast::LiteralObject)
            return new ObjectPattern( Util::map(function (f) { return new FieldPattern (f.ident, f.expr || f.ident) }, 
                                                e.fields) );

        // The 5 May 2008 grammar says that more expressions are allowed but
        // the WG has decided against that.

        Parse::internalError(this, "patternFromExpr, invalid pattern " + e);
    }

    function fullCommaExpression(): Ast::Expr 
        dynamic override (alpha=allowColon, beta=allowIn) commaExpression();

    function commaExpression(): Ast::Expr {
        let expr = assignmentExpression();
        while (match(Token::Comma))
            expr = new Ast::BinaryExpr(Ast::commaOp, expr, assignmentExpression());
        return expr;
    }

    // PATTERNS

    function pattern () : Pattern {
        switch (hd ()) {
        case Token::LeftBrace:   return objectPattern ();
        case Token::LeftBracket: return arrayPattern ();
        default:                 return simplePattern ();
        }
    }

    function simplePattern () : Pattern {
        switch (gamma) {
        case noExpr:    return new IdentifierPattern (identifier ());
        case allowExpr: return new SimplePattern (leftHandSideExpression ());
        }
    }

    function arrayPattern () : Ast::Expr {
        eat (Token::LeftBracket);
        let [pattern, spread] = elementListPattern ();
        eat (Token::RightBracket);

        return new ArrayPattern (pattern, spread);
    }

    function elementListPattern ()
        dynamic override (beta=allowIn) arrayElements(pattern);

    function objectPattern () : Ast::Expr {
        let fields = [];

        eat (Token::LeftBrace);
        if (hd() != Token::RightBrace) {
            do 
                fields.push (fieldPattern ());
            while (match(Token::Comma));
        }
        eat (Token::RightBrace);
        return new ObjectPattern (fields); 
    }

    function fieldPattern () : FieldPattern {
        let field_name = fieldName ();
        let field_pattern = null;

        if (match(Token::Colon))
            field_pattern = dynamic override (alpha=allowColon, beta=allowIn) pattern ();
        else {
            if (field_name is Ast::Identifier)
                field_pattern = new IdentifierPattern (field_name.ident);
            else {
                // Could be qualified?
                Parse::internalError(this, "unsupported fieldPattern " + field_name);
            }
        }

        return new FieldPattern (field_name, field_pattern);
    }

    function typedPattern () : [Pattern,Ast::TypeExpr] {
        let the_pattern = dynamic override (gamma=noExpr) pattern ();

        if (match(Token::Colon)) 
            return [the_pattern, typeExpression ()];
        return [the_pattern, Ast::anyType];
    }

    function typedPatternOrLike () : [Pattern,Ast::TypeExpr] {
        let the_pattern = dynamic override (gamma=noExpr) pattern ();

        if (match(Token::Colon)) 
            return [the_pattern, typeExpression ()];
        if (match(Token::Like))
            return [the_pattern, new Ast::LikeType(typeExpression())];
        return [the_pattern, Ast::anyType];
    }

    // TYPE EXPRESSIONS

    function typeExpression () : Ast::TypeExpr {
        let hasNullability = false;
        let nullable = false;

        if (match(Token::QuestionMark)) {
            hasNullability = true;
            nullable = true;
        }
        else if (match(Token::Not)) {
            hasNullability = true;
            nullable = false;
        }
        let t = basicTypeExpression ();

        if (hasNullability)
            return new Ast::NullableType (t,nullable);
        return t;
    }

    function typeName(): Ast::TypeExpr {
        let name = new Ast::TypeName(nameExpression());
        if (hd() != Token::LeftDotAngle)
            return cx.resolveTypeNameExpr(name);
        let ts = typeApplication();
        return new Ast::AppType(name, ts);
    }

    function basicTypeExpression () : Ast::TypeExpr {
        switch (hd ()) {
        case Token::Mult:        return next(), Ast::anyType;
        case Token::Null:        return next(), Ast::nullType;
        case Token::Function:    return functionType();
        case Token::LeftParen:   return unionType();
        case Token::LeftBrace:   return objectType();
        case Token::LeftBracket: return arrayType();
        default:                
            if (matchToken(Token::id_undefined))
                return Ast::undefinedType;
            return typeName();
        }
    }

    function functionType() : Ast::TypeExpr {
        match(Token::Function);

        let type_params = typeParameters ();

        eat (Token::LeftParen);
        let this_type = thisType();
        let paramTypes = [];
        let initRequired = false;
        let hasRest = false;

        while (hd() != Token::RightParen) {
            if (match(Token::TripleDot)) {
                hasRest = true;
                if (hd() == Token::Identifier)  // optional name
                    identifier();
                break;
            }
            if (hd() == Token::Identifier && hd2() == Token::Colon) {
                identifier();
                eat(Token::Colon);
            }
            let ty = typeExpression();
            if (initRequired && hd() != Token::Assign)
                Parse::syntaxError(this, "Non-optional argument cannot follow optional argument.");
            initRequired = match(Token::Assign);
            paramTypes.push({ty: ty, optional: initRequired});
            if (!match(Token::Comma))
                break;
        }

        eat(Token::RightParen);
        let res = resultType();
        
        return new Ast::FunctionType(type_params, this_type, paramTypes, hasRest, res);
    }

    function unionType () : Ast::TypeExpr {
        eat (Token::LeftParen);
        let type_exprs = typeExpressionListBar ();
        eat (Token::RightParen);

        return new Ast::UnionType (type_exprs);
    }

    function objectType () : Ast::TypeExpr {
        eat (Token::LeftBrace);
        let field_types = fieldTypeList ();
        eat (Token::RightBrace);

        return new Ast::ObjectType (field_types);
    }

    function fieldTypeList () : [Ast::FieldType] {
        let types = [];
        if (hd () !== Token::RightBrace) {
            do 
                types.push (fieldType ());
            while (match(Token::Comma));
        }
        return types;
    }

    function fieldType () : Ast::FieldType {
        let name = fieldName ();
        eat (Token::Colon);
        let t = typeExpression ();

        return new Ast::FieldType(name, t);
    }

    function arrayType () : Ast::TypeExpr {
        eat (Token::LeftBracket);
        let [elements, spread] = elementTypeList ();
        eat (Token::RightBracket);

        return new Ast::ArrayType (elements, spread);
    }

    function elementTypeList()
        arrayElements(typeExpression);

    function typeExpressionList () : [Ast::TypeExpr]
        listOfTypeExpressions(Token::Comma);

    function typeExpressionListBar () : [Ast::TypeExpr]
        listOfTypeExpressions(Token::BitwiseOr);

    function listOfTypeExpressions(separator) {
        let types = [];
        do 
            types.push (typeExpression ()); 
        while (match(separator));
        return types;
    }

    // STATEMENTS

    function statement () : Ast::Stmt {
        switch (hd()) {
        case Token::SemiColon: return let (stmt = new Ast::EmptyStmt) next(), stmt;
        case Token::LeftBrace: return new Ast::BlockStmt( statementBlock() );
        case Token::Break:     return let (stmt = breakStatement ()) semicolon(omega), stmt;
        case Token::Continue:  return let (stmt = continueStatement ()) semicolon(omega), stmt;
        case Token::Do:        return let (stmt = doStatement()) semicolon(omega), stmt;
        case Token::For:       return forStatement();
        case Token::If:        return ifStatement();
        case Token::Return:    return let (stmt = returnStatement()) semicolon(omega), stmt;
        case Token::Switch:    return (hd2() == Token::Type) ? switchTypeStatement() : switchStatement();
        case Token::Throw:     return let (stmt = throwStatement()) semicolon(omega), stmt;
        case Token::Try:       return tryStatement ();
        case Token::While:     return whileStatement();
        case Token::With:      return withStatement ();
        default:
            if (hd() == Token::Identifier && hd2() == Token::Colon)
                return labeledStatement ();
            if (hd() == Token::Let && hd2() == Token::LeftParen)
                return letBlockStmtOrLetExprStmt();
            return let (stmt = new Ast::ExprStmt(fullCommaExpression())) semicolon(omega), stmt;
        }
    }

    function substatement () : Ast::Stmt {
        if (hd() == Token::Var)
            return dynamic override (tau=statementBlk) variableDefinition();
        return statement ();
    }

    // Yes, takes omega explicitly
    function semicolon (omega: OMEGA) {
        switch (omega) {
        case fullStmt:
            switch (hd ()) {
            case Token::SemiColon:
                next();
                return;
            case Token::EOS:
            case Token::RightBrace:
                // Inserting it
                return;
            default:
                if (!newline ())
                    Parse::syntaxError(this, "Expecting semicolon or newline, found " + lexeme().text);
                // Inserting it
                return;
            }

        case abbrevStmt:  // Abbrev, ShortIf
            switch (hd ()) {
            case Token::SemiColon:
                next();
                return;
            default:
                // Inserting it
                return;
            }

        default:
            Parse::internalError(this, "Unhandled statement mode " + omega);
        }
    }

    function noNewline() {
        switch (hd ()) {
        case Token::EOS:
        case Token::SemiColon:
        case Token::RightBrace:
            return false;
        default:
            if (newline())
                return false;
            return true;
        }
    }

    function statementBlock () : Ast::Block {
        eat (Token::LeftBrace);
        cx.enterBlock ();
        let [directive_list,_] = dynamic override (omega=fullStmt, tau=localBlk) directivesLocal();
        let head = cx.exitBlock ();
        eat (Token::RightBrace);

        return new Ast::Block (head, directive_list);
    }

    function labeledStatement () : Ast::Stmt {
        let label = identifier ();
        eat(Token::Colon);

        let stmt = dynamic override (omega=fullStmt) substatement();

        let s = stmt;
        while (s is Ast::LabeledStmt)
            s = s.stmt;
        if (s is Ast::LabelSet)
            s.labels.push(label);

        return new Ast::LabeledStmt(label, stmt);
    }

    // See also letExpression()

    function letBlockStmtOrLetExprStmt() {
        eat(Token::Let);
        eat(Token::LeftParen);
        let [fixtures, inits] = letBindingList();
        eat(Token::RightParen);
        if (match(Token::LeftBrace)) {
            cx.enterBlock();
            let [directive_list,_] = dynamic override (omega=fullStmt, tau=localBlk) directivesLocal();
            let head = cx.exitBlock();
            match(Token::RightBrace);
            return new Ast::LetBlockStmt(new Ast::Head(fixtures, inits), head, directive_list);
        }

        // Extension: allow let expressions at the top level.  Trigger
        // an error if the extension is not enabled.

        if (!ESC::flags.ext_toplevel_letexpr)
            eat(Token::LeftBrace);

        let expr = fullCommaExpression();
        return new Ast::ExprStmt(new Ast::LetExpr(new Ast::Head(fixtures, inits), expr));
    }

    function returnStatement () : Ast::Stmt {
        eat (Token::Return);
        return new Ast::ReturnStmt (noNewline() ? fullCommaExpression() : null);
    }

    function breakStatement () : Ast::Stmt
        new Ast::BreakStmt( breakOrContinueLabel(Token::Break) );

    function continueStatement () : Ast::Stmt
        new Ast::ContinueStmt( breakOrContinueLabel(Token::Continue) );

    function breakOrContinueLabel(tok) {
        eat(tok);
        return noNewline() ? identifier() : null;
    }

    function ifStatement () : Ast::Stmt {
        eat(Token::If);
        let test = parenExpression();
        let consequent = substatement();
        let alternate = null;
        if (match(Token::Else))
            alternate = substatement();

        return new Ast::IfStmt(test, consequent, alternate);
    }

    function whileStatement () : Ast::Stmt {
        eat(Token::While);
        let test = parenExpression();
        let body = substatement(); 
 
        return new Ast::WhileStmt (test, body);
    }

    function doStatement () : Ast::Stmt {
        eat(Token::Do);
        let body = substatement(); 
        eat(Token::While);
        let test = parenExpression ();
 
        return new Ast::DoWhileStmt (test, body);
    }

    function forStatement () : Ast::Stmt {
        let init=null;
        let bindings=null, it=null, ro=false;

        eat (Token::For);
        let is_each = matchToken(Token::id_each);
        eat (Token::LeftParen);

        switch (hd()) {
        case Token::SemiColon:
            break;
        case Token::Const:
        case Token::Var:
        case Token::Let:
            cx.enterBlock ();
            [it,ro,bindings] = bindingForInitialiser();
            break;
        default:
            init = dynamic override (alpha=allowColon, beta=noIn) commaExpression();
        }

        // Always process bindings first to set up the head
        if (bindings != null)
            init = collapseBindings(it, ro, bindings);

        if (match(Token::In)) {
            let pattern = null;

            if (bindings != null) {
                if (bindings.length != 1)
                    Parse::syntaxError(this, "Only one variable binding allowed in for-in");
                [pattern] = bindings[0];
            }

            let objexpr = fullCommaExpression ();
            eat (Token::RightParen);
            let body = substatement (); 
                
            if (bindings == null)
                pattern = patternFromExpr(init);

            let tmp = new Ast::GetCogenTemp();
            let [_,assignment] = desugarBindingPattern(pattern, Ast::anyType, tmp, cx.getDefaultNamespace(), null, null);

            if (bindings != null)
                return new Ast::ForInBindingStmt(cx.exitBlock(), assignment, tmp, init, objexpr, body, is_each);
            return new Ast::ForInStmt(assignment, tmp, objexpr, body, is_each);
        }
        else {
            if (is_each)
                Parse::syntaxError(this, "'for each' requires 'in' style loop");

            eat (Token::SemiColon);
            let test = hd() == Token::SemiColon ? null : fullCommaExpression();
            eat (Token::SemiColon);
            let update = hd() == Token::RightParen ? null : fullCommaExpression();
            eat (Token::RightParen);
            let body = substatement (); 

            if (bindings != null)
                return new Ast::ForBindingStmt(cx.exitBlock(), init, test, update, body);
            return new Ast::ForStmt(init,test,update,body);
        }
    }

    function bindingForInitialiser() {
        let [it, ro] = dynamic override (tau=localBlk) variableDefinitionKind();
        let bindings = [];

        do {
            let [pattern, annotation] = dynamic override (beta=noIn) typedPattern ();
            let initializer = null;

            if (match(Token::Assign))
                initializer = dynamic override (alpha=allowColon, beta=noIn) assignmentExpression ();

            bindings.push([pattern, annotation, initializer]);
        } while (match(Token::Comma));

        return [it, ro, bindings];
    }

    function collapseBindings(it, ro, bindings) {
        let init = null;
        for ( let i=0, limit=bindings.length ; i < limit ; i++ ) {
            let [pattern, annotation, initializer] = bindings[i];
            let [fixtures, ie] = desugarBindingPattern(pattern, annotation, initializer, cx.getDefaultNamespace(), it, ro);
            cx.addFixtures(it, fixtures);
            init = init ? new Ast::BinaryExpr(Ast::commaOp, init, ie) : ie;
        }
        return init;
    }

    function switchStatement () : Ast::Stmt {
        cx.enterBlock ();
        eat (Token::Switch);
        let expr = parenExpression ();

        eat (Token::LeftBrace);
        let cases = null;
        if (hd () == Token::Case || hd() == Token::Default) 
            cases = caseElements ();
        eat (Token::RightBrace);
        let head = cx.exitBlock ();

        return new Ast::SwitchStmt (head, expr, cases);
    }

    function caseElements () : [Ast::Case] {
        let cases = [];
        let hasDefault = false;
        loop:
        for (;;) {
            switch (hd ()) {
            case Token::RightBrace:
                break loop;
            case Token::Default:
                eat(Token::Default);
                eat(Token::Colon);
                if (hasDefault)
                    Parse::syntaxError(this, "Duplicate 'default' clause in 'switch'");
                hasDefault = true;
                cases.push(new Ast::Case(null, []));
                break;
            case Token::Case: {
                eat(Token::Case);
                let item = fullCommaExpression();
                eat(Token::Colon);
                cases.push (new Ast::Case (item,[]));
                break;
            }
            default: {
                let [items,_] = dynamic override (tau=localBlk) directivesLocal(1);            // 'abbrev' is handled by RightBrace check in head
                Util::pushOnto(cases[cases.length-1].stmts, items);
                break;
            }
            }
        }

        return cases;
    }

    function throwStatement () : Ast::Stmt {
        eat (Token::Throw);
        return new Ast::ThrowStmt( fullCommaExpression () );
    }

    function tryStatement () : Ast::Stmt {
        eat (Token::Try);

        let tryblock = statementBlock ();
        let catchblocks = catches ();
        let finallyblock = null;

        if (match(Token::Finally)) {
            cx.topFunction().uses_finally = true;
            finallyblocks = statementBlock ();
        }

        return new Ast::TryStmt (tryblock,catchblocks,finallyblock);
    }

    function catches () : [Ast::Catch] {
        let catchblocks = [];

        while (match(Token::Catch))
            catchblocks.push (catchClause ());

        cx.topFunction().uses_catch = catchblocks.length > 0;
        return catchblocks;
    }

    // FIXME: Is this right?  The catchvar is in an outer scope and
    // any local bindings might shadow it.  It is /not/ like
    // LetBlockStatement.  But it might be ES3 compatible.

    function catchClause (require_type=false) : Ast::Catch {
        eat (Token::LeftParen);
        let [pattern,ty] = dynamic override (alpha=allowColon, beta=allowIn) typedPattern();
        if (require_type && ty == null)
            Parse::syntaxError(this, "Type annotation required here");
        eat (Token::RightParen);
        let catchblock = statementBlock ();

        let [f,i] = desugarBindingPattern (pattern, ty, new Ast::GetParam (0), Ast::publicNS, Ast::varInit, false);
        let head = new Ast::Head (f,[i]);

        return new Ast::Catch (head,catchblock);
    }

    function withStatement () : Ast::Stmt {
        if (cx.getStrict())
            Parse::syntaxError(this, "The 'with' statement is not allowed in strict mode.");
        eat (Token::With);
        cx.topFunction().uses_with = true;
        let expr = parenExpression ();
        let body = substatement ();
        return new Ast::WithStmt (expr, body);
    }

    function switchTypeStatement () : Ast::Stmt {
        eat (Token::Switch);
        eat (Token::Type);
        let expr = parenExpression ();
        eat (Token::LeftBrace);
        let typecases = typeCases ();
        eat (Token::RightBrace);

        return new Ast::SwitchTypeStmt (expr,typecases);
    }

    function typeCases () : [Ast::Catch] {
        let cases = [];

        // SYNTACTIC CONDITION: All clauses for 'switch type' must
        // have type annotations.

        while (match(Token::Case))
            cases.push(catchClause(true));

        // FIXME.  The Parser Who Knew Too Much.
        //
        // 'switch type' currently compiles to a try/catch, so the
        // function needs to know that a catch is used.  This will go
        // away when 'switch type' is reimplemented (bug 432468).

        cx.topFunction().uses_catch = true;
            
        return cases;
    }

    function superStatement(): Ast::Stmt {
        eat(Token::Super);
        let [args, spread] = argumentList();
        cx.topFunction().uses_super = true;
        return new Ast::SuperStmt(args, spread);
    }

    // DEFINITIONS

    function variableDefinition (attrs) : ? Ast::Expr {
        let [it, ro] = variableDefinitionKind ();
        let [fxtrs, exprs] = variableBindingList (attrs.ns, it, ro);

        cx.addFixtures (it, fxtrs, attrs.static);
        if (tau == classBlk && !attrs.static) {
            cx.addInits(it, exprs);
            return null;
        }
        return exprListToCommaExpr (exprs);
    }

    function variableDefinitionKind () {
        if (tau == statementBlk && (hd() == Token::Const || hd() == Token::Let))
            Parse::syntaxError(this, "'" + lexeme().text + "' not allowed here");

        if (match(Token::Const)) {
            if (match(Token::Function))
                Parse::syntaxError(this, "'const function' not allowed here");
            return [tau == classBlk ? Ast::varInit : Ast::letInit, true];
        }

        if (match(Token::Var))
            return [Ast::varInit, false];

        if (match(Token::Let))
            return [Ast::letInit, false];

        Parse::syntaxError(this, "Illegal binding keyword " + lexeme().text);
    }

    function variableBindingList (ns: Ast::Namespace, it: Ast::INIT_TARGET, ro: boolean ) : [[Ast::Fixture], [Ast::Expr]] {
        let fixtures = [], exprs = [];
        do
            variableBinding(ns, it, ro, fixtures, exprs);
        while (match(Token::Comma));
        return [fixtures, exprs];
    }

    function variableBinding (ns, it, ro, fixtures, exprs) {
        let [pattern, annotation] = typedPattern ();
        let initializer;

        if (match(Token::Assign))
            initializer = dynamic override (alpha=allowColon) assignmentExpression ();
        else if (!(pattern is IdentifierPattern)) 
            Parse::syntaxError(this, "Destructuring pattern without initializer");
        else
            initializer = new Ast::LiteralUndefined();
        let [f,i] = desugarBindingPattern (pattern,annotation,initializer,ns,it,ro);

        Util::pushOnto(fixtures, f);
        exprs.push(i);
    }

    function functionDefinition (attrs: ATTRS) : [Ast::Stmt] {
        let is_const = false;     // FIXME: not currently used

        if (tau != classBlk && tau != interfaceBlk)
            match(Token::Const);
        eat (Token::Function);

        cx.enterFunction(attrs);

        // LANGUAGE BUG: interfaces allow getters and setters, but
        // those are implementation mechanisms.  Better would be to
        // allow a "var" attribute that matches all kinds of
        // properties as well as properties defined by getters and
        // setters.  That would be a (much) better fit with interfaces
        // as essentially type definitions.

        let name = functionName (tau == globalBlk || tau == classBlk || tau == interfaceBlk);
        let signature = functionSignature (name.kind);
        let body, strict;

        if (attrs.native || tau == interfaceBlk) {
            semicolon(fullStmt);
            body = [];
            strict = false;
        }
        else 
            [body, strict] = dynamic override (beta=allowIn) functionBody (true);

        let [vars,attr] = cx.exitFunction();

        let {params, defaults, resultType, thisType, numparams} = signature;
        let func = new Ast::Func (name, body, params, numparams, vars, defaults, resultType, attr, strict);

        if (strict)
            checkStrictFunction(func);

        let name = new Ast::PropName (new Ast::Name(attrs.ns, name.ident));
        let fxtr = new Ast::MethodFixture (func, Ast::anyType, true, attrs.override, attrs.final);
        cx.addFixture(tau == localBlk ? Ast::letInit : Ast::varInit, name, fxtr, attrs.static);
    }

    function functionExpression (kind) : Ast::Expr {
        let name = null;
        let signature;
        let pos = position();
        
        cx.enterFunction(makeAttrs());

        switch (kind) { 
        case Ast::ordinaryFunction:
            eat (Token::Function);
            if (hd() != Token::LeftParen)
                name = functionName(false);
            signature = functionSignature(kind);
            break;
        case Ast::staticInitFunction:
            signature = emptySignature();
            break;
        case Ast::getterFunction:
        case Ast::setterFunction:
            signature = functionSignature(kind);
            break;
        }

        let [body, strict] = functionBody (false);
        let [vars,attr] = cx.exitFunction();
        let {params, numparams, defaults, resultType} = signature;
        let func = new Ast::Func(name, 
                                 body, 
                                 params, 
                                 numparams,
                                 vars, 
                                 defaults, 
                                 resultType,
                                 attr,
                                 strict,
                                 pos,
                                 filename);
        if (strict)
            checkStrictFunction(func);

        let fnexpr = new Ast::LiteralFunction(func, pos);

        return fnexpr;
    }

    function constructorDefinition (ns, attrs) : [Ast::Stmt] {
        eat (Token::Function);

        cx.enterFunction(attrs);

        let name = identifier ();
        let signature = constructorSignature ();
        let body;
        let strict;

        if (attrs.native) {
            semicolon(fullStmt);
            body = [];
            strict = false;
        }
        else {
            eat(Token::LeftBrace);
            [body,strict] = dynamic override (omega=fullStmt, tau=constructorBlk) directivesLocal();
            eat (Token::RightBrace);
        }

        let [vars,attr] = cx.exitFunction();
        let {params, defaults, settings, superArgs, numparams, superSpread} = signature;
        let func = new Ast::Func (new Ast::FuncName(Ast::ordinaryFunction, name), 
                                  body, 
                                  params, 
                                  numparams, 
                                  vars, 
                                  defaults, 
                                  Ast::voidType, 
                                  attr,
                                  strict);
        if (strict)
            checkStrictFunction(func);
        return new Ast::Ctor (settings,superArgs,superSpread,func);
    }

    static type CTOR_SIG = 
    { typeParams : [Ast::TypeExpr]
      , params : Ast::Head
      , paramTypes : [Ast::TypeExpr]
      , defaults : [Ast::Expr]
      , settings : [Ast::Expr]
      , superArgs: [Ast::Expr]
      , spread: ? Ast::Expr }

    static type FUNC_SIG = 
    { typeParams : [Ast::TypeExpr]
      , params : Ast::Head
      , paramTypes : [Ast::TypeExpr]
      , defaults : [Ast::Expr]
      , resultType : Ast::TypeExpr
      , thisType : ? Ast::TypeExpr }

    function constructorSignature () : CTOR_SIG {
        eat (Token::LeftParen);
        let ps = parameters ();
        eat (Token::RightParen);
        let [settings,superArgs,superSpread] = constructorInitialiser ();

        let { fixtures, inits, exprs, types, numparams } = ps;

        return { typeParams: []
               , params: new Ast::Head (fixtures,inits)
               , paramTypes: types
               , numparams: numparams
               , defaults: exprs
               , settings: settings
               , superArgs: superArgs
               , superSpread: superSpread };
    }

    /*

    constructor initializers are represented by two lists. the first
    list represents the initializers and will consist of set exprs or
    let exprs (if there are temps for destructuring). the second list
    represents the arguments to the call the the super constructor

    */

    function constructorInitialiser () : [[Ast::Expr], [Ast::Expr], Ast::Expr] {
        let settings=[];
        let superArgs=null;
        let superSpread=null;

    settings_parser:
        if (match(Token::Colon)) {
            while (hd() != Token::Super) {
                settings.push(setting());
                if (!match(Token::Comma))
                    break settings_parser;
            }
            if (match(Token::Super))
                [superArgs, superSpread] = argumentList();
        }

        return [settings, superArgs, superSpread];
    }

    /*

    Setting
    Pattern(noIn, allowExpr)  VariableInitialisation(allowIn)


    function A (a) : [q::x,r::y] = a { }


    let ($t0 = a) q::x = $t0[0], r::y = $t0[1]

    let ($t0 = a)
    init (This,q,[x,$t0[0]),
    init (This,r,[y,$t0[1])


    */

    function setting () : Ast::Expr {
        let lhs = dynamic override (beta=allowIn, gamma=allowExpr) pattern ();
        eat (Token::Assign);
        let rhs = fullAssignmentExpression ();

        let [fxtrs,inits] = desugarBindingPattern (lhs, Ast::anyType, rhs, null, Ast::instanceInit, null);
        // FIXME: assert fxtrs is empty

        return inits;
    }

    function functionName (allow_getter_setter) : Ast::FUNC_NAME {
        let kind = Ast::ordinaryFunction;
        let ident = null;

        if (allow_getter_setter) {
            if (matchToken(Token::id_get)) {
                if (hd() == Token::LeftParen)
                    ident = Token::sym_get;
                else
                    kind = Ast::getterFunction;
            }
            else if (matchToken(Token::id_set)) {
                if (hd() == Token::LeftParen)
                    ident = Token::sym_set;
                else
                    kind = Ast::setterFunction;
            }
        }

        if (ident == null)
            ident = propertyIdentifier();
        return new Ast::FuncName(kind, ident);
    }

    // Also implements getterSignature() and setterSignature()

    function functionSignature(kind) : FUNC_SIG {
        let type_params = null;
        let this_type = null;

        if (kind == Ast::ordinaryFunction)
            type_params = typeParameters ();

        eat (Token::LeftParen);

        if (kind == Ast::ordinaryFunction)
            this_type = thisType();

        let ps = parameters();
        if (kind == Ast::getterFunction && ps.numparams != 0)
            Parse::syntaxError(this, "Zero parameters required for getter function");
        if (kind == Ast::setterFunction && ps.numparams != 1)
            Parse::syntaxError(this, "One parameter required for setter function");
        eat (Token::RightParen);

        let restype = null;
        switch (kind) {
        case Ast::ordinaryFunction:
        case Ast::getterFunction:
            restype = resultTypeOrLike();
            break;
        case Ast::setterFunction:
            restype = resultTypeVoid();
            break;
        default:
            Parse::internalError(this, "Don't know what result type to expect here: " + kind);
        }

        // Translate bindings and init steps into fixtures and inits (Head)
        let { fixtures, inits, exprs, types, numparams } = ps;
        return  { typeParams: type_params
                , params: new Ast::Head (fixtures,inits)
                , paramTypes: types
                , numparams: numparams
                , defaults: exprs
                , ctorInits: null
                , resultType: restype
                , thisType: this_type };
    }

    function thisType() {
        if (match(Token::This)) {
            eat(Token::Colon);
            let this_type = typeName();
            if (match(Token::Comma))
                if (hd() == Token::RightParen)
                    Parse::syntaxError(this, "Expected parameter, found ')'");
            return this_type;
        }
        return null;
    }

    function emptySignature() {
        return { typeParams: []
               , params: new Ast::Head([],[])
               , paramTypes: []
               , numparams: 0
               , defaults: []
               , ctorInits: null
               , resultType: Ast::anyType
               , thisType: null };
    }

    function checkStrictFunction(fn) {
        // FIXME -- implement
        
        // no duplicate parameter names
        // no bound var (including fn) at the body top-level that is also a param name
        // no duplicate var/fn
    }

    function typeParameters () : [Ast::IDENT] {
        let tparams = [];
        if (match(Token::LeftDotAngle)) {
            do 
                tparams.push(identifier());
            while(match(Token::Comma));
            if (hd() == Token::BREAK_RBROCKET)
                brocket();
            eat(Token::GreaterThan);
        }
        return tparams;
    }

    static type PARAMETERS = {
        fixtures: [Ast::Fixture],
        inits: [Ast::Expr],
        exprs: [Ast::Expr],
        types: [Ast::TypeExpr],
        numparams: *
    };

    function parameters () : PARAMETERS {
        let params = { fixtures: [], inits: [], exprs: [], types: [], numparams: 0 };
        let initRequired = false;

        while (hd() != Token::RightParen) {
            if (hd() == Token::TripleDot) {
                cx.topFunction().uses_rest = true;
                restParameter (params);
                break;
            }

            initRequired = parameterInit(params, initRequired);
            params.numparams++;

            if (!match(Token::Comma))
                break;
        }
        return params;
    }

    function restParameter(params) {
        eat(Token::TripleDot);
        if (hd() != Token::RightParen) { // optional name / pattern
            let pat = dynamic override (beta=noIn, gamma=noExpr) simplePattern();
            let [f, i] =
                desugarBindingPattern(pat, 
                                      Ast::anyType, 
                                      new Ast::GetParam(params.numparams), 
                                      Ast::publicNS, 
                                      Ast::letInit, 
                                      false);
            f.push(new Ast::Fixture(new Ast::TempName(params.numparams), 
                                    new Ast::ValFixture(Ast::anyType,false)));
            
            Util::pushOnto(params.fixtures, f);
            params.inits.push(i);
            params.types.push(Ast::anyType);
        }
    }

    function parameterInit (params, initRequired) {
        let param = parameter ();
        let initexpr = null;

        if (match(Token::Assign))
            initexpr = dynamic override (alpha=allowColon, beta=allowIn) nonAssignmentExpression();
        else if (initRequired)
            Parse::syntaxError(this, "Expected mandatory initializer expression, found " + lexeme().text);

        let [f,i,t] = parameterToFixturesAndInitAndType(param, params.numparams);

        Util::pushOnto(params.fixtures, f);
        params.inits.push(i);
        if (initexpr)
            params.exprs.push(initexpr);
        params.types.push(t);

        return initexpr != null;
    }

    function parameter () : [Ast::VAR_DEFN_TAG, [Pattern, Ast::TypeExpr]] {
        let kind = match(Token::Const) ? Ast::constTag : Ast::varTag;
        let pattern = dynamic override (beta=allowIn) typedPatternOrLike ();
        return [kind,pattern];
    }

    function parameterToFixturesAndInitAndType(param, n) : [[Ast::Fixture], Ast::Expr, Ast::TypeExpr] {
        let [k,[p,t]] = param;
        let [f,i] = desugarBindingPattern (p, t, new Ast::GetParam (n), Ast::publicNS, Ast::letInit, false);
        f.push (new Ast::Fixture(new Ast::TempName (n), 
                                 new Ast::ValFixture (t,false))); // temp for desugaring
        return [f,i,t];
    }

    function resultType () : [Ast::TypeExpr] {
        if (match(Token::Colon)) {
            if (match(Token::Void))
                return Ast::voidType;
            return typeExpression ();
        }
        return Ast::anyType;
    }

    function resultTypeVoid () {
        if (match(Token::Colon))
            eat(Token::Void);
    }

    function resultTypeOrLike () : [Ast::TypeExpr] {
        if (match(Token::Colon)) {
            if (match(Token::Void))
                return Ast::voidType;
            return typeExpression ();
        }
        if (match(Token::Like))
            return new Ast::LikeType(typeExpression());
        return Ast::anyType;
    }

    function functionBody (semi) : [[Ast::Stmt], Boolean] {
        if (match(Token::LeftBrace)) {
            let r = dynamic override (omega=fullStmt, tau=localBlk) directivesLocal();
            eat (Token::RightBrace);
            return r;
        }

        let expr = dynamic override (alpha=allowColon) assignmentExpression ();
        if (semi)
            semicolon(omega);
        return [[new Ast::ReturnStmt (expr)], false];
    }

    // Instance variables are initialized by executing initializing
    // expressions in top-to-bottom order, then calling the
    // constructor.  'ihead' may have both fixtures and initializing
    // expressions.
    //
    // Class variables are initialized by executing initializing
    // expressions and blocks in top-to-bottom order.  'chead' may
    // have fixtures, but will not have initializing expressions --
    // those are all in the 'body'.

    function classDefinitionOrDeclaration (attrs): void  {
        eat (Token::Class);

        let classid = identifier ();
        let cname = new Ast::Name(attrs.ns, classid);
        let [typeformals, nonnullable] = typeSignature ();
        let superclass = extendsClause();
        let superinterfaces = interfaceList(Token::id_implements);
        let fx;

        if (superclass != null || superinterfaces.length != 0 || hd() == Token::LeftBrace) {
            let protectedNs = new Ast::ProtectedNamespace (classid);  // FIXME: needs to use attrs.ns too!
            let privateNs = new Ast::PrivateNamespace (classid);      // FIXME: needs to use attrs.ns too!

            currentClassName = classid;                               // FIXME: needs to use attrs.ns too!
            cx.enterClassStatic();
            cx.enterClassInstance();
            cx.pushNamespace(protectedNs);
            cx.pushNamespace(privateNs);
            cx.addFixture(Ast::varInit,
                          new Ast::PropName(new Ast::Name(privateNs, Token::sym_private)),
                          new Ast::NamespaceFixture(privateNs),
                          true);
            cx.addFixture(Ast::varInit,
                          new Ast::PropName(new Ast::Name(privateNs, Token::sym_protected)),
                          new Ast::NamespaceFixture(protectedNs),
                          true);

            eat (Token::LeftBrace);
            let [directive_list, ctor] = directivesClass();
            eat (Token::RightBrace);

            let ihead = cx.exitClassInstance();
            let chead = cx.exitClassStatic();
            currentClassName = Token::sym_EMPTY;

            if (ctor == null)
                ctor = makeDefaultCtor(classid);
            
            if (superclass == null)
                superclass = new Ast::TypeName( new Ast::Identifier(Token::sym_Object, Ast::publicNSSL) );

            let ctype = Ast::anyType;                                 // FIXME: proper class type
            let itype = Ast::anyType;                                 // FIXME: proper instance type
            let cls = new Ast::Cls (cname,
                                    typeformals,
                                    nonnullable,
                                    superclass,
                                    superinterfaces,
                                    protectedNs,
                                    ctor,
                                    chead,
                                    ihead,
                                    ctype,
                                    itype,
                                    directive_list,
                                    attrs.dynamic,
                                    attrs.final);

            fx = new Ast::ClassFixture (cls);
        }
        else {
            semicolon(fullStmt);
            fx = new Ast::ClassFixtureFwd(typeformals, nonnullable);
        }
        cx.addFixture(Ast::varInit, new Ast::PropName(cname), fx);
    }

    function interfaceDefinitionOrDeclaration(attrs): void {
        eat (Token::Interface);
            
        let interfaceid = identifier ();
        let iname = new Ast::Name(attrs.ns, interfaceid);
        let [typeformals, nonnullable] = typeSignature ();
        let superinterfaces = interfaceList(Token::id_extends);
        let fx;

        // LANGUAGE BUG.  It may be useful for interfaces to be
        // declared nonnullable (all implementing classes must be
        // nonnullable) or nullable (all implementing classes must be
        // nullable).

        if (nonnullable)
            Parse::syntaxError(this, "Interfaces may not carry nullability annotations");

        if (superinterfaces.length != 0 || hd() == Token::LeftBrace) {
            currentClassName = Token::sym_EMPTY;

            cx.enterClassInstance();
            eat(Token::LeftBrace);
            directivesInterface();
            eat(Token::RightBrace);
            let ihead = cx.exitClassInstance();

            let iface = new Ast::Interface(iname, typeformals, superinterfaces, ihead);

            fx = new Ast::InterfaceFixture (iface);
        }
        else {
            semicolon(fullStmt);
            fx = new Ast::InterfaceFixtureFwd (typeformals);
        }
        cx.addFixture(Ast::varInit, new Ast::PropName(iname), fx);
    }

    function makeDefaultCtor(classname) {
        cx.enterFunction(makeAttrs());
        let ctorbody = [];
        let params = new Ast::Head([],[]);
        let numparams = 0;
        let vars = new Ast::Head([],[]);
        let defaults = [];
        let ty = Ast::anyType;
        let attr = cx.exitFunction();
        let func = new Ast::Func (new Ast::FuncName(Ast::ordinaryFunction, classname),
                                  ctorbody,
                                  params,
                                  numparams,
                                  vars,
                                  defaults,
                                  ty,
                                  attr,
                                  false);
        return new Ast::Ctor ([],[], null, func);
    }

    // LANGUAGE BUG.  If there is a pragma "use default nullability !"
    // or similar (and there should be one) then it will be useful for
    // a type signature to be able to state "?" explicitly.

    function typeSignature () : [[Ast::IDENT], boolean] {
        let type_params = typeParameters();
        let is_nonnullable = match(Token::Not);

        return [type_params, is_nonnullable];
    }

    function extendsClause() {
        if (matchToken(Token::id_extends))
            return typeName ();
        return null;
    }

    function interfaceList(tok) {  // tok is Token::id_extends or Token::id_implements
        let names = [];
        if (matchToken(tok)) {
            do 
                names.push(typeName());
            while (match(Token::Comma));
        }
        return names;
    }

    function namespaceDefinition (attrs): void {
        eat (Token::Namespace);
        let name = identifier ();
        let initializer = null;

        if (match(Token::Assign)) {
            if (hd() == Token::StringLiteral)
                initializer = let (s = lexeme()) next(), s;
            else
                initializer = cx.resolveNamespaceExpr(nameExpression());
        }

        semicolon (fullStmt);

        let nsVal;
        if (initializer === null) 
            nsVal = new Ast::UnforgeableNamespace (getAnonymousName(name));
        else 
            nsVal = new Ast::ForgeableNamespace (initializer);

        let fxname = new Ast::PropName (new Ast::Name(attrs.ns, name));
        let fxtr = new Ast::NamespaceFixture (nsVal);
        cx.addFixture(Ast::varInit, fxname, fxtr, attrs.static);
    }

    // FIXME: not what you want, and anyhow it looks completely
    // ill-defined since any property name is possible in the 
    // global object.

    function getAnonymousName (seedStr) {
        return seedStr;
    }

    function typeDefinitionOrDeclaration(attrs): void {
        eat (Token::Type);
        let typename = identifier();
        let [formals, nonnullable] = typeSignature();
        let fx;
        if (match(Token::Assign)) {
            let typedef = typeExpression ();
            fx = new Ast::TypeFixture(formals, nonnullable, typedef);
        }
        else
            fx = new Ast::TypeFixtureFwd(formals, nonnullable);
        semicolon (fullStmt);
        cx.addFixture(Ast::varInit, new Ast::PropName (new Ast::Name(attrs.ns, typename)), fx, attrs.static);
    }

    // DIRECTIVES

    static const ATTR_FINAL = 1;
    static const ATTR_DYNAMIC = 2;
    static const ATTR_NATIVE = 4;
    static const ATTR_OVERRIDE = 8;
    static const ATTR_PROTOTYPE = 16;
    static const ATTR_STATIC = 32;

    function checkAttr(bitvector, bit, name) {
        if (bitvector & bit)
            Parse::syntaxError(this, "Duplicate attribute '" + name + "'");
        return bitvector | bit;
    }

    function makeAttrs(ns=null, bits=0) : ATTRS {
        return { ns: ns || cx.getDefaultNamespace()
               , dynamic: (bits & ATTR_DYNAMIC) != 0
               , final: (bits & ATTR_FINAL) != 0
               , native: (bits & ATTR_NATIVE) != 0
               , override: (bits & ATTR_OVERRIDE) != 0
               , prototype: (bits & ATTR_PROTOTYPE) != 0
               , static: (bits & ATTR_STATIC) != 0 }
    }

    // Consume a string of attributes and a namespace or expression.
    //
    // Return [true, ns, attrs] if a valid attribute string terminated
    // by a definition keyword.
    //
    // Return [false, expr, 0] if a valid expr stmt; semicolon or
    // newline not consumed.
    //
    // Return [false, null, 0] if statement-defining keyword, "use",
    // "super", semicolon, or unconsumed expression start encountered.

    static const ATTRIBUTE_KEYWORD = 1;
    static const DEFINING_KEYWORD = 2;
    static const STATEMENT_FIRST = 4;

    static const attrbits = [];
    static {
        attrbits[Token::Dynamic] = ATTRIBUTE_KEYWORD;
        attrbits[Token::Final] = ATTRIBUTE_KEYWORD;
        attrbits[Token::Native] = ATTRIBUTE_KEYWORD;
        attrbits[Token::Override] = ATTRIBUTE_KEYWORD;
        attrbits[Token::__Proto__] = ATTRIBUTE_KEYWORD;
        attrbits[Token::Static] = ATTRIBUTE_KEYWORD;
        attrbits[Token::Var] = DEFINING_KEYWORD;
        attrbits[Token::Const] = DEFINING_KEYWORD;
        attrbits[Token::Let] = DEFINING_KEYWORD;
        attrbits[Token::Function] = DEFINING_KEYWORD;
        attrbits[Token::Class] = DEFINING_KEYWORD;
        attrbits[Token::Interface] = DEFINING_KEYWORD;
        attrbits[Token::Type] = DEFINING_KEYWORD;
        attrbits[Token::Namespace] = DEFINING_KEYWORD;
        attrbits[Token::Use] = DEFINING_KEYWORD;
        attrbits[Token::SemiColon] = STATEMENT_FIRST;
        attrbits[Token::LeftBrace] = STATEMENT_FIRST;
        attrbits[Token::Break] = STATEMENT_FIRST;
        attrbits[Token::Continue] = STATEMENT_FIRST;
        attrbits[Token::Default] = STATEMENT_FIRST;
        attrbits[Token::Do] = STATEMENT_FIRST;
        attrbits[Token::For] = STATEMENT_FIRST;
        attrbits[Token::If] = STATEMENT_FIRST;
        attrbits[Token::Let] = STATEMENT_FIRST;
        attrbits[Token::Return] = STATEMENT_FIRST;
        attrbits[Token::Super] = STATEMENT_FIRST;
        attrbits[Token::Switch] = STATEMENT_FIRST;
        attrbits[Token::Throw] = STATEMENT_FIRST;
        attrbits[Token::Try] = STATEMENT_FIRST;
        attrbits[Token::Use] = STATEMENT_FIRST;
        attrbits[Token::While] = STATEMENT_FIRST;
        attrbits[Token::With] = STATEMENT_FIRST;
    }

    function attributeString() {
        let attrs = 0;
        let ns = null;
        let labels = null;
        let flag = false;

        while (hd() == Token::Identifier && hd2() == Token::Colon) {
            labels = labels || [];
            labels.push(identifier());
            eat(Token::Colon);
        }
        if (labels != null)
            return [false, null, 0, labels];

    loop:
        do {
            let t = hd();
            if (attrbits[t] == STATEMENT_FIRST) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Illegal attribute on statement.");
                return [false, null, 0, null];
            }

            if (attrbits[t] == DEFINING_KEYWORD) {
                flag = true;
                break loop;
            }

            if (attrbits[t] == ATTRIBUTE_KEYWORD) {
                switch (t) {
                case Token::Dynamic:
                    // Language extension, this feels hackish.
                    if (ESC::flags.ext_dynamic_override && ns == null && attrs == 0 && hd2() == Token::Override)
                        return [false, null, 0, null];
                    attrs = checkAttr(attrs, ATTR_DYNAMIC, "dynamic"); 
                    next();
                    break;
                case Token::Final:
                    attrs = checkAttr(attrs, ATTR_FINAL, "final"); 
                    next();
                    break;
                case Token::Native:
                    attrs = checkAttr(attrs, ATTR_NATIVE, "native"); 
                    next();
                    break;
                case Token::Override:
                    attrs = checkAttr(attrs, ATTR_OVERRIDE, "override");
                    next();
                    break;
                case Token::__Proto__:
                    attrs = checkAttr(attrs, ATTR_PROTOTYPE, "__proto__");
                    next();
                    break;
                case Token::Static:
                    // 'static { ... }'
                    if (ns == null && attrs == 0 && hd2() == Token::LeftBrace)
                        return [false, null, 0, null];
                    attrs = checkAttr(attrs, ATTR_STATIC, "static"); 
                    next();
                    break;
                }
                continue;
            }

            if (ns != null) 
                Parse::syntaxError(this, "Expected attribute keyword or definition keyword, found " + lexeme().text);
            let expr = fullCommaExpression(); 
            if (expr is Ast::IdentExpr || expr is Ast::LiteralString) 
                ns = expr;
            else if (attrs != 0)
                Parse::syntaxError(this, "Namespace expression required here.");
            else
                return [false, expr, 0, null];
        } while (noNewline());
        if ((attrs != 0) && !flag)
            Parse::syntaxError(this, "Illegal use of attribute keyword.");
        return [flag, ns, attrs, null];
    }

    // This returns a list of top-level statements in the order
    // they should be evaluated.

    function directivesGlobal(): [Ast::Stmt] {
        // pragma, var, let, const, function, type, namespace, class, interface, statement
        let stmts = [];

        Util::assert(tau == globalBlk);

        while (hd () != Token::RightBrace && hd () != Token::EOS) {
            let [found_attrs, ns, attrs, labels] = attributeString();

            if (!found_attrs && ns != null) {
                semicolon(omega);
                stmts.push(new Ast::ExprStmt(ns));
                continue;
            }

            if (ns != null)
                ns = cx.resolveNamespaceExpr(ns);

            let t = hd();

            if (t == Token::Use) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Pragmas cannot carry attributes");
                if (labels != null)
                    Parse::syntaxError(this, "Pragmas cannot be labeled");
                pragma(true);
                continue;
            }

            if (t == Token::Var ||
                t == Token::Let && hd2() != Token::LeftParen ||
                t == Token::Const && hd2() != Token::Function) {
                if (attrs != 0)
                    Parse::syntaxError(this, "Illegal attribute on variable definition.");
                if (labels != null && t != Token::Var)
                    Parse::syntaxError(this, "Only 'var' definitions can be labelled.");
                let e = dynamic override (beta=allowIn) variableDefinition (makeAttrs(ns, attrs));
                semicolon(fullStmt);
                if (e != null) {
                    // explicitly ignoring the labels here
                    stmts.push(new Ast::ExprStmt(e));
                }
                continue;
            }

            if (t == Token::Function ||
                t == Token::Const && hd2() == Token::Function) {
                if ((attrs & ~ATTR_NATIVE) != 0)
                    Parse::syntaxError(this, "Illegal attribute on function definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled function definition.");
                dynamic override (omega=fullStmt) functionDefinition (makeAttrs(ns, attrs));
                firstdirective = false;
                continue;
            }

            if (t == Token::Type) {
                if (attrs != 0)
                    Parse::syntaxError(this, "Illegal attribute on type definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled type definition.");
                typeDefinitionOrDeclaration (makeAttrs(ns, attrs));
                continue;
            }

            if (t == Token::Namespace) {
                if (attrs != 0)
                    Parse::syntaxError(this, "Illegal attribute on namespace definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled namespace definition.");
                namespaceDefinition (makeAttrs(ns, attrs));
                continue;
            }

            if (t == Token::Class) {
                if ((attrs & ~(ATTR_DYNAMIC | ATTR_FINAL)) != 0)
                    Parse::syntaxError(this, "Illegal attribute on class definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled class definition.");
                dynamic override (tau = classBlk) classDefinitionOrDeclaration(makeAttrs(ns, attrs));
                continue;
            }

            if (t == Token::Interface) {
                if (attrs != 0)
                    Parse::syntaxError(this, "Illegal attribute on interface definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled interface definition.");
                dynamic override (tau = interfaceBlk) interfaceDefinitionOrDeclaration(makeAttrs(ns, attrs));
                continue;
            }

            pushStmt(stmts, labels, statement());
        }

        return stmts;
    }

    // This returns the statements for the instance initializer, and
    // the constructor (or null).  The statements incorporate static
    // intializer blocks.

    function directivesClass(): [Ast::Stmt, Ast::Ctor] {
        // pragma, var, const, function, type, namespace, static-init
        let stmts = [];
        let ctor = null;

        Util::assert( tau == classBlk );

        while (hd () != Token::RightBrace && hd () != Token::EOS) {
            let [found_attrs, ns, attrs, labels] = attributeString();

            if (!found_attrs && ns != null)
                Parse::syntaxError(this, "Statements are not allowed in class blocks.");

            if (ns != null)
                ns = cx.resolveNamespaceExpr(ns);

            let t = hd();

            if (t == Token::Static) {
                eat(Token::Static);
                stmts.push(new Ast::ExprStmt(new Ast::CallExpr(functionExpression(Ast::staticInitFunction), [], null)));
                continue;
            }

            if (t == Token::Use) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Pragmas cannot carry attributes");
                if (labels != null)
                    Parse::syntaxError(this, "Pragmas cannot be labeled");
                pragma(true);
                continue;
            }

            if (t == Token::Var || t == Token::Let || t == Token::Const) {
                if ((attrs & ~(ATTR_PROTOTYPE | ATTR_STATIC)) != 0)
                    Parse::syntaxError(this, "Illegal attribute on variable definition.");
                let e = dynamic override (beta=allowIn) variableDefinition (makeAttrs(ns, attrs));
                semicolon(fullStmt);
                if (e != null)
                    stmts.push(new Ast::ExprStmt(e));
                continue;
            }

            if (t == Token::Function) {
                if (currentClassName == lexeme2()) {
                    if (attrs != 0)
                        Parse::syntaxError(this, "Illegal attribute on constructor function.");
                    if (ctor != null)
                        Parse::syntaxError(this, "A constructor has already been defined for this class.");
                    ctor = dynamic override (omega=fullStmt) constructorDefinition (cx.getDefaultNamespace(), makeAttrs(ns, attrs));
                }
                else
                    dynamic override (omega=fullStmt) functionDefinition (makeAttrs(ns, attrs));
                continue;
            }

            if (t == Token::Type) {
                if ((attrs & ATTR_STATIC) == 0)
                    Parse::syntaxError(this, "'type' definition must be static.");
                if ((attrs & ~ATTR_STATIC) != 0)
                    Parse::syntaxError(this, "Illegal attribute on type definition.");
                typeDefinitionOrDeclaration(makeAttrs(ns, attrs));
                continue;
            }

            if (t == Token::Namespace) {
                if ((attrs & ATTR_STATIC) == 0)
                    Parse::syntaxError(this, "'namespace' definition must be static.");
                if ((attrs & ~ATTR_STATIC) != 0)
                    Parse::syntaxError(this, "Illegal attribute on namespace definition.");
                namespaceDefinition(makeAttrs(ns, attrs));
                continue;
            }

            Parse::syntaxError(this, "Property definition, 'static' block, or pragma is required here.");
        }
        return [stmts, ctor];
    }

    // This operates by side effect on the context: it defines a
    // type that has no code or data.

    function directivesInterface(): void {
        // function
        Util::assert( tau == interfaceBlk );
        while (hd () != Token::RightBrace && hd () != Token::EOS) {
            let [found_attrs, ns, attrs, labels] = attributeString();

            if (!found_attrs && ns != null)
                Parse::syntaxError(this, "Statements are not allowed in interface blocks.");

            if (attrs != 0 || labels != null)
                Parse::syntaxError(this, "Attributes and labels are not allowed on interface methods.");

            if (ns != null)
                ns = cx.resolveNamespaceExpr(ns);

            let t = hd();

            if (t == Token::Function) {
                dynamic override (omega=fullStmt) functionDefinition(makeAttrs(ns, 0));
                continue;
            }

            if (t == Token::Use) {
                if (ns != null) 
                    Parse::syntaxError(this, "Pragmas cannot carry attributes");
                pragma(true);
                continue;
            }

            Parse::syntaxError(this, "'function' is required here.");
        }
    }

    // This returns a list of block-local statements in the order
    // they should be evaluated, and a strict-mode flag.

    function directivesLocal(n=1e500): [Ast::Stmt, Boolean] {
        // pragma, var, let, const, function, statement
        let stmts = [];
        let strict_pragma_ok = true;

        Util::assert( tau == localBlk || tau == constructorBlk );
        while (hd () != Token::RightBrace && hd () != Token::EOS && n-- != 0) {
            let [found_attrs, ns, attrs, labels] = attributeString();

            if (!found_attrs && ns != null) {
                semicolon(omega);
                stmts.push(new Ast::ExprStmt(ns));
                continue;
            }

            if (ns != null)
                ns = cx.resolveNamespaceExpr(ns);

            if (hd() != Token::Use)
                strict_pragma_ok = false;

            let t = hd();

            if (t == Token::Use) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Illegal attribute on pragma.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled pragma.");
                dynamic override (tau=localBlk) pragma(strict_pragma_ok);
                continue;
            }

            if (t == Token::Var || t == Token::Let || (t == Token::Const && hd2() != Token::Function)) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Illegal attribute on variable definition.");
                let e = dynamic override (beta=allowIn, tau=localBlk) variableDefinition (makeAttrs(Ast::publicNS, attrs));
                semicolon(fullStmt);
                if (e != null)
                    stmts.push(new Ast::ExprStmt(e));
                continue;
            }

            if (t == Token::Function || (t == Token::Const && hd2() == Token::Function)) {
                if ((attrs & ~ATTR_NATIVE) != 0 || ns != null)
                    Parse::syntaxError(this, "Illegal attribute on function definition.");
                if (labels != null)
                    Parse::syntaxError(this, "Illegal labeled function definition.");
                dynamic override (omega=fullStmt, tau=localBlk) functionDefinition (makeAttrs(Ast::publicNS, attrs));
                continue;
            }

            if (t == Token::Super && tau == constructorBlk) {
                if (attrs != 0 || ns != null)
                    Parse::syntaxError(this, "Illegal attribute on 'super' statement.");
                stmts.push(dynamic override (tau=localBlk) superStatement());
                semicolon(omega);
                continue;
            }

            pushStmt(stmts, labels, dynamic override (tau=localBlk) statement());
        }

        return [stmts, cx.getStrict()];
    }

    function pushStmt(stmts, labels, s) {
        if (labels != null) {
            if (s is Ast::LabelSet)
                Util::pushOnto(s.labels, labels);
            for ( let i=labels.length-1 ; i >= 0 ; --i )
                s = new Ast::LabeledStmt(labels[i], s);
        }
        stmts.push(s);
    }

    // PRAGMAS

    function pragma (strict_ok_here=false) {
        eat(Token::Use);
        do {
            if (match(Token::Namespace)) 
                cx.openNamespace (namespaceExpression());
            else if (match(Token::Default)) {
                eat(Token::Namespace);
                if (tau == localBlk)
                    Parse::syntaxError(this, "Pragma 'use default namespace' is not allowed in blocks.");
                let name = namespaceExpression();
                cx.setDefaultNamespace(name);
            }
            else if (matchToken(Token::id_strict)) {
                if (tau != globalBlk && !strict_ok_here)
                    Parse::syntaxError(this, "Pragma 'use strict' is not allowed here.");
                cx.setStrict(true);
            }
            else if (matchToken(Token::id_standard)) {
                if (tau != globalBlk)
                    Parse::syntaxError(this, "Pragma 'use standard' is not allowed except at the global level.");
                cx.setStrict(false);
            }
            else
                Parse::syntaxError(this, "Unknown token following 'use': " + lexeme().text);
        } while (match(Token::Comma));
        semicolon (fullStmt);
    }

    // PROGRAMS

    // Synthesize attributes in f.

    function computeAttributes(f: FuncAttr) {
        let reify_activation = false;

        // If there's eval then variable lookup will be by name;
        // new names may be added at run-time.

        reify_activation = reify_activation || f.uses_eval;

        // If there's with then variable lookup will be by name;
        // the set of names in the object is generally unknown.
        // (It's possible to do better for objects of known
        // non-dynamic types, but that's unlikely to be a common
        // case.)

        reify_activation = reify_activation || f.uses_with;

        // If there's a nested function definition or function expression 
        // then it will close over its reified scope.
        //
        // FIXME: If the nested function has no free variables, or its
        // free variables have constant values, then optimizations are
        // possible.

        reify_activation = reify_activation || f.children.length > 0;

        // Catch/finally both imply the use of newcatch/pushscope
        // in a simplistic model.
        //
        // FIXME: In actuality, newcatch/pushscope and lookup by
        // name of the catch var are not necessary unless there is
        // a method in the catch handler that captures the
        // environment, so we can do better -- similar to "let"
        // optimization.

        reify_activation = reify_activation || (f.uses_catch || f.uses_finally); 

        // Even assuming having a nested function does not require
        // reifying the activation, do reify it if the child
        // requires its activation to be reified.

        for ( let i=0, limit=f.children.length ; i < limit ; i++ ) {
            let c = f.children[i];
            computeAttributes(c);
            reify_activation = reify_activation || c.reify_activation;
        }
        f.reify_activation = reify_activation;
    }

    function program (define_namespaces=true) : Ast::Program {
        start();

        // FIXME: The following realization of the internal namespace
        // is not at all safe, but using a random number here makes
        // testing more difficult, and using a hash of the input is
        // slow.  So this will do for the moment.

        // FIXME: Namespaces are not defined for eval, but the
        // following handling of that problem is probably insufficient
        // because references to 'public' and 'internal' in the eval
        // code should reference the ones inherited from the calling
        // context.
        //
        // On the other hand, should global.eval() expose the internal
        // namespace of the caller??

        let internalNamespace = new Ast::InternalNamespace(Token::intern("<#internal " + filename + ">"));

        cx.enterGlobal ();
        if (define_namespaces) {
            cx.addFixture(Ast::varInit, 
                          new Ast::PropName(new Ast::Name(internalNamespace, Token::sym_internal)),
                          new Ast::NamespaceFixture(internalNamespace));
            cx.addFixture(Ast::varInit,
                          new Ast::PropName(new Ast::Name(internalNamespace, Token::sym_public)),
                          new Ast::NamespaceFixture(Ast::publicNS));
        }
        cx.pushNamespace(internalNamespace);
        cx.pushNamespace(Ast::publicNS);

        let global_directives = directivesGlobal ();
        let vhead = cx.exitGlobal ();

        if (hd () != Token::EOS)
            Parse::syntaxError(this, "extra tokens after end of program.");

        computeAttributes(cx.topFunction());

        return new Ast::Program ( global_directives,
                                  vhead,
                                  cx.topFunction(),
                                  filename );
    }
}
