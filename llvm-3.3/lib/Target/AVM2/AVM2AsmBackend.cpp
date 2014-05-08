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

//===-- AVM2AsmBackend.cpp - AVM2 Assembler Backend -------------------------===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2TargetAsmInfo.h"
#include "AVM2TargetMachine.h"
#include "AVM2.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionCOFF.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/Object/MachOFormat.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace
{

class AVM2ABCObjectWriter : public MCObjectWriter
{
public:
    AVM2ABCObjectWriter(raw_ostream &OS) : MCObjectWriter(OS, true) { }

    virtual void ExecutePostLayoutBinding(llvm::MCAssembler&, const llvm::MCAsmLayout&) {
        assert(false);
    }

    virtual void RecordRelocation(const llvm::MCAssembler&, const llvm::MCAsmLayout&, const llvm::MCFragment*, const llvm::MCFixup&, llvm::MCValue, uint64_t&) {
        assert(false);
    }

    virtual void WriteObject(llvm::MCAssembler&, const llvm::MCAsmLayout&) {
        assert(false);
    }
};

class AVM2AsmBackend : public MCAsmBackend
{
public:
    AVM2AsmBackend(const Target &T) : MCAsmBackend() { }

    MCObjectWriter *createObjectWriter(raw_ostream &OS) const {
        return new AVM2ABCObjectWriter(OS);
    }

    virtual unsigned int getNumFixupKinds() const {
        assert(false);
        return -1;
    }

    virtual void applyFixup(const llvm::MCFixup&, char*, unsigned int, uint64_t) const {
        assert(false);
    }

    virtual bool mayNeedRelaxation(const llvm::MCInst&) const {
        assert(false);
        return false;
    }

    virtual void relaxInstruction(const llvm::MCInst&, llvm::MCInst&) const {
        assert(false);
    }

    virtual bool writeNopData(uint64_t, llvm::MCObjectWriter*) const {
        assert(false);
        return false;
    }

    virtual bool fixupNeedsRelaxation(const llvm::MCFixup&, uint64_t, 
            const MCRelaxableFragment *DF, const llvm::MCAsmLayout&) const {
        assert(false);
        return false;
    }
};

} // end anonymous namespace

MCAsmBackend *llvm::createAVM2AsmBackend(const Target &T, StringRef TT, 
        StringRef)
{
    return new AVM2AsmBackend(T);
}

