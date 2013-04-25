/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "zlib.h"

// Bugzilla 729336 followup: fix project files to add lzma to search path
#include "../other-licenses/lzma/LzmaLib.h"

#ifndef UINT32_T_MAX
    #define UINT32_T_MAX (uint32_t(0xFFFFFFFFUL))
#endif

// Compiler and architecture dependent byte swapping functions.
//
// Define HAVE_BYTESWAP32 and/or HAVE_BYTESWAP16 on platforms where there are
// instructions or fast idioms for byte swapping backed by compiler intrinsics.
//
// This is not quite a porting API, because we do not abstract byteSwap16()
// and byteSwap32() generally: if fast byte swapping is not available then 
// it's probably faster to use generic in-line byte load/store code.  Hence 
// the code is all here and not in a platform file.
//
// Survey of architectures:
//
//   On x86 there is BSWAP (32-bit) and XCHG (16-bit).
//   On PPC there is supposedly a three-operation sequence that byte swaps 32-bit
//     data [Warren, Hacker's Delight].
//   On MIPS a two-operation sequence of WSBH and ROTR can byte swap a 32-bit word,
//     WSBH can byte swap a halfword by itself.
//   On SH4 a combination of SWAP.B (twice) and SWAP.H should byte swap a 32-bit word,
//     while SWAP.B by itself should byte swap a 16-bit halfword.
//   On ARM there are the REV (32-bit) and REV16 (16-bit) instructions since ARMv6.
//   To my knowledge SPARC has no byte swap instructions.
//
// Survey of compilers:
//
//   MSVC starting with Visual Studio 2005 have _byteswap_ulong, _byteswap_ushort, _byteswap_uint64.
//   GCC starting with 4.3 have __builtin_bswap32 and __builtin_bswap64, must include <byteswap.h>

#if defined VMCFG_IA32 || defined VMCFG_AMD64 || defined VMCFG_PPC || defined VMCFG_MIPS || defined VMCFG_SH4 || defined VMCFG_ARM

#if defined _MSC_VER
#define HAVE_BYTESWAP32
#define HAVE_BYTESWAP16
REALLY_INLINE uint32_t byteSwap32(uint32_t x)
{
    return _byteswap_ulong(x);
}

REALLY_INLINE uint16_t byteSwap16(uint16_t x)
{
    return _byteswap_ushort(x);
}
#endif // _MSC_VER

#if defined __GNUC__ && (__GNUC__ == 4 && __GNUC_MINOR__ >= 3 || __GNUC__ > 4)
#include <byteswap.h>
#define HAVE_BYTESWAP32

REALLY_INLINE uint32_t byteSwap32(uint32_t x)
{
    return __builtin_bswap32(x);
}
#endif // __GNUC__

#endif // Various architectures with byteswap idioms backed by intrinsics

// MAX_BYTEARRAY_STORE_LENGTH is the largest value that can be taken on by m_length,
// strictly less than 2^32.  See constraint in ByteArray::Grower::ReallocBackingStore().
//
// MAX_BYTEARRAY_SHORT_ACCESS_LENGTH is a value no smaller than 4095 and no greater
// than 2^32-1-MAX_BYTEARRAY_STORE_LENGTH.
//
// The purpose of these two limits is to allow relatively efficient bounds checking
// on common ByteArray accesses, see comments on ByteArray::requestBytesForShortRead()
// and ByteArray::requestBytesForShortWrite() for more information.

#define MAX_BYTEARRAY_STORE_LENGTH (MMgc::GCHeap::kMaxObjectSize - MMgc::GCHeap::kBlockSize*2)
#define MAX_BYTEARRAY_SHORT_ACCESS_LENGTH (MMgc::GCHeap::kBlockSize*2 - 1)

namespace avmplus
{
    template <typename T> REALLY_INLINE T min(T a, T b) {
        return (a < b) ? a : b;
    }

    // Rounds a up to nearest integral multiple of b.
    // requires: T is integral type.
    // returns r, where r is least integral multiple of b such that r >= a.
    //
    // Bugzilla 699176: lift to shared header or specialize to uint64_t.
    template <typename T> REALLY_INLINE T roundUpTo(T a, T b) {
        return ((a + (b - 1)) / b) * b;
    }

#ifdef SECURITYFIX_ELLIS //See https://bugzilla.mozilla.org/show_bug.cgi?id=790837
    /**
     * check for overlap in the address ranges [dst,dst+count] and [src,src+count].
     * Motivating use case: checking whether it's safe to call memcpy or whether
     * (the potentially slower) memmove must be used instead.
     * NB. memcpy is allowed to copy backwards, in big gulps, etc. No overlap is acceptable.
     * I (matz) wonder if the extra checking done by this method washes out the benefit of calling memcopy?
     * Prompted by (google)fuzzing bug 3303673.
     */
     inline bool non_overlapping_memcpy_args(const void * dst, const void *src, const uint32_t count)
     {
         AvmAssert(count >=0 );
         //replaces: if ((uintptr_t(dst) - uintptr_t(src)) >= uintptr_t(count))
         //which was one sided and hence wrong.
         //pointer difference is naturally long on x86_64, but 32 bit on ia32,
         // do calculation carefully in pointer sized ints.
         uintptr_t p_src = uintptr_t(src);
         uintptr_t p_dst = uintptr_t(dst);
        
         //compare the pointers unsigned.
         //do the subtraction such that result is positive and can't overflow anything.
         //if the difference is > uint32_t, it's okay, compare to count will be right.
        
         if ( p_src >= p_dst ){
             return (p_src - p_dst) >= count; 
         }else{
             return (p_dst - p_src) >= count;
         }
     }

    // When we might be reading or writing to ourself, use this function
    // apparently SunPro compiler doesn't like combining REALLY_INLINE with static functions in CPP files
    /*static*/
    REALLY_INLINE void move_or_copy(void* dst, const void* src, uint32_t count)
    {
        if ( non_overlapping_memcpy_args(dst,src,count) )
        {
            VMPI_memcpy(dst, src, count); //not overlapping, so can safely use memcpy
        }
        else
        {
            VMPI_memmove(dst, src, count); //overlap. have to use memmove.
        }
    }

#else /*SECURITYFIX_ELLIS*/
    REALLY_INLINE void move_or_copy(void* dst, const void* src, uint32_t count)
    {
        if ((uintptr_t(dst) - uintptr_t(src)) >= uintptr_t(count))
        {
            VMPI_memcpy(dst, src, count);
        }
        else
        {
            VMPI_memmove(dst, src, count);
        }
    }
#endif /*SECURITYFIX_ELLIS*/



    //
    // ByteArray
    //
    
    ByteArray::ByteArray(Toplevel* toplevel)
        : DataIOBase()
        , DataInput()
        , DataOutput()
        , m_toplevel(toplevel)
        , m_gc(toplevel->core()->GetGC())
        , m_subscribers(m_gc, 0)
        , m_copyOnWriteOwner(NULL)
        , m_position(0)
        , m_buffer(mmfx_new(Buffer()))
        , m_isShareable(false)
        , m_isLinkWrapper(false)
    {
        static_assert(uint64_t(MAX_BYTEARRAY_STORE_LENGTH) < 0x100000000ULL, "Constraint on MAX_BYTEARRAY_STORE_LENGTH");
        static_assert(MAX_BYTEARRAY_SHORT_ACCESS_LENGTH >= 4095, "Constraint on MAX_BYTEARRAY_SHORT_ACCESS_LENGTH");
        static_assert(uint64_t(MAX_BYTEARRAY_STORE_LENGTH) + uint64_t(MAX_BYTEARRAY_SHORT_ACCESS_LENGTH) < 0x100000000ULL, "Constraints on internal ByteArray constants");
        AvmAssert(m_gc != NULL);

        m_buffer->array = NULL;
        m_buffer->capacity = 0;
        m_buffer->length = 0;
    }


    ByteArray::ByteArray(Toplevel* toplevel, const ByteArray& other)
        : DataIOBase()
        , DataInput()
        , DataOutput()
        , m_toplevel(toplevel)
        , m_gc(toplevel->core()->GetGC())
        , m_subscribers(m_gc, 0)
        , m_copyOnWriteOwner(NULL)
        , m_position(0)
		, m_buffer(other.m_isShareable ? other.m_buffer : mmfx_new(Buffer) )
        , m_isShareable(other.m_isShareable)
        , m_isLinkWrapper(false)
    {
        AvmAssert(m_gc != NULL);
        if (m_isShareable) {
            return;
        }
        m_buffer->capacity = other.m_buffer->capacity;
        m_buffer->length = other.m_buffer->length;
        // only allocate a new array if the other bytearray has data in it
        if (other.m_buffer->array) {
            m_buffer->array = mmfx_new_array_opt(uint8_t, m_buffer->capacity, MMgc::kCanFailAndZero);

            if (!m_buffer->array)
               ThrowMemoryError();
            
            TellGcNewBufferMemory(m_buffer->array, m_buffer->capacity);
            VMPI_memcpy(m_buffer->array, other.m_buffer->array, m_buffer->length);
        }
        else {
            m_buffer->array = NULL;
        }
    }


    ByteArray::ByteArray(Toplevel* toplevel, ByteArray::Buffer* source, bool shareable)
        : DataIOBase()
        , DataInput()
        , DataOutput()
        , m_toplevel(toplevel)
        , m_gc(toplevel->core()->GetGC())
        , m_subscribers(m_gc, 0)
        , m_copyOnWriteOwner(NULL)
        , m_position(0)
        , m_buffer(source)
        , m_isShareable(shareable) 
        , m_isLinkWrapper(false)
    {
		// Note that this constructor is only used when receiving a ByteArray from another worker
		if (!m_isShareable) {
			// If we made a copy of the ByteArray (i.e. it's not sharable), then we should account for the memory that we're about to receive.
			// (until now, it's been sitting in the message queue, not assigned to any GC)
			TellGcNewBufferMemory(m_buffer->array, m_buffer->capacity);
		}
    }


    /* virtual */ void ByteArray::Buffer::destroy() 
    {
        if (array)
        {
            AvmAssert(capacity > 0);
            // FIXME: the lack of the following appears to be accidentally correct, because
            // for normal uses array will be NULL in this->destroy()
            // and the Isolate use doesn't signal dependent allocation
            //m_gc->SignalDependentDeallocation(capacity, MMgc::typeByteArray);
            mmfx_delete_array(array);
        }
        mmfx_delete(this);
    }

    /* virtual */ ByteArray::Buffer::~Buffer() 
    {
    }

    ByteArray::~ByteArray()
    {
        if (!m_isShareable) {
            // no: this can reallocate memory, which is bad to do in a dtor
            // m_subscribers.clear();
            _Clear();
        } else {
            m_copyOnWriteOwner  = NULL;
            // rely on refcounting, i.e., m_buffer->destroy()
        }
        m_buffer = NULL;
    }

    void ByteArray::Clear()
    {
        if (m_subscribers.length() > 0)
        {
            AvmAssert(false); // shouldn't get here?
            m_toplevel->throwRangeError(kInvalidRangeError);
        }
        _Clear();
    }
    
    void ByteArray::_Clear()
    {
        if (m_buffer->array && !IsCopyOnWrite())
        {
            AvmAssert(m_buffer->capacity > 0);
            // Note that TellGcXXX always expects capacity, not (logical) length.
            TellGcDeleteBufferMemory(m_buffer->array, m_buffer->capacity);
            mmfx_delete_array(m_buffer->array);
        }
        m_buffer->array             = NULL;
        m_buffer->capacity          = 0;
        m_buffer->length            = 0;
        m_copyOnWriteOwner  = NULL;
    }

    void ByteArray::SetCopyOnWriteOwner(MMgc::GCObject* owner)
    {
        if (owner != NULL && m_gc->IsPointerToGCPage(this))
        {
            WB(m_gc, m_gc->FindBeginningFast(this), &m_copyOnWriteOwner, owner);
        }
        else
        {
            m_copyOnWriteOwner = owner;
        }
    }

    void ByteArray::SetCopyOnWriteData(MMgc::GCObject* owner, const uint8_t* data, uint32_t length)
    {
        Clear();
        m_buffer->array             = const_cast<uint8_t*>(data);
        m_buffer->capacity          = length;
        m_buffer->length            = length;
        // we must have a non-null value for m_copyOnWriteOwner, as we
        // use it as an implicit boolean as well, so if none is provided,
        // cheat and use m_gc->emptyWeakRef
        if (owner == NULL)
            owner = (MMgc::GCObject*)m_gc->emptyWeakRef;
        SetCopyOnWriteOwner(owner);
    }
        
    REALLY_INLINE bool ByteArray::Grower::RequestWillReallocBackingStore() const
    {
        return m_minimumCapacity > m_owner->m_buffer->capacity || m_owner->IsCopyOnWrite();
    }

    REALLY_INLINE bool ByteArray::Grower::RequestExceedsMemoryAvailable() const
    {
        return m_minimumCapacity > (MMgc::GCHeap::kMaxObjectSize - MMgc::GCHeap::kBlockSize*2);
    }

    void FASTCALL ByteArray::Grower::EnsureWritableCapacity()
    {
        if (RequestExceedsMemoryAvailable())
            m_owner->ThrowMemoryError();

        if (RequestWillReallocBackingStore())
        {
            uint32_t newCapacity = m_owner->m_buffer->capacity << 1;
            if (newCapacity < m_minimumCapacity)
                newCapacity = m_minimumCapacity;
            if (newCapacity < kGrowthIncr)
                newCapacity = kGrowthIncr;

            ReallocBackingStore(newCapacity);
        }
    }

    void FASTCALL ByteArray::Grower::ReallocBackingStore(uint32_t newCapacity)
    {
        // The extra check on maximum size is necessary because
        // mmfx_new_array_opt doesn't return NULL but instead Aborts
        // when the size approaches 2^32. We want to be consistent
        // across the entire uint32_t range and throw a memory error
        // instead.  The subtraction of two blocks has to do with how
        // FixedMalloc::LargeAlloc does this check.
        //
        // The length limitation is however useful for other purposes.
        // Observe that the length limit is duplicated above in the 
        // definition of MAX_BYTEARRAY_STORE_LENGTH and that there is 
        // code below in ByteArray::requestBytesForShortRead() and
        // ByteArray::requestBytesForShortWrite() that depends on the
        // limit on m_length, see comments there and also comments on
        // MAX_BYTEARRAY_STORE_LENGTH.

        static_assert(MAX_BYTEARRAY_STORE_LENGTH == (MMgc::GCHeap::kMaxObjectSize - MMgc::GCHeap::kBlockSize*2),
                      "Constraint on maximum ByteArray storage size");

        if (newCapacity > (MMgc::GCHeap::kMaxObjectSize - MMgc::GCHeap::kBlockSize*2))
            m_owner->ThrowMemoryError();

        // Bugzilla 699176, 763519: EnsureWritableCapacity passes a
        // newCapacity that is strictly larger than our existing one,
        // but Grower::SetLengthCommon, when called from length
        // setter, can pass a newCapacity that is smaller (by design).
        // Therefore, eagerly return here only when the old and new
        // capacities are equal (or if the owner is copy-on-write).
        if (!(newCapacity != m_owner->m_buffer->capacity || m_owner->IsCopyOnWrite()))
        {
            return;
        }

        if (newCapacity < m_minimumCapacity)
            newCapacity = m_minimumCapacity;
        if (newCapacity < kGrowthIncr)
            newCapacity = kGrowthIncr;
        
        m_oldArray = m_owner->m_buffer->array;
        m_oldLength = m_owner->m_buffer->length;
        m_oldCapacity = m_owner->m_buffer->capacity;
        
        uint8_t* newArray = mmfx_new_array_opt(uint8_t, newCapacity, MMgc::kCanFail);
        if (!newArray)
            m_owner->ThrowMemoryError();

        // Note that TellGcXXX always expects capacity, not (logical) length.
        m_owner->TellGcNewBufferMemory(newArray, newCapacity);
        if (m_oldArray)
        {
            VMPI_memcpy(newArray, m_oldArray, min(newCapacity, m_oldLength));
            if (newCapacity > m_oldLength)
                VMPI_memset(newArray+m_oldLength, 0, newCapacity-m_oldLength);
        }
        else
        {
            VMPI_memset(newArray, 0, newCapacity);
        }

        m_owner->m_buffer->array = newArray;
        m_owner->m_buffer->capacity = newCapacity;
        if (m_owner->m_copyOnWriteOwner != NULL)
        {
            m_owner->m_copyOnWriteOwner = NULL;
            // Set this to NULL so we don't attempt to delete it in our dtor.
            m_oldArray = NULL;
        }
    }

    /*
        Why the "Grower" class?
        
        (1) It provides a clean way to defer discarding the old buffer until the
            end of the calling function; this matters in the case of Write(),
            as it's legal to call Write() on your own buffer, and so if growth
            occurs, you must not discard the old buffer until copying takes place.
        (2) It avoid redundant calls to NotifySubscribers(); previously we'd call
            once when a reallocation occurred, then again when the length field
            changed.
        (3) It streamlines copy-on-write handling; formerly we either did
            redundant CopyOnWrite alloc-and-copy followed by a Grow alloc-and-copy,
            or an if-else clause with redundant alloc-and-copy.
            
        Of course, the dtor will be skipped (and thus notify and deletes skipped)
        if a longjmp over it, but the old code was subject to the same defects,
        so this is not a new liability.
    */
    ByteArray::Grower::~Grower()
    {
        if (m_oldArray != m_owner->m_buffer->array || m_oldLength != m_owner->m_buffer->length)
        {
            m_owner->NotifySubscribers();
        }
        // m_oldArray could be NULL if we grew a copy-on-write ByteArray.
        if (m_oldArray != NULL && m_oldArray != m_owner->m_buffer->array)
        {
            // Note that TellGcXXX always expects capacity, not (logical) length.
            m_owner->TellGcDeleteBufferMemory(m_oldArray, m_oldCapacity);
            mmfx_delete_array(m_oldArray);
        }
    }

    //
    // The following code is added to support sharing byte array data between
    // two Isolates. Domain memory is affected by changes to shared byte arrays
    // and must be notified within a safepoint when a backing store has been 
    // changed. Every attempt was made to avoid a safepoint when not 
    // needed.
    //
    // Assumptions: 
    //    - Acquiring a safepoint is more expensive than 2-3 function calls and
    //      several if statements.
    //    - A byte array is only considered shared when its buffer has more than 
    //      a single reference to it and the only way to get an additional reference
    //      is to share a byte array between isolates, using setSharedProperty or 
    //      a similar mechanism.
    //
    //  Invariants:
    //    - all safepoint operations must happen using a SafepointTask object
    //    - a safepoint cannot be recursively entered
    //    - a safepoint cannot be long jumped out of; any exception must be
    //      caught and not allowed to escape the safepoint.
    //    - if the current operation is not in a safepoint no operations can be
    //      in a safepoint task.
    //    - only operations that change the size of a byte array's backing
    //      store may require a safepoint
    //    - only backing stores that are shared between isolates may require
    //      a safepoint.
    // 

    //
    // This is the base class for all ByteArray tasks that *may* need to run
    // in a safepoint.  the exec method makes the determination of requiring
    // safepoint based on if the associated byte array is local or shared.
    // 
    class ByteArrayTask: public vmbase::SafepointTask
    {
    public:
        ByteArrayTask(ByteArray* ba)
            : m_byteArray(ba)
            , m_core(ba->m_toplevel->core())
            , m_exception(NULL)
        {}

        void exec()
        {
            Isolate* isolate = m_core->getIsolate();
            isolate->getAggregate()->runSafepointTaskHoldingIsolateMapLock(this);
            if (m_exception) {
                m_core->throwException(m_exception);
            }
        }

    protected:
        ByteArray* m_byteArray;
        AvmCore* m_core;
        Exception* m_exception;
    };

    //
    // ensures that we change the length of the byte array within a safepoint
    // when required. if the new length will not cause a resize or if the
    // byte array isn't shared then no safepoint 
    // 
    class ByteArraySetLengthTask: public ByteArrayTask
    {
    public:
        ByteArraySetLengthTask(ByteArray* ba, uint32_t newLength, bool calledFromLengthSetter)
            : ByteArrayTask(ba)
            , m_newLength(newLength)
            , m_calledFromLengthSetter(calledFromLengthSetter)
        {
        }

        void run()  
        {
            // safepoints cannot survive exceptions
            TRY(m_core, kCatchAction_Rethrow)
            {
                m_byteArray->UnprotectedSetLengthCommon(m_newLength, m_calledFromLengthSetter);
            }
            CATCH(Exception* e)
            {
                m_exception = e;
            }
            END_CATCH;
            END_TRY;
        }
        
    private:
        uint32_t m_newLength;
        bool m_calledFromLengthSetter;
    };

    //
    // this task ensures that the compare and swap operation is atomic
    // if the backing store will be updated when the length is set this
    // task ensures for shared byte arrays that it will be performed
    // in a safepoint.
    // 
    class ByteArrayCompareAndSwapLengthTask: public ByteArrayTask
    {
    public:
        ByteArrayCompareAndSwapLengthTask(ByteArray* ba, uint32_t expectedLength, uint32_t newLength)
            : ByteArrayTask(ba) 
            , result(0)
            , m_expectedLength(expectedLength)
            , m_newLength(newLength)
        {
        }

        void run()
        {
            // safepoints cannot survive exceptions
            TRY(m_core, kCatchAction_Rethrow)
            {
                // we are in a safepoint or no other threads can possibly
                // be referencing this byte array
                result = m_byteArray->UnprotectedAtomicCompareAndSwapLength(m_expectedLength, m_newLength);
            }
            CATCH(Exception* e)
            {
                m_exception = e;
            }
            END_CATCH;
            END_TRY;
        }

        uint32_t result;

    private:
        uint32_t m_expectedLength;
        uint32_t m_newLength;
    };

    //
    // this task is used for lzma compression operations on 
    // shared byte arrays *only*.
    // it allows the very expensive lzma compression algorithm
    // to run on a temporary buffer outside of a safepoint
    // and then to swap the temp/scratch buffer in a safepoint
    //
    class ByteArraySwapBufferTask: public ByteArrayTask
    {
    public:
        ByteArraySwapBufferTask(ByteArray* ba, ByteArray::Buffer* dest)
            : ByteArrayTask(ba)
            , m_destination(dest)
        {}

        void run()
        {
            ByteArray::Buffer* src = m_byteArray->getUnderlyingBuffer();

            AvmAssert((src != NULL) && (m_destination != NULL));
            m_destination->array = src->array;
            m_destination->capacity = src->capacity;
            m_destination->length = src->length;

            // we don't want to swap the sources array
            // value we just want to set it to null
            // as the compression method will notify
            // the GC and delete it after the safepoint
            // has completed
            src->array = NULL;

            m_byteArray->m_buffer = m_destination;

            // notify all interested parties
            ByteArray::UpdateSubscribers();
        }

    private:
        FixedHeapRef<ByteArray::Buffer> m_destination;
    };

    uint8_t* FASTCALL ByteArray::GetWritableBuffer()
    {
        // setlength is always called before using this
        // we are asserting that to remain the case
        return m_buffer->array;
    }

    uint8_t& ByteArray::operator[](uint32_t index)
    {
        if (index >= m_buffer->length)
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243
        {
            // Suppose index=2^32-1.  Then:
            //
            //  - on 32-bit systems m_array[index] will reference the byte preceding
            //    the array in memory, which should not be read or written, and
            //
            //  - on 64-bit systems m_array[index] will reference the byte at offset
            //    2^31-1 from the start of m_array, most likely crashing the program
            //    on access because it's not allocated.
            //
            // In both it sets the length to 0, which is not expected.
            //
            // The choice of MemoryError here is not arbitrary: this is the error
            // we would get by trying to grow the array to accomodate 2^32 bytes,
            // and indeed if index == 2^32-2 then SetLength will throw that error
            // because the max array size is 2^32-8K, see EnsureWritableCapacity().
            //
            // Observe that ByteArrayObject::setUintProperty() does not range check.
            // Hence we check here and thereby cover all other callers too.
            //
            // (ByteArrayObject::getUintProperty() calls uint8_t operator[], not
            // uint8_t& operator[].  The former range checks properly.)

            if (index == UINT32_T_MAX)
                ThrowMemoryError();
            SetLength(index + 1);
        }
#else
            SetLength(index + 1);
#endif
        return m_buffer->array[index];
    }

    // Set the length to x+y, but check for overflow.
    //
    // MemoryError is the error thrown by SetLength(index), or really by
    // ByteArray::Grower::ReallocBackingStore(), if we try to create a
    // buffer larger than the buffer limit, so it's the most consistent 
    // error to throw here.
    //
    // We don't know anything about x and y so 64-bit math is the most
    // expedient solution.  SetLength() is not hot in any case, it's always
    // guarded by some other check.

    void FASTCALL ByteArray::SetLength(uint32_t x, uint32_t y)
    {
        uint64_t sum = uint64_t(x) + uint64_t(y);
        if (sum >= 0x100000000ULL)
            ThrowMemoryError();
        SetLength(uint32_t(sum));
    }

    // Bugzilla 699176: This threshold controls whether we switch to
    // large-size handling.  The chosen increment (24 MB) was
    // suggested by Scott Petersen based on experiments with policy in
    // Alchemy; the chosen threshold (also 24 MB) was assumed to be an
    // appropriate value, but it would be worthwhile to check that
    // assumption when time permits.

    static const uint32_t kHugeGrowthThreshold = 24*1024*1024;
    static const uint32_t kHugeGrowthIncr = 24*1024*1024;

    void ByteArray::SetLengthCommon(uint32_t newLength, bool calledFromLengthSetter)
    {
        if (IsShared()) {
            ByteArraySetLengthTask task(this, newLength, calledFromLengthSetter);
            task.exec();
        }
        else {
            UnprotectedSetLengthCommon(newLength, calledFromLengthSetter);
        }
    }

    void ByteArray::UnprotectedSetLengthCommon(uint32_t newLength, bool calledFromLengthSetter)
    {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=723401
        if (m_subscribers.length() > 0 && newLength < DomainEnv::GLOBAL_MEMORY_MIN_SIZE) {
#else
        if (m_subscribers.length() > 0 && m_buffer->length < DomainEnv::GLOBAL_MEMORY_MIN_SIZE) {
#endif
            m_toplevel->throwRangeError(kInvalidRangeError);
        }

        Grower grower(this, newLength);
        grower.SetLengthCommon(newLength, calledFromLengthSetter);
    }


    void ByteArray::Grower::SetLengthCommon(uint32_t newLength, bool calledFromLengthSetter)
    {
        if (!calledFromLengthSetter ||
            (newLength < kHugeGrowthThreshold &&
             m_owner->m_buffer->length < kHugeGrowthThreshold))
        {
            if (newLength > m_owner->m_buffer->capacity)
            {
                EnsureWritableCapacity();
            } 
        }
        else
        {
            // Bugzilla 699176: Setting the length is different than other
            // expanding/contracting operations; it represents the client
            // saying exactly how much (or how little) storage they
            // anticipate using.
            //
            // Thus, when we are (or will be) at a large size:
            // 1. Call ReallocBackingStore() directly and unconditionally,
            //    rather than conditionally via EnsureWritableCapacity().
            // 2. Round capacity up to multiple of kHugeGrowthIncr to
            //    avoid some pathoglogical cases.

            // (overflow paranoia; see notes above SetLength(uint32_t, uint32_t).
            uint64_t newCapRoundedUp =
                roundUpTo(uint64_t(newLength), uint64_t(kHugeGrowthIncr));
            uint32_t newCap = ((newCapRoundedUp <= MAX_BYTEARRAY_STORE_LENGTH)
                               ? uint32_t(newCapRoundedUp) : newLength);

            AvmAssert(newCap >= newLength);

            if (newCap != m_owner->m_buffer->capacity)
            {
                ReallocBackingStore(newCap);
            } 
        }
        
        m_owner->m_buffer->length = newLength;
        
        if (m_owner->m_position > newLength) {
            m_owner->m_position = newLength;
        }

        // notify all subscribers to this byte array data if it is being
        // shared between workers
        ByteArray::UpdateSubscribers();
    }


    void FASTCALL ByteArray::SetLength(uint32_t newLength)
    {
        SetLengthCommon(newLength, false);
    }

    void FASTCALL ByteArray::SetLengthFromAS3(uint32_t newLength)
    {
        SetLengthCommon(newLength, true);
    }

    void ByteArray::Read(void* buffer, uint32_t count)
    {
        CheckEOF(count);
        move_or_copy(buffer, m_buffer->array + m_position, count);
        m_position += count;
    }

    /*static*/
    void ByteArray::UpdateSubscribers() 
    {
        // anytime the length of the bytearray is updated notify all workers
        // regardless of if the backing store is reallocated or not.
        if (vmbase::SafepointRecord::hasCurrent()) {
            if (vmbase::SafepointRecord::current()->manager()->inSafepointTask()) {
                AvmCore::getActiveCore()->getIsolate()->getAggregate()->reloadGlobalMemories();
            }
        }
    }

    void ByteArray::Write(const void* buffer, uint32_t count)
    {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243
        // Though m_length is limited to 2^32-8K by EnsureWritableCapacity() above, m_position is 
        // not similarly limited - it can be set beyond length and can be as large as 2^32-1.
        // No callers to Write() check that m_position + count does not overflow, so we
        // check that here.  There's a question of whether MemoryError or RangeError is the "best"
        // error to throw; ReadByteArray throws RangeError in the similar circumstance but as
        // EnsureWritableCapacity() always throws MemoryError, we choose that here.
        if (count > UINT32_T_MAX - m_position) // Do not rearrange, guards against 64-bit overflow
            ThrowMemoryError();
#else
#endif
        uint32_t writeEnd = m_position + count;
        
        Grower grower(this, writeEnd);
        grower.EnsureWritableCapacity();
        
        move_or_copy(m_buffer->array + m_position, buffer, count);
        m_position += count;
        if (m_buffer->length < m_position)
            m_buffer->length = m_position;
    }

    void ByteArray::EnsureCapacity(uint32_t capacity)
    {
        Grower grower(this, capacity);
        grower.EnsureWritableCapacity();
    }

    NO_INLINE void ByteArray::EnsureCapacityNoInline(uint32_t capacity)
    {
        this->EnsureCapacity(capacity);
    }

    bool ByteArray::EnsureCapacityOrFail(uint32_t newCap,
                                         enum CatchAction catch_action,
                                         Exception **exn_recv)
    {
        // Argh, even when refactored like so, Android NDK r6b
        // compiler (in darwin-x86/arm-linux-androideabi/bin/g++, aka
        // GCC 4.4.3, at -O3 optimization level) continues to issue
        // warnings (promoted to errors by -Werror) of the form:
        //
        //   error: argument 'this' might be clobbered by 'longjmp' or 'vfork'
        //   error: argument 'newCap' might be clobbered by 'longjmp' or 'vfork'
        //
        // A quick survey hints that this is a GNU compiler bug.  The
        // main work-arounds Felix has identified are to either:
        // * put 'this' and 'newCap' into volatile locals, or
        // * keep the call inside the TRY body from being inlined.

        TRY(m_toplevel->core(), catch_action)
        {
            EnsureCapacityNoInline(newCap);
            return true;
        }
        CATCH(Exception *exn)
        {
            *exn_recv = exn;
            return false;
        }
        END_CATCH
        END_TRY
    }

    
    void ByteArray::NotifySubscribers()
    {
        for (uint32_t i = 0, n = m_subscribers.length(); i < n; ++i)
        {
            AvmAssert(m_buffer->length >= DomainEnv::GLOBAL_MEMORY_MIN_SIZE);
 
            DomainEnv* subscriber = m_subscribers.get(i);
            if (subscriber)
            {
                subscriber->notifyGlobalMemoryChanged(m_buffer->array, m_buffer->length);
            }
            else
            {
                // Domain went away? remove link
                m_subscribers.removeAt(i);
                --i;
            }
        }
    }
 
    bool ByteArray::addSubscriber(DomainEnv* subscriber)
    {
        if (m_buffer->length >= DomainEnv::GLOBAL_MEMORY_MIN_SIZE)
        {
            removeSubscriber(subscriber);
            m_subscribers.add(subscriber);
            // notify the new "subscriber" of the current state of the world
            subscriber->notifyGlobalMemoryChanged(m_buffer->array, m_buffer->length);
            return true;
        }
        return false;
    }
 
    bool ByteArray::removeSubscriber(DomainEnv* subscriber)
    {
        for (uint32_t i = 0, n = m_subscribers.length(); i < n; ++i)
        {
            if (m_subscribers.get(i) == subscriber)
            {
                m_subscribers.removeAt(i);
                return true;
            }
        }
        return false;
    }
 
#ifdef DEBUGGER
    uint64_t ByteArray::bytesUsed() const
    {
        // If m_copyOnWrite is set, then we don't own the buffer, so the profiler
        // should not attribute it to us.
        return IsCopyOnWrite() ? 0 : m_buffer->capacity;
    }
#endif

    void ByteArray::TellGcNewBufferMemory(const uint8_t* buf, uint32_t numberOfBytes)
    {
        if (buf && numberOfBytes > 0)
        {
            m_gc->SignalDependentAllocation(numberOfBytes, MMgc::typeByteArray);
        }
    }

    void ByteArray::TellGcDeleteBufferMemory(const uint8_t* buf, uint32_t numberOfBytes)
    {
        // Note that we can't rely on using m_toplevel->core()->GetGC();
        // order of destruction is unspecified, and if this is called at destruction
        // time, Toplevel might have been destroyed before us. So keep a separate GC*.
        if (buf && numberOfBytes > 0)
        {
            m_gc->SignalDependentDeallocation(numberOfBytes, MMgc::typeByteArray);
        }
    }

    void ByteArray::Compress(CompressionAlgorithm algorithm)
    {
        switch (algorithm) {
        case k_lzma:
            CompressViaLzma();
            break;
        case k_zlib:
        default:
            CompressViaZlibVariant(algorithm);
            break;
        }
    }

    // lzma-compressed data format:
    // 5 bytes: LZMA properties
    // 8 bytes: uncompressed size k (little-endian)
    // k bytes: payload (the compressed data)

    static const uint32_t lzmaHeaderSize = LZMA_PROPS_SIZE + 8;

    struct lzma_compressed {
        uint8_t lzmaProps[LZMA_PROPS_SIZE];
        // our max byte array length is 4 bytes but size in lzma
        // header is 8 bytes; we reflect that here by splitting the
        // two (and leaving the high bits at zero)
        uint8_t unpackedSize[4];
        uint8_t unpackedSizeHighBits[4]; // (not uint32_t; that injects padding)
        uint8_t compressedPayload[1];    // payload is variable sized.
    };

    REALLY_INLINE uint32_t umax(uint32_t a, uint32_t b) { return a > b ? a : b; }

    void ByteArray::CompressViaLzma()
    {
        // Snarf the data and give ourself some empty data
        // (remember, existing data might be copy-on-write so don't dance on it)
        uint8_t* origData                       = m_buffer->array;
        uint32_t origLen                        = m_buffer->length;
        uint32_t origCap                        = m_buffer->capacity;
        uint32_t origPos                        = m_position;
        MMgc::GCObject* origCopyOnWriteOwner    = m_copyOnWriteOwner;
        if (!origLen) // empty buffer should give empty result
            return;

        // we need to create a new scratch buffer that we will
        // swap with previous one when the compression operation is
        // completed, so that we don't disturb any other workers
        // that are referencing this one.
        // This is done to avoid a long safepoint task as all other
        // workers must be halted during a safepoint.
        const bool cShared = IsShared();		// ByteArray's sharedness is immutable implicitly for the duration of this op since this worker is doing this op and cannot share it and it is not already shared (e.g. placed in a MessaegChannel).        
		FixedHeapRef<Buffer> origBuffer = m_buffer;
        if (cShared) {
            m_buffer = mmfx_new(Buffer());
        }

        m_buffer->array    = NULL;
        m_buffer->length   = 0;
        m_buffer->capacity = 0;
        m_position         = 0;
        m_copyOnWriteOwner = NULL;

        // Unlike zlib, lzma does not provide a method for computing
        // any upper bound on "compressed" size.  So we guess, and
        // retry if the guess was too aggressive.  The retry is only
        // needed if the data was incompressible; we compress anyhow
        // for compatibility with the lzma v1 format (the v1 headers
        // do not have a way to flag an uncompressed payload).

        uint32_t newCap = umax(origCap, lzmaHeaderSize);
        size_t lzmaPropsSize = LZMA_PROPS_SIZE;
        int retcode;
        struct lzma_compressed *destOverlay;
        size_t destLen;

    retry_compress:
        Exception *exn;
        bool ensured = EnsureCapacityOrFail(newCap, kCatchAction_Rethrow, &exn);
        if (!ensured)
        {
			// clean up when the EnsureCapacity call fails.
            if (cShared) {
                m_buffer = origBuffer;
            }

            m_buffer->array    = origData;
            m_buffer->length   = origLen;
            m_buffer->capacity = origCap;
            m_position         = origPos;
            SetCopyOnWriteOwner(origCopyOnWriteOwner);

            m_toplevel->core()->throwException(exn);
        }

        destOverlay = (struct lzma_compressed*) m_buffer->array;
        destLen = m_buffer->capacity - lzmaHeaderSize;

        // if this bytearray's data is being shared then we have to
        // snap shot it before we run any compression algorithm otherwise
        // the data could be changed in a way that may cause an exploit
        uint8_t* dataSnapshot = origData;
        if (cShared) {
            dataSnapshot = mmfx_new_array(uint8_t, origLen);
            VMPI_memcpy(dataSnapshot, origData, origLen);
        }

        retcode = LzmaCompress(destOverlay->compressedPayload, &destLen,
                               dataSnapshot, origLen,
                               destOverlay->lzmaProps, &lzmaPropsSize,
                               9, // -1 would yield default level (5),
                               1<<20, // 0 would yield default dictSize (1<<24)
                               -1,  // default lc (3),
                               -1,  // default lp (0),
                               -1,  // default pb (2),
                               -1,  // default fb (32),
                               1); // -1 would yield default numThreads (2)

        if (cShared) {
            mmfx_delete_array(dataSnapshot);
        }

        switch (retcode) {
        case SZ_OK:
            if (destLen > (m_buffer->capacity - lzmaHeaderSize)) {
                AvmAssertMsg(false, "LZMA broke its contract.");

                // Belt-and-suspenders: If control gets here,
                // something is terribly wrong, and LZMA is lying to
                // us.  Rather than risk establishing a bogus structure,
                // fail as if lzma returned an error code.
                goto error_cases;
            }

            destOverlay->unpackedSize[0] = (uint8_t)((origLen)       & 0xFF);
            destOverlay->unpackedSize[1] = (uint8_t)((origLen >> 8)  & 0xFF);
            destOverlay->unpackedSize[2] = (uint8_t)((origLen >> 16) & 0xFF);
            destOverlay->unpackedSize[3] = (uint8_t)((origLen >> 24) & 0xFF);

            AvmAssert(destOverlay->unpackedSizeHighBits[0] == 0
                      && destOverlay->unpackedSizeHighBits[1] == 0
                      && destOverlay->unpackedSizeHighBits[2] == 0
                      && destOverlay->unpackedSizeHighBits[3] == 0);

            m_buffer->length = uint32_t(lzmaHeaderSize + destLen);
            break;
        case SZ_ERROR_OUTPUT_EOF:
            // Our guessed target length was not conservative enough.
            // Since this is a compression algorithm, go with linear
            // growth on failure (rather than e.g. exponential).
            newCap += origCap;

            goto retry_compress;
        case SZ_ERROR_MEM:
        case SZ_ERROR_PARAM:
        case SZ_ERROR_THREAD:
        default:
        error_cases:
            if (cShared) {
                m_buffer = origBuffer;
            }
            // On other failures, just give up.

            // Even though we set length to 0 (effectively clearing
            // the state), we set array back to origData so that its
            // memory will be properly managed.
            m_buffer->array    = origData;
            m_buffer->length   = 0;
            m_buffer->capacity = origCap;
            break;
        }

        // Analogous to zlib, maintain policy that Compress() sets
        // position == length (while Uncompress() sets position == 0).
        m_position = m_buffer->length;

        if (cShared) {
            ByteArraySwapBufferTask task(this, origBuffer);
            task.exec();
        }

        if (origData && origData != m_buffer->array && origCopyOnWriteOwner == NULL)
        {
            // Note that TellGcXXX always expects capacity, not (logical) length.
            TellGcDeleteBufferMemory(origData, origCap);
            mmfx_delete_array(origData);
        }
    }

    void ByteArray::CompressViaZlibVariant(CompressionAlgorithm algorithm)
    {
        // Snarf the data and give ourself some empty data
        // (remember, existing data might be copy-on-write so don't dance on it)
        uint8_t* origData                       = m_buffer->array;
        uint32_t origLen                        = m_buffer->length;
        uint32_t origCap                        = m_buffer->capacity;
		uint32_t origPos						= m_position;
        MMgc::GCObject* origCopyOnWriteOwner    = m_copyOnWriteOwner;
        if (!origLen) // empty buffer should give empty result
            return;

        const bool cShared = IsShared();		// ByteArray's sharedness is immutable implicitly for the duration of this op since this worker is doing this op and cannot share it and it is not already shared (e.g. placed in a MessaegChannel).
        // if this bytearray's data is being shared then we have to
        // snap shot it before we run any compression algorithm otherwise
        // the data could be changed in a way that may cause an exploit
        uint8_t* dataSnapshot = origData;
        FixedHeapRef<Buffer> origBuffer = m_buffer;
        if (cShared) {
            dataSnapshot = mmfx_new_array(uint8_t, origLen);
            VMPI_memcpy(dataSnapshot, origData, origLen);
            m_buffer = mmfx_new(Buffer());
        }

        m_buffer->array    = NULL;
        m_buffer->length   = 0;
        m_buffer->capacity = 0;
        m_position         = 0;
        m_copyOnWriteOwner = NULL;

        int error = Z_OK;
        
        // Use zlib to compress the data. This next block is essentially the
        // implementation of the compress2() method, but modified to pass a
        // negative window value (-15) to deflateInit2() for k_deflate mode
        // in order to obtain deflate-only compression (no ZLib headers).

        const int MAX_WINDOW_RAW_DEFLATE = -15;
        const int DEFAULT_MEMORY_USE = 8;

        z_stream stream;
        VMPI_memset(&stream, 0, sizeof(stream));
        error = deflateInit2(&stream,
                                Z_BEST_COMPRESSION,
                                Z_DEFLATED,
                                algorithm == k_zlib ? MAX_WBITS : MAX_WINDOW_RAW_DEFLATE,
                                DEFAULT_MEMORY_USE,
                                Z_DEFAULT_STRATEGY);
        AvmAssert(error == Z_OK);

        uint32_t newCap = deflateBound(&stream, origLen);
		Exception *exn;
        bool ensured =
			EnsureCapacityOrFail(newCap, kCatchAction_Rethrow, &exn);
        if (!ensured)
        {
			// clean up when the EnsureCapacity call fails.
            if (cShared) {
                m_buffer = origBuffer;
				mmfx_delete_array(dataSnapshot);
            }
			else 
			{
				m_buffer->array    = origData;
				m_buffer->length   = origLen;
				m_buffer->capacity = origCap;
				m_position         = origPos;
				SetCopyOnWriteOwner(origCopyOnWriteOwner);
			}
			
            m_toplevel->core()->throwException(exn);
        }
		
        stream.next_in = dataSnapshot;
        stream.avail_in = origLen;
        stream.next_out = m_buffer->array;
        stream.avail_out = m_buffer->capacity;

        error = deflate(&stream, Z_FINISH);
        AvmAssert(error == Z_STREAM_END);

        m_buffer->length = stream.total_out;
        AvmAssert(m_buffer->length <= m_buffer->capacity);

        // Note that Compress() has always ended with position == length,
        // but Uncompress() has always ended with position == 0.
        // Weird, but we must maintain it.
        m_position = m_buffer->length;

        deflateEnd(&stream);

        if (cShared)
        {
            mmfx_delete_array(dataSnapshot);
            ByteArraySwapBufferTask task(this, origBuffer);
            task.exec();
        }
        // Note: the Compress() method has never reported an error for corrupted data,
        // so we won't start now. (Doing so would probably require a version check,
        // to avoid breaking content that relies on misbehavior.)
        if (origData && origData != m_buffer->array && origCopyOnWriteOwner == NULL)
        {
            // Note that TellGcXXX always expects capacity, not (logical) length.
            TellGcDeleteBufferMemory(origData, origCap);
            mmfx_delete_array(origData);
        }
    }

    void ByteArray::Uncompress(CompressionAlgorithm algorithm)
    {
        switch (algorithm) {
        case k_lzma:
            UncompressViaLzma();
            break;
        case k_zlib:
        default:
            UncompressViaZlibVariant(algorithm);
            break;
        }
    }

    void ByteArray::UncompressViaLzma()
    {
        // Snarf the data and give ourself some empty data
        // (remember, existing data might be copy-on-write so don't dance on it)
        uint8_t* origData                       = m_buffer->array;
        uint32_t origCap                        = m_buffer->capacity;
        uint32_t origLen                        = m_buffer->length;
        uint32_t origPos                        = m_position;
        MMgc::GCObject* origCopyOnWriteOwner    = m_copyOnWriteOwner;

        if (!origLen) // empty buffer should give empty result
            return;

        if (!m_buffer->array || m_buffer->length < lzmaHeaderSize)
            return;

        // if this bytearray's data is being shared then we have to
        // snap shot it before we run any compression algorithm otherwise
        // the data could be changed in a way that may cause an exploit
        uint8_t* dataSnapshot = origData;
        const bool cShared = IsShared();		// ByteArray's sharedness is immutable implicitly for the duration of this op since this worker is doing this op and cannot share it and it is not already shared (e.g. placed in a MessaegChannel).
        if (cShared) {
            dataSnapshot = mmfx_new_array(uint8_t, origLen);
            VMPI_memcpy(dataSnapshot, origData, origLen);
        }

        struct lzma_compressed *srcOverlay;
        srcOverlay = (struct lzma_compressed*)dataSnapshot;

        uint32_t unpackedLen;
        unpackedLen  =  (uint32_t)srcOverlay->unpackedSize[0];
        unpackedLen +=  (uint32_t)srcOverlay->unpackedSize[1] << 8;
        unpackedLen +=  (uint32_t)srcOverlay->unpackedSize[2] << 16;
        unpackedLen +=  (uint32_t)srcOverlay->unpackedSize[3] << 24;

        // check that size is reasonable before modifying internal structure.
        if (srcOverlay->unpackedSizeHighBits[0] != 0 ||
            srcOverlay->unpackedSizeHighBits[1] != 0 ||
            srcOverlay->unpackedSizeHighBits[2] != 0 ||
            srcOverlay->unpackedSizeHighBits[3] != 0)
        {
            if (cShared) {
                mmfx_delete_array(dataSnapshot);
            }
            // We can't allocate a byte array of such large size.
            ThrowMemoryError();
        }

        size_t srcLen = (origLen - lzmaHeaderSize);

        FixedHeapRef<Buffer> origBuffer = m_buffer;
        if (cShared) {
            m_buffer = mmfx_new(Buffer());
        }

        m_buffer->array    = NULL;
        m_buffer->length   = 0;
        m_buffer->capacity = 0;
        m_position         = 0;
        m_copyOnWriteOwner = NULL;

        // Since we rely on unpackedLen being correct, we do not need
        // to loop with different trial lengths; either it works on
        // first try, or it will always fail.
        Exception *exn;
        bool ensured =
            EnsureCapacityOrFail(unpackedLen, kCatchAction_Rethrow, &exn);
        if (!ensured)
        {
			// clean up when the EnsureCapacity call fails.
            if (cShared) {
                m_buffer = origBuffer;
				mmfx_delete_array(dataSnapshot);
            }

            // (keep in sync with state restoration in error_cases: labelled below)
            m_buffer->array    = origData;
            m_buffer->length   = origLen;
            m_buffer->capacity = origCap;
            m_position         = origPos;
            SetCopyOnWriteOwner(origCopyOnWriteOwner);
            origBuffer = NULL; // release ref before throwing
            m_toplevel->core()->throwException(exn);
        }

        int retcode = SZ_OK;
        size_t destLen = unpackedLen;

        retcode = LzmaUncompress(m_buffer->array, &destLen,
                                 srcOverlay->compressedPayload, &srcLen,
                                 srcOverlay->lzmaProps, LZMA_PROPS_SIZE);

        if (cShared) {
            mmfx_delete_array(dataSnapshot);
        }

        switch (retcode) {
        case SZ_OK:                // - OK
            if (destLen != unpackedLen) {
                // Belt-and-suspenders: If control gets here,
                // something is terribly wrong, and either LZMA is
                // lying, or the lzma header in source byte array got
                // garbled.  Rather than risk establishing a bogus
                // structure, fail as if lzma returned an error code.
                goto error_cases;
            }

            m_buffer->length = uint32_t(destLen);

            if (cShared) {
                ByteArraySwapBufferTask task(this, origBuffer);
                task.exec();
            }

            if (origData && origData != m_buffer->array && origCopyOnWriteOwner == NULL)
            {
                // Note that TellGcXXX always expects capacity, not (logical) length.
                TellGcDeleteBufferMemory(origData, origCap);
                mmfx_delete_array(origData);
            }

            break;

        case SZ_ERROR_DATA:        // - Data error
        case SZ_ERROR_MEM:         // - Memory allocation arror
        case SZ_ERROR_UNSUPPORTED: // - Unsupported properties
        case SZ_ERROR_INPUT_EOF:   // - it needs more bytes in input buffer (src)
        default:
        error_cases:
            // In error cases:
            if (cShared) {
                m_buffer = origBuffer;
            }

            // 1) free the new buffer
            TellGcDeleteBufferMemory(m_buffer->array, m_buffer->capacity);
            mmfx_delete_array(m_buffer->array);

            // 2) put the original data back.
            // (keep in sync with state restoration above)
            m_buffer->array    = origData;
            m_buffer->length   = origLen;
            m_buffer->capacity = origCap;
            m_position         = origPos;
            SetCopyOnWriteOwner(origCopyOnWriteOwner);
            origBuffer = NULL; // release ref before throwing
            toplevel()->throwIOError(kCompressedDataError);
            break;
        }

    }

    void ByteArray::UncompressViaZlibVariant(CompressionAlgorithm algorithm)
    {
        // Snarf the data and give ourself some empty data
        // (remember, existing data might be copy-on-write so don't dance on it)
        uint8_t* origData                       = m_buffer->array;
        uint32_t origCap                        = m_buffer->capacity;
        uint32_t origLen                        = m_buffer->length;
        uint32_t origPos                        = m_position;
        MMgc::GCObject* origCopyOnWriteOwner    = m_copyOnWriteOwner;
        if (!origLen) // empty buffer should give empty result
            return;

        const bool cShared = IsShared();		// ByteArray's sharedness is immutable implicitly for the duration of this op since this worker is doing this op and cannot share it and it is not already shared (e.g. placed in a MessaegChannel).
        FixedHeapRef<Buffer> origBuffer = m_buffer;
        m_buffer->array    = NULL;
        m_buffer->length   = 0;
        m_buffer->capacity = 0;
        m_position         = 0;
        m_copyOnWriteOwner = NULL;
		
		int error = Z_OK;
		uint8_t* scratch = NULL;
        uint8_t* dataSnapshot = NULL;
        
		TRY(m_toplevel->core(), kCatchAction_Rethrow)
		{
            // if this bytearray's data is being shared then we have to
            // snap shot it before we run any compression algorithm otherwise
            // the data could be changed in a way that may cause an exploit
            dataSnapshot = origData;
            if (cShared) {
                m_buffer = mmfx_new(Buffer());
                dataSnapshot = mmfx_new_array(uint8_t, origLen);
                VMPI_memcpy(dataSnapshot, origData, origLen);
            }
            
			// we know that the uncompressed data will be at least as
			// large as the compressed data, so let's start there,
			// rather than at zero.
			EnsureCapacity(origCap);

			const uint32_t kScratchSize = 8192;
			scratch = mmfx_new_array(uint8_t, kScratchSize);
			
			z_stream stream;
			VMPI_memset(&stream, 0, sizeof(stream));
			error = inflateInit2(&stream, algorithm == k_zlib ? 15 : -15);
			AvmAssert(error == Z_OK);

			stream.next_in = dataSnapshot;
			stream.avail_in = origLen;
			while (error == Z_OK)
			{
				stream.next_out = scratch;
				stream.avail_out = kScratchSize;
				error = inflate(&stream, Z_NO_FLUSH);
				Write(scratch, kScratchSize - stream.avail_out);
			}

			inflateEnd(&stream);
            
            mmfx_delete_array(scratch);
            
            if (cShared) {
                mmfx_delete_array(dataSnapshot);
            }
  		}
		CATCH(Exception* e)
		{
			mmfx_delete_array(scratch);
			
			if (cShared) {
				mmfx_delete_array(dataSnapshot);
			}

			m_toplevel->core()->throwException(e);
		}
		END_CATCH;
		END_TRY;
		

        if (error == Z_STREAM_END)
        {
            if (cShared) {
                ByteArraySwapBufferTask task(this, origBuffer);
                task.exec();
            }
            // everything is cool
            if (origData && origData != m_buffer->array && origCopyOnWriteOwner == NULL)
            {
                // Note that TellGcXXX always expects capacity, not (logical) length.
                TellGcDeleteBufferMemory(origData, origCap);
                mmfx_delete_array(origData);
            }

            // Note that Compress() has always ended with position == length,
            // but Uncompress() has always ended with position == 0.
            // Weird, but we must maintain it.
            m_position = 0;
        }
        else
        {
            // When we error:

            // 1) free the new buffer
            TellGcDeleteBufferMemory(m_buffer->array, m_buffer->capacity);
            mmfx_delete_array(m_buffer->array);

            if (cShared) {
                m_buffer = origBuffer;
            }

            // 2) put the original data back.
            m_buffer->array    = origData;
            m_buffer->length   = origLen;
            m_buffer->capacity = origCap;
            m_position         = origPos;
            SetCopyOnWriteOwner(origCopyOnWriteOwner);
            origBuffer = NULL; // release ref before throwing
            toplevel()->throwIOError(kCompressedDataError);
        }
    }

    // For requestBytesForShortRead() there is no limit on m_position, but m_length 
    // is limited to MAX_BYTEARRAY_STORE_LENGTH, which is well below 2^32.  We limit 
    // nbytes to MAX_BYTEARRAY_SHORT_ACCESS_LENGTH, which is less than
    // 2^32-MAX_BYTEARRAY_STORE_LENGTH but at least 4095.  Callers that might have a
    // larger value for nbytes should not use this API.  The purpose of all these limits
    // is to make the range check tractably small for inlining in jitted code without
    // limiting the ByteArray size unreasonably.  (Requiring m_length < 2^31 would lead 
    // to further optimizations but that seems unreasonably short.)
    //
    // Observe that ByteArray::Grower::ReallocBackingStore() implements the appropriate
    // limit on m_capacity, and that is the only code that allocates memory for ByteArray.
    // Everywhere else we ensure m_length <= m_capacity.

    REALLY_INLINE uint8_t* ByteArray::requestBytesForShortRead(uint32_t nbytes)
    {
        AvmAssert(m_buffer->length <= m_buffer->capacity);
        AvmAssert(m_buffer->capacity <= MAX_BYTEARRAY_STORE_LENGTH);
        AvmAssert(nbytes > 0 && nbytes <= MAX_BYTEARRAY_SHORT_ACCESS_LENGTH);

        // m_position + nbytes does not overflow uint32_t in the second disjunct because:
        //
        //   m_position < m_length <= m_capacity <= MAX_BYTEARRAY_STORE_LENGTH      (by the first disjunct + global invariants)
        //   nbytes <= MAX_BYTEARRAY_SHORT_ACCESS_LENGTH                            (global invariant)
        //   MAX_BYTEARRAY_STORE_LENGTH + MAX_BYTEARRAY_SHORT_ACCESS_LENGTH < 2^32  (global invariant)

        if (m_position >= m_buffer->length || m_position + nbytes > m_buffer->length)
            ThrowEOFError();      // Does not return
        uint8_t *b = m_buffer->array + m_position;
        m_position += nbytes;
        return b;
    }

    // Same argument as for requestBytesForShortRead(), above.
    
    REALLY_INLINE uint8_t* ByteArray::requestBytesForShortWrite(uint32_t nbytes)
    {
        AvmAssert(nbytes > 0 && nbytes <= MAX_BYTEARRAY_SHORT_ACCESS_LENGTH);
    
        // m_position + nbytes does not overflow uint32_t in the second disjunct because:
        //
        //   m_position < m_length <= m_capacity <= MAX_BYTEARRAY_STORE_LENGTH     (by the first disjunct + global invariants)
        //   nbytes <= MAX_BYTEARRAY_SHORT_ACCESS_LENGTH                           (global invariant)
        //   MAX_BYTEARRAY_STORE_LENGTH + MAX_BYTEARRAY_SHORT_ACCESS_LENGTH < 2^32 (global invariant)

        if (m_position >= m_buffer->length || m_position + nbytes > m_buffer->length)
            SetLength(m_position, nbytes);  // The addition would *not* be safe against overflow here
    
        AvmAssert(m_buffer->length <= m_buffer->capacity);
        AvmAssert(m_buffer->capacity <= MAX_BYTEARRAY_STORE_LENGTH);
        AvmAssert(m_buffer->length >= nbytes && m_position <= m_buffer->length - nbytes);

        uint8_t *b = m_buffer->array + m_position;
        m_position += nbytes;
        return b;
    }

    int32_t ByteArray::CAS(uint32_t index, int32_t expected, int32_t next)
    {
		if (m_buffer->length == 0)
			m_toplevel->throwRangeError(kInvalidRangeError);
        if (index > (m_buffer->length - sizeof(int32_t))) // Handle the race. 
            m_toplevel->throwRangeError(kInvalidRangeError);
        if (index % sizeof(expected) != 0) // require word alignment
            m_toplevel->throwRangeError(kInvalidRangeError);
        uint8_t* wordptr = &m_buffer->array[index];
        return vmbase::AtomicOps::compareAndSwap32WithBarrierPrev(expected, next, (int32_t*)wordptr);
    }

    bool ByteArray::isShareable () const
    {
        return m_isShareable;
    }

    bool ByteArray::setShareable (bool value)
    {
        if (m_isShareable == value)
            return false; // no change.
        if (m_isShareable == true && value == false)
            return false; // FIXME can't 'un-share' yet. No change

        m_isShareable = value;
        return true;  // changed.
    }
    
    
    //
    // ByteArrayObject
    //
    
    ByteArrayObject::ByteArrayObject(VTable* ivtable, ScriptObject* delegate)
        : ScriptObject(ivtable, delegate)
        , m_byteArray(toplevel())
    {
        c.set(&m_byteArray, sizeof(ByteArray));
        ByteArrayClass* cls = toplevel()->byteArrayClass();
        m_byteArray.SetObjectEncoding((ObjectEncoding)cls->get_defaultObjectEncoding());
        toplevel()->byteArrayCreated(this);
    }

    ByteArrayObject::ByteArrayObject(VTable* ivtable, ScriptObject* delegate, const ByteArray& source)
        : ScriptObject(ivtable, delegate)
        , m_byteArray(toplevel(), source)
    {
        c.set(&m_byteArray, sizeof(ByteArray));
        toplevel()->byteArrayCreated(this);
    }


    ByteArrayObject::ByteArrayObject(VTable* ivtable, ScriptObject* delegate, ByteArray::Buffer* source)
        : ScriptObject(ivtable, delegate)
        , m_byteArray(toplevel(), source, false)
    {
        c.set(&m_byteArray, sizeof(ByteArray));
        toplevel()->byteArrayCreated(this);
    }


    // Inspection of the object code (GCC 4.2) shows that the forced inlining of GetLength
    // and operator[] lead to optimal code here: no redundant range check is being executed
    // even if there is a redundant check in operator[].
    Atom ByteArrayObject::getUintProperty(uint32_t i) const
    {
        if (i < m_byteArray.GetLength())
        {
            intptr_t const b = m_byteArray[i];
            return atomFromIntptrValue(b);
        }
        else
        {
            return undefinedAtom;
        }
    }
    
    bool ByteArrayObject::hasUintProperty(uint32_t i) const
    {
        return i < m_byteArray.GetLength();
    }
    
    void ByteArrayObject::setUintProperty(uint32_t i, Atom value)
    {
        m_byteArray[i] = uint8_t(AvmCore::integer(value));
    }
    
    Atom ByteArrayObject::getAtomProperty(Atom name) const
    {
        uint32_t index;
        if (AvmCore::getIndexFromAtom(name, &index))
        {
            return getUintProperty(index);
        }

        return ScriptObject::getAtomProperty(name);
    }
    
    void ByteArrayObject::setAtomProperty(Atom name, Atom value)
    {
        uint32_t index;
        if (AvmCore::getIndexFromAtom(name, &index))
        {
            setUintProperty(index, value);
        }
        else
        {
            ScriptObject::setAtomProperty(name, value);
        }
    }
    
    bool ByteArrayObject::hasAtomProperty(Atom name) const
    {
        if (core()->currentBugCompatibility()->bugzilla558863)
        {
            uint32_t index;
            if (AvmCore::getIndexFromAtom(name, &index))
            {
                return index < m_byteArray.GetLength();
            }

            return ScriptObject::hasAtomProperty(name);
        }
        else
        {
            return ScriptObject::hasAtomProperty(name)
                || getAtomProperty(name) != undefinedAtom;
        }
    }

    Atom ByteArrayObject::getMultinameProperty(const Multiname* name) const
    {
        uint32_t index;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=664005
        if (name->isValidDynamicName() && name->getName()->parseIndex(index))
#else
        if (name->getName()->parseIndex(index))
#endif
        {
            return getUintProperty(index);
        }

        return ScriptObject::getMultinameProperty(name);
    }

    void ByteArrayObject::setMultinameProperty(const Multiname* name, Atom value)
    {
        uint32_t index;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=664005
        if (name->isValidDynamicName() && name->getName()->parseIndex(index))
#else
        if (name->getName()->parseIndex(index))
#endif
        {
            setUintProperty(index, value);
        }
        else
        {
            ScriptObject::setMultinameProperty(name, value);
        }
    }

    bool ByteArrayObject::hasMultinameProperty(const Multiname* name) const
    {
        uint32_t index;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=664005
        if (name->isValidDynamicName() && name->getName()->parseIndex(index))
#else
        if (name->getName()->parseIndex(index))
#endif
        {
            return index < m_byteArray.GetLength();
        }

        return ScriptObject::hasMultinameProperty(name);
    }

    String* ByteArrayObject::_toString()
    {
        uint32_t len = m_byteArray.GetLength();
        const uint8_t* c = m_byteArray.GetReadableBuffer();

        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        if (len >= 3)
        {
            // UTF8 BOM
            if ((c[0] == 0xef) && (c[1] == 0xbb) && (c[2] == 0xbf))
            {
                return core->newStringUTF8((const char*)c + 3, len - 3);
            }
            else if ((c[0] == 0xfe) && (c[1] == 0xff))
            {
                //UTF-16 big endian
                c += 2;
                len = (len - 2) >> 1;
                return core->newStringEndianUTF16(/*littleEndian*/false, (const wchar*)c, len);
            }
            else if ((c[0] == 0xff) && (c[1] == 0xfe))
            {
                //UTF-16 little endian
                c += 2;
                len = (len - 2) >> 1;
                return core->newStringEndianUTF16(/*littleEndian*/true, (const wchar*)c, len);
            }
        }
#ifndef MARK_SECURITY_CHANGE  // http://watsonexp.corp.adobe.com/#bug=3350293
        uint8_t* buffer = NULL;

        if (c && len > 0 && c[len-1] != 0)
        {
            buffer = mmfx_new_array(uint8_t, len+1);
            VMPI_memcpy(buffer, c, len);
            buffer[len] = 0;
        }

        String* result = toplevel->tryFromSystemCodepage(buffer ? buffer : c);

        if (buffer)
            mmfx_delete_array(buffer);
#else
		String* result = toplevel->tryFromSystemCodepage(c);
#endif		
        if (result != NULL)
            return result;
        
        // Use newStringUTF8() with "strict" explicitly set to false to mimick old,
        // buggy behavior, where malformed UTF-8 sequences are stored as single characters.
        return core->newStringUTF8((const char*)c, len, false);
    }
    
    Atom ByteArrayObject::readObject()
    {
        return m_byteArray.ReadObject();
    }

    void ByteArrayObject::writeObject(Atom value)
    {
        m_byteArray.WriteObject(value);
    }
    
    int ByteArrayObject::readByte()
    {
        return (int8_t)*m_byteArray.requestBytesForShortRead(1);
    }

    int ByteArrayObject::readUnsignedByte()
    {
        return *m_byteArray.requestBytesForShortRead(1);
    }

    REALLY_INLINE uint32_t ByteArrayObject::read16()
    {
        uint8_t *b = m_byteArray.requestBytesForShortRead(2);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian()) {  // GetNativeEndian expands to a constant
#if defined VMCFG_UNALIGNED_INT_ACCESS
            return *(uint16_t*)b;
#elif defined VMCFG_BIG_ENDIAN
            return ((uint32_t)b[0] << 8) | (uint32_t)b[1]; // read big-endian
#else
            return ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP16
            return byteSwap16(*(uint16_t*)b);
#elif defined VMCFG_BIG_ENDIAN
            return ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
#else
            return ((uint32_t)b[0] << 8) | (uint32_t)b[1]; // read big-endian
#endif
        }
    }
    
    int ByteArrayObject::readShort()
    {
        return (int)(int16_t)read16();
    }

    int ByteArrayObject::readUnsignedShort()
    {
        return (int)read16();
    }

    REALLY_INLINE uint32_t ByteArrayObject::read32()
    {
        uint8_t* b = m_byteArray.requestBytesForShortRead(4);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())  // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS
            return *(uint32_t*)b;
#elif defined VMCFG_BIG_ENDIAN
            return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; // read big-endian
#else
            return ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            return byteSwap32(*(uint32_t*)b);
#elif defined VMCFG_BIG_ENDIAN
            return ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
#else
            return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; // read big-endian
#endif
        }
    }

    int ByteArrayObject::readInt()
    {
        return (int)read32();
    }

    uint32_t ByteArrayObject::readUnsignedInt()
    {
        return read32();
    }

    // Some observations from x86 testing of float and double reading:
    //
    //  - It pays to make use of VMCFG_UNALIGNED_FP_ACCESS.
    //  - It pays to assemble an int32 in a register and then store it in
    //    a 32-bit field of the union, that is then read (either alone for
    //    float or as part of a pair for double), rather than shuffling 
    //    bytes individually into a byte array in the union.

    // Bugzilla 569691/685441: Do not try to be clever here by loading from
    // '*(uint32_t*)b' into 'u.word', even if VMCFG_UNALIGNED_INT_ACCESS and
    // on native endianness - gcc may emit code that loads directly
    // to the ARM VFP register, and that requires VMCFG_UNALIGNED_FP_ACCESS.

    REALLY_INLINE float ByteArrayObject::readFloatHelper(uint8_t* b)
    {
        union {
            uint32_t word;
            float    fval;
        } u;
        
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())  // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_FP_ACCESS
            return *(float*)b;
#elif defined VMCFG_BIG_ENDIAN
            u.word = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; // read big-endian
            return u.fval;
#else
            u.word = ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
            return u.fval;
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            u.word = byteSwap32(*(uint32_t*)b);
#elif defined VMCFG_BIG_ENDIAN
            u.word = ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read little-endian
#else
            u.word = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; // read big-endian
#endif
            return u.fval;
        }    }
    
    double ByteArrayObject::readFloat()
    {
        return readFloatHelper(m_byteArray.requestBytesForShortRead(4));
    }

#ifdef VMCFG_FLOAT
    // There will be four endianness checks here but one hopes the compiler will
    // common them after inlining.
    void ByteArrayObject::readFloat4(float4_t* retval)
    {
        uint8_t *b = m_byteArray.requestBytesForShortRead(16);
        float x = readFloatHelper(b);
        float y = readFloatHelper(b+4);
        float z = readFloatHelper(b+8);
        float w = readFloatHelper(b+12);
        float4_t v = { x, y, z, w };
        *retval = v;
    }
#endif

    // Bugzilla 569691/685441: Do not try to be clever here by loading from
    // '*(uint32_t*)b' into 'u.word[i]', even if VMCFG_UNALIGNED_INT_ACCESS and
    // on native endianness - gcc may emit code that loads directly
    // to the ARM VFP register, and that requires VMCFG_UNALIGNED_FP_ACCESS.
    
    double ByteArrayObject::readDouble()
    {
        // Handle reversed word order for doubles
#if defined VMCFG_DOUBLE_MSW_FIRST
        const int first = 1;
        const int second = 0;
#else
        const int first = 0;
        const int second = 1;
#endif
        union {
            uint32_t words[2];
            double   dval;
        } u;
        uint8_t *b = m_byteArray.requestBytesForShortRead(8);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())  // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_FP_ACCESS
            return *(double*)b;
#elif defined VMCFG_BIG_ENDIAN
            u.words[first] = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; // read
            u.words[second] = ((uint32_t)b[4] << 24) | ((uint32_t)b[5] << 16) | ((uint32_t)b[6] << 8) | (uint32_t)b[7]; //   big-endian
            return u.dval;
#else
            u.words[first] = ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; // read
            u.words[second] = ((uint32_t)b[7] << 24) | ((uint32_t)b[6] << 16) | ((uint32_t)b[5] << 8) | (uint32_t)b[4]; //   little-endian
            return u.dval;
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            u.words[first] = byteSwap32(*(uint32_t*)(b+4));
            u.words[second] = byteSwap32(*(uint32_t*)b);
#elif defined VMCFG_BIG_ENDIAN
            u.words[first] = ((uint32_t)b[7] << 24) | ((uint32_t)b[6] << 16) | ((uint32_t)b[5] << 8) | (uint32_t)b[4]; // read
            u.words[second] = ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[0]; //   little-endian
#else
            u.words[first] = ((uint32_t)b[4] << 24) | ((uint32_t)b[5] << 16) | ((uint32_t)b[6] << 8) | (uint32_t)b[7]; // read
            u.words[second] = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; //   big-endian
#endif
            return u.dval;
        }
    }

    bool ByteArrayObject::readBoolean()
    {
        return (*m_byteArray.requestBytesForShortRead(1)) != 0;
    }

    void ByteArrayObject::writeBoolean(bool value)
    {
        m_byteArray.requestBytesForShortWrite(1)[0] = (value ? 1 : 0);
    }

    void ByteArrayObject::writeByte(int value)
    {
        m_byteArray.requestBytesForShortWrite(1)[0] = (uint8_t)value;
    }

    void ByteArrayObject::writeShort(int value)
    {
        uint8_t *b = m_byteArray.requestBytesForShortWrite(2);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())   // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS
            *(int16_t*)b = (int16_t)value;
#elif defined VMCFG_BIG_ENDIAN
            b[0] = (uint8_t)(value >> 8);  // write
            b[1] = (uint8_t)value;         //   big-endian
#else
            b[1] = (uint8_t)(value >> 8);  // write
            b[0] = (uint8_t)value;         //   little-endian
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP16
            *(int16_t*)b = byteSwap16((uint16_t)value);
#elif defined VMCFG_BIG_ENDIAN
            b[1] = (uint8_t)(value >> 8);  // write
            b[0] = (uint8_t)value;         //   little-endian
#else
            b[0] = (uint8_t)(value >> 8);  // write
            b[1] = (uint8_t)value;         //   big-endian
#endif
        }
    }

    REALLY_INLINE void ByteArrayObject::write32(uint32_t value)
    {
        uint8_t *b = m_byteArray.requestBytesForShortWrite(4);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())   // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS
            *(uint32_t*)b = value;
#elif defined VMCFG_BIG_ENDIAN
            b[0] = (uint8_t)(value >> 24);   // write
            b[1] = (uint8_t)(value >> 16);   //   big
            b[2] = (uint8_t)(value >> 8);    //     endian
            b[3] = (uint8_t)value;
#else
            b[3] = (uint8_t)(value >> 24);   // write
            b[2] = (uint8_t)(value >> 16);   //   little
            b[1] = (uint8_t)(value >> 8);    //     endian
            b[0] = (uint8_t)value;
#endif
        }
        else
        {
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            *(uint32_t*)b = byteSwap32(value);
#elif defined VMCFG_BIG_ENDIAN
            b[3] = (uint8_t)(value >> 24);   // write
            b[2] = (uint8_t)(value >> 16);   //   little
            b[1] = (uint8_t)(value >> 8);    //     endian
            b[0] = (uint8_t)value;
#else
            b[0] = (uint8_t)(value >> 24);   // write
            b[1] = (uint8_t)(value >> 16);   //   big
            b[2] = (uint8_t)(value >> 8);    //     endian
            b[3] = (uint8_t)value;
#endif
        }
    }

    void ByteArrayObject::writeInt(int value)
    {
        write32((uint32_t)value);
    }

    void ByteArrayObject::writeUnsignedInt(uint32_t value)
    {
        write32(value);
    }
    
    // Bugzilla 569691/685441: Do not try to be clever here by storing from
    // 'u.word' into '*(uint32_t*)b', even if both VMCFG_UNALIGNED_INT_ACCESS and
    // on native endianness - gcc will emit code that stores directly
    // from the ARM VFP register, and that requires VMCFG_UNALIGNED_FP_ACCESS.

    REALLY_INLINE void ByteArrayObject::writeFloatHelper(float value, uint8_t* b)
    {
        union {
            uint32_t word;
            float    fval;
        } u;
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())   // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_FP_ACCESS
            *(float*)b = value;
#elif defined VMCFG_BIG_ENDIAN
            u.fval = value;
            uint32_t w = u.word;
            b[0] = (uint8_t)(w >> 24);   // write
            b[1] = (uint8_t)(w >> 16);   //   big
            b[2] = (uint8_t)(w >> 8);    //     endian
            b[3] = (uint8_t)w;
#else
            u.fval = value;
            uint32_t w = u.word;
            b[3] = (uint8_t)(w >> 24);   // write
            b[2] = (uint8_t)(w >> 16);   //   little
            b[1] = (uint8_t)(w >> 8);    //     endian
            b[0] = (uint8_t)w;
#endif
        }
        else
        {
            u.fval = value;
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            *(uint32_t*)b = byteSwap32(u.word);
#elif defined VMCFG_BIG_ENDIAN
            uint32_t w = u.word;
            b[3] = (uint8_t)(w >> 24);   // write
            b[2] = (uint8_t)(w >> 16);   //   little
            b[1] = (uint8_t)(w >> 8);    //     endian
            b[0] = (uint8_t)w;
#else
            uint32_t w = u.word;
            b[0] = (uint8_t)(w >> 24);   // write
            b[1] = (uint8_t)(w >> 16);   //   big
            b[2] = (uint8_t)(w >> 8);    //     endian
            b[3] = (uint8_t)w;
#endif
        }
    }

    void ByteArrayObject::writeFloat(double value)
    {
        writeFloatHelper((float)value, m_byteArray.requestBytesForShortWrite(4));
    }

#ifdef VMCFG_FLOAT
    // There will be four endianness checks here but one hopes the compiler will
    // common them after inlining.
    void ByteArrayObject::writeFloat4(const float4_t& v)
    {
        uint8_t* b = m_byteArray.requestBytesForShortWrite(16);
        writeFloatHelper(f4_x(v), b);
        writeFloatHelper(f4_y(v), b+4);
        writeFloatHelper(f4_z(v), b+8);
        writeFloatHelper(f4_w(v), b+12);
    }
#endif
    
    // Bugzilla 569691/685441: Do not try to be clever here by storing from
    // 'u.word[i]' into '*(uint32_t*)b', even if both VMCFG_UNALIGNED_INT_ACCESS and
    // on native endianness - gcc will emit code that stores directly
    // from the ARM VFP register, and that requires VMCFG_UNALIGNED_FP_ACCESS.
    
    void ByteArrayObject::writeDouble(double value)
    {
        // Handle reversed word order for doubles
#if defined VMCFG_DOUBLE_MSW_FIRST
        const int first = 1;
        const int second = 0;
#else
        const int first = 0;
        const int second = 1;
#endif
        union {
            uint32_t words[2];
            double   dval;
        } u;
        uint8_t *b = m_byteArray.requestBytesForShortWrite(8);
        if (m_byteArray.GetEndian() == m_byteArray.GetNativeEndian())   // GetNativeEndian expands to a constant
        {
#if defined VMCFG_UNALIGNED_FP_ACCESS
            *(double*)b = value;
#elif defined VMCFG_BIG_ENDIAN
            uint32_t w;
            u.dval = value;
            w = u.words[first];
            b[0] = (uint8_t)(w >> 24);   // write
            b[1] = (uint8_t)(w >> 16);   //   big
            b[2] = (uint8_t)(w >> 8);    //     endian
            b[3] = (uint8_t)w;
            w = u.words[second];
            b[4] = (uint8_t)(w >> 24);   // write
            b[5] = (uint8_t)(w >> 16);   //   big
            b[6] = (uint8_t)(w >> 8);    //     endian
            b[7] = (uint8_t)w;
#else
            uint32_t w;
            u.dval = value;
            w = u.words[first];
            b[3] = (uint8_t)(w >> 24);   // write
            b[2] = (uint8_t)(w >> 16);   //   little
            b[1] = (uint8_t)(w >> 8);    //     endian
            b[0] = (uint8_t)w;
            w = u.words[second];
            b[7] = (uint8_t)(w >> 24);   // write
            b[6] = (uint8_t)(w >> 16);   //   little
            b[5] = (uint8_t)(w >> 8);    //     endian
            b[4] = (uint8_t)w;
#endif
        }
        else
        {
            u.dval = value;
#if defined VMCFG_UNALIGNED_INT_ACCESS && defined HAVE_BYTESWAP32
            *(uint32_t*)b = byteSwap32(u.words[second]);       // write
            *(uint32_t*)(b+4) = byteSwap32(u.words[first]);   //   opposite endianness
#elif defined VMCFG_BIG_ENDIAN
            uint32_t w;
            u.dval = value;
            w = u.words[first];
            b[7] = (uint8_t)(w >> 24);   // write
            b[6] = (uint8_t)(w >> 16);   //   little
            b[5] = (uint8_t)(w >> 8);    //     endian
            b[4] = (uint8_t)w;
            w = u.words[second];
            b[3] = (uint8_t)(w >> 24);   // write
            b[2] = (uint8_t)(w >> 16);   //   little
            b[1] = (uint8_t)(w >> 8);    //     endian
            b[0] = (uint8_t)w;
#else
            uint32_t w;
            u.dval = value;
            w = u.words[second];
            b[0] = (uint8_t)(w >> 24);   // write
            b[1] = (uint8_t)(w >> 16);   //   big
            b[2] = (uint8_t)(w >> 8);    //     endian
            b[3] = (uint8_t)w;
            w = u.words[first];
            b[4] = (uint8_t)(w >> 24);   // write
            b[5] = (uint8_t)(w >> 16);   //   big
            b[6] = (uint8_t)(w >> 8);    //     endian
            b[7] = (uint8_t)w;
#endif
        }
    }

    ByteArray::CompressionAlgorithm ByteArrayObject::algorithmToEnum(String* algorithm)
    {
        Toplevel* toplevel = this->toplevel();
        toplevel->checkNull(algorithm, "algorithm");
        if (algorithm->equalsLatin1("zlib"))
        {
            return ByteArray::k_zlib;
        }
        if (algorithm->equalsLatin1("deflate"))
        {
            return ByteArray::k_deflate;
        }
        if( algorithm->equalsLatin1("lzma"))
        {
            return ByteArray::k_lzma;
        }
        else
        {
            // Unknown format
            toplevel->throwIOError(kCompressedDataError);
            return ByteArray::k_zlib; // not reached, pacify compiler
        }
    }

    void ByteArrayObject::_compress(String* algorithm)
    {
        m_byteArray.Compress(algorithmToEnum(algorithm));
    }

    void ByteArrayObject::_uncompress(String* algorithm)
    {
        m_byteArray.Uncompress(algorithmToEnum(algorithm));
    }

    void ByteArrayObject::writeBytes(ByteArrayObject *bytes,
                                     uint32_t offset,
                                     uint32_t length)
    {
        toplevel()->checkNull(bytes, "bytes");

        if (length == 0) {
            length = bytes->get_length() - offset;
        }
        
        m_byteArray.WriteByteArray(bytes->GetByteArray(), 
                                   offset, 
                                   length);
    }

    void ByteArrayObject::readBytes(ByteArrayObject *bytes,
                                    uint32_t offset,
                                    uint32_t length)
    {
        toplevel()->checkNull(bytes, "bytes");

        if (length == 0) {
            length = m_byteArray.Available();
        }

        m_byteArray.ReadByteArray(bytes->GetByteArray(),
                                  offset,
                                  length);
    }

    String* ByteArrayObject::readMultiByte(uint32_t length, String* charSet)
    {
        toplevel()->checkNull(charSet, "charSet");
        return m_byteArray.ReadMultiByte(length, charSet);
    }
    
    String* ByteArrayObject::readUTF()
    {
        return readUTFBytes((uint32_t)readUnsignedShort());
    }

    String* ByteArrayObject::readUTFBytes(uint32_t length)
    {
        if (m_byteArray.Available() < length)
            toplevel()->throwEOFError(kEOFError);

        const uint8_t* p = (const uint8_t*)m_byteArray.GetReadableBuffer() + m_byteArray.GetPosition();

        uint32_t countBytesConsumed = length;
        // Skip UTF8 BOM (it is consumed from input, but not part of output).
        if (length >= 3 && p[0] == 0xEFU && p[1] == 0xBBU && p[2] == 0xBFU)
        {
            p += 3;
            length -= 3;
        }
        
        // Bugzilla 687341: we must stop at NUL, so here we need to scan for a NUL
        // in the portion we're considering.  With a fix to that bug we won't need
        // to do so.
#if 1
        const uint8_t* limit = p + length;
        const uint8_t* q = p;
        while (q < limit && *q != 0)
            q++;
#else
        const uint8_t* q = p + length;
#endif

        String *result = toplevel()->core()->newStringUTF8((const char*)p, int(q-p));

        // The position is always updated as if the entire string had been consumed, 
        // even if there was a NUL that made us stop early.
        m_byteArray.SetPosition(m_byteArray.GetPosition()+countBytesConsumed);

        return result;
    }

    void ByteArrayObject::writeMultiByte(String* value, String* charSet)
    {
        toplevel()->checkNull(value, "value");
        toplevel()->checkNull(charSet, "charSet");
        m_byteArray.WriteMultiByte(value, charSet);
    }
    
    void ByteArrayObject::writeUTF(String* value)
    {
        toplevel()->checkNull(value, "value");
        m_byteArray.WriteUTF(value);
    }

    void ByteArrayObject::writeUTFBytes(String* value)
    {
        toplevel()->checkNull(value, "value");
        m_byteArray.WriteUTFBytes(value);
    }

    uint32_t ByteArrayObject::get_objectEncoding()
    {
        return m_byteArray.GetObjectEncoding();
    }

    void ByteArrayObject::set_objectEncoding(uint32_t objectEncoding)
    {
        if ((objectEncoding == kAMF3)||(objectEncoding == kAMF0))
        {
            m_byteArray.SetObjectEncoding(ObjectEncoding(objectEncoding));
        }
        else
        {
            toplevel()->throwArgumentError(kInvalidEnumError, "objectEncoding");
        }
    }

    Stringp ByteArrayObject::get_endian()
    {
        return (m_byteArray.GetEndian() == kBigEndian) ? core()->kbigEndian : core()->klittleEndian;
    }

    void ByteArrayObject::set_endian(Stringp type)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();

        toplevel->checkNull(type, "endian");

        type = core->internString(type);
        if (type == core->kbigEndian)
        {
            m_byteArray.SetEndian(kBigEndian);
        }
        else if (type == core->klittleEndian)
        {
            m_byteArray.SetEndian(kLittleEndian);
        }
        else
        {
            toplevel->throwArgumentError(kInvalidEnumError, "type");
        }
    }
    
    void ByteArrayObject::clear()
    {
        m_byteArray.Clear();
        m_byteArray.SetPosition(0);
    }

    int32_t ByteArrayObject::atomicCompareAndSwapIntAt(int32_t byteIndex , int32_t expectedValue, int32_t newValue )
    {
        return m_byteArray.CAS(byteIndex, expectedValue, newValue);
    }
    
    int32_t ByteArrayObject::atomicCompareAndSwapLength(int32_t expectedLength, int32_t newLength)
    {
        if (m_byteArray.IsShared()) {
            ByteArrayCompareAndSwapLengthTask task(&m_byteArray, expectedLength, newLength);
            task.exec();
            return task.result;
        }
        else {
            return m_byteArray.UnprotectedAtomicCompareAndSwapLength(expectedLength, newLength);
        }
    }

    int32_t ByteArray::UnprotectedAtomicCompareAndSwapLength(int32_t expectedLength, int32_t newLength)
    {
        int32_t result = GetLength();
        if (expectedLength == result) {
            const bool CalledFromAS3Setter = true;
            Grower grower(this, newLength);
            grower.SetLengthCommon(newLength, CalledFromAS3Setter);
        }
        return result;
    }

    void ByteArrayObject::set_shareable(bool val)
    {
        m_byteArray.setShareable(val);
    }
    
    bool ByteArrayObject::get_shareable()
    {
        return m_byteArray.isShareable();
    }

	ChannelItem* ByteArrayObject::makeChannelItem()
	{
        class ByteArrayChannelItem: public ChannelItem
        {
        public:
            ByteArrayChannelItem(ByteArray::Buffer* value, bool shareable)
            {
                m_value = value;
                m_isShareable = shareable;
            }

            void intern(ByteArrayObject* ba) const
            {
                Toplevel* tl = ba->toplevel();
                tl->internObject(m_value, ba);
            }

            Atom getAtom(Toplevel* toplevel) const
            {
		        ByteArrayObject* baObject = toplevel->getInternedObject(m_value).staticCast<ByteArrayObject>();
		        if (baObject == NULL) 
		        {
			        ByteArrayClass* baClass = toplevel->byteArrayClass();
                    ByteArray ba(toplevel, m_value, m_isShareable);
			        baObject = new (toplevel->gc(), MMgc::kExact) ByteArrayObject(baClass->ivtable(), baClass->prototypePtr(), ba);
			        toplevel->internObject(m_value, baObject);
		        }
		        return baObject->toAtom();
            }

        private:
            bool m_isShareable;
            FixedHeapRef<ByteArray::Buffer> m_value;
        };

        ByteArrayChannelItem* item = NULL;
		ByteArray::Buffer* buffer = GetByteArray().getUnderlyingBuffer();
        const bool cIsShareable = GetByteArray().isShareable();
		if (!cIsShareable)
		{
			ByteArray::Buffer* copy = mmfx_new(ByteArray::Buffer);
			copy->capacity = buffer->capacity;
			copy->length = buffer->length;
			if (buffer->array) {
				copy->array = mmfx_new_array_opt(uint8_t, buffer->capacity, MMgc::kCanFailAndZero);
                if (copy->array) {
				    VMPI_memcpy(copy->array, buffer->array, buffer->length);
                }
			} else {
				copy->array = NULL;
			}
            item = mmfx_new(ByteArrayChannelItem(copy, cIsShareable));
		} else {
            item = mmfx_new(ByteArrayChannelItem(buffer, cIsShareable));
		}
        item->intern(this);
        return item;
	}
	
#ifdef DEBUGGER
    uint64_t ByteArrayObject::bytesUsed() const
    {
        uint64_t size = ScriptObject::bytesUsed();
        size += m_byteArray.bytesUsed();
        return size;
    }
#endif

    //
    // ByteArrayClass
    //

    ByteArrayClass::ByteArrayClass(VTable *vtable)
        : ClassClosure(vtable)
    {
        setPrototypePtr(toplevel()->objectClass->construct());
        set_defaultObjectEncoding(kEncodeDefault);
    }

    GCRef<ByteArrayObject> ByteArrayClass::constructByteArray()
    {
        return constructObject();
    }
}

