; ModuleID = '2003-08-03-ReservedWordGlobal.ll'
@Sp = linkonce global i32 0		; <i32*> [#uses=1]

define i32 @main() {
	store i32 123, i32* @Sp
	ret i32 0
}
