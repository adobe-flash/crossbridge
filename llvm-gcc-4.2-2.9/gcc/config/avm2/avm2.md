;// Copyright (c) 2013 Adobe Systems Inc
;
;// Permission is hereby granted, free of charge, to any person obtaining a copy
;// of this software and associated documentation files (the "Software"), to deal
;// in the Software without restriction, including without limitation the rights
;// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;// copies of the Software, and to permit persons to whom the Software is
;// furnished to do so, subject to the following conditions:
;
;// The above copyright notice and this permission notice shall be included in
;// all copies or substantial portions of the Software.
;
;// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
;// THE SOFTWARE.

(define_constants
  [; Relocation specifiers
   (UNSPEC_GOT                  0)
   (UNSPEC_GOTOFF               1)
   (UNSPEC_GOTPCREL             2)
   (UNSPEC_GOTTPOFF             3)
   (UNSPEC_TPOFF                4)
   (UNSPEC_NTPOFF               5)
   (UNSPEC_DTPOFF               6)
   (UNSPEC_GOTNTPOFF            7)
   (UNSPEC_INDNTPOFF            8)
  ])

;; LLVM LOCAL begin - AVM2 target
(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "jmp\t%l0"
)

;; Indirect jump through a register
(define_insn "indirect_jump"
 [(set (pc) (match_operand:SI 0 "" "r"))]
  "GET_CODE (operands[0]) != MEM || GET_CODE (XEXP (operands[0], 0)) != PLUS"
  "jmp\t%A0"
)

;;; Unused letters:
;;;     B     H           TU W   
;;;           h jk          vw  z

;; Integer register constraints.
;; It is not necessary to define 'r' here.
(define_register_constraint "R" "LEGACY_REGS"
 "Legacy register---the eight integer registers available on all
  i386 processors (@code{a}, @code{b}, @code{c}, @code{d},
  @code{si}, @code{di}, @code{bp}, @code{sp}).")

(define_register_constraint "q" "Q_REGS"
 "Any register accessible as @code{@var{r}l}.  In 32-bit mode, @code{a},
  @code{b}, @code{c}, and @code{d}; in 64-bit mode, any integer register.")

(define_register_constraint "Q" "Q_REGS"
 "Any register accessible as @code{@var{r}h}: @code{a}, @code{b},
  @code{c}, and @code{d}.")

(define_register_constraint "l" "INDEX_REGS"
 "@internal Any register that can be used as the index in a base+index
  memory access: that is, any general register except the stack pointer.")

(define_register_constraint "a" "AREG"
 "The @code{a} register.")

(define_register_constraint "b" "BREG"
 "The @code{b} register.")

(define_register_constraint "c" "CREG"
 "The @code{c} register.")

(define_register_constraint "d" "DREG"
 "The @code{d} register.")

(define_register_constraint "S" "SIREG"
 "The @code{si} register.")

(define_register_constraint "D" "DIREG"
 "The @code{di} register.")

(define_register_constraint "A" "AD_REGS"
 "The @code{a} and @code{d} registers, as a pair (for instructions
  that return half the result in one and half in the other).")

;; Floating-point register constraints.
(define_register_constraint "f"
 "NO_REGS"
 "Any 80387 floating-point (stack) register.")

(define_register_constraint "t"
 "NO_REGS"
 "Top of 80387 floating-point stack (@code{%st(0)}).")

(define_register_constraint "u"
 "NO_REGS"
 "Second from top of 80387 floating-point stack (@code{%st(1)}).")

;; Vector registers (also used for plain floating point nowadays).
(define_register_constraint "y" "NO_REGS"
 "Any MMX register.")

(define_register_constraint "x" "NO_REGS"
 "Any SSE register.")

;; APPLE LOCAL begin 5612787 mainline sse4
;; We use the Y prefix to denote any number of conditional register sets:
;;  0	First SSE register.
;;  t	SSE2 enabled
;;  i	SSE2 inter-unit moves enabled
;;  m	MMX inter-unit moves enabled

(define_register_constraint "Y0" "NO_REGS"
 "First SSE register (@code{%xmm0}).")

(define_register_constraint "Yt" "NO_REGS"
 "@internal Any SSE register, when SSE2 is enabled.")

(define_register_constraint "Yi"
 "NO_REGS"
 "@internal Any SSE register, when SSE2 and inter-unit moves are enabled.")

(define_register_constraint "Ym"
 "NO_REGS"
 "@internal Any MMX register, when inter-unit moves are enabled.")
;; APPLE LOCAL end 5612787 mainline sse4

;; Integer constant constraints.
(define_constraint "I"
  "Integer constant in the range 0 @dots{} 31, for 32-bit shifts."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 31")))

(define_constraint "J"
  "Integer constant in the range 0 @dots{} 63, for 64-bit shifts."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 63")))

(define_constraint "K"
  "Signed 8-bit integer constant."
  (and (match_code "const_int")
       (match_test "ival >= -128 && ival <= 127")))

(define_constraint "L"
  "@code{0xFF} or @code{0xFFFF}, for andsi as a zero-extending move."
  (and (match_code "const_int")
       (match_test "ival == 0xFF || ival == 0xFFFF")))

(define_constraint "M"
  "0, 1, 2, or 3 (shifts for the @code{lea} instruction)."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 3")))

(define_constraint "N"
  "Unsigned 8-bit integer constant (for @code{in} and @code{out} 
   instructions)."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 255")))

(define_constraint "O"
  "@internal Integer constant in the range 0 @dots{} 127, for 128-bit shifts."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 127")))


;; Constant-or-symbol-reference constraints.

;; Return 1 if VALUE can be stored in the zero extended immediate field.
(define_predicate "x86_64_zext_immediate_operand"
  (match_code "const_double,const_int,symbol_ref,label_ref,const")
{
  switch (GET_CODE (op))
    {
    case CONST_DOUBLE:
      if (HOST_BITS_PER_WIDE_INT == 32)
	return (GET_MODE (op) == VOIDmode && !CONST_DOUBLE_HIGH (op));
      else
	return 0;

    case CONST_INT:
      if (HOST_BITS_PER_WIDE_INT == 32)
	return INTVAL (op) >= 0;
      else
	return !(INTVAL (op) & ~(HOST_WIDE_INT) 0xffffffff);

    case SYMBOL_REF:
      /* For certain code models, the symbolic references are known to fit.  */
      /* TLS symbols are not constant.  */
      if (SYMBOL_REF_TLS_MODEL (op))
	return false;
      return false;

    case LABEL_REF:
      /* For certain code models, the code is near as well.  */
      return false;

    case CONST:
      /* We also may accept the offsetted memory references in certain
	 special cases.  */
      if (GET_CODE (XEXP (op, 0)) == PLUS)
	{
	  rtx op1 = XEXP (XEXP (op, 0), 0);
	  rtx op2 = XEXP (XEXP (op, 0), 1);

	  if (false)
	    return 0;
	  switch (GET_CODE (op1))
	    {
	    case SYMBOL_REF:
	      /* TLS symbols are not constant.  */
	      if (SYMBOL_REF_TLS_MODEL (op1))
		return 0;
	      /* For small code model we may accept pretty large positive
		 offsets, since one bit is available for free.  Negative
		 offsets are limited by the size of NULL pointer area
		 specified by the ABI.  */
	      if (false
		  && GET_CODE (op2) == CONST_INT
		  && trunc_int_for_mode (INTVAL (op2), DImode) > -0x10000
		  && trunc_int_for_mode (INTVAL (op2), SImode) == INTVAL (op2))
		return 1;
	      /* ??? For the kernel, we may accept adjustment of
		 -0x10000000, since we know that it will just convert
		 negative address space to positive, but perhaps this
		 is not worthwhile.  */
	      break;

	    case LABEL_REF:
	      /* These conditions are similar to SYMBOL_REF ones, just the
		 constraints for code models differ.  */
	      if (false)
		return 1;
	      break;

	    default:
	      return 0;
	    }
	}
      break;

    default:
      gcc_unreachable ();
    }
  return 0;
})

(define_constraint "e"
  "32-bit signed integer constant, or a symbolic reference known
   to fit that range (for immediate operands in sign-extending x86-64
   instructions)."
  (match_operand 0 "immediate_operand"))

(define_constraint "Z"
  "32-bit unsigned integer constant, or a symbolic reference known
   to fit that range (for immediate operands in zero-extending x86-64
   instructions)."
  (match_operand 0 "x86_64_zext_immediate_operand"))

;; LLVM LOCAL end - AVM2 target
