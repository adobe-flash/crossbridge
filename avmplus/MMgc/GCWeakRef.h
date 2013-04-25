/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// GCWeakRef.h
// GC weak references (aka safe handles) as template classes
//

#ifndef _GC_WEAK_REF_H_
#define _GC_WEAK_REF_H_

namespace MMgc
{
    /**
     * Weak reference type (not exposed to AS3).
     *
     * GC::GetWeakRef(o) will return a unique GCWeakRef instance for the object o, possibly
     * creating that instance.  The weakref instance holds o weakly, that is, if the last 
     * reference to o is from the weakref instance then o becomes reclaimable.
     *
     * The implementation maintains a table of mappings from objects to their weakrefs.
     *
     * When the GC prepares to run finalizers it walks the mappings table.  If a weakref
     * holds an object that is not marked then the weakref is cleared.  If a weakref
     * holds an object that is marked, but the weakref is itself not marked, then the weakref
     * becomes marked, that is, weakrefs live at least as long as their objects.
     *
     * When finalizers run there is no reason to handle weakrefs specially, though GetWeakRef
     * has to check that a weak reference is not created to a dead object by a finalizer.
     */
    class GCWeakRef : public GCObject
    {
        friend class GC;
    public:
        // Use 'get' to read the value in almost every situation.
        //
        // In particular use 'get' to read a value that may be stored in a data structure,
        // used as the 'this' value in a call, or may in any other way escape the local
        // context.
        //
        // If you're just checking whether the object is NULL you should use 'isNull'.
        //
        // If you are calling 'get' from a presweep handler and it is important that the
        // value you are reading should not be marked if it has not been marked already,
        // and you're sure you know what that means, then call 'peek' instead.
        //
        // Wizards' technical note:
        //
        // 'get' contains a read barrier that causes the object to be marked if it is read
        // during presweep and is unmarked at that time - see Bugzilla 572331.

        GCObject *get();

        // @return true iff the weak ref has been cleared and no longer holds onto its object.
        
        bool isNull() { return m_obj == NULL; }

        // Wizard API:
        //
        // Use 'peek' to read the value from a presweep handler when it is important that that
        // value not be marked if it is not already marked, because you sincerely want the value
        // to be garbage collected.  You promise not to store the value read by 'peek' into any
        // heap variable or location where it may subsequently be found by, or given to, the garbage
        // collector, and you acknowledge that the storage pointed to by the value (if not NULL
        // and not marked) almost certainly will be reclaimed by the garbage collector once all
        // presweep handlers have run (absent a call to 'get').
        //
        // If you don't know what all that means then you are not sufficiently wizardly, and you
        // should not use 'peek' but 'get'.

        GCObject *peek() { return (GCObject*)m_obj; }

    private:
        /**
         * When allocating a GCWeakRef, tell the GC we don't contain pointers
         * (overriding the default base-class behavior).
         */
        static void *operator new(size_t size, GC *gc)
        {
            return gc->Alloc(size, 0);
        }

        /**
         * GCWeakRef cannot be subclassed.
         * GCWeakRef should never be destructed, the GC should just reclaim the object.
         */
        GCWeakRef(const void *obj) : m_obj(obj)
        {
#ifdef MMGC_MEMORY_INFO
            obj_creation = obj;
#endif
        }
        const void *m_obj;
#ifdef MMGC_MEMORY_INFO
        const void* obj_creation;
#endif
    };

#if 0
    // something like this would be nice
    template<class T> class GCWeakRefPtr
    {

    public:
        GCWeakRefPtr() {}
        GCWeakRefPtr(T t) {  set(t);}
        ~GCWeakRefPtr() { t = NULL; }

        T operator=(const GCWeakRefPtr<T>& wb)
        {
            return set(wb.t);
        }

        T operator=(T tNew)
        {
            return set(tNew);
        }

        operator T() const { return (T) t->get(); }

        bool operator!=(T other) const { return other != t; }

        T operator->() const
        {
            return (T) t->get();
        }
    private:
        T set(const T tNew)
        {
            t = tNew->GetWeakRef();
        }
        GCWeakRef* t;
    };
#endif
}

#endif // _GC_WEAK_REF_H_
