; ModuleID = '<stdin>'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-apple-darwin10.0.0"

define double @_Z3adddd(double %x, double %y) ssp {
entry:
  %ov = alloca i8, align 1
  %call = call zeroext i1 @_Z7bothintdd(double %x, double %y)
  br i1 %call, label %if.then, label %if.end10

if.then:                                          ; preds = %entry
  %call6 = call double @_Z5addovddPb(double %x, double %y, i8* %ov)
  %tmp7 = load i8* %ov, align 1, !tbaa !0
  %tmp = and i8 %tmp7, 1
  %tobool = icmp eq i8 %tmp, 0
  br i1 %tobool, label %return, label %if.end10

if.end10:                                         ; preds = %if.then, %entry
  %call13 = call double @_Z7slowadddd(double %x, double %y)
  ret double %call13

return:                                           ; preds = %if.then
  ret double %call6
}

declare zeroext i1 @_Z7bothintdd(double, double)

declare double @_Z5addovddPb(double, double, i8*)

declare double @_Z7slowadddd(double, double)

!0 = metadata !{metadata !"bool", metadata !1}
!1 = metadata !{metadata !"omnipotent char", metadata !2}
!2 = metadata !{metadata !"Simple C/C++ TBAA", null}
