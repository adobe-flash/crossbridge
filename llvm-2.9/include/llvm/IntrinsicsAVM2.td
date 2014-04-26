//===- IntrinsicsAVM2.td - Defines AVM2 intrinsics -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the AVM2-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// SSE1

// Arithmetic ops
let TargetPrefix = "avm2" in {  // All intrinsics start with "llvm.avm2.".
  def int_avm2_sse_mul_ps : GCCBuiltin<"__builtin_avm2_mulps">,
              Intrinsic<[llvm_v4f32_ty], [llvm_v4f32_ty,
                         llvm_v4f32_ty], [IntrNoMem]>;
}

// Arithmetic ops
let TargetPrefix = "avm2shim" in {  // All intrinsics start with "llvm.avm2shim.".
  def int_avm2shim_sse_mul_ps : GCCBuiltin<"__builtin_avm2_mulps">,
              Intrinsic<[llvm_v4f32_ty], [llvm_v4f32_ty,
                         llvm_v4f32_ty], [IntrNoMem]>;
}