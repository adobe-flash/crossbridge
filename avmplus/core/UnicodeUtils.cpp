/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    /**
     *
     *  Table of Unicode characters that can be represented in ECMAScript
     *  ECMA-262 Section 15.1.3
     *
     *  Code Point         Value Representation  1st Octet 2nd Octet 3rd Octet 4th Octet
     *
     *  0x0000 - 0x007F    00000000 0zzzzzzz     0zzzzzzz
     *  0x0080 - 0x07FF    00000yyy yyzzzzzz     110yyyyy  10zzzzzz
     *  0x0800 - 0xD7FF    xxxxyyyy yyzzzzzz     1110xxxx  10yyyyyy  10zzzzzz
     *
     *  0xD800 - 0xDBFF    110110vv vvwwwwxx     11110uuu  10uuwwww  10xxyyyy  10zzzzzz
     *  followed by        followed by
     *  0xDC00 - 0xDFFF    110111yy yyzzzzzz
     *
     *  0xD800 - 0xDBFF
     *  not followed by                          causes URIError
     *  0xDC00 - 0xDFFF
     *
     *  0xDC00 - 0xDFFF                          causes URIError
     *
     *  0xE000 - 0xFFFF    xxxxyyyy yyzzzzzz     1110xxxx 10yyyyyy 10zzzzzz
     *
     */

    int UnicodeUtils::Utf16ToUtf8(const wchar *in,
                                  int inLen,
                                  uint8_t *out,
                                  int outMax)
    {
        int outLen = 0;
        if (out)
        {
            // Output buffer passed in; actually encode data.
            while (inLen > 0)
            {
                wchar ch = *in;
                inLen--;
                if (ch < 0x80) {
                    if (--outMax < 0) {
                        return -1;
                    }
                    *out++ = (uint8_t)ch;
                    outLen++;
                }
                else if (ch < 0x800) {
                    if ((outMax -= 2) < 0) {
                        return -1;
                    }
                    *out++ = (uint8_t)(0xC0 | ((ch>>6)&0x1F));
                    *out++ = (uint8_t)(0x80 | (ch&0x3F));
                    outLen += 2;
                }
                else if (ch >= 0xD800 && ch <= 0xDBFF) {
                    if (--inLen < 0) {
                        return -1;
                    }
                    wchar ch2 = *++in;
                    if (ch2 < 0xDC00 || ch2 > 0xDFFF) {
                        // This is an invalid UTF-16 surrogate pair sequence
                        // Encode the replacement character instead
                        ch = 0xFFFD;
                        goto Encode3;
                    }

                    uint32_t ucs4 = ((ch-0xD800)<<10) + (ch2-0xDC00) + 0x10000;
                    if ((outMax -= 4) < 0) {
                        return -1;
                    }
                    *out++ = (uint8_t)(0xF0 | ((ucs4>>18)&0x07));
                    *out++ = (uint8_t)(0x80 | ((ucs4>>12)&0x3F));
                    *out++ = (uint8_t)(0x80 | ((ucs4>>6)&0x3F));
                    *out++ = (uint8_t)(0x80 | (ucs4&0x3F));
                    outLen += 4;
                }
                else {
                    if (ch >= 0xDC00 && ch <= 0xDFFF) {
                        // This is an invalid UTF-16 surrogate pair sequence
                        // Encode the replacement character instead
                        ch = 0xFFFD;
                    }
                  Encode3:
                    if ((outMax -= 3) < 0) {
                        return -1;
                    }
                    *out++ = (uint8_t)(0xE0 | ((ch>>12)&0x0F));
                    *out++ = (uint8_t)(0x80 | ((ch>>6)&0x3F));
                    *out++ = (uint8_t)(0x80 | (ch&0x3F));
                    outLen += 3;
                }
                in++;
            }
        }
        else
        {
            // Count output characters without actually encoding.
            while (inLen > 0)
            {
                wchar ch = *in;
                inLen--;
                if (ch < 0x80) {
                    outLen++;
                }
                else if (ch < 0x800) {
                    outLen += 2;
                }
                else if (ch >= 0xD800 && ch <= 0xDBFF) {
                    if (--inLen < 0) {
                        return -1;
                    }
                    wchar ch2 = *++in;
                    if (ch2 < 0xDC00 || ch2 > 0xDFFF) {
                        // Invalid...
                        // We'll encode 0xFFFD for this
                        outLen += 3;
                    } else {
                        outLen += 4;
                    }
                }
                else {
                    outLen += 3;
                }
                in++;
            }
        }
        return outLen;
    }

    int32_t UnicodeUtils::Utf8ToUtf16(const uint8_t *in,
                                      int32_t inLen,
                                      wchar *out,
                                      int32_t outMax,
                                      bool strict)
    {
        int32_t outLen = 0;
        uint32_t outch;
        while (inLen > 0)
        {
            uint32_t c = uint32_t (*in);

            switch (c >> 4)
            {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                // 0xxx xxxx
                // Let the converted == false case handle this.
                break;

            case 12: case 13:
                // 110xxxxx   10xxxxxx
                if (inLen < 2) {
                    // Invalid
                    goto invalid;
                }
                if ((in[1]&0xC0) != 0x80) {
                    // Invalid
                    goto invalid;
                }
                outch = ((c<<6 & 0x7C0) | (in[1] & 0x3F));
                if (outch < 0x80) {
                    // Overlong sequence, reject as invalid.
                    goto invalid;
                }
                in += 2;
                inLen -= 2;
                if (out) {
                    if (--outMax < 0) {
                        return -1;
                    }
                    *out++ = (wchar)(outch);
                }
                outLen++;
                continue;

            case 14:
                // 1110xxxx  10xxxxxx  10xxxxxx
                if (inLen < 3) {
                    // Invalid
                    goto invalid;
                }
                if ((in[1]&0xC0) != 0x80 || (in[2]&0xC0) != 0x80) {
                    // Invalid
                    goto invalid;
                }
                outch = ((c<<12 & 0xF000) | (in[1]<<6 & 0xFC0) | (in[2] & 0x3F));
                if (outch < 0x800) {
                    // Overlong sequence, reject as invalid.
                    goto invalid;
                }
                in += 3;
                inLen -= 3;
                if (out) {
                    if (--outMax < 0) {
                        return -1;
                    }
                    *out++ = (wchar)(outch);
                }
                outLen++;
                continue;

            case 15:
                // 11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
                // 111110xx ... is always invalid
                // 1111110x ... is always invalid
                // note: when 'strict' is false, we need to mimic the behavior of FP9/FP10,
                // which did not do the c&8 test. so skip it for bug-compatibility.
                if ((strict && (c & 0x08)) || (inLen < 4)) {
                    // Invalid
                    goto invalid;
                }
                if ((in[1]&0xC0) != 0x80 ||
                    (in[2]&0xC0) != 0x80 ||
                    (in[3]&0xC0) != 0x80)
                {
                    goto invalid;
                }

                outch = ((c<<18     & 0x1C0000) |
                         (in[1]<<12 & 0x3F000) |
                         (in[2]<<6  & 0xFC0) |
                         (in[3]     & 0x3F));
                if (outch < 0x10000) {
                    // Overlong sequence, reject as invalid.
                    goto invalid;
                }

                in += 4;
                inLen -= 4;

                // Encode as UTF-16 surrogate sequence
                if (out) {
                    if ((outMax -= 2) < 0) {
                        return -1;
                    }
                    *out++ = (wchar) (((outch-0x10000)>>10) & 0x3FF) + 0xD800;
                    *out++ = (wchar) ((outch-0x10000) & 0x3FF) + 0xDC00;
                }
                outLen += 2;
                continue;
            default:
            invalid:
                if (strict)
                    return -1;
                // else fall thru
            }

            // ! converted
            if (out) {
                if (--outMax < 0) {
                    return -1;
                }
                *out++ = (wchar)c;
            }
            inLen--;
            in++;
            outLen++;
        }
        return outLen;
    }

    int32_t UnicodeUtils::Utf8ToUcs4(const uint8_t *chars,
                                     int32_t len,
                                     uint32_t *out,
                                     bool bugzilla609416)
    {
        // U-00000000 - U-0000007F:     0xxxxxxx
        // U-00000080 - U-000007FF:     110xxxxx 10xxxxxx
        // U-00000800 - U-0000FFFF:     1110xxxx 10xxxxxx 10xxxxxx
        // U-00010000 - U-001FFFFF:     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        // U-00200000 - U-03FFFFFF:     111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        // U-04000000 - U-7FFFFFFF:     1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

        // The minUCS4 table enforces the security rule that an
        // overlong UTF-8 sequence is forbidden, if a shorter
        // sequence could encode the same character.
        static const uint32_t minUCS4[] = {
            0x00000000,
            0x00000080,
            0x00000800,
            0x00010000,
            0x00200000,
            0x04000000
        };
        int32_t n = 0;
        uint32_t b;
        if (len < 1) {
            return 0;
        }
        switch (chars[0]>>4) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            n = 1;
            b = chars[0];
            break;
        case 12: case 13:
            n = 2;
            b = chars[0]&0x1F;
            break;
        case 14:
            n = 3;
            b = chars[0]&0x0F;
            break;
        case 15:
            switch (chars[0]&0x0C) {
            case 0x00:
            case 0x04:
                n = 4;
                b = chars[0]&0x07;
                break;
            case 0x08:
                n = 5;
                b = chars[0]&0x03;
                break;
            case 0x0C:
                n = 6;
                b = chars[0]&0x01;
                break;
            default:  // invalid character, should not get here
                if (bugzilla609416)
                {
                    return 0;
                }
            }
            if (bugzilla609416)
            {
                break;
            }

        default: // invalid character, should not get here
            return 0;
        }
        if (len < n) {
            return 0;
        }
        for (int i=1; i<n; i++) {
            if ((chars[i]&0xC0) != 0x80) {
                return 0;
            }
            b = (b<<6) | (chars[i]&0x3F);
        }
        if (b < minUCS4[n-1]) {
            return 0;
        }
        *out = b;
        return n;
    }

    int32_t UnicodeUtils::Ucs4ToUtf8(uint32_t value,
                                     uint8_t *chars)
    {
        // U-00000000 - U-0000007F:     0xxxxxxx
        // U-00000080 - U-000007FF:     110xxxxx 10xxxxxx
        // U-00000800 - U-0000FFFF:     1110xxxx 10xxxxxx 10xxxxxx
        // U-00010000 - U-001FFFFF:     11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        // U-00200000 - U-03FFFFFF:     111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        // U-04000000 - U-7FFFFFFF:     1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (value < 0x80) {
            *chars = (uint8_t)value;
            return 1;
        }
        if (value < 0x800) {
            chars[0] = (uint8_t)(0xC0 | ((value>>6)&0x1F));
            chars[1] = (uint8_t)(0x80 | (value&0x3F));
            return 2;
        }
        if (value < 0x10000) {
            chars[0] = (uint8_t)(0xE0 | ((value>>12)&0x0F));
            chars[1] = (uint8_t)(0x80 | ((value>>6)&0x3F));
            chars[2] = (uint8_t)(0x80 | (value&0x3F));
            return 3;
        }
        if (value < 0x200000) {
            chars[0] = (uint8_t)(0xF0 | ((value>>18)&0x07));
            chars[1] = (uint8_t)(0x80 | ((value>>12)&0x3F));
            chars[2] = (uint8_t)(0x80 | ((value>>6)&0x3F));
            chars[3] = (uint8_t)(0x80 | (value&0x3F));
            return 4;
        }
        if (value < 0x4000000) {
            chars[0] = (uint8_t)(0xF8 | ((value>>24)&0x03));
            chars[1] = (uint8_t)(0x80 | ((value>>18)&0x3F));
            chars[2] = (uint8_t)(0x80 | ((value>>12)&0x3F));
            chars[3] = (uint8_t)(0x80 | ((value>>6)&0x3F));
            chars[4] = (uint8_t)(0x80 | (value&0x3F));
            return 5;
        }
        if (value < 0x80000000) {
            chars[0] = (uint8_t)(0xFC | ((value>>30)&0x01));
            chars[1] = (uint8_t)(0x80 | ((value>>24)&0x3F));
            chars[2] = (uint8_t)(0x80 | ((value>>18)&0x3F));
            chars[3] = (uint8_t)(0x80 | ((value>>12)&0x3F));
            chars[4] = (uint8_t)(0x80 | ((value>>6)&0x3F));
            chars[5] = (uint8_t)(0x80 | (value&0x3F));
            return 6;
        }
        return 0;
    }
}
