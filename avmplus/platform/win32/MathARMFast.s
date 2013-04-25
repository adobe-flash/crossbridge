; note, this file is NOT SUBJECT to the normal MPL of the rest of Tamarin and must not be made public

; it's marked with MARK_SECURITY_CHANGE for now, to indicate that we must not accidentally integrate

; it into a public repository unless licensing issues are clear (although it is not a security issue

; in any way, that's just a convenient placeholder to avoid accidental integration)



    INCLUDE kxarm.h

   

    TEXTAREA



    EXPORT SqrtFast		

    EXPORT AbsFast		

    EXPORT FloorFast		

    EXPORT CeilFast	

    EXPORT LrintFast	

    EXPORT LrintfFast	

	



;double SqrtFast(double)

SqrtFast PROC

	

	VMOV D1, R0, R1

	

	VSQRT.F64 D0, D1

	

	VMOV R0, R1, D0

	

	bx		lr

	

	ENDP

	

;double AbsFast(double value)

AbsFast PROC

	VMOV D1, R0, R1

	

	VABS.F64 D0, D1

	

	VMOV R0, R1, D0

	

	bx		lr

	

	ENDP

	

	

;double FloorFast(double value)

FloorFast PROC

	VMOV D1, R0, R1

	

	VMRS R2, FPSCR ; Read Floating-point System Control Register

	;Round to minus infinity

	BIC R0, R2, #0x00400000

	ORR R0, R0, #0x00800000

	VMSR FPSCR, R0 ; Write Floating-point System Control Register

	

	VCVTR.S32.F64 S4, D1

	VCVT.F64.S32 D0, S4

	

	VMSR FPSCR, R2 ; Write Floating-point System Control Register

	

	VMOV R0, R1, D0

	

	bx		lr

	

	ENDP

	

	

;double CeilFast(double value)

CeilFast PROC

	VMOV D1, R0, R1

	

	VMRS R2, FPSCR ; Read Floating-point System Control Register

	;Round to plus infinity

	BIC R0, R2, #0x00800000

	ORR R0, R0, #0x00400000

	VMSR FPSCR, R0 ; Write Floating-point System Control Register

	

	VCVTR.S32.F64 S4, D1

	VCVT.F64.S32 D0, S4

	

	VMSR FPSCR, R2 ; Write Floating-point System Control Register

	

	VMOV R0, R1, D0

	

	bx		lr

	

	ENDP

	

;int LrintFast(double value)

LrintFast PROC

	VMOV D1, R0, R1

	

	VCVTR.S32.F64 S4, D1

	

	VMOV R0, S4

	

	bx		lr

	

	ENDP

	



;int LrintfFast(float value)

LrintfFast PROC

	VMOV S1, R0

	

	VCVTR.S32.F32 S4, S1

	

	VMOV R0, S4

	

	bx		lr

	

	ENDP



	END