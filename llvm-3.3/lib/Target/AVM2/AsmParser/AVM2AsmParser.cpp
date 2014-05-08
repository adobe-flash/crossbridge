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

#include "AVM2.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCTargetAsmParser.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
using namespace llvm;

namespace {
    class AVM2AsmParser : public MCTargetAsmParser {
    public:
        AVM2AsmParser(MCSubtargetInfo &sti, MCAsmParser &parser)
            : MCTargetAsmParser()
        {
            //printf("AVM2AsmParser!\n");
        }
        
        virtual bool ParseRegister(unsigned int&, llvm::SMLoc&, llvm::SMLoc&)
        {
            return false; // abort();
        }
        
        virtual bool ParseInstruction(llvm::ParseInstructionInfo&, llvm::StringRef, llvm::SMLoc, llvm::SmallVectorImpl<llvm::MCParsedAsmOperand*>&)
        {
            abort();
			return false;
        }
        
        virtual bool ParseDirective(llvm::AsmToken)
        {
            return false; // abort();
        }
        
        virtual bool MatchAndEmitInstruction(llvm::SMLoc, unsigned int&, llvm::SmallVectorImpl<llvm::MCParsedAsmOperand*>&, llvm::MCStreamer&, unsigned int&, bool)
        {
            return false; // abort();
        }

        virtual bool mnemonicIsValid(StringRef Mnemonic)
        {
            abort();
            return false;
        }

        virtual void convertToMapAndConstraints(unsigned int, const llvm::SmallVectorImpl<llvm::MCParsedAsmOperand*>&)
        {
            abort();
        }
    };
}

/// Force static initialization.
extern "C" void LLVMInitializeAVM2AsmParser() {
    RegisterMCAsmParser<AVM2AsmParser> X(TheAVM2Target);
}

