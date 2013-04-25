/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ALLOCATIONMACROS__
#define __ALLOCATIONMACROS__

// The allocation macros. mmfx_? ones will have an alternative implementation of mapping to original new/delete implementations
// the others are always on.

namespace MMgc
{
    enum NewDummyOperand { kUseFixedMalloc };
}

#ifndef MMGC_OVERRIDE_GLOBAL_NEW

// Used for allocating/deallocating memory with MMgc's fixed allocator.
// The memory allocated using these macros will be released when the MMgc aborts due to
// an unrecoverable out of memory situation.
#define mmfx_new(new_data)                  new (MMgc::kUseFixedMalloc) new_data
#define mmfx_new0(new_data)                 new (MMgc::kUseFixedMalloc, MMgc::kZero) new_data

#define mmfx_new_array(type, n)             ::MMgcConstructTaggedArray((type*)NULL, n, MMgc::kNone)

#define mmfx_new_opt(new_data, opts)        new (MMgc::kUseFixedMalloc, opts) new_data
#define mmfx_new_array_opt(type, n, opts)   ::MMgcConstructTaggedArray((type*)NULL, n, opts)

#define mmfx_delete(p)                      ::MMgcDestructTaggedScalarChecked(p)
#define mmfx_delete_array(p)                ::MMgcDestructTaggedArrayChecked(p)

#define mmfx_alloc(_siz)                    MMgc::AllocCall(_siz)
#define mmfx_alloc_opt(_siz, opts)          MMgc::AllocCall(_siz, opts)
#define mmfx_free(_ptr)                     MMgc::DeleteCall(_ptr)

#else

#define mmfx_new(new_data)                  new new_data
#define mmfx_new0(new_data)                 new (MMgc::kZero) new_data

#define mmfx_new_array(type, n)             new type[MMgc::GCHeap::CheckForNewSizeOverflow(n,sizeof(type),/*canFail*/false)]

#define mmfx_new_opt(new_data, opts)        new (opts) new_data
#define mmfx_new_array_opt(type, n, opts)   new (opts) type[MMgc::GCHeap::CheckForNewSizeOverflow(n,sizeof(type),(opts&MMgc::kCanFail)!=0)]
#define mmfx_delete(p)                      delete p
#define mmfx_delete_array(p)                delete[] p

#define mmfx_alloc(_siz)                    new char[_siz]
#define mmfx_alloc_opt(_siz, opts)          new (opts) char[_siz]
#define mmfx_free(_ptr)                     delete [] (char*)_ptr

#endif // MMGC_OVERRIDE_GLOBAL_NEW

// Used to allocate memory from the system default operator new. The lifetime of these
// allocations may exceed the lifetime of MMgc.
#define system_new(new_data)                new new_data
#define system_new_array(type, n)           new type[n]
#define system_delete(p)                    delete p
#define system_delete_array(p)              delete[] p

#endif // __ALLOCATIONMACROS__
