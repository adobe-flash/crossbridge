/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Token,
    namespace Token;

// A token has a "kind" (the token type) and a "text" (the lexeme, a
// string) and a "hash" (an ID for the text).
//
// For predefined tokens the kind is equal to its index in the
// token store.
// 
// NOTE, other parts of the compiler in-line tokenKind, tokenText, and
// tokenHash manually, and know what a Tok looks like.  Don't treat
// Tok as an abstraction!

/*Ast interface Serializable;*/

final class Tok /*implements Serializable*/
{
    const kind: double;
    const hash: double;
    const text: String;

    function Tok(kind,text,hash) : kind=kind, text=text, hash=hash {}

    // A toString call on this is almost certainly an error that we
    // want to catch very quickly; use serialize() instead.  (May lift
    // this restriction when type annotations are properly supported
    // in ESC.)

    function toString() {
        Util::internalError(null, 0, "Cannot convert Tok data to string: " + serialize());
    }

    function serialize()
        "<#token " + kind + " " + text + " " + hash + ">"
}

function tokenKind(tid)
    tokenStore[tid].kind;

function tokenText(tid)
    tokenStore[tid].text;


// Map from (kind, text) to index in tokenStore.  It is used to speed
// up duplicate testing in makeInstance(), below.

const tokenHash = 
    new Util::Hashtable( (function(x) x.hash),
                         (function(a,b) a.text === b.text && a.kind === b.kind),
                         false );
    
// Map from token number to a Tok structure.
//
// Metric: Self-compiling parse.es, about 1000 unique tokens are
// created (beyond the ones that initially populate the token store).

const tokenStore: [...Tok] = [] : [...Tok];  // "new [Tok]" when that works

// Attribute table for tokens.  Used by the parser for token
// classification.  Entries are bit vectors (some tokens have multiple
// attributes).
//
// More attributes could be added here as bit fields, for example
// opcodes for binary and unary operators.

const RESERVED = 2;          // It is a reserved identifier
const LITERAL = 4;           // It is a token that yields a literal
const LEXEME = 8;            // It is a token that has a useful lexeme
const EQUALITY = 16;         // It is ==, !=, ===, !==
const RELATIONAL = 32;       // It is <, <=, >, >=, In, InstanceOf, Is, Cast, Like
const RELTYPE = 64;          // Relational operator with typeExpr rhs
const SHIFTOP = 128;         // It is <<, >>, or >>>
const ADDITIVEOP = 256;      // It is + or -
const MULTIPLICATIVEOP = 512;// It is *, /, or %
const OPASSIGN = 1024;       // += and so on

const tokenAttr: [...double] = [] : [...double];  // "new [double]" when that works

function isReserved(token)
    (tokenAttr[token] & RESERVED) != 0;

function hasLexeme(token)
    (tokenAttr[token] & LEXEME) != 0;

function isLiteral(token)
    (tokenAttr[token] & LITERAL) != 0;

function isEquality(token)
    (tokenAttr[token] & EQUALITY) != 0;

function isRelational(token, allowIn)
    (tokenAttr[token] & RELATIONAL) != 0 && (allowIn || token != In);

function isRelationalType(token)
    (tokenAttr[token] & RELTYPE) != 0;

function isShift(token)
    (tokenAttr[token] & SHIFTOP) != 0;

function isAdditive(token)
    (tokenAttr[token] & ADDITIVEOP) != 0;

function isMultiplicative(token)
    (tokenAttr[token] & MULTIPLICATIVEOP) != 0;

function isOpAssign(token)
    (tokenAttr[token] & OPASSIGN) != 0;

// The variable "hash_helper" is used to avoid allocation on the fast
// path inside makeInstance().  It may not make a huge difference, but
// there you have it.

internal var hash_helper = {kind: 0, text: "", hash: 0};

// Note, trying to use the built-in string hashing algorithm by
// storing a name->ID map in a plain object has about the same
// performance as using Util::hash_string (it may be a hair faster),
// but it uses more memory.
//
// The cost incurred by using real string hashing over a name->ID map
// are the call to Util::hash_string on every call to makeInstance as
// well as a === comparison of strings in the compare function for
// tokenHash.  Thus at some point -- when the compiler is much faster
// -- it may pay to revisit the decision.

function makeInstance(kind, text) {
    hash_helper.kind = kind;
    hash_helper.text = text;
    hash_helper.hash = Util::hash_string(text);
    var tid = tokenHash.read(hash_helper);
    if (tid === false) {
        tid = tokenStore.length;
        tok = new Tok(hash_helper.kind, hash_helper.text, hash_helper.hash);
        tokenStore.push(tok);
        tokenHash.write(tok, tid);
    }
    return tid;
}

function intern(s) {
    return tokenStore[makeInstance(Token::Identifier, String(s))];
}

// punctuation and operators

internal function setup(id, name, attr=0) {
    let tok = new Tok(id, name, Util::hash_string(name));
    tokenStore[id] = tok;
    tokenHash.write(tok, id);
    tokenAttr[id] = attr;
}

const Minus = 0;                                        setup(Token::Minus,"minus", ADDITIVEOP);
const MinusMinus = Token::Minus + 1;                    setup(Token::MinusMinus,"minusminus");
const Not = Token::MinusMinus + 1;                      setup(Token::Not,"not");
const NotEqual = Token::Not + 1;                        setup(Token::NotEqual,"notequals", EQUALITY);
const StrictNotEqual = Token::NotEqual + 1;             setup(Token::StrictNotEqual,"strictnotequals", EQUALITY);
const Remainder = Token::StrictNotEqual + 1;            setup(Token::Remainder,"modulus", MULTIPLICATIVEOP);
const RemainderAssign = Token::Remainder + 1;           setup(Token::RemainderAssign,"modulusassign", OPASSIGN);
const BitwiseAnd = Token::RemainderAssign + 1;          setup(Token::BitwiseAnd,"bitwiseand");
const LogicalAnd = Token::BitwiseAnd + 1;               setup(Token::LogicalAnd,"logicaland");
const LogicalAndAssign = Token::LogicalAnd + 1;         setup(Token::LogicalAndAssign,"logicalandassign", OPASSIGN);
const BitwiseAndAssign = Token::LogicalAndAssign + 1;   setup(Token::BitwiseAndAssign,"bitwiseandassign", OPASSIGN);
const LeftParen = Token::BitwiseAndAssign + 1;          setup(Token::LeftParen,"leftparen");
const RightParen = Token::LeftParen + 1;                setup(Token::RightParen,"rightparen");
const Mult = Token::RightParen + 1;                     setup(Token::Mult,"mult", MULTIPLICATIVEOP);
const MultAssign = Token::Mult + 1;                     setup(Token::MultAssign,"multassign", OPASSIGN);
const Comma = Token::MultAssign  + 1;                   setup(Token::Comma,"comma");
const Dot = Token::Comma + 1;                           setup(Token::Dot,"dot");
const TripleDot = Token::Dot + 1;                       setup(Token::TripleDot,"tripledot");
const LeftDotAngle = Token::TripleDot + 1;              setup(Token::LeftDotAngle,"leftdotangle");
const Div = Token::LeftDotAngle + 1;                    setup(Token::Div,"div", MULTIPLICATIVEOP);
const DivAssign = Token::Div + 1;                       setup(Token::DivAssign,"divassign", OPASSIGN);
const Colon = Token::DivAssign + 1;                     setup(Token::Colon,"colon");
const DoubleColon = Token::Colon + 1;                   setup(Token::DoubleColon,"doublecolon");
const SemiColon = Token::DoubleColon + 1;               setup(Token::SemiColon,"semicolon");
const QuestionMark = Token::SemiColon + 1;              setup(Token::QuestionMark,"questionmark");
const LeftBracket = Token::QuestionMark + 1;            setup(Token::LeftBracket,"leftbracket");
const RightBracket = Token::LeftBracket + 1;            setup(Token::RightBracket,"rightbracket");
const LogicalXorAssign = Token::RightBracket + 1;       setup(Token::LogicalXorAssign,"logicalxorassign", OPASSIGN);
const LeftBrace = Token::LogicalXorAssign + 1;          setup(Token::LeftBrace,"leftbrace");
const LogicalOr = Token::LeftBrace + 1;                 setup(Token::LogicalOr,"logicalor");
const LogicalOrAssign = Token::LogicalOr + 1;           setup(Token::LogicalOrAssign,"logicalorassign", OPASSIGN);
const BitwiseOr = Token::LogicalOrAssign + 1;           setup(Token::BitwiseOr,"bitwiseor");
const BitwiseOrAssign = Token::BitwiseOr + 1;           setup(Token::BitwiseOrAssign,"bitwiseorassign", OPASSIGN);
const BitwiseXor = Token::BitwiseOrAssign + 1;          setup(Token::BitwiseXor,"bitwisexor");
const BitwiseXorAssign = Token::BitwiseXor + 1;         setup(Token::BitwiseXorAssign,"bitwisexorassign", OPASSIGN);
const RightBrace = Token::BitwiseXorAssign + 1;         setup(Token::RightBrace,"rightbrace");
const BitwiseNot = Token::RightBrace + 1;               setup(Token::BitwiseNot,"bitwisenot");
const Plus = Token::BitwiseNot + 1;                     setup(Token::Plus,"plus", ADDITIVEOP);
const PlusPlus = Token::Plus + 1;                       setup(Token::PlusPlus,"plusplus");
const PlusAssign = Token::PlusPlus + 1;                 setup(Token::PlusAssign,"plusassign", OPASSIGN);
const LessThan = Token::PlusAssign + 1;                 setup(Token::LessThan,"lessthan", RELATIONAL);
const LeftShift = Token::LessThan + 1;                  setup(Token::LeftShift,"leftshift", SHIFTOP);
const LeftShiftAssign = Token::LeftShift + 1;           setup(Token::LeftShiftAssign,"leftshiftassign", OPASSIGN);
const LessThanOrEqual = Token::LeftShiftAssign + 1;     setup(Token::LessThanOrEqual,"lessthanorequals", RELATIONAL);
const Assign = Token::LessThanOrEqual + 1;              setup(Token::Assign,"assign");
const MinusAssign = Token::Assign + 1;                  setup(Token::MinusAssign,"minusassign", OPASSIGN);
const Equal = Token::MinusAssign + 1;                   setup(Token::Equal,"equals", EQUALITY);
const StrictEqual = Token::Equal + 1;                   setup(Token::StrictEqual,"strictequals", EQUALITY);
const GreaterThan = Token::StrictEqual + 1;             setup(Token::GreaterThan,"greaterthan", RELATIONAL);
const GreaterThanOrEqual = Token::GreaterThan + 1;      setup(Token::GreaterThanOrEqual,"greaterthanorequals", RELATIONAL);
const RightShift = Token::GreaterThanOrEqual + 1;       setup(Token::RightShift,"rightshift", SHIFTOP);
const RightShiftAssign = Token::RightShift + 1;         setup(Token::RightShiftAssign,"rightshiftassign", OPASSIGN);
const UnsignedRightShift = Token::RightShiftAssign + 1; setup(Token::UnsignedRightShift,"unsignedrightshift", SHIFTOP);
const UnsignedRightShiftAssign = Token::UnsignedRightShift + 1;   setup(Token::UnsignedRightShiftAssign,"unsignedrightshiftassign", OPASSIGN);

// reserved identifiers

const Break = Token::UnsignedRightShiftAssign + 1;      setup(Token::Break,"break", RESERVED);
const Case = Token::Break + 1;                          setup(Token::Case,"case", RESERVED);
const Cast = Token::Case + 1;                           setup(Token::Cast,"cast", RESERVED|RELATIONAL|RELTYPE);
const Catch = Token::Cast + 1;                          setup(Token::Catch,"catch", RESERVED);
const Class = Token::Catch + 1;                         setup(Token::Class,"class", RESERVED);
const Const = Token::Class + 1;                         setup(Token::Const,"const", RESERVED);
const Continue = Token::Const + 1;                      setup(Token::Continue,"continue", RESERVED);
const Default = Token::Continue + 1;                    setup(Token::Default,"default", RESERVED);
const Delete = Token::Default + 1;                      setup(Token::Delete,"delete", RESERVED);
const Do = Token::Delete + 1;                           setup(Token::Do,"do", RESERVED);
const Dynamic = Token::Do + 1;                          setup(Token::Dynamic,"dynamic", RESERVED);
const Else = Token::Dynamic + 1;                        setup(Token::Else,"else", RESERVED);
const False = Token::Else + 1;                          setup(Token::False,"false", RESERVED|LITERAL);
const Final = Token::False + 1;                         setup(Token::Final,"final", RESERVED);
const Finally = Token::Final + 1;                       setup(Token::Finally,"finally", RESERVED);
const For = Token::Finally + 1;                         setup(Token::For,"for", RESERVED);
const Function = Token::For + 1;                        setup(Token::Function,"function", RESERVED);
const If = Token::Function + 1;                         setup(Token::If,"if", RESERVED);
const In = Token::If + 1;                               setup(Token::In,"in", RESERVED|RELATIONAL);
const InstanceOf = Token::In + 1;                       setup(Token::InstanceOf,"instanceof", RESERVED|RELATIONAL);
const Interface = Token::InstanceOf + 1;                setup(Token::Interface,"interface", RESERVED);
const Is = Token::Interface + 1;                        setup(Token::Is,"is", RESERVED|RELATIONAL|RELTYPE);
const Let = Token::Is + 1;                              setup(Token::Let,"let", RESERVED);
const Like = Token::Let + 1;                            setup(Token::Like,"like", RESERVED|RELATIONAL|RELTYPE);
const Namespace = Token::Like + 1;                      setup(Token::Namespace,"namespace", RESERVED);
const Native = Token::Namespace + 1;                    setup(Token::Native,"native", RESERVED);
const New = Token::Native + 1;                          setup(Token::New,"new", RESERVED);
const Null = Token::New + 1;                            setup(Token::Null,"null", RESERVED|LITERAL);
const Override = Token::Null + 1;                       setup(Token::Override,"override", RESERVED);
const Return = Token::Override + 1;                     setup(Token::Return,"return", RESERVED);
const Static = Token::Return + 1;                       setup(Token::Static,"static", RESERVED);
const Super = Token::Static + 1;                        setup(Token::Super,"super", RESERVED);
const Switch = Token::Super + 1;                        setup(Token::Switch,"switch", RESERVED);
const This = Token::Switch + 1;                         setup(Token::This,"this", RESERVED);
const Throw = Token::This + 1;                          setup(Token::Throw,"throw", RESERVED);
const True = Token::Throw + 1;                          setup(Token::True,"true", RESERVED|LITERAL);
const Try = Token::True + 1;                            setup(Token::Try,"try", RESERVED);
const Type = Token::Try + 1;                            setup(Token::Type,"type", RESERVED);
const TypeOf = Token::Type + 1;                         setup(Token::TypeOf,"typeof", RESERVED);
const Use = Token::TypeOf + 1;                          setup(Token::Use,"use", RESERVED);
const Var = Token::Use + 1;                             setup(Token::Var,"var", RESERVED);
const Void = Token::Var + 1;                            setup(Token::Void,"void", RESERVED);
const While = Token::Void + 1;                          setup(Token::While,"while", RESERVED);
const With = Token::While + 1;                          setup(Token::With,"with", RESERVED);
const Yield = Token::With + 1;                          setup(Token::Yield,"yield", RESERVED);
const __Proto__ = Token::Yield + 1;                     setup(Token::__Proto__,"__proto__", RESERVED);

// sundry

const Identifier = Token::__Proto__ + 1;                setup(Token::Identifier,"identifier", LEXEME);
const IntLiteral = Token::Identifier + 1;               setup(Token::IntLiteral,"intliteral", LEXEME|LITERAL);
const UIntLiteral = Token::IntLiteral + 1;              setup(Token::UIntLiteral,"uintliteral", LEXEME|LITERAL);
const DoubleLiteral = Token::UIntLiteral + 1;           setup(Token::DoubleLiteral,"doubleliteral", LEXEME|LITERAL);
const DecimalLiteral = Token::DoubleLiteral + 1;        setup(Token::DecimalLiteral,"decimalliteral", LEXEME|LITERAL);
const RegexpLiteral = Token::DecimalLiteral + 1;        setup(Token::RegexpLiteral,"regexpliteral", LEXEME|LITERAL);
const StringLiteral = Token::RegexpLiteral + 1;         setup(Token::StringLiteral,"stringliteral", LEXEME|LITERAL);

// meta

const EOS = Token::StringLiteral + 1;                   setup(Token::EOS,"EOS");
const BREAK_SLASH = Token::EOS + 1;                     setup(Token::BREAK_SLASH,"BREAK_SLASH");
const BREAK_RBROCKET = Token::BREAK_SLASH + 1;          setup(Token::BREAK_RBROCKET,"BREAK_RBROCKET");
const NONE = Token::BREAK_RBROCKET + 1;                 setup(Token::NONE,"NONE");

// Pre-computed identifier IDs, used by the parser for handling
// contextually reserved words.  (Arguably these belong in the
// parser?)

const id_each = makeInstance(Token::Identifier, "each");
const id_extends = makeInstance(Token::Identifier, "extends");
const id_generator = makeInstance(Token::Identifier, "generator");
const id_get = makeInstance(Token::Identifier, "get");
const id_implements = makeInstance(Token::Identifier, "implements");
const id_set = makeInstance(Token::Identifier, "set");
const id_standard = makeInstance(Token::Identifier, "standard");
const id_strict = makeInstance(Token::Identifier, "strict");
const id_undefined = makeInstance(Token::Identifier, "undefined");

// Pre-computed token values, used by various parts of the system.
// (Saves the time of re-interning these strings whenever they are
// needed.)

const sym_EMPTY = intern("");
const sym_ESC = intern("ESC");
const sym_Array = intern("Array");
const sym_Boolean = intern("Boolean");
const sym_Class = intern("Class");
const sym_Function = intern("Function");
const sym_Namespace = intern("Namespace");
const sym_Number = intern("Number");
const sym_Object = intern("Object");
const sym_RegExp = intern("RegExp");
const sym_String = intern("String");
const sym_TypeError = intern("TypeError");
const sym_arguments = intern("arguments");
const sym_boolean = intern("boolean");
const sym_eval = intern("eval");
const sym_eval_hook = intern("eval_hook");
const sym_evaluateInScopeArray = intern("evaluateInScopeArray");
const sym_get = intern("get");
const sym_int = intern("int");
const sym_internal = intern("internal");
const sym_length = intern("length");
const sym_private = intern("private");
const sym_protected = intern("protected");
const sym_public = intern("public");
const sym_set = intern("set");
const sym_slice = intern("slice");
const sym_string = intern("string");
const sym_uint = intern("uint");
const sym_undefined = intern("undefined");
const sym_x = intern("x");
