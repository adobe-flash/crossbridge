/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef DICTIONARYGLUE_INCLUDED
#define DICTIONARYGLUE_INCLUDED

namespace avmplus
{
    class GC_AS3_EXACT_WITH_HOOK(DictionaryObject, ScriptObject)
    {
        friend class DictionaryClass;
    protected:
        DictionaryObject(VTable *vtable, ScriptObject *delegate);
        
    public:

        void init(bool weakKeys);

        // multiname and Stringp forms fall through to ScriptObject
        virtual Atom getAtomProperty(Atom name) const;
        virtual bool hasAtomProperty(Atom name) const;
        virtual bool deleteAtomProperty(Atom name);
        virtual void setAtomProperty(Atom name, Atom value);

        virtual Atom nextName(int index);
        virtual Atom nextValue(int index);
        virtual int nextNameIndex(int index);

        bool isUsingWeakKeys() const { return getHeapHashtable()->weakKeys(); }

    private:

        // Deliberately declared out-of-line; inline declaration inexplicably
        // makes certain permutations of XCode 3.2.x for iOS segfault.
        // Voodoo coding FTW!
        void gcTraceHook_DictionaryObject(MMgc::GC *gc);
            
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=666720
        HeapHashtable* getHeapHashtable() const;
#else
        inline HeapHashtable* getHeapHashtable() const
        {
            // uintptr_t (rather than char*) to avoid "increases required alignment" warning
            return *(HeapHashtable**)(uintptr_t(this) + vtable->traits->getHashtableOffset());
        }
#endif

        Atom FASTCALL getKeyFromObject(Atom object) const;

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DictionaryObject)

    private:
        DECLARE_SLOTS_DictionaryObject;
    // ------------------------ DATA SECTION END
    };

    class GC_AS3_EXACT(DictionaryClass, ClassClosure)
    {
    private:
        DictionaryClass(VTable *vtable);
    public:
    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DictionaryClass)

    private:
        DECLARE_SLOTS_DictionaryClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* DICTIONARYGLUE_INCLUDED */
