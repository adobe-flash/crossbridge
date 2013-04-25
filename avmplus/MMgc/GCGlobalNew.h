/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCGLOBALNEW__
#define __GCGLOBALNEW__

#ifdef MMGC_ENABLE_CPP_EXCEPTIONS
  #define MMGC_NEW_THROWS_CLAUSE throw (std::bad_alloc)
  #define MMGC_DELETE_THROWS_CLAUSE throw ()
#else
  #define MMGC_NEW_THROWS_CLAUSE
  #define MMGC_DELETE_THROWS_CLAUSE
#endif

namespace MMgc
{
#ifdef MMGC_USE_SYSTEM_MALLOC
    /**
     * Allocate memory from the system heap using VMPI_alloc, with options.
     *
     * @param size  the size in bytes of the request
     * @param opts  an options bit vector
     *
     * @return  A pointer to a suitable memory area.  NULL only if kCanFail is part of
     *          opts.  The memory is zeroed only of kZero is part of opts.
     *
     * @note If memory cannot be allocated and kCanFail is not part of opts then the regular
     *       OOM handling is run, in the hope that this will free some memory on the system
     *       heap as well.  If memory is not available on a second allocation attempt then
     *       the system will enter the abort state.
     */
    void *SystemNew(size_t size, FixedMallocOpts opts);

    /**
     * Free memory allocated with SystemNew.  p may be NULL.
     */
    void SystemDelete(void* p);
#endif

    /**
     * Simply an abstraction around a lower-level allocator: no headers, no tagging.
     * @return NULL only if ((opts & kCanFail) != 0), zeroed memory only if ((opts & kZero) != 0).
     */
    REALLY_INLINE void *AllocCallInline(size_t size, FixedMallocOpts opts=kNone)
    {
#ifdef MMGC_USE_SYSTEM_MALLOC
        return SystemNew(size, opts);
#else
        return FixedMalloc::GetFixedMalloc()->OutOfLineAlloc(size, opts);
#endif
    }

    /**
     * Out-of-line implementation of AllocCallInline.
     */
    void *AllocCall(size_t, FixedMallocOpts opts=kNone);

    /**
     * Simply an abstraction around a lower-level deallocator, for objects allocated
     * by AllocCallInline / AllocCall (no headers, no tagging).
     *
     * @param p  The object to be deallocated.  May be NULL.
     */
    REALLY_INLINE void DeleteCallInline(void *p)
    {
#ifdef MMGC_USE_SYSTEM_MALLOC
        SystemDelete(p);
#else
        FixedMalloc::GetFixedMalloc()->OutOfLineFree(p);
#endif
    }

    /**
     * Out-of-line implementation of DeleteCallInline.
     */
    void DeleteCall(void *);
};

#ifdef MMGC_OVERRIDE_GLOBAL_NEW

// Global operator new and delete implementations to allocate/free from FixedMalloc.
// Using these may create a problem if compiled with system (OS) headers that also use the default
// new & delete. This happens for example in ATL of the Windows ActiveX builds. Also, out of memory
// handling will release everything allocated with FixedMalloc when abort due to OOM occurs. If
// things in the system headers/plugin interface/platform globals are allocated with these, it will
// result most likely in a crash. By using the GCAllocObject you can try to go around some of these
// issues, but atleast to system headers may pose a problem.

// User-defined operator new.
REALLY_INLINE void *operator new(size_t size) MMGC_NEW_THROWS_CLAUSE
{
    return MMgc::AllocCallInline(size);
}

REALLY_INLINE void *operator new(size_t size, MMgc::FixedMallocOpts opts) MMGC_NEW_THROWS_CLAUSE
{
    return MMgc::AllocCallInline(size, opts);
}

REALLY_INLINE void *operator new[](size_t size) MMGC_NEW_THROWS_CLAUSE
{
    return MMgc::AllocCallInline(size);
}

REALLY_INLINE void *operator new[](size_t size, MMgc::FixedMallocOpts opts) MMGC_NEW_THROWS_CLAUSE
{
    return ( (size==0) ? NULL : MMgc::AllocCallInline(size, opts) );
}

// User-defined operator delete.
REALLY_INLINE void operator delete( void *p) MMGC_DELETE_THROWS_CLAUSE
{
    MMgc::DeleteCallInline(p);
}

REALLY_INLINE void operator delete[]( void *p ) MMGC_DELETE_THROWS_CLAUSE
{
    MMgc::DeleteCallInline(p);
}

// map these to nothingness
#define MMGC_DECLARE_OPERATOR_DELETES_FOR_CLASS
#define MMGC_DECLARE_SPECIALIZED_DESTRUCTORCALL_TEMPLATES( TYPE )
#define MMGC_DECLARE_GLOBAL_DELETE_FNCS_AS_FRIEND( TYPE )
#define MMGC_USING_DELETE_OPERATOR( TYPE )

#else /* MMGC_OVERRIDE_GLOBAL_NEW */

// Defines allocation and deallocation functions used by allocation and deallocation macros.
// operator new and new[] have a specialized signature by having an ignored MMgc::kUseFixedMalloc parameter.
// Specialized operator delete and delete[] are provided to get rid of compiler warnings.

void* operator new(size_t size, MMgc::NewDummyOperand /*ignored*/) MMGC_NEW_THROWS_CLAUSE;
void *operator new(size_t size, MMgc::NewDummyOperand /*ignored*/, MMgc::FixedMallocOpts opts) MMGC_NEW_THROWS_CLAUSE;

// Empty deletes to get rid of warnings
// has impl for solaris (maybe solaris is generating C++ exception based delete calls?)
REALLY_INLINE void operator delete(void *, MMgc::NewDummyOperand /* ignored */) MMGC_DELETE_THROWS_CLAUSE { GCAssert(false); }

// b/c we need to use the fancy template routines for new[] these arent used
//void* operator new[](size_t size, size_t arraySize, MMgc::NewDummyOperand /*ignored*/) MMGC_NEW_THROWS_CLAUSE;
//void* operator new[](size_t size, size_t arraySize, MMgc::NewDummyOperand /*ignored*/, MMgc::FixedMallocOpts opts) NEW_THROWS_CLAUSE;
//void operator delete[](void *p, MMgc::NewDummyOperand /* ignored */) MMGC_DELETE_THROWS_CLAUSE;

// These values are '8' because some architectures (MIPS for sure, PPC probably) require
// that structures be aligned on the natural boundary for their largest element, which
// is typically a 'double', ie, 8 bytes.

#define MMGC_ARRAYHEADER_SIZE  8    // Space for array length before array
#define MMGC_GUARDCOOKIE_SIZE  8    // Space for cookie before object (precedes header)

namespace MMgc
{
    /**********************************
     * INTERNAL APIS
     **********************************/

#ifdef MMGC_DELETE_DEBUGGING
    #define mmgc_debug_only(_x) _x
#else
    #define mmgc_debug_only(_x)
#endif

#ifdef MMGC_DELETE_DEBUGGING
    // Verify that p points to an object allocated by NewTaggedScalar.
    void VerifyTaggedScalar(void* p);

    // Verify that p points to an object allocated by NewTaggedArray.
    void VerifyTaggedArray(void* p, bool primitive);
#endif

    /**
     * Allocate space for a scalar, with error checking (in debug builds).
     * The operators new are defined in GCGlobalNew.cpp; they inline NewTaggedScalar as
     * their entire function body, so NewTaggedScalar defines the semantics of 'new'.
     *
     * @param size  The size in bytes of the object
     * @param opts  A bit vector of the usual MMgc options (kCanFail, kZero).
     *
     * @return A pointer to the object.  Returns NULL only if ((opts & kCanFail) != 0)
     *         and the object can't be allocated due to an out-of-memory condition or
     *         if the object would exceed the maximum object size.
     *
     * In debug builds, this tags the memory, and the untagging call
     */
    void* NewTaggedScalar(size_t size, FixedMallocOpts opts=kNone);

    /**
     * Allocate space for an array, with error checking (in debug builds).
     *
     * @param count        The number of array elements
     * @param elsize       The size in bytes of each array element
     * @param opts         A bit vector of the usual MMgc options (kCanFail, kZero).
     * @param isPrimitive  A flag indicating whether the array needs a header storing
     *                     the value of count: if false, the header is created and
     *                     the value of count is stored; if true, it is not.
     *
     * @return  A pointer to element 0 of the array.  Returns NULL only if ((opts & kCanFail) != 0)
     *          and the object can't be allocated due to an out-of-memory condition or
     *          if the object would exceed the maximum object size.
     */
    void* NewTaggedArray(size_t count, size_t elsize, FixedMallocOpts opts, bool isPrimitive);

    /* Deallocate an object allocated by NewTaggedScalar.  Does /not/ check the tag.
     *
     * @param p  A non-NULL pointer returned from NewTaggedScalar.
     */
    void DeleteTaggedScalar(void *);

    /* Deallocate an object allocated by NewTaggedScalar after checking
     * the tag (in debug builds).
     *
     * @param p  A pointer returned from NewTaggedScalar, or NULL.
     */
    void DeleteTaggedScalarChecked(void* p);

    /* Deallocate an object allocated by NewTaggedArray /if/ the allocation
     * call specified isPrimitive=false.  Does /not/ check the tag.
     *
     * @param p  A non-NULL pointer returned from NewTaggedArray with isPrimitive=false.
     */
    void DeleteTaggedArrayWithHeader(void *);

    /* Delete an object allocated by NewTaggedVecor /if/ the allocation
     * call specified isPrimitive=true, after checking the tag (in debug builds).
     *
     * @param p  A pointer returned from NewTaggedArray with isPrimitive=false, or NULL.
     */
    void DeleteTaggedArrayWithHeaderChecked(void* p, bool primitive);
};

    template <class T>
    T *MMgcConstructTaggedArray(T* /*dummy template arg*/, size_t count, MMgc::FixedMallocOpts opts)
    {
        T *mem = (T*) MMgc::NewTaggedArray(count, sizeof(T), opts, false /* !isPrimitive */);
        // Observe that mem can be NULL only if (opts & kCanFail) is true, so there is no
        // reason to redundantly test that condition here, and in fact adding the condition
        // will confuse tools like Coverity (bugzilla 655048).
        if (mem == NULL)
            return NULL;
        T *tp = mem;
        for(size_t i=count; i>0; i--, tp++)
            new ((void*)tp) T;
        return mem;
    }

#define DECLARE_PRIM_ARRAY_NEW(_x)  \
    template <> REALLY_INLINE _x *MMgcConstructTaggedArray(_x*, size_t count, MMgc::FixedMallocOpts opts)   \
    {                                                                                               \
        return (_x*)MMgc::NewTaggedArray(count, sizeof(_x), opts, true /* isPrimitive */);          \
    }

    // Scalar delete mechanism. First calls the destructor and then deletefunc to release the memory.

    template <typename T> REALLY_INLINE void MMgcDestructTaggedScalarChecked( T* mem )
    {
        if( mem )
        {
            mmgc_debug_only(MMgc::VerifyTaggedScalar(mem);)
            //  Call destructor first and then release memory
            mem->~T();
            MMgc::DeleteTaggedScalar( mem );
        }
    }

    //  Array delete mechanism
    template <typename T> REALLY_INLINE void MMgcDestructTaggedArrayChecked( T* mem )
    {
        if( mem )
        {
            mmgc_debug_only(MMgc::VerifyTaggedArray(mem, false);)

            char *p = (char*)mem - MMGC_ARRAYHEADER_SIZE;
            size_t count = *(size_t*)(void*)p;

            T* tail = mem + count;
            while ( tail > mem )
            {
                tail--;
                tail->~T();
            }
            MMgc::DeleteTaggedArrayWithHeader( mem );
        }
    }

#define DECLARE_PRIM_ARRAY_DELETE(_x)                                   \
    template <> REALLY_INLINE void MMgcDestructTaggedArrayChecked( _x* mem )    \
    {                                                                   \
        MMgc::DeleteTaggedArrayWithHeaderChecked( mem, true );          \
    }                                                                   \
                                                                        \
    template <> REALLY_INLINE void MMgcDestructTaggedScalarChecked(_x *mem) {       \
        MMgc::DeleteTaggedScalarChecked(mem);                           \
    }


#define DECLARE_PRIM_ARRAY(_x)                  \
    DECLARE_PRIM_ARRAY_NEW(_x)                  \
    DECLARE_PRIM_ARRAY_DELETE(_x)               \

    // Specialized function templates, for built-in types so that we
    // don't call destructor and add extra overhead.

    DECLARE_PRIM_ARRAY(uint8_t)
    DECLARE_PRIM_ARRAY(int8_t)
#if defined(__GNUC__) || defined(__ARMCC__) || defined(__WINSCW__) || (defined(_MSC_VER) && (_MSC_VER >= 1600))
    // msvc 2008 treats char and (u)int8_t as the same but msvc 2010+, as well as gcc and armcc don't.
    DECLARE_PRIM_ARRAY(char)
#elif !defined(_MSC_VER) && !defined(__SUNPRO_CC)
#error "Unknown compiler, check whether your compiler compiles with the above declaration, if it does you probably want it"
#endif
    DECLARE_PRIM_ARRAY(uint16_t)
    DECLARE_PRIM_ARRAY(int16_t)
    DECLARE_PRIM_ARRAY(uint32_t)
    DECLARE_PRIM_ARRAY(int32_t)
    DECLARE_PRIM_ARRAY(uint64_t)
    DECLARE_PRIM_ARRAY(int64_t)
    DECLARE_PRIM_ARRAY(float)
    DECLARE_PRIM_ARRAY(double)
    DECLARE_PRIM_ARRAY(void*)
    DECLARE_PRIM_ARRAY(void**)

    template <> REALLY_INLINE void *MMgcConstructTaggedArray(void*, size_t count, MMgc::FixedMallocOpts opts)
    {
        return (void*)MMgc::NewTaggedArray(count, sizeof(void*), opts, true /* isPrimitive */);
    }

    DECLARE_PRIM_ARRAY_DELETE(void)

#undef DECLARE_PRIM_ARRAY_NEW
#undef DECLARE_PRIM_ARRAY_DELETE
#undef DECLARE_PRIM_ARRAY

#ifdef __GNUC__
    // G++ pukes on the ::, MSC requires them

// If a class has private destructor adding this macro will allow the delete mechanisms to call that.
#define MMGC_DECLARE_GLOBAL_DELETE_FNCS_AS_FRIEND( TYPE )   \
    friend void MMgcDestructTaggedScalarChecked<>( TYPE* );         \
    friend void MMgcDestructTaggedArrayChecked<>( TYPE* );

#else

// If a class has private destructor adding this macro will allow the delete mechanisms to call that.
#define MMGC_DECLARE_GLOBAL_DELETE_FNCS_AS_FRIEND( TYPE )   \
    friend void ::MMgcDestructTaggedScalarChecked<>( TYPE* );           \
    friend void ::MMgcDestructTaggedArrayChecked<>( TYPE* );

#endif

// this is necessary for the friend calls above to resolve properly w GCC
//using namespace MMgc;

// If a class is a base class for multiple inheritance or virtual inheritance, it may need to define
// these macros (MMGC_DECLARE_OPERATOR_DELETES_FOR_CLASS) inside the class definition to create a class specific delete,
// and MMGC_DECLARE_SPECIALIZED_DESTRUCTORCALL_TEMPLATES outside the class definition to have specialized function
// template for the destructor.
// This is done in cases where the pointer to be destructed is actually the pointer of a non-first base class.
#define MMGC_DECLARE_SPECIALIZED_DESTRUCTORCALL_TEMPLATES( TYPE ) \
    MMGC_DECLARE_SPECIALIZED_DESTRUCTORCALL_TEMPLATE( TYPE ) \
    MMGC_DECLARE_SPECIALIZED_DESTRUCTORARRAYCALL_TEMPLATE( TYPE )

#define MMGC_DECLARE_SPECIALIZED_DESTRUCTORCALL_TEMPLATE( TYPE )        \
    template <> REALLY_INLINE void MMgcDestructTaggedScalarChecked( TYPE* mem ) { if(mem) delete mem; }

#define MMGC_DECLARE_SPECIALIZED_DESTRUCTORARRAYCALL_TEMPLATE( TYPE )   \
    template <> REALLY_INLINE void MMgcDestructTaggedArrayChecked( TYPE* mem ) { if(mem) delete[] mem; }

// These dont need to call destructors as the operator delete will do that automatically for us.
#define MMGC_DECLARE_OPERATOR_DELETES_FOR_CLASS \
    MMGC_DECLARE_SCALAR_DELETE_FOR_CLASS \
    MMGC_DECLARE_VECTOR_DELETE_FOR_CLASS

#define MMGC_DECLARE_SCALAR_DELETE_FOR_CLASS \
    void operator delete(void* p) { MMgc::DeleteTaggedScalarChecked(p); }

#define MMGC_DECLARE_VECTOR_DELETE_FOR_CLASS \
    void operator delete[](void* p) { MMgc::DeleteTaggedArrayWithHeaderChecked(p, false); }

// If a class derives from multiple baseclasses that define delete operator, this can be used to
// select with one to use.
#define MMGC_USING_DELETE_OPERATOR( TYPE ) \
    using TYPE::operator delete

#ifndef MMGC_OVERRIDE_NEWDELETE_DEBUGGING
//#define MMGC_OVERRIDE_NEWDELETE_DEBUGGING
#endif

#ifdef MMGC_OVERRIDE_NEWDELETE_DEBUGGING
// These help to track where global operator new/delete/new[]/delete[] are used in compile time in MSVC.
// If you compile the project with /EHsc /Ob1 /W4 flags and compile. This will generate C4714 warnings
// (forceinline failed) for places where these operators are used. So far the try catch seems to be the
// way to do this, but for loops are supposed to generate that as well. May not be turned on on builds that
// are to be executed.
REALLY_INLINE void *operator new(size_t size) MMGC_NEW_THROWS_CLAUSE
{
    (void)size; GCAssert(false);

    /*
    try
    {
    }
    catch (...)
    {
    }*/
    return (void*)-1;
}

REALLY_INLINE void *operator new[](size_t size) MMGC_NEW_THROWS_CLAUSE
{
    (void)size;GCAssert(false);
    /*
    try
    {
    }
    catch (...)
    {
    }*/
    return (void*)-1;
}

// User-defined operator delete.
REALLY_INLINE void operator delete( void *p) MMGC_DELETE_THROWS_CLAUSE
{
    (void)p;
    GCAssert(false); /*
                     try
                     {
                     }
                     catch (...)
                     {
                     }*/
}

REALLY_INLINE void operator delete[]( void *p ) MMGC_DELETE_THROWS_CLAUSE
{
    (void)p;
    GCAssert(false);/*
                    try
                    {
                    }
                    catch (...)
                    {
                    }*/
}
#endif // MMGC_OVERRIDE_NEWDELETE_DEBUGGING

#ifndef MMGC_DELETE_DEBUGGING

namespace MMgc
{
    // Debug versions of these functions are out-of-line.

    REALLY_INLINE void DeleteTaggedScalar( void* p )
    {
        DeleteCallInline(p);
    }

    REALLY_INLINE void DeleteTaggedArrayWithHeader( void* p )
    {
        if( p )
        {
            p = ((char*)p - MMGC_ARRAYHEADER_SIZE);
            DeleteCallInline(p);
        }
    }

    REALLY_INLINE void DeleteTaggedScalarChecked(void* p)
    {
        if( p )
        {
            DeleteTaggedScalar( p );
        }
    }

    REALLY_INLINE void DeleteTaggedArrayWithHeaderChecked(void* p, bool primitive)
    {
        if (p)
        {
            if (primitive)
                DeleteTaggedScalar(p);
            else
                DeleteTaggedArrayWithHeader(p);
        }
    }
}

#endif // MMGC_DELETE_DEBUGGING

#endif /* MMGC_OVERRIDE_GLOBAL_NEW */

#endif // __GCGLOBALNEW__
