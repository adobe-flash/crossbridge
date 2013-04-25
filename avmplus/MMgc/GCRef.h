/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCRef__
#define __GCRef__

// This allows "this" to be replaced with "thisRef".  GetThisRef is
// declared on GCObject and GCFinalizedObject so this macro can only
// be used in subclasses of them.  We may later require a class level
// macro that declares a proper "thisRef" function to be used in order
// to avoid macro collisions on the "thisRef" id.
#define thisRef GetThisRef(this)

//  Specify a "NULL" value as a GCRef
#define GCREF_NULL(type) GCRef< type>()

//  Define a member that's unprotected by reference counting or write barriers. Generally speaking this is
//  a dangerous practice that should be used with extreme caution.  Please use "GCMember" to define garbage
//  collected members of a class.
#define GCREF_UNPROTECTEDMEMBER(type) GCRef< type>

//  Unions do not allow objects that have complex constructors, so unions should always be considered "unprotected members".
//  Similar to the warning above, avoid using this macro wherever possible by NOT using unions that contain GC values as members
//  of a class.
#define GCREF_UNPROTECTEDMEMBER_UNION(type) GCRef_Union< type>

//  These Macros should be used when casting a GCRef or GCMember to or from "void". This will assist future efforts to remove
//  implicit casting between GCRef/GCMember and raw pointers.
#define GCREF_CASTTOVOID(value) ((void *)value)
#define GCREF_CASTTOVOID_CONST(value) ((const void *)value)
#define GCREF_CASTFROMVOID(type, value) ((type*)value)

namespace avmplus { template <class T2> class GCList; }

namespace MMgc
{
    //  The MMgc::GCAPI namespace will be globally open to improve readability of these commonly used utilities
    namespace GCAPI
    {

        //  GCRef Definition
        template <class T>
        class GCRef
        {
            
            //  Grant friend access to all types of GCRefs
            template <class T2> friend class GCRef;
            
            //  Grant friend access to all types of GCRef_Unions
            template <class T2> friend class GCRef_Union;
                    
            template <class T2> friend class avmplus::GCList;
            
        protected:
            
            //  Allow GCRef subclasses to have access to the Raw GC pointer of any other GCRef
            template <class T2>
            REALLY_INLINE T *ProtectedGetOtherRawPtr(const GCRef<T2> &other)
            {
                return other.t;
            }

        public:

            //  Allow casting to T* (or T<base> *) for implicit conversion to raw pointers
            REALLY_INLINE operator T*() const
            {
                return t;
            }

            //  Allow assignment to T*(or T<derived>*) for implicit conversion from raw pointers
            REALLY_INLINE GCRef& operator=(T *tNew);
            
            //  Construct from T* (or T<derived>*) for implicit conversion from raw pointers
            REALLY_INLINE GCRef(T* valuePtr) : t(valuePtr)
            {
            }
        
            //  Copy the value of one GCRef from another GCRef
            template <class T1, class T2>
            static void memcpy(GCRef<T1> &dst, const GCRef<T2> &src, size_t size){ ::memcpy(dst.t, src.t, size) ;}

            //  Allows GCRefs to be used like pointers
            REALLY_INLINE T* operator->() const
            {
                return t;
            }
            
            //  Allow Any GCRef to be assigned to another GCRef
            template <class T2>
            REALLY_INLINE GCRef& operator =(const GCRef<T2> &other);
            
            //  Allow Any GCRef<derived> to be cast to a GCRef<base>
            template < class T2 >
            REALLY_INLINE operator GCRef<T2>() const
            {
                GCRef<T2> temp;
                temp.t = t;
                return temp;
            }
            
            //  staticCast<T2> implements standard C++ "static_cast" for casting one GCRef<T> to another GCRef<T2> of defined base or derived type
            template < class T2 >
            REALLY_INLINE GCRef<T2> staticCast() const
            {
                GCRef<T2> temp;
                temp.t = static_cast<T2*>(t);
                return temp;
            }
        
            //  reinterpretCast<T2> implements standard C++ "reinterpret_cast" for casting one GCRef<T> to another GCRef<T2> arbitrarily
            template < class T2 >
            REALLY_INLINE GCRef<T2> reinterpretCast() const
            {
                GCRef<T2> temp;
                temp.t = reinterpret_cast<T2*>(t);
                return temp;
            }
            
            REALLY_INLINE T& operator*()
            {
                return *t;
            }
                
            //  Support existing smart ptr "Clear" syntax
            void Clear()
            {
                t = NULL;
            }
                            
            //  Allow pointer value comparison between two GCRefs
            template <class T2>
            REALLY_INLINE bool operator==(const GCRef<T2> &other) const
            {
                return (t == other.t);
            }

            template <class T2>
            REALLY_INLINE bool operator!=(const GCRef<T2>& other) const
            {
                return (t != other.t);
            }

            //  Allows direct deletion of GC objects.
            REALLY_INLINE void Delete()
            {
                // ExactGC does not tolerate dangling pointers so it
                // is necessary to set the explicitly deleted GC
                // object pointer to NULL.  It is also important to
                // set the pointer to NULL before deleting it as
                // deletion can cause allocator activity
                // (eg. destructor allocates and causes GC to run).
                
                T* tCopy = t;
                t = NULL;
                delete tCopy;
            }

            //  Constructor defaults GC memory pointer to NULL
            REALLY_INLINE GCRef() : t(0)
            {
            }

            //  Create from other GCRef<T2>
            template <class T2>
            explicit REALLY_INLINE GCRef(const GCRef<T2> &other) : t(other.t)
            {
            }
                
        protected:
            //  The raw managed memory pointer
            T* t;
        };
        
       
        //  The GCRef_Union class should only be used in unions where members may not have constructors defined.  This
        //  class is designed only to bridge union members as closely to possible to GCRef's.  Note: Unions that define
        //  GCRef_Union members ARE NOT protected by WriteBarriers or Reference counting.  Use of this class is strongly discouraged.
        template <class T>
        class GCRef_Union
        {
        public:
            //  Allow Any GCRef_Union to be assigned to another GCRef
            template <class T2>
            REALLY_INLINE GCRef_Union& operator =(const GCRef<T2> &other);

            //  Cast a GCRef_Union to a GCRef<T>
            REALLY_INLINE operator GCRef<T>() const
            {
                GCRef<T> temp;
                temp.t = (T *)t;
                return temp;
            }
            
            REALLY_INLINE operator T*() const
            {
                return t;
            }
            
            void operator=(T *tNew);
                  
            //  Allows GCRef_Union to be used like pointers
            REALLY_INLINE T* operator->() const
            {
                return t;
            }
            
            //  Allow pointer value comparison on the referenced GC memory pointer
            template <class T2>
            REALLY_INLINE bool operator==(const GCRef<T2> &other) const
            {
                return (t == other.t);
            }

            template <class T2>
            REALLY_INLINE bool operator!=(const GCRef<T2>& other) const
            {
                return (t != other.t);
            }

        private:
            T* t;
        };
    }
}

using namespace MMgc::GCAPI;

#endif //  __GCRef__
