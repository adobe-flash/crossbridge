/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_DateObject__
#define __avmplus_DateObject__


namespace avmplus
{
    /**
     * The DateObject class is the C++ implementation of instances
     * of the Date class in the ECMA-262 Specification.
     */
    class GC_AS3_EXACT(DateObject, ScriptObject)
    {
    protected:
        /**
         * This variant is only used for creating the prototype
         */
        DateObject(VTable* ivtable, ScriptObject *objectPrototype)
            : ScriptObject(ivtable, objectPrototype)
        {
            SAMPLE_FRAME("Date", core());
            AvmAssert(traits()->getSizeOfInstance() == sizeof(DateObject));
            date.setTime(MathUtils::kNaN);  // Date.prototype should be the "Invalid Date"
        }

        /**
         * This variant is used for creating all dates other
         * than Date.prototype
         */
        DateObject(DateClass *type, const Date& date)
            : ScriptObject(type->ivtable(), type->prototypePtr())
        {
            AvmAssert(traits()->getSizeOfInstance() == sizeof(DateObject));
            this->date = date;
        }

    public:
        REALLY_INLINE static DateObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* prototype)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) DateObject(ivtable, prototype);
        }

        REALLY_INLINE static DateObject* create(MMgc::GC* gc, DateClass* type, const Date& date)
        {
            return new (gc, MMgc::kExact, type->ivtable()->getExtraSize()) DateObject(type, date);
        }
        
        // renamed to avoid hiding ScriptObject::toString
        Stringp _toString(int index);
        double AS3_valueOf();
        double _setTime(double value);
        double _get(int index);
        double _set(int index, Atom* argv, int argc);

        double AS3_getUTCFullYear() { return _get(Date::kUTCFullYear); }
        double AS3_getUTCMonth() { return _get(Date::kUTCMonth); }
        double AS3_getUTCDate() { return _get(Date::kUTCDate); }
        double AS3_getUTCDay() { return _get(Date::kUTCDay); }
        double AS3_getUTCHours() { return _get(Date::kUTCHours); }
        double AS3_getUTCMinutes() { return _get(Date::kUTCMinutes); }
        double AS3_getUTCSeconds() { return _get(Date::kUTCSeconds); }
        double AS3_getUTCMilliseconds() { return _get(Date::kUTCMilliseconds); }
        double AS3_getFullYear() { return _get(Date::kFullYear); }
        double AS3_getMonth() { return _get(Date::kMonth); }
        double AS3_getDate() { return _get(Date::kDate); }
        double AS3_getDay() { return _get(Date::kDay); }
        double AS3_getHours() { return _get(Date::kHours); }
        double AS3_getMinutes() { return _get(Date::kMinutes); }
        double AS3_getSeconds() { return _get(Date::kSeconds); }
        double AS3_getMilliseconds() { return _get(Date::kMilliseconds); }
        double AS3_getTimezoneOffset() { return _get(Date::kTimezoneOffset); }
        double AS3_getTime() { return _get(Date::kTime); }

        double _setUTCFullYear(Atom* argv, int argc) { return _set(-1, argv, argc); }
        double _setUTCMonth(Atom* argv, int argc) { return _set(-2, argv, argc); }
        double _setUTCDate(Atom* argv, int argc) { return _set(-3, argv, argc); }
        double _setUTCHours(Atom* argv, int argc) { return _set(-4, argv, argc); }
        double _setUTCMinutes(Atom* argv, int argc) { return _set(-5, argv, argc); }
        double _setUTCSeconds(Atom* argv, int argc) { return _set(-6, argv, argc); }
        double _setUTCMilliseconds(Atom* argv, int argc) { return _set(-7, argv, argc); }

        double _setFullYear(Atom* argv, int argc) { return _set(1, argv, argc); }
        double _setMonth(Atom* argv, int argc) { return _set(2, argv, argc); }
        double _setDate(Atom* argv, int argc) { return _set(3, argv, argc); }
        double _setHours(Atom* argv, int argc) { return _set(4, argv, argc); }
        double _setMinutes(Atom* argv, int argc) { return _set(5, argv, argc); }
        double _setSeconds(Atom* argv, int argc) { return _set(6, argv, argc); }
        double _setMilliseconds(Atom* argv, int argc) { return _set(7, argv, argc); }

#ifdef AVMPLUS_VERBOSE
    public:
        PrintWriter& print(PrintWriter& prw) const;
#endif
        
    // ------------------------ DATA SECTION BEGIN
    public:
        GC_DATA_BEGIN(DateObject)

        Date date;           // Not subject to GC, contains no pointers

        GC_DATA_END(DateObject)

    private:
        DECLARE_SLOTS_DateObject;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_DateObject__ */
