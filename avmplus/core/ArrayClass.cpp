/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

#ifdef _MAC
#ifndef __GNUC__
// inline_max_total_size() defaults to 10000.
// This module includes so many inline functions that we
// exceed this limit and we start getting compile warnings,
// so bump up the limit for this file.
#pragma inline_max_total_size(100000)
#endif
#endif

using namespace MMgc;

namespace avmplus
{
    // todo while close, the implementations here do not exactly match the
    // ECMA spec, and probably in ways that the SpiderMonkey test suite won't
    // catch... for instance, it will go haywire with array indices over
    // 2^32 because we're not applying the ToInteger algorithm correctly.
    // Indices have to handled as doubles until the last minute, then...
    // the full 2^32 range is needed but there is behavior involving
    // negative numbers too.

    ArrayClass::ArrayClass(VTable* cvtable)
    : ClassClosure(cvtable)
    {
        AvmCore* core = this->core();
        Toplevel* toplevel = this->toplevel();

        AvmAssert(traits()->getSizeOfInstance() == sizeof(ArrayClass));

        VTable* ivtable = this->ivtable();
        ScriptObject* objectPrototype = toplevel->objectClass->prototypePtr();
        setPrototypePtr(ArrayObject::create(core->GetGC(), ivtable, objectPrototype, 0));

        // According to ECMAscript spec (ECMA-262.pdf)
        // generic support: concat, join, pop, push, reverse, shift, slice, sort, splice, unshift
        // NOT generic: toString, toLocaleString
        // unknown: sortOn (our own extension)
    }

    static ArrayObject* toArray(Atom instance)
    {
        return AvmCore::isObject(instance) ? AvmCore::atomToScriptObject(instance)->toArrayObject() : NULL;
    }

    /**
    15.4.4.4 Array.prototype.concat ( [ item1 [ , item2 [ , ] ] ] )
    When the concat method is called with zero or more arguments item1, item2, etc., it returns an array containing
    the array elements of the object followed by the array elements of each argument in order.
    The following steps are taken:
    1. Let A be a new array created as if by the expression new Array().
    2. Let n be 0.
    3. Let E be this object.
    4. If E is not an Array object, go to step 16.
    5. Let k be 0.
    6. Call the [[Get]] method of E with argument "length".
    7. If k equals Result(6) go to step 19.
    8. Call ToString(k).
    9. If E has a property named by Result(8), go to step 10,
    but if E has no property named by Result(8), go to step 13.
    10. Call ToString(n).
    11. Call the [[Get]] method of E with argument Result(8).
    12. Call the [[Put]] method of A with arguments Result(10) and Result(11).
    13. Increase n by 1.
    14. Increase k by 1.
    15. Go to step 7.
    16. Call ToString(n).
    17. Call the [[Put]] method of A with arguments Result(16) and E.
    18. Increase n by 1.
    19. Get the next argument in the argument list; if there are no more arguments, go to step 22.
    20. Let E be Result(19).
    21. Go to step 4.
    22. Call the [[Put]] method of A with arguments "length" and n.
    23. Return A.
    The length property of the concat method is 1.
    NOTE The concat function is intentionally generic; it does not require that its this value be an Array object. Therefore it can be
    transferred to other kinds of objects for use as a method. Whether the concat function can be applied successfully to a host
    object is implementation-dependent.
    */

    /*static*/ void ArrayClass::array_concat(Toplevel* /*toplevel*/, ArrayObject* a, ArrayObject* b)
    {
        if (!a->try_concat(b))
        {
            for (uint32_t j = 0, n = b->getLengthProperty(); j < n; ++j)
            {
                Atom ba = b->getUintProperty(j);
                a->push(&ba, 1);
            }
        }
    }


    /*static*/ ArrayObject* ArrayClass::generic_concat(Toplevel* toplevel, Atom thisAtom, ArrayObject* args)
    {
        ScriptObject* d = AvmCore::isObject(thisAtom) ? AvmCore::atomToScriptObject(thisAtom) : NULL;
        uint32_t len = d ? d->getLengthProperty() : 0;

        uint32_t newLength = len;
        uint32_t argc = args->getLength();
        for (uint32_t i = 0; i< argc; i++)
        {
            Atom atom = args->getUintProperty(i);
            ArrayObject* b = toArray(atom);
            newLength += b ? b->getLengthProperty() : 1;
        }

        ArrayObject* out = toplevel->arrayClass()->newArray(newLength);
        ArrayObject* a = toArray(thisAtom);
        if (a)
        {
            array_concat(toplevel, out, a);
        }

        for (uint32_t i = 0; i < argc; i++)
        {
            Atom atom = args->getUintProperty(i);
            ArrayObject* b = toArray(atom);
            if (b)
            {
                array_concat(toplevel, out, b);
            }
            else
            {
                out->push(&atom, 1);
            }
        }

        return out;
    }

    /**
     * Array.prototype.pop()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */
    /*static*/ Atom ArrayClass::generic_pop(Toplevel* /*toplevel*/, Atom thisAtom)
    {
        ArrayObject* a = toArray(thisAtom);

        if (a)
            return a->pop();

        if (!AvmCore::isObject(thisAtom))
            return undefinedAtom;

        // Different than Rhino (because of delete) but matches 262.pdf
        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();
        if (!len)
        {
            d->setLengthProperty(0);
            return undefinedAtom;
        }
        else
        {
            Atom outAtom = d->getUintProperty (len-1);
            d->delUintProperty (len - 1);
            d->setLengthProperty(len - 1);
            return outAtom;
        }
    }

    /**
     * Array.prototype.reverse()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */
    /*static*/ Atom ArrayClass::generic_reverse(Toplevel* /*toplevel*/, Atom thisAtom)
    {
        ArrayObject* a = toArray(thisAtom);
        if (a && a->try_reverse())
        {
            return thisAtom;
        }

        // generic object version
        if (!AvmCore::isObject(thisAtom))
            return thisAtom;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t j = d->getLengthProperty();

        uint32_t i = 0;
        if (j)
            j--;

        while (i < j) {
            Atom frontAtom = d->getUintProperty(i);
            Atom backAtom  = d->getUintProperty(j);

            d->setUintProperty(i++, backAtom);
            d->setUintProperty(j--, frontAtom);
        }

        return thisAtom;
    }

    /**
     * Array.prototype.shift()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */
    /*static*/ Atom ArrayClass::generic_shift(Toplevel* /*toplevel*/, Atom thisAtom)
    {
        ArrayObject* a = toArray(thisAtom);

        Atom result;
        if (a && a->try_shift(result))
        {
            return result;
        }

        if (!AvmCore::isObject(thisAtom))
            return undefinedAtom;

        Atom outAtom;
        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        if (len == 0)
        {
            // set len to 0 (ecmascript spec)
            d->setLengthProperty(0);
            outAtom = undefinedAtom;
        }
        else
        {
            // Get the 0th element to return
            outAtom = d->getUintProperty(0);

            // Move all of the elements down
            for (uint32_t i=0; i<len-1; i++) {
                d->setUintProperty(i, d->getUintProperty(i+1));
            }

            d->delUintProperty (len - 1);
            d->setLengthProperty(len - 1);
        }

        return outAtom;
    }

    /**
     * Array.prototype.slice()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */
    /*static*/ ArrayObject* ArrayClass::generic_slice(Toplevel* toplevel, Atom thisAtom, double A, double B)
    {
        if (!AvmCore::isObject(thisAtom))
            return 0;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        // if a param is passed then the first one is A
        // if no params are passed then A = 0
        uint32_t a = NativeObjectHelpers::ClampIndex(A, len);
        uint32_t b = NativeObjectHelpers::ClampIndex(B, len);
        if (b < a)
            b = a;

        ArrayObject *out = toplevel->arrayClass()->newArray(b-a);

        uint32_t outIndex=0;
        for (uint32_t i=a; i<b; i++) {
            out->setUintProperty (outIndex++, d->getUintProperty (i));
        }

        return out;
    }

    static inline bool defined(Atom atom) { return (atom != undefinedAtom); }

    typedef HeapList<AtomList> HeapAtomList;

    /**
     * ArraySort implements actionscript Array.sort().
     * It's also the base class SortWithParameters, which handles all
     * other permutations of Array.
     *
     * NOTE: Instances of ArraySort must be stack-allocated, as it uses
     * avmStackAlloc for a temporary data structure.
     */
    class ArraySort
    {
    public:
        /*************************************************************
         * Forward declarations required by public methods
         *************************************************************/

        /** Options from script */
        enum {
            kCaseInsensitive        = 1
            ,kDescending            = 2
            ,kUniqueSort            = 4
            ,kReturnIndexedArray    = 8
            ,kNumeric               = 16
        };

        /** 
         * Array.sortOn() will pass an array of field names, which is converted to a stack-
         * allocated array of FieldName structures (hence no write barriers here).
         */
        struct FieldName
        {
            String* name;
            int options;
        };

        typedef int     (*CompareFuncPtr)   (const ArraySort *s, uint32_t i, uint32_t j);

        static int StringCompareFunc(const ArraySort *s, uint32_t j, uint32_t k) { return s->StringCompare(j, k); }
        static int CaseInsensitiveStringCompareFunc(const ArraySort *s, uint32_t j, uint32_t k) { return s->CaseInsensitiveStringCompare(j, k); }
        static int ScriptCompareFuncCompatible(const ArraySort *s, uint32_t j, uint32_t k) { return s->ScriptCompareCompatible(j, k); }
        static int ScriptCompareFuncCorrect(const ArraySort *s, uint32_t j, uint32_t k) { return s->ScriptCompareCorrect(j, k); }
        static int NumericCompareFuncCompatible(const ArraySort *s, uint32_t j, uint32_t k) { return s->NumericCompareCompatible(j, k); }
        static int NumericCompareFuncCorrect(const ArraySort *s, uint32_t j, uint32_t k) { return s->NumericCompareCorrect(j, k); }
        static int DescendingCompareFunc(const ArraySort *s, uint32_t j, uint32_t k) { return s->altCmpFunc(s, k, j); }
        static int FieldCompareFunc(const ArraySort *s, uint32_t j, uint32_t k) { return s->FieldCompare(j, k); }

    public:
        /*************************************************************
         * Public Functions
         *************************************************************/

        ArraySort(Atom &result, ArrayClass *f, ScriptObject *d, int options, CompareFuncPtr cmpFunc,
            CompareFuncPtr altCmpFunc, Atom cmpActionScript, uint32_t numFields = 0, FieldName *fields = NULL);

        ~ArraySort();

        // do the sort
        Atom sort();

    private:
        /*************************************************************
         * Private Functions
         *************************************************************/

        // non-recursive quicksort implementation
        void qsort(uint32_t lo, uint32_t hi);

        // qsort() is abstracted from the array by swap() and compare()
        // compare(), in turn, is abstracted from the array via get()
        //
        // cmpFunc is conditional, for instance :
        //      cmpFunc = DefaultCompareFunc;   // Array.sort()
        //      cmpFunc = ScriptCompareFunc;    // Array.sort(compareFunction)
        int compare(uint32_t lhs, uint32_t rhs) const   { return cmpFunc(this, lhs, rhs); }
        Atom get(uint32_t i)      const   { return atoms->list.get(index[i]); }
        void swap(uint32_t j, uint32_t k)
        {
            uint32_t temp = index[j];
            index[j] = index[k];
            index[k] = temp;
        }

        inline int StringCompare(uint32_t j, uint32_t k) const;
        inline int CaseInsensitiveStringCompare(uint32_t j, uint32_t k) const;
        inline int ScriptCompareCompatible(uint32_t j, uint32_t k) const;
        inline int ScriptCompareCorrect(uint32_t j, uint32_t k) const;
        inline int NumericCompareCompatible(uint32_t j, uint32_t k) const;
        inline int NumericCompareCorrect(uint32_t j, uint32_t k) const;
        inline int FieldCompare(uint32_t j, uint32_t k) const;

        /**
         * null check + pointer cast.  only used in contexts where we know we
         * have a ScriptObject* already (verified, etc)
         */
        ScriptObject* toFieldObject(Atom atom) const;

    private:
        /*************************************************************
         * Private Member Variables
         *************************************************************/

        ScriptObject *d;
        AvmCore* core;
        Toplevel* toplevel;
        int options;

        CompareFuncPtr cmpFunc;
        CompareFuncPtr altCmpFunc;
        Atom cmpActionScript;


        GC::AllocaAutoPtr index_autoptr;
        uint32_t* index;
        HeapAtomList* atoms;

        uint32_t numFields;
        FieldName *fields;
        HeapAtomList* fieldatoms;
    };

    ArraySort::ArraySort(
        Atom &result,
        ArrayClass *f,
        ScriptObject *d,
        int options,
        CompareFuncPtr cmpFunc,
        CompareFuncPtr altCmpFunc,
        Atom cmpActionScript,
        uint32_t numFields,
        FieldName *fields
    ) :
        d(d),
        core(f->core()),
        toplevel(f->toplevel()),
        options(options),
        cmpFunc(cmpFunc),
        altCmpFunc(altCmpFunc),
        cmpActionScript(cmpActionScript),
        index_autoptr(),
        index(NULL),
        atoms(NULL),
        numFields(numFields),
        fields(fields),
        fieldatoms(NULL)
    {
        uint32_t len = d->getLengthProperty();
        uint32_t iFirstAbsent = len;

        // new class[n] compiles into code which tries to allocate n * sizeof(class).
        // unfortunately, the generated assembly does not protect against this product overflowing int.
        // So I limit the length -- for larger values, I expect new will fail anyways.
        if ((len > 0) && (len < (0x10000000)))
        {
            index = (uint32_t*)avmStackAlloc(core, index_autoptr, GCHeap::CheckForCallocSizeOverflow(len, sizeof(uint32_t)));
            atoms = new (core->GetGC()) HeapAtomList(core->GetGC(), len);
        }

        if (!index || !atoms)
        {
            // return the unsorted array.

            // todo : grandma : Should I be raising an exception? Sort too big?
            // could also fallback to an unindexed sort, but with that many elements, it won't finish for hours
            result = d->atom();
            return;
        }

        uint32_t i, j;
        uint32_t newlen = len;

        // One field value - pre-get our field values so we can just do a regular sort
        if (cmpFunc == ArraySort::FieldCompareFunc && numFields == 1)
        {
            fieldatoms = new (core->GetGC()) HeapAtomList(core->GetGC(), len);

            // note, loop needs to go until i = -1, but i is unsigned. 0xffffffff is a valid index, so check (i+1) != 0.
            for (i = (len - 1), j = len; (i+1) != 0; i--)
            {
                index[i] = i;
                Atom a = d->getUintProperty(i);
                fieldatoms->list.set(i, a);

                if (AvmCore::isObject (a))
                {
                    ScriptObject* obj = AvmCore::atomToScriptObject (a);

                    Stringp name = fields[0].name;

                    // NOTE we want to sort the public members of the caller's version
                    Multiname mname(core->findPublicNamespace(), name);

                    // An undefined prop just becomes undefined in our sort
                    Atom x = toplevel->getproperty(obj->atom(), &mname, obj->vtable);
                    atoms->list.set(i, x);
                }
                else
                {
                    j--;

                    uint32_t temp = index[i];
                    index[i] = index[j];

                    if (!d->hasUintProperty(i)) {
                        newlen--;
                        index[j] = index[newlen];
                        index[newlen] = temp;
                    } else {
                        index[j] = temp;
                    }
                }
            }

            int opt = fields[0].options;

            if (opt & ArraySort::kNumeric) {
                this->cmpFunc = core->currentBugCompatibility()->bugzilla524122 ?
                                    ArraySort::NumericCompareFuncCorrect :
                                    ArraySort::NumericCompareFuncCompatible;
            } else if (opt & ArraySort::kCaseInsensitive) {
                this->cmpFunc = ArraySort::CaseInsensitiveStringCompareFunc;
            } else {
                this->cmpFunc = ArraySort::StringCompareFunc;
            }

            if (opt & ArraySort::kDescending) {
                this->altCmpFunc = this->cmpFunc;
                this->cmpFunc = ArraySort::DescendingCompareFunc;
            }
        }
        else
        {
            bool isNumericCompare = (cmpFunc == ArraySort::NumericCompareFuncCompatible) ||
                                    (altCmpFunc == ArraySort::NumericCompareFuncCompatible) ||
                                    (cmpFunc == ArraySort::NumericCompareFuncCorrect) ||
                                    (altCmpFunc == ArraySort::NumericCompareFuncCorrect);
            // note, loop needs to go until i = -1, but i is unsigned. 0xffffffff is a valid index, so check (i+1) != 0.
            for (i = (len - 1), j = len; (i+1) != 0; i--)
            {
                index[i] = i;
                atoms->list.set(i, d->getUintProperty(i));

                // We want to throw if this is an Array.NUMERIC sort and any items are not numbers,
                // and not strings that can be converted into numbers
                if(isNumericCompare && !core->isNumberOrFloat(atoms->list.get(i)))
                {
                    double val = AvmCore::number(atoms->list.get(i));
                    if(MathUtils::isNaN(val))
                        // throw exception (not a Number)
                        toplevel->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(atoms->list.get(i)), core->toErrorString(core->traits.number_itraits));

                }

                // getAtomProperty() returns undefined when that's the value of the property.
                // It also returns undefined when the object does not have the property.
                //
                // SortCompare() from ECMA 262 distinguishes these two cases. The end
                // result is undefined comes after everything else, and missing properties
                // come after that.
                //
                // To simplify our compare, partitition the array into { defined | undefined | missing }
                // Note that every missing element shrinks the length -- we'll set this new
                // length at the end of this routine when we are done.

                if (!defined(atoms->list.get(i))) {
                    j--;

                    uint32_t temp = index[i];
                    index[i] = index[j];

                    if (!d->hasUintProperty(i)) {
                        newlen--;
                        index[j] = index[newlen];
                        index[newlen] = temp;
                    } else {
                        index[j] = temp;
                    }
                }
            }
        }

        iFirstAbsent = newlen;

        // The portion of the array containing defined values is now [0, j).
        // The portion of the array containing values undefined is now [j, iFirstAbsent).
        // The portion of the array containing absent values is now [iFirstAbsent, len).

        // now sort the remaining defined() elements
        qsort(0, j-1);

        if (options & kUniqueSort)
        {
            // todo : kUniqueSort could throw an exception.
            // todo : kUniqueSort could abort the sort once equal members are found
            for (uint32_t i = 0; i < (len - 1); i++)
            {
                if (compare(i, (i+1)) == 0)
                {
                    result = core->uintToAtom(0);
                    return;
                }
            }
        }

        if (options & kReturnIndexedArray)
        {
            // return the index array without modifying the original array
            ArrayObject *obj = toplevel->arrayClass()->newArray(len);

            for (uint32_t i = 0; i < len; i++)
            {
                obj->setUintProperty(i, core->uintToAtom(index[i]));
            }

            result = obj->atom();
        }
        else
        {
            // If we need to use our fieldatoms as results, temporarily swap them with
            // our atoms array so the below code works on the right data. Fieldatoms contain
            // our original objects while atoms contain our objects[field] values for faster
            // sorting.
            HeapAtomList* temp = atoms;
            if (fieldatoms)
            {
                atoms = fieldatoms;
            }

            for (i = 0; i < iFirstAbsent; i++) {
                d->setUintProperty(i, get(i));
            }

            for (i = iFirstAbsent; i < len; i++) {
                d->delUintProperty(i);
            }

            //a->setLength(len);  ES3: don't shrink array on sort.  Seems silly
            result = d->atom();
            atoms = temp;

        }

        return;
    }

    ArraySort::~ArraySort()
    {
        delete atoms;
        delete fieldatoms;
        fields = NULL;
    }

    /*
     * QuickSort a portion of the ArrayObject.
     */
    void ArraySort::qsort(uint32_t lo, uint32_t hi)
    {
        // This is an iterative implementation of the recursive quick sort.
        // Recursive implementations are basically storing nested (lo,hi) pairs
        // in the stack frame, so we can avoid the recursion by storing them
        // in an array.
        //
        // Once partitioned, we sub-partition the smaller half first. This means
        // the greatest stack depth happens with equal partitions, all the way down,
        // which would be 1 + log2(size), which could never exceed 33.

        uint32_t size;
        struct StackFrame { uint32_t lo, hi; };
        StackFrame stk[33];
        int stkptr = 0;

        // leave without doing anything if the array is empty (lo > hi) or only one element (lo == hi)
        if (lo >= hi)
            return;

        // code below branches to this label instead of recursively calling qsort()
    recurse:

        size = (hi - lo) + 1; // number of elements in the partition

        if (size < 4) {

            // It is standard to use another sort for smaller partitions,
            // for instance c library source uses insertion sort for 8 or less.
            //
            // However, as our swap() is essentially free, the relative cost of
            // compare() is high, and with profiling, I found quicksort()-ing
            // down to four had better performance.
            //
            // Although verbose, handling the remaining cases explicitly is faster,
            // so I do so here.

            if (size == 3) {
                if (compare(lo, lo + 1) > 0) {
                    swap(lo, lo + 1);
                    if (compare(lo + 1, lo + 2) > 0) {
                        swap(lo + 1, lo + 2);
                        if (compare(lo, lo + 1) > 0) {
                            swap(lo, lo + 1);
                        }
                    }
                } else {
                    if (compare(lo + 1, lo + 2) > 0) {
                        swap(lo + 1, lo + 2);
                        if (compare(lo, lo + 1) > 0) {
                            swap(lo, lo + 1);
                        }
                    }
                }
            } else if (size == 2) {
                if (compare(lo, lo + 1) > 0)
                    swap(lo, lo + 1);
            } else {
                // size is one, zero or negative, so there isn't any sorting to be done
            }
        } else {
            // qsort()-ing a near or already sorted list goes much better if
            // you use the midpoint as the pivot, but the algorithm is simpler
            // if the pivot is at the start of the list, so move the middle
            // element to the front!
            uint32_t pivot = lo + (size / 2);
            swap(pivot, lo);


            uint32_t left = lo;
            uint32_t right = hi + 1;

            for (;;) {
                // Move the left right until it's at an element greater than the pivot.
                // Move the right left until it's at an element less than the pivot.
                // If left and right cross, we can terminate, otherwise swap and continue.
                //
                // As each pass of the outer loop increments left at least once,
                // and decrements right at least once, this loop has to terminate.

                do  {
                    left++;
                } while ((left <= hi) && (compare(left, lo) <= 0));

                do  {
                    right--;
                } while ((right > lo) && (compare(right, lo) >= 0));

                if (right < left)
                    break;

                swap(left, right);
            }

            // move the pivot after the lower partition
            swap(lo, right);

            // The array is now in three partions:
            //  1. left partition   : i in [lo, right), elements less than or equal to pivot
            //  2. center partition : i in [right, left], elements equal to pivot
            //  3. right partition  : i in (left, hi], elements greater than pivot
            // NOTE : [ means the range includes the lower bounds, ( means it excludes it, with the same for ] and ).

            // Many quick sorts recurse into the left partition, and then the right.
            // The worst case of this can lead to a stack depth of size -- for instance,
            // the left is empty, the center is just the pivot, and the right is everything else.
            //
            // If you recurse into the smaller partition first, then the worst case is an
            // equal partitioning, which leads to a depth of log2(size).
            if ((right - 1 - lo) >= (hi - left))
            {
                if ((lo + 1) < right)
                {
                    stk[stkptr].lo = lo;
                    stk[stkptr].hi = right - 1;
                    ++stkptr;
                }

                if (left < hi)
                {
                    lo = left;
                    goto recurse;
                }
            }
            else
            {
                if (left < hi)
                {
                    stk[stkptr].lo = left;
                    stk[stkptr].hi = hi;
                    ++stkptr;
                }

                if ((lo + 1) < right)
                {
                    hi = right - 1;
                    goto recurse;           /* do small recursion */
                }
            }
        }

        // we reached the bottom of the well, pop the nested stack frame
        if (--stkptr >= 0)
        {
            lo = stk[stkptr].lo;
            hi = stk[stkptr].hi;
            goto recurse;
        }

        // we've returned to the top, so we are done!
        return;
    }

    /*
     * compare(j, k) as string's
     */
    int ArraySort::StringCompare(uint32_t j, uint32_t k) const
    {
        Atom x = get(j);
        Atom y = get(k);

        Stringp str_lhs = core->string(x);
        Stringp str_rhs = core->string(y);

        return str_rhs->Compare(*str_lhs);
    }

    /*
     * compare(j, k) as case insensitive string's
     */
    int ArraySort::CaseInsensitiveStringCompare(uint32_t j, uint32_t k) const
    {
        Atom x = get(j);
        Atom y = get(k);

        Stringp str_lhs = core->string(x)->toLowerCase();
        Stringp str_rhs = core->string(y)->toLowerCase();

        return str_rhs->Compare(*str_lhs);
    }

    /*
     * compare(j, k) using an actionscript function
     */
    int ArraySort::ScriptCompareCompatible(uint32_t j, uint32_t k) const
    {
        ScriptObject *o = (ScriptObject*) AvmCore::atomToScriptObject(cmpActionScript);

        // Coercing result to integer may result in inccorect change of sign.
        // See bug 532454.
        Atom args[3] = { d->atom(), get(j), get(k) };
        double result = AvmCore::toInteger(o->call(2, args));
        return (result > 0 ? 1 : (result < 0 ? -1 : 0));
    }

    /*
     * compare(j, k) using an actionscript function
     */
    int ArraySort::ScriptCompareCorrect(uint32_t j, uint32_t k) const
    {
        // todo must figure out the kosher way to invoke
        // callbacks like the sort comparator.

        // todo what is thisAtom supposed to be for the
        // comparator?  Passing in the array for now.

        ScriptObject *o = (ScriptObject*) AvmCore::atomToScriptObject(cmpActionScript);

        // cn: don't use AvmCore::integer on result of call.  The returned
        //  value could be bigger than 2^32 and toInt32 will return the
        //  ((value % 2^32) - 2^31), which could change the intended sign of the number.

        Atom args[3] = { d->atom(), get(j), get(k) };
        double result = AvmCore::number(o->call(2, args));
        return (result > 0 ? 1 : (result < 0 ? -1 : 0));
    }

    /*
     * compare(j, k) as numbers
     */
    int ArraySort::NumericCompareCompatible(uint32_t j, uint32_t k) const
    {
        Atom atmj = get(j);
        Atom atmk = get(k);
        // Integer checks makes an int array sort about 3x faster.
        // A double array sort is 5% slower because of this overhead
        if (atomIsBothIntptr(atmj, atmk))
        {
            // This is incorrect, see bugzilla 524122.  NumericCompareCorrect, below, fixes the bug.
            return ((int)atmj - (int)atmk);
        }

        double x = AvmCore::number(atmj);
        double y = AvmCore::number(atmk);
        double diff = x - y;

        if (diff == diff) { // same as !isNaN
            return (diff < 0) ? -1 : ((diff > 0) ? 1 : 0);
        } else if (!MathUtils::isNaN(y)) {
            return 1;
        } else if (!MathUtils::isNaN(x)) {
            return -1;
        } else {
            return 0;
        }
    }

    /*
     * compare(j, k) as numbers
     */
    int ArraySort::NumericCompareCorrect(uint32_t j, uint32_t k) const
    {
        Atom atmj = get(j);
        Atom atmk = get(k);
        // Integer checks makes an int array sort about 3x faster.
        // A double array sort is 5% slower because of this overhead
        if (atomIsBothIntptr(atmj, atmk))
        {
            // Must convert to native values.  Just subtracting the atoms may lead to
            // overflows which result in the incorrect sign being returned.  See
            // NumericCompareCompatible, above.
            intptr_t tmp = atomGetIntptr(atmj) - atomGetIntptr(atmk);
#ifdef AVMPLUS_64BIT
            // On 64-bit systems atoms carry more than 32 significant bits of integer
            // data, so we need to be careful.
            if (tmp < 0) return -1;
            if (tmp == 0) return 0;
            return 1;
#else
            return int(tmp);
#endif
        }
        
        double x = AvmCore::number(atmj);
        double y = AvmCore::number(atmk);
        double diff = x - y;
        
        if (diff == diff) { // same as !isNaN
            return (diff < 0) ? -1 : ((diff > 0) ? 1 : 0);
        } else if (!MathUtils::isNaN(y)) {
            return 1;
        } else if (!MathUtils::isNaN(x)) {
            return -1;
        } else {
            return 0;
        }
    }
    
    ScriptObject* ArraySort::toFieldObject(Atom atom) const
    {
        if (atomKind(atom) != kObjectType)
        {
            #if 0
            /* cn: ifdefed out, not sure what the intent was here, but calling code in FieldCompare
             *  does null checking, apparently expecting this function to return null when the item
             *  isn't an object (and thus can't have custom properties added to it). */
            // TypeError in ECMA
            toplevel->throwTypeError(
                       (atom == undefinedAtom) ? kConvertUndefinedToObjectError :
                                            kConvertNullToObjectError);
            #endif
            return NULL;
        }
        return AvmCore::atomToScriptObject(atom);
    }

    /*
     * FieldCompare is for Array.sortOn()
     */
    inline int ArraySort::FieldCompare(uint32_t lhs, uint32_t rhs) const
    {
        Atom j, k;
        int opt = options;
        int result = 0;

        j = get(lhs);
        k = get(rhs);

        ScriptObject* obj_j = toFieldObject(j);
        ScriptObject* obj_k = toFieldObject(k);

        if (!(obj_j && obj_k))
        {
            if (obj_k) {
                result = 1;
            } else if (obj_j) {
                result = -1;
            } else {
                result = 0;
            }
            return (opt & kDescending) ? -result : result;
        }

        for (uint32_t i = 0; i < numFields; i++)
        {
            Stringp name = fields[i].name;
            // NOTE compare the names of the caller's version
            Multiname mname(core->findPublicNamespace(), name);

            opt = fields[i].options; // override the group defaults with the current field

            Atom x = toplevel->getproperty(obj_j->atom(), &mname, obj_j->vtable);
            Atom y = toplevel->getproperty(obj_k->atom(), &mname, obj_k->vtable);

            bool def_x = defined(x);
            bool def_y = defined(y);

            if (!(def_x && def_y))
            {
                // ECMA 262 : Section 15.4.4.11 lists the rules.
                // There is a difference between the object has a property
                // with value undefined, and it does not have the property,
                // for which getAtomProperty() returns undefined.

                // def_x implies has_x
                // def_y implies has_y

                if (def_y) {
                    result = 1;
                } else if (def_x) {
                    result = -1;
                } else {
                    bool has_x = (toplevel->getBinding(obj_j->vtable->traits, &mname) != BIND_NONE) || obj_j->hasStringProperty(name);
                    bool has_y = (toplevel->getBinding(obj_k->vtable->traits, &mname) != BIND_NONE) || obj_k->hasStringProperty(name);

                    if (!has_x && has_y) {
                        result = 1;
                    } else if (has_x && !has_y) {
                        result = -1;
                    } else {
                        result = 0;
                    }
                }
            } else if (opt & kNumeric) {
                double lhs = AvmCore::number(x);
                double rhs = AvmCore::number(y);
                double diff = lhs - rhs;

                if (diff == diff) { // same as !isNaN
                    result = (diff < 0) ? -1 : ((diff > 0) ? 1 : 0);
                } else if (!MathUtils::isNaN(rhs)) {
                    result = 1;
                } else if (!MathUtils::isNaN(lhs)) {
                    result = -1;
                } else {
                    result = 0;
                }
            }
            else
            {
                Stringp str_lhs = core->string(x);
                Stringp str_rhs = core->string(y);

                if (opt & kCaseInsensitive)
                {
                    str_lhs = str_lhs->toLowerCase();
                    str_rhs = str_rhs->toLowerCase();
                }

                result = str_rhs->Compare(*str_lhs);
            }

            if (result != 0)
                break;
        }

        if (opt & kDescending)
            return -result;
        else
            return result;
    }

    /**
     * Array.prototype.sort()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */

    // thisAtom is object to process
    // 1st arg of args is a function or a number
    // 2nd arg of args is a number
    //
    // valid AS3 syntax:
    // sort()
    // sort(function object)
    // sort(number flags)
    // sort(function object, number flags)

    // This takes a args object because there is no way to distinguigh between sort()
    // and sort(undefined, 0) if we take default parameters.
    /*static*/ Atom ArrayClass::generic_sort(Toplevel* toplevel, Atom thisAtom, ArrayObject *args)
    {
        AvmCore* core = toplevel->core();
        if (!AvmCore::isObject(thisAtom))
            return undefinedAtom;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        ArraySort::CompareFuncPtr compare = NULL;
        ArraySort::CompareFuncPtr altCompare = NULL;

        Atom cmp = undefinedAtom;
        int opt = 0;
        if (args->getLength() >= 1)
        {
            // function ptr
            Atom arg0 = args->getUintProperty(0);
            if (AvmCore::isObject (arg0))
            {
                // make sure the sort function is callable
                cmp = arg0;
                toplevel->coerce(cmp, core->traits.function_itraits);
                compare = core->currentBugCompatibility()->bugzilla532454 ?
                                ArraySort::ScriptCompareFuncCorrect :
                                ArraySort::ScriptCompareFuncCompatible;
                if (args->getLength() >= 2)
                {
                    Atom arg1 = args->getUintProperty(1);
                    if (core->isNumberOrFloat(arg1))
                    {
                        opt = AvmCore::integer(arg1);
                    }
                    else
                    {
                        // throw exception (not a Number)
                        toplevel->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(arg1), core->toErrorString(core->traits.number_itraits));
                    }
                }
            }
            else if (core->isNumberOrFloat(arg0))
            {
                opt = AvmCore::integer(arg0);
            }
            else
            {
                // throw exception (not a function)
                toplevel->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(arg0), core->toErrorString(core->traits.function_itraits));
            }
        }

        if (cmp == undefinedAtom)
        {
            if (opt & ArraySort::kNumeric) {
                compare = core->currentBugCompatibility()->bugzilla524122 ?
                                ArraySort::NumericCompareFuncCorrect :
                                ArraySort::NumericCompareFuncCompatible;
            } else if (opt & ArraySort::kCaseInsensitive) {
                compare = ArraySort::CaseInsensitiveStringCompareFunc;
            } else {
                compare = ArraySort::StringCompareFunc;
            }
        }

        if (opt & ArraySort::kDescending) {
            altCompare = compare;
            compare = ArraySort::DescendingCompareFunc;
        }

        Atom result;
        ArraySort sort(result, toplevel->arrayClass(), d, opt, compare, altCompare, cmp);

        return result;
    }


    /**
     * Array.prototype.sortOn()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */
    /*static*/ Atom ArrayClass::generic_sortOn(Toplevel* toplevel, Atom thisAtom, Atom namesAtom, Atom optionsAtom)
    {
        AvmCore* core = toplevel->core();
        if (!AvmCore::isObject(thisAtom))
            return undefinedAtom;
        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);

        // Possible combinations:
        //  Array.sortOn(String)
        //  Array.sortOn(String, options)
        //  Array.sortOn(Array of String)
        //  Array.sortOn(Array of String, options)
        //  Array.sortOn(Array of String, Array of options)

        //  What about options which must be global, such as kReturnIndexedArray?
        //  Perhaps it is the union of all field's options?

        ArraySort::FieldName *fn = NULL;
        GC::AllocaAutoPtr fn_autoptr;
        uint32_t nFields = 0;
        int options = 0;
        
        if (toplevel->builtinClasses()->get_StringClass()->isType(namesAtom))
        {
            nFields = 1;

            options = AvmCore::integer(optionsAtom);

            fn = (ArraySort::FieldName*) avmStackAllocArray(core, fn_autoptr, nFields, sizeof(ArraySort::FieldName));
            fn[0].name = core->internString(namesAtom);
            fn[0].options = options;
        }
        else if (toplevel->builtinClasses()->get_ArrayClass()->isType(namesAtom))
        {
            ArrayObject *obj = (ArrayObject *)AvmCore::atomToScriptObject(namesAtom);

            nFields = obj->getLength();
            fn = (ArraySort::FieldName*) avmStackAllocArray(core, fn_autoptr, nFields, sizeof(ArraySort::FieldName));

            for (uint32_t i = 0; i < nFields; i++)
            {
                fn[i].name = core->intern(obj->getUintProperty(i));
                fn[i].options = 0;
            }

            if (toplevel->builtinClasses()->get_ArrayClass()->isType(optionsAtom))
            {
                ArrayObject *obj = (ArrayObject *)AvmCore::atomToScriptObject(optionsAtom);
                uint32_t nOptions = obj->getLength();
                if (nOptions == nFields)
                {
                    // The first options are used for uniqueSort and returnIndexedArray option
                    options = AvmCore::integer(obj->getUintProperty(0));
                    for (uint32_t i = 0; i < nFields; i++)
                    {
                        fn[i].options = AvmCore::integer(obj->getUintProperty (i));
                    }
                }
            }
            else
            {
                options = AvmCore::integer(optionsAtom);
                for (uint32_t i = 0; i < nFields; i++)
                {
                    fn[i].options = options;
                }
            }
        }

        Atom result;
        ArraySort sort(result, toplevel->arrayClass(), d, options, ArraySort::FieldCompareFunc, NULL, undefinedAtom, nFields, fn);
        return result;
    }

    /**
     * Array.prototype.splice()
     * TRANSFERABLE: Needs to support generic objects as well as Array objects
     */

    // Spidermonkey behavior that we are mimicking...
    // splice() - no arguments - return undefined
    // splice(org arg) coerce the input to a number (otherwise it's zero) normal behavior
    // splice (two args) - coerce both args to numbers (otherwise they are zero)
    /*static*/ ArrayObject* ArrayClass::generic_splice(Toplevel* toplevel, Atom thisAtom, ArrayObject* args)
    {
        // This will return null but this case should never get hit (see Array.as)
        if (!args->getLength())
            return 0;

        if (!AvmCore::isObject(thisAtom))
            return 0;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);

        uint32_t len = d->getLengthProperty();

        uint32_t insertPoint = NativeObjectHelpers::ClampIndex(AvmCore::toInteger(args->getUintProperty(0)),len);

        double d_deleteCount = args->getLength() > 1 ? AvmCore::toInteger(args->getUintProperty(1)) : (len - insertPoint);
        uint32_t deleteCount = (d_deleteCount < 0) ? 0 : AvmCore::integer_d(d_deleteCount);
        if (deleteCount > (len - insertPoint)) {
            deleteCount = len - insertPoint;
        }
        uint32_t end = insertPoint + deleteCount;

        uint32_t insertCount = (args->getLength() > 2) ? (args->getLength() - 2) : 0;
        long l_shiftAmount = (long)insertCount - (long) deleteCount; // long because result could be negative
        uint32_t shiftAmount;

        ArrayObject* a = toArray(thisAtom);
        ArrayObject* out;
        if (a && (out = a->try_splice(insertPoint, insertCount, deleteCount, args, 2)) != NULL)
        {
            return out;
        }
        // Copy out the elements we are going to remove
        out = toplevel->arrayClass()->newArray(deleteCount);
        for (uint32_t i=0; i< deleteCount; i++) {
            out->setUintProperty(i, d->getUintProperty(i+insertPoint));
        }

        // delete items by shifting elements past end (of delete) by l_shiftAmount
        if (l_shiftAmount < 0) {
            // Shift the remaining elements down
            shiftAmount = (uint32_t)(-l_shiftAmount);

            for (uint32_t i=end; i<len; i++) {
                d->setUintProperty(i-shiftAmount, d->getUintProperty(i));
            }

            // delete top elements here to match ECMAscript spec (generic object support)
            for (uint32_t i=len-shiftAmount; i<len; i++) {
                d->delUintProperty (i);
            }
        } else {
            // Shift the remaining elements up.
            shiftAmount = (uint32_t)l_shiftAmount;

            for (uint32_t i=len; i > end; ) { // Note: i is unsigned, can't check if --i >=0.
                --i;
                d->setUintProperty(i+shiftAmount, d->getUintProperty(i));
            }
        }

        // Add the items to insert
        for (uint32_t i=0; i<insertCount; i++) {
            d->setUintProperty(insertPoint+i, args->getUintProperty(i + 2));
        }

        // shrink array if shiftAmount is negative
        d->setLengthProperty(len+l_shiftAmount);

        return out;
    }

    ArrayObject* ArrayClass::newarray(Atom* argv, int argc)
    {
        return ArrayObject::createSimple(core()->GetGC(), ivtable(), prototypePtr(), argv, argc);
    }

    ArrayObject* ArrayClass::newArray(uint32_t capacity)
    {
        return ArrayObject::createSimple(core()->GetGC(), ivtable(), prototypePtr(), capacity);
    }

#ifdef VMCFG_AOT
    template <typename ADT>
    ArrayObject* ArrayClass::newArray(MethodEnv *env, ADT argDesc, va_list ap)
    {
        uint32_t argc = argDescArgCount(argDesc);
        // removed assert, argc is unsigned!
        return ArrayObject::createSimple<ADT>(core()->GetGC(), ivtable(), prototypePtr(), env, argDesc, argc, ap);
    }

    template ArrayObject* ArrayClass::newArray(MethodEnv *env, uint32_t argDesc, va_list ap);
    template ArrayObject* ArrayClass::newArray(MethodEnv *env, char* argDesc, va_list ap);
#endif

    /*static*/ int ArrayClass::generic_indexOf(Atom thisAtom, Atom searchElement, int startIndex)
    {
        if (!AvmCore::isObject(thisAtom))
            return -1;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        uint32_t start = NativeObjectHelpers::ClampIndexInt(startIndex, len);

        for (uint32_t i = start; i < len; i++)
        {
            Atom atom = d->getUintProperty(i);
            if (AvmCore::stricteq(atom, searchElement) == trueAtom)
                return i;
        }

        return -1;
    }

    /*static*/ int ArrayClass::generic_lastIndexOf(Atom thisAtom, Atom searchElement, int startIndex)
    {
        if (!AvmCore::isObject(thisAtom))
            return -1;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        int start = NativeObjectHelpers::ClampIndexInt(startIndex, len);
        if (start == int(len))
            start--;

        for (int i = start; i >= 0; i--)
        {
            Atom atom = d->getUintProperty(i);
            if (AvmCore::stricteq(atom, searchElement) == trueAtom)
                return i;
        }

        return -1;
    }

    /*static*/ bool ArrayClass::generic_every(Toplevel* toplevel, Atom thisAtom, ScriptObject *callback, Atom thisObject)
    {
        if (!AvmCore::isObject(thisAtom) || !callback)
            return true;

        if (callback->isMethodClosure() && !AvmCore::isNull(thisObject))
        {
            toplevel->throwTypeError(kArrayFilterNonNullObjectError);
        }

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        AvmCore* core = toplevel->core();
        for (uint32_t i = 0; i < len; i++)
        {
            // If thisObject is null, the call function will substitute the global object.
            // args are modified in place by callee
            Atom args[4] = { thisObject,
                d->getUintProperty(i),  // element
                core->uintToAtom(i),    // index
                thisAtom
            };
            Atom result = callback->call(3, args);
            if (result != trueAtom)
                return false;
        }

        return true;
    }

    /*static*/ ArrayObject* ArrayClass::generic_filter(Toplevel* toplevel, Atom thisAtom, ScriptObject *callback, Atom thisObject)
    {
        ArrayObject *r = toplevel->arrayClass()->newArray();

        if (!AvmCore::isObject(thisAtom) || !callback)
            return r;

        if (callback->isMethodClosure() && !AvmCore::isNull(thisObject))
        {
            toplevel->throwTypeError(kArrayFilterNonNullObjectError);
        }

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        AvmCore* core = toplevel->core();
        for (uint32_t i = 0; i < len; i++)
        {
            //  The Array and/or the args may be modified by the caller,
            //  so get a local reference to the element.
            Atom element = d->getUintProperty(i);
            // If thisObject is null, the call function will substitute the global object
            // args are modified in place by callee
            Atom args[4] = {
                thisObject,
                element,
                core->uintToAtom(i), // index
                thisAtom
            };
            Atom result = callback->call(3, args);
            if (result == trueAtom)
                r->push(&element, 1);
        }

        return r;
    }

    /*static*/ void ArrayClass::generic_forEach(Toplevel* toplevel, Atom thisAtom, ScriptObject *callback, Atom thisObject)
    {
        if (!AvmCore::isObject(thisAtom) || !callback)
            return;

        if (callback->isMethodClosure() && !AvmCore::isNull(thisObject))
        {
            toplevel->throwTypeError(kArrayFilterNonNullObjectError);
        }

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        AvmCore* core = toplevel->core();
        for (uint32_t i = 0; i < len; i++)
        {
            // If thisObject is null, the call function will substitute the global object
            // args are modified in place by callee
            Atom args[4] = { thisObject,
                d->getUintProperty(i),  // element
                core->uintToAtom(i),    // index
                thisAtom
            };
            callback->call(3, args);
        }
    }

    /*static*/ bool ArrayClass::generic_some(Toplevel* toplevel, Atom thisAtom, ScriptObject *callback, Atom thisObject)
    {
        if (!AvmCore::isObject(thisAtom) || !callback)
            return false;

        if (callback->isMethodClosure() && !AvmCore::isNull(thisObject))
        {
            toplevel->throwTypeError(kArrayFilterNonNullObjectError);
        }

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        AvmCore* core = toplevel->core();
        for (uint32_t i = 0; i < len; i++)
        {
            // If thisObject is null, the call function will substitute the global object
            // args are modified in place by callee
            Atom args[4] = {
                thisObject,
                d->getUintProperty(i),  // element
                core->uintToAtom(i),    // index
                thisAtom
            };
            Atom result = callback->call(3, args);
            if (result == trueAtom)
                return true;
        }

        return false;
    }

    /*static*/ ArrayObject* ArrayClass::generic_map(Toplevel* toplevel, Atom thisAtom, ScriptObject *callback, Atom thisObject)
    {
        ArrayObject *r = toplevel->arrayClass()->newArray();

        if (!AvmCore::isObject(thisAtom) || !callback)
            return r;

        ScriptObject *d = AvmCore::atomToScriptObject(thisAtom);
        uint32_t len = d->getLengthProperty();

        AvmCore* core = toplevel->core();
        for (uint32_t i = 0; i < len; i++)
        {
            // If thisObject is null, the call function will substitute the global object
            // args are modified in place by callee
            Atom args[4] = {
                thisObject,
                d->getUintProperty(i),  // element
                core->uintToAtom(i),    // index
                thisAtom
            };
            Atom result = callback->call(3, args);
            r->push(&result, 1);
        }

        return r;
    }

    /* static */ uint32_t ArrayClass::generic_unshift(Toplevel* /*toplevel*/, Atom thisAtom, ArrayObject* args)
    {
        ArrayObject* a = toArray(thisAtom);
        if (!a || !a->try_unshift(args))
        {
            for (uint32_t i = args->getLength() ; i > 0; i--)
            {
                Atom atom = args->getUintProperty(i - 1);
                a->unshift(&atom, 1);
            }
        }
        return a->getLengthProperty();
    }

    /*static*/
    ScriptObject* FASTCALL ArrayClass::createUnsubclassedInstanceProc(ClassClosure* cls)
    {
        return new (cls->gc(), MMgc::kExact, cls->getExtraSize())
            ArrayObject(cls->ivtable(),
                        cls->prototypePtr(),
                        /*capacity = */0,
                        /*simple   = */true);
    }

}
