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

//===-- AVM2.h - Top-level interface for AVM2 representation --*- C++ -*-===//

#ifndef TARGET_AVM2_H
#define TARGET_AVM2_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionCOFF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/CodeGen/ValueTypes.h"

#include <iosfwd>
#include <cassert>

namespace llvm
{
class FunctionPass;
class TargetMachine;

FunctionPass *createAVM2ISelDag(TargetMachine &TM, CodeGenOpt::Level OptLevel);
extern Target TheAVM2Target;

// TODO move?
// metadata to apply to initialization functions (that probably shouldn't be JITted)
#define INIT_MD "[ExecPolicy(OSR=2)]"

// TODO move?
inline StringRef getSectionName(const MCSection &Sect)
{
    switch(Sect.getVariant()) {
    case MCSection::SV_COFF:
        assert(false); // we should have chosen elf
        return ((const MCSectionCOFF *)&Sect)->getSectionName();
    case MCSection::SV_ELF:
        return ((const MCSectionELF *)&Sect)->getSectionName();
    case MCSection::SV_MachO:
        assert(false); // we should have chosen elf
        return ((const MCSectionMachO *)&Sect)->getSectionName();
    default:
        assert(false);
    }
    return StringRef();
}

// TODO move?
struct AVM2MCStreamerObserver {
    virtual void notifyCommonSym(MCSymbol *Sym, bool IsLocal) = 0;
    virtual void notifyWeakReference(MCSymbol *Alias, const MCSymbol *Target) = 0;
    virtual void notifyDataSym(const MCSymbol *Sym, unsigned Offs) = 0;
    virtual void notifyLabelEmitted(MCSymbol *Sym, unsigned LineNo, bool InFn) = 0;
    virtual bool isPrivate(const MCSymbol *Sym) = 0;
    virtual void notifyPackageDirective(const char *Pkg) = 0;
};

// TODO move?
class AVM2MCStreamer : public MCStreamer
{
public:
    AVM2MCStreamer(MCContext &Context) : MCStreamer(SK_AsmStreamer, Context) {}
    virtual void setObserver(AVM2MCStreamerObserver *O) = 0;
    virtual bool setUseSecondStream() = 0;
    virtual void setModulePackageName(const std::string &mn) = 0;
    virtual void setUseInlineAsm(bool use) = 0;
    virtual void setEmitDebugLines(bool emit) = 0;
    virtual void setEmitFixupCalls(bool emit) = 0;

    virtual void EmitCFIEndProcImpl(MCDwarfFrameInfo &Frame) {
        RecordProcEnd(Frame);
    }
};

MCCodeEmitter *createAVM2MCCodeEmitter(const MCInstrInfo &MCII,
        const MCRegisterInfo &RI,
        const MCSubtargetInfo &STI,
        MCContext &Ctx);

MCAsmBackend *createAVM2AsmBackend(const Target &T,
                                       StringRef TT,
                                       StringRef);

} // end namespace llvm;

// Defines symbolic names for AVM2 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_HEADER
#define GET_REGINFO_ENUM
#include "AVM2GenRegisterInfo.h.inc"

// Defines symbolic names for the AVM2 instructions.
//
#define GET_INSTRINFO_ENUM
#include "AVM2GenInstrInfo.inc"

namespace AVM2ISD
{
}
#endif
