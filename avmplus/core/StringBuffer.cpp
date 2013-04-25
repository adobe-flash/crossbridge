/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    using namespace MMgc;

    StringBuffer::StringOutputStream::StringOutputStream(MMgc::GC *gc)
    {
        m_gc = gc;
        m_buffer = NULL;
        m_length = 0;
    }

    StringBuffer::StringOutputStream::~StringOutputStream()
    {
        if (m_buffer != NULL) {
            m_gc->Free(m_buffer);
            m_buffer = 0;
        }
        m_gc = 0;
        m_length = 0;
    }

    void StringBuffer::StringOutputStream::write(const char* utf8)
    {
        writeN(utf8, String::Length(utf8));
    }

    void StringBuffer::StringOutputStream::ensureCapacity(size_t count)
    {
        if (m_buffer == NULL || m_length+count >= (size_t)GC::Size(m_buffer))
        {
            size_t newCapacity = (m_length+count+1)*2;
            char* newBuffer = (char*) m_gc->Alloc(newCapacity);
            if (m_buffer != NULL) {
                VMPI_memcpy(newBuffer, m_buffer, m_length);
                m_gc->Free(m_buffer);
            }
            if (m_gc->IsPointerToGCPage(this))
                WB(m_gc, m_gc->FindBeginningFast(this), &m_buffer, newBuffer);
            else
                m_buffer = newBuffer;
        }
    }

    void StringBuffer::StringOutputStream::writeN(const char* buffer, size_t count)
    {
        ensureCapacity(m_length + count);
        VMPI_memcpy(m_buffer+m_length, buffer, count);
        m_length += (int)count;
        m_buffer[m_length] = 0;
    }

    void StringBuffer::writeN(const char* buffer, size_t count)
    {
        m_stream.writeN(buffer, count);
    }
}
