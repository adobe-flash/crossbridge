// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//===- AVM2CallingConv.td - Calling Conventions AVM2 -----*- tablegen -*-===//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the AVM2 architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

// AVM2 32-bit C return-value convention.
def RetCC_AVM2_32 : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[EAX, EDX]>>,	// lseek() returns long long in i32, i32.
  CCIfType<[i64], CCAssignToReg<[EAX, EDX]>>,
  CCIfType<[f32], CCAssignToReg<[SST0]>>,
  CCIfType<[f64], CCAssignToReg<[ST0]>>
]>;

// AVM2 32-bit C Calling convention.
def CC_AVM2_32 : CallingConv<[
  // All arguments are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;
