;; APPLE LOCAL file 6258536 atomic builtins
;; Machine description for ARM synchronization instructions.
;; Copyright (C) 2009 Free Software Foundation, Inc.
;; Contributed by Apple, Inc.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 2, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING.  If not, write to the
;; Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;; MA 02110-1301, USA.


(define_mode_macro IMODE [QI HI SI DI])
(define_mode_attr ldrex [(QI "ldrexb") (HI "ldrexh")
                         (SI "ldrex") (DI "ldrexd")])
(define_mode_attr strex [(QI "strexb") (HI "strexh")
                         (SI "strex") (DI "strexd")])

(define_expand "memory_barrier"
  [(set (mem:BLK (match_dup 0))
        (unspec:BLK [(mem:BLK (match_dup 0))] UNSPEC_BARRIER))]
  "TARGET_32BIT && arm_arch6"
{
  operands[0] = gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode));
  MEM_VOLATILE_P (operands[0]) = 1;
  if (arm_arch6 && !arm_arch7a)
    {
      rtx par;
      par = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc(2));
      /* v6 needs a scratch register */
      XVECEXP (par, 0, 0) =
        gen_rtx_SET (VOIDmode, gen_rtx_MEM (BLKmode, operands[0]),
                     gen_rtx_UNSPEC (BLKmode,
                                     gen_rtvec (1, gen_rtx_MEM (BLKmode,
                                                                operands[0])),
                                     UNSPEC_BARRIER));
      XVECEXP (par, 0, 1) = gen_rtx_CLOBBER (VOIDmode, gen_rtx_SCRATCH(SImode));
      emit_insn (par);
      DONE;
    }
})

(define_insn "arm_memory_barrier_v7"
  [(set (match_operand:BLK 0 "" "")
        (unspec:BLK [(match_operand:BLK 1 "" "")] UNSPEC_BARRIER))]
  "TARGET_32BIT && arm_arch7a"
  "dmb\tish"
  [(set_attr "length" "4")]
)

;; This version matches the define_expand above.
(define_insn "arm_memory_barrier_v6_scratch"
  [(set (match_operand:BLK 0 "" "")
        (unspec:BLK [(match_operand:BLK 1 "" "")] UNSPEC_BARRIER))
   (clobber (match_scratch:SI 2 "=&r"))]
  "TARGET_32BIT && arm_arch6 && !arm_arch7a"
  "mov\t%2, #0\n\tmcr\tp15, 0, %2, c7, c10, 5"
  [(set (attr "length")
	(if_then_else (eq_attr "is_thumb" "yes")
		      (const_int 6)
		      (const_int 8)))]
)

;; This version is used directly by the compare_and_swap splitter below.
;; That runs after reload is complete, so we cannot use a new define_scratch.
;; reload is not available to allocate one for us.
(define_insn "arm_memory_barrier_v6_explicit"
  [(set (match_operand:BLK 0 "" "")
        (unspec:BLK [(match_operand:BLK 1 "" "")] UNSPEC_BARRIER))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  "TARGET_32BIT && arm_arch6 && !arm_arch7a"
  "mov\t%2, #0\n\tmcr\tp15, 0, %2, c7, c10, 5"
  [(set (attr "length")
	(if_then_else (eq_attr "is_thumb" "yes")
		      (const_int 6)
		      (const_int 8)))]
)

(define_expand "memory_sync"
  [(set (mem:BLK (match_dup 0))
        (unspec_volatile:BLK [(mem:BLK (match_dup 0))] UNSPEC_SYNC))]
  "TARGET_32BIT && arm_arch6"
{
  operands[0] = gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode));
  MEM_VOLATILE_P (operands[0]) = 1;
  operands[0] = gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (Pmode));
  MEM_VOLATILE_P (operands[0]) = 1;
  if (arm_arch6 && !arm_arch7a)
    {
      rtx par;
      par = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc(2));
      /* v6 needs a scratch register */
      XVECEXP (par, 0, 0) =
        gen_rtx_SET (BLKmode, operands[0],
                     gen_rtx_UNSPEC_VOLATILE (BLKmode,
                                              gen_rtvec (1, operands[0]),
                                              UNSPEC_SYNC));
      XVECEXP (par, 0, 1) = gen_rtx_CLOBBER (VOIDmode, gen_rtx_SCRATCH(SImode));
      emit_insn (par);
      DONE;
    }
})

(define_insn "arm_memory_sync_v7"
  [(set (match_operand:BLK 0 "" "")
        (unspec_volatile:BLK [(mem:BLK (match_operand 1))] UNSPEC_SYNC))]
  "TARGET_32BIT && arm_arch7a"
  "dsb"
  [(set_attr "length" "4")]
)

;; This version matches the define_expand above.
(define_insn "arm_memory_sync_v6_scratch"
  [(set (match_operand:BLK 0 "" "")
        (unspec_volatile:BLK [(mem:BLK (match_operand 1))] UNSPEC_SYNC))
   (clobber (match_scratch:SI 2 "=&r"))]
  "TARGET_32BIT && arm_arch6 && !arm_arch7a"
  "mov\t%2, #0\n\tmcr\tp15, 0, %2, c7, c10, 4"
  [(set (attr "length")
	(if_then_else (eq_attr "is_thumb" "yes")
		      (const_int 6)
		      (const_int 8)))]
)

;; This version is used directly by the compare_and_swap splitter below.
;; That runs after reload is complete, so we cannot use a new define_scratch.
;; reload is not available to allocate one for us.
(define_insn "arm_memory_sync_v6_explicit"
  [(set (match_operand:BLK 0 "" "")
        (unspec_volatile:BLK [(mem:BLK (match_operand 1))] UNSPEC_SYNC))
   (clobber (match_operand:SI 2 "register_operand" "=&r"))]
  "TARGET_32BIT && arm_arch6 && !arm_arch7a"
  "mov\t%2, #0\n\tmcr\tp15, 0, %2, c7, c10, 4"
  [(set (attr "length")
	(if_then_else (eq_attr "is_thumb" "yes")
		      (const_int 6)
		      (const_int 8)))]
)

(define_insn "load_locked_<mode>"
  [(set (match_operand:IMODE 0 "register_operand" "=r")
        (unspec_volatile:IMODE
          [(match_operand:IMODE 1 "memory_operand" "Q")] VUNSPEC_LL))]
  "TARGET_32BIT && arm_arch6"
  "<ldrex>\t%0, %1"
  [(set_attr "length" "4")]
)

(define_insn "store_conditional_<mode>"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec_volatile:SI [(const_int 0)] VUNSPEC_SC))
   (set (match_operand:IMODE 1 "memory_operand" "=Q")
        (match_operand:IMODE 2 "register_operand" "r"))]
  "TARGET_32BIT && arm_arch6"
  "<strex>\t%0, %2, %1"
  [(set_attr "length" "4")]
)

(define_insn_and_split "sync_compare_and_swap<mode>"
  [(set (match_operand:IMODE 0 "register_operand" "=&r")
	(match_operand:IMODE 1 "memory_operand" "+Q"))
   (set (match_dup 1)
	(unspec_volatile:IMODE
	  [(match_dup 1)
	   (match_operand:IMODE 2 "register_operand" "r")
	   (match_operand:IMODE 3 "register_operand" "r")]
	  UNSPEC_CMPXCHG))
   (clobber (match_scratch:SI 4 "=&r"))
   (clobber (reg:CC CC_REGNUM))]
  "TARGET_32BIT && arm_arch6"
  "#"
  "&& reload_completed"
  [(const_int 0)]
{
  arm_split_compare_and_swap(operands[0], operands[1], operands[2],
                             operands[3], operands[4]);
  DONE;
}
  [(set_attr "length" "48")]
)

