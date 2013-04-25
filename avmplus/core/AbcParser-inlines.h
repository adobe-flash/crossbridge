/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE int AbcParser::readU16(const uint8_t* p) const
{
    if (p < abcStart || p+1 >= abcEnd)
        toplevel->throwVerifyError(kCorruptABCError);
    return p[0] | p[1]<<8;
}

REALLY_INLINE int AbcParser::readS32(const uint8_t *&p) const
{
    // We have added kBufferPadding bytes to the end of the main swf buffer.
    // Why?  Here we can read from 1 to 5 bytes.  If we were to
    // put the required safety checks at each byte read, we would slow
    // parsing of the file down.  With this buffer, only one check at the
    // top of this function is necessary. (we will read on into our own memory)
    if ( p < abcStart || p >= abcEnd )
        toplevel->throwVerifyError(kCorruptABCError);

    int result = p[0];
    if (!(result & 0x00000080)) {
        p++;
        return result;
    }
    result = (result & 0x0000007f) | p[1]<<7;
    if (!(result & 0x00004000)) {
        p += 2;
        return result;
    }
    result = (result & 0x00003fff) | p[2]<<14;
    if (!(result & 0x00200000)) {
        p += 3;
        return result;
    }
    result = (result & 0x001fffff) | p[3]<<21;
    if (!(result & 0x10000000)) {
        p += 4;
        return result;
    }
    result = (result & 0x0fffffff) | p[4]<<28;
    p += 5;
    return result;
}

} // namespace avmplus
