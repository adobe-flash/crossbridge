/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_BitSet__
#define __avmplus_BitSet__


namespace avmplus
{
    /**
     * Bit vectors are an efficent method of keeping True/False information
     * on a set of items or conditions. Class BitSet provides functions
     * to manipulate individual bits in the vector.
     *
     * Since most vectors are rather small an array of machine words is used by
     * default to house the value of the bits.  If more bits are needed
     * then an array is allocated dynamically outside of this object.
     *
     * This object is not optimized for a fixed sized bit vector
     * it instead allows for dynamically growing the bit vector.
     */
    class BitSet
    {
        private:
            uintptr_t *getbits() {
                return capacity > kDefaultCapacity ? bits.ptr : bits.ar;
            }
            const uintptr_t *getbits() const {
                return capacity > kDefaultCapacity ? bits.ptr : bits.ar;
            }
            static const uintptr_t kOne = 1;

        public:

            enum {  kUnit = 8*sizeof(uintptr_t),
                    kDefaultCapacity = 4   };

            BitSet(): capacity(kDefaultCapacity) {
                reset();
            }

            BitSet(int bitcap) : capacity(kDefaultCapacity) {
                reset();
                int cap = ((bitcap+kUnit-1)/kUnit);
                if (cap > kDefaultCapacity)
                    grow(cap);
            }

            virtual ~BitSet() {
                if (capacity > kDefaultCapacity) {
                    mmfx_delete_array(bits.ptr);
                    bits.ptr = 0;
                    capacity = kDefaultCapacity;
                }
            }

            void reset()
            {
                uintptr_t *bits = getbits();
                for (int i=0, n = capacity; i < n; i++)
                    bits[i] = 0;
            }

            void set(int bitNbr)
            {
                int index = bitNbr / kUnit;
                int bit = bitNbr % kUnit;
                int cap = this->capacity;
                if (index >= cap) {
                    cap *= 2;
                    while (index >= cap) {
                        cap *= 2;
                    }
                    grow(cap);
                }
                getbits()[index] |= kOne << bit;
            }

            void clear(int bitNbr)
            {
                int index = bitNbr / kUnit;
                int bit = bitNbr % kUnit;
                if (index < capacity)
                    getbits()[index] &= ~(kOne << bit);
            }

            bool get(int bitNbr) const
            {
                int index = bitNbr / kUnit;
                int bit = bitNbr % kUnit;
                return index < capacity && (getbits()[index] & kOne<<bit) != 0;
            }

            uintptr_t setFrom(BitSet &other) {
                int c = other.capacity;
                if (c > capacity)
                    grow(c);
                uintptr_t *bits = getbits();
                uintptr_t *otherbits = other.getbits();
                uintptr_t newbits = 0;
                for (int i=0; i < c; i++) {
                    uintptr_t b = bits[i];
                    uintptr_t b2 = otherbits[i];
                    newbits |= b2 & ~b; // bits in b2 that are not in b
                    bits[i] = b|b2;
                }
                return newbits;
            }

        private:

            // Grow the array until at least newCapacity big
            void grow(int newCapacity)
            {
                uintptr_t* newBits = mmfx_new_array( uintptr_t, newCapacity);
                // copy the old one
                uintptr_t *bits = getbits();
                for (int i=0, n=capacity; i < n; i++)
                    newBits[i] = bits[i];
                for (int i=capacity; i < newCapacity; i++)
                    newBits[i] = 0;

                // in with the new out with the old
                if (capacity > kDefaultCapacity)
                    mmfx_delete_array( bits );

                this->bits.ptr = newBits;
                capacity = newCapacity;
            }

            // by default we use the array, but if the vector
            // size grows beyond kDefaultCapacity we allocate
            // space dynamically.
            int capacity;
            union
            {
                uintptr_t ar[kDefaultCapacity];
                uintptr_t*  ptr;
            }
            bits;
    };

    // a simpler version of BitSet that doesn't dynamically grow,
    // and doesn't require allocation for <=31 (or 63) size. This expects
    // to be embedded as a member variable in a GCObject and will
    // use WB if allocation is necessary.
    class FixedBitSet : public MMgc::GCInlineObject
    {
    public:
        inline explicit FixedBitSet() : m_bits(0)
        #ifdef _DEBUG
            , m_cap(0)
        #endif
        {
        }

        inline void gcTrace(MMgc::GC* gc)
        {
            if (!(m_bits & 1))
                gc->TraceLocation(&m_bits);
        }

        // blows away existing content, sets all bits to 0
        inline void resize(MMgc::GC* gc, uint32_t cap)
        {
            const uint32_t MAX_INLINE_BITS = BITS_PER_UINTPTR - 1;
            if (cap > MAX_INLINE_BITS)
            {
                // always allocate one more bit than we need to simplify the logic elsewhere (bit 0 goes unused)
                const uint32_t count = (cap + (BITS_PER_UINTPTR - 1) + 1) / BITS_PER_UINTPTR;
                const uint32_t sz = count * sizeof(uintptr_t);
                uintptr_t* v = (uintptr_t*)gc->Alloc(sz, MMgc::GC::kZero);
                void* beginning =  gc->FindBeginningFast(this);
                if (beginning)
                {
                    WB(gc, beginning, &m_bits, v);
                }
                else
                {
                    m_bits = uintptr_t(v);
                }
            }
            else
            {
                m_bits = 0x1;
            }
        #ifdef _DEBUG
            m_cap = cap;
        #endif
        }

        inline void set(uint32_t bit)
        {
            AvmAssert(bit < m_cap);
            uintptr_t* v = (m_bits & 1) ? &m_bits : (uintptr_t*)m_bits;
            const uint32_t nbit = bit + 1;
            v[nbit / BITS_PER_UINTPTR] |= (uintptr_t(1) << (nbit & (BITS_PER_UINTPTR-1)));
        }

        inline void clr(uint32_t bit)
        {
            AvmAssert(bit < m_cap);
            uintptr_t* v = (m_bits & 1) ? &m_bits : (uintptr_t*)m_bits;
            const uint32_t nbit = bit + 1;
            v[nbit / BITS_PER_UINTPTR] &= ~(uintptr_t(1) << (nbit & (BITS_PER_UINTPTR-1)));
        }

        inline bool test(uint32_t bit) const
        {
            AvmAssert(bit < m_cap);
            const uint32_t nbit = bit + 1;
            uintptr_t w = m_bits;
            if (!(w & 1))
                w = ((const uintptr_t*)m_bits)[nbit / BITS_PER_UINTPTR];
            return (w & (uintptr_t(1) << (nbit & (BITS_PER_UINTPTR-1)))) != 0;
        }

        inline size_t allocatedSize()
        {
            return ((m_bits & 1) || (m_bits == 0)) ? 0 : MMgc::GC::Size((void*)m_bits);
        }

        #ifdef _DEBUG
        inline uint32_t cap() const { return uint32_t(m_cap); }
        #endif

    private:
        enum { BITS_PER_UINTPTR = sizeof(uintptr_t)*8 };
    private:
        uintptr_t m_bits;   // low bit 1 == inline, low bit 0 == dynamic alloc
        #ifdef _DEBUG
        uintptr_t m_cap;
        #endif
    };

}

#endif /* __avmplus_BitSet__ */
