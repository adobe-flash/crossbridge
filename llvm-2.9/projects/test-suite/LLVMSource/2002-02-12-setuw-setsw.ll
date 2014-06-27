; ModuleID = '2002-02-12-setuw-setsw.ll'

define i32 @main(i32 %argc, i8** %argv) {
	%T1 = bitcast i32 2 to i32		; <i32> [#uses=1]
	%tmp = add i32 %T1, -2		; <i32> [#uses=1]
	%cond = icmp eq i32 %tmp, 0		; <i1> [#uses=1]
	br i1 %cond, label %Ok, label %Fail

Ok:		; preds = %0
	ret i32 0

Fail:		; preds = %0
	ret i32 1
}
