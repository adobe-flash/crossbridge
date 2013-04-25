/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#include "avmplusList-impl.h"

namespace avmplus
{
    // Force explicit instantiations for various non-inlined ListImpl methods;
    // some compilers don't need this, but some do. (I'm looking at you, XCode.)

    template class ListImpl<MMgc::GCObject*, GCListHelper>;
    template class ListImpl<MMgc::RCObject*, RCListHelper>;
    template class ListImpl<MMgc::GCObject*, WeakRefListHelper>;
    template class ListImpl<Atom, AtomListHelper>;
    template class ListImpl< UnmanagedPointer, DataListHelper<UnmanagedPointer> >;
    template class ListImpl< uint8_t, DataListHelper<uint8_t> >;
    template class ListImpl< int32_t, DataListHelper<int32_t> >;
    template class ListImpl< uint32_t, DataListHelper<uint32_t> >;
    template class ListImpl< uint64_t, DataListHelper<uint64_t> >;
#ifdef VMCFG_FLOAT
    template class ListImpl< float, DataListHelper<float> >;
    template class ListImpl< float4_t, DataListHelper<float4_t, 16> >;
#endif    
    template class ListImpl< double, DataListHelper<double> >;
    template class ListImpl< char, DataListHelper<char> >;
}
