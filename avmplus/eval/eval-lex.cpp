/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_EVAL

#include "eval.h"

namespace avmplus
{
    namespace RTC
    {
#define OCT CHAR_ATTR_OCTAL | CHAR_ATTR_DECIMAL | CHAR_ATTR_HEX
#define DEC CHAR_ATTR_DECIMAL | CHAR_ATTR_HEX
#define HEX CHAR_ATTR_HEX | CHAR_ATTR_LETTER
#define LTR CHAR_ATTR_LETTER
#define DLR CHAR_ATTR_DOLLAR
#define UBR CHAR_ATTR_UNDERBAR

        const uint8_t Lexer::char_attrs[128] = {
        /*   0 -  15 */   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        /*  16 -  31 */   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        /*  32 -  47 */   0,   0,   0,   0, DLR,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        /*  48 -  63 */ OCT, OCT, OCT, OCT, OCT, OCT, OCT, OCT, DEC, DEC,   0,   0,   0,   0,   0,   0,
        /*  64 -  79 */   0, HEX, HEX, HEX, HEX, HEX, HEX, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR,
        /*  80 -  95 */ LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR,   0,   0,   0,   0, UBR,
        /*  96 - 111 */   0, HEX, HEX, HEX, HEX, HEX, HEX, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR,
        /* 112 - 127 */ LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR, LTR,   0,   0,   0,   0,   0
        };

#undef OCT
#undef DEC
#undef HEX
#undef LTR
#undef DLR
#undef UBR

        // Scanner
        //
        // src must be NUL-terminated!

        Lexer::Lexer(Compiler* compiler, const wchar* src, uint32_t srclen, bool keyword_or_ident)
            : compiler(compiler)
            , src(src)
            , limit(src + srclen - 1)
            , idx(src)
            , mark(NULL)
            , lineno(1)
            , keyword_or_ident(keyword_or_ident)
    #ifdef DEBUG
            , last_token(T_LAST)
            , traceflag(false)
    #endif
        {
            AvmAssert(*limit == 0);
        }

        /* Read slash-delimited string plus following flags; return as one string.
         * The initial '/' has been consumed.
         *
         * IMPLEMENTME: Must strip blanks from /x expressions in non-standard mode,
         * and it probably easiest for it to happen here.
         *
         * The AVM+ regex engine should evolve to handle \<newline> and blank
         * stripping directly, as that provides better debuggability.
         */
        Token Lexer::regexpImpl()
        {
            AvmAssert(last_token == T_BreakSlash);

            StringBuilder s(compiler);
            bool in_charset = false;
            int c;

            s.append('/');
            for (;;) {
                c = *idx++;
                switch (c) {
                    case 0:
                        if (idx == limit)
                            compiler->syntaxError(lineno, SYNTAXERR_EOT_IN_REGEXP);
                        break;
                    case '/':
                        if (!in_charset)
                            goto end_loop;
                        break;
                    case '[':
                        in_charset = true;
                        break;
                    case ']':
                        in_charset = false;
                        break;
                    case '\\':
                        c = *idx++;
                        switch (c) {
                            case 0:
                                if (idx == limit)
                                    compiler->syntaxError(lineno, SYNTAXERR_EOT_IN_REGEXP);
                                break;
                            case '\r':
                                if (*idx == '\n')
                                    idx++;
                                if (compiler->standard_regex)
                                    compiler->syntaxError(lineno, SYNTAXERR_NEWLINE_IN_REGEXP);
                                continue;
                            case '\n':
                            case UNICHAR_LS:
                            case UNICHAR_PS:
                                if (compiler->standard_regex)
                                    compiler->syntaxError(lineno, SYNTAXERR_NEWLINE_IN_REGEXP);
                                continue;
                            case 'u':
                                // PCRE does not understand \u sequences, so expand legal ones here.
                                mark = idx;
                                if (hexDigits(4)) {
                                    s.append((wchar)parseInt(16));
                                    continue;
                                }
                                idx = mark;
                                s.append('u');
                                continue;
                        }
                        s.append('\\');
                        break;
                    case '\n':
                    case '\r':
                    case UNICHAR_LS:
                    case UNICHAR_PS:
                        compiler->syntaxError(lineno, SYNTAXERR_NEWLINE_IN_REGEXP);
                }
                s.append(c);
            }
        end_loop:
            s.append('/');

            // Flags
            while (isUnicodeIdentifierPart(c = *idx)) {
                idx++;
                s.append(c);
            }

            val.s = s.str();
            DEBUG_ONLY(last_token = T_RegexpLiteral);
            return T_RegexpLiteral;
        }

        Token Lexer::divideOperatorImpl()
        {
            AvmAssert(last_token == T_BreakSlash);
            switch (*idx) {
                case '=':
                    idx++;
                    return T_DivideAssign;

                default :
                    return T_Divide;
            }
        }

        Token Lexer::rightAngleImpl()
        {
            AvmAssert(last_token == T_BreakRightAngle);
            return T_GreaterThan;
        }
        
        Token Lexer::leftAngleImpl()
        {
            AvmAssert(last_token == T_BreakLeftAngle);
            return T_LessThan;
        }
        
        Token Lexer::rightShiftOrRelationalOperatorImpl()
        {
            AvmAssert(last_token == T_BreakRightAngle);
            switch (*idx) {
                case '=':
                    idx++;
                    return T_GreaterThanOrEqual;

                case '>':
                    idx++;
                    switch (*idx) {
                        case '=':
                            idx++;
                            return T_RightShiftAssign;

                        case '>':
                            idx++;
                            if (*idx == '=') {
                                idx++;
                                return T_UnsignedRightShiftAssign;
                            }
                            return T_UnsignedRightShift;

                        default:
                            return T_RightShift;
                    }

                default:
                    return T_GreaterThan;
            }
        }

        Token Lexer::leftShiftOrRelationalOperatorImpl()
        {
            AvmAssert(last_token == T_BreakLeftAngle);
            switch (*idx) {
                case '=':
                    idx++;
                    return T_LessThanOrEqual;

                case '<':
                    idx++;
                    if (*idx == '=') {
                        idx++;
                        return T_LeftShiftAssign;
                    }
                    return T_LeftShift;

                default:
                    return T_LessThan;
            }
        }

        /* Get the next token.
         *
         * How end-of-input is detected without checking explicitly: We
         * check for end-of-input every time we see 0, which is allowed in
         * the input (string literals, regexp literals, comments) but
         * tends to be very rare in practice.
         *
         * Note that subscanners that consume input until something
         * happens (like strings and comments and xml) will need to check
         * for 0 as well.
         */
        Token Lexer::lexImpl()
        {
            DEBUG_ONLY(last_token = T_LAST);
            for (;;) {
                switch (*idx++) {
                    case 0:
                        if (idx >= limit) {
                            idx = limit;
                            return compiler->parser.onEOS(&lineno, &val);
                        }
                        compiler->syntaxError(lineno, SYNTAXERR_ILLEGALCHAR_NUL);

                    case '\n' :
                        lineno++;
                        continue;

                    case '\r' :
                        if (*idx == '\n')
                            idx++;
                        lineno++;
                        continue;

                    case ' ' :
                    case '\t' :
                    case '\v' :
                    case '\f' :
                        continue;

                    case '(':
                        return T_LeftParen;

                    case ')':
                        return T_RightParen;

                    case ',':
                        return T_Comma;

                    case ';':
                        return T_Semicolon;

                    case '?':
                        return T_Question;

                    case '[':
                        return T_LeftBracket;

                    case ']':
                        return T_RightBracket;

                    case '{':
                        return T_LeftBrace;

                    case '}':
                        return T_RightBrace;

                    case '~':
                        return T_BitwiseNot;

                    case '/':
                        switch (*idx) {
                            case '/':
                                idx++;
                                lineComment();
                                continue;

                            case '*':
                                idx++;
                                blockComment();
                                continue;

                            default:
                                DEBUG_ONLY(last_token = T_BreakSlash);
                                return T_BreakSlash;
                        }

                    case '\'':
                    case '"':
                        return stringLiteral(idx[-1]);

                    case '.':
                        switch (*idx) {
                            case '.':
                                if (idx[1] == '.') {
                                    idx += 2;
                                    return T_TripleDot;
                                }
                                idx++;
                                return T_DoubleDot;

                            case '<':
                                idx++;
                                return T_LeftDotAngle;

                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                mark = --idx;
                                return numberLiteral();

                            default:
                                return T_Dot;
                        }

                    case '-':
                        switch (*idx) {
                            case '-':
                                idx++;
                                return T_MinusMinus;

                            case '=':
                                idx++;
                                return T_MinusAssign;

                            default:
                                return T_Minus;
                        }

                    case '!':
                        if (*idx == '=') {
                            idx++;
                            if (*idx == '=') {
                                idx++;
                                return T_StrictNotEqual;
                            }
                            return T_NotEqual;
                        }
                        return T_Not;

                    case '%':
                        if (*idx == '=') {
                            idx++;
                            return T_RemainderAssign;
                        }
                        return T_Remainder;

                    case '&':
                        switch (*idx) {
                            case '=':
                                idx++;
                                return T_BitwiseAndAssign;

                            case '&':
                                idx++;
                                if (*idx == '=') {
                                    idx++;
                                    return T_LogicalAndAssign;
                                }
                                return T_LogicalAnd;

                            default:
                                return T_BitwiseAnd;
                        }

                    case '*':
                        if (*idx == '=') {
                            idx++;
                            return T_MultiplyAssign;
                        }
                        return T_Multiply;

                    case ':':
                        if (*idx == ':') {
                            idx++;
                            return T_DoubleColon;
                        }
                        return T_Colon;

                    case '^':
                        if (*idx == '=') {
                            idx++;
                            return T_BitwiseXorAssign;
                        }
                        return T_BitwiseXor;

                    case '|':
                        switch (*idx) {
                            case '=':
                                idx++;
                                return T_BitwiseOrAssign;

                            case '|':
                                idx++;
                                if (*idx == '=') {
                                    idx++;
                                    return T_LogicalOrAssign;
                                }
                                return T_LogicalOr;

                            default:
                                return T_BitwiseOr;
                        }

                    case '+':
                        switch (*idx) {
                            case '+':
                                idx++;
                                return T_PlusPlus;

                            case '=':
                                idx++;
                                return T_PlusAssign;

                            default:
                                return T_Plus;
                        }

                    case '<':
                        DEBUG_ONLY(last_token = T_BreakLeftAngle);
                        return T_BreakLeftAngle;

                    case '=':
                        if (*idx == '=') {
                            idx++;
                            if (*idx == '=') {
                                idx++;
                                return T_StrictEqual;
                            }
                            return T_Equal;
                        }
                        return T_Assign;

                    case '>':
                        DEBUG_ONLY(last_token = T_BreakRightAngle);
                        return T_BreakRightAngle;

                    case '@':
                        return T_AtSign;

                    case '`':
                        return identifier();

                        // Begin generated code
                    case 'a':
                        if (idx[0] == 's' &&
                            !compiler->es3_keywords &&
                            notPartOfIdent(idx[1])) {
                            idx += 1;
                            return T_As;
                        }
                        goto bigswitch_end;
                    case 'b':
                        if (idx[0] == 'r' &&
                            idx[1] == 'e' &&
                            idx[2] == 'a' &&
                            idx[3] == 'k' &&
                            notPartOfIdent(idx[4])) {
                            idx += 4;
                            return T_Break;
                        }
                        goto bigswitch_end;
                    case 'c':
                        switch(idx[0]) {
                            case 'a':
                                switch(idx[1]) {
                                    case 's':
                                        if (idx[2] == 'e' &&
                                            notPartOfIdent(idx[3])) {
                                            idx += 3;
                                            return T_Case;
                                        }
                                        goto bigswitch_end;
                                    case 't':
                                        if (idx[2] == 'c' &&
                                            idx[3] == 'h' &&
                                            notPartOfIdent(idx[4])) {
                                            idx += 4;
                                            return T_Catch;
                                        }
                                        goto bigswitch_end;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'l':
                                if (idx[1] == 'a' &&
                                    idx[2] == 's' &&
                                    idx[3] == 's' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[4])) {
                                    idx += 4;
                                    return T_Class;
                                }
                                goto bigswitch_end;
                            case 'o':
                                switch(idx[1]) {
                                    case 'n':
                                        switch(idx[2]) {
                                            case 's':
                                                if (idx[3] == 't' &&
                                                    !compiler->es3_keywords &&
                                                    notPartOfIdent(idx[4])) {
                                                    idx += 4;
                                                    return T_Const;
                                                }
                                                goto bigswitch_end;
                                            case 't':
                                                if (idx[3] == 'i' &&
                                                    idx[4] == 'n' &&
                                                    idx[5] == 'u' &&
                                                    idx[6] == 'e' &&
                                                    notPartOfIdent(idx[7])) {
                                                    idx += 7;
                                                    return T_Continue;
                                                }
                                                goto bigswitch_end;
                                            default:
                                                goto bigswitch_end;
                                        }
                                    default:
                                        goto bigswitch_end;
                                }
                            default:
                                goto bigswitch_end;
                        }
                    case 'd':
                        switch(idx[0]) {
                            case 'e':
                                switch(idx[1]) {
                                    case 'f':
                                        if (idx[2] == 'a' &&
                                            idx[3] == 'u' &&
                                            idx[4] == 'l' &&
                                            idx[5] == 't' &&
                                            notPartOfIdent(idx[6])) {
                                            idx += 6;
                                            return T_Default;
                                        }
                                        goto bigswitch_end;
                                    case 'l':
                                        if (idx[2] == 'e' &&
                                            idx[3] == 't' &&
                                            idx[4] == 'e' &&
                                            notPartOfIdent(idx[5])) {
                                            idx += 5;
                                            return T_Delete;
                                        }
                                        goto bigswitch_end;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'o':
                                if (!notPartOfIdent(idx[1]))
                                    goto bigswitch_end;
                                idx += 1;
                                return T_Do;
                            case 'y':
                                if (idx[1] == 'n' &&
                                    idx[2] == 'a' &&
                                    idx[3] == 'm' &&
                                    idx[4] == 'i' &&
                                    idx[5] == 'c' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[6])) {
                                    idx += 6;
                                    return T_Dynamic;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'e':
                        if (idx[0] == 'l' &&
                            idx[1] == 's' &&
                            idx[2] == 'e' &&
                            notPartOfIdent(idx[3])) {
                            idx += 3;
                            return T_Else;
                        }
                        goto bigswitch_end;
                    case 'f':
                        switch(idx[0]) {
                            case 'a':
                                if (idx[1] == 'l' &&
                                    idx[2] == 's' &&
                                    idx[3] == 'e' &&
                                    notPartOfIdent(idx[4])) {
                                    idx += 4;
                                    return T_False;
                                }
                                goto bigswitch_end;
                            case 'i':
                                switch(idx[1]) {
                                    case 'n':
                                        switch(idx[2]) {
                                            case 'a':
                                                switch(idx[3]) {
                                                    case 'l':
                                                        if (idx[4] == 'l' &&
                                                            idx[5] == 'y' &&
                                                            !compiler->es3_keywords &&
                                                            notPartOfIdent(idx[6])) {
                                                            idx += 6;
                                                            return T_Finally;
                                                        }
                                                        if (!compiler->es3_keywords && !notPartOfIdent(idx[4]))
                                                            goto bigswitch_end;
                                                        idx += 4;
                                                        return T_Final;
                                                    default:
                                                        goto bigswitch_end;
                                                }
                                            default:
                                                goto bigswitch_end;
                                        }
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'o':
                                if (idx[1] == 'r' &&
                                    notPartOfIdent(idx[2])) {
                                    idx += 2;
                                    return T_For;
                                }
                                goto bigswitch_end;
                            case 'u':
                                if (idx[1] == 'n' &&
                                    idx[2] == 'c' &&
                                    idx[3] == 't' &&
                                    idx[4] == 'i' &&
                                    idx[5] == 'o' &&
                                    idx[6] == 'n' &&
                                    notPartOfIdent(idx[7])) {
                                    idx += 7;
                                    return T_Function;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'g':
                        if (idx[0] == 'o' &&
                            idx[1] == 't' &&
                            idx[2] == 'o' &&
                            !compiler->es3_keywords &&
                            notPartOfIdent(idx[3])) {
                            idx += 3;
                            return T_Goto;
                        }
                        goto bigswitch_end;
                    case 'i':
                        switch(idx[0]) {
                            case 'f':
                                if (!notPartOfIdent(idx[1]))
                                    goto bigswitch_end;
                                idx += 1;
                                return T_If;
                            case 'm':
                                if (idx[1] == 'p' &&
                                    idx[2] == 'o' &&
                                    idx[3] == 'r' &&
                                    idx[4] == 't' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[5])) {
                                    idx += 5;
                                    return T_Import;
                                }
                                goto bigswitch_end;
                            case 'n':
                                switch(idx[1]) {
                                    case 'c':
                                        if (idx[2] == 'l' &&
                                            idx[3] == 'u' &&
                                            idx[4] == 'd' &&
                                            idx[5] == 'e' &&
                                            !compiler->es3_keywords &&
                                            notPartOfIdent(idx[6])) {
                                            idx += 6;
                                            return T_Include;
                                        }
                                        goto bigswitch_end;
                                    case 's':
                                        if (idx[2] == 't' &&
                                            idx[3] == 'a' &&
                                            idx[4] == 'n' &&
                                            idx[5] == 'c' &&
                                            idx[6] == 'e' &&
                                            idx[7] == 'o' &&
                                            idx[8] == 'f' &&
                                            notPartOfIdent(idx[9])) {
                                            idx += 9;
                                            return T_InstanceOf;
                                        }
                                        goto bigswitch_end;
                                    case 't':
                                        switch(idx[2]) {
                                            case 'e':
                                                switch(idx[3]) {
                                                    case 'r':
                                                        switch(idx[4]) {
                                                            case 'f':
                                                                if (idx[5] == 'a' &&
                                                                    idx[6] == 'c' &&
                                                                    idx[7] == 'e' &&
                                                                    !compiler->es3_keywords &&
                                                                    notPartOfIdent(idx[8])) {
                                                                    idx += 8;
                                                                    return T_Interface;
                                                                }
                                                                goto bigswitch_end;
                                                            case 'n':
                                                                if (idx[5] == 'a' &&
                                                                    idx[6] == 'l' &&
                                                                    !compiler->es3_keywords &&
                                                                    notPartOfIdent(idx[7])) {
                                                                    idx += 7;
                                                                    return T_Internal;
                                                                }
                                                                goto bigswitch_end;
                                                            default:
                                                                goto bigswitch_end;
                                                        }
                                                    default:
                                                        goto bigswitch_end;
                                                }
                                            default:
                                                goto bigswitch_end;
                                        }
                                    default:
                                        if (!notPartOfIdent(idx[1]))
                                            goto bigswitch_end;
                                        idx += 1;
                                        return T_In;
                                }
                            case 's':
                                if (!compiler->es3_keywords && !notPartOfIdent(idx[1]))
                                    goto bigswitch_end;
                                idx += 1;
                                return T_Is;
                            default:
                                goto bigswitch_end;
                        }
                    case 'n':
                        switch(idx[0]) {
                            case 'a':
                                switch(idx[1]) {
                                    case 'm':
                                        if (idx[2] == 'e' &&
                                            idx[3] == 's' &&
                                            idx[4] == 'p' &&
                                            idx[5] == 'a' &&
                                            idx[6] == 'c' &&
                                            idx[7] == 'e' &&
                                            !compiler->es3_keywords &&
                                            notPartOfIdent(idx[8])) {
                                            idx += 8;
                                            return T_Namespace;
                                        }
                                        goto bigswitch_end;
                                    case 't':
                                        if (idx[2] == 'i' &&
                                            idx[3] == 'v' &&
                                            idx[4] == 'e' &&
                                            !compiler->es3_keywords &&
                                            notPartOfIdent(idx[5])) {
                                            idx += 5;
                                            return T_Native;
                                        }
                                        goto bigswitch_end;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'e':
                                if (idx[1] == 'w' &&
                                    notPartOfIdent(idx[2])) {
                                    idx += 2;
                                    return T_New;
                                }
                                goto bigswitch_end;
                            case 'u':
                                if (idx[1] == 'l' &&
                                    idx[2] == 'l' &&
                                    notPartOfIdent(idx[3])) {
                                    idx += 3;
                                    return T_Null;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'o':
                        if (idx[0] == 'v' &&
                            idx[1] == 'e' &&
                            idx[2] == 'r' &&
                            idx[3] == 'r' &&
                            idx[4] == 'i' &&
                            idx[5] == 'd' &&
                            idx[6] == 'e' &&
                            !compiler->es3_keywords &&
                            notPartOfIdent(idx[7])) {
                            idx += 7;
                            return T_Override;
                        }
                        goto bigswitch_end;
                    case 'p':
                        switch(idx[0]) {
                            case 'a':
                                if (idx[1] == 'c' &&
                                    idx[2] == 'k' &&
                                    idx[3] == 'a' &&
                                    idx[4] == 'g' &&
                                    idx[5] == 'e' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[6])) {
                                    idx += 6;
                                    return T_Package;
                                }
                                goto bigswitch_end;
                            case 'r':
                                switch(idx[1]) {
                                    case 'i':
                                        if (idx[2] == 'v' &&
                                            idx[3] == 'a' &&
                                            idx[4] == 't' &&
                                            idx[5] == 'e' &&
                                            !compiler->es3_keywords &&
                                            notPartOfIdent(idx[6])) {
                                            idx += 6;
                                            return T_Private;
                                        }
                                        goto bigswitch_end;
                                    case 'o':
                                        if (idx[2] == 't' &&
                                            idx[3] == 'e' &&
                                            idx[4] == 'c' &&
                                            idx[5] == 't' &&
                                            idx[6] == 'e' &&
                                            idx[7] == 'd' &&
                                            !compiler->es3_keywords &&
                                            notPartOfIdent(idx[8])) {
                                            idx += 8;
                                            return T_Protected;
                                        }
                                        goto bigswitch_end;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'u':
                                if (idx[1] == 'b' &&
                                    idx[2] == 'l' &&
                                    idx[3] == 'i' &&
                                    idx[4] == 'c' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[5])) {
                                    idx += 5;
                                    return T_Public;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'r':
                        if (idx[0] == 'e' &&
                            idx[1] == 't' &&
                            idx[2] == 'u' &&
                            idx[3] == 'r' &&
                            idx[4] == 'n' &&
                            notPartOfIdent(idx[5])) {
                            idx += 5;
                            return T_Return;
                        }
                        goto bigswitch_end;
                    case 's':
                        switch(idx[0]) {
                            case 't':
                                if (idx[1] == 'a' &&
                                    idx[2] == 't' &&
                                    idx[3] == 'i' &&
                                    idx[4] == 'c' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[5])) {
                                    idx += 5;
                                    return T_Static;
                                }
                                goto bigswitch_end;
                            case 'u':
                                if (idx[1] == 'p' &&
                                    idx[2] == 'e' &&
                                    idx[3] == 'r' &&
                                    !compiler->es3_keywords &&
                                    notPartOfIdent(idx[4])) {
                                    idx += 4;
                                    return T_Super;
                                }
                                goto bigswitch_end;
                            case 'w':
                                if (idx[1] == 'i' &&
                                    idx[2] == 't' &&
                                    idx[3] == 'c' &&
                                    idx[4] == 'h' &&
                                    notPartOfIdent(idx[5])) {
                                    idx += 5;
                                    return T_Switch;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 't':
                        switch(idx[0]) {
                            case 'h':
                                switch(idx[1]) {
                                    case 'i':
                                        if (idx[2] == 's' &&
                                            notPartOfIdent(idx[3])) {
                                            idx += 3;
                                            return T_This;
                                        }
                                        goto bigswitch_end;
                                    case 'r':
                                        if (idx[2] == 'o' &&
                                            idx[3] == 'w' &&
                                            notPartOfIdent(idx[4])) {
                                            idx += 4;
                                            return T_Throw;
                                        }
                                        goto bigswitch_end;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'r':
                                switch(idx[1]) {
                                    case 'u':
                                        if (idx[2] == 'e' &&
                                            notPartOfIdent(idx[3])) {
                                            idx += 3;
                                            return T_True;
                                        }
                                        goto bigswitch_end;
                                    case 'y':
                                        if (!notPartOfIdent(idx[2]))
                                            goto bigswitch_end;
                                        idx += 2;
                                        return T_Try;
                                    default:
                                        goto bigswitch_end;
                                }
                            case 'y':
                                if (idx[1] == 'p' &&
                                    idx[2] == 'e' &&
                                    idx[3] == 'o' &&
                                    idx[4] == 'f' &&
                                    notPartOfIdent(idx[5])) {
                                    idx += 5;
                                    return T_TypeOf;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'u':
                        if (idx[0] == 's' &&
                            idx[1] == 'e' &&
                            !compiler->es3_keywords &&
                            notPartOfIdent(idx[2])) {
                            idx += 2;
                            return T_Use;
                        }
                        goto bigswitch_end;
                    case 'v':
                        switch(idx[0]) {
                            case 'a':
                                if (idx[1] == 'r' &&
                                    notPartOfIdent(idx[2])) {
                                    idx += 2;
                                    return T_Var;
                                }
                                goto bigswitch_end;
                            case 'o':
                                if (idx[1] == 'i' &&
                                    idx[2] == 'd' &&
                                    notPartOfIdent(idx[3])) {
                                    idx += 3;
                                    return T_Void;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }
                    case 'w':
                        switch(idx[0]) {
                            case 'h':
                                if (idx[1] == 'i' &&
                                    idx[2] == 'l' &&
                                    idx[3] == 'e' &&
                                    notPartOfIdent(idx[4])) {
                                    idx += 4;
                                    return T_While;
                                }
                                goto bigswitch_end;
                            case 'i':
                                if (idx[1] == 't' &&
                                    idx[2] == 'h' &&
                                    notPartOfIdent(idx[3])) {
                                    idx += 3;
                                    return T_With;
                                }
                                goto bigswitch_end;
                            default:
                                goto bigswitch_end;
                        }

                        // End generated code

                    case '\\':
                        // In ES3, the only way backslash can appear in the input in
                        // this position is as the first character of an identifier,
                        // represented as a character escape.  So break out.
                        goto bigswitch_end;

                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        mark = --idx;
                        return numberLiteral();

                    default: {

                        // Check for Unicode space characters and line
                        // terminators here in order to avoid inhibiting
                        // switch optimization on less-capable compilers.

                        int c = idx[-1];

                        // No-break space.

                        if (c == 0x00A0)
                            continue;

                        // Quick check that saves us from the tedious
                        // testing most of the time, also another hack to
                        // get around poor switch code generation.

                        if (c >= UNICHAR_LOWEST_ODDSPACE) {
                            switch (c) {
                                case UNICHAR_Zs1:
                                case UNICHAR_Zs2:
                                case UNICHAR_Zs3:
                                case UNICHAR_Zs4:
                                case UNICHAR_Zs5:
                                case UNICHAR_Zs6:
                                case UNICHAR_Zs7:
                                case UNICHAR_Zs8:
                                case UNICHAR_Zs9:
                                case UNICHAR_Zs10:
                                case UNICHAR_Zs11:
                                case UNICHAR_Zs12:
                                case UNICHAR_Zs13:
                                case UNICHAR_Zs14:
                                case UNICHAR_Zs15:
                                case UNICHAR_Zs16:
                                case UNICHAR_BOM:
                                    continue;
                                case UNICHAR_LS:
                                case UNICHAR_PS:
                                    lineno++;
                                    continue;
                            }
                        }

                        goto bigswitch_end;
                    }
                }

            bigswitch_end:
                // end of bigswitch.

                // Identifiers are handled here (and only here).
                //
                // It is never necessary to check whether an
                // identifier not containing an escape squeuence is a
                // keyword.  In cases where the input is "breakX"
                // where X is a non-identifier character then break
                // will in fact be returned first because of how the
                // notPartOfIdentifier logic works, but since the X is
                // not an identifier constituent there will be an
                // immediate syntax error.  That's good enough for me.

                // If the scanner is used to re-check whether an identifier
                // containing a backslash sequence looks like a keyword then
                // we can stop here.

                if (keyword_or_ident) {
                    DEBUG_ONLY(last_token = T_Identifier);
                    return T_Identifier;
                }

                --idx;
                return identifier();

            }
        }

        // All number literals start here without having consumed any
        // input.

        Token Lexer::numberLiteral()
        {
            switch (*idx) {
                case '0':
                    // Octal / hex / a single 0 / 0.<something> /0e<something>
                    switch (idx[1]) {
                        case 'x':
                        case 'X':
                            idx += 2;
                            mark = idx;
                            if (!hexDigits(-1))
                                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);
                            return integerLiteral(16);

                        case '.':
                            idx += 2;
                            numberFraction(true);
                            return floatingLiteral();

                        case 'E':
                        case 'e':
                            idx += 2;
                            numberExponent();
                            return floatingLiteral();

                        default: {
                            if (!compiler->octal_literals)
                                break;

                            // Octal or single '0'
                            const wchar* startIndex = idx;
                            octalDigits(-1);
                            switch (*idx) {
                                case '8':
                                case '9':
                                    // Ignore the leading 0 and parse it as decimal.
                                    // Firefox extension; clear error in E262-3.
                                    idx = startIndex;
                                    break;
                                default:
                                    return integerLiteral(8);
                            }
                        }
                    }
                    break;

                case '.':
                    idx++;
                    numberFraction(false);
                    return floatingLiteral();
            }

            if (numberLiteralPrime())
                return floatingLiteral();
            else
                return integerLiteral(10);
        }

        Token Lexer::integerLiteral(int base)
        {
            bool isFloat = checkNextCharForNumber(base == 10);
            if (isFloat)
            {
                val.f = parseFloat();
                DEBUG_ONLY(last_token = T_FloatLiteral);
                return T_FloatLiteral;
            }
            else
            {
            double n = parseInt(base);
            if (n >= (-0x7fffffff - 1) && n <= 0x7FFFFFFF) {
                val.i = (int32_t)n;
                DEBUG_ONLY(last_token = T_IntLiteral);
                return T_IntLiteral;
            }
            if (n >= 0x80000000U && n <= 0xFFFFFFFFU) {
                val.u = (uint32_t)n;
                DEBUG_ONLY(last_token = T_UIntLiteral);
                return T_UIntLiteral;
            }
            val.d = n;
            DEBUG_ONLY(last_token = T_DoubleLiteral);
            return T_DoubleLiteral;
        }
        }

        Token Lexer::floatingLiteral()
        {
            bool isFloat = checkNextCharForNumber(true);
            if (isFloat)
            {
                val.f = parseFloat();
                DEBUG_ONLY(last_token = T_FloatLiteral);
                return T_FloatLiteral;
            }
            else
            {
            val.d = parseDouble();
            DEBUG_ONLY(last_token = T_DoubleLiteral);
            return T_DoubleLiteral;
        }
        }

        // Check that the next character allows a number to end at the current
        // location, and error out if not.  Note that the identifierStart condition
        // also handles hexadecimal digits properly.
        //
        // If allowFloat is true, then allow a trailing 'f', and if the trailing 'f'
        // is present then return true.

        bool Lexer::checkNextCharForNumber(bool allowFloat)
        {
            (void)allowFloat;
            int c = *idx;
            bool isFloat = false;
#ifdef VMCFG_FLOAT
            if (allowFloat && (c == 'f' || c == 'F'))
            {
                isFloat = true;
                idx++;
                c = *idx;
            }
#endif
            if ((c >= '0' && c <= '9') || isUnicodeIdentifierStart(c))
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGALCHAR_POSTNUMBER, c);
            return isFloat;
        }

        // Returns true iff the literal contains a decimal point or an
        // exponent marker, otherwise false.

        bool Lexer::numberLiteralPrime()
        {
            if (!decimalDigits(-1))
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);

            switch (*idx) {
                case '.':
                    idx++;
                    numberFraction (true);
                    return true;

                case 'e':
                case 'E':
                    idx++;
                    numberExponent ();
                    return true;

                default:
                    return false;
            }
        }

        // The '.' has been consumed.
        //
        // has_leading_digits should be true if digits have been seen
        // before the '.'.

        void Lexer::numberFraction(bool has_leading_digits)
        {
            if (!decimalDigits (-1) && !has_leading_digits)
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);

            switch (*idx) {
                case 'e':
                case 'E':
                    idx++;
                    numberExponent ();
                    break;
            }
        }

        // The 'e' has been consumed...

        void Lexer::numberExponent()
        {
            switch (*idx) {
                case '+':
                case '-':
                    idx++;
                    break;
            }
            if (!decimalDigits(-1))
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);
        }

        bool Lexer::digits(int k, int attrmask)
        {
            const wchar* startIndex = idx;
            int c;
            while (k != 0 && (c = *idx) < 128 && (char_attrs[c] & attrmask) != 0) {
                k--;
                idx++;
            }
            return idx > startIndex && k <= 0;
        }

        void Lexer::lineComment()
        {
            for (;;) {
                switch (*idx++) {
                    case 0:
                    case '\n':
                    case '\r':
                    case UNICHAR_LS:
                    case UNICHAR_PS:
                        idx--;
                        return;
                }
            }
        }

        void Lexer::blockComment()
        {
            for (;;) {
                int c;
                while ((c = *idx++) != '*' &&
                       c != 0  &&
                       c != '\n' &&
                       c != '\r'  &&
                       c != UNICHAR_LS &&
                       c != UNICHAR_PS)
                    ;
                if (c == '*') {
                    if (*idx == '/') {
                        idx++;
                        return;
                    }
                    continue;
                }
                if (c == 0) {
                    if (idx >= limit) {
                        idx = limit;
                        compiler->syntaxError(lineno, SYNTAXERR_EOI_IN_COMMENT);
                    }
                    continue;
                }
                if (c == '\r') {
                    if (*idx == '\n')
                        idx++;
                    c = '\n';
                }
                AvmAssert(c == '\n' || c == UNICHAR_LS || c == UNICHAR_PS);
                lineno++;
            }
        }

        Token Lexer::identifier()
        {
            // The common case here is that an identifier is a sequence of simple ASCII
            // characters, followed by a non-identifier-constituent ASCII character.  We
            // optimize for this.

            int c;
            const wchar* start = idx;

            if ((c = *idx) < 128 && (char_attrs[c] & CHAR_ATTR_INITIAL) != 0) {
                idx++;
                while ((c = *idx) < 128 && (char_attrs[c] & CHAR_ATTR_SUBSEQUENT) != 0)
                    idx++;
            }

            if (notPartOfIdent(c) && c != '\\') {
                if (idx == start)
                    compiler->syntaxError(lineno, SYNTAXERR_ILLEGALCHAR, *idx);
                val.s = compiler->intern(start, uint32_t(idx-start));
                DEBUG_ONLY(last_token = T_Identifier);
                return T_Identifier;
            }

            // Slow case.
            //
            // If at first we fail then try and try again...

            StringBuilder s(compiler);
            bool has_backslashes = false;
            for (;;) {
                c = *idx;
                if (c != '\\') {
                    if (!(idx == start ? isUnicodeIdentifierStart(c) : isUnicodeIdentifierPart(c)))
                        break;
                    idx++;
                }
                else {
                    has_backslashes = true;
                    idx++;
                    c = *idx;
                    if (c != 'u')
                        compiler->internalError(lineno, "Only unicode escapes allowed here");
                    idx++;
                    c = unicodeEscape();
                    if (!(compiler->liberal_idents || (idx == start ? isUnicodeIdentifierStart(c) : isUnicodeIdentifierPart(c))))
                        compiler->internalError(lineno, "Illegal identifier: unicode character is not allowed in identifier");
                }
                s.append(c);
            }

            if (has_backslashes && !compiler->liberal_idents) {
                // The ES3 spec requires that identifiers constructed
                // with escape sequences must be checked after the
                // fact to see if they are keywords.  Here we create a
                // new lexer to do that.

                StringBuilder s2(compiler);
                s2.append(&s);
                s2.append(0);
                Str* text = s2.str();
                uint32_t textlen = s2.length();
                Lexer subscan(compiler, text->s, textlen, true);
                uint32_t l;
                TokenValue v;
                if (subscan.lex(&l, &v) != T_Identifier)
                    compiler->syntaxError(lineno, SYNTAXERR_IDENT_IS_KWD);
                AvmAssert(subscan.lex(&l, &v) == T_EOS);
            }

            if (s.length() == 0)
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGALCHAR, *idx);

            val.s = s.str();
            DEBUG_ONLY(last_token = T_Identifier);
            return T_Identifier;
        }

        Token Lexer::stringLiteral(int delimiter)
        {
            StringBuilder s(compiler);
            int c;

            // The common case here is that the string contains unproblematic ASCII characters.
            //
            // OPTIMIZEME: for the common case we should not need to accumulate data in a StringBuilder,
            // we should be able to go straight to a Str, as for the identifier case.  (Probably
            // less critical here though.)

            for (;;) {
                const wchar* start = idx;

                // OPTIMIZEME: too many conditions in this test now.  Should bias for ASCII
                // and use table lookup to test for delimiters, NUL, and line endings.

                while ((c = *idx) != delimiter &&
                       c != '\\' &&
                       c != 0 &&
                       c != '\n' &&
                       c != '\r' &&
                       c != UNICHAR_LS &&
                       c != UNICHAR_PS &&
                       c != UNICHAR_BOM)
                    idx++;
                s.append(start, idx);

                switch (*idx) {
                    case '\'':
                    case '"':
                        if (*idx == delimiter) {
                            idx++;
                            val.s = s.str();
                            DEBUG_ONLY(last_token = T_StringLiteral);
                            return T_StringLiteral;
                        }
                        break;  // syntax error

                    case '\\':
                        idx++;

                        switch (*idx) {
                            case '\r':
                                idx++;
                                if (*idx == '\n')
                                    idx++;
                                lineno++;
                                continue;

                            case UNICHAR_LS:
                            case UNICHAR_PS:
                            case '\n':
                                idx++;
                                lineno++;
                                continue;

                            default:
                                s.append(escapeSequence());
                                continue;
                        }

                    case UNICHAR_BOM:
                        s.append(' ');
                        idx++;
                        continue;

                    case 0:
                        if (idx < limit) {
                            s.append(0);
                            idx++;
                            continue;
                        }
                        break;  // syntax error
                }

                compiler->syntaxError(lineno, SYNTAXERR_UNTERMINATED_STRING);
            }
        }

        int Lexer::escapeSequence()
        {
            switch (*idx) {
                case  '0':
                case  '1':
                case  '2':
                case  '3':
                case  '4':
                case  '5':
                case  '6':
                case  '7':
                    return octalOrNulEscape ();

                case 'x':
                    idx++;
                    // Note, handle \x<whatever> as "x" followed by <whatever> when <whatever> is not two hex digits
                    mark = idx;
                    if (hexDigits(2)) {
                        idx = mark;
                        return hexEscape(2);
                    }
                    idx = mark;
                    return 'x';

                case 'u':
                    idx++;
                    // Note, handle \u<whatever> as "u" followed by <whatever> when <whatever> is not two four digits
                    mark = idx;
                    if (hexDigits(4)) {
                        idx = mark;
                        return unicodeEscape ();
                    }
                    idx = mark;
                    return 'u';

                case 'b':
                    idx++;
                    return '\b';

                case 'f':
                    idx++;
                    return '\f';

                case 'n':
                    idx++;
                    return '\n';

                case 'r':
                    idx++;
                    return '\r';

                case 't':
                    idx++;
                    return '\t';

                case 'v':
                    idx++;
                    return '\v';

                case  '\'':
                case  '"':
                case  '\\':
                    return *idx++;

                case 0:
                    if (idx+1 >= limit)
                        compiler->syntaxError(lineno, SYNTAXERR_EOI_IN_ESC);
                    idx++;
                    return 0;

                case     '\n':
                case     '\r':
                case UNICHAR_LS:
                case UNICHAR_PS:
                    compiler->syntaxError(lineno, SYNTAXERR_EOL_IN_ESC);

                default:
                    return *idx++;
            }
        }

        int Lexer::octalOrNulEscape()
        {
            int c;
            if ((c = *idx) >= 128 || (char_attrs[c] & CHAR_ATTR_OCTAL) == 0)
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);

            if (c == '0') {
                idx++;
                if ((c = *idx) < 128 && (char_attrs[c] & CHAR_ATTR_OCTAL) != 0)
                    return octalEscape(3);
                else
                    return 0;
            }

            if (c <= '3')
                return octalEscape(3);

            return octalEscape(2);
        }

        int Lexer::octalEscape(int n)
        {
            mark = idx;
            octalDigits(n);                 // Ignore result
            return (int)parseInt(8);
        }

        // Any leading x or u has been consumed.  n is the number of
        // digits to consume and require.

        int Lexer::hexEscape(int n)
        {
            mark = idx;
            if (!hexDigits(n))
                compiler->syntaxError(lineno, SYNTAXERR_ILLEGAL_NUMBER);
            return (int)parseInt(16);
        }

        // Any leading u has been consumed.

        int Lexer::unicodeEscape()
        {
            if (*idx == '{') {
                idx++;
                mark = idx;
                if (!hexDigits(-1) || *idx != '}')
                    compiler->syntaxError(lineno, SYNTAXERR_INVALID_VAR_ESC);
                int n = (int)parseInt(16);
                idx++;
                return n;
            }
            return hexEscape(4);
        }

        // Parses string of digits in the index in the range [mark,idx) in the given base.
        // Reliably returns Infinity on overflow.

        static uint32_t digitValue(wchar c)
        {
            if (c <= '9')
                return c - '0';
            if (c <= 'F')
                return c - ('A' - 10);
            return c - ('a' - 10);
        }

        // For large values the algorithms for handling hex and octal have better
        // behavior than the straightforward loop that accumulates a result in a double.

        double Lexer::parseInt(int base)
        {
            uint64_t bits = 0;
            uint32_t scale = 0;
            uint32_t k = 0;

            while (mark < idx && *mark == '0')
                mark++;

            if (mark==idx)
                return 0.0;

            switch (base) {
                case 8:
                    for ( const wchar* i=mark ; i < idx ; i++ ) {
                        if (k < 22) {
                            bits = bits << 3 | digitValue(*i);
                            k++;
                        }
                        scale += 3;
                    }
                    goto bits_and_scale;

                case 16:
                    for ( const wchar* i=mark ; i < idx ; i++ ) {
                        if (k < 16) {
                            bits = bits << 4 | digitValue(*i);
                            k++;
                        }
                        scale += 4;
                    }
                    goto bits_and_scale;

                case 10:
                    return parseDouble();

                default:
                    compiler->internalError(lineno, "Unknown base in parseInt");
            }

        bits_and_scale:
            // Left-adjust
            uint32_t n = scale;
            if (n < 33) { bits <<= 32; n += 32; }
            if (n < 49) { bits <<= 16; n += 16; }
            if (n < 57) { bits <<= 8; n += 8; }
            if (n < 61) { bits <<= 4; n += 4; }
            if (n < 63) { bits <<= 2; n += 2; }
            if (n < 64) { bits <<= 1; n += 1; }

            // Normalize
            if ((int64_t)bits > 0) { bits <<= 1; scale--; }
            if ((int64_t)bits > 0) { bits <<= 1; scale--; }
            if ((int64_t)bits > 0) { bits <<= 1; scale--; }

            // Get rid of implicit leading bit, shift into position
            bits <<= 1;
            uint64_t lost = bits & 0xFFF;
            bits >>= 12;
            scale--;

            // Round to even
            // FIXME: it would seem necessary to re-normalize following rounding.
            if (lost > 0x800)
                bits += 1;
            else if (lost == 0x800) {
                if (bits & 1)
                    bits += 1;
            }
            bits &= ~(uint64_t)0 >> 12;

            // Compute and insert exponent
            // FIXME: overflow integer constants properly to Infinity.
            bits |= (uint64_t)(1023 + scale) << 52;
            double_overlay d(bits);
            return d.value;
        }

        float Lexer::parseFloat()
        {
            bool adjust = false;
            if (idx[-1] == 'f')
            {
                adjust = true;
                idx--;
            }
            float f = (float)parseDouble();
            if (adjust)
                idx++;
            return f;
        }

        double Lexer::parseDouble()
        {
            // NOTE: Though convertStringToDouble takes a 'len' parameter, it does *not* check this
            // parameter to determine whether it should stop parsing!  We are saved here only because
            // we've stopped scanning at the point where that function will stop parsing, and because
            // our input is NUL-terminated.
            //
            // NOTE: The 'strict' flag must be false or we'll get an error because the function will
            // be confused by trailing non-NUL characters.
            //
            // NOTE: String API misfeature.  Silly to have to create a new string here.

            double n;
            StringBuilder s(compiler);
            s.append(mark, idx);
            DEBUG_ONLY(bool flag =) compiler->context->stringToDouble(s.chardata(), &n);
            AvmAssert(flag);
            return n;
        }

        bool Lexer::isUnicodeIdentifierStart(int c)
        {
            if (c < 128)
                return (char_attrs[c] & CHAR_ATTR_INITIAL) != 0;
            else
                return isNonASCIIIdentifierStart((wchar)c);
        }

        bool Lexer::isUnicodeIdentifierPart(int c)
        {
            if (c < 128)
                return (char_attrs[c] & CHAR_ATTR_SUBSEQUENT) != 0;
            else
                return isNonASCIIIdentifierSubsequent((wchar)c);
        }

#ifdef DEBUG

        void Lexer::print(Token t, uint32_t/* l*/, TokenValue v)
        {
            char buf[200];
            *buf = 0;
            switch (t) {
                case T_Identifier:
                    VMPI_strcpy(buf, "I ");
                    getn(buf+2, v.s, sizeof(buf)-2);
                    break;
                case T_StringLiteral:
                    VMPI_strcpy(buf, "S ");
                    getn(buf+2, v.s, sizeof(buf)-2);
                    break;
                case T_RegexpLiteral:
                    VMPI_strcpy(buf, "R ");
                    getn(buf+2, v.s, sizeof(buf)-2);
                    break;
                case T_IntLiteral:
                    VMPI_sprintf(buf, "i %d", v.i);
                    break;
                case T_UIntLiteral:
                    VMPI_sprintf(buf, "u %u", v.u);
                    break;
                case T_DoubleLiteral:
                    VMPI_sprintf(buf, "d %g", v.d);
                    break;
                case T_FloatLiteral:
                    VMPI_sprintf(buf, "d %g", (double)v.f);
                    break;
                default:
                    break;
            }
            printf("%d %s\n", (int)t, buf);
        }

#endif // DEBUG

    }
}

#endif // VMCFG_EVAL
