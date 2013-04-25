/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_FloatClass__
#define __avmplus_FloatClass__
#ifdef VMCFG_FLOAT

namespace avmplus
{
    /**
     * class Float
     */
    class GC_AS3_EXACT(FloatClass, ClassClosure)
    {
    protected:
        FloatClass(VTable* cvtable);
    public:
        REALLY_INLINE static FloatClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) FloatClass(cvtable);
        }
                
        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv)
        {
            // Note: Tamarin returns 0 for Float() with no args
            return construct(argc,argv);
        }
        
        float abs(float x);
        float acos(float x);
        float asin(float x);
        float atan(float x);
        float atan2(float y, float x);
        float ceil(float x);
        float cos(float x);
        float exp(float x);
        float floor(float x);
        float log(float x);
        float pow(float x, float y);
        float random();
        float reciprocal(float x);
        float round(float x);
        float rsqrt(float x);
        float sin(float x);
        float sqrt(float x);
        float tan(float x);
        float _minValue();
        Stringp _convert(float n, int precision, int mode);
        Stringp _floatToString(float n, int radix);
       
        // ------------------------ DATA SECTION BEGIN
    private:    
        GC_NO_DATA(FloatClass)
        
        DECLARE_SLOTS_FloatClass;
        // ------------------------ DATA SECTION END
    };
}
#endif
#endif /* __avmplus_FloatClass__ */
