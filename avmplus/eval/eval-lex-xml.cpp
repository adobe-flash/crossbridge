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
        Token Lexer::xmlAtomImpl()
        {
            mark=idx;
            switch (idx[0]) {
                case 0:
                    compiler->syntaxError(lineno, SYNTAXERR_XML_EOI_IN_MARKUP);
                case '<':
                    switch (idx[1]) {
                        case '!':
                            if (idx[2] == '[' &&
                                idx[3] == 'C' &&
                                idx[4] == 'D' &&
                                idx[5] == 'A' &&
                                idx[6] == 'T' &&
                                idx[7] == 'A' &&
                                idx[8] == '[') {
                                idx += 9;
                                return xmlMarkup(T_XmlCDATA);
                            }
                            if (idx[2] == '-' && idx[3] == '-') {
                                idx += 4;
                                return xmlMarkup(T_XmlComment);
                            }
                            compiler->syntaxError(lineno, SYNTAXERR_XML_INVALID_LEFTBANG);
                            
                        case '?':
                            idx += 2;
                            return xmlMarkup(T_XmlProcessingInstruction);
                            
                        case '/':
                            idx += 2;
                            return T_XmlLeftAngleSlash;

                        default:
                            idx += 1;
                            return T_XmlLeftAngle;
                    }
                    
                case '/':
                    if (idx[1] == '>') {
                        idx += 2;
                        return T_XmlSlashRightAngle;
                    }
                    compiler->syntaxError(lineno, SYNTAXERR_XML_INVALID_SLASH);
                    
                case '>':
                    idx += 1;
                    return T_XmlRightAngle;
                    
                case '{':
                    idx += 1;
                    return T_XmlLeftBrace;
                    
                case '}':
                    idx += 1;
                    return T_XmlRightBrace;

                case '=':
                    idx += 1;
                    return T_XmlEquals;

                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    return xmlWhitespace();
                    
                case '"':
                case '\'':
                    return xmlString();
                    
                default:
                    if (isXmlNameStart(idx[0]))
                        return xmlName();
                    else
                        return xmlText();
            }
        }

        // Capture everything from the starting through the ending punctuation.

        Token Lexer::xmlMarkup(Token token)
        {
            uint32_t l = lineno;
            switch (token) {
                case T_XmlComment:
                    mark = idx-4; // "<!--"
                    break;
                case T_XmlCDATA:
                    mark = idx-9; // "<![CDATA["
                    break;
                case T_XmlProcessingInstruction:
                    mark = idx-2; // "<?"
                    break;
                default:
                    AvmAssert(!"Inconsistent internal state");
            }

            while (idx < limit) {
                if (idx[0] == '-' || idx[0] == '?' || idx[0] == ']') {
                    switch (token) {
                        case T_XmlComment:
                            if (idx[0] == '-' && idx[1] == '-') {
                                // Done; we require > to follow but it's not part of the stop condition.
                                if (idx[2] != '>')
                                    compiler->syntaxError(lineno, SYNTAXERR_XML_ILLEGAL_CHARS);
                                idx += 3;
                                goto endloop;
                            }
                            break;
                        case T_XmlCDATA:
                            if (idx[0] == ']' && idx[1] == ']' && idx[2] == '>') {
                                // Done.
                                idx += 3;
                                goto endloop;
                            }
                            break;
                        case T_XmlProcessingInstruction:
                            if (idx[0] == '?' && idx[1] == '>') {
                                // Done.
                                idx += 2;
                                goto endloop;
                            }
                            break;
                    }
                }

                switch (idx[0]) {
                    case '\n':
                        lineno++;
                        break;
                    case '\r':
                        lineno++;
                        if (idx[1] == '\n')
                            idx++;
                        break;
                    default:
                        idx++;
                        break;
                }
            }
            
        endloop:
            if (idx == limit)
                compiler->syntaxError(l, SYNTAXERR_XML_UNTERMINATED);
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return token;
        }

        Token Lexer::xmlWhitespace()
        {
            mark = idx;
            while (idx < limit) {
                switch (*idx) {
                    case ' ':
                    case '\t':
                        break;
                    case '\r':
                        lineno++;
                        if (idx[1] == '\n')
                            idx++;
                        break;
                    case '\n':
                        lineno++;
                        break;
                    default:
                        goto end_loop;
                }
                idx++;
            }
        end_loop:
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlWhitespace;
        }

        Token Lexer::xmlName()
        {
            AvmAssert( isXmlNameStart(*idx) );
            mark = idx;
            while (isXmlNameSubsequent(*idx))
                idx++;
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlName;
        }
        
        // mark has been set at the beginning of the starting punctuation,
        // we wish to capture the ending punctuation as well.
        
        Token Lexer::xmlString()
        {
            wchar terminator = *idx;
            uint32_t l = lineno;

            idx++;
            while (idx < limit && *idx != terminator) {
                if (*idx == '\r') {
                    idx++;
                    if (*idx == '\n')
                        idx++;
                    lineno++;
                }
                else if (*idx == '\n') {
                    idx++;
                    lineno++;
                }
                else
                    idx++;
            }

            if (idx == limit)
                compiler->syntaxError(l, SYNTAXERR_XML_UNTERMINATED);

            idx++;
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlString;
        }

        // FIXME: E4X says to stop only at "{" and "<".

        Token Lexer::xmlText()
        {
            mark = idx;
            while (idx < limit) {
                switch (*idx) {
                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                    case '{':
                    case '}':
                    case '<':
                    case '>':
                    case '/':
                    case '=':
                        goto end_loop;
                    default:
                        if (isXmlNameStart(*idx))
                            goto end_loop;
                }
                idx++;
            }
        end_loop:
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlText;
        }

        bool Lexer::isXmlNameStart(wchar c)
        {
            return isUnicodeLetter(c) || c == ':' || c == '_';
        }
        
        bool Lexer::isXmlNameSubsequent(wchar c)
        {
            return isUnicodeLetter(c) || isUnicodeDigit(c) || c == '_' || c == ':' || c == '.' || c == '-';
        }
        
    }
}

#endif // VMCFG_EVAL
