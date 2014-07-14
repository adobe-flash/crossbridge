; ModuleID = 'InvokeUnwind.ll'

declare void @abort()

define internal void @throw(i1 %ShouldThrow) {
	br i1 %ShouldThrow, label %Throw, label %NoThrow

Throw:		; preds = %0
	unwind

NoThrow:		; preds = %0
	ret void
}

define i32 @main() {
	invoke void @throw( i1 false )
			to label %Cont unwind label %Abort

Cont:		; preds = %0
	invoke void @throw( i1 true )
			to label %Abort unwind label %Exc

Abort:		; preds = %Cont, %0
	call void @abort( )
	ret i32 1

Exc:		; preds = %Cont
	ret i32 0
}
