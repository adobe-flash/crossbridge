/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include <setjmp.h>


extern "C"
{
    __declspec(naked)
    int __cdecl _setjmp3(jmp_buf /*jmpbuf*/, int /*arg*/)
    {
        _asm
        {
            mov edx, [esp+4]
            mov DWORD PTR [edx],ebp
            mov DWORD PTR [edx+4],ebx
            mov DWORD PTR [edx+8],edi
            mov DWORD PTR [edx+12],esi
            mov DWORD PTR [edx+16],esp
            mov eax, DWORD PTR [esp]
            mov DWORD PTR [edx+20],eax
            mov DWORD PTR [edx+24],0xFFFFFFFF
            mov DWORD PTR [edx+28],0xFFFFFFFF
            mov DWORD PTR [edx+32],0x56433230
            mov DWORD PTR [edx+36],0
            sub eax,eax
            ret
        }
    }

    // Disable the "ebp was modified" warning.
    // We really do want to modify it.
    #pragma warning ( disable : 4731 )

    // flow in or out of inline asm code suppresses global optimization
    #pragma warning ( disable : 4740 )

    __declspec(noreturn)
    void __cdecl longjmp(jmp_buf jmpbuf, int result)
    {
        _asm
        {
            mov edx,[jmpbuf]
            mov eax,[result]
            mov ebp,DWORD PTR [edx]
            mov ebx,DWORD PTR [edx+4]
            mov edi,DWORD PTR [edx+8]
            mov esi,DWORD PTR [edx+12]
            mov esp,DWORD PTR [edx+16]
            add esp,4
            jmp DWORD PTR [edx+20]
        }
    }

    #pragma warning ( default : 4740 )
}
