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
//===- AVM2RegisterInfo.td - AVM2 Register defs ----------*- tablegen -*-===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the AVM2 register file 
//===----------------------------------------------------------------------===//

class AVM2Reg<string n> : Register<n> {
  field bits<7> Num;
  let Namespace = "AVM2";
}

// Registers are identified with 7-bit ID numbers.

// Ri - 32-bit integer registers
class Ri<bits<7> num, string n> : AVM2Reg<n> {
  let Num = num;
}

// Rd - Slots in the FP register file for 64-bit floating-point values.
class Rd<bits<7> num, string n> : AVM2Reg<n> {
  let Num = num;
}

// Rf - Slots in the FP register file for 32-bit floating-point values.
class Rf<bits<7> num, string n, list<Register> aliases> : AVM2Reg<n> {
  let Num = num;
  let Aliases = aliases;
}

// AVM registers (which overlap Intel and try to make use of Intel DWARF #s)
def EAX : Ri< 64, "EAX">, DwarfRegNum<[0]>;
def EDX : Ri< 65, "EDX">, DwarfRegNum<[2]>; 
def EDP : Ri< 66, "EDP">, DwarfRegNum<[4]>;  // debug pointer (ESP)
def EI32 : Ri< 67, "EI32">, DwarfRegNum<[3]>;  // debug pointer (EBX)
def EF32 : Ri< 68, "EF32">, DwarfRegNum<[6]>;  // debug pointer (ESI)
def EF64 : Ri< 69, "EF64">, DwarfRegNum<[7]>;  // debug pointer (EDI)
def EBP : Ri< 70, "EBP">, DwarfRegNum<[5]>;
def ESP : Ri< 71, "ESP">, DwarfRegNum<[1]>; // (ECX)
def EIP : Ri< 80, "EIP">, DwarfRegNum<[8]>;

// Intel FP regs
def ST0 : Rd< 64, "ST0">, DwarfRegNum<[-1]>;
// Fake "float" return register
def SST0 : Rf< 64, "ST0", [ST0]>, DwarfRegNum<[-1]>;

// Register classes.
//
// SFXME: the register order should be defined in terms of the preferred
// allocation order...
//
def IntRegs : RegisterClass<"AVM2", [i32], 9, 
                                     (add EAX, EDX, EDP, EI32, EF32, EF64, EBP, ESP, EIP
                                     )> {
  code MethodProtos = [{
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  code MethodBodies = [{
    IntRegsClass::iterator
    IntRegsClass::allocation_order_end(const MachineFunction &MF) const {
      return begin();
    }
  }];
}

def SPRegs : RegisterClass<"AVM2", [f32], 1, (add
  SST0
  )> {
  code MethodProtos = [{
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  code MethodBodies = [{
    SPRegsClass::iterator
    SPRegsClass::allocation_order_end(const MachineFunction &MF) const {
      return begin();
    }
  }];
}

def FPRegs : RegisterClass<"AVM2", [f64], 1, (add
  ST0
  )> {
  code MethodProtos = [{
    iterator allocation_order_end(const MachineFunction &MF) const;
  }];
  code MethodBodies = [{
    FPRegsClass::iterator
    FPRegsClass::allocation_order_end(const MachineFunction &MF) const {
      return begin();
    }
  }];
}

def CF : AVM2Reg<"carry">;
