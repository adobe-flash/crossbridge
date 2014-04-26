# APPLE LOCAL file 4099000
#ifndef __x86_64__
#define THUNK(REG)				\
.private_extern ___i686.get_pc_thunk.REG	;\
___i686.get_pc_thunk.REG:			;\
	movl    (%esp,1),%REG			;\
	ret					;

#ifdef L_get_pc_thunk_ax
THUNK(eax)
#endif
#ifdef L_get_pc_thunk_dx
THUNK(edx)
#endif
#ifdef L_get_pc_thunk_cx
THUNK(ecx)
#endif
#ifdef L_get_pc_thunk_bx
THUNK(ebx)
#endif
#ifdef L_get_pc_thunk_si
THUNK(esi)
#endif
#ifdef L_get_pc_thunk_di
THUNK(edi)
#endif
#ifdef L_get_pc_thunk_bp
THUNK(ebp)
#endif
#endif
