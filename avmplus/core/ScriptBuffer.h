/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ScriptBuffer__
#define __avmplus_ScriptBuffer__


namespace avmplus
{
    /**
     * ScriptBufferImpl is the base class for script buffer
     * implementations.
     *
     * This is a pure base class which must be subclassed to be used.
     * Its methods are not virtual for performance reasons.
     * For default script buffer behavior, use BasicScriptBufferImpl.
     */
    class ScriptBufferImpl : public MMgc::GCFinalizedObject
    {
    public:
        operator uint8_t* () const { return buffer; }
        size_t getSize() const { return size; }
        uint8_t* getBuffer() const { return (uint8_t*)buffer; }

        uint8_t operator[] (int index) const {
            return buffer[index];
        }
        uint8_t& operator[] (int index) {
            return buffer[index];
        }
    protected:
        ScriptBufferImpl() {}
        uint8_t *buffer;
        size_t size;
    };

    /**
     * BasicScriptBufferImpl is a ScriptBuffer implementation that
     * owns its own buffer and has no external dependencies.
     */
    class BasicScriptBufferImpl : public ScriptBufferImpl
    {
    public:
        BasicScriptBufferImpl(size_t _size)
        : ScriptBufferImpl()
        {
            this->size = _size;
            buffer = (uint8_t*)(this+1);
        }
        // override to skip memset and prevent marking
        static void *operator new(size_t size, MMgc::GC *gc, size_t extra = 0)
        {
            return gc->Alloc(size + extra, 0);
        }
    };

    /**
     * ReadOnlyScriptBufferImpl is a ScriptBuffer implementation that
     * points to an external buffer that must stay around longer than
     * the script buffer itself.
     */
    class ReadOnlyScriptBufferImpl : public ScriptBufferImpl
    {
    public:
        ReadOnlyScriptBufferImpl(const uint8_t * _buf, size_t _size)
        : ScriptBufferImpl()
        {
            this->size = _size;
            this->buffer = (uint8_t*) _buf;
        }
    };

    /**
     * ScriptBuffer is a "handle" for ScriptBufferImpl which is more convenient
     * to pass around and use than a ScriptBufferImpl pointer.  It defines
     * operator[] to make a ScriptBuffer look like an array.
     */
    class ScriptBuffer
    {
    public:
        ScriptBuffer() {
            m_impl = NULL;
        }
        ScriptBuffer(ScriptBufferImpl* rep) {
            m_impl = rep;
        }
        ScriptBuffer(const ScriptBuffer& toCopy) {
            m_impl = toCopy.m_impl;
        }
        ScriptBuffer& operator= (const ScriptBuffer& toCopy) {
            m_impl = toCopy.m_impl;
            return *this;
        }
        ScriptBufferImpl* getImpl() const {
            return m_impl;
        }
        operator ScriptBufferImpl*() const {
            return m_impl;
        }
        operator uint8_t* () const {
            return m_impl->getBuffer();
        }
        size_t getSize() const {
            return m_impl->getSize();
        }
        uint8_t* getBuffer() const {
            return m_impl->getBuffer();
        }
        uint8_t operator[] (int index) const {
            return (*m_impl)[index];
        }
        uint8_t& operator[] (int index) {
            return (*m_impl)[index];
        }

    private:
        ScriptBufferImpl* m_impl;
    };
}

#endif /* __avmplus_ScriptBuffer__ */
