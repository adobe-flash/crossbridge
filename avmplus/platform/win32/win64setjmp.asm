; This Source Code Form is subject to the terms of the Mozilla Public
; License, v. 2.0. If a copy of the MPL was not distributed with this
; file, You can obtain one at http://mozilla.org/MPL/2.0/.


PUBLIC longjmp64
PUBLIC _setjmp3
PUBLIC modInternal

.data?

.code

_setjmp3 : ; proc jmp_buf : qword arg : qword

	; RCX = jmp_buf structure
	; RDX = addr to jump back to
	
	mov         qword ptr [rcx],rdx 
	mov         qword ptr [rcx+8],rbx 
	mov         qword ptr [rcx+18h],rbp 
	mov         qword ptr [rcx+20h],rsi 
	mov         qword ptr [rcx+28h],rdi 
	mov         qword ptr [rcx+30h],r12 
	mov         qword ptr [rcx+38h],r13 
	mov         qword ptr [rcx+40h],r14 
	mov         qword ptr [rcx+48h],r15 
	lea         r8,[rsp+8] 
	mov         qword ptr [rcx+10h],r8 
	mov         r8,qword ptr [rsp] 
	mov         qword ptr [rcx+50h],r8 
	stmxcsr     dword ptr [rcx+58h] 
	fnstcw      word ptr [rcx+5Ch] 
	movdqu      xmmword ptr [rcx+60h],xmm6 
	movdqu      xmmword ptr [rcx+70h],xmm7 
	movdqu      xmmword ptr [rcx+80h],xmm8 
	movdqu      xmmword ptr [rcx+90h],xmm9 
	movdqu      xmmword ptr [rcx+0A0h],xmm10 
	movdqu      xmmword ptr [rcx+0B0h],xmm11 
	movdqu      xmmword ptr [rcx+0C0h],xmm12 
	movdqu      xmmword ptr [rcx+0D0h],xmm13 
	movdqu      xmmword ptr [rcx+0E0h],xmm14 
	movdqu      xmmword ptr [rcx+0F0h],xmm15 
	xor         eax,eax 
	ret              

	
	
longjmp64: ; proc jmp_buf : qword result qword)
			
  mov         rax,rdx 
	
  mov         rbx,qword ptr [rcx+8] 
  ; rsp see below
  ; rbp see below
  mov         rsi,qword ptr [rcx+20h] 
  mov         rdi,qword ptr [rcx+28h] 
  mov         r12,qword ptr [rcx+30h] 
  mov         r13,qword ptr [rcx+38h] 
  mov         r14,qword ptr [rcx+40h] 
  mov         r15,qword ptr [rcx+48h] 
  ; rip see below
  
  ldmxcsr     dword ptr [rcx+58h] ; spare?
  fnclex           
  fldcw       word ptr [rcx+5Ch] 
  
  movdqu      xmm6,xmmword ptr [rcx+60h] 
  movdqu      xmm7,xmmword ptr [rcx+70h] 
  movdqu      xmm8,xmmword ptr [rcx+80h] 
  movdqu      xmm9,xmmword ptr [rcx+90h] 
  movdqu      xmm10,xmmword ptr [rcx+0A0h] 
  movdqu      xmm11,xmmword ptr [rcx+0B0h] 
  movdqu      xmm12,xmmword ptr [rcx+0C0h] 
  movdqu      xmm13,xmmword ptr [rcx+0D0h] 
  movdqu      xmm14,xmmword ptr [rcx+0E0h] 
  movdqu      xmm15,xmmword ptr [rcx+0F0h] 
  
  mov         rdx,qword ptr [rcx+50h] ; rip
  mov         rbp,qword ptr [rcx+18h] 
  mov         rsp,qword ptr [rcx+10h] 
  jmp         rdx 
  
  
  
modInternal:

	emms

	sub         rsp,28h 
	movsd       mmword ptr [rsp+10h],xmm1 
	movsd       mmword ptr [rsp+20h],xmm0 
	fld         qword ptr [rsp+10h] 
	fld         qword ptr [rsp+20h] 
	
ModLoop: 
	fprem            
	wait             
	fnstsw      ax   
	test        ax,400h 
	jne         ModLoop
	 
	fstp        qword ptr [rsp] 
	movlpd      xmm0,qword ptr [rsp] 
  		
	add         rsp,28h 
	ret              
    

END