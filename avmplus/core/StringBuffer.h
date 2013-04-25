/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_StringBuffer__
#define __avmplus_StringBuffer__

namespace avmplus
{
    /**
     * StringBuffer is a PrintWriter subclass which enables easy, cout-like output of
     * text to a string buffer in memory.
     *
     * NOTE!  This is *not* a GC class.  A StringBuffer instance *must* be allocated
     * on the stack, inside GCRoots, or inside GC'd objects (as a member, not a pointer).
     *
     * A StringBuffer has a destructor which will free resouces opportunistically,
     * but if an exception jumps over the destructor then resources will not leak.
     */
    class StringBuffer : public PrintWriter
    {
    public:
        StringBuffer(AvmCore* core)
            : PrintWriter(core)
            , m_stream(core->gc)
        {
            setOutputStream(&m_stream);
        }

        void writeN(const char* utf8, size_t count);

        const char* c_str() { return m_stream.c_str(); }    // note, always in UTF8 form
        int length() const { return m_stream.length(); }
        void reset() { m_stream.reset(); }

        String* toString() { return core()->newStringUTF8(c_str(), length()); }

    private:
        /**
         * StringOutputStream accumulates any data written to the stream into an in-memory buffer.
         * The buffer can later be retrieved using the c_str method.
         *
         * NOTE!  This is *not* a GC object.  You *must* ensure that the destructor of this object
         * is called in order to properly deallocate resources.
         *
         * lhansen made StringOutputStream a private class of StringBuffer on 2011-02-26 because
         * StringBuffer is the only client of StringOutputStream in the entire Flash Player source tree.
         */
        class StringOutputStream : public NonGCOutputStream
        {
        public:
            StringOutputStream(MMgc::GC *gc);
            ~StringOutputStream();

            const char* c_str() { return m_buffer; }    // note, always in UTF8 form
            int length() const { return m_length; }
            void reset() { m_length=0; }
            
            void write(const char* utf8);
            void writeN(const char* utf8, size_t count);
            
        private:
            enum { kInitialCapacity = 256 };

            void ensureCapacity(size_t count);
    
            MMgc::GC* m_gc;
            char* m_buffer;     // managed memory
            int m_length;
            
            void grow(int newLength);
        };
        
        StringOutputStream m_stream;
    };
}

#endif /* __avmplus_StringBuffer__ */
