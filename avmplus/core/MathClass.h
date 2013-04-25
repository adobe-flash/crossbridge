/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_MathClass__
#define __avmplus_MathClass__


namespace avmplus
{
    /**
     * class Math
     */
    class GC_AS3_EXACT(MathClass, ClassClosure)
    {
    protected:
        MathClass(VTable* cvtable);
    public:
        REALLY_INLINE static MathClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) MathClass(cvtable);
        }

#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~MathClass()
        {
            seed.uValue = 0;
            seed.uXorMask = 0;
            seed.uSequenceLength = 0;
        }
#endif

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

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
        double _min(double x, double y);
        double _max(double x, double y);

        inline double min2(double x, double y) { return _min(x, y); }
        inline double max2(double x, double y) { return _max(x, y); }

        // cn:  max/min declared with rest args so we can implement ES3 spec'd length property of 2
        //      and still allow any number of arguments.
        double max(double x, double y, const Atom* argv, uint32_t argc);
        double min(double x, double y, const Atom* argv, uint32_t argc);

        // Shared between Math and Number.
        static double maxImpl(double x, double y, const Atom* argv, uint32_t argc);
        static double minImpl(double x, double y, const Atom* argv, uint32_t argc);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(MathClass)

        TRandomFast seed;

        DECLARE_SLOTS_MathClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_MathClass__ */
