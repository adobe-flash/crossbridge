/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_NumberClass__
#define __avmplus_NumberClass__


namespace avmplus
{
    /**
     * class Number
     */
    class GC_AS3_EXACT(NumberClass, ClassClosure)
    {
    protected:
        NumberClass(VTable* cvtable);
    public:
        REALLY_INLINE static NumberClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) NumberClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv)
        {
            // Note: SpiderMonkey returns 0 for Number() with no args
            return construct(argc,argv);
        }

        Stringp _convert(double n, int precision, int mode);
        Stringp _numberToString(double n, int radix);
        double _minValue();

        double abs(double x);
        double acos(double x);
        double asin(double x);
        double atan(double x);
        double atan2(double y, double x);
        double ceil(double x);
        double cos(double x);
        double exp(double x);
        double floor(double x);
        double log(double x);
        double pow(double x, double y);
        double random();
        double round(double x);
        double sin(double x);
        double sqrt(double x);
        double tan(double x);
        double min(double x, double y, const Atom* argv, uint32_t argc);
        double max(double x, double y, const Atom* argv, uint32_t argc);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(NumberClass)

        DECLARE_SLOTS_NumberClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_NumberClass__ */
