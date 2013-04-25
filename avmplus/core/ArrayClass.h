/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ArrayClass__
#define __avmplus_ArrayClass__


namespace avmplus
{
    /**
     * class Array
     */
    class GC_AS3_EXACT(ArrayClass, ClassClosure)
    {
    protected:
        ArrayClass(VTable* cvtable);
        
    public:
        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv)
        {
            return construct(argc, argv);
        }

        // create a new array, even when argc=1
        ArrayObject* newarray(Atom* argv, int argc);

        ArrayObject* newArray(uint32_t capacity=0);

#ifdef VMCFG_AOT
        template <typename ADT>
        ArrayObject* newArray(MethodEnv *env, ADT argDesc, va_list ap);
#endif

        // AS3 glue code methods (not inlined, just declared here for convenience)
        ArrayObject* _concat(Atom thisAtom, ArrayObject* args) { return ArrayClass::generic_concat(toplevel(), thisAtom, args); }
        Atom _pop(Atom thisAtom) { return ArrayClass::generic_pop(toplevel(), thisAtom); }
        Atom _reverse(Atom thisAtom) { return ArrayClass::generic_reverse(toplevel(), thisAtom); }
        Atom _shift(Atom thisAtom) { return ArrayClass::generic_shift(toplevel(), thisAtom); }
        ArrayObject* _slice(Atom thisAtom, double A, double B) { return ArrayClass::generic_slice(toplevel(), thisAtom, A, B); }
        Atom _sort(Atom thisAtom, ArrayObject *args) { return ArrayClass::generic_sort(toplevel(), thisAtom, args); }
        Atom _sortOn(Atom thisAtom, Atom namesAtom, Atom optionsAtom) { return ArrayClass::generic_sortOn(toplevel(), thisAtom, namesAtom, optionsAtom); }
        ArrayObject* _splice(Atom thisAtom, ArrayObject* args) { return ArrayClass::generic_splice(toplevel(), thisAtom, args); }
        int _indexOf(Atom thisAtom, Atom searchElement, int startIndex) { return ArrayClass::generic_indexOf(thisAtom, searchElement, startIndex); }
        int _lastIndexOf(Atom thisAtom, Atom searchElement, int startIndex) { return ArrayClass::generic_lastIndexOf(thisAtom, searchElement, startIndex); }
        bool _every(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_every(toplevel(), thisAtom, callback, thisObject); }
        ArrayObject* _filter(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_filter(toplevel(), thisAtom, callback, thisObject); }
        void _forEach(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_forEach(toplevel(), thisAtom, callback, thisObject); }
        bool _some(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_some(toplevel(), thisAtom, callback, thisObject); }
        ArrayObject* _map(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_map(toplevel(), thisAtom, callback, thisObject); }
        uint32_t _unshift(Atom thisAtom, ArrayObject* args) { return ArrayClass::generic_unshift(toplevel(), thisAtom, args); }

        // inline wrappers for legacy code.
        inline ArrayObject* concat(Atom thisAtom, ArrayObject* args) { return ArrayClass::generic_concat(toplevel(), thisAtom, args); }
        inline Atom pop(Atom thisAtom) { return ArrayClass::generic_pop(toplevel(), thisAtom); }
        inline Atom reverse(Atom thisAtom) { return ArrayClass::generic_reverse(toplevel(), thisAtom); }
        inline Atom shift(Atom thisAtom) { return ArrayClass::generic_shift(toplevel(), thisAtom); }
        inline ArrayObject* slice(Atom thisAtom, double A, double B) { return ArrayClass::generic_slice(toplevel(), thisAtom, A, B); }
        inline Atom sort(Atom thisAtom, ArrayObject *args) { return ArrayClass::generic_sort(toplevel(), thisAtom, args); }
        inline Atom sortOn(Atom thisAtom, Atom namesAtom, Atom optionsAtom) { return ArrayClass::generic_sortOn(toplevel(), thisAtom, namesAtom, optionsAtom); }
        inline ArrayObject* splice(Atom thisAtom, ArrayObject* args) { return ArrayClass::generic_splice(toplevel(), thisAtom, args); }
        inline int indexOf(Atom thisAtom, Atom searchElement, int startIndex) { return ArrayClass::generic_indexOf(thisAtom, searchElement, startIndex); }
        inline int lastIndexOf(Atom thisAtom, Atom searchElement, int startIndex) { return ArrayClass::generic_lastIndexOf(thisAtom, searchElement, startIndex); }
        inline bool every(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_every(toplevel(), thisAtom, callback, thisObject); }
        inline ArrayObject* filter(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_filter(toplevel(), thisAtom, callback, thisObject); }
        inline void forEach(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_forEach(toplevel(), thisAtom, callback, thisObject); }
        inline bool some(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_some(toplevel(), thisAtom, callback, thisObject); }
        inline ArrayObject* map(Atom thisAtom, ScriptObject* callback, Atom thisObject) { return ArrayClass::generic_map(toplevel(), thisAtom, callback, thisObject); }

        static ArrayObject* generic_concat(Toplevel* toplevel, Atom thisAtom, ArrayObject* args);
        static Atom generic_pop(Toplevel* toplevel, Atom thisAtom);
        static Atom generic_reverse(Toplevel* toplevel, Atom thisAtom);
        static Atom generic_shift(Toplevel* toplevel, Atom thisAtom);
        static ArrayObject* generic_slice(Toplevel* toplevel, Atom thisAtom, double A, double B);
        static Atom generic_sort(Toplevel* toplevel, Atom thisAtom, ArrayObject *args);
        static Atom generic_sortOn(Toplevel* toplevel, Atom thisAtom, Atom namesAtom, Atom optionsAtom);
        static ArrayObject* generic_splice(Toplevel* toplevel, Atom thisAtom, ArrayObject* args);
        static int generic_indexOf(Atom thisAtom, Atom searchElement, int startIndex);
        static int generic_lastIndexOf(Atom thisAtom, Atom searchElement, int startIndex);
        static bool generic_every(Toplevel* toplevel, Atom thisAtom, ScriptObject* callback, Atom thisObject);
        static ArrayObject* generic_filter(Toplevel* toplevel, Atom thisAtom, ScriptObject* callback, Atom thisObject);
        static void generic_forEach(Toplevel* toplevel, Atom thisAtom, ScriptObject* callback, Atom thisObject);
        static bool generic_some(Toplevel* toplevel, Atom thisAtom, ScriptObject* callback, Atom thisObject);
        static ArrayObject* generic_map(Toplevel* toplevel, Atom thisAtom, ScriptObject* callback, Atom thisObject);
        static uint32_t generic_unshift(Toplevel* toplevel, Atom thisAtom, ArrayObject* args);

    private:
        static void array_concat(Toplevel* toplevel, ArrayObject* a, ArrayObject* b);

    private:
        friend class ClassClosure;
        static ScriptObject* FASTCALL createUnsubclassedInstanceProc(ClassClosure* cls);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(ArrayClass)

        DECLARE_SLOTS_ArrayClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_ArrayClass__ */
