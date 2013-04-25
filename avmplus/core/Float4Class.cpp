/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"
#include <float.h>
#ifdef VMCFG_FLOAT

namespace avmplus
{
    Float4Class::Float4Class(VTable* cvtable)
    : ClassClosure(cvtable)
    {
        toplevel()->_float4Class = this;
        // prototype objects are always vanilla objects.
        createVanillaPrototype();
    }

    void Float4Class::fromComponents(float4_t* ret, float x, float y, float z, float w)
    {
        float* retp = reinterpret_cast<float*>(ret);
        retp[0] = x; retp[1] = y; retp[2] = z; retp[3] = w;
    }
    
    Atom Float4Class::construct(int argc, Atom* argv)
    {
        // Float called as constructor creates new Float instance
        // Note: new Float() with no args returns 0 - as does Number
        if (argc == 0)
            return core()->float4Atom(zeroIntAtom);  

        if(argc == 1)
            return core()->float4Atom(argv[1]);
        
        if(argc != 4)
            vtable->init->argcError(argc);

        float4_t val = { AvmCore::singlePrecisionFloat(argv[1]),
                         AvmCore::singlePrecisionFloat(argv[2]),
                         AvmCore::singlePrecisionFloat(argv[3]),
                         AvmCore::singlePrecisionFloat(argv[4])};
        return core()->float4ToAtom(val);
    }

    // OPTIMIZEME: These are generic implementations.  Faster MMX/SSE/Neon implementations are
    // desirable but mostly in jitted code, so these may be OK for the interpreter and calls
    // from the runtime.  Still, if there are precision differences between the C++ implementations
    // and the native MMX/SSE/Neon implementations then we'd be better off deferring to the
    // intrinsics here so that there's no divergence between jitted and interpreted code.
    //
    // FIXME: Bugzilla 703605 has more information.

#define F4OP2(res,s1,s2,op) fromComponents(res, op( f4_x(s1),f4_x(s2)) , op(f4_y(s1), f4_y(s2)) , op( f4_z(s1), f4_z(s2)) , op(f4_w(s1), f4_w(s2)))
#define F4OP2SEL(res,s1,s2,op) fromComponents( res,               \
                             (f4_x(s1) op f4_x(s2)) ? 1.0f : 0.0f , \
                             (f4_y(s1) op f4_y(s2)) ? 1.0f : 0.0f , \
                             (f4_z(s1) op f4_z(s2)) ? 1.0f : 0.0f , \
                             (f4_w(s1) op f4_w(s2)) ? 1.0f : 0.0f )
#define F4OP2SELe(res,s1,s2,op)  fromComponents( res,                     \
                             f4_x(s1) op f4_x(s2) ? f4_x(s1) : f4_x(s2) , \
                             f4_y(s1) op f4_y(s2) ? f4_y(s1) : f4_y(s2) , \
                             f4_z(s1) op f4_z(s2) ? f4_z(s1) : f4_z(s2) , \
                             f4_w(s1) op f4_w(s2) ? f4_w(s1) : f4_w(s2) )

#define F4OP1(res,s1,op, ... ) fromComponents(res,op(f4_x(s1) __VA_ARGS__), op(f4_y(s1) __VA_ARGS__), op(f4_z(s1) __VA_ARGS__), op(f4_w(s1) __VA_ARGS__))

    void Float4Class::isGreater(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SEL(result, x, y, >);
    }

    void Float4Class::isGreaterOrEqual(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SEL(result, x, y, >=);
    }

    void Float4Class::isLess(float4_t* result, const float4_t& x, const float4_t& y) 
    {
        F4OP2SEL(result, x, y, <);
    }
    
    void Float4Class::isLessOrEqual(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SEL(result, x, y, <=);
    }

    void Float4Class::isEqual(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SEL(result, x, y, ==);
    }

    void Float4Class::isNotEqual(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SEL(result, x, y, !=);
    }

    void Float4Class::abs(float4_t* result, const float4_t& x)
    {
        F4OP1(result, x,fabsf);
    }

    
    // FIXME (Bugzilla 703605): Probably not right for either NaN nor -0 vs +0
    void Float4Class::min(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SELe(result, x, y, <);
    }

    // FIXME (Bugzilla 703605): Probably not right for either NaN nor -0 vs +0
    void Float4Class::max(float4_t* result, const float4_t& x, const float4_t& y)
    {
        F4OP2SELe(result, x, y, >);
    }

   // FIXME (Bugzilla 703605): Here we allow reduced precision so we almost certainly want to delegate to the
    // native SIMD implementation to get consistent interpreter/JIT behavior.
    void Float4Class::reciprocal(float4_t* result, const float4_t& x)
    {
        F4OP1(result, x, 1.0f/);
    }

    // FIXME: (Bugzilla 703605) We want consistent interpreter/jit behavior, so we want to defer to the
    // machine primitive.
    void Float4Class::rsqrt(float4_t* result, const float4_t& x)
    {
        F4OP1(result, x, 1.0f/sqrtf);
    }

    void Float4Class::sqrt(float4_t* result, const float4_t& x)
    {
        F4OP1(result, x, sqrtf);
    }

    void Float4Class::normalize(float4_t* result, const float4_t& x)
    {
        float m = magnitude(x);
        F4OP1(result, x, , /m);
    }

    void Float4Class::cross(float4_t* result, const float4_t& a, const float4_t& b)
    {
        fromComponents(result, f4_y(a) * f4_z(b) - f4_z(a) * f4_y(b),
                            f4_z(a) * f4_x(b) - f4_x(a) * f4_z(b),
                            f4_x(a) * f4_y(b) - f4_y(a) * f4_x(b),
                               0.0f);
    }

    float Float4Class::dot(const float4_t& a, const float4_t& b)
    {
        return f4_x(a) * f4_x(b) +
               f4_y(a) * f4_y(b) +
               f4_z(a) * f4_z(b) +
               f4_w(a) * f4_w(b);
    }

    float Float4Class::dot2(const float4_t& a, const float4_t& b)
    {
        return f4_x(a) * f4_x(b) +
               f4_y(a) * f4_y(b);
    }
    
    float Float4Class::dot3(const float4_t& a, const float4_t& b)
    {
        return f4_x(a) * f4_x(b) +
               f4_y(a) * f4_y(b) +
               f4_z(a) * f4_z(b);
    }

    float Float4Class::magnitude(const float4_t& x)
    {
        return sqrtf(dot(x, x));
    }
    
    float Float4Class::magnitude2(const float4_t& x)
    {
        return sqrtf(dot2(x, x));
    }

    float Float4Class::magnitude3(const float4_t& x)
    {
        return sqrtf(dot3(x, x));
    }
    
    float Float4Class::distance(const float4_t& x, const float4_t& y)
    {
        return magnitude(f4_sub(x, y));
    }

    float Float4Class::distance2(const float4_t& x, const float4_t& y)
    {
        return magnitude2(f4_sub(x, y));
    }

    float Float4Class::distance3(const float4_t& x, const float4_t& y)
    {
        return magnitude3(f4_sub(x, y));
    }

    // "How" is really only one byte that specifies which field to get - two bits for each of the float4 positions.
    // i.e. two bits specify which of the x/y/z/w to return as "x", the next two bits specify what to return as "y" and so on.
    void Float4Class::_swizzle(float4_t* result, const float4_t& val, int32_t how)
    {
        AvmAssert(how >= 0 && how < 256);
#ifndef VMCFG_GENERIC_FLOAT4
#ifdef(VMCFG_SSE2)
#define CASE(x)  case x: result = _mm_shuffle_ps(val, val, x);
#else
#error Unknown target for swizzle!
#endif
        switch(how){ // must explicitly expand; the "shuffle" intrinsic doesn't take variables. 
            CASE(0);CASE(1);CASE(2);CASE(3);CASE(4);CASE(5);CASE(6);CASE(7);CASE(8);CASE(9);
            CASE(10);CASE(11);CASE(12);CASE(13);CASE(14);CASE(15);CASE(16);CASE(17);CASE(18);CASE(19);
            CASE(20);CASE(21);CASE(22);CASE(23);CASE(24);CASE(25);CASE(26);CASE(27);CASE(28);CASE(29);
            CASE(30);CASE(31);CASE(32);CASE(33);CASE(34);CASE(35);CASE(36);CASE(37);CASE(38);CASE(39);
            CASE(40);CASE(41);CASE(42);CASE(43);CASE(44);CASE(45);CASE(46);CASE(47);CASE(48);CASE(49);
            CASE(50);CASE(51);CASE(52);CASE(53);CASE(54);CASE(55);CASE(56);CASE(57);CASE(58);CASE(59);
            CASE(60);CASE(61);CASE(62);CASE(63);CASE(64);CASE(65);CASE(66);CASE(67);CASE(68);CASE(69);
            CASE(70);CASE(71);CASE(72);CASE(73);CASE(74);CASE(75);CASE(76);CASE(77);CASE(78);CASE(79);
            CASE(80);CASE(81);CASE(82);CASE(83);CASE(84);CASE(85);CASE(86);CASE(87);CASE(88);CASE(89);
            CASE(90);CASE(91);CASE(92);CASE(93);CASE(94);CASE(95);CASE(96);CASE(97);CASE(98);CASE(99);
            CASE(100);CASE(101);CASE(102);CASE(103);CASE(104);CASE(105);CASE(106);CASE(107);CASE(108);CASE(109);
            CASE(110);CASE(111);CASE(112);CASE(113);CASE(114);CASE(115);CASE(116);CASE(117);CASE(118);CASE(119);
            CASE(120);CASE(121);CASE(122);CASE(123);CASE(124);CASE(125);CASE(126);CASE(127);CASE(128);CASE(129);
            CASE(130);CASE(131);CASE(132);CASE(133);CASE(134);CASE(135);CASE(136);CASE(137);CASE(138);CASE(139);
            CASE(140);CASE(141);CASE(142);CASE(143);CASE(144);CASE(145);CASE(146);CASE(147);CASE(148);CASE(149);
            CASE(150);CASE(151);CASE(152);CASE(153);CASE(154);CASE(155);CASE(156);CASE(157);CASE(158);CASE(159);
            CASE(160);CASE(161);CASE(162);CASE(163);CASE(164);CASE(165);CASE(166);CASE(167);CASE(168);CASE(169);
            CASE(170);CASE(171);CASE(172);CASE(173);CASE(174);CASE(175);CASE(176);CASE(177);CASE(178);CASE(179);
            CASE(180);CASE(181);CASE(182);CASE(183);CASE(184);CASE(185);CASE(186);CASE(187);CASE(188);CASE(189);
            CASE(190);CASE(191);CASE(192);CASE(193);CASE(194);CASE(195);CASE(196);CASE(197);CASE(198);CASE(199);
            CASE(200);CASE(201);CASE(202);CASE(203);CASE(204);CASE(205);CASE(206);CASE(207);CASE(208);CASE(209);
            CASE(210);CASE(211);CASE(212);CASE(213);CASE(214);CASE(215);CASE(216);CASE(217);CASE(218);CASE(219);
            CASE(220);CASE(221);CASE(222);CASE(223);CASE(224);CASE(225);CASE(226);CASE(227);CASE(228);CASE(229);
            CASE(230);CASE(231);CASE(232);CASE(233);CASE(234);CASE(235);CASE(236);CASE(237);CASE(238);CASE(239);
            CASE(240);CASE(241);CASE(242);CASE(243);CASE(244);CASE(245);CASE(246);CASE(247);CASE(248);CASE(249);
            CASE(250);CASE(251);CASE(252);CASE(253);CASE(254);CASE(255);
#undef CASE
default: return _mm_set1_ps(0); // make all compilers happy
        }
#else       // No intrinsic available; do the shuffling "by hand"
        float* retp = reinterpret_cast<float*>(result);
        const float* valp = reinterpret_cast<const float*>(&val);
        for (int i = 0; i < 4; ++i) {
            *retp++ = valp[how & 3];
            how >>= 2;
        }
#endif 
    }
}
#endif
