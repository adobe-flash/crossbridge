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

//===-- AVM2/AVM2MCCodeEmitter.cpp - Convert AVM2 code to ABC -------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#define DEBUG_TYPE "mccodeemitter"
#include "AVM2TargetAsmInfo.h"
#include "AVM2TargetMachine.h"
#include "AVM2.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionCOFF.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/Object/MachOFormat.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace
{
class AVM2MCCodeEmitter : public MCCodeEmitter
{
    AVM2MCCodeEmitter(const AVM2MCCodeEmitter &); // DO NOT IMPLEMENT
    void operator=(const AVM2MCCodeEmitter &); // DO NOT IMPLEMENT
    const MCInstrInfo &MCII;
    const MCSubtargetInfo &STI;
    MCContext &Ctx;
public:

    AVM2MCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
                     MCContext &ctx) : MCII(mcii), STI(sti), Ctx(ctx) { }

    virtual void EncodeInstruction(const llvm::MCInst&, llvm::raw_ostream&, llvm::SmallVectorImpl<llvm::MCFixup>&) const {
        assert(false);
    }
};
} // end anonymous namespace

MCCodeEmitter *llvm::createAVM2MCCodeEmitter(const MCInstrInfo &MCII,
        const MCRegisterInfo &RI,
        const MCSubtargetInfo &STI,
        MCContext &Ctx)
{
    return new AVM2MCCodeEmitter(MCII, STI, Ctx);
}
