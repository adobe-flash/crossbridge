; ModuleID = 'SymbolTest'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64"
target triple = "avm2-unknown-freebsd8"

define void @symbolTest_default() nounwind {
entry:
  ret void;
}

define private void @symbolTest_private() nounwind {
entry:
  ret void;
}

define linker_private void @symbolTest_linker_private() nounwind {
entry:
  ret void;
}

define linker_private_weak void @symbolTest_linker_private_weak() nounwind {
entry:
  ret void;
}

define internal void @symbolTest_internal() nounwind {
entry:
  ret void;
}

define available_externally void @symbolTest_available_externally() nounwind {
entry:
  ret void;
}

define linkonce void @symbolTest_linkonce() nounwind {
entry:
  ret void;
}

@symbolTest_common = common unnamed_addr global i32 0;

define weak void @symbolTest_weak() nounwind {
entry:
  ret void;
}

; appending  ?

; extern weak?

define linkonce_odr void @symbolTest_linkonce_odr() nounwind {
entry:
  ret void;
}

; linkonce_odr_auto_hide ?

define weak_odr void @symbolTest_weak_odr() nounwind {
entry:
  ret void;
}

define external void @symbolTest_external() nounwind {
entry:
  ret void;
}

; dllimport ?

; dllexport ?


; -----------------------------------


define hidden void @symbolTest_default_hidden() nounwind {
entry:
  ret void;
}

define private hidden void @symbolTest_private_hidden() nounwind {
entry:
  ret void;
}

define linker_private hidden void @symbolTest_linker_private_hidden() nounwind {
entry:
  ret void;
}

define linker_private_weak hidden void @symbolTest_linker_private_weak_hidden() nounwind {
entry:
  ret void;
}

define internal hidden void @symbolTest_internal_hidden() nounwind {
entry:
  ret void;
}

define available_externally hidden void @symbolTest_available_externally_hidden() nounwind {
entry:
  ret void;
}

define linkonce hidden void @symbolTest_linkonce_hidden() nounwind {
entry:
  ret void;
}

@symbolTest_common_hidden = common hidden unnamed_addr global i32 0;

define weak hidden void @symbolTest_weak_hidden() nounwind {
entry:
  ret void;
}

; appending  ?

; extern weak?

define linkonce_odr hidden void @symbolTest_linkonce_odr_hidden() nounwind {
entry:
  ret void;
}

; linkonce_odr_auto_hide ?

define weak_odr hidden void @symbolTest_weak_odr_hidden() nounwind {
entry:
  ret void;
}

define external hidden void @symbolTest_external_hidden() nounwind {
entry:
  ret void;
}

; dllimport ?

; dllexport ?


; -----------------------------------


define protected void @symbolTest_default_protected() nounwind {
entry:
  ret void;
}

define private protected void @symbolTest_private_protected() nounwind {
entry:
  ret void;
}

define linker_private protected void @symbolTest_linker_private_protected() nounwind {
entry:
  ret void;
}

define linker_private_weak protected void @symbolTest_linker_private_weak_protected() nounwind {
entry:
  ret void;
}

define internal protected void @symbolTest_internal_protected() nounwind {
entry:
  ret void;
}

define available_externally protected void @symbolTest_available_externally_protected() nounwind {
entry:
  ret void;
}

define linkonce protected void @symbolTest_linkonce_protected() nounwind {
entry:
  ret void;
}

@symbolTest_common_protected = common protected unnamed_addr global i32 0;

define weak protected void @symbolTest_weak_protected() nounwind {
entry:
  ret void;
}

; appending  ?

; extern weak?

define linkonce_odr protected void @symbolTest_linkonce_odr_protected() nounwind {
entry:
  ret void;
}

; linkonce_odr_auto_hide ?

define weak_odr protected void @symbolTest_weak_odr_protected() nounwind {
entry:
  ret void;
}

define external protected void @symbolTest_external_protected() nounwind {
entry:
  ret void;
}

; dllimport ?

; dllexport ?
