/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCMemberBase__
#define __GCMemberBase__

namespace MMgc
{
    //  Since GCFinalizedObject doesn't derive from GCObject, create a GCMemberBase
    //  class so that we don't have to copy/paste this in both inner class definitions.
    
    //  GCMemberBase class:
    //
    //  Separate definitons of GCMember for GCObject and GCFinalizedObject are necessary in order to
    //  restrict usage of GCMember only as member variables of GCObject or GCFinalizedObject.  We do not
    //  want to allow GCMember to be declared on a non-GCObject (except for GCRoots-- see definition in GCRoot::GCMember)
    //  The GCMemberBase class contains the shared functionality between GCObject::GCMember and GCFinalizedObject::GCMember.
    //
    
    template <class T>
    class GCMemberBase : public GCRef<T>
    {
        friend class GC;
    private:
        //  'set' is invoked whenever the garbage collected pointer value 't' changes
        void set(const T* tNew);
        T* value() const;
        T* const* location() const;

    protected:
        
        //  In order to keep the usage syntax uniform as "GCMember",
        //  Protect the constructors so that only "GCMember" subclasses are allowed to use this object.
        explicit GCMemberBase();
  
        template<class T2>
        explicit GCMemberBase(const GCRef<T2>& other);
 
        // copy constructor
        explicit GCMemberBase(const GCMemberBase<T>& other);

        ~GCMemberBase();

    public:
 
        GCMemberBase& operator=(const GCMemberBase& other);

        template<class T2>
        void operator=(const GCRef<T2>& other);

        //  Overload the T* assignment operator so that we can set GCMember's directly to NULL.
        void operator=(T* tNew);
        
    };
}

#endif /* __GCMemberBase__ */
