/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "avmplus.h"

// This is the maximum value for the characters Left field in the
// m_bitsAndFlags field of the String instance. Strings grow by
// by doubling the available buffer size until the characters left
// exceeds the size of the field. From there on, the buffer grows
// linear. Note that this number is divided by two to make sure that
// the counter actually, spans the entire buffer area; if this would
// not be done, the calculation of characters left (using GC::Size())
// would return a wrong number.
#define TSTR_MAX_CHARSLEFT ((uint32_t)TSTR_CHARSLEFT_MASK >> TSTR_CHARSLEFT_SHIFT >> 1)

// This is the maximum value of extra bytes to allocate during a
// string concatenation operation if PleaseAlloc() fails. The number
// is quite small to not kill the app just because of a large extra
// chunk of RAM being allocated just in case there are more concatenation
// operations.
#define TSTR_MAX_LOMEM_EXTRABYTES 16384

// Never allocate dynamic strings smaller than this (in bytes)
#define TSTR_MIN_DYNAMIC_ALLOCATION 32

// in fixDependentString, don't bother going dependent->dynamic if the memory
// we would save is < this (in bytes)
#define TSTR_DEPENDENT_STRING_NUISANCE_SAVINGS int32_t(sizeof(String))

using namespace MMgc;

namespace avmplus
{
    // Use this constant for 16 and 32 bit strings of zero length
    // we do not want any string to contain NULL buffer pointers
    union Zero
    {
        uint32_t u32;
        wchar u16;
        uint8_t u8;
        char c8;
    };
    static const Zero k_zero = { 0 };

/////////////////////////// Helpers: Widening //////////////////////////////

    REALLY_INLINE void _widen8_16(const uint8_t* src, wchar* dst, int32_t len)
    {
        while (len-- > 0)
            *dst++ = wchar(*src++);
    }

/////////////////////////// Helpers: Narrowing //////////////////////////////

    REALLY_INLINE void _narrow16_8(const wchar* src, uint8_t* dst, int32_t len)
    {
        while (len-- > 0)
        {
            AvmAssert(*src <= 0xFF);
            *dst++ = (uint8_t) *src++;
        }
    }

/////////////////////////// Helpers: Copying ///////////////////////////////

    // Copy the contents of the given buffer to another buffer, given two widths.

    static void* _copyBuffers(const void* src, void* dst, int32_t srcLen, String::Width srcWidth, String::Width dstWidth)
    {
        if (srcWidth == dstWidth)
            VMPI_memcpy(dst, src, srcLen << srcWidth);
        else if (srcWidth == String::k8)
            _widen8_16((const uint8_t*) src, (wchar*) dst, srcLen);
        else
            _narrow16_8((const wchar*) src, (uint8_t*) dst, srcLen);
        // return the new buffer pointer
        return (char*) dst + (srcLen << dstWidth);
    }

#ifdef _DEBUG
    void String::verify7bit() const
    {
        if (!(m_bitsAndFlags & TSTR_7BIT_FLAG))
            return;

        if (getWidth() == k8)
        {
            Pointers ptrs(this);
            for (int32_t i = 0; i < m_length; ++i)
            {
                if (ptrs.p8[i] > 127)
                {
                    AvmAssert(!"hi bit found on string with TSTR_7BIT_FLAG set");
                }
            }
        }
        else
        {
            AvmAssert(!"only k8 strings should set TSTR_7BIT_FLAG");
        }
    }
#endif

/////////////////////////// Helper: get AvmCore /////////////////////////////

    REALLY_INLINE MMgc::GC* _gc(const String* s)
    {
        return MMgc::GC::GetGC(s);
    }

    REALLY_INLINE AvmCore* _core(const String* s)
    {
        MMgc::GC *gc = MMgc::GC::GetGC(s);
        return gc->core();
    }

/////////////////////////// Helpers: templated functions /////////////////////////////

    //
    // use typetraits to generate a compile-time error if you attempt to use a
    // non-unsigned char type as an argument to these.
    //
    // Note that we deliberately are using a bitwise &, not a logical && here...
    // GCC 4.3 and later, with -Wlogical-op enabled, will complain about the &&
    // construct with
    //
    // "warning: logical '&&' with non-zero constant will always evaluate as true"
    //
    // and since many embedders compile with warnings-as-errors enabled, this crushes
    // the build. This seems like a highly questionable warning to me, but since there
    // is a valid workaround, we'll go with it rather than requiring embedders to disable
    // that warning.
    //
    #define PREVENT_SIGNED_CHAR_PTR(TYPE) \
    { \
        typedef MMgc::is_same<char, uint8_t> is_char_unsigned; \
        typedef MMgc::is_same<char, TYPE> is_char; \
        MMGC_STATIC_ASSERT(!(is_char::value & !is_char_unsigned::value)); \
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR1, typename STR2>
    /*static*/ REALLY_INLINE bool equalsImpl(const STR1* str1, const STR2* str2, int32_t len)
    {
        PREVENT_SIGNED_CHAR_PTR(STR1)
        PREVENT_SIGNED_CHAR_PTR(STR2)

        for (int32_t j = 0; j < len; j++)
        {
            if (str1[j] != str2[j])
                return false;
        }
        return true;
    }

    // NOTE: this routine cannot use memcmp(), as the test
    // ecma3/String/localeCompare_rt.as depends on the difference
    // in character values, and memcmp() does not guarantee to
    // return this value

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR1, typename STR2>
    /*static*/ REALLY_INLINE int32_t compareImpl(const STR1* str1, const STR2* str2, int32_t len)
    {
        PREVENT_SIGNED_CHAR_PTR(STR1)
        PREVENT_SIGNED_CHAR_PTR(STR2)

        int32_t res = 0;
        while (len-- > 0 && !res)
        {
            res = int32_t(*str2++ - *str1++);
        }
        return res;
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR, typename PATTERN>
    /*static*/ REALLY_INLINE int32_t indexOfImpl(const STR* str, int32_t start, int32_t right, const PATTERN* pat, int32_t patlen)
    {
        PREVENT_SIGNED_CHAR_PTR(STR)
        PREVENT_SIGNED_CHAR_PTR(PATTERN)
        AvmAssert(patlen > 0);

        // even with REALLY_INLINE, some compilers will be reluctant to inline equalsImpl here,
        // so we explicitly repeat the code here.
        const PATTERN pat0 = pat[0];
        const STR* const end = str + right;
        for (const STR* probe = str + start; probe <= end; ++probe)
        {
            if (probe[0] == pat0)
            {
                for (int32_t j = 1; j < patlen; j++)
                {
                    if (probe[j] != pat[j])
                        goto no_match;
                }
                return int32_t(uintptr_t(probe - str));
            }
        no_match:
            // some compilers will complain about a label with no statement following,
            // hence the continue
            continue;
        }

        return -1;
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR>
    /*static*/ REALLY_INLINE int32_t indexOfCharCodeImpl(const STR* str, int32_t start, int32_t right, wchar c)
    {
        PREVENT_SIGNED_CHAR_PTR(STR)

        const STR* p = str + start - 1;
        const STR* const end = str + right;
        while (++p <= end)
            if (*p == c)
                return int32_t(p - str);

        return -1;
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR, typename PATTERN>
    /*static*/ REALLY_INLINE int32_t lastIndexOfImpl(const STR* str, int32_t const start, const PATTERN* pat, int32_t patlen)
    {
        PREVENT_SIGNED_CHAR_PTR(STR)
        PREVENT_SIGNED_CHAR_PTR(PATTERN)
        AvmAssert(patlen > 0);
        AvmAssert(start >= 0);

        // even with REALLY_INLINE, some compilers will be reluctant to inline equalsImpl here,
        // so we explicitly repeat the code here.
        const PATTERN pat0 = pat[0];
        for (const STR* probe = str + start; probe >= str; --probe)
        {
            if (probe[0] == pat0)
            {
                for (int32_t j = 1; j < patlen; j++)
                {
                    if (pat[j] != probe[j])
                        goto no_match;
                }
                return int32_t(uintptr_t(probe - str));
            }
        no_match:
            // some compilers will complain about a label with no statement following,
            // hence the continue
            continue;
        }
        return -1;
    }

    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions.
    template <typename STR>
    /*static*/ REALLY_INLINE int32_t hashCodeImpl(const STR* str, int32_t len)
    {
        PREVENT_SIGNED_CHAR_PTR(STR)

        // must be same signed-ness as other hashcode functions.
        // experimentation shows better results from signed (vs unsigned).
        int32_t hashCode = 0;
        while (len--)
            hashCode = (hashCode >> 28) ^ (hashCode << 4) ^ *str++;
        return hashCode;
    }

    /*static*/ REALLY_INLINE int32_t hashCodeAddChar(int32_t hashCode, char val)
    {
        // must be same signed-ness as other hashcode functions.
        // (match sign propagation in right-shift >> 28, for better or worse)
        return ((hashCode >> 28) ^ (hashCode << 4) ^ val);
    }

    /*static*/ REALLY_INLINE int32_t stringLengthUInt(uint32_t value)
    {
        if      (value < 10) return 1;
        else if (value < 100) return 2;
        else if (value < 1000) return 3;
        else if (value < 10000) return 4;
        else if (value < 100000) return 5;
        else if (value < 1000000) return 6;
        else if (value < 10000000) return 7;
        else if (value < 100000000) return 8;
        else if (value < 1000000000) return 9;
        else return 10;
    }

////////////////////////////// Constructors ////////////////////////////////

    // ctor for a static string.

    REALLY_INLINE String::String(const void* buffer, Width w, int32_t length, bool is7bit) :
#ifdef DEBUGGER
        AvmPlusScriptableObject(sotString()),
#endif
        m_buffer(buffer),   // static data - no WB() needed
        m_extra(NULL),
        m_length(length),
        m_bitsAndFlags(w | (kStatic << TSTR_TYPE_SHIFT) | (uint32_t(is7bit) << TSTR_7BIT_SHIFT))
    {
        AvmAssert(m_length >= 0);
        AvmAssert((uint64_t(m_length) << getWidth()) <= 0x7FFFFFFFU);
    }

    // ctor for a dynamic string.

    REALLY_INLINE String::String(MMgc::GC* gc, void* buffer, Width w, int32_t length, int32_t charsLeft, bool is7bit) :
#ifdef DEBUGGER
        AvmPlusScriptableObject(sotString()),
        m_buffer((void*)NULL),
#else
        m_buffer(buffer),
#endif
        m_extra(NULL),
        m_length(length),
        m_bitsAndFlags(w | (kDynamic << TSTR_TYPE_SHIFT) | (charsLeft << TSTR_CHARSLEFT_SHIFT) | (uint32_t(is7bit) << TSTR_7BIT_SHIFT))
    {
        AvmAssert(m_length >= 0);
        AvmAssert((uint64_t(m_length) << getWidth()) <= 0x7FFFFFFFU);
#ifdef DEBUGGER
        /*
            an explicit WB is only necessary if there's the possibility of an allocation between the time
            the container (String) is allocated and time we set the field. In nondebugger builds, there is no
            such possibility, but in DEBUGGER builds, the superclass ctor (AvmPlusScriptableObject) can
            cause an allocation if sampling is enabled. ("Icky and brittle", but that's how it is.)
        */
        WB(gc, this, &this->m_buffer.pv, buffer);
#else
        (void)gc;
#endif
    }

    // ctor for a dependent string.

    REALLY_INLINE String::String(MMgc::GC* gc, Stringp master, int32_t start, int32_t length) :
#ifdef DEBUGGER
        AvmPlusScriptableObject(sotString()),
#endif
        m_buffer(uintptr_t(start << master->getWidth())),
#ifdef DEBUGGER
        m_extra(NULL),
#else
        m_extra(master),
#endif
        m_length(length),
        // note that we propagate TSTR_7BIT_FLAG: if the entire master is TSTR_7BIT_FLAG, then so is the dependent string.
        // @todo: a dependent string could qualify for TSTR_7BIT_FLAG even if it's master is not. worth checking for?
        m_bitsAndFlags((master->m_bitsAndFlags & (TSTR_WIDTH_MASK | TSTR_7BIT_FLAG)) | (kDependent << TSTR_TYPE_SHIFT))
    {
        AvmAssert(m_length >= 0);
        AvmAssert((uint64_t(m_length) << getWidth()) <= 0x7FFFFFFFU);
#ifdef DEBUGGER
        WBRC(gc, this, &this->m_extra.master, master);
#else
        /*
            an explicit WBRC is only necessary if there's the possibility of an allocation between the time
            the container (String) is allocated and time we set the field. In nondebugger builds, there is no
            such possibility, but in DEBUGGER builds, the superclass ctor (AvmPlusScriptableObject) can
            cause an allocation if sampling is enabled. ("Icky and brittle", but that's how it is.)
        */
        AvmAssert(master != NULL);
        master->IncrementRef();
        (void)gc;
#endif
    }

    // add a and b and check for overflow

    static int32_t int32AddChecked(int32_t a, int32_t b)
    {
        if ((a | b) >= 0)       // both nonnegative?
        {
            uint64_t x = uint64_t(a) + uint64_t(b);
            if (x <= 0x7FFFFFFFU)
                return int32_t(x);
        }
        GCHeap::SignalObjectTooLarge();
        /*NOTREACHED*/
        return 0;
    }

    // shift a left by b and check for overflow

    static int32_t int32ShlChecked(int32_t a, int32_t b)
    {
        AvmAssert(b < 32);      // ok for this to be DEBUG-only, the shift amounts are known constants for practical purposes
        if ((a | b) >= 0)       // both nonnegative?
        {
            uint64_t x = uint64_t(a) << uint64_t(b);
            if (x <= 0x7FFFFFFFU)
                return int32_t(x);
        }
        GCHeap::SignalObjectTooLarge();
        /*NOTREACHED*/
        return 0;
    }

    static uint32_t uint32ShlChecked(uint32_t a, uint32_t b)
    {
        AvmAssert(b < 32);      // ok for this to be DEBUG-only, the shift amounts are known constants for practical purposes
        uint64_t x = uint64_t(a) << uint64_t(b);
        if (x <= 0xFFFFFFFFU)
            return uint32_t(x);
        GCHeap::SignalObjectTooLarge();
        /*NOTREACHED*/
        return 0;
    }

    // Private static method to create a dependent string

    // in nondebug builds this is a trivial wrapper around the ctor (which is itself trivial), so REALLY_INLINE it
    REALLY_INLINE /*static*/ Stringp String::createDependent(GC* gc, Stringp master, int32_t start, int32_t len)
    {
        AvmAssert(len >= 0);
        // master cannot be a dependent string
        AvmAssert(!master->isDependent());
        MMGC_MEM_TAG( "Strings" );
        Stringp s = new(gc, MMgc::kExact) String(gc, master, start, len);
        VERIFY_7BIT(s);
        return s;
    }

    // Private static method to create a dynamic string, given a buffer and its size in characters

    /*static*/ Stringp String::createDynamic(GC* gc, const void* data, int32_t len, Width w, bool is7bit, int32_t extra)
    {
        AvmAssert(w != kAuto);
        AvmAssert(len >= 0);

        // only 8-bit strings should set the 7-bit flag
        if (w != k8)
            is7bit = false;

        // a zero-length dynamic string is legal, but a zero-length GC allocation is not.
        int32_t alloc = int32AddChecked(len, extra);

        MMGC_MEM_TAG( "Strings" );

        // First, use PleaseAlloc(), and if the call fails, reduce the amount of extra data
        // to TSTR_MAX_EXTRA_BYTES_IN_LOW_MEMORY and do an Alloc(), which may fail.
        void* buffer = gc->PleaseAlloc(int32ShlChecked(alloc, w), 0);
        if (buffer == NULL)
        {
            if (extra > (TSTR_MAX_LOMEM_EXTRABYTES >> w))
                extra = TSTR_MAX_LOMEM_EXTRABYTES >> w;
            alloc = len + extra;                    // This is safe because the new value of 'extra' is smaller than the old, and the old was checked
            buffer = gc->Alloc(alloc << w, 0);      // Ditto
        }

        int32_t bufLen = (int32_t) (GC::Size(buffer) >> w);     // Note bufLen may be larger than (alloc << w)
        int32_t charsLeft = bufLen - len;
        // the extra character must not exceed the available field size.
        //
        // ok for this to be an assertion because we will not have problems with mmgc rounding up
        // to a 4KB boundary; the max chars left is about 4KB.
        AvmAssert(charsLeft <= int32_t((uint32_t) TSTR_CHARSLEFT_MASK >> TSTR_CHARSLEFT_SHIFT));

        Stringp s = new(gc, MMgc::kExact) String(gc, buffer, w, len, charsLeft, is7bit);

        if (data != NULL && len != 0)
            VMPI_memcpy(buffer, data, size_t(len << w));    // This is safe because alloc >= len and buffer size is alloc << w and that has been checked.
#ifdef _DEBUG
        // Terminate string with 0 for better debugging display
        if (charsLeft)
        {
            if (w == k8)
                s->m_buffer.p8[len] = 0;
            else
                s->m_buffer.p16[len] = 0;
        }
        if (data != NULL)
        {
            // don't try to verify if data==null, buffer will be random! (caller will verify)
            VERIFY_7BIT(s);
        }
#endif
        return s;
    }

    // Private static method to create a string, given a static buffer and its size in characters

    /*static*/ Stringp String::createStatic(GC* gc, const void* data, int32_t len, Width w, bool is7bit)
    {
        AvmAssert(w != kAuto);
        AvmAssert(len >= 0);
        MMGC_MEM_TAG( "Strings" );
        // only 8-bit strings should set the 7-bit flag
        if (w != k8)
            is7bit = false;
        Stringp s = new (gc, MMgc::kExact) String(data, w, len, is7bit);
        VERIFY_7BIT(s);
        return s;
    }

    // Create a string out of an 8bit buffer. Characters are just widened and copied, not interpreted as UTF8.

    Stringp String::createLatin1(AvmCore* core, const char* buffer, int32_t len, Width desiredWidth, bool staticBuf)
    {
        if (buffer == NULL)
        {
            len = 0;
            buffer = &k_zero.c8;
            staticBuf = true;
        }
        if (len < 0)
            len = Length(buffer);

        if (desiredWidth == kAuto)
            desiredWidth = k8;

        if (desiredWidth == k8 && core->kEmptyString != NULL)
        {
            // core has been initialized, check for cached characters
            if (len == 0)
                return core->kEmptyString;

            if (len == 1 && *((uint8_t*) buffer) < 128)
                return core->cachedChars[*((uint8_t*) buffer)];
        }

        const bool is7bit = false; // actually, might be, we just haven't checked yet.

        Stringp s = NULL;
        GC* gc = core->GetGC();
        if (staticBuf && desiredWidth == k8)
        {
            s = createStatic(gc, buffer, len, k8, is7bit);
        }
        else
        {
            s = createDynamic(gc, NULL, len, desiredWidth, is7bit);
            _copyBuffers(buffer, s->m_buffer.p8, len, k8, desiredWidth);
            VERIFY_7BIT(s);
        }
        return s;
    }

    Stringp String::getFixedWidthString(Width w) const
    {
        if (w == getWidth())
            return (Stringp) this;

        if (w == kAuto)
            return NULL;

        const bool is7bit = false;
        Stringp newStr = createDynamic(_gc(this), NULL, m_length, w, is7bit);

        Pointers ptrs(this);
        Pointers new_ptrs(newStr);
        _copyBuffers(ptrs.pv, new_ptrs.pv, m_length, getWidth(), w);

        VERIFY_7BIT(newStr);
        return newStr;
    }

/////////////////////////////// Destructors & tracers ////////////////////////////////

    String::~String()
    {
        switch (getType())
        {
            case kDynamic: {
                // Never necessary to WB() when we store NULL over a non-RC pointer
                void* pv = m_buffer.pv;
                m_buffer.pv = NULL;         // Avoid dangling the pointer
                _gc(this)->FreeNotNull(pv);
                break;
            }
            case kDependent:
                // WBRC() is however necessary when we store NULL over an RC pointer
                WBRC_NULL(&m_extra.master);
                break;
            default: ; // kStatic
        }
        m_extra.master = NULL;  // might have already been cleared above, but that's ok
        m_buffer.p8 = NULL;     // might have already been cleared above, but that's ok
        m_length = 0;
        m_bitsAndFlags = 0;
    }

    /*virtual*/
    bool String::gcTrace(GC* gc, size_t cursor)
    {
        (void)cursor;
        switch (getType())
        {
            case kDynamic:
                gc->TraceLocation(&m_buffer.pv);
                break;
            case kDependent:
                gc->TraceLocation(&m_extra.master);
                break;
        }
        return false;
    }

    /////////////////////////////// Conversions ////////////////////////////////

    void String::makeDynamic(const uint8_t* dataStart, uint32_t dataSize)
    {
        AvmAssert(dataStart != NULL);
        AvmAssert(dataSize > 0);
        switch(getType())
        {
            case kStatic:
                // do not convert if range is given and data falls within
                if ((uint32_t) (m_buffer.p8 - dataStart) >= dataSize)
                    break;
                // else fall thru
            case kDependent:
                convertToDynamic();
                break;
            default:;
        }
    }

    void String::fixDependentString()
    {
        if (!isDependent())
            return;

        // If the length of this instance is > the master length, this
        // string is the result of an auto-concat; we keep the master in
        // in that case - there is little to no memory gain to be expected.
        // Also, forget about conversion if the memory to be gained is minimal.
        if (m_length >= (m_extra.master->m_length - (TSTR_DEPENDENT_STRING_NUISANCE_SAVINGS >> getWidth())))
            return;

        // Do not convert to a static string here, because a static string
        // may be converted to a dynamic string, which would leave leave an interior
        // pointer to the static content in the string instance.
        convertToDynamic();
    }

    void String::convertToDynamic()
    {
        AvmAssert(getType() != kDynamic);
        // Convert this string to be a dynamic string
        int32_t bytes = m_length << getWidth();         // No overflow by definition
        GC* gc = _gc(this);
        MMGC_MEM_TYPE( this );
        void* buf = gc->Alloc(bytes, 0);
        VMPI_memcpy(buf, Pointers(this).pv, bytes);
        WB(gc, this, &this->m_buffer.pv, buf);
        if (isDependent())
            WBRC_NULL(&m_extra.master);
        setType(kDynamic);
    }

/////////////////////////////// Comparison /////////////////////////////////

    int32_t String::Compare(String& other, int32_t other_start, int32_t other_length) const
    {
        if (this == &other)
            return 0;

        if (other_start >= other.m_length)
            return -1;

        if (other_length < 1 || other_length > other.m_length)
            other_length = other.m_length;
        if (other_start < 0)
            other_start = 0;

        int32_t result = 0;

        int32_t len = (m_length < other_length) ? m_length : other_length;  // choose smaller of two
        if (len > 0)
        {
            Pointers ptrs(this);
            Pointers other_ptrs(&other);
            Width const w1 = this->getWidth();
            Width const w2 = other.getWidth();
            switch ((w1 << 1) + w2)
            {
                case (k8 << 1) + k8:
                    result = compareImpl(ptrs.p8, other_ptrs.p8 + other_start, len);
                    break;

                case (k8 << 1) + k16:
                    result = compareImpl(ptrs.p8, other_ptrs.p16 + other_start, len);
                    break;

                case (k16 << 1) + k8:
                    result = compareImpl(ptrs.p16, other_ptrs.p8 + other_start, len);
                    break;

                case (k16 << 1) + k16:
                    result = compareImpl(ptrs.p16, other_ptrs.p16 + other_start, len);
                    break;
            }
        }

        if (result == 0)
        {
            // catch substring compares
            result = (other_length - m_length);
            if (result < 0)
                result = -1;
            else if (result > 0)
                result = 1;
        }
        return result;
    }

    bool String::equalsLatin1(const char* p, int32_t len) const
    {
        if (len < 0)
            len = Length(p);

        if (len != length())
            return false;

        Pointers ptrs(this);
        if (getWidth() == k8)
        {
            return equalsImpl(ptrs.p8, (const uint8_t*)p, len);
        }
        else
        {
            return equalsImpl(ptrs.p16, (const uint8_t*)p, len);
        }
    }

    bool String::equalsUTF16(const wchar* p, int32_t len) const
    {
        if (len != length())
            return false;

        Pointers ptrs(this);
        if (getWidth() == k8)
        {
            return equalsImpl(ptrs.p8, p, len);
        }
        else
        {
            return equalsImpl(ptrs.p16, p, len);
        }
    }

    bool String::equals(Stringp that) const
    {
        if (this == that)
            return true;

        int32_t const len1 = this->length();
        int32_t const len2 = that->length();
        if (len1 != len2)
            return false;

        Width const w1 = this->getWidth();
        Width const w2 = that->getWidth();

        Pointers thisbuf(this);
        Pointers thatbuf(that);

        switch ((w1 << 1) + w2)
        {
            case (k8 << 1) + k8:
                return equalsImpl(thisbuf.p8, thatbuf.p8, len1);

            case (k8 << 1) + k16:
                return equalsImpl(thisbuf.p8, thatbuf.p16, len1);

            case (k16 << 1) + k8:
                return equalsImpl(thisbuf.p16, thatbuf.p8, len1);

            case (k16 << 1) + k16:
                return equalsImpl(thisbuf.p16, thatbuf.p16, len1);
        }
        AvmAssert(0);
        return true;
    }

#ifdef VMCFG_NANOJIT
    /*static*/ int32_t String::equalsWithNullChecks(Stringp s1, Stringp s2)
    {
        if (s1 == s2)
            return true;

        if (!s1 || !s2)
            return false;

        int32_t const len1 = s1->length();
        int32_t const len2 = s2->length();
        if (len1 != len2)
            return false;

        Width const w1 = s1->getWidth();
        Width const w2 = s2->getWidth();

        Pointers thisbuf(s1);
        Pointers thatbuf(s2);

        switch ((w1 << 1) + w2)
        {
            case (k8 << 1) + k8:
                return equalsImpl(thisbuf.p8, thatbuf.p8, len1);

            case (k8 << 1) + k16:
                return equalsImpl(thisbuf.p8, thatbuf.p16, len1);

            case (k16 << 1) + k8:
                return equalsImpl(thisbuf.p16, thatbuf.p8, len1);

            case (k16 << 1) + k16:
                return equalsImpl(thisbuf.p16, thatbuf.p16, len1);
        }
        AvmAssert(0);
        return true;
    }
#endif // VMCFG_NANOJIT

/////////////////////////////// Hash Codes /////////////////////////////////

    // The hashing algorithm uses the full character width

    int32_t String::hashCodeLatin1(const char* buf, int32_t len)
    {
        return hashCodeImpl((const utf8_t*)buf, len);
    }

    int32_t String::hashCodeUTF16(const wchar* buf, int32_t len)
    {
        return hashCodeImpl(buf, len);
    }

    int32_t String::hashCodeUInt(uint32_t value)
    {
        // Below is largely partial-evaluated composition of
        // MathUtils::convertIntegerToStringBuffer and
        // String::hashCodeLatin1

        int32_t radix = 10;

        int32_t hashCode = 0;

        int32_t len = stringLengthUInt(value);

        if (value == 0)
        {
            hashCode = hashCodeAddChar(hashCode, '0');
        }
        else
        {
            uintptr_t uVal = (uintptr_t)value;
            uintptr_t factor = 1;
            uintptr_t cursor = len;
            while (--cursor)
                factor *= radix;
            cursor = len;

            while (cursor > 0)
            {
                uintptr_t j = uVal;
                j /= factor;
                uVal %= factor;
                factor /= radix;
                AvmAssert( j < 10 );
                hashCode = hashCodeAddChar(hashCode, (char)(j+'0'));
                cursor--;
            }
        }

        return hashCode;
    }

    int32_t String::hashCode() const
    {
        if (m_length != 0)
        {
            Pointers ptrs(this);
            if (getWidth() == k8)
            {
                return hashCodeImpl(ptrs.p8, m_length);
            }
            else
            {
                return hashCodeImpl(ptrs.p16, m_length);
            }
        }
        return 0;
    }

//////////////////////////////// Accessors /////////////////////////////////

    wchar FASTCALL String::charAt(int32_t index) const
    {
        AvmAssert(index >= 0 && index < m_length);

        Pointers ptrs(this);
        return (getWidth() == k8) ? ptrs.p8[index] : ptrs.p16[index];
    }

    int32_t String::indexOf(Stringp substr, int32_t start) const
    {
        if (substr == NULL)
            return -1;

        // bug 78346: argv[1] might be less than zero.
        // We clamp it to zero for two reasons:
        //  1. A movie created prior to this fix with a small negative value probably worked,
        //  so let's fix it without breaking them.
        //  2. I am told this is what java does.
        if (start < 0)
            start = 0;

        int32_t len = this->length();

        if (start > len)
            start = len;

        int32_t sublen = substr->length();

        if (sublen == 0)
            return start;

        // right is the last character in selfString subStr could be found at
        // (and further, and there isn't enough of selfString remaining for a match to be possible)
        const int32_t right = len - sublen;
        if (right < 0)
            return -1;

        Width const w1 = getWidth();
        Width const w2 = substr->getWidth();

        Pointers selfBuf(this);
        Pointers subBuf(substr);

        // For maximum performance, use different cases for k8/k16 combinations
        if (sublen == 1)
        {
            switch ((w1 << 1) + w2)
            {
                case (k8 << 1) + k8:
                    return indexOfCharCodeImpl(selfBuf.p8, start, right, subBuf.p8[0]);

                case (k8 << 1) + k16:
                    return indexOfCharCodeImpl(selfBuf.p8, start, right, subBuf.p16[0]);

                case (k16 << 1) + k8:
                    return indexOfCharCodeImpl(selfBuf.p16, start, right, subBuf.p8[0]);

                case (k16 << 1) + k16:
                    return indexOfCharCodeImpl(selfBuf.p16, start, right, subBuf.p16[0]);
            }
        }
        else
        {
            switch ((w1 << 1) + w2)
            {
                case (k8 << 1) + k8:
                    return indexOfImpl(selfBuf.p8, start, right, subBuf.p8, sublen);

                case (k8 << 1) + k16:
                    return indexOfImpl(selfBuf.p8, start, right, subBuf.p16, sublen);

                case (k16 << 1) + k8:
                    return indexOfImpl(selfBuf.p16, start, right, subBuf.p8, sublen);

                case (k16 << 1) + k16:
                    return indexOfImpl(selfBuf.p16, start, right, subBuf.p16, sublen);
            }
        }
        AvmAssert(0);
        return -1;
    }

    int32_t String::lastIndexOf(Stringp substr, int32_t start) const
    {
        if (substr == NULL)
            return -1;

        // lastIndexOf("anything", negative-number) can't match anything, ever:
        // match FP10's behavior and return -1 immediately.
        if (start < 0)
            return -1;

        int32_t len = this->length();

        if (start > len)
            start = len;

        int32_t sublen = substr->length();

        if (sublen == 0)
            return start;

        // Any match starting after right must necessarily fail, as there will be
        // insufficient characters remaining to match.
        const int32_t right = len - sublen;

        // The substring is longer than selfString, and therefore no match is possible.
        // We must avoid setting start to a negative value below.
        if (right < 0)
            return -1;

        // bug 78346: argv[1] might be greater than right
        //  (similar reasons to above apply).
        if (start > right)
            start = right;

        Width w1 = getWidth();
        Width w2 = substr->getWidth();

        Pointers selfBuf(this);
        Pointers subBuf(substr);

        // For maximum performance, use different cases for k8/k16 combinations
        switch ((w1 << 1) + w2)
        {
            case (k8 << 1) + k8:
                return lastIndexOfImpl(selfBuf.p8, start, subBuf.p8, sublen);

            case (k8 << 1) + k16:
                return lastIndexOfImpl(selfBuf.p8, start, subBuf.p16, sublen);

            case (k16 << 1) + k8:
                return lastIndexOfImpl(selfBuf.p16, start, subBuf.p8, sublen);

            case (k16 << 1) + k16:
                return lastIndexOfImpl(selfBuf.p16, start, subBuf.p16, sublen);
        }
        AvmAssert(0);
        return -1;
    }

    int32_t String::indexOfLatin1(const char* p, int32_t sublen, int32_t start, int32_t end) const
    {
        if (start < 0)
            start = 0;
        if (end < 0)
            end = 0;
        if (end > m_length)
            end = m_length;

        if (p == NULL || end <= start)
            return -1;

        if (sublen < 0)
            sublen = Length(p);

        if (sublen == 0)
            return start;

        int32_t right = end - sublen;
        if (right < 0)
            return -1;

        Width w = getWidth();

        Pointers selfBuf(this);
        if (w == k8)
        {
            return indexOfImpl(selfBuf.p8, start, right, (const uint8_t*)p, sublen);
        }
        else
        {
            return indexOfImpl(selfBuf.p16, start, right, (const uint8_t*)p, sublen);
        }
    }

    int32_t String::indexOfCharCode(wchar c, int32_t start, int32_t end) const
    {
        if (start < 0)
            start = 0;
        if (end < 0)
            end = 0;
        if (end > m_length)
            end = m_length;

        if (end <= start)
            return -1;

        int32_t right = end - 1;
        if (right < 0)
            return -1;

        Pointers ptrs(this);
        if (getWidth() == String::k8)
        {
            return indexOfCharCodeImpl(ptrs.p8, start, right, c);
        }
        else
        {
            return indexOfCharCodeImpl(ptrs.p16, start, right, c);
        }
    }

    bool String::matchesLatin1(const char* p, int32_t len, int32_t pos)
    {
        if (p == NULL || pos >= m_length)
            return false;

        if (pos < 0)
            pos = 0;
        if (len < 0)
            len = Length(p);

        Pointers ptrs(this);
        if (getWidth() == k8)
        {
            return equalsImpl(ptrs.p8 + pos, (const uint8_t*)p, len);
        }
        else
        {
            return equalsImpl(ptrs.p16 + pos, (const uint8_t*)p, len);
        }
    }

    bool String::matchesLatin1_caseless(const char* p, int32_t len, int32_t pos)
    {
        if (p == NULL || pos >= m_length)
            return false;

        if (pos < 0)
            pos = 0;
        if (len < 0)
            len = Length(p);

        StringIndexer self(this);
        while (len--)
        {
            wchar const ch1 = wCharToUpper(self[pos++]);
            wchar const ch2 = wCharToUpper(uint8_t(*p++));
            if (ch1 != ch2)
                return false;
        }
        return true;
    }

///////////////////////////// Concatenation ////////////////////////////////

    /*static*/ Stringp String::concatStrings(Stringp leftStr, Stringp rightStr)
    {
        if (leftStr == NULL || leftStr->m_length == 0)
            return rightStr;

        return leftStr->_append(rightStr, Pointers(rightStr), rightStr->length(), rightStr->getWidth());
    }

    Stringp String::append(Stringp rightStr)
    {
        if (rightStr == NULL || rightStr->m_length == 0)
            return this;

        return _append(rightStr, Pointers(rightStr), rightStr->length(), rightStr->getWidth());
    }

    // NB: if rightStrPtr is nonnull, it is assumed to be the source of rightStr.
    // In this case, rightStr is assumed to be invalidate by any possible GC activity,
    // and will be re-created from rightStrPtr.
    Stringp String::_append(Stringp rightStrPtr, const Pointers& rightStr, int32_t numChars, Width charWidth)
    {
        if (numChars <= 0)
            return this;

        AvmAssert(charWidth != kAuto);

        MMgc::GC* gc = _gc(this);
        // fromCharCode() optimization: If this string is empty, and the buffer is a single
        // ASCII character, return the cached character
        if (m_length == 0 && numChars == 1)
        {
            // Sun studio generated wrong code for the following Conditional operator.
            // Add type conversion to wchar as a workaround.
            wchar ch = (charWidth == k8) ? (wchar)rightStr.p8[0] : (wchar)rightStr.p16[0];
            if (ch < 128)
                return gc->core()->cachedChars[ch];
        }

        Width thisWidth = getWidth();
        Width newWidth = (thisWidth < charWidth) ? charWidth : thisWidth;

        int32_t newLen = int32AddChecked(m_length, numChars);

        Stringp master = (isDependent()) ? m_extra.master : this;
        // check for characters left in leftStr's buffer, or if leftStr has spent its padding already
        // string types other than kDynamic have charsLeft == 0
        int32_t charsLeft = 0;
        int32_t charsUsed = 0;
        if (thisWidth >= charWidth)
        {
            // in-place append only if rightStr's width fits into leftStr
            charsLeft = master->getCharsLeft();
            if (!master->isStatic())
                // charsUsed is the number of chars already spent behind m_length
                // Usage of m_buffer is OK - master can never be dependent
                charsUsed = int32_t((GC::Size(master->m_buffer.pv) >> thisWidth) - master->m_length - charsLeft);
        }
        int32_t start = 0;  // string start for dependent strings

        // it is possible to append in-place if
        // 1) this is a kDynamic string and charsUsed == 0
        // 2) this is a kDependent string and the end matches the master's real end
        switch (getType())
        {
            case kDynamic:
                if (charsUsed != 0)
                    // someone else has already appended in-place
                    charsLeft = 0;
                break;
            case kDependent:
                start = (int32_t) m_buffer.offset_bytes >> thisWidth;
                if ((start + m_length) != master->m_length + charsUsed)
                    charsLeft = 0;
                break;
            default:;   // kStatic
        }

        // the big check: are there enough chars left?
        if (numChars <= charsLeft)
        {
            // the right-hand string fits into the buffer end
            _copyBuffers(rightStr.pv,
                         Pointers(this).p8 + (m_length << thisWidth),   // m_length << thiswidth is safe by definition
                         numChars, charWidth, newWidth);

            charsUsed += numChars;
            charsLeft -= numChars;
            master->setCharsLeft(charsLeft);
#ifdef _DEBUG
            // Terminate string with 0 for better debugging display
            if (charsLeft && !master->isStatic())
            {
                int32_t end = master->m_length + charsUsed;
                if (newWidth == k8)
                    master->m_buffer.p8[end] = 0;
                else
                    master->m_buffer.p16[end] = 0;
            }
#endif
            Stringp s = createDependent(gc, master, start, newLen);
            // createDependent propagates the 7-bit flag, which is a good idea for substrings.
            // however, if we are adding data, it may no longer qualify. rather than checking
            // the appended data, just clear the flag (it will be lazily recalculated as needed)
            s->m_bitsAndFlags &= ~TSTR_7BIT_FLAG;
            return s;
        }

        // fall thru - string does not fit
        // create a new kDynamic string containing the concatenated string
        // See the definition of TSTR_MAX_CHARSLEFT above for an explanation
        // of this algorithm
        int32_t newSize = (newLen < TSTR_MIN_DYNAMIC_ALLOCATION) ? TSTR_MIN_DYNAMIC_ALLOCATION : int32ShlChecked(newLen, 1);
        int32_t extra   = newSize - newLen;
        if (extra > (int32_t) TSTR_MAX_CHARSLEFT)
            extra = (int32_t) TSTR_MAX_CHARSLEFT;

        const bool is7bit = false; // actually, might be, we just haven't checked yet.
        Stringp newStr = createDynamic(gc, NULL, newLen, newWidth, is7bit, extra);

        // note that createDynamic has invalidated any existing Pointers structs...
        const void* srcLeft = Pointers(this).pv;
        const void* srcRight = rightStrPtr ? Pointers(rightStrPtr).pv : rightStr.pv;

        // copy leftStr
        void* ptr = _copyBuffers(srcLeft,
                      newStr->m_buffer.pv,
                      m_length,
                      thisWidth, newWidth);

        // append src
        _copyBuffers(srcRight,
                      ptr,
                      numChars,
                      charWidth, newWidth);

#ifdef _DEBUG
        // Terminate string with 0 for better debugging display
        if (newStr->getCharsLeft())
        {
            if (newWidth == k8)
                newStr->m_buffer.p8[newStr->m_length] = 0;
            else
                newStr->m_buffer.p16[newStr->m_length] = 0;
        }
#endif
        VERIFY_7BIT(newStr);
        return newStr;
    }

/////////////////////////////// substrings /////////////////////////////////

    // Remember: string runs from start up to, but not including, the end!
    Stringp String::substring(int32_t start, int32_t end)
    {
        if (start < 0)
            start = 0;
        else if (start > m_length)
            start = m_length;
        if (end < 0)
            end = 0;
        else if (end > m_length)
            end = m_length;

        // does the substring span this string?
        if (start == 0 && end == m_length)
            return this;

        MMgc::GC* gc = _gc(this);
        AvmCore* core = gc->core();
        // for empty strings, return the standard Empty String instance
        if (end <= start)
            return core->kEmptyString;

        // for single characters < 128, return the cached string
        if (end == (start + 1))
        {
            wchar const ch = charAt(start);
            if (ch < 128)
                return core->cachedChars[ch];
        }

        // otherwise, create a dependent string
        Stringp master = this;
        if (isDependent())
        {
            // get the string offset
            master = m_extra.master;
            // TODO: possible 32-bit overflow for a very huge dependent string
            int32_t offset = (int32_t) m_buffer.offset_bytes >> getWidth();
            start += (int32_t) offset;
            end   += (int32_t) offset;
        }
        AvmAssert(!master->isDependent());
        // you may be tempted to optimize for static strings, by creating
        // a new static string pointing to the substring... do not do this.
        // it will break static strings that point directly into ABC buffers.
        return createDependent(gc, master, start, end - start);
    }

    Stringp String::clone(AvmCore* targetCore)
    {
        // FIXME can this work? ensure GC is not running for the duration of this call?
        // FIXME ensure that client code interns the return value in targetCore appropriately.
        int32_t len = length();
        String::Pointers p(this);
        // hopefully _core() is not used in here
        if (getWidth() == String::k8) {
            return createLatin1(targetCore, (const char*) p.p8,  len, String::k8, false);
        } else {
            return createUTF16(targetCore, p.p16, len, String::k16, false, false);
        }
    }

    Stringp String::substr(int32_t start, int32_t len)
    {
        start = (int32_t) NativeObjectHelpers::ClampIndexInt(start, m_length);
        len = (int32_t) NativeObjectHelpers::ClampIndexInt(len, m_length);
        int32_t end = (m_length-len<=start)? m_length:start+len; // no need to further clamp
        return substring(start, end);
    }

    Stringp String::slice(int32_t start, int32_t end)
    {
        start = (int32_t) NativeObjectHelpers::ClampIndexInt(start, m_length);
        end = (int32_t) NativeObjectHelpers::ClampIndexInt(end, m_length);
        if (end < start)
            end = start;

        return substr(start, end-start);
    }

////////////////////////////////// Parsing ////////////////////////////////////

    // This routine is a very specific parser to generate a positive integer from a string.
    // The following are supported:
    // "0" - one single digit for zero - NOT "00" or any other form of zero
    //[1-9]+[0-9]* up to 2^32-2 (4294967294)
    // 2^32-1 (4294967295) is not supported (see ECMA quote below).
    // The ECMA that we're supporting with this routine is...
    // cn:  the ES3 test for a valid array index is
    //  "A property name P (in the form of a string value) is an array index if and
    //  only if ToString(ToUint32(P)) is equal to P and ToUint32(P) is not equal to 2^32-1."

    // Don't support 000000 as 0.
    // We don't support 0x1234 as 1234 in hex since string(1234) doesn't equal '0x1234')
    // No leading zeros are supported

    bool String::parseIndex(uint32_t& result) const
    {
        // avoid parsing if this string is definitely not a candidate
        // A string that is more than 10 digits (and does not start with 0)
        // will always be greater than 2^32-1 (4294967295) or not a numeric string
        if (m_bitsAndFlags & TSTR_NOUINT_FLAG)
            return false;

        if (m_bitsAndFlags & (TSTR_UINT28_FLAG | TSTR_UINT32_FLAG))
        {
            result = m_extra.index;
            return true;
        }

        int64_t n = 0;
        wchar wch;
        Pointers ptrs(this);
        Width w = getWidth();
        if (m_length == 0 || m_length > 10)
            goto bad;

        // collect the value
        for (int32_t i = 0; i < m_length; i++)
        {
            if (w == k8)
                wch = wchar(*ptrs.p8++);
            else
                wch = *ptrs.p16++;
            // bad character, or leading zero?
            if (wch < '0' || wch > '9' || (i == 1 && n == 0))
                goto bad;
            n = (n * 10) + wch - '0';
        }
        // out of range?
        if (n >= 0xFFFFFFFF)
            goto bad;
        if (n & ScriptObject::MAX_INTEGER_MASK)
            m_bitsAndFlags |= TSTR_NOINT_FLAG;
        if (!isDependent())
        {
            m_bitsAndFlags |= TSTR_UINT32_FLAG;
            m_extra.index = uint32_t(n);
        }
        result = uint32_t(n);
        return true;
    bad:
        m_bitsAndFlags |= TSTR_NOINT_FLAG | TSTR_NOUINT_FLAG;
        return false;
    }

    Atom String::getIntAtom() const
    {
        // The call uses a simplified version of parseIndex with 32-bit arithmetic,
        // since we can only fit 28 bits.
        if (m_bitsAndFlags & TSTR_NOINT_FLAG)
            return 0;

        if (m_bitsAndFlags & TSTR_UINT28_FLAG)
            return Atom((m_extra.index << 3) | AtomConstants::kIntptrType);

        int32_t n = 0;
        wchar wch;
        Pointers ptrs(this);
        Width w = getWidth();
        if (m_length == 0 || m_length > 10)
            goto bad;

        // collect the value
        for (int32_t i = 0; i < m_length; i++)
        {
            if (w == k8)
                wch = wchar(*ptrs.p8++);
            else
                wch = *ptrs.p16++;
            // bad character, or leading zero?
            if (wch < '0' || wch > '9' || (i == 1 && n == 0))
                goto bad;
            n = (n * 10) + wch - '0';
            if (n & ScriptObject::MAX_INTEGER_MASK)
                goto bad;
        }
        if (!isDependent())
        {
            m_bitsAndFlags |= TSTR_UINT28_FLAG;
            m_extra.index = n;
        }
        return Atom((n << 3) | AtomConstants::kIntptrType);
    bad:
        m_bitsAndFlags |= TSTR_NOINT_FLAG;
        return 0;
    }

/////////////////////////////// Case conversion /////////////////////////////////


    static const wchar lowerCaseBase[] =
    {
        /*0x0061,   0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
        0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074,
        0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,*/   0x00B5, /*0x00E0,   0x00E1, 0x00E2,
        0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC,
        0x00ED, 0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6,
        0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE,*/   0x00FF, /*0x0101,   0x0103,
        0x0105, 0x0107, 0x0109, 0x010B, 0x010D, 0x010F, 0x0111, 0x0113, 0x0115, 0x0117,
        0x0119, 0x011B, 0x011D, 0x011F, 0x0121, 0x0123, 0x0125, 0x0127, 0x0129, 0x012B,
        0x012D, 0x012F,*/   0x0131, 0x0133, 0x0135, 0x0137, /*0x013A,   0x013C, 0x013E, 0x0140,
        0x0142, 0x0144, 0x0146, 0x0148,*/   /*0x014B,   0x014D, 0x014F, 0x0151, 0x0153, 0x0155,
        0x0157, 0x0159, 0x015B, 0x015D, 0x015F, 0x0161, 0x0163, 0x0165, 0x0167, 0x0169,
        0x016B, 0x016D, 0x016F, 0x0171, 0x0173, 0x0175, 0x0177,*/   0x017A, 0x017C, 0x017E,
        0x017F, 0x0183, 0x0185, 0x0188, 0x018C, 0x0192, 0x0195, 0x0199, 0x01A1, 0x01A3,
        0x01A5, 0x01A8, 0x01AD, 0x01B0, 0x01B4, 0x01B6, 0x01B9, 0x01BD, 0x01BF, 0x01C5,
        0x01C6, 0x01C8, 0x01C9, 0x01CB, 0x01CC, 0x01CE, 0x01D0, 0x01D2, 0x01D4, 0x01D6,
        0x01D8, 0x01DA, 0x01DC, 0x01DD, 0x01DF, 0x01E1, 0x01E3, 0x01E5, 0x01E7, 0x01E9,
        0x01EB, 0x01ED, 0x01EF, 0x01F2, 0x01F3, 0x01F5, 0x01F9, 0x01FB, 0x01FD, 0x01FF,
        /*0x0201,   0x0203, 0x0205, 0x0207, 0x0209, 0x020B, 0x020D, 0x020F, 0x0211, 0x0213,
        0x0215, 0x0217, 0x0219, 0x021B, 0x021D, 0x021F, 0x0223, 0x0225, 0x0227, 0x0229,
        0x022B, 0x022D, 0x022F, 0x0231, 0x0233,*/   0x0253, 0x0254, 0x0256, 0x0257, 0x0259,
        0x025B, 0x0260, 0x0263, 0x0268, 0x0269, 0x026F, 0x0272, 0x0275, 0x0280, 0x0283,
        0x0288, 0x028A, 0x028B, 0x0292, 0x0345, 0x03AC, 0x03AD, 0x03AE, 0x03AF, /*0x03B1,
        0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA, 0x03BB,
        0x03BC, 0x03BD, 0x03BE, 0x03BF, 0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5,
        0x03C6, 0x03C7, 0x03C8, 0x03C9, 0x03CA, 0x03CB,*/   0x03CC, 0x03CD, 0x03CE, 0x03D0,
        0x03D1, 0x03D5, 0x03D6, /*0x03DB,   0x03DD, 0x03DF, 0x03E1, 0x03E3, 0x03E5, 0x03E7,
        0x03E9, 0x03EB, 0x03ED, 0x03EF,*/   0x03F0, 0x03F1, 0x03F2, 0x03F5, /*0x0430,   0x0431,
        0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B,
        0x043C, 0x043D, 0x043E, 0x043F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445,
        0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,*/
        /*0x0450,   0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459,
        0x045A, 0x045B, 0x045C, 0x045D, 0x045E, 0x045F,*/   /*0x0461,   0x0463, 0x0465, 0x0467,
        0x0469, 0x046B, 0x046D, 0x046F, 0x0471, 0x0473, 0x0475, 0x0477, 0x0479, 0x047B,
        0x047D, 0x047F, 0x0481, 0x048D, 0x048F, 0x0491, 0x0493, 0x0495, 0x0497, 0x0499,
        0x049B, 0x049D, 0x049F, 0x04A1, 0x04A3, 0x04A5, 0x04A7, 0x04A9, 0x04AB, 0x04AD,
        0x04AF, 0x04B1, 0x04B3, 0x04B5, 0x04B7, 0x04B9, 0x04BB, 0x04BD, 0x04BF,*/   0x04C2,
        0x04C4, 0x04C8, 0x04CC, /*0x04D1,   0x04D3, 0x04D5, 0x04D7, 0x04D9, 0x04DB, 0x04DD,
        0x04DF, 0x04E1, 0x04E3, 0x04E5, 0x04E7, 0x04E9, 0x04EB, 0x04ED, 0x04EF, 0x04F1,
        0x04F3, 0x04F5, 0x04F9, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567,
        0x0568, 0x0569, 0x056A, 0x056B, 0x056C, 0x056D, 0x056E, 0x056F, 0x0570, 0x0571,
        0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577, 0x0578, 0x0579, 0x057A, 0x057B,
        0x057C, 0x057D, 0x057E, 0x057F, 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585,
        0x0586, 0x1E01, 0x1E03, 0x1E05, 0x1E07, 0x1E09, 0x1E0B, 0x1E0D, 0x1E0F, 0x1E11,
        0x1E13, 0x1E15, 0x1E17, 0x1E19, 0x1E1B, 0x1E1D, 0x1E1F, 0x1E21, 0x1E23, 0x1E25,
        0x1E27, 0x1E29, 0x1E2B, 0x1E2D, 0x1E2F, 0x1E31, 0x1E33, 0x1E35, 0x1E37, 0x1E39,
        0x1E3B, 0x1E3D, 0x1E3F, 0x1E41, 0x1E43, 0x1E45, 0x1E47, 0x1E49, 0x1E4B, 0x1E4D,
        0x1E4F, 0x1E51, 0x1E53, 0x1E55, 0x1E57, 0x1E59, 0x1E5B, 0x1E5D, 0x1E5F, 0x1E61,
        0x1E63, 0x1E65, 0x1E67, 0x1E69, 0x1E6B, 0x1E6D, 0x1E6F, 0x1E71, 0x1E73, 0x1E75,
        0x1E77, 0x1E79, 0x1E7B, 0x1E7D, 0x1E7F, 0x1E81, 0x1E83, 0x1E85, 0x1E87, 0x1E89,
        0x1E8B, 0x1E8D, 0x1E8F, 0x1E91, 0x1E93, 0x1E95,*/   0x1E9B, /*0x1EA1,   0x1EA3, 0x1EA5,
        0x1EA7, 0x1EA9, 0x1EAB, 0x1EAD, 0x1EAF, 0x1EB1, 0x1EB3, 0x1EB5, 0x1EB7, 0x1EB9,
        0x1EBB, 0x1EBD, 0x1EBF, 0x1EC1, 0x1EC3, 0x1EC5, 0x1EC7, 0x1EC9, 0x1ECB, 0x1ECD,
        0x1ECF, 0x1ED1, 0x1ED3, 0x1ED5, 0x1ED7, 0x1ED9, 0x1EDB, 0x1EDD, 0x1EDF, 0x1EE1,
        0x1EE3, 0x1EE5, 0x1EE7, 0x1EE9, 0x1EEB, 0x1EED, 0x1EEF, 0x1EF1, 0x1EF3, 0x1EF5,
        0x1EF7, 0x1EF9,*/   /*0x1F00,   0x1F01, 0x1F02, 0x1F03, 0x1F04, 0x1F05, 0x1F06, 0x1F07,*/
        /*0x1F10,   0x1F11, 0x1F12, 0x1F13, 0x1F14, 0x1F15, 0x1F20, 0x1F21, 0x1F22, 0x1F23,
        0x1F24, 0x1F25, 0x1F26, 0x1F27, 0x1F30, 0x1F31, 0x1F32, 0x1F33, 0x1F34, 0x1F35,
        0x1F36, 0x1F37,*/   0x1F40, 0x1F41, 0x1F42, 0x1F43, 0x1F44, 0x1F45, 0x1F51, 0x1F53,
        0x1F55, 0x1F57, 0x1F60, 0x1F61, 0x1F62, 0x1F63, 0x1F64, 0x1F65, 0x1F66, 0x1F67,
        0x1F70, 0x1F71, 0x1F72, 0x1F73, 0x1F74, 0x1F75, 0x1F76, 0x1F77, 0x1F78, 0x1F79,
        0x1F7A, 0x1F7B, 0x1F7C, 0x1F7D, 0x1F80, 0x1F81, 0x1F82, 0x1F83, 0x1F84, 0x1F85,
        0x1F86, 0x1F87, 0x1F90, 0x1F91, 0x1F92, 0x1F93, 0x1F94, 0x1F95, 0x1F96, 0x1F97,
        0x1FA0, 0x1FA1, 0x1FA2, 0x1FA3, 0x1FA4, 0x1FA5, 0x1FA6, 0x1FA7, 0x1FB0, 0x1FB1,
        0x1FB3, 0x1FBE, 0x1FC3, 0x1FD0, 0x1FD1, 0x1FE0, 0x1FE1, 0x1FE5, 0x1FF3  /*0x2170,
        0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A,
        0x217B, 0x217C, 0x217D, 0x217E, 0x217F, 0x24D0, 0x24D1, 0x24D2, 0x24D3, 0x24D4,
        0x24D5, 0x24D6, 0x24D7, 0x24D8, 0x24D9, 0x24DA, 0x24DB, 0x24DC, 0x24DD, 0x24DE,
        0x24DF, 0x24E0, 0x24E1, 0x24E2, 0x24E3, 0x24E4, 0x24E5, 0x24E6, 0x24E7, 0x24E8,
        0x24E9  0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49,
        0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53,
        0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A*/
    };

    static const wchar upperCaseConversion[] =
    {
        /*0x0041,   0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A,
        0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054,
        0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A,*/   0x039C, /*0x00C0,   0x00C1, 0x00C2,
        0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC,
        0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6,
        0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE,*/   0x0178, /*0x0100,   0x0102,
        0x0104, 0x0106, 0x0108, 0x010A, 0x010C, 0x010E, 0x0110, 0x0112, 0x0114, 0x0116,
        0x0118, 0x011A, 0x011C, 0x011E, 0x0120, 0x0122, 0x0124, 0x0126, 0x0128, 0x012A,
        0x012C, 0x012E,*/   0x0049, 0x0132, 0x0134, 0x0136, /*0x0139,   0x013B, 0x013D, 0x013F,
        0x0141, 0x0143, 0x0145, 0x0147,*/   /*0x014A,   0x014C, 0x014E, 0x0150, 0x0152, 0x0154,
        0x0156, 0x0158, 0x015A, 0x015C, 0x015E, 0x0160, 0x0162, 0x0164, 0x0166, 0x0168,
        0x016A, 0x016C, 0x016E, 0x0170, 0x0172, 0x0174, 0x0176,*/   0x0179, 0x017B, 0x017D,
        0x0053, 0x0182, 0x0184, 0x0187, 0x018B, 0x0191, 0x01F6, 0x0198, 0x01A0, 0x01A2,
        0x01A4, 0x01A7, 0x01AC, 0x01AF, 0x01B3, 0x01B5, 0x01B8, 0x01BC, 0x01F7, 0x01C4,
        0x01C4, 0x01C7, 0x01C7, 0x01CA, 0x01CA, 0x01CD, 0x01CF, 0x01D1, 0x01D3, 0x01D5,
        0x01D7, 0x01D9, 0x01DB, 0x018E, 0x01DE, 0x01E0, 0x01E2, 0x01E4, 0x01E6, 0x01E8,
        0x01EA, 0x01EC, 0x01EE, 0x01F1, 0x01F1, 0x01F4, 0x01F8, 0x01FA, 0x01FC, 0x01FE,
        /*0x0200,   0x0202, 0x0204, 0x0206, 0x0208, 0x020A, 0x020C, 0x020E, 0x0210, 0x0212,
        0x0214, 0x0216, 0x0218, 0x021A, 0x021C, 0x021E, 0x0222, 0x0224, 0x0226, 0x0228,
        0x022A, 0x022C, 0x022E, 0x0230, 0x0232,*/   0x0181, 0x0186, 0x0189, 0x018A, 0x018F,
        0x0190, 0x0193, 0x0194, 0x0197, 0x0196, 0x019C, 0x019D, 0x019F, 0x01A6, 0x01A9,
        0x01AE, 0x01B1, 0x01B2, 0x01B7, 0x0399, 0x0386, 0x0388, 0x0389, 0x038A, /*0x0391,
        0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039A, 0x039B,
        0x039C, 0x039D, 0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A3, 0x03A3, 0x03A4, 0x03A5,
        0x03A6, 0x03A7, 0x03A8, 0x03A9, 0x03AA, 0x03AB,*/   0x038C, 0x038E, 0x038F, 0x0392,
        0x0398, 0x03A6, 0x03A0, /*0x03DA,   0x03DC, 0x03DE, 0x03E0, 0x03E2, 0x03E4, 0x03E6,
        0x03E8, 0x03EA, 0x03EC, 0x03EE,*/   0x039A, 0x03A1, 0x03A3, 0x0395, /*0x0410,   0x0411,
        0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B,
        0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425,
        0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,*/
        /*0x0400,   0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409,
        0x040A, 0x040B, 0x040C, 0x040D, 0x040E, 0x040F,*//*0x0460,  0x0462, 0x0464, 0x0466,
        0x0468, 0x046A, 0x046C, 0x046E, 0x0470, 0x0472, 0x0474, 0x0476, 0x0478, 0x047A,
        0x047C, 0x047E, 0x0480, 0x048C, 0x048E, 0x0490, 0x0492, 0x0494, 0x0496, 0x0498,
        0x049A, 0x049C, 0x049E, 0x04A0, 0x04A2, 0x04A4, 0x04A6, 0x04A8, 0x04AA, 0x04AC,
        0x04AE, 0x04B0, 0x04B2, 0x04B4, 0x04B6, 0x04B8, 0x04BA, 0x04BC, 0x04BE,*/   0x04C1,
        0x04C3, 0x04C7, 0x04CB, /*0x04D0,   0x04D2, 0x04D4, 0x04D6, 0x04D8, 0x04DA, 0x04DC,
        0x04DE, 0x04E0, 0x04E2, 0x04E4, 0x04E6, 0x04E8, 0x04EA, 0x04EC, 0x04EE, 0x04F0,
        0x04F2, 0x04F4, 0x04F8, 0x0531, 0x0532, 0x0533, 0x0534, 0x0535, 0x0536, 0x0537,
        0x0538, 0x0539, 0x053A, 0x053B, 0x053C, 0x053D, 0x053E, 0x053F, 0x0540, 0x0541,
        0x0542, 0x0543, 0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B,
        0x054C, 0x054D, 0x054E, 0x054F, 0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555,
        0x0556, 0x1E00, 0x1E02, 0x1E04, 0x1E06, 0x1E08, 0x1E0A, 0x1E0C, 0x1E0E, 0x1E10,
        0x1E12, 0x1E14, 0x1E16, 0x1E18, 0x1E1A, 0x1E1C, 0x1E1E, 0x1E20, 0x1E22, 0x1E24,
        0x1E26, 0x1E28, 0x1E2A, 0x1E2C, 0x1E2E, 0x1E30, 0x1E32, 0x1E34, 0x1E36, 0x1E38,
        0x1E3A, 0x1E3C, 0x1E3E, 0x1E40, 0x1E42, 0x1E44, 0x1E46, 0x1E48, 0x1E4A, 0x1E4C,
        0x1E4E, 0x1E50, 0x1E52, 0x1E54, 0x1E56, 0x1E58, 0x1E5A, 0x1E5C, 0x1E5E, 0x1E60,
        0x1E62, 0x1E64, 0x1E66, 0x1E68, 0x1E6A, 0x1E6C, 0x1E6E, 0x1E70, 0x1E72, 0x1E74,
        0x1E76, 0x1E78, 0x1E7A, 0x1E7C, 0x1E7E, 0x1E80, 0x1E82, 0x1E84, 0x1E86, 0x1E88,
        0x1E8A, 0x1E8C, 0x1E8E, 0x1E90, 0x1E92, 0x1E94,*/   0x1E60, /*0x1EA0,   0x1EA2, 0x1EA4,
        0x1EA6, 0x1EA8, 0x1EAA, 0x1EAC, 0x1EAE, 0x1EB0, 0x1EB2, 0x1EB4, 0x1EB6, 0x1EB8,
        0x1EBA, 0x1EBC, 0x1EBE, 0x1EC0, 0x1EC2, 0x1EC4, 0x1EC6, 0x1EC8, 0x1ECA, 0x1ECC,
        0x1ECE, 0x1ED0, 0x1ED2, 0x1ED4, 0x1ED6, 0x1ED8, 0x1EDA, 0x1EDC, 0x1EDE, 0x1EE0,
        0x1EE2, 0x1EE4, 0x1EE6, 0x1EE8, 0x1EEA, 0x1EEC, 0x1EEE, 0x1EF0, 0x1EF2, 0x1EF4,
        0x1EF6, 0x1EF8,*/   /*0x1F08,   0x1F09, 0x1F0A, 0x1F0B, 0x1F0C, 0x1F0D, 0x1F0E, 0x1F0F,*/
        /*0x1F18,   0x1F19, 0x1F1A, 0x1F1B, 0x1F1C, 0x1F1D, 0x1F28, 0x1F29, 0x1F2A, 0x1F2B,
        0x1F2C, 0x1F2D, 0x1F2E, 0x1F2F, 0x1F38, 0x1F39, 0x1F3A, 0x1F3B, 0x1F3C, 0x1F3D,
        0x1F3E, 0x1F3F,*/   0x1F48, 0x1F49, 0x1F4A, 0x1F4B, 0x1F4C, 0x1F4D, 0x1F59, 0x1F5B,
        0x1F5D, 0x1F5F, 0x1F68, 0x1F69, 0x1F6A, 0x1F6B, 0x1F6C, 0x1F6D, 0x1F6E, 0x1F6F,
        0x1FBA, 0x1FBB, 0x1FC8, 0x1FC9, 0x1FCA, 0x1FCB, 0x1FDA, 0x1FDB, 0x1FF8, 0x1FF9,
        0x1FEA, 0x1FEB, 0x1FFA, 0x1FFB, 0x1F88, 0x1F89, 0x1F8A, 0x1F8B, 0x1F8C, 0x1F8D,
        0x1F8E, 0x1F8F, 0x1F98, 0x1F99, 0x1F9A, 0x1F9B, 0x1F9C, 0x1F9D, 0x1F9E, 0x1F9F,
        0x1FA8, 0x1FA9, 0x1FAA, 0x1FAB, 0x1FAC, 0x1FAD, 0x1FAE, 0x1FAF, 0x1FB8, 0x1FB9,
        0x1FBC, 0x0399, 0x1FCC, 0x1FD8, 0x1FD9, 0x1FE8, 0x1FE9, 0x1FEC, 0x1FFC  /*0x2160,
        0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A,
        0x216B, 0x216C, 0x216D, 0x216E, 0x216F  0x24B6, 0x24B7, 0x24B8, 0x24B9, 0x24BA,
        0x24BB, 0x24BC, 0x24BD, 0x24BE, 0x24BF, 0x24C0, 0x24C1, 0x24C2, 0x24C3, 0x24C4,
        0x24C5, 0x24C6, 0x24C7, 0x24C8, 0x24C9, 0x24CA, 0x24CB, 0x24CC, 0x24CD, 0x24CE,
        0x24CF  0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29,
        0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33,
        0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A*/
    };


    // 12sep02 grandma : table driven inline function is 14x faster than original function,
    // Using first 100 movies of ATS, HashKey alone calls CharToUpper() 360,000 times.

    // Only entries 0-254 are used in these tables in unicharToUpper and unicharToLower
    // so entry 255 has been removed.  Entry 255 is handled by the lowerCaseBase code.

    static const uint8_t tolower_map[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0-15
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //16-31
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //32-47
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //48-63
        0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //64-79
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, //80-95
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //96-111
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //112-127
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //128-143
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //144-159
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //160-175
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //176-191
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //192-207
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, //208-223
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //224-239
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00        //240-254
    };

    static const uint8_t toupper_map[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0-15
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //16-31
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //32-47
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //48-63
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //64-79
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //80-95
        0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //96-111
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, //112-127
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //128-143
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //144-159
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //160-175
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //176-191
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //192-207
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //208-223
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, //224-239
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20        //240-254
    };


    // WARNING: This is used by the core flash code. Any change to this utility, or the tables
    //          it relies on, will break legacy Flash content.
    /*static*/ uint32_t String::unicharToUpper(uint32_t ch)
    {
        if (ch < 0xFF)
            return toupper_map[ch] ^ ch;

        // offset 0x1C60
        /*
        if ( (ch>=0x2D00 && ch<=0x2D25) )   // Georgian
        {
            return (ch - 0x1C60);
        }
        */

        // offset 80
        if ( (ch>=0x0450 && ch<=0x045F) )
        {
            return (ch - 0x50);
        }

        // offset 48
        if ( (ch>=0x0561 && ch<=0x0586) )
        {
            return (ch - 0x30);
        }

        // offset -32
        if ( (ch>=0x03B1 && ch<=0x03CB) ||
             (ch>=0x0430 && ch<=0x044F) ||
             (ch>=0xFF41 && ch<=0xFF5A) )
        {
            return (ch - 0x20);
        }

        // offset 26
        if ( (ch>=0x24D0 && ch<=0x24E9) )
        {
            return (ch - 0x1A);
        }

        // offset 16
        if ( (ch>=0x2170 && ch<=0x217F) )
        {
            return (ch - 0x10);
        }

        // offset +8(positive)
        if ( (ch>=0x1F00 && ch<=0X1F07) ||
             (ch>=0x1F10 && ch<=0x1F15) ||
             (ch>=0x1F20 && ch<=0x1F27) ||
             (ch>=0x1F30 && ch<=0x1F37) )
        {
            return (ch + 0x8);
        }

        // offset -1
        if ( (ch>=0x0101 && ch<=0x0233) )
        {
            if ( ((ch<=0x012F) && (ch&0x1)) ||                              // odd only
                 ((ch>=0x013A && ch<=0x0148) && !(ch&0x1)) ||               // even only
                 ((ch>=0x014B && ch<=0x0177) && (ch&1)) ||                  // odd only
                 ((ch>=0x0201 && ch<=0x0233) && (ch&1) && ch!=0x0221) )     // odd only
            {
                return (ch - 1);
            }
        }

        if ( ch&1 ) // only looking for odd chars here
        {
            // GREEK
            if ( ch>=0x03D9 && ch<=0x03EF )
                    return (ch - 1);
            // CYRILLIC
            if ( ((ch>=0x0461 && ch<=0x04BF) && !(ch==0x0483 || ch==0x0485 || ch==0x487 || ch==0x0489)) ||
                  (ch>=0x04D1 && ch<=0x04F9) )
            {
                    if (ch!=0x0483 && ch!=0x0485 && ch!=0x487 && ch!=0x0489)
                        return (ch - 1);
            }
            // LATIN_EXT_ADD
            if ( (ch>=0x1E01 && ch<=0x1E95) || (ch>=0x1EA1 && ch<=0x1EF9) )
                return (ch - 1);
        }


        AvmAssert(sizeof(lowerCaseBase) == sizeof(upperCaseConversion));

        uint32_t result = ch;
        // Do a binary search in lowerCaseBase for char
        int32_t lo = 0;
        int32_t hi = (sizeof(lowerCaseBase) / sizeof(lowerCaseBase[0])) - 1;

        while (lo <= hi)
        {
            int32_t pivot = (lo+hi)>>1;
            uint32_t testChar = lowerCaseBase[pivot];

            if (ch == testChar)
            {
                // Use that index into lowerCaseConversion for a return value
                result = upperCaseConversion[pivot];
                break;
            }
            else if (ch < testChar)
            {
                hi = pivot-1;
            }
            else
            {
                lo = pivot+1;
            }
        }

        return result;
    }

    static const wchar upperCaseBase[] =
    {
        /*0x0100,   0x0102, 0x0104, 0x0106,
        0x0108, 0x010A, 0x010C, 0x010E, 0x0110, 0x0112, 0x0114, 0x0116, 0x0118, 0x011A,
        0x011C, 0x011E, 0x0120, 0x0122, 0x0124, 0x0126, 0x0128, 0x012A, 0x012C, 0x012E,*/
        0x0130, 0x0132, 0x0134, 0x0136, /*0x0139,   0x013B, 0x013D, 0x013F, 0x0141, 0x0143,
        0x0145, 0x0147, 0x014A, 0x014C, 0x014E, 0x0150, 0x0152, 0x0154, 0x0156, 0x0158,
        0x015A, 0x015C, 0x015E, 0x0160, 0x0162, 0x0164, 0x0166, 0x0168, 0x016A, 0x016C,
        0x016E, 0x0170, 0x0172, 0x0174, 0x0176,*/   0x0178, 0x0179, 0x017B, 0x017D, 0x0181,
        0x0182, 0x0184, 0x0186, 0x0187, 0x0189, 0x018A, 0x018B, 0x018E, 0x018F, 0x0190,
        0x0191, 0x0193, 0x0194, 0x0196, 0x0197, 0x0198, 0x019C, 0x019D, 0x019F, 0x01A0,
        0x01A2, 0x01A4, 0x01A6, 0x01A7, 0x01A9, 0x01AC, 0x01AE, 0x01AF, 0x01B1, 0x01B2,
        0x01B3, 0x01B5, 0x01B7, 0x01B8, 0x01BC, 0x01C4, 0x01C5, 0x01C7, 0x01C8, 0x01CA,
        0x01CB, 0x01CD, 0x01CF, 0x01D1, 0x01D3, 0x01D5, 0x01D7, 0x01D9, 0x01DB, 0x01DE,
        0x01E0, 0x01E2, 0x01E4, 0x01E6, 0x01E8, 0x01EA, 0x01EC, 0x01EE, 0x01F1, 0x01F2,
        0x01F4, 0x01F6, 0x01F7, 0x01F8, 0x01FA, 0x01FC, 0x01FE, /*0x0200,   0x0202, 0x0204,
        0x0206, 0x0208, 0x020A, 0x020C, 0x020E, 0x0210, 0x0212, 0x0214, 0x0216, 0x0218,
        0x021A, 0x021C, 0x021E, 0x0222, 0x0224, 0x0226, 0x0228, 0x022A, 0x022C, 0x022E,
        0x0230, 0x0232,*/   0x0386, 0x0388, 0x0389, 0x038A, 0x038C, 0x038E, 0x038F, /*0x0391,
        0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039A, 0x039B,
        0x039C, 0x039D, 0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03A6,
        0x03A7, 0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03DA, 0x03DC, 0x03DE, 0x03E0, 0x03E2,
        0x03E4, 0x03E6, 0x03E8, 0x03EA, 0x03EC, 0x03EE,*/   0x03F4, /*0x0400,   0x0401, 0x0402,
        0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B, 0x040C,
        0x040D, 0x040E, 0x040F,*/   /*0x0410,   0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416,
        0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420,
        0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A,
        0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 0x0460, 0x0462, 0x0464, 0x0466, 0x0468,
        0x046A, 0x046C, 0x046E, 0x0470, 0x0472, 0x0474, 0x0476, 0x0478, 0x047A, 0x047C,
        0x047E, 0x0480, 0x048C, 0x048E, 0x0490, 0x0492, 0x0494, 0x0496, 0x0498, 0x049A,
        0x049C, 0x049E, 0x04A0, 0x04A2, 0x04A4, 0x04A6, 0x04A8, 0x04AA, 0x04AC, 0x04AE,
        0x04B0, 0x04B2, 0x04B4, 0x04B6, 0x04B8, 0x04BA, 0x04BC, 0x04BE,*/   0x04C1, 0x04C3,
        0x04C7, 0x04CB, /*0x04D0,   0x04D2, 0x04D4, 0x04D6, 0x04D8, 0x04DA, 0x04DC, 0x04DE,
        0x04E0, 0x04E2, 0x04E4, 0x04E6, 0x04E8, 0x04EA, 0x04EC, 0x04EE, 0x04F0, 0x04F2,
        0x04F4, 0x04F8, 0x0531, 0x0532, 0x0533, 0x0534, 0x0535, 0x0536, 0x0537, 0x0538,
        0x0539, 0x053A, 0x053B, 0x053C, 0x053D, 0x053E, 0x053F, 0x0540, 0x0541, 0x0542,
        0x0543, 0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B, 0x054C,
        0x054D, 0x054E, 0x054F, 0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555, 0x0556,*/
        /*
        // cn: added Georgian.  Not in the UnicodeData-3.2.0 spreadsheet, but was added later
        0x10a0, 0x10a1, 0x10a2, 0x10a3, 0x10a4, 0x10a5, 0x10a6, 0x10a7, 0x10a8, 0x10a9,
        0x10aa, 0x10ab, 0x10ac, 0x10ad, 0x10ae, 0x10af, 0x10b0, 0x10b1, 0x10b2, 0x10b3,
        0x10b4, 0x10b5, 0x10b6, 0x10b7, 0x10b8, 0x10b9, 0x10ba, 0x10bb, 0x10bc, 0x10bd,
        0x10be, 0x10bf, 0x10c0, 0x10c1, 0x10c2, 0x10c3, 0x10c4, 0x10c5,
        // cn: end Georgian.
        */
        /*0x1E00,   0x1E02, 0x1E04, 0x1E06, 0x1E08, 0x1E0A, 0x1E0C, 0x1E0E, 0x1E10, 0x1E12,
        0x1E14, 0x1E16, 0x1E18, 0x1E1A, 0x1E1C, 0x1E1E, 0x1E20, 0x1E22, 0x1E24, 0x1E26,
        0x1E28, 0x1E2A, 0x1E2C, 0x1E2E, 0x1E30, 0x1E32, 0x1E34, 0x1E36, 0x1E38, 0x1E3A,
        0x1E3C, 0x1E3E, 0x1E40, 0x1E42, 0x1E44, 0x1E46, 0x1E48, 0x1E4A, 0x1E4C, 0x1E4E,
        0x1E50, 0x1E52, 0x1E54, 0x1E56, 0x1E58, 0x1E5A, 0x1E5C, 0x1E5E, 0x1E60, 0x1E62,
        0x1E64, 0x1E66, 0x1E68, 0x1E6A, 0x1E6C, 0x1E6E, 0x1E70, 0x1E72, 0x1E74, 0x1E76,
        0x1E78, 0x1E7A, 0x1E7C, 0x1E7E, 0x1E80, 0x1E82, 0x1E84, 0x1E86, 0x1E88, 0x1E8A,
        0x1E8C, 0x1E8E, 0x1E90, 0x1E92, 0x1E94, 0x1EA0, 0x1EA2, 0x1EA4, 0x1EA6, 0x1EA8,
        0x1EAA, 0x1EAC, 0x1EAE, 0x1EB0, 0x1EB2, 0x1EB4, 0x1EB6, 0x1EB8, 0x1EBA, 0x1EBC,
        0x1EBE, 0x1EC0, 0x1EC2, 0x1EC4, 0x1EC6, 0x1EC8, 0x1ECA, 0x1ECC, 0x1ECE, 0x1ED0,
        0x1ED2, 0x1ED4, 0x1ED6, 0x1ED8, 0x1EDA, 0x1EDC, 0x1EDE, 0x1EE0, 0x1EE2, 0x1EE4,
        0x1EE6, 0x1EE8, 0x1EEA, 0x1EEC, 0x1EEE, 0x1EF0, 0x1EF2, 0x1EF4, 0x1EF6, 0x1EF8,
        0x1F08, 0x1F09, 0x1F0A, 0x1F0B, 0x1F0C, 0x1F0D, 0x1F0E, 0x1F0F, 0x1F18, 0x1F19,
        0x1F1A, 0x1F1B, 0x1F1C, 0x1F1D, 0x1F28, 0x1F29, 0x1F2A, 0x1F2B, 0x1F2C, 0x1F2D,
        0x1F2E, 0x1F2F, 0x1F38, 0x1F39, 0x1F3A, 0x1F3B, 0x1F3C, 0x1F3D, 0x1F3E, 0x1F3F,*/
        0x1F48, 0x1F49, 0x1F4A, 0x1F4B, 0x1F4C, 0x1F4D, 0x1F59, 0x1F5B, 0x1F5D, 0x1F5F,
        0x1F68, 0x1F69, 0x1F6A, 0x1F6B, 0x1F6C, 0x1F6D, 0x1F6E, 0x1F6F, 0x1F88, 0x1F89,
        0x1F8A, 0x1F8B, 0x1F8C, 0x1F8D, 0x1F8E, 0x1F8F, 0x1F98, 0x1F99, 0x1F9A, 0x1F9B,
        0x1F9C, 0x1F9D, 0x1F9E, 0x1F9F, 0x1FA8, 0x1FA9, 0x1FAA, 0x1FAB, 0x1FAC, 0x1FAD,
        0x1FAE, 0x1FAF, 0x1FB8, 0x1FB9, 0x1FBA, 0x1FBB, 0x1FBC, 0x1FC8, 0x1FC9, 0x1FCA,
        0x1FCB, 0x1FCC, 0x1FD8, 0x1FD9, 0x1FDA, 0x1FDB, 0x1FE8, 0x1FE9, 0x1FEA, 0x1FEB,
        0x1FEC, 0x1FF8, 0x1FF9, 0x1FFA, 0x1FFB, 0x1FFC, 0x2126, 0x212A, 0x212B  /*0x2160,
        0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A,
        0x216B, 0x216C, 0x216D, 0x216E, 0x216F, 0x24B6, 0x24B7, 0x24B8, 0x24B9, 0x24BA,
        0x24BB, 0x24BC, 0x24BD, 0x24BE, 0x24BF, 0x24C0, 0x24C1, 0x24C2, 0x24C3, 0x24C4,
        0x24C5, 0x24C6, 0x24C7, 0x24C8, 0x24C9, 0x24CA, 0x24CB, 0x24CC, 0x24CD, 0x24CE,
        0x24CF, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29,
        0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 0xFF30, 0xFF31, 0xFF32, 0xFF33,
        0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A*/
    };

    static const wchar lowerCaseConversion[] =
    {
        /*0x0101,   0x0103, 0x0105, 0x0107,
        0x0109, 0x010B, 0x010D, 0x010F, 0x0111, 0x0113, 0x0115, 0x0117, 0x0119, 0x011B,
        0x011D, 0x011F, 0x0121, 0x0123, 0x0125, 0x0127, 0x0129, 0x012B, 0x012D, 0x012F,*/
        0x0069, 0x0133, 0x0135, 0x0137, /*0x013A,   0x013C, 0x013E, 0x0140, 0x0142, 0x0144,
        0x0146, 0x0148, 0x014B, 0x014D, 0x014F, 0x0151, 0x0153, 0x0155, 0x0157, 0x0159,
        0x015B, 0x015D, 0x015F, 0x0161, 0x0163, 0x0165, 0x0167, 0x0169, 0x016B, 0x016D,
        0x016F, 0x0171, 0x0173, 0x0175, 0x0177,*/   0x00FF, 0x017A, 0x017C, 0x017E, 0x0253,
        0x0183, 0x0185, 0x0254, 0x0188, 0x0256, 0x0257, 0x018C, 0x01DD, 0x0259, 0x025B,
        0x0192, 0x0260, 0x0263, 0x0269, 0x0268, 0x0199, 0x026F, 0x0272, 0x0275, 0x01A1,
        0x01A3, 0x01A5, 0x0280, 0x01A8, 0x0283, 0x01AD, 0x0288, 0x01B0, 0x028A, 0x028B,
        0x01B4, 0x01B6, 0x0292, 0x01B9, 0x01BD, 0x01C6, 0x01C6, 0x01C9, 0x01C9, 0x01CC,
        0x01CC, 0x01CE, 0x01D0, 0x01D2, 0x01D4, 0x01D6, 0x01D8, 0x01DA, 0x01DC, 0x01DF,
        0x01E1, 0x01E3, 0x01E5, 0x01E7, 0x01E9, 0x01EB, 0x01ED, 0x01EF, 0x01F3, 0x01F3,
        0x01F5, 0x0195, 0x01BF, 0x01F9, 0x01FB, 0x01FD, 0x01FF, /*0x0201,   0x0203, 0x0205,
        0x0207, 0x0209, 0x020B, 0x020D, 0x020F, 0x0211, 0x0213, 0x0215, 0x0217, 0x0219,
        0x021B, 0x021D, 0x021F, 0x0223, 0x0225, 0x0227, 0x0229, 0x022B, 0x022D, 0x022F,
        0x0231, 0x0233,*/   0x03AC, 0x03AD, 0x03AE, 0x03AF, 0x03CC, 0x03CD, 0x03CE, /*0x03B1,
        0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA, 0x03BB,
        0x03BC, 0x03BD, 0x03BE, 0x03BF, 0x03C0, 0x03C1, 0x03C3, 0x03C4, 0x03C5, 0x03C6,
        0x03C7, 0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03DB, 0x03DD, 0x03DF, 0x03E1, 0x03E3,
        0x03E5, 0x03E7, 0x03E9, 0x03EB, 0x03ED, 0x03EF,*/   0x03B8, /*0x0450,   0x0451, 0x0452,
        0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C,
        0x045D, 0x045E, 0x045F,*/   /*0x0430,   0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436,
        0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 0x0440,
        0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A,
        0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x0461, 0x0463, 0x0465, 0x0467, 0x0469,
        0x046B, 0x046D, 0x046F, 0x0471, 0x0473, 0x0475, 0x0477, 0x0479, 0x047B, 0x047D,
        0x047F, 0x0481, 0x048D, 0x048F, 0x0491, 0x0493, 0x0495, 0x0497, 0x0499, 0x049B,
        0x049D, 0x049F, 0x04A1, 0x04A3, 0x04A5, 0x04A7, 0x04A9, 0x04AB, 0x04AD, 0x04AF,
        0x04B1, 0x04B3, 0x04B5, 0x04B7, 0x04B9, 0x04BB, 0x04BD, 0x04BF,*/   0x04C2, 0x04C4,
        0x04C8, 0x04CC, /*0x04D1,   0x04D3, 0x04D5, 0x04D7, 0x04D9, 0x04DB, 0x04DD, 0x04DF,
        0x04E1, 0x04E3, 0x04E5, 0x04E7, 0x04E9, 0x04EB, 0x04ED, 0x04EF, 0x04F1, 0x04F3,
        0x04F5, 0x04F9, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567, 0x0568,
        0x0569, 0x056A, 0x056B, 0x056C, 0x056D, 0x056E, 0x056F, 0x0570, 0x0571, 0x0572,
        0x0573, 0x0574, 0x0575, 0x0576, 0x0577, 0x0578, 0x0579, 0x057A, 0x057B, 0x057C,
        0x057D, 0x057E, 0x057F, 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586,*/
        // cn: added Georgian.  Not in the UnicodeData-3.2.0 spreadsheet, but was added later
        /*
        0x10d0, 0x10d1, 0x10d2, 0x10d3, 0x10d4, 0x10d5, 0x10d6, 0x10d7, 0x10d8, 0x10d9,
        0x10da, 0x10db, 0x10dc, 0x10dd, 0x10de, 0x10df, 0x10e0, 0x10e1, 0x10e2, 0x10e3,
        0x10e4, 0x10e5, 0x10e6, 0x10e7, 0x10e8, 0x10e9, 0x10ea, 0x10eb, 0x10ec, 0x10ed,
        0x10ee, 0x10ef, 0x10f0, 0x10f1, 0x10f2, 0x10f3, 0x10f4, 0x10f5,
        // cn: end Georgian.
        */
        /*0x1E01,   0x1E03, 0x1E05, 0x1E07, 0x1E09, 0x1E0B, 0x1E0D, 0x1E0F, 0x1E11, 0x1E13,
        0x1E15, 0x1E17, 0x1E19, 0x1E1B, 0x1E1D, 0x1E1F, 0x1E21, 0x1E23, 0x1E25, 0x1E27,
        0x1E29, 0x1E2B, 0x1E2D, 0x1E2F, 0x1E31, 0x1E33, 0x1E35, 0x1E37, 0x1E39, 0x1E3B,
        0x1E3D, 0x1E3F, 0x1E41, 0x1E43, 0x1E45, 0x1E47, 0x1E49, 0x1E4B, 0x1E4D, 0x1E4F,
        0x1E51, 0x1E53, 0x1E55, 0x1E57, 0x1E59, 0x1E5B, 0x1E5D, 0x1E5F, 0x1E61, 0x1E63,
        0x1E65, 0x1E67, 0x1E69, 0x1E6B, 0x1E6D, 0x1E6F, 0x1E71, 0x1E73, 0x1E75, 0x1E77,
        0x1E79, 0x1E7B, 0x1E7D, 0x1E7F, 0x1E81, 0x1E83, 0x1E85, 0x1E87, 0x1E89, 0x1E8B,
        0x1E8D, 0x1E8F, 0x1E91, 0x1E93, 0x1E95, 0x1EA1, 0x1EA3, 0x1EA5, 0x1EA7, 0x1EA9,
        0x1EAB, 0x1EAD, 0x1EAF, 0x1EB1, 0x1EB3, 0x1EB5, 0x1EB7, 0x1EB9, 0x1EBB, 0x1EBD,
        0x1EBF, 0x1EC1, 0x1EC3, 0x1EC5, 0x1EC7, 0x1EC9, 0x1ECB, 0x1ECD, 0x1ECF, 0x1ED1,
        0x1ED3, 0x1ED5, 0x1ED7, 0x1ED9, 0x1EDB, 0x1EDD, 0x1EDF, 0x1EE1, 0x1EE3, 0x1EE5,
        0x1EE7, 0x1EE9, 0x1EEB, 0x1EED, 0x1EEF, 0x1EF1, 0x1EF3, 0x1EF5, 0x1EF7, 0x1EF9,
        0x1F00, 0x1F01, 0x1F02, 0x1F03, 0x1F04, 0x1F05, 0x1F06, 0x1F07, 0x1F10, 0x1F11,
        0x1F12, 0x1F13, 0x1F14, 0x1F15, 0x1F20, 0x1F21, 0x1F22, 0x1F23, 0x1F24, 0x1F25,
        0x1F26, 0x1F27, 0x1F30, 0x1F31, 0x1F32, 0x1F33, 0x1F34, 0x1F35, 0x1F36, 0x1F37,*/
        0x1F40, 0x1F41, 0x1F42, 0x1F43, 0x1F44, 0x1F45, 0x1F51, 0x1F53, 0x1F55, 0x1F57,
        0x1F60, 0x1F61, 0x1F62, 0x1F63, 0x1F64, 0x1F65, 0x1F66, 0x1F67, 0x1F80, 0x1F81,
        0x1F82, 0x1F83, 0x1F84, 0x1F85, 0x1F86, 0x1F87, 0x1F90, 0x1F91, 0x1F92, 0x1F93,
        0x1F94, 0x1F95, 0x1F96, 0x1F97, 0x1FA0, 0x1FA1, 0x1FA2, 0x1FA3, 0x1FA4, 0x1FA5,
        0x1FA6, 0x1FA7, 0x1FB0, 0x1FB1, 0x1F70, 0x1F71, 0x1FB3, 0x1F72, 0x1F73, 0x1F74,
        0x1F75, 0x1FC3, 0x1FD0, 0x1FD1, 0x1F76, 0x1F77, 0x1FE0, 0x1FE1, 0x1F7A, 0x1F7B,
        0x1FE5, 0x1F78, 0x1F79, 0x1F7C, 0x1F7D, 0x1FF3, 0x03C9, 0x006B, 0x00E5 /*0x2170,
        0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A,
        0x217B, 0x217C, 0x217D, 0x217E, 0x217F, 0x24D0, 0x24D1, 0x24D2, 0x24D3, 0x24D4,
        0x24D5, 0x24D6, 0x24D7, 0x24D8, 0x24D9, 0x24DA, 0x24DB, 0x24DC, 0x24DD, 0x24DE,
        0x24DF, 0x24E0, 0x24E1, 0x24E2, 0x24E3, 0x24E4, 0x24E5, 0x24E6, 0x24E7, 0x24E8,
        0x24E9, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49,
        0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 0xFF50, 0xFF51, 0xFF52, 0xFF53,
        0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A*/
    };


    // WARNING: This is used by the core flash code. Any change to this utility, or the tables
    //          it relies on, will break legacy Flash content.
    /*static*/ uint32_t String::unicharToLower(uint32_t ch)
    {
        if (ch < 0xFF)
            return tolower_map[ch] ^ ch;

        // offset x1C60
        if( ch>=0x10A0 && ch<=0x10C5 )  // Georgian
        {
            return (ch + 48); // WRONG
            //return (ch + 0x1C60); // CORRECT
        }

        // offset 80
        if ( (ch>=0x0400 && ch<=0x040F) )
        {
            return (ch + 0x50);
        }

        // offset 48
        if ( (ch>=0x0531 && ch<=0x0556) )
        {
            return (ch + 0x30);
        }

        // offset 32
        if ( (ch>=0x0391 && ch<=0x03AB) ||
             (ch>=0x0410 && ch<=0x042F) ||
             (ch>=0xFF21 && ch<=0xFF3A) )
        {
            return (ch + 0x20);
        }

        // offset 26
        if ( (ch>=0x24B6 && ch<=0x24CF) )
        {
            return (ch + 0x1A);
        }

        // offset 16
        if ( (ch>=0x2160 && ch<=0x216F) )
        {
            return (ch + 0x10);
        }

        // offset -8(negative)
        if ( (ch>=0x1F08 && ch<=0X1F0F) ||
             (ch>=0x1F18 && ch<=0x1F1D) ||
             (ch>=0x1F28 && ch<=0x1F2F) ||
             (ch>=0x1F38 && ch<=0x1F3F) )
        {
            return (ch - 0x8);
        }

        // offset +1
        if ( (ch>=0x0100 && ch<=0x0232) )
        {
            // only even numbers
            if ( ((ch<=0x012E) && !(ch&0x1)) ||
                 ((ch>=0x0139 && ch<=0x0147) && (ch&0x1)) ||
                 ((ch>=0x014A && ch<=0x0176) && !(ch&1)) ||
                 ((ch>=0x0200 && ch<=0x0232) && !(ch&1) && (ch!=0x0220)) )
            {
                return (ch + 1);
            }
        }

        if ( !(ch&1) ) // only looking for even chars
        {
            if ( (ch>=0x03D8 && ch<=0x03EE) )
                    return (ch + 1);
            if ( ((ch>=0x0460 && ch<=0x04BE) && !(ch==0x0482 || ch==0x0484 || ch==0x486 || ch==0x0488)) ||
                  (ch>=0x04D0 && ch<=0x04F8) )
            {
                    if (ch!=0x0482 && ch!=0x0484 && ch!=0x486 && ch!=0x0488)
                        return (ch + 1);
            }
            if ( (ch>=0x1E00 && ch<=0x1E94) || (ch>=0x1EA0 && ch<=0x1EF8) )
                return (ch + 1);
        }


        AvmAssert(sizeof(upperCaseBase) == sizeof(lowerCaseConversion));

        uint32_t result = ch;
        // Do a binary search in upperCaseBase for char
        int32_t lo = 0;
        int32_t hi = (sizeof(upperCaseBase) / sizeof(upperCaseBase[0])) - 1;

        while (lo <= hi)
        {
            int32_t pivot = (lo+hi)>>1;
            uint32_t testChar = upperCaseBase[pivot];

            if (ch == testChar)
            {
                // Use that index into lowerCaseConversion for a return value
                result =  lowerCaseConversion[pivot];
                break;
            }
            else if (ch < testChar)
            {
                hi = pivot-1;
            }
            else
            {
                lo = pivot+1;
            }
        }

        return result;
    }

    Stringp String::toUpperCase()
    {
        return caseChange(unicharToUpper);
    }

    Stringp String::toLowerCase()
    {
        return caseChange(unicharToLower);
    }

    Stringp String::caseChange(uint32_t(*unimapper)(uint32_t))
    {
        if (!this->length())
            return this;

        // Flag to detect whether any changes were made
        bool changed = false;

        // 0xFF is a special case: ToUpper(0xFF) == 0x178, so we need a wider string
        // if the string contains 0xFF
        Width w = getWidth();
        if (w == k8 && VMPI_memchr(Pointers(this).p8, 0xFF, m_length) != 0)
            w = k16;

        GC* gc = _gc(this);
        const bool is7bit = false;
        Stringp newStr = createDynamic(gc, NULL, m_length, w, is7bit);

        Pointers src(this); // can't re-use the Pointers from VMPI_memchr; createDynamic may have invalidated it
        Pointers dst(newStr);

        int32_t i;
        uint32_t ch1, ch2;
        if (getWidth() == k8)
        {
            for (i = 0; i < m_length; i++)
            {
                ch1 = uint32_t(*src.p8++);
                ch2 = unimapper (ch1);
                if (w == k16)
                    *dst.p16++ = wchar(ch2);
                else
                    *dst.p8++ = uint8_t(ch2);
                if (ch1 != ch2)
                    changed = true;
            }
        }
        else
        {
            for (i = 0; i < m_length; i++)
            {
                ch1 = uint32_t(*src.p16++);
                ch2 = unimapper(ch1);
                *dst.p16++ = wchar(ch2);
                if (ch1 != ch2)
                    changed = true;
            }
        }
        VERIFY_7BIT(this);
        VERIFY_7BIT(newStr);
        return changed ? newStr : this;
    }

    double String::toNumber()
    {
        return MathUtils::convertStringToNumber(this);
    }

    bool String::isWhitespace() const
    {
        StringIndexer self((Stringp) this);
        for (int32_t i = 0 ; i < length(); i++)
        {
            wchar ch = self[i];
            if (!isSpace(ch))
                return false;
        }
        return true;
    }

    /*static*/ int32_t FASTCALL String::Length(const wchar* str)
    {
        AvmAssert(str != NULL);

        const wchar* s = str;
        while (*s)
            s++;
        ptrdiff_t len = s - str;
        if (len <= 0x7FFFFFFF)
            return int32_t(len);
        GCHeap::SignalObjectTooLarge();
        /*NOTREACHED*/
        return 0;
    }

    /*static*/ int32_t FASTCALL String::Length(const char* str)
    {
        AvmAssert(str != NULL);

        size_t len = VMPI_strlen(str);
        if (len <= 0x7FFFFFFF)
            return int32_t(len);
        GCHeap::SignalObjectTooLarge();
        /*NOTREACHED*/
        return 0;
    }

    int String::_indexOf(Stringp substr, int startPos)
    {
        return (int) indexOf(substr, (int32_t) startPos);
    }

    // Coded as a macro for best performance with MSVC 2008 32-bit
    // -1 (or negative infinity) - position equals -1 (out of bounds)
    // >length (or positive infinity) - position equals length
    // non-NaN - integer cast to determine position
    // NaN - use NaNValue (usually zero but length for lastIndexOf)
    #define INTCLAMP(iPos, dPos, length, NaNValue) \
    { \
        if (dPos <= -1)  \
            iPos = -1; \
        else if (dPos >= length) \
            iPos = length; \
        else if (!MathUtils::isNaNInline(dPos)) { \
            iPos = (int32_t) dPos; \
        } \
        else { \
            iPos = NaNValue; \
        } \
    }

    int32_t String::AS3_indexOf(Stringp substr, double dStartPos)
    {
        int32_t iPos;
        INTCLAMP(iPos, dStartPos, this->length(), 0);
        return indexOf(substr, iPos);
    }

    int String::_lastIndexOf(Stringp substr, int iStartPos)
    {
        return (int) lastIndexOf(substr, (int32_t) iStartPos);
    }

    int32_t String::AS3_lastIndexOf(Stringp substr, double dStartPos)
    {
        // unlike most other calls, this one has nan->length rather than nan->0
        int32_t iStartPos;
        INTCLAMP(iStartPos, dStartPos, this->length(), this->length());
        return lastIndexOf(substr, iStartPos);
    }

    Stringp String::_substr(int start, int count)
    {
        int len = this->length();
        start = (int)NativeObjectHelpers::ClampIndexInt(start, len);
        // ClampIndex takes a double (not int or uint) for first parm...
        // we must cast these to double before addition, otherwise we
        // can have numeric overflow with the default arg (end=0x7fffffff)
        // and wrap to negative, which would be bad...

        // Do some sanity checks on our ints to see if they will fall within a valid integer range
        // !!@what about negative values?
        int end;
        if (count == 0x7fffffff)    // largest positive int value, also the default arg
        {
            end = len;
        }
        else if ((count > 0x3fffffff) || (start > 0x3fffffff)) // might overflow - use doubles
        {
            end = (int)NativeObjectHelpers::ClampIndex(double(count) + double(start), len);
        }
        else
        {
            end = (int)NativeObjectHelpers::ClampIndexInt(count + start, len);
        }

        if (end < start)
            end = start;

        return substr(start, end-start);
    }

    Stringp String::AS3_substr(double d_start, double d_count)
    {
        int32_t len = this->length();
        int32_t start = (int32_t) NativeObjectHelpers::ClampIndex(MathUtils::toInt(d_start), len);
        // ClampIndex takes a double (not int or uint) for first parm...
        // we must cast these to double before addition, otherwise we
        // can have numeric overflow with the default arg (end=0x7fffffff)
        // and wrap to negative, which would be bad...
        int32_t end = (int32_t)NativeObjectHelpers::ClampIndex(MathUtils::toInt(d_count) + (double)start, len);
        if (end < start)
            end = start;

        return substr(start, end-start);
    }

    Stringp String::_substring(int start, int end)
    {
        NativeObjectHelpers::ClampBInt(start, end, this->length());
        return substring((int32_t) start, (int32_t) end);
    }

    Stringp String::AS3_substring(double d_start, double d_end)
    {
        double start = MathUtils::toInt(d_start);
        double end = MathUtils::toInt(d_end);
        NativeObjectHelpers::ClampB(start, end, length());
        return substring((int32_t) start, (int32_t) end);
    }

    Stringp String::_slice(int start, int end)
    {
        return slice((int32_t) start, (int32_t) end);
    }

    Stringp String::AS3_slice(double d_start, double d_end)
    {
        int32_t len = this->length();
        int32_t start = (int32_t)NativeObjectHelpers::ClampIndex(MathUtils::toInt(d_start), len);
        int32_t end = (int32_t)NativeObjectHelpers::ClampIndex(MathUtils::toInt(d_end), len);
        if (end < start)
            end = start;

        return substring(start, end);
    }

    Stringp String::_charAtI(int32_t iPos)
    {
        AvmCore* core = _core(this);
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            wchar const ch = (wchar) (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];

            // newStringUTF16 does the cachedChar optimization internally, but short-circuiting the test
            // here is worthwhile because it's a pretty common case
            if (ch < 128)
                return core->cachedChars[ch];
            return core->newStringUTF16(&ch, 1);
        }
        else {
            return core->kEmptyString;
        }
    }

    Stringp String::_charAtU(uint32_t iPos)
    {
        AvmCore* core = _core(this);
        if (iPos < uint32_t(m_length)) {
            Pointers ptrs(this);
            wchar const ch = (wchar) (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];

            // newStringUTF16 does the cachedChar optimization internally, but short-circuiting the test
            // here is worthwhile because it's a pretty common case
            if (ch < 128)
                return core->cachedChars[ch];
            return core->newStringUTF16(&ch, 1);
        }
        else {
            return core->kEmptyString;
        }
    }

    Stringp String::AS3_charAt(double dPos)
    {
        int32_t iPos;
        INTCLAMP(iPos, dPos, this->length(), 0);

        AvmCore* core = _core(this);
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            wchar const ch = (wchar) (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];

            // newStringUTF16 does the cachedChar optimization internally, but short-circuiting the test
            // here is worthwhile because it's a pretty common case
            if (ch < 128)
                return core->cachedChars[ch];
            return core->newStringUTF16(&ch, 1);
        }
        else {
            return core->kEmptyString;
        }
    }

    Stringp String::AS3_toUpperCase()
    {
        return toUpperCase();
    }

    Stringp String::AS3_toLowerCase()
    {
        return toLowerCase();
    }

    double String::_charCodeAtDI(int32_t iPos)
    {
        double d;
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            d = (double) ((getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos]);
        }
        else
            d = MathUtils::kNaN;
        return d;
    }

    int32_t String::_charCodeAtII(int32_t iPos)
    {
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            return (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];
        }
        else
            return 0;
    }

    double String::_charCodeAtDU(uint32_t iPos)
    {
        double d;
        if (iPos < uint32_t(m_length)) {
            Pointers ptrs(this);
            d = (double) ((getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos]);
        }
        else
            d = MathUtils::kNaN;
        return d;
    }

    int32_t String::_charCodeAtIU(uint32_t iPos)
    {
        if (iPos < uint32_t(m_length)) {
            Pointers ptrs(this);
            return (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];
        }
        else
            return 0;
    }

    double String::AS3_charCodeAt(double dPos)
    {
        int32_t iPos;
        INTCLAMP(iPos, dPos, m_length, 0);
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            return (double) ((getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos]);
        }
        else
            return MathUtils::kNaN;
    }

    int32_t String::_charCodeAtID(double dPos)
    {
        int32_t iPos;
        INTCLAMP(iPos, dPos, m_length, 0);
        // use unsigned compare as faster equivalent to >= 0 && < m_length
        if (uint32_t(iPos) < uint32_t(m_length)) {
            Pointers ptrs(this);
            return (getWidth() == k8) ? ptrs.p8[iPos] : ptrs.p16[iPos];
        }
        else
            return 0;
    }

    int32_t String::AS3_localeCompare(Atom other)
    {
        Stringp otherStr;

        if (AvmCore::isString(other))
        {
            otherStr = AvmCore::atomToString(other);
        }
        else
        {
            AvmCore* core = _core(this);

            // legacy behavior special-cases NULL (including undefined).
            if (AvmCore::isNullOrUndefined(other))
            {
                if (!core->currentBugCompatibility()->bugzilla585791)
                    return (m_length == 0) ? 1 : 0;
            }

            // Proper ECMAScript behavior:
            // The 'this' object is converted to string, call it a.
            // The argument is converted to string, call it b.
            // Those strings are compared and a string comparison value is returned.
            otherStr = core->string(other);
        }

        return otherStr->Compare(*this);
    }

#ifdef DEBUGGER
    uint64_t String::bytesUsed() const
    {
        uint64_t bytesUsed = sizeof(String);

        // If getType() == kDependent, the buffer's memory is account for by
        // the string upon which this string depends, so we should not include
        // it here.
        //
        // If getType() == kStatic, the buffer's memory is not on the GC heap,
        // and freeing the string would not free the buffer, so we should not
        // include its size here.
        if (getType() == kDynamic)
            bytesUsed += GC::Size(m_buffer.pv);

        return bytesUsed;
    }

    Stringp String::getMasterString() const
    {
        if (isDependent())
            return m_extra.master;
        else
            return NULL;
    }
#endif

////////////////////////////// Helpers: Width Analysis /////////////////////////////////

    // The character widths structure used by the _analyze() functions below

    struct StringWidths
    {
        int32_t ascii, w8, w16, w32;
    };

    // Analyze a UTF-8 string buffer and find out about the character widths.
    // Return false if the string contains malformed or too big sequences,
    // and strict mode is requested.

    // RFC 2279

    //   0000 0000-0000 007F   0xxxxxxx
    //   0000 0080-0000 07FF   110xxxxx 10xxxxxx
    //   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

    //   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //   0400 0000-7FFF FFFF   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    static bool _analyzeUtf8(const utf8_t* in, int32_t inLen, StringWidths& r, bool strict)
    {
        r.ascii = r.w8 = r.w16 = r.w32 = 0;
        while (inLen > 0)
        {
            uint32_t c = uint32_t (*in);

            switch (c >> 4)
            {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    r.w8++;
                    r.ascii++;
                    in++;
                    inLen--;
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
                    c = (((c<<6) & 0x7C0) | (in[1] & 0x3F));
                    if (c < 0x80) {
                        // Overlong sequence, reject as invalid.
                        goto invalid;
                    }
                    in += 2;
                    inLen -= 2;
                    if (c < 0xFF)
                        r.w8++;
                    else
                        r.w16++;
                    break;

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
                    c = (((c<<12) & 0xF000) | ((in[1]<<6) & 0xFC0) | (in[2] & 0x3F));
                    if (c < 0x800) {
                        // Overlong sequence, reject as invalid.
                        goto invalid;
                    }
                    in += 3;
                    inLen -= 3;
                    r.w16++;
                    break;

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

                    c = (((c<<18) & 0x1C0000) |
                         ((in[1]<<12) & 0x3F000) |
                         ((in[2]<<6)  & 0xFC0) |
                         (in[3]     & 0x3F));
                    if (c < 0x10000) {
                        // Overlong sequence, reject as invalid.
                        goto invalid;
                    }
                    in += 4;
                    inLen -= 4;
                    r.w32++;
                    break;

                default:
                invalid:
                    // if non-strict, simply treat as character
                    if (strict)
                        return false;
                    else
                    {
                        r.w8++;
                        in++;
                        inLen--;
                    }
            }
        }
        return true;
    }

    // Analyze a UTF-16 string buffer and find out about the character widths.
    // chars >= 0x10000 are encoded as 0xD800 + (upper 10 bits) and 0xDC00 + (lower 10 bits)
    // Returns false in strict mode if a starting surrogate pair character is not followed
    // by a correct 2nd pair character

    static bool _analyzeUtf16(const wchar* p, int32_t len, StringWidths& r, bool strict)
    {
        r.ascii = r.w8 = r.w16 = r.w32 = 0;
        while (len-- > 0)
        {
            wchar ch = *p++;
            if (ch <= 0x7F)
                r.ascii++;
            if (ch <= 0xFF)
                r.w8++;
            else if (ch < 0xD800 || ch > 0xDBFF)
                r.w16++;
            else
            {
                if (len == 0 || *p < 0xDC00 || *p > 0xDFFF)
                {
                    if (strict)
                        return false;
                    // found one or two characters depending on whether the 2nd surrpgate character was present
                    r.w16 += len ? 2 : 1;
                }
                else
                    r.w32++;
                p++; len--;
            }
        }
        return true;
    }

    // Create a string out of an UTF-8 buffer.
    Stringp String::createUTF8
        (AvmCore* core, const utf8_t* buffer, int32_t len, Width desiredWidth, bool staticBuf, bool strict)
    {
        if (buffer == NULL)
        {
            len = 0;
            buffer = &k_zero.u8;
            staticBuf = true;
        }
        if (len < 0)
            len = Length((const char*)buffer);

        // determine the string width to use
        StringWidths widths;
        if (!_analyzeUtf8(buffer, len, widths, strict))
        {
            // TODO: bad character sequence error
            AvmAssert(!"bad UTF8 character sequence");
            return NULL;
        }

        if (desiredWidth == kAuto)
        {
            if (widths.w32 != 0 || widths.w16 != 0)
                desiredWidth = String::k16;
            else
                desiredWidth = String::k8;
        }

        if (desiredWidth == String::k8 && core->kEmptyString != NULL)
        {
            // core has been initialized, check for cached characters
            if (len == 0)
                return core->kEmptyString;

            if (len == 1 && *buffer < 128)
                return core->cachedChars[*buffer];
        }

        const bool is7bit = (widths.ascii == widths.w8) && !widths.w16 && !widths.w32;

        Stringp s = NULL;
        GC* gc = core->GetGC();
        if (desiredWidth == k8)
        {
            if (widths.w16 != 0 || widths.w32 != 0)
                // cannot do 8-bit string with this data
                // TODO: string-too-wide error
                s = NULL;
            else if (staticBuf && is7bit)
            {
                // works, because we only have 7-bit ASCII
                s = createStatic(gc, buffer, widths.w8, String::k8, is7bit);
            }
            else
            {
                s = createDynamic(gc, buffer, widths.w8, String::k8, is7bit);
                uint32_t uch;
                int32_t bytesRead;
                Pointers dst(s); // NB, we assume that Utf8ToUcs4 doesn't allocate memory!
                bool bugzilla609416 = core->currentBugCompatibility()->bugzilla609416;
                while (len > 0)
                {
                    if (*((int8_t*) buffer) > 0)
                    {
                        // ASCII
                        *dst.p8++ = char (*buffer++);
                        len--;
                    }
                    else
                    {
                        bytesRead = UnicodeUtils::Utf8ToUcs4(buffer, len, &uch, bugzilla609416);
                        if (bytesRead == 0)
                        {
                            // invalid sequence (only if strict was false)
                            *dst.p8++ = char (*buffer++);
                            len--;
                        }
                        else
                        {
                            buffer += bytesRead;
                            len -= bytesRead;
                            *dst.p8++ = char (uch);
                        }
                    }
                }
            }
        }
        else
        {
            // surrogate pairs need 2 characters
            s = createDynamic(gc, NULL, int32AddChecked(int32AddChecked(widths.w8, widths.w16), int32AddChecked(widths.w32, widths.w32)), String::k16, is7bit);
            int32_t const convertedlen = UnicodeUtils::Utf8ToUtf16(buffer, len, s->m_buffer.p16, s->m_length, strict);
            // if this assert fires, _analyzeUtf8 didn't agree with UnicodeUtils::Utf8ToUtf16, which is bad.
            AvmAssert(convertedlen == s->m_length);
            if (convertedlen < 0)
                return NULL;
        }
        VERIFY_7BIT(s);
        return s;
    }

    // Create a string out of an UTF-16 buffer.
    Stringp String::createUTF16
        (AvmCore* core, const wchar* buffer, int32_t len, Width desiredWidth, bool staticBuf, bool strict)
    {
        if (buffer == NULL)
        {
            len = 0;
            buffer = &k_zero.u16;
            staticBuf = true;
        }
        if (len < 0)
            len = Length(buffer);

        bool is7bit = false;
        int32_t stringLength = len;
        if (desiredWidth != k16)
        {
            // determine the string width to use
            StringWidths widths;
            if (!_analyzeUtf16(buffer, len, widths, strict))
            {
                // TODO: bad character sequence error
                return NULL;
            }

            if (desiredWidth == kAuto)
            {
                if (widths.w32 != 0 || widths.w16 != 0)
                    desiredWidth = k16;
                else
                    desiredWidth = k8;
            }
            else if (desiredWidth == k8 && (widths.w16 != 0 || widths.w32 != 0))
                // cannot do 8-bit string with this data
                // TODO: string-too-wide error
                return NULL;

            is7bit = (widths.ascii == widths.w8) && !widths.w16 && !widths.w32;
        }

        if (desiredWidth == k8 && core->kEmptyString != NULL)
        {
            // core has been initialized, check for cached characters
            if (len == 0)
                return core->kEmptyString;

            if (len == 1 && *buffer < 128)
                return core->cachedChars[*buffer];
        }

        if (desiredWidth == k16 && staticBuf)
        {
            return createStatic(core->GetGC(), buffer, len, k16, is7bit);
        }

        // found the width to use, now create that string
        Stringp s = createDynamic(core->GetGC(), NULL, stringLength, desiredWidth, is7bit);

        String::Pointers ptrs(s);
        if (desiredWidth == k8)
        {
            while (len-- > 0)
                *ptrs.p8++ = (char) *buffer++;
        }
        else
        {
            VMPI_memcpy(ptrs.pv, buffer, len << desiredWidth);
        }
        VERIFY_7BIT(s);
        return s;
    }

    PrintWriter& String::print(PrintWriter& prw) const
    {
        if (getWidth() == k8 && (m_bitsAndFlags & TSTR_7BIT_FLAG))
        {
            // fast form
            Pointers ptrs(this);
            prw.writeN( (const char*)ptrs.p8, m_length );
        }
        else
        {
            for (int32_t i=0, n=length(); i<n; i++)
            {
                prw << (wchar)charAt(i);
            }
        }
        return prw;
    }

    ///////////////////////////////////////////////////////////////////////////////////

    // ---------------------------------------------------------------------------

    //   0000 0000-0000 007F   0xxxxxxx
    //   0000 0080-0000 07FF   110xxxxx 10xxxxxx
    //   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

    //   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //   0400 0000-7FFF FFFF   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    StUTF8String::StUTF8String(Stringp str)
    {
        if (!str || !str->length())
        {
            m_buffer = &k_zero.c8;
            m_length = 0;
            return;
        }

        VERIFY_7BIT(str);

        int32_t len = 0;

        MMgc::GC* gc = _gc(str);
        if (str->getWidth() == String::k8)
        {
            len = str->m_length;

            // if we know it's 7-bit, we don't need to walk it.
            if (!(str->m_bitsAndFlags & String::TSTR_7BIT_FLAG))
            {
                const uint8_t* srcBuf = String::Pointers(str).p8;
                int32_t auxchars = 0;
                for (int32_t i = str->m_length; i--;)
                    auxchars += (*srcBuf++ > 127);

                len = int32AddChecked(len, auxchars);

                // no hi bits? set the bit!
                if (len == str->m_length)
                    str->m_bitsAndFlags |= String::TSTR_7BIT_FLAG;
            }

            // Deliberately using gc'ed memory here (not mmfx, unmanaged memory)
            // so that longjmp's past our dtor won't cause a long-term leak
            char* dstBuf = (char*)gc->Alloc(uint32_t(len)+1, 0);
            // can't re-use the Pointers from 7BIT above; gc->Alloc may have invalidated it
            const uint8_t* srcBuf = String::Pointers(str).p8;
            m_buffer = dstBuf;
            m_length = len;

            if (len == str->m_length)
            {
                VMPI_memcpy(dstBuf, srcBuf, len);
                dstBuf[len] = 0;
            }
            else
            {
                for (int32_t i = str->m_length; i--;)
                {
                    wchar ch = wchar(*srcBuf++);
                    if (ch >= 128)
                    {
                        *dstBuf++ = char(0xC0 + ((ch >> 6) & 0x3));
                        ch = 0x80 + (ch & 0x3F);
                        // fall thru
                    }
                    *dstBuf++ = char(ch);
                }
                AvmAssert(dstBuf - m_buffer == len);
                *dstBuf = 0;
            }
        }
        else
        {
            len = UnicodeUtils::Utf16ToUtf8(String::Pointers(str).p16, str->length(), NULL, 0);
            if (len < 0)
                len = 0;
            char* dstBuf = (char*) gc->Alloc(uint32_t(len)+1, 0);
            m_buffer = dstBuf;
            m_length = len;
            dstBuf[len] = 0;
            // can't re-use the Pointers from Utf16ToUtf8 above; gc->Alloc may have invalidated it
            UnicodeUtils::Utf16ToUtf8(String::Pointers(str).p16, str->length(), (uint8_t*) dstBuf, len);
        }
    }

    StUTF8String::~StUTF8String()
    {
        if (m_buffer != &k_zero.c8) {
            // StrUTF8String is only ever stack allocated so it's OK just to free m_buffer without NULLing it first.
            GC::GetGC(m_buffer)->Free(m_buffer);
        }
    }

    StUTF16String::StUTF16String(Stringp str)
    {
        if (!str || !str->length())
        {
            m_length = 0;
            m_buffer = &k_zero.u16;
            return;
        }

        VERIFY_7BIT(str);

        MMgc::GC* gc = _gc(str);

        m_length = str->m_length;
        // Deliberately using gc'ed memory here (not mmfx, unmanaged memory)
        // so that longjmp's past our dtor won't cause a long-term leak
        wchar* dst = (wchar*) gc->Alloc(uint32ShlChecked(uint32_t(m_length)+1, String::k16), 0);
        m_buffer = dst;
        dst[m_length] = 0;
        String::Pointers ptrs(str);
        _copyBuffers(ptrs.pv, dst, m_length, str->getWidth(), String::k16);
    }

    StUTF16String::~StUTF16String()
    {
        if (m_buffer != &k_zero.u16) {
            // StrUTF16String is only ever stack allocated so it's OK just to free m_buffer without NULLing it first.
            GC::GetGC(m_buffer)->Free(m_buffer);
        }
    }

    // The following table is the length of an UTF-8 sequence, indexed by the
    // first character masked with 0x3F. It is assumed that the data is good
    // UTF-8, which is true, since we just created it :) The table is only
    // used for characters >= 0x80.
    // 0000 0080-0000 07FF   000xxxxx
    // 0000 0800-0000 FFFF   0010xxxx
    // 0001 0000-001F FFFF   00110xxx
    // 0020 0000-03FF FFFF   001110xx
    // 0400 0000-7FFF FFFF   0011110x

    static const uint8_t utf8Lengths[64] =
    {
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6
    };

    StIndexableUTF8String::StIndexableUTF8String(Stringp s) :
        StUTF8String(s),
        m_lastPos(0),
        m_lastUtf8Pos(0)
    {
        m_indexable = (s->length() == this->length());  // can't init in init-list, generates warning :-(
    }

    int32_t StIndexableUTF8String::toUtf8Index(int32_t pos)
    {
        if (pos <= 0 || pos >= length() || m_indexable)
            return pos;

        // optimization: these two members kick in if this method
        // is called for positions in ascending order (which pcre usually does)
        if (pos < m_lastPos)
            m_lastPos = m_lastUtf8Pos = 0;

        int32_t utf8Pos = m_lastUtf8Pos;

        const utf8_t* p = (const utf8_t*) this->c_str() + m_lastUtf8Pos;
        for (int32_t i = m_lastPos; i < pos; i++)
        {
            utf8_t ch = *p;
            if (ch < 128)
                p++, utf8Pos++; // ASCII
            else
            {
                int32_t len = utf8Lengths[ch & 0x3F];
                p += len;
                utf8Pos += len;
            }
        }
        m_lastPos = pos;
        m_lastUtf8Pos = utf8Pos;
        return utf8Pos;
    }

    int32_t StIndexableUTF8String::toIndex(int32_t utf8Pos)
    {
        if (utf8Pos <= 0 || m_indexable)
            return utf8Pos;

        // optimization: these two members kick in if this method
        // is called for positions in ascending order (which pcre usually does)
        if (utf8Pos < m_lastUtf8Pos)
            m_lastPos = m_lastUtf8Pos = 0;

        int32_t i = m_lastUtf8Pos;
        int32_t pos = m_lastPos;

        const utf8_t* p = (const utf8_t*) this->c_str() + m_lastUtf8Pos;
        while (i < utf8Pos)
        {
            if (i >= length())
                break;
            utf8_t ch = *p;
            if (!(ch & 0x80))
                i++, p++;   // ASCII
            else
            {
                int32_t len = utf8Lengths[ch & 0x3F];
                i += len;
                p += len;
            }
            pos++;
        }
        m_lastPos = pos;
        m_lastUtf8Pos = utf8Pos;
        return pos;
    }

}   // end namespace avmplus
