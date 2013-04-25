/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

inline Token Lexer::lex(uint32_t* linep, TokenValue* valuep)
{
    Token t = lexImpl();
    *linep = lineno;
    *valuep = val;
#ifdef DEBUG
    if (traceflag)
        print(t, *linep, *valuep);
#endif
    return t;
}

inline Token Lexer::regexp(uint32_t* linep, TokenValue* valuep)
{
    Token t = regexpImpl();
    *linep = lineno;
    *valuep = val;
#ifdef DEBUG
    if (traceflag)
        print(t, *linep, *valuep);
#endif
    return t;
}

inline Token Lexer::divideOperator(uint32_t* linep)
{
    Token t = divideOperatorImpl();
    *linep = lineno;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline Token Lexer::xmlAtom(uint32_t* linep, TokenValue* valuep)
{
    Token t = xmlAtomImpl();
    *linep = lineno;
    *valuep = val;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline Token Lexer::rightAngle(uint32_t* linep)
{
    Token t = rightAngleImpl();
    *linep = lineno;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline Token Lexer::leftAngle(uint32_t* linep)
{
    Token t = leftAngleImpl();
    *linep = lineno;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline Token Lexer::leftShiftOrRelationalOperator(uint32_t* linep)
{
    Token t = leftShiftOrRelationalOperatorImpl();
    *linep = lineno;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline Token Lexer::rightShiftOrRelationalOperator(uint32_t* linep)
{
    Token t = rightShiftOrRelationalOperatorImpl();
    *linep = lineno;
#ifdef DEBUG
    if (traceflag) {
        TokenValue garbage;
        garbage.i = 0;
        print(t, *linep, garbage);
    }
#endif
    return t;
}

inline bool Lexer::octalDigits(int k) { return digits(k, CHAR_ATTR_OCTAL); }
inline bool Lexer::decimalDigits(int k) { return digits(k, CHAR_ATTR_DECIMAL); }
inline bool Lexer::hexDigits(int k) { return digits(k, CHAR_ATTR_HEX); }
        
// mis-named, isSubsequent would be better?
inline bool Lexer::notPartOfIdent(int c)
{
    return (c < 128 && (char_attrs[c] & CHAR_ATTR_SUBSEQUENT) == 0) || !isUnicodeIdentifierPart(c);
}

inline void Lexer::xmlPushback(wchar c)
{
    (void)c;
    AvmAssert(idx > src && idx[-1] == c);
    idx--;
}

#ifdef DEBUG

inline void Lexer::trace()
{
    traceflag = true;
}

inline bool Lexer::getTrace() const
{
    return traceflag;
}

#endif
}}
