/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCRef_inlines__
#define __GCRef_inlines__

namespace MMgc
{
    namespace GCAPI
    {
        template < class T >
        GCRef<T>& GCRef<T>::operator=(T *tNew)
        {
#ifdef DEBUG
            GC *gc = GC::GetActiveGC();
            GCAssert(!gc->IsPointerToGCPage(this));
#endif
            t = tNew;
            return *this;
        }
        
        template < class T >
        template < class T2 >
        GCRef<T>& GCRef<T>::operator=(const GCRef<T2>& other)
        {
#ifdef DEBUG
            GC *gc = GC::GetActiveGC();
            GCAssert(!gc->IsPointerToGCPage(this));
#endif
            t = other.t;
            return *this;
        }

        template < class T >
        void GCRef_Union<T>::operator=(T *tNew)
        {
#ifdef DEBUG
            GC *gc = GC::GetActiveGC();
            GCAssert(!gc->IsPointerToGCPage(this));
#endif
            t = tNew;
        }
        
        template < class T >
        template < class T2 >
        GCRef_Union<T>& GCRef_Union<T>::operator=(const GCRef<T2>& other)
        {
#ifdef DEBUG
            GC *gc = GC::GetActiveGC();
            GCAssert(!gc->IsPointerToGCPage(this));
#endif
            t = other.t;
            return *this;
        }
    }
}

#endif /* __GCRef_inlines__ */
