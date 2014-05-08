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

//===- AVM2InstrInfo.td - Target Description for AVM2 Target ------------===//
//
// This file describes the AVM2 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

/***
Partial TODO

- get rid of trig?
- turn select into diamond (asc and falcon do w/ ternary anyhow)! Follow Sparc

***/

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "AVM2InstrFormats.td"

def UseInlineAsm : Predicate<"Subtarget.useInlineAsm()">;
def UseIntrinsics : Predicate<"Subtarget.useIntrinsics()">;

def SDT_FROUND_INREG : SDTypeProfile<1, 1, [SDTCisVT<0, f64>, SDTCisVT<1, f64>]>;
def fround_inreg : SDNode<"ISD::FP_ROUND_INREG", SDT_FROUND_INREG, []>;

def SDT_AVM2Call    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;
def call          : SDNode<"AVM2ISD::CALL", SDT_AVM2Call,
                           [SDNPHasChain]>;

def retflag       : SDNode<"AVM2ISD::RET_FLAG", SDTNone,
                           [SDNPHasChain]>;

def SDT_AVM2CallSeq : SDTypeProfile<0, 1, [ SDTCisVT<0, i32> ]>;
def SDT_AVM2CallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32> ]>;
def SDT_AVM2CallSeqEnd   : SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                        SDTCisVT<1, i32> ]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_AVM2CallSeqStart,
                           [SDNPHasChain]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_AVM2CallSeqEnd,
                           [SDNPHasChain, SDNPOutGlue]>;

def SDT_AVM2EH_SJLJ_Setjmp : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisPtrTy<1>,
                                                 SDTCisInt<2>]>;
def SDT_AVM2EH_SJLJ_Longjmp: SDTypeProfile<0, 2, [SDTCisPtrTy<0>, SDTCisInt<1>]>;

def SDT_AVM2EH_SJLJ_DispatchSetup: SDTypeProfile<0, 1, [SDTCisPtrTy<0>]>;

def AVM2eh_sjlj_setjmp: SDNode<"AVM2ISD::EH_SJLJ_SETJMP",
                               SDT_AVM2EH_SJLJ_Setjmp, [SDNPHasChain]>;
def AVM2eh_sjlj_longjmp: SDNode<"AVM2ISD::EH_SJLJ_LONGJMP",
                               SDT_AVM2EH_SJLJ_Longjmp, [SDNPHasChain]>;
def AVM2eh_sjlj_dispatchsetup: SDNode<"AVM2ISD::EH_SJLJ_DISPATCHSETUP",
                               SDT_AVM2EH_SJLJ_DispatchSetup, [SDNPHasChain]>;

// Integer "NOP" no-op
def SDT_INOP : SDTypeProfile<1, 1, [ SDTCisVT<0, i32>, SDTCisVT<1, i32> ]>;
def inop : SDNode<"AVM2ISD::INOP", SDT_INOP, []>;

// Float "NOP" no-op
def SDT_FNOP : SDTypeProfile<1, 1, [ SDTCisVT<0, f64>, SDTCisVT<1, f64> ]>;
def fnop : SDNode<"AVM2ISD::FNOP", SDT_FNOP, []>;

// Conditional branch
def SDT_CBR : SDTypeProfile<0, 2, [SDTCisVT<0, i32>, SDTCisVT<1, OtherVT>]>;
def cbr : SDNode<"AVM2ISD::CBR", SDT_CBR, [SDNPHasChain]>;
def fcbr : SDNode<"AVM2ISD::FCBR", SDT_CBR, [SDNPHasChain]>;

// Select
def SDT_SL : SDTypeProfile<1, 3, [ SDTCisVT<0, i32>, SDTCisVT<1, i32>, SDTCisVT<2, i32>, SDTCisVT<3, i32>]>;
def sl : SDNode<"AVM2ISD::SL", SDT_SL, []>;
def SDT_FSL : SDTypeProfile<1, 3, [ SDTCisVT<0, f64>, SDTCisVT<1, i32>, SDTCisVT<2, f64>, SDTCisVT<3, f64>]>;
def fsl : SDNode<"AVM2ISD::FSL", SDT_FSL, []>;

//===----------------------------------------------------------------------===//
// Feature predicates.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//

def MEMrr : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops IntRegs, IntRegs);
}

def MEMri : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops IntRegs, i32imm);
}

def MEMrr_f64 : Operand<f64> {
  let PrintMethod = "printFloatOperand";
  let MIOperandInfo = (ops IntRegs, IntRegs);
}

def MEMri_f64 : Operand<f64> {
  let PrintMethod = "printFloatOperand";
  let MIOperandInfo = (ops IntRegs, i32imm);
}

def ADDRrr_f64 : ComplexPattern<f64, 2, "SelectADDRrr", [], []>;
def ADDRri_f64 : ComplexPattern<f64, 2, "SelectADDRri", [frameindex], []>;

def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex], []>;

def brtarget : Operand<OtherVT>;
def calltarget : Operand<i32>;
def glob : Operand<i32>;

def _f64imm : Operand<f64> {
  let PrintMethod = "printFloatImm";
  let MIOperandInfo = (ops i32imm, i32imm);
}

def F64OP : ComplexPattern<f64, 2, "SelectFloatImm", [], []>;

def ccond : Operand<i32> {
  let PrintMethod = "printCond";
  let MIOperandInfo = (ops i32imm, IntRegs, IntRegs);
}

def COP : ComplexPattern<i32, 3, "SelectCond", [], []>;

def fccond : Operand<i32> {
  let PrintMethod = "printFloatCond";
  let MIOperandInfo = (ops i32imm, FPRegs, FPRegs);
}

def FCOP : ComplexPattern<i32, 3, "SelectFloatCond", [], []>;

//===----------------------------------------------------------------------===//
// AVM2 Flag Conditions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction Class Templates
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

class InstS2<dag outs, dag ins, string s1, string s2, list<dag> pattern> :
   InstAVM2<outs, ins, !strconcat(s1, s2), pattern>;

class InstS3<dag outs, dag ins, string s1, string s2, string s3, list<dag> pattern> :
   InstS2<outs, ins, !strconcat(s1, s2), s3, pattern>;

class InstS4<dag outs, dag ins, string s1, string s2, string s3, string s4, list<dag> pattern> :
   InstS3<outs, ins, !strconcat(s1, s2), s3, s4, pattern>;

class InstS5<dag outs, dag ins, string s1, string s2, string s3, string s4, string s5, list<dag> pattern> :
   InstS4<outs, ins, !strconcat(s1, s2), s3, s4, s5, pattern>;

class InstS6<dag outs, dag ins, string s1, string s2, string s3, string s4, string s5, string s6, list<dag> pattern> :
   InstS5<outs, ins, !strconcat(s1, s2), s3, s4, s5, s6, pattern>;

// Pseudo instructions.
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
   : InstAVM2<outs, ins, asmstr, pattern>;

let Defs = [ESP], Uses = [ESP] in
{
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),
                               "//ADJCALLSTACKDOWN $amt",
                               [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            "//ADJCALLSTACKUP $amt1",
                            [(callseq_end timm:$amt1, timm:$amt2)]>;

}

def IMPLICIT_DEF_Int : Pseudo<(outs IntRegs:$dst), (ins),
                              "//IMPLICIT_DEF $dst",
                              [(set IntRegs:$dst, (undef))]>;
def IMPLICIT_DEF_FP  : Pseudo<(outs FPRegs:$dst), (ins),
                              "//IMPLICIT_DEF $dst",
                              [(set FPRegs:$dst, (undef))]>;
def IMPLICIT_DEF_SP  : Pseudo<(outs SPRegs:$dst), (ins),
                              "//IMPLICIT_DEF $dst",
                              [(set SPRegs:$dst, (undef))]>;

let Defs =
  [ EAX, EDX, ST0, EBP ],
  hasSideEffects = 1, isBarrier = 1 in {
  // TODO what's with $val? do we need it?
  def eh_sjlj_setjmp : Pseudo<(outs), (ins IntRegs:$src, IntRegs:$val),
                                   "//^^^ sjlj_setjmp $src $val",
                         [(set EAX, (AVM2eh_sjlj_setjmp IntRegs:$src, IntRegs:$val))]>;
}

let isBarrier = 1, hasSideEffects = 1, isTerminator = 1,
    Defs = [ ESP, EBP ] in {
def eh_sjlj_longjmp : Pseudo<(outs), (ins IntRegs:$src, IntRegs:$scratch),
                             "//^^^ sjlj_longjmp $src $scratch",
                         [(AVM2eh_sjlj_longjmp IntRegs:$src, IntRegs:$scratch)]>;
}

                  
//*** Instructions
  
//*** Branching

let AddedComplexity = 10, isCall = 1, Defs = [EAX,EDX,ST0,SST0,CF], Uses = [ESP] in
{
  def pCALLg : InstAVM2<(outs), (ins calltarget:$sym), "$sym()", [(call (inop tglobaladdr:$sym))]>;
  def pCALLe : InstAVM2<(outs), (ins calltarget:$sym), "$sym()", [(call (inop texternalsym:$sym))]>;
}
    
let isCall = 1, Defs = [EAX,EDX,ST0,SST0,CF], Uses = [ESP] in
{
  def pCALLrr : InstAVM2<(outs), (ins MEMrr:$ptr), "ptr2fun[$ptr]()", [(call ADDRrr:$ptr)]>;
  def pCALLri : InstAVM2<(outs), (ins MEMri:$ptr), "ptr2fun[$ptr]()", [(call ADDRri:$ptr)]>;
}

let isReturn = 1, isTerminator = 1 in
{
  def RETL : InstAVM2<(outs), (ins), "return\n", [(retflag)]>;
}

let isBranch = 1, isTerminator = 1 in
{
  def asCBR : InstAVM2<(outs), (ins brtarget:$dst, ccond:$cond), "__asm(push($cond), iftrue, $dst)", [(cbr COP:$cond, bb:$dst)]>,
    Requires<[UseInlineAsm]>;
  def asFCBR : InstAVM2<(outs), (ins brtarget:$dst, fccond:$cond), "__asm(push($cond), iftrue, $dst)", [(fcbr FCOP:$cond, bb:$dst)]>,
    Requires<[UseInlineAsm]>;
  def inCBR : InstAVM2<(outs), (ins brtarget:$dst, ccond:$cond), "if($cond) goto $dst", [(cbr COP:$cond, bb:$dst)]>,
    Requires<[UseIntrinsics]>;
  def inFCBR : InstAVM2<(outs), (ins brtarget:$dst, fccond:$cond), "if($cond) goto $dst", [(fcbr FCOP:$cond, bb:$dst)]>,
    Requires<[UseIntrinsics]>;
}

let isBranch = 1, isTerminator = 1, isBarrier = 1 in
{
  def asBR : InstAVM2<(outs), (ins brtarget:$dst), "__asm(jump, $dst)", [(br bb:$dst)]>,
    Requires<[UseInlineAsm]>;
  def inBR : InstAVM2<(outs), (ins brtarget:$dst), "goto $dst", [(br bb:$dst)]>,
    Requires<[UseIntrinsics]>;
}

let usesCustomInserter = 1 in
{
  def SL : InstAVM2<(outs IntRegs:$dst), (ins ccond:$cond, IntRegs:$a, IntRegs:$b), "$dst = (($cond) ? $a : $b) // CUSTOM",
    [(set IntRegs:$dst, (sl COP:$cond, IntRegs:$a, IntRegs:$b))]>;
  def FSL : InstAVM2<(outs IntRegs:$dst), (ins fccond:$cond, IntRegs:$a, IntRegs:$b), "$dst = (($cond) ? $a : $b) // CUSTOM",
    [(set IntRegs:$dst, (sl FCOP:$cond, IntRegs:$a, IntRegs:$b))]>;
  def SLF : InstAVM2<(outs FPRegs:$dst), (ins ccond:$cond, FPRegs:$a, FPRegs:$b), "$dst = (($cond) ? $a : $b) // CUSTOM",
    [(set FPRegs:$dst, (fsl COP:$cond, FPRegs:$a, FPRegs:$b))]>;
  def FSLF : InstAVM2<(outs FPRegs:$dst), (ins fccond:$cond, FPRegs:$a, FPRegs:$b), "$dst = (($cond) ? $a : $b) // CUSTOM",
    [(set FPRegs:$dst, (fsl FCOP:$cond, FPRegs:$a, FPRegs:$b))]>;
}

//*** Register movement / misc

def NOP : InstAVM2<(outs IntRegs:$reg), (ins), "// nop", []>;

def MOVrr : InstAVM2<(outs IntRegs:$dst), (ins IntRegs:$src), "$dst = $src",
  []>;

def FMOVrr : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = $src",
  []>;

def SMOVrr : InstAVM2<(outs SPRegs:$dst), (ins SPRegs:$src), "$dst = $src",
  []>;

def DECri : InstAVM2<(outs IntRegs:$dst), (ins i32imm:$n), "$dst -= $n", []>;

def INCri : InstAVM2<(outs IntRegs:$dst), (ins i32imm:$n), "$dst += $n", []>;

// TODO defs/uses for ESP
def asPUSHr : InstAVM2<(outs), (ins IntRegs:$reg), "esp -= 4;\n\t__asm(push($reg), push(esp), op(0x3c))", []>,
  Requires<[UseInlineAsm]>;
def asPOPr : InstAVM2<(outs IntRegs:$reg), (ins), "$reg = __xasm<int>(push(esp), op(0x37));\n\tesp += 4", []>,
  Requires<[UseInlineAsm]>;
def inPUSHr : InstAVM2<(outs), (ins IntRegs:$reg), "esp -= 4;\n\tsi32($reg, esp)", []>,
  Requires<[UseIntrinsics]>;
def inPOPr : InstAVM2<(outs IntRegs:$reg), (ins), "$reg = li32(esp);\n\tesp += 4", []>,
  Requires<[UseIntrinsics]>;

//*** Arith

// for non commutative / associative
multiclass NBIN<string asmstr, SDNode opnode, SDNode fopnode> {
  def ir : InstS3<(outs IntRegs:$dst), (ins i32imm:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode imm:$a, IntRegs:$b))]>;
  def ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>;
  def rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>;

  def fir : InstS3<(outs FPRegs:$dst), (ins _f64imm:$a, FPRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set FPRegs:$dst, (fopnode (fnop F64OP:$a), FPRegs:$b))]>;
  def fri : InstS3<(outs FPRegs:$dst), (ins FPRegs:$a, _f64imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set FPRegs:$dst, (fopnode FPRegs:$a, (fnop F64OP:$b)))]>;
  def frr : InstS3<(outs FPRegs:$dst), (ins FPRegs:$a, FPRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set FPRegs:$dst, (fopnode FPRegs:$a, FPRegs:$b))]>;
}

// for unsigned (which are non commutative / associative)
multiclass UBIN<string asmstr, SDNode opnode> {
  def as_ir : InstS3<(outs IntRegs:$dst), (ins i32imm:$a, IntRegs:$b), "$dst = (__xasm<uint>(push($a), convert_u) ", asmstr, " __xasm<uint>(push($b), convert_u))",
    [(set IntRegs:$dst, (opnode imm:$a, IntRegs:$b))]>,
    Requires<[UseInlineAsm]>;
  def as_ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = (__xasm<uint>(push($a), convert_u) ", asmstr, " __xasm<uint>(push($b), convert_u))",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>,
    Requires<[UseInlineAsm]>;
  def as_rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = (__xasm<uint>(push($a), convert_u) ", asmstr, " __xasm<uint>(push($b), convert_u))",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>,
    Requires<[UseInlineAsm]>;

  def in_ir : InstS3<(outs IntRegs:$dst), (ins i32imm:$a, IntRegs:$b), "$dst = uint($a) ", asmstr, " uint($b)",
    [(set IntRegs:$dst, (opnode imm:$a, IntRegs:$b))]>,
    Requires<[UseIntrinsics]>;
  def in_ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = uint($a) ", asmstr, " uint($b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>,
    Requires<[UseIntrinsics]>;
  def in_rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = uint($a) ", asmstr, " uint($b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>,
    Requires<[UseIntrinsics]>;
}

// for commutative / associative
multiclass BIN<string asmstr, SDNode opnode, SDNode fopnode> {
  def ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>;
  def rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>;

  def fri : InstS3<(outs FPRegs:$dst), (ins FPRegs:$a, _f64imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set FPRegs:$dst, (fopnode FPRegs:$a, (fnop F64OP:$b)))]>;
  def frr : InstS3<(outs FPRegs:$dst), (ins FPRegs:$a, FPRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set FPRegs:$dst, (fopnode FPRegs:$a, FPRegs:$b))]>;
}

// for integer only
multiclass IBIN<string asmstr, SDNode opnode> {
  def ir : InstS3<(outs IntRegs:$dst), (ins i32imm:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode imm:$a, IntRegs:$b))]>;
  def ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>;
  def rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>;
}

// non commutative / associative
multiclass INBIN<string asmstr, SDNode opnode> {
  def ri : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, i32imm:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, imm:$b))]>;
  def rr : InstS3<(outs IntRegs:$dst), (ins IntRegs:$a, IntRegs:$b), "$dst = ($a ", asmstr, " $b)",
    [(set IntRegs:$dst, (opnode IntRegs:$a, IntRegs:$b))]>;
}

defm ADD : BIN<"+", add, fadd>;
defm SUB : NBIN<"-", sub, fsub>;
defm MUL : BIN<"*", mul, fmul>;
defm DIV : NBIN<"/", sdiv, fdiv>;
defm UDIV : UBIN<"/", udiv>;
defm REM : NBIN<"%", srem, frem>;
defm UREM : UBIN<"%", urem>;
defm SRL : INBIN<">>>", srl>;
defm SRA : INBIN<">>", sra>;
defm SHL : INBIN<"<<", shl>;
defm AND : INBIN<"&", and>;
defm OR : INBIN<"\\|", or>;
defm XOR : INBIN<"^", xor>;

def asFP_TO_SI : InstAVM2<(outs IntRegs:$dst), (ins FPRegs:$src), "$dst = (__xasm<int>(push($src), convert_i))",
  [(set IntRegs:$dst, (fp_to_sint FPRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inFP_TO_SI : InstAVM2<(outs IntRegs:$dst), (ins FPRegs:$src), "$dst = int($src)",
  [(set IntRegs:$dst, (fp_to_sint FPRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asFP_TO_UI : InstAVM2<(outs IntRegs:$dst), (ins FPRegs:$src), "$dst = (__xasm<uint>(push($src), convert_u))",
  [(set IntRegs:$dst, (fp_to_uint FPRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inFP_TO_UI : InstAVM2<(outs IntRegs:$dst), (ins FPRegs:$src), "$dst = uint($src)",
  [(set IntRegs:$dst, (fp_to_uint FPRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asSI_TO_FP : InstAVM2<(outs FPRegs:$dst), (ins IntRegs:$src), "$dst = (__xasm<Number>(push($src), convert_d))",
  [(set FPRegs:$dst, (sint_to_fp IntRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inSI_TO_FP : InstAVM2<(outs FPRegs:$dst), (ins IntRegs:$src), "$dst = Number($src)",
  [(set FPRegs:$dst, (sint_to_fp IntRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asUI_TO_FP : InstAVM2<(outs FPRegs:$dst), (ins IntRegs:$src), "$dst = (__xasm<Number>(push($src), convert_u, convert_d))",
  [(set FPRegs:$dst, (uint_to_fp IntRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inUI_TO_FP : InstAVM2<(outs FPRegs:$dst), (ins IntRegs:$src), "$dst = Number(uint($src))",
  [(set FPRegs:$dst, (uint_to_fp IntRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asSP_TO_SI : InstAVM2<(outs IntRegs:$dst), (ins SPRegs:$src), "$dst = (__xasm<int>(push($src), convert_i))",
  [(set IntRegs:$dst, (fp_to_sint SPRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inSP_TO_SI : InstAVM2<(outs IntRegs:$dst), (ins SPRegs:$src), "$dst = int($src)",
  [(set IntRegs:$dst, (fp_to_sint SPRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asSP_TO_UI : InstAVM2<(outs IntRegs:$dst), (ins SPRegs:$src), "$dst = (__xasm<uint>(push($src), convert_u))",
  [(set IntRegs:$dst, (fp_to_uint SPRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inSP_TO_UI : InstAVM2<(outs IntRegs:$dst), (ins SPRegs:$src), "$dst = uint($src)",
  [(set IntRegs:$dst, (fp_to_uint SPRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asSI_TO_SP : InstAVM2<(outs SPRegs:$dst), (ins IntRegs:$src), "$dst = (__xasm<Number>(push($src), convert_d))",
  [(set SPRegs:$dst, (sint_to_fp IntRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inSI_TO_SP : InstAVM2<(outs SPRegs:$dst), (ins IntRegs:$src), "$dst = Number($src)",
  [(set SPRegs:$dst, (sint_to_fp IntRegs:$src))]>,
  Requires<[UseIntrinsics]>;

def asUI_TO_SP : InstAVM2<(outs SPRegs:$dst), (ins IntRegs:$src), "$dst = (__xasm<Number>(push($src), convert_u, convert_d))",
  [(set SPRegs:$dst, (uint_to_fp IntRegs:$src))]>,
  Requires<[UseInlineAsm]>;

def inUI_TO_SP : InstAVM2<(outs SPRegs:$dst), (ins IntRegs:$src), "$dst = Number(uint($src))",
  [(set SPRegs:$dst, (uint_to_fp IntRegs:$src))]>,
  Requires<[UseIntrinsics]>;

// note no "$dst" in any of these patterns... they're handled specially in the AsmPrinter
def LDmri : InstAVM2<(outs IntRegs:$dst), (ins MEMri:$src), "($src)",
  [(set IntRegs:$dst, ADDRri:$src)]>;
def LDmrr : InstAVM2<(outs IntRegs:$dst), (ins MEMrr:$src), "($src)",
  [(set IntRegs:$dst, (inop ADDRrr:$src))]>;

let isReMaterializable = 1, isMoveImm = 1 in
{
  // note no "$dst" in any of these patterns... they're handled specially in the AsmPrinter
  def CLDc : InstAVM2<(outs IntRegs:$dst), (ins glob:$src), "($src)",
    [(set IntRegs:$dst, (inop tconstpool:$src))]>;
  def CLDg : InstAVM2<(outs IntRegs:$dst), (ins glob:$src), "($src)",
    [(set IntRegs:$dst, (inop tglobaladdr:$src))]>;
  def CLDe : InstAVM2<(outs IntRegs:$dst), (ins glob:$src), "($src)",
    [(set IntRegs:$dst, (inop texternalsym:$src))]>;
  def CLDi : InstAVM2<(outs IntRegs:$dst), (ins i32imm:$src), "($src)",
    [(set IntRegs:$dst, imm:$src)]>;
  def CFLDi : InstAVM2<(outs FPRegs:$dst), (ins _f64imm:$src), "($src)",
    [(set FPRegs:$dst, (fnop F64OP:$src))]>;
}

multiclass asLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs IntRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set IntRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs IntRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set IntRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseInlineAsm]>;
}

multiclass inLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs IntRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set IntRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS5<(outs IntRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set IntRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseIntrinsics]>;
}

// YYY better i1 load / store?
defm asLD32 : asLOAD<"(__xasm<int>(push(", "), op(0x37)))", load>;
defm asLD16 : asLOAD<"(__xasm<int>(push(", "), op(0x36), op(0x52)))", sextloadi16>;
defm asLDA16 : asLOAD<"(__xasm<int>(push(", "), op(0x36)))", extloadi16>;
defm asLDU16 : asLOAD<"(__xasm<int>(push(", "), op(0x36)))", zextloadi16>;
defm asLD8 : asLOAD<"(__xasm<int>(push(", "), op(0x35), op(0x51)))", sextloadi8>;
defm asLDA8 : asLOAD<"(__xasm<int>(push(", "), op(0x35)))", extloadi8>;
defm asLDU8 : asLOAD<"(__xasm<int>(push(", "), op(0x35)))", zextloadi8>;
defm asLD1 : asLOAD<"(__xasm<int>(push(", "), op(0x35), op(0x50)))", sextloadi1>;
defm asLDA1 : asLOAD<"(__xasm<int>(push(", "), op(0x35)))", extloadi1>;
defm asLDU1 : asLOAD<"(__xasm<int>(push(", "), op(0x35)))", zextloadi1>;

defm inLD32 : inLOAD<"li32(", ")", load>;
defm inLD16 : inLOAD<"sxi16(li16(", "))", sextloadi16>;
defm inLDA16 : inLOAD<"li16(", ")", extloadi16>;
defm inLDU16 : inLOAD<"li16(", ")", zextloadi16>;
defm inLD8 : inLOAD<"sxi8(li8(", "))", sextloadi8>;
defm inLDA8 : inLOAD<"li8(", ")", extloadi8>;
defm inLDU8 : inLOAD<"li8(", ")", zextloadi8>;
defm inLD1 : inLOAD<"sxi1(li8(", "))", sextloadi1>;
defm inLDA1 : inLOAD<"li8(", ")", extloadi1>;
defm inLDU1 : inLOAD<"li8(", ")", zextloadi1>;

multiclass asFLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs FPRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set FPRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs FPRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set FPRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseInlineAsm]>;
}

multiclass inFLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs FPRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set FPRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS5<(outs FPRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set FPRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseIntrinsics]>;
}

defm asFLD64 : asFLOAD<"(__xasm<Number>(push(", "), op(0x39)))", load>;
defm asFLD32 : asFLOAD<"(__xasm<Number>(push(", "), op(0x38)))", extloadf32>;

defm inFLD64 : inFLOAD<"lf64(", ")", load>;
defm inFLD32 : inFLOAD<"lf32(", ")", extloadf32>;

multiclass asSLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs SPRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set SPRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs SPRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set SPRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseInlineAsm]>;
}

multiclass inSLOAD<string l, string r, PatFrag opnode> {
  def ri : InstS5<(outs SPRegs:$dst), (ins MEMri:$src), "$dst = (", l, "$src", r, ")",
    [(set SPRegs:$dst, (opnode ADDRri:$src))]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS5<(outs SPRegs:$dst), (ins MEMrr:$src), "$dst = (", l, "$src", r, ")",
    [(set SPRegs:$dst, (opnode ADDRrr:$src))]>,
    Requires<[UseIntrinsics]>;
}

defm asSLD32 : asSLOAD<"(__xasm<Number>(push(", "), op(0x38)))", load>;
defm inSLD32 : inSLOAD<"lf32(", ")", load>;

multiclass asSTORE<string l, string m, string r, PatFrag opnode> {
  def ri : InstS5<(outs), (ins MEMri:$dst, IntRegs:$src), l, "$src", m, "$dst", r,
    [(opnode IntRegs:$src, ADDRri:$dst)]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs), (ins MEMrr:$dst, IntRegs:$src), l, "$src", m, "$dst", r,
    [(opnode IntRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseInlineAsm]>;
}

multiclass inSTORE<string i, PatFrag opnode> {
  def ri : InstS2<(outs), (ins MEMri:$dst, IntRegs:$src), i, "($src, $dst)",
    [(opnode IntRegs:$src, ADDRri:$dst)]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS2<(outs), (ins MEMrr:$dst, IntRegs:$src), i, "($src, $dst)",
    [(opnode IntRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseIntrinsics]>;
}

// YYY better i1 load / store?
defm asST32 : asSTORE<"__asm(push(", "), push(", "), op(0x3c))", store>;
defm asST16 : asSTORE<"__asm(push(", "), push(", "), op(0x3b))", truncstorei16>;
defm asST8 : asSTORE<"__asm(push(", "), push(", "), op(0x3a))", truncstorei8>;
defm asST1 : asSTORE<"__asm(push(", "), push(", "), op(0x3a))", truncstorei8>;

defm inST32 : inSTORE<"si32", store>;
defm inST16 : inSTORE<"si16", truncstorei16>;
defm inST8 : inSTORE<"si8", truncstorei8>;
defm inST1 : inSTORE<"si8", truncstorei8>;

multiclass asFSTORE<string l, string m, string r, PatFrag opnode> {
  def ri : InstS5<(outs), (ins MEMri:$dst, FPRegs:$src), l, "$src", m, "$dst", r,
    [(opnode FPRegs:$src, ADDRri:$dst)]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs), (ins MEMrr:$dst, FPRegs:$src), l, "$src", m, "$dst", r,
    [(opnode FPRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseInlineAsm]>;
}

multiclass inFSTORE<string i, PatFrag opnode> {
  def ri : InstS2<(outs), (ins MEMri:$dst, FPRegs:$src), i, "($src, $dst)",
    [(opnode FPRegs:$src, ADDRri:$dst)]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS2<(outs), (ins MEMrr:$dst, FPRegs:$src), i, "($src, $dst)",
    [(opnode FPRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseIntrinsics]>;
}

defm asFST64 : asFSTORE<"__asm(push(", "), push(", "), op(0x3e) /*FST64*/)", store>;
defm asFST32 : asFSTORE<"__asm(push(", "), push(", "), op(0x3d) /*FST32*/)", truncstoref32>;

defm inFST64 : inFSTORE<"sf64", store>;
defm inFST32 : inFSTORE<"sf32", truncstoref32>;

multiclass asSSTORE<string l, string m, string r, PatFrag opnode> {
  def ri : InstS5<(outs), (ins MEMri:$dst, SPRegs:$src), l, "$src", m, "$dst", r,
    [(opnode SPRegs:$src, ADDRri:$dst)]>,
    Requires<[UseInlineAsm]>;
  def rr : InstS5<(outs), (ins MEMrr:$dst, SPRegs:$src), l, "$src", m, "$dst", r,
    [(opnode SPRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseInlineAsm]>;
}

multiclass inSSTORE<string i, PatFrag opnode> {
  def ri : InstS2<(outs), (ins MEMri:$dst, SPRegs:$src), i, "($src, $dst)",
    [(opnode SPRegs:$src, ADDRri:$dst)]>,
    Requires<[UseIntrinsics]>;
  def rr : InstS2<(outs), (ins MEMrr:$dst, SPRegs:$src), i, "($src, $dst)",
    [(opnode SPRegs:$src, ADDRrr:$dst)]>,
    Requires<[UseIntrinsics]>;
}

defm asSST32 : asSSTORE<"__asm(push(", "), push(", "), op(0x3d))", store>;
defm inSST32 : inSSTORE<"sf32", store>;

//YYY doesnt round!
def FAKE_FRR : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = $src/*round_inreg*/",
  [(set FPRegs:$dst, (fround_inreg FPRegs:$src))]>;

def FAKE_FEX : InstAVM2<(outs FPRegs:$dst), (ins SPRegs:$src), "$dst = $src/*fextend*/",
  [(set FPRegs:$dst, (fextend SPRegs:$src))]>;

def FAKE_FRO : InstAVM2<(outs SPRegs:$dst), (ins FPRegs:$src), "$dst = $src/*fround*/",
  [(set SPRegs:$dst, (fround FPRegs:$src))]>;

def FABS : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = Math.abs($src)",
  [(set FPRegs:$dst, (fabs FPRegs:$src))]>;

def FNEG : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = -$src",
  [(set FPRegs:$dst, (fneg FPRegs:$src))]>;

def FSIN : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = Math.sin($src)",
  [(set FPRegs:$dst, (fsin FPRegs:$src))]>;

def FCOS : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = Math.cos($src)",
  [(set FPRegs:$dst, (fcos FPRegs:$src))]>;

def FSQRT : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$src), "$dst = Math.sqrt($src)",
  [(set FPRegs:$dst, (fsqrt FPRegs:$src))]>;

def fpow      : SDNode<"ISD::FPOW"      , SDTFPBinOp>;
def FPOW : InstAVM2<(outs FPRegs:$dst), (ins FPRegs:$base, FPRegs:$pow), "$dst = Math.pow($base, $pow)",
  [(set FPRegs:$dst, (fpow FPRegs:$base, FPRegs:$pow))]>;

