; ModuleID = 'AliasTest'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64"
target triple = "avm2-unknown-freebsd8"

@aliasTest_alias = alias void ()* @aliasTest_actual

define void @aliasTest_actual() nounwind {
entry:
  ret void;
}
