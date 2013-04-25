/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

inline void FunctionDefn::cogenGuts(Compiler* compiler, Ctx* ctx, ABCMethodInfo** info, ABCMethodBodyInfo** body)
{
    cogenGuts(compiler, ctx, false, info, body);
}

inline bool Parser::newline()
{
    return LP < L0;
}

inline uint32_t Parser::position()
{
    return L0 + line_offset;
}

inline Token Parser::hd()
{
    return T0;
}

inline Str* Parser::identValue()
{
    AvmAssert(T0 == T_Identifier);
    return V0.s;
}

inline Str* Parser::stringValue()
{
    AvmAssert(T0 == T_StringLiteral);
    return V0.s;
}

inline Str* Parser::regexValue()
{
    AvmAssert(T0 == T_RegexpLiteral);
    return V0.s;
}

inline int32_t Parser::intValue()
{
    AvmAssert(T0 == T_IntLiteral);
    return V0.i;
}

inline uint32_t Parser::uintValue()
{
    AvmAssert(T0 == T_UIntLiteral);
    return V0.u;
}

inline double Parser::doubleValue()
{
    AvmAssert(T0 == T_DoubleLiteral);
    return V0.d;
}

inline float Parser::floatValue() 
{
    AvmAssert(T0 == T_FloatLiteral);
    return V0.f;
}

inline bool Parser::isOpAssign(Token t)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isOpAssign;
}

inline bool Parser::isMultiplicative(Token t)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isMultiplicative;
}

inline bool Parser::isAdditive(Token t)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isAdditive;
}

inline bool Parser::isRelational(Token t, bool in_allowed)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isRelational && (in_allowed || t != T_In);
}

inline bool Parser::isEquality(Token t)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isEquality;
}

inline bool Parser::isShift(Token t)
{
    return t < T_OPERATOR_SENTINEL && tokenMapping[t].isShift;
}

inline Unop Parser::tokenToUnaryOperator(Token t)
{
    AvmAssert(t < T_OPERATOR_SENTINEL);
    return (Unop)tokenMapping[t].unaryOp;
}

inline Binop Parser::tokenToBinaryOperator(Token t)
{
    AvmAssert(t < T_OPERATOR_SENTINEL);
    return (Binop)tokenMapping[t].binaryOp;
}

inline void Parser::setUsesArguments()
{
    topRib->body.uses_arguments = true;
}

inline void Parser::setUsesDefaultXmlNamespace()
{
    topRib->body.uses_dxns = true;
}

inline void Parser::xmlAtom()
{
    T0 = lexer->xmlAtom(&L0, &V0);
}

inline void Parser::xmlPushback(wchar c)
{
    lexer->xmlPushback(c);
}
}}
