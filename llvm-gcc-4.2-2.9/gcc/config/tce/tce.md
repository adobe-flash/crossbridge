;; LLVM LOCAL begin - TCE target
(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "*"
)

;; Indirect jump through a register
(define_insn "indirect_jump"
 [(set (pc) (match_operand:SI 0 "" "r"))]
  "GET_CODE (operands[0]) != MEM || GET_CODE (XEXP (operands[0], 0)) != PLUS"
  ""
)
;; LLVM LOCAL end - TCE target
