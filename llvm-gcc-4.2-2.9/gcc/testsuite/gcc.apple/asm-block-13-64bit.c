/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target i?86*-*-darwin* x86_64-*-darwin* } } */
/* { dg-options { -m64 -fasm-blocks -msse3 } } */
/* Radar 6276417 */

char fooc; short foos; int fooi; long long fooll; float foof; double food; long double foold;
asm void bar() { fool:
	adc	eax, 1
	adc	fool, 1
	adc	eax, eax
	adc	fool, eax
	adc	eax, fool
	add	eax, 1
	add	fool, 1
	add	eax, eax
	add	fool, eax
	add	eax, fool
	addpd	xmm0, xmm0
	addpd	xmm0, fool
	addps	xmm0, xmm0
	addps	xmm0, fool
	addsd	xmm0, xmm0
	addsd	xmm0, fool
	addss	xmm0, xmm0
	addss	xmm0, fool
	addsubpd	xmm0, xmm0
	addsubpd	xmm0, fool
	addsubps	xmm0, xmm0
	addsubps	xmm0, fool
	and	eax, 1
	and	fool, 1
	and	eax, eax
	and	fool, eax
	and	eax, fool
	andnpd	xmm0, xmm0
	andnpd	xmm0, fool
	andnps	xmm0, xmm0
	andnps	xmm0, fool
	andpd	xmm0, xmm0
	andpd	xmm0, fool
	andps	xmm0, xmm0
	andps	xmm0, fool
	; arpl	ax, ax
	; arpl	foos, ax
	bsf	eax, eax
	bsf	eax, fool
	bsr	eax, eax
	bsr	eax, fool
	bt	eax, eax
	bt	fool, eax
	bt	eax, 1
	bt	fool, 1
	btc	eax, eax
	btc	fool, eax
	btc	eax, 1
	btc	fool, 1
	btr	eax, eax
	btr	fool, eax
	btr	eax, 1
	btr	fool, 1
	bts	eax, eax
	bts	fool, eax
	bts	eax, 1
	bts	fool, 1
	; call	eax
	; call	fool
	; call	fool
	clflush	fool
	cmova	ax, ax
	cmova	ax, foos
	cmova	eax, eax
	cmova	eax, fooi
	cmova	rax, rax
	cmova	rax, fooll
	cmovae	eax, eax
	cmovae	eax, fool
	cmovb	eax, eax
	cmovb	eax, fool
	cmovbe	eax, eax
	cmovbe	eax, fool
	cmovc	eax, eax
	cmovc	eax, fool
	cmove	eax, eax
	cmove	eax, fool
	cmovg	eax, eax
	cmovg	eax, fool
	cmovge	eax, eax
	cmovge	eax, fool
	cmovl	eax, eax
	cmovl	eax, fool
	cmovle	eax, eax
	cmovle	eax, fool
	cmovna	eax, eax
	cmovna	eax, fool
	cmovnae	eax, eax
	cmovnae	eax, fool
	cmovnb	eax, eax
	cmovnb	eax, fool
	cmovnbe	eax, eax
	cmovnbe	eax, fool
	cmovnc	eax, eax
	cmovnc	eax, fool
	cmovne	eax, eax
	cmovne	eax, fool
	cmovng	eax, eax
	cmovng	eax, fool
	cmovnge	eax, eax
	cmovnge	eax, fool
	cmovnl	eax, eax
	cmovnl	eax, fool
	cmovnle	eax, eax
	cmovnle	eax, fool
	cmovno	eax, eax
	cmovno	eax, fool
	cmovnp	eax, eax
	cmovnp	eax, fool
	cmovns	eax, eax
	cmovns	eax, fool
	cmovnz	eax, eax
	cmovnz	eax, fool
	cmovo	eax, eax
	cmovo	eax, fool
	cmovp	eax, eax
	cmovp	eax, fool
	cmovpe	eax, eax
	cmovpe	eax, fool
	cmovpo	eax, eax
	cmovpo	eax, fool
	cmovs	eax, eax
	cmovs	eax, fool
	cmovz	eax, eax
	cmovz	eax, fool
	cmp	eax, 1
	cmp	fool, 1
	cmp	eax, eax
	cmp	fool, eax
	cmp	eax, fool
	cmpeqpd	xmm0, xmm0
	cmpeqpd	xmm0, fool
	cmpeqps	xmm0, xmm0
	cmpeqps	xmm0, fool
	cmpeqsd	xmm0, xmm0
	cmpeqsd	xmm0, fool
	cmpeqss	xmm0, xmm0
	cmpeqss	xmm0, fool
	cmplepd	xmm0, xmm0
	cmplepd	xmm0, fool
	cmpleps	xmm0, xmm0
	cmpleps	xmm0, fool
	cmplesd	xmm0, xmm0
	cmplesd	xmm0, fool
	cmpless	xmm0, xmm0
	cmpless	xmm0, fool
	cmpltpd	xmm0, xmm0
	cmpltpd	xmm0, fool
	cmpltps	xmm0, xmm0
	cmpltps	xmm0, fool
	cmpltsd	xmm0, xmm0
	cmpltsd	xmm0, fool
	cmpltss	xmm0, xmm0
	cmpltss	xmm0, fool
	cmpneqpd	xmm0, xmm0
	cmpneqpd	xmm0, fool
	cmpneqps	xmm0, xmm0
	cmpneqps	xmm0, fool
	cmpneqsd	xmm0, xmm0
	cmpneqsd	xmm0, fool
	cmpneqss	xmm0, xmm0
	cmpneqss	xmm0, fool
	cmpnlepd	xmm0, xmm0
	cmpnlepd	xmm0, fool
	cmpnleps	xmm0, xmm0
	cmpnleps	xmm0, fool
	cmpnlesd	xmm0, xmm0
	cmpnlesd	xmm0, fool
	cmpnless	xmm0, xmm0
	cmpnless	xmm0, fool
	cmpnltpd	xmm0, xmm0
	cmpnltpd	xmm0, fool
	cmpnltps	xmm0, xmm0
	cmpnltps	xmm0, fool
	cmpnltsd	xmm0, xmm0
	cmpnltsd	xmm0, fool
	cmpnltss	xmm0, xmm0
	cmpnltss	xmm0, fool
	cmpordpd	xmm0, xmm0
	cmpordpd	xmm0, fool
	cmpordps	xmm0, xmm0
	cmpordps	xmm0, fool
	cmpordsd	xmm0, xmm0
	cmpordsd	xmm0, fool
	cmpordss	xmm0, xmm0
	cmpordss	xmm0, fool
	cmppd	xmm0, xmm0, 4
	cmppd	xmm0, fool, 4
	cmpps	xmm0, xmm0, 4
	cmpps	xmm0, fool, 4
	cmpsd	xmm0, xmm0, 4
	cmpsd	xmm0, fool, 4
	cmpss	xmm0, xmm0, 4
	cmpss	xmm0, fool, 4
	cmpunordpd	xmm0, xmm0
	cmpunordpd	xmm0, fool
	cmpunordps	xmm0, xmm0
	cmpunordps	xmm0, fool
	cmpunordsd	xmm0, xmm0
	cmpunordsd	xmm0, fool
	cmpunordss	xmm0, xmm0
	cmpunordss	xmm0, fool
	cmpxchg	fool, eax
	cmpxchg	eax, eax
	comisd	xmm0, xmm0
	comisd	xmm0, fool
	comiss	xmm0, xmm0
	comiss	xmm0, fool
	cvtdq2pd	xmm0, xmm0
	cvtdq2pd	xmm0, fool
	cvtdq2ps	xmm0, xmm0
	cvtdq2ps	xmm0, fool
	cvtpd2dq	xmm0, xmm0
	cvtpd2dq	xmm0, fool
	cvtpd2pi	mm0, xmm0
	cvtpd2pi	mm0, fool
	cvtpd2ps	xmm0, xmm0
	cvtpd2ps	xmm0, fool
	cvtpi2pd	xmm0, mm0
	cvtpi2pd	xmm0, fool
	cvtpi2ps	xmm0, mm0
	cvtpi2ps	xmm0, fool
	cvtps2dq	xmm0, xmm0
	cvtps2dq	xmm0, fool
	cvtps2pd	xmm0, xmm0
	cvtps2pd	xmm0, fool
	cvtps2pi	mm0, xmm0
	cvtps2pi	mm0, fool
	cvtsd2si	eax, xmm0
	cvtsd2si	rax, xmm0
	cvtsd2si	eax, fool
	cvtsd2si	rax, fool
	cvtsd2ss	xmm0, xmm0
	cvtsd2ss	xmm0, fool
	cvtsi2sd	xmm0, eax
	cvtsi2sd	xmm0, fooi
	cvtsi2sd	xmm0, rax
	cvtsi2sd	xmm0, fooll
	cvtsi2ss	xmm0, eax
	cvtsi2ss	xmm0, fooi
	cvtsi2ss	xmm0, rax
	cvtsi2ss	xmm0, fooll
	cvtss2sd	xmm0, xmm0
	cvtss2sd	xmm0, fool
	cvtss2si	eax, xmm0
	cvtss2si	eax, fool
	cvttpd2dq	xmm0, xmm0
	cvttpd2dq	xmm0, fool
	cvttpd2pi	mm0, xmm0
	cvttpd2pi	mm0, fool
	cvttps2dq	xmm0, xmm0
	cvttps2dq	xmm0, fool
	cvttps2pi	mm0, xmm0
	cvttps2pi	mm0, fool
	cvttsd2si	eax, xmm0
	cvttsd2si	eax, fool
	cvttss2si	eax, xmm0
	cvttss2si	eax, fool
	dec	ah
	dec	fooc
	dec	ax
	dec	foos
	dec	eax
	dec	fooi
	div	ah
	div	fooc
	div	ax
	div	foos
	div	eax
	div	fooi
	divpd	xmm0, xmm0
	divpd	xmm0, fool
	divps	xmm0, xmm0
	divps	xmm0, fool
	divsd	xmm0, xmm0
	divsd	xmm0, fool
	divss	xmm0, xmm0
	divss	xmm0, fool
	enter	1, 1
	fadd	st(0), st(2)
	fadd	st(2), st(0)
	fadd	foof
	fadd	food
	faddp	st(2), st(0)
	fbld	fool
	fbstp	fool
	fcmovb	st(0), st(2)
	fcmovbe	st(0), st(2)
	fcmove	st(0), st(2)
	fcmovnb	st(0), st(2)
	fcmovnbe	st(0), st(2)
	fcmovne	st(0), st(2)
	fcmovnu	st(0), st(2)
	fcmovu	st(0), st(2)
	fcom	st(2)
	fcom	foof
	fcom	food
	fcomi	st(0), st(2)
	fcomip	st(0), st(2)
	fcomp	st(2)
	fcomp	foof
	fcomp	food
	fdiv	st(0), st(2)
	fdiv	st(2), st(0)
	fdiv	foof
	fdiv	food
	fdivp	st(2), st(0)
	fdivr	st(0), st(2)
	fdivr	foof
	fdivr	food
	fdivrp	st(2), st(0)
	ffree	st(2)
	fiadd	foos
	fiadd	fooi
	ficom	foos
	ficom	fooi
	ficomp	foos
	ficomp	fooi
	fidiv	foos
	fidiv	fooi
	fidivr	foos
	fidivr	fooi
	fild	foos
	fild	fooi
	fild	fooll
	fimul	foos
	fimul	fooi
	fist	foos
	fist	fooi
	fistp	foos
	fistp	fooi
	fistp	fooll
	fisttp	foos
	fisttp	fooi
	fisttp	fooll
	fisub	foos
	fisub	fooi
	fisubr	foos
	fisubr	fooi
	fld	st(2)
	fld	foof
	fld	food
	fld	foold
	fldcw	foos
	fldenv	fool
	fldt	fool
	fmul	st(2), st(0)
	fmul	st(0), st(2)
	fmul	foof
	fmul	food
	fmulp	st(2), st(0)
	fnsave	fool
	fnstcw	fool
	fnstenv	fool
	fnstsw	fool
	fnstsw	eax
	frstor	fool
	fsave	fool
	fst	st(2)
	fst	foof
	fst	food
	fstcw	fool
	fstenv	fool
	fstp	st(2)
	fstp	foof
	fstp	food
	fstp	foold
	fstsw	fool
	fstsw	eax
	fsub	st(2), st(0)
	fsub	st(0), st(2)
	fsub	foof
	fsub	food
	fsubr	st(2), st(0)
	fsubr	st(0), st(2)
	fsubr	foof
	fsubr	food
	fucom	st(2)
	fucomi	st(0), st(2)
	fucomip	st(0), st(2)
	fucomp	st(2)
	fxch	st(2)
	fxrstor	fool
	fxsave	fool
	haddpd	xmm0, xmm0
	haddpd	xmm0, fool
	haddps	xmm0, xmm0
	haddps	xmm0, fool
	hsubpd	xmm0, xmm0
	hsubpd	xmm0, fool
	hsubps	xmm0, xmm0
	hsubps	xmm0, fool
	idiv	ah
	idiv	fooc
	idiv	ax
	idiv	foos
	idiv	eax
	idiv	fooi
	idiv	rax
	idiv	fooll
	imul	eax, eax, 4
	imul	eax, fool, 4
	in	eax, 1
	inc	ah
	inc	fooc
	inc	ax
	inc	foos
	inc	eax
	inc	fooi
	inc	rax
	inc	fooll
	; ins	fooc, dx
	; ins	foos, dx
	; ins	fooi, dx
	int	1
	invlpg	fool
	ja	fool
	jae	fool
	jb	fool
	jbe	fool
	jc	fool
	; jcxz	.-24
	je	fool
	jecxz	.-24
	jg	fool
	jge	fool
	jl	fool
	jle	fool
	; jmp	fool
	; jmp	eax
	; jmp	fooi
	jna	fool
	jnae	fool
	jnb	fool
	jnc	fool
	jne	fool
	jng	fool
	jnge	fool
	jnl	fool
	jnle	fool
	jno	fool
	jnp	fool
	jns	fool
	jnz	fool
	jo	fool
	jp	fool
	jpe	fool
	jpo	fool
	js	fool
	jz	fool
	lar	eax, eax
	lar	eax, fool
	lddqu	xmm0, fool
	ldmxcsr	fool
	; lds	ax, foos
	; lds	eax, fooi
	; lds	rax, fooll
	lea	eax, fool
	; les	ax, foos
	; les	eax, fooi
	; les	rax, fooll
	lfs	ax, foos
	lfs	eax, fooi
	lfs	rax, fooll
	lgdt	fool
	lgs	ax, foos
	lgs	eax, fooi
	lgs	rax, fooll
	lidt	fool
	lldt	ax
	lldt	foos
	lmsw	fool
	; lods	fooc
	; lods	foos
	; lods	fooi
	; lods	fooll
	loop	.-24
	loope	.-24
	loopne	.-24
	loopnz	.-24
	loopz	.-24
	lsl	ax, ax
	lsl	ax, foos
	lsl	eax, eax
	lsl	eax, fooi
	lss	ax, foos
	lss	eax, fooi
	lss	rax, fooll
	ltr	ax
	ltr	foos
	maskmovdqu	xmm0, xmm0
	maskmovq	mm0, mm0
	maxpd	xmm0, xmm0
	maxpd	xmm0, fool
	maxps	xmm0, xmm0
	maxps	xmm0, fool
	maxsd	xmm0, xmm0
	maxsd	xmm0, fool
	maxss	xmm0, xmm0
	maxss	xmm0, fool
	minpd	xmm0, xmm0
	minpd	xmm0, fool
	minps	xmm0, xmm0
	minps	xmm0, fool
	minsd	xmm0, xmm0
	minsd	xmm0, fool
	minss	xmm0, xmm0
	minss	xmm0, fool
	mov	ah, ah
	mov	fooc, ah
	mov	ah, 1
	mov	fooc, 1
	mov	ax, ax
	mov	foos, ax
	mov	ax, 1
	mov	foos, 1
	mov	eax, eax
	mov	fooi, eax
	mov	eax, 1
	mov	fooi, 1
	mov	rax, rax
	mov	fooll, rax
	mov	rax, 1
	mov	fooll, 1
	mov	ah, ah
	mov	ah, 1
	mov	ah, fooc
	mov	ax, ax
	mov	ax, 1
	mov	ax, foos
	mov	eax, eax
	mov	eax, 1
	mov	eax, fooi
	mov	rax, rax
	mov	rax, 1
	mov	rax, fooll
	movapd	xmm0, xmm0
	movapd	xmm0, fool
	movapd	xmm0, xmm0
	movapd	fool, xmm0
	movaps	xmm0, xmm0
	movaps	xmm0, fool
	movaps	xmm0, xmm0
	movaps	fool, xmm0
	movd	eax, xmm0
	movd	fool, xmm0
	movd	xmm0, eax
	movd	xmm0, fool
	movd	mm0, eax
	movd	mm0, fool
	movd	eax, mm0
	movd	fool, mm0
	movddup	xmm0, xmm0
	movddup	xmm0, fool
	movdq2q	mm0, xmm0
	movdqa	xmm0, xmm0
	movdqa	xmm0, fool
	movdqu	xmm0, xmm0
	movdqu	xmm0, fool
	movhlps	xmm0, xmm0
	movhpd	xmm0, fool
	movhpd	fool, xmm0
	movhps	xmm0, fool
	movhps	fool, xmm0
	movlhps	xmm0, xmm0
	movlpd	xmm0, fool
	movlpd	fool, xmm0
	movlps	xmm0, fool
	movlps	fool, xmm0
	movmskpd	eax, xmm0
	movmskps	eax, xmm0
	movntdq	fool, xmm0
	movnti	fool, eax
	movntpd	fool, xmm0
	movntps	fool, xmm0
	movntq	fool, mm0
	movq	xmm0, xmm0
	movq	xmm0, fool
	movq	fool, xmm0
	movq	mm0, mm0
	movq	mm0, fool
	movq	fool, mm0
	movq2dq	xmm0, mm0
	movs	fooc, fooc
	movs	foos, foos
	movs	fooi, fooi
	movs	fooll, fooll
	movsd	xmm0, xmm0
	movsd	fool, xmm0
	movsd	xmm0, xmm0
	movsd	xmm0, fool
	movshdup	xmm0, xmm0
	movshdup	xmm0, fool
	movsldup	xmm0, xmm0
	movsldup	xmm0, fool
	movss	xmm0, xmm0
	movss	fool, xmm0
	movss	xmm0, xmm0
	movss	xmm0, fool
	movsx	ax, ah
	movsx	ax, fooc
	movsx	eax, ah
	movsx	eax, fooc
	movsx	eax, ax
	movsx	eax, foos
	movupd	xmm0, xmm0
	movupd	xmm0, fool
	movupd	xmm0, xmm0
	movupd	fool, xmm0
	movups	xmm0, xmm0
	movups	xmm0, fool
	movups	xmm0, xmm0
	movups	fool, xmm0
	movzx	ax, ah
	movzx	ax, fooc
	movzx	eax, ah
	movzx	eax, fooc
	movzx	eax, ax
	movzx	eax, foos
	mul	ah
	mul	fooc
	mul	ax
	mul	foos
	mul	eax
	mul	fooi
	mulpd	xmm0, xmm0
	mulpd	xmm0, fool
	mulps	xmm0, xmm0
	mulps	xmm0, fool
	mulsd	xmm0, xmm0
	mulsd	xmm0, fool
	mulss	xmm0, xmm0
	mulss	xmm0, fool
	neg	ah
	neg	fooc
	neg	ax
	neg	foos
	neg	eax
	neg	fooi
	not	ah
	not	fooc
	not	ax
	not	foos
	not	eax
	not	fooi
	or	ah, ah
	or	fooc, ah
	or	ah, 1
	or	fooc, 1
	or	ax, ax
	or	foos, ax
	or	ax, 1
	or	foos, 1
	or	eax, eax
	or	fooi, eax
	or	eax, 1
	or	fooi, 1
	or	rax, rax
	or	fooll, rax
	or	rax, 1
	or	fooll, 1
	or	ah, fooc
	or	ax, foos
	or	eax, fooi
	orpd	xmm0, xmm0
	orpd	xmm0, fool
	orps	xmm0, xmm0
	orps	xmm0, fool
	out	1, al
	out	dx, al
	out	1, ax
	out	dx, ax
	out	1, eax
	out	dx, eax
	; outs	dx, fooc
	; outs	dx, foos
	; outs	dx, fooi
	packssdw	xmm0, xmm0
	packssdw	xmm0, fool
	packssdw	mm0, mm0
	packssdw	mm0, fool
	packsswb	xmm0, xmm0
	packsswb	xmm0, fool
	packsswb	mm0, mm0
	packsswb	mm0, fool
	packuswb	xmm0, xmm0
	packuswb	xmm0, fool
	packuswb	mm0, mm0
	packuswb	mm0, fool
	paddb	xmm0, xmm0
	paddb	xmm0, fool
	paddb	mm0, mm0
	paddb	mm0, fool
	paddd	xmm0, xmm0
	paddd	xmm0, fool
	paddd	mm0, mm0
	paddd	mm0, fool
	paddq	xmm0, xmm0
	paddq	xmm0, fool
	paddq	mm0, mm0
	paddq	mm0, fool
	paddsb	xmm0, xmm0
	paddsb	xmm0, fool
	paddsb	mm0, mm0
	paddsb	mm0, fool
	paddsw	xmm0, xmm0
	paddsw	xmm0, fool
	paddsw	mm0, mm0
	paddsw	mm0, fool
	paddusb	xmm0, xmm0
	paddusb	xmm0, fool
	paddusb	mm0, mm0
	paddusb	mm0, fool
	paddusw	xmm0, xmm0
	paddusw	xmm0, fool
	paddusw	mm0, mm0
	paddusw	mm0, fool
	paddw	xmm0, xmm0
	paddw	xmm0, fool
	paddw	mm0, mm0
	paddw	mm0, fool
	pand	xmm0, xmm0
	pand	xmm0, fool
	pand	mm0, mm0
	pand	mm0, fool
	pandn	xmm0, xmm0
	pandn	xmm0, fool
	pandn	mm0, mm0
	pandn	mm0, fool
	pavgb	xmm0, xmm0
	pavgb	xmm0, fool
	pavgb	mm0, mm0
	pavgb	mm0, fool
	pavgw	xmm0, xmm0
	pavgw	xmm0, fool
	pavgw	mm0, mm0
	pavgw	mm0, fool
	pcmpeqb	xmm0, xmm0
	pcmpeqb	xmm0, fool
	pcmpeqb	mm0, mm0
	pcmpeqb	mm0, fool
	pcmpeqd	xmm0, xmm0
	pcmpeqd	xmm0, fool
	pcmpeqd	mm0, mm0
	pcmpeqd	mm0, fool
	pcmpeqw	xmm0, xmm0
	pcmpeqw	xmm0, fool
	pcmpeqw	mm0, mm0
	pcmpeqw	mm0, fool
	pcmpgtb	xmm0, xmm0
	pcmpgtb	xmm0, fool
	pcmpgtb	mm0, mm0
	pcmpgtb	mm0, fool
	pcmpgtd	xmm0, xmm0
	pcmpgtd	xmm0, fool
	pcmpgtd	mm0, mm0
	pcmpgtd	mm0, fool
	pcmpgtw	xmm0, xmm0
	pcmpgtw	xmm0, fool
	pcmpgtw	mm0, mm0
	pcmpgtw	mm0, fool
	pextrw	eax, xmm0, 4
	pextrw	rax, xmm0, 4
	pextrw	eax, mm0, 4
	pextrw	rax, mm0, 4
	pinsrw	xmm0, eax, 4
	pinsrw	mm0, eax, 4
	pinsrw	xmm0, rax, 4
	pinsrw	mm0, rax, 4
	pinsrw	xmm0, fool, 4
	pinsrw	mm0, fool, 4
	pmaddwd	xmm0, xmm0
	pmaddwd	xmm0, fool
	pmaddwd	mm0, mm0
	pmaddwd	mm0, fool
	pmaxsw	xmm0, xmm0
	pmaxsw	xmm0, fool
	pmaxsw	mm0, mm0
	pmaxsw	mm0, fool
	pmaxub	xmm0, xmm0
	pmaxub	xmm0, fool
	pmaxub	mm0, mm0
	pmaxub	mm0, fool
	pminsw	xmm0, xmm0
	pminsw	xmm0, fool
	pminsw	mm0, mm0
	pminsw	mm0, fool
	pminub	xmm0, xmm0
	pminub	xmm0, fool
	pminub	mm0, mm0
	pminub	mm0, fool
	pmovmskb	eax, xmm0
	pmovmskb	rax, xmm0
	pmovmskb	eax, mm0
	pmovmskb	rax, mm0
	pmulhuw	xmm0, xmm0
	pmulhuw	xmm0, fool
	pmulhuw	mm0, mm0
	pmulhuw	mm0, fool
	pmulhw	xmm0, xmm0
	pmulhw	xmm0, fool
	pmulhw	mm0, mm0
	pmulhw	mm0, fool
	pmullw	xmm0, xmm0
	pmullw	xmm0, fool
	pmullw	mm0, mm0
	pmullw	mm0, fool
	pmuludq	xmm0, xmm0
	pmuludq	xmm0, fool
	pmuludq	mm0, mm0
	pmuludq	mm0, fool
	pop	ax
	pop	foos
	pop	rax
	pop	fooll
	por	xmm0, xmm0
	por	xmm0, fool
	por	mm0, mm0
	por	mm0, fool
	prefetchnta	fool
	prefetcht0	fool
	prefetcht1	fool
	prefetcht2	fool
	psadbw	xmm0, xmm0
	psadbw	xmm0, fool
	psadbw	mm0, mm0
	psadbw	mm0, fool
	pshufd	xmm0, xmm0, 4
	pshufd	xmm0, fool, 4
	pshufhw	xmm0, xmm0, 4
	pshufhw	xmm0, fool, 4
	pshuflw	xmm0, xmm0, 4
	pshuflw	xmm0, fool, 4
	pshufw	mm0, mm0, 4
	pshufw	mm0, fool, 4
	pslld	xmm0, xmm0
	pslld	xmm0, fool
	pslld	xmm0, 1
	pslld	mm0, mm0
	pslld	mm0, fool
	pslld	mm0, 1
	pslldq	xmm0, 1
	psllq	xmm0, xmm0
	psllq	xmm0, fool
	psllq	xmm0, 1
	psllq	mm0, mm0
	psllq	mm0, fool
	psllq	mm0, 1
	psllw	xmm0, xmm0
	psllw	xmm0, fool
	psllw	xmm0, 1
	psllw	mm0, mm0
	psllw	mm0, fool
	psllw	mm0, 1
	psrad	xmm0, xmm0
	psrad	xmm0, fool
	psrad	xmm0, 1
	psrad	mm0, mm0
	psrad	mm0, fool
	psrad	mm0, 1
	psraw	xmm0, xmm0
	psraw	xmm0, fool
	psraw	xmm0, 1
	psraw	mm0, mm0
	psraw	mm0, fool
	psraw	mm0, 1
	psrld	xmm0, xmm0
	psrld	xmm0, fool
	psrld	xmm0, 1
	psrld	mm0, mm0
	psrld	mm0, fool
	psrld	mm0, 1
	psrldq	xmm0, 1
	psrlq	xmm0, xmm0
	psrlq	xmm0, fool
	psrlq	xmm0, 1
	psrlq	mm0, mm0
	psrlq	mm0, fool
	psrlq	mm0, 1
	psrlw	xmm0, xmm0
	psrlw	xmm0, fool
	psrlw	xmm0, 1
	psrlw	mm0, mm0
	psrlw	mm0, fool
	psrlw	mm0, 1
	psubb	xmm0, xmm0
	psubb	xmm0, fool
	psubb	mm0, mm0
	psubb	mm0, fool
	psubd	xmm0, xmm0
	psubd	xmm0, fool
	psubd	mm0, mm0
	psubd	mm0, fool
	psubq	xmm0, xmm0
	psubq	xmm0, fool
	psubq	mm0, mm0
	psubq	mm0, fool
	psubsb	xmm0, xmm0
	psubsb	xmm0, fool
	psubsb	mm0, mm0
	psubsb	mm0, fool
	psubsw	xmm0, xmm0
	psubsw	xmm0, fool
	psubsw	mm0, mm0
	psubsw	mm0, fool
	psubusb	xmm0, xmm0
	psubusb	xmm0, fool
	psubusb	mm0, mm0
	psubusb	mm0, fool
	psubusw	xmm0, xmm0
	psubusw	xmm0, fool
	psubusw	mm0, mm0
	psubusw	mm0, fool
	psubw	xmm0, xmm0
	psubw	xmm0, fool
	psubw	mm0, mm0
	psubw	mm0, fool
	punpckhbw	xmm0, xmm0
	punpckhbw	xmm0, fool
	punpckhbw	mm0, mm0
	punpckhbw	mm0, fool
	punpckhdq	xmm0, xmm0
	punpckhdq	xmm0, fool
	punpckhdq	mm0, mm0
	punpckhdq	mm0, fool
	punpckhqdq	xmm0, xmm0
	punpckhqdq	xmm0, fool
	punpckhwd	xmm0, xmm0
	punpckhwd	xmm0, fool
	punpckhwd	mm0, mm0
	punpckhwd	mm0, fool
	punpcklbw	xmm0, xmm0
	punpcklbw	xmm0, fool
	punpcklbw	mm0, mm0
	punpcklbw	mm0, fool
	punpckldq	xmm0, xmm0
	punpckldq	xmm0, fool
	punpckldq	mm0, mm0
	punpckldq	mm0, fool
	punpcklqdq	xmm0, xmm0
	punpcklqdq	xmm0, fool
	punpcklwd	xmm0, xmm0
	punpcklwd	xmm0, fool
	punpcklwd	mm0, mm0
	punpcklwd	mm0, fool
	push	ax
	push	foos
	push	rax
	push	fooll
	push	1
	pxor	xmm0, xmm0
	pxor	xmm0, fool
	pxor	mm0, mm0
	pxor	mm0, fool
	rcl	ah, 1
	rcl	fooc, 1
	rcl	ax, 1
	rcl	foos, 1
	rcl	eax, 1
	rcl	fooi, 1
	rcl	ah, cl
	rcl	fooc, cl
	rcl	ax, cl
	rcl	foos, cl
	rcl	eax, cl
	rcl	fooi, cl
	rcpps	xmm0, xmm0
	rcpps	xmm0, fool
	rcpss	xmm0, xmm0
	rcpss	xmm0, fool
	rcr	ah, 1
	rcr	fooc, 1
	rcr	ax, 1
	rcr	foos, 1
	rcr	eax, 1
	rcr	fooi, 1
	rcr	ah, cl
	rcr	fooc, cl
	rcr	ax, cl
	rcr	foos, cl
	rcr	eax, cl
	rcr	fooi, cl
	ret	1
	rol	ah, 1
	rol	fooc, 1
	rol	ax, 1
	rol	foos, 1
	rol	eax, 1
	rol	fooi, 1
	rol	ah, cl
	rol	fooc, cl
	rol	ax, cl
	rol	foos, cl
	rol	eax, cl
	rol	fooi, cl
	ror	ah, 1
	ror	fooc, 1
	ror	ax, 1
	ror	foos, 1
	ror	eax, 1
	ror	fooi, 1
	ror	ah, cl
	ror	fooc, cl
	ror	ax, cl
	ror	foos, cl
	ror	eax, cl
	ror	fooi, cl
	rsqrtps	xmm0, xmm0
	rsqrtps	xmm0, fool
	rsqrtss	xmm0, xmm0
	rsqrtss	xmm0, fool
	sal	ah, 1
	sal	fooc, 1
	sal	ax, 1
	sal	foos, 1
	sal	eax, 1
	sal	fooi, 1
	sal	ah, cl
	sal	fooc, cl
	sal	ax, cl
	sal	foos, cl
	sal	eax, cl
	sal	fooi, cl
	sar	ah, 1
	sar	fooc, 1
	sar	ax, 1
	sar	foos, 1
	sar	eax, 1
	sar	fooi, 1
	sar	ah, cl
	sar	fooc, cl
	sar	ax, cl
	sar	foos, cl
	sar	eax, cl
	sar	fooi, cl
	sbb	ah, ah
	sbb	fooc, ah
	sbb	ah, 1
	sbb	fooc, 1
	sbb	ax, ax
	sbb	foos, ax
	sbb	ax, 1
	sbb	foos, 1
	sbb	eax, eax
	sbb	fooi, eax
	sbb	eax, 1
	sbb	fooi, 1
	sbb	ah, fooc
	sbb	ax, foos
	sbb	eax, fooi
	scas	fooc
	scas	foos
	scas	fooi
	scas	fooll
	seta	ah
	seta	fool
	setae	ah
	setae	fool
	setb	ah
	setb	fool
	setbe	ah
	setbe	fool
	setc	ah
	setc	fool
	sete	ah
	sete	fool
	setg	ah
	setg	fool
	setge	ah
	setge	fool
	setl	ah
	setl	fool
	setle	ah
	setle	fool
	setna	ah
	setna	fool
	setnae	ah
	setnae	fool
	setnb	ah
	setnb	fool
	setnbe	ah
	setnbe	fool
	setnc	ah
	setnc	fool
	setne	ah
	setne	fool
	setng	ah
	setng	fool
	setnge	ah
	setnge	fool
	setnl	ah
	setnl	fool
	setnle	ah
	setnle	fool
	setno	ah
	setno	fool
	setnp	ah
	setnp	fool
	setns	ah
	setns	fool
	setnz	ah
	setnz	fool
	seto	ah
	seto	fool
	setp	ah
	setp	fool
	setpe	ah
	setpe	fool
	setpo	ah
	setpo	fool
	sets	ah
	sets	fool
	setz	ah
	setz	fool
	; sgdt	fool
	shl	ah, 1
	shl	fooc, 1
	shl	ax, 1
	shl	foos, 1
	shl	eax, 1
	shl	fooi, 1
	shl	ah, cl
	shl	fooc, cl
	shl	ax, cl
	shl	foos, cl
	shl	eax, cl
	shl	fooi, cl
	shld	ax, ax, 4
	shld	foos, ax, 4
	shld	eax, eax, 4
	shld	fooi, eax, 4
	shld	rax, rax, 4
	shld	fooll, rax, 4
	shr	ah, 1
	shr	fooc, 1
	shr	ax, 1
	shr	foos, 1
	shr	eax, 1
	shr	fooi, 1
	shr	ah, cl
	shr	fooc, cl
	shr	ax, cl
	shr	foos, cl
	shr	eax, cl
	shr	fooi, cl
	shrd	ax, ax, 4
	shrd	foos, ax, 4
	shrd	eax, eax, 4
	shrd	fooi, eax, 4
	shrd	rax, rax, 4
	shrd	fooll, rax, 4
	shufpd	xmm0, xmm0, 4
	shufpd	xmm0, fool, 4
	shufps	xmm0, xmm0, 4
	shufps	xmm0, fool, 4
	; sidt	fool
	sldt	ax
	sldt	fool
	smsw	ax
	smsw	fool
	sqrtpd	xmm0, xmm0
	sqrtpd	xmm0, fool
	sqrtps	xmm0, xmm0
	sqrtps	xmm0, fool
	sqrtsd	xmm0, xmm0
	sqrtsd	xmm0, fool
	sqrtss	xmm0, xmm0
	sqrtss	xmm0, fool
	stmxcsr	fool
	stos	fool
	str	ax
	str	fool
	sub	eax, 1
	sub	fool, 1
	sub	eax, eax
	sub	fool, eax
	sub	eax, fool
	subpd	xmm0, xmm0
	subpd	xmm0, fool
	subps	xmm0, xmm0
	subps	xmm0, fool
	subsd	xmm0, xmm0
	subsd	xmm0, fool
	subss	xmm0, xmm0
	subss	xmm0, fool
	test	eax, eax
	test	ah, 1
	test	fooc, 1
	test	ax, 1
	test	foos, 1
	test	eax, 1
	test	fooi, 1
	ucomisd	xmm0, xmm0
	ucomisd	xmm0, fool
	ucomiss	xmm0, xmm0
	ucomiss	xmm0, fool
	unpckhpd	xmm0, xmm0
	unpckhpd	xmm0, fool
	unpckhps	xmm0, xmm0
	unpckhps	xmm0, fool
	unpcklpd	xmm0, xmm0
	unpcklpd	xmm0, fool
	unpcklps	xmm0, xmm0
	unpcklps	xmm0, fool
	verr	ax
	verr	foos
	verw	ax
	verw	foos
	xadd	ah, ah
	xadd	fooc, ah
	xadd	ax, ax
	xadd	foos, ax
	xadd	eax, eax
	xadd	fooi, eax
	xchg	eax, eax
	xchg	fool, eax
	xchg	eax, fool
	xlat	fool
	xor	eax, eax
	xor	fool, eax
	xor	eax, 1
	xor	fool, 1
	xor	eax, fool
	xorpd	xmm0, xmm0
	xorpd	xmm0, fool
	xorps	xmm0, xmm0
	xorps	xmm0, fool
}
