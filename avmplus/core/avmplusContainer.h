/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Container__
#define __avmplus_Container__

namespace avmplus
{
    /**
     * The instances of this utility class hold fixed-size indexed
     * sets of constructor-less structs with a 'void gcTrace(GC*)'
     * method.  The structs will be initialized to all-zero-bits.
     */
    template <class T>
    class ExactStructContainer : public MMgc::GCFinalizedObject
    {
    public:
        static ExactStructContainer* create(MMgc::GC* gc, void (*finalizer)(ExactStructContainer<T>* self), uint32_t capacity) {
            size_t extra = 0;
            if (capacity > 0)
                extra = MMgc::GCHeap::CheckForCallocSizeOverflow(capacity-1, sizeof(T));
            ExactStructContainer<T>* obj;
            if (finalizer == NULL)
                obj = new (gc, MMgc::kExact, MMgc::kNoFinalize, extra) ExactStructContainer<T>(capacity, finalizer);
            else
                obj = new (gc, MMgc::kExact, extra) ExactStructContainer<T>(capacity, finalizer);
            return obj;
        }

        ~ExactStructContainer() {
            if (_finalizer != NULL)
                _finalizer(this);
        }

        uint32_t capacity() {
            return _capacity;
        }

        T& get(uint32_t index) {
            AvmAssert(index < capacity());
            return elements[index];
        }

        T& operator[](uint32_t index) {
            return get(index);
        }

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor)
        {
            uint32_t cap = capacity();
            const uint32_t work_increment = 2000/sizeof(void*);
            if (work_increment * cursor >= cap)
                return false;
            size_t work = work_increment;
            bool more = true;
            if (work_increment * (cursor + 1) >= cap) {
                work = cap - (work_increment * cursor);
                more = false;
            }
            for ( size_t i=0 ; i < work ; i++ )
                elements[(work_increment * cursor) + i].gcTrace(gc);
            return more;
        }
        
    private:
        ExactStructContainer(uint32_t capacity, void (*finalizer)(ExactStructContainer* self)) : _capacity(capacity), _finalizer(finalizer) {}

        uint32_t _capacity;
        void     (*_finalizer)(ExactStructContainer* self);
        T        elements[1];     // Actually max(1,capacity())

        friend class CodegenLIR; // for offsetof(elements)
    };
}

#endif /* __avmplus_Container__ */
