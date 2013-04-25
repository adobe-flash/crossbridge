/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_UnicodeUtils__
#define __avmplus_UnicodeUtils__

namespace avmplus
{
    /**
     * Utility class for converting between Unicode representations.
     */
    class UnicodeUtils
    {
    public:
        /**
         * Utf16ToUtf8 converts a UTF-16 sequence to UTF-8.
         *
         * This encoder will check UTF-16 sequences for validity,
         * mainly meaning invalid surrogate sequences
         * (0xD800-0xDBFF not preceding 0xDC00-0xDFFF)
         *
         * Parameters:
         * @param in      input UTF-16 buffer to encode.  The buffer is not
         *                assumed to be zero-terminated.
         * @param inLen   length of input buffer, in wchars.
         * @param out     output UTF-8 buffer.  NULL may be passed, in
         *                which case the UTF-16 sequence is checked for
         *                validity and the number of output bytes
         *                is calculated.
         * @param outMax  size available in output buffer, in # of bytes.
         *                Ignored if out is NULL.
         *
         * @return The number of bytes encoded, or -1 if an error occurs.
         *
         *    An error occurs in any of the following conditions:
         *    - The UTF-16 sequence is invalid (bad surrogates)
         *    - The output buffer contains insufficient space
         *      to represent the entire UTF-8 sequence.
         */
        static int32_t Utf16ToUtf8(const wchar *in,
                               int32_t inLen,
                               uint8_t *out,
                               int32_t outMax);

        /**
         * Utf8ToUtf16 converts a UTF-8 sequence to UTF-16.
         *
         * This decoder will check UTF-8 sequences for validity,
         * including checking for overlong sequences (considered a
         * security risk).  It will decode UCS-4 values from
         * 0x010000-0x10FFFF to a UTF-16 surrogate sequence.
         *
         * @param in     input UTF-8 buffer to decode.  The buffer is not
         *               assumed to be zero-terminated.
         * @param inLen  length of input buffer, in bytes.
         * @param out    output UTF-16 buffer.  NULL may be passed, in
         *               which case the UTF-8 sequence is checked for
         *               validity and the number of output characters
         *               is calculated.
         * @param outMax size available in output buffer, in # of wchars.
         *               Ignored if out is NULL.
         * @param strict if false, invalid UTF-8 sequences are copied as
         *               single characters; if true, the method returns
         *               -1. Setting strict to false conserves backwards
         *               compabitility, and should be revisited in a future
         *               release.
         *
         * @return The number of wchars decoded, or -1 if an error occurs.
         *
         *    An error occurs in any of the following conditions:
         *    - The UTF-8 sequence is invalid, and strict is true.
         *    - The output buffer contains insufficient space
         *      to represent the entire UTF-8 sequence.
         *    - The UTF-8 sequence contains characters that cannot
         *      be represented in ECMAScript (code point >0x10FFFF),
         *      and strict is true
         */
        static int32_t Utf8ToUtf16(const uint8_t *in,
                                   int32_t inLen,
                                   wchar   *out,
                                   int32_t outMax,
                                   bool    strict);

        inline int32_t Utf8Count(const uint8_t *in, int32_t inLen, bool strict) {
            return Utf8ToUtf16(in, inLen, NULL, 0, strict);
        }

        /**
         * Utf8ToUcs4 converts the UTF-8 sequence "chars", which
         * is "len" bytes in length, to a single UCS-4 character.
         * The input is not assumed to be null-terminated.
         *
         * The actual number of bytes consumed is returned, or
         * 0 is the UTF-8 sequence is malformed.
         */
        static int32_t Utf8ToUcs4(const uint8_t *chars,
                                  int32_t len,
                                  uint32_t *out,
                                  bool bugzilla609416);

        /**
         * Ucs4ToUtf8 takes a single 32-bit UCS-4 character as
         * input and encodes it as UTF-8.
         *
         * The output buffer "out" is assumed to be at least 6
         * bytes in length.  The output is not null-terminated.
         *
         * Ucs4ToUtf8 returns the number of bytes written to
         * "out", or 0 if the UCS-4 character cannot be encoded
         * to UTF-8 (>0x7fffffff)
         */
        static int32_t Ucs4ToUtf8(uint32_t value,
                                  uint8_t* out);
    };
}

#endif /* __avmplus_UnicodeUtils__ */
