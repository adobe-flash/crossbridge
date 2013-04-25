;; Ideas from Bill, for OP_add

;; initial
(let* ((t0 (xori lhs kIntptrType))
      (t1 (xori rhs kIntptrType))
      (t2 (ori t1 t2))
      (t3 (andi t2 kAtomTypeMask))
      (t4 (eqi t3 0)))
 (labels ((fallback ()
	    (let* ((t5 (calli op_add_a_aa coreAddr lhs rhs)))
	      (done t5)))
	   (haveintptr ()
	    (let* ((lhsStripped (subi lhs kIntptrType)))
	      (addjovi lhsStripped rhs done fallback)))
	   (done (result)
	     (sti result vars disp ACCSET_STACK)))
   (jt t4 haveintptr fallback)))

;; slavish lirasm style
(let* ((c0 (immi kIntptrType))
      (t0 (xori lhs c0))
      (t1 (xori rhs c0))
      (t2 (ori t1 t2))
      (c1 (immi kAtomTypeMask))
      (t3 (andi t2 c1))
      (c2 (immi 0))
      (t4 (eqi t3 c2)))
 (labels ((fallback ()
	    (let* ((t5 (calli op_add_a_aa coreAddr lhs rhs)))
	      (done t5)))
	   (haveintptr ()
	    (let* ((lhsStripped (subi lhs c0)))
	      (addjovi lhsStripped rhs done fallback)))
	   (done (result)
	     (sti result vars disp ACCSET_VARS)))
   (jt t4 haveintptr fallback)))

;; simplified, with nesting
(labels
   ((fallback ()
      (done (calli op_add_a_aa coreAddr lhs rhs)))
    (haveintptr ()
      (addjovi (subi lhs kIntptrType) rhs done fallback))
    (done (result)
      (sti result vars disp ACCSET_STACK)))
 (jt
	(eqi
		(andi (ori (xori lhs kIntptrType) 
			       (xori rhs kIntptrType))
              kAtomTypeMask)
		0)
	haveintptr
	fallback))

