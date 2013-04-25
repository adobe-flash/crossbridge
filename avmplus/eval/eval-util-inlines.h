/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

inline void* Allocator::alloc(size_t nbytes)
{
    nbytes = (nbytes + 7) & ~7;
    if (current_top + nbytes <= current_limit) {
        void *p = current_top;
        current_top += nbytes;
        return p;
    }
    return allocSlow(nbytes);
}

inline uint8_t* emitU16(uint8_t* out, uint16_t val)
{
    *out++ = val & 255;
    *out++ = (val >> 8) & 255;
    return out;
}

inline uint8_t* emitU30(uint8_t* out, uint32_t val)
{
    AvmAssert(val <= 0x3FFFFFFF);
    return emitU32(out, val);
}

inline uint8_t* emitS32(uint8_t* out, int32_t val)
{
    return emitU32(out, (uint32_t)val);
}

template<class T> inline Seq<T>* SeqBuilder<T>::get() const
{
    return items;
}

template<class T> inline void SeqBuilder<T>::clear()
{
    items = last = NULL;
}

template<class T> inline bool SeqBuilder<T>::isEmpty()
{
    return items == NULL;
}
    
inline uint32_t ByteBuffer::size() const
{
    return last == NULL ? 0 : (uint32_t)(size_rest + (out - last->start));
}

inline void ByteBuffer::makeRoom(uint32_t nbytes)
{
    if (out + nbytes > limit)
        makeRoomSlow(nbytes);
}

inline void ByteBuffer::emitU8(uint8_t v)
{
    makeRoom(1);
    *out++ = v;
}

inline void ByteBuffer::emitS8(int8_t v)
{
    emitU8((uint8_t)v);
}

inline void ByteBuffer::emitU16(uint16_t v)
{
    makeRoom(2);
    out = avmplus::RTC::emitU16(out, v);
}

inline void ByteBuffer::emitS24(int32_t v)
{
    makeRoom(3);
    out = avmplus::RTC::emitS24(out, v);
}

inline void ByteBuffer::emitU30(uint32_t v)
{
    makeRoom(5);
    out = avmplus::RTC::emitU30(out, v);
}

inline void ByteBuffer::emitU32(uint32_t v)
{
    makeRoom(5);
    out = avmplus::RTC::emitU32(out, v);
}

inline void ByteBuffer::emitS32(int32_t v)
{
    makeRoom(5);
    out = avmplus::RTC::emitS32(out, v);
}

inline void ByteBuffer::emitDouble(double v)
{
    makeRoom(8);
    out = avmplus::RTC::emitDouble(out, v);
}

inline void ByteBuffer::emitFloat(float v)
{
    makeRoom(4);
    out = avmplus::RTC::emitFloat(out, v);
}

inline void ByteBuffer::emitFloat4(const float4_t& v)
{
    makeRoom(16);
    out = avmplus::RTC::emitFloat4(out, v);
}

inline void ByteBuffer::emitUtf8(uint32_t nbytes, Str* str)
{
    makeRoom(nbytes);
    out = avmplus::RTC::emitUtf8(out, str);
}

inline uint32_t StringBuilder::length()
{
    return len;
}

}}
