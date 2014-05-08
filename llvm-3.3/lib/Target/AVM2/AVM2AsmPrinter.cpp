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

//===-- AVM2AsmPrinter.cpp - AVM2 LLVM assembly writer ------------------===//
 
static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#define DEBUG_TYPE "asm-printer"
#include "AVM2.h"
#include "AVM2InstrInfo.h"
#include "AVM2TargetMachine.h"
#include "llvm/IR/Module.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSection.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/SparseBitVector.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"

#ifndef _WIN32
#include <uuid/uuid.h>
#else
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")
#endif

#include <algorithm>
#include <map>
#include <set>
#include <stdio.h>
#include <cmath>

using namespace llvm;

static cl::opt<bool> GenDbgSymTable(
  "gendbgsymtable",
  cl::init(false),
  cl::desc("INERNAL flascc opt"),
  cl::Hidden);

cl::opt<bool> InstrumentFlasccInitCode(
  "instrument-flascc-init-code",
  cl::init(false),
  cl::desc("INERNAL flascc opt"),
  cl::Hidden);

#if !defined(__CYGWIN__) && !defined(__linux__)
extern "C" char* __dtoa(double d, int mode, int ndigits, int *decpt,
            int *sign, char **rve);
extern "C" void __freedtoa (char*);
#endif

namespace
{

// AS3 metadata to mark a symbol weak
// keep in sync w/ SwfResolve.java
static const char *WeakMD = "[Weak]";
// AS3 metadata to mark a function as a "global method"
// keep in sync w/ SwfResolve.java
static const char *GlobalMethodMD = "[GlobalMethod]";
// AS3 metadata to mark an ABC as a link candidate
// AlcTool will only link ABCs that have this metadata specified
static const char *LinkABCMD = "[LinkABC]";
// AS3 metadata name for identfying C symbols to abcnm and other tools
// keep in sync w/ AbcNM.java
static const char *CsymMDName = "Csym";

#ifdef __MINGW32__
    #define RPC_CSTR unsigned char*
#endif

class UUID
{
#if _WIN32
	::UUID uuid;

public:
  UUID()
  {
    UuidCreate(&uuid);
  }

  std::string str() const
  {
	RPC_CSTR cstr = NULL;
	UuidToStringA((::UUID*)&uuid, &cstr);
	std::string str((char *)cstr);
	RpcStringFreeA(&cstr);
    return str;
  }
#else
  uuid_t uuid;

public:
  UUID()
  {
    uuid_generate(uuid);
  }

  std::string str() const
  {
    char buf[40];

    uuid_unparse(uuid, buf);
    return buf;
  }
#endif
};
    
static const char* implicitDefs[5] = {
    "__DYNAMIC",
    "memset",
    "memcpy",
    "memmove",
    "abort"
};
    
bool isImplicitlyDefined(MCSymbol &Sym)
{
    StringRef sr = Sym.getName();
    
    for(unsigned int i=0; i<sizeof(implicitDefs)/sizeof(char*); i++)
    {
        if(sr.equals(implicitDefs[i]))
            return true;
    }
    return false;
}

bool isImplicitlyDefined(const char *Sym)
{
    for(unsigned int i=0; i<sizeof(implicitDefs)/sizeof(char*); i++)
    {
        if(strcmp(Sym, implicitDefs[i]) == 0)
            return true;
    }
    return false;
}

class AVM2AsmPrinter : public AsmPrinter, public AVM2MCStreamerObserver
{
public:
    explicit AVM2AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
        : AsmPrinter(TM, Streamer), FunCount(0), CurModule(NULL) {
        ((AVM2MCStreamer *)&Streamer)->setObserver(this);
    }

    unsigned getFunAlignment() {
        return 4;
    }

    std::string getMangSectionName(const MCSection &Sect) {
        StringRef SectName = getSectionName(Sect);
        SmallString<256> MN;
        Mang->getNameWithPrefix(MN, SectName);
        std::string R = MN.str();
        return "S" + R;

    }

    struct CSymEntry
    {
        StringRef type, name, section;
        int value, num;

        CSymEntry(StringRef _t, StringRef _n) : type(_t), name(_n), num(2) {}
        CSymEntry(StringRef _t, StringRef _n, StringRef _s) : type(_t), name(_n), section(_s), num(3) {}
        CSymEntry(StringRef _t, StringRef _n, StringRef _s, int _v) : type(_t), name(_n), section(_s), value(_v), num(4) {}
    };
    std::vector<CSymEntry> CSymData;

    void emitCsymMD(raw_svector_ostream &OS, bool record, StringRef type, StringRef name)
    {
        OS << "[" << CsymMDName << "(\"" << type << "\", \"" << name << "\")]\n";
        if(record)
            CSymData.push_back(CSymEntry(type, name));
    }

    void emitCsymMD(raw_svector_ostream &OS, bool record, StringRef type, StringRef name, StringRef section)
    {
        OS << "[" << CsymMDName << "(\"" << type << "\", \"" << name << "\", \"" << section << "\")]\n";
        if(record)
            CSymData.push_back(CSymEntry(type, name, section));
    }

    void emitCsymMD(raw_svector_ostream &OS, bool record, StringRef type, StringRef name, StringRef section, int value)
    {
        OS << "[" << CsymMDName << "(\"" << type << "\", \"" << name << "\", \"" << section << "\", " << value << ")]\n";
        if(record)
            CSymData.push_back(CSymEntry(type, name, section, value));
    }

    virtual void notifyPackageDirective(const char *Pkg) {
      if(Pkg)
      {
        if(Pkg == (const char *)-1)
            ChangePackage(kPrivate);
        else
            ChangePackage(Pkg);
      }
      else
        ChangePackage(kPublic);
    }

    typedef std::set<const MCSymbol *> SymSet;
    SymSet CommonSyms;

    virtual void notifyCommonSym(MCSymbol *Sym, bool IsLocal) {
	// TODO how does this work w/ templates?
        Sym->setSection(*(getObjFileLowering().getBSSSection()));
        if(!IsLocal) {
            CommonSyms.insert(Sym);
        }
    }

    typedef std::map<const MCSymbol *, const MCSymbol *> SymRefMap;
    SymRefMap WeakRefs;

    virtual void notifyWeakReference(MCSymbol *Alias, const MCSymbol *Target) {
        WeakRefs.insert(std::make_pair(Alias, Target));
    }

    typedef std::map<const MCSymbol *, unsigned> Sym2NumMap;
    Sym2NumMap Sym2Num;

    typedef std::set<const MCSection *> SectSet;
    SectSet Sections;

    virtual void notifyDataSym(const MCSymbol *Sym, unsigned Offs) {
        Sections.insert(&(Sym->getSection()));
        Sym2Num.insert(std::make_pair(Sym, Offs));
    }

    typedef std::set<const MCSymbol *> SymbolSet;
    SymbolSet LabelsSeen;
    unsigned LastLabelLineNo; // line # on which the last label was emitteed

    virtual void notifyLabelEmitted(MCSymbol *Sym, unsigned LineNo, bool InFn) {
        LastLabelLineNo = LineNo;

	// work around -ffuction-sections
	Sym->setSection(*(getObjFileLowering().getTextSection())); 

        if(MMI->hasDebugInfo()) { // only need "text" symbols and sections when debugging
            Sections.insert(&(Sym->getSection()));
            Sym2Num.insert(std::make_pair(Sym, LineNo));
        }
        if(InFn) { //xxx && LabelsSeen.find(Sym) == LabelsSeen.end()) // TODO we only want labels on back-branches, right?
            LabelsSeen.insert(Sym);
	    if(TM.getSubtarget<AVM2Subtarget>().useInlineAsm())
	    {
            	StringRef Str("\t__asm(label)\n");
            	OutStreamer.EmitRawText(Str);
	    }
        }
    }

    std::set<const MCSymbol*> PrivateSyms;
    virtual bool isPrivate(const MCSymbol *Sym)
    {
        return PrivateSyms.find(Sym) != PrivateSyms.end();
    }

    bool FunHasAS3Sig; // using "as3sig" annotation
    bool FunIsNaked; // current function is naked
    bool FunCallsSetJmp; // MF->callsSetJmp() || we use eh_sjlj_setjmp for current function
    unsigned FunSJID; // current setjmp id

    // functions w/ explicitly specified AS3 signatures and/or packages
    mutable enum PackageType
    {
      kPrivate,
      kPublic,
      kCustom
    } CurPackage; // the package we're currently in

    typedef std::map<const Function *, std::string> Fun2StrMap;
    typedef std::multimap<const Function *, std::string> Fun2StrMultiMap;
    Fun2StrMultiMap Fun2Import;
    Fun2StrMap Fun2Sig;
    Fun2StrMap Fun2Package;
    Fun2StrMultiMap Fun2MD;

    unsigned FunCount;
    typedef std::map<const Function *, unsigned> Fun2NumMap;
    Fun2NumMap Fun2Num;
    typedef std::set<std::string> TEMap; // target externals
    TEMap TE;

    void removeTE(const MCSymbol &Sym) {
        SmallString<128> Str;
        raw_svector_ostream OS(Str);

        OS << Sym;
        OS.flush(); // TODO why is this necessary?

        std::string S(Str.begin(), Str.end());
        TE.erase(S);
    }

    virtual const char *getPassName() const {
        return "AVM2 Assembly Printer";
    }

    void printCond(const MachineInstr *MI, int opNum, raw_ostream &OS, bool isFloat = false) {
        int CC = MI->getOperand(opNum).getImm();
#if 0 // __xasm generates more efficient bytecode here than "uint" when compiled by asc, but "uint" is still correct
#define UB "__xasm<uint>(push("
#define UE "), convert_u)"
#else
#define UB "uint("
#define UE ")"
#endif
        static const char *IntOp[] = {
            //     SETFALSE,      //    0 0 0 0       Always false (always folded)
            "false",
            //     SETOEQ,        //    0 0 0 1       True if ordered and equal
            "($1)==($2)",
            //     SETOGT,        //    0 0 1 0       True if ordered and greater than
            "($1)>($2)",
            //     SETOGE,        //    0 0 1 1       True if ordered and greater than or equal
            "($1>=$2)",
            //     SETOLT,        //    0 1 0 0       True if ordered and less than
            "($1)<($2)",
            //     SETOLE,        //    0 1 0 1       True if ordered and less than or equal
            "($1)<=($2)",
            //     SETONE,        //    0 1 1 0       True if ordered and operands are unequal
            "($1)!=($2)",
            //     SETO,          //    0 1 1 1       True if ordered (no nans)
            "true",
            //     SETUO,         //    1 0 0 0       True if unordered: isnan(X) | isnan(Y)
            "false",
            //     SETUEQ,        //    1 0 0 1       True if unordered or equal
            UB"$1"UE"=="UB"$2"UE,
            //     SETUGT,        //    1 0 1 0       True if unordered or greater than
            UB"$1"UE">"UB"$2"UE,
            //     SETUGE,        //    1 0 1 1       True if unordered, greater than, or equal
            UB"$1"UE">="UB"$2"UE,
            //     SETULT,        //    1 1 0 0       True if unordered or less than
            UB"$1"UE"<"UB"$2"UE,
            //     SETULE,        //    1 1 0 1       True if unordered, less than, or equal
            UB"$1"UE"<="UB"$2"UE,
            //     SETUNE,        //    1 1 1 0       True if unordered or not equal
            UB"$1"UE"!="UB"$2"UE,
            //     SETTRUE,       //    1 1 1 1       Always true (always folded)
            "true",
        };
#undef UB
#undef UE
        static const char *FloatOp[] = {
            //     SETFALSE,      //    0 0 0 0       Always false (always folded)
            "false",
            //     SETOEQ,        //    0 0 0 1       True if ordered and equal
            "($1)==($2)",
            //     SETOGT,        //    0 0 1 0       True if ordered and greater than
            "($1)>($2)",
            //     SETOGE,        //    0 0 1 1       True if ordered and greater than or equal
            "($1>=$2)",
            //     SETOLT,        //    0 1 0 0       True if ordered and less than
            "($1)<($2)",
            //     SETOLE,        //    0 1 0 1       True if ordered and less than or equal
            "($1)<=($2)",
            //     SETONE,        //    0 1 1 0       True if ordered and operands are unequal
            "($1)!=($2)",
#if CMP_HELPERS
            //     SETO,          //    0 1 1 1       True if ordered (no nans)
            "ord(($1),($2))",
            //     SETUO,         //    1 0 0 0       True if unordered: isnan(X) | isnan(Y)
            "uno(($1),($2))",
            //     SETUEQ,        //    1 0 0 1       True if unordered or equal
            "unoeq(($1),($2))",
            //     SETUGT,        //    1 0 1 0       True if unordered or greater than
            "unogt(($1),($2))",
            //     SETUGE,        //    1 0 1 1       True if unordered, greater than, or equal
            "unoge(($1),($2))",
            //     SETULT,        //    1 1 0 0       True if unordered or less than
            "unolt(($1),($2))",
            //     SETULE,        //    1 1 0 1       True if unordered, less than, or equal
            "unole(($1),($2))",
            //     SETUNE,        //    1 1 1 0       True if unordered or not equal
            "unone(($1),($2))",
#else
            // note: using bitor and bitand here on purpose! logical and/or introduce flow control in the abc...
            // and always evaluating all 3 here is sufficiently cheap (but having real unordered compare ops would be better)
            //     SETO,          //    0 1 1 1       True if ordered (no nans)
            "(($1)==($1) & ($2)==($2))",
            //     SETUO,         //    1 0 0 0       True if unordered: isnan(X) | isnan(Y)
            "(($1)!=($1) | ($2)!=($2))",
            //     SETUEQ,        //    1 0 0 1       True if unordered or equal
            "(($1)==($2) | ($1)!=($1) | ($2)!=($2))",
            //     SETUGT,        //    1 0 1 0       True if unordered or greater than
            "(($1)>($2) | ($1)!=($1) | ($2)!=($2))",
            //     SETUGE,        //    1 0 1 1       True if unordered, greater than, or equal
            "(($1)>=($2) | ($1)!=($1) | ($2)!=($2))",
            //     SETULT,        //    1 1 0 0       True if unordered or less than
            "(($1)<($2) | ($1)!=($1) | ($2)!=($2))",
            //     SETULE,        //    1 1 0 1       True if unordered, less than, or equal
            "(($1)<=($2) | ($1)!=($1) | ($2)!=($2))",
            //     SETUNE,        //    1 1 1 0       True if unordered or not equal
            "(($1)!=($2) | ($1)!=($1) | ($2)!=($2))",
#endif
            //     SETTRUE,       //    1 1 1 1       Always true (always folded)
            "true",
            //     SETFALSE2,     //  1 X 0 0 0       Always false (always folded)
            //     SETEQ,         //  1 X 0 0 1       True if equal
            //     SETGT,         //  1 X 0 1 0       True if greater than
            //     SETGE,         //  1 X 0 1 1       True if greater than or equal
            //     SETLT,         //  1 X 1 0 0       True if less than
            //     SETLE,         //  1 X 1 0 1       True if less than or equal
            //     SETNE,         //  1 X 1 1 0       True if not equal
            //     SETTRUE2,      //  1 X 1 1 1       Always true (always folded)
        };

        if(CC > ISD::SETTRUE || TM.getSubtarget<AVM2Subtarget>().ForceOrderedCompares) { // if don't care, make ordered
            CC &= 7;
        }
        const char *tmpl = isFloat ? FloatOp[CC] : IntOp[CC];
        char buf[256];
        char *dst = buf;
        char ch;

        while((ch = *tmpl++))
        {
          if(ch == '$')
          {
            int opOffs = (*tmpl++) - '0';

            if(dst != buf)
            {
              *dst = 0;
              dst = buf;
              OS << buf;
            }
            printOperand(MI, MI->getOperand(opNum+opOffs), OS);
          }
          else
            *dst++ = ch;
        }
        if(dst != buf)
        {
          *dst = 0;
          OS << buf;
        }
    }

    void printFloatCond(const MachineInstr *MI, int opNum, raw_ostream &OS) {
        printCond(MI, opNum, OS, true);
    }

    void printFloatImm(const MachineInstr *MI, int opNum, raw_ostream &OS) {
        struct dword {
            uint32_t hi, lo;
        };

        union {
            double d;
            dword dw;
        };
        
        dw.hi = MI->getOperand(opNum).getImm();
        dw.lo = MI->getOperand(opNum+1).getImm();
        char buf[512];

        // We must print special values ourselves to work around a MingW bug
        if(std::isnan(d)) {
            sprintf(&buf[0], "%s.nan", getPackageName(*CurModule).c_str());
        } else if(std::isinf(d) != 0 && d > 0) {
            sprintf(&buf[0], "%s.inf", getPackageName(*CurModule).c_str());
        } else if(std::isinf(d) != 0 && d < 0) {
            sprintf(&buf[0], "-%s.inf", getPackageName(*CurModule).c_str());
        } else {
            
            #if defined(__CYGWIN__) || defined(__linux__)
            sprintf(&buf[0], "%.17e", d);
            #else
            // (s)printf is no good on mingw, it lacks the precision we need
            int decpt, signflag;
            char *eptr;
            char *dtoaresult = ::__dtoa(d, 2, 32, &decpt, &signflag, &eptr);
            char signchar = signflag ? '-' : ' ';

            if(decpt == 0)
                sprintf(&buf[0], "%c0.%s", signchar, dtoaresult);
            else
                sprintf(&buf[0], "%c0.%se%d", signchar, dtoaresult, decpt);
            ::__freedtoa(dtoaresult);
            #endif
        }

        OS << &buf[0];
    }

	MachineLocation getDebugValueLocation(const MachineInstr *MI) const {
	  MachineLocation Location;
	  // TODO
	  assert(false);
	  return Location;
	}

    std::map<const TargetRegisterClass *, unsigned> FunRCVRegCount;
    typedef std::map<unsigned, unsigned> VReg2IndexMap;
    VReg2IndexMap FunVReg2RCRelativeIndex;

    const MachineInstr *regIsTrivial(const MachineRegisterInfo *RI, unsigned Reg);

    void printRegister(const MachineRegisterInfo *RI, unsigned Reg, raw_ostream &O);
    void printOperand(const MachineInstr *MI, const MachineOperand &MO, raw_ostream &O);

    void printRegister(const MachineInstr *MI, unsigned Reg, raw_ostream &O);
    void printOperand(const MachineInstr *MI, int opNum, raw_ostream &OS);
    void printMemOperand(const MachineInstr *MI, int opNum, raw_ostream &OS,
                         const char *Modifier = 0);

    // true if opcode represents a trivial "load" -- i.e., immediate, simple address
    static bool opcodeIsTrivial(int Opcode) {
        switch(Opcode) {
        case AVM2::LDmri:
        case AVM2::LDmrr:
        case AVM2::CLDc:
        case AVM2::CLDg:
        case AVM2::CLDe:
        case AVM2::CLDi:
        case AVM2::CFLDi:
            return true;
        }
        return false;
    }

    bool IsCallToSetJmp(const MachineInstr *MI) {
        StringRef SR;

        switch(MI->getOpcode()) {
        case AVM2::eh_sjlj_setjmp:
            return true;
        case AVM2::pCALLe:
            SR = MI->getOperand(0).getSymbolName();
            break;
        case AVM2::pCALLg:
            SR = MI->getOperand(0).getGlobal()->getName();
            break;
        default:
            return false;
        }
        const char *sz = SR.data();
        size_t len = SR.size();
        while(len && (*sz == '\2' || *sz == '_')) {
            sz++;
            len--;
        }
        return (len == 6) && !strncmp(sz, "setjmp", 6);
    }

    virtual void EmitInlineAsmProlog(const MachineInstr *MI)
    {
        ChangePackage(kPrivate);
    }
            
    virtual void EmitInstruction(const MachineInstr *MI) {
#if 0
        DebugLoc DL = MI->getDebugLoc();
        if(!DL.isUnknown()) {
            SmallString<128> Str;
            raw_svector_ostream OS(Str);
            OS << "trace(\"@" << DL.getLine() << ": \"" << "+ESP/16)\n";
            OutStreamer.EmitRawText(OS.str());
        }
#endif
		MCLineEntry::Make(&OutStreamer, getCurrentSection());

        bool opIsTrivial = opcodeIsTrivial(MI->getOpcode());

        if(opIsTrivial) {
            const MachineRegisterInfo *RI = &(MF->getRegInfo());

            // don't gen the instruction if it's trivial
            if(regIsTrivial(RI, MI->getOperand(0).getReg())) {
                return;
            }
        }

        SmallString<128> Str;
        raw_svector_ostream OS(Str);
        if(opIsTrivial) {
            OS << "\t";
            printRegister(MI, MI->getOperand(0).getReg(), OS);
            OS << " =";
        }
        
        bool IsSetJmp = IsCallToSetJmp(MI);
        int Opcode = MI->getOpcode();

        if(Opcode == AVM2::eh_sjlj_longjmp) { // special case eh_sjlj_longjmp!
            OS << "\tlongjmp(";
            printOperand(MI, MI->getOperand(0), OS); // pointer
            OS << ", 1)\n"; // eh_sjlj_longjmp always passes "1" as second param
        } else if(Opcode == AVM2::eh_sjlj_setjmp) { // special case eh_sjlj_setjmp!
            OS << "\teax = setjmp(";
            printOperand(MI, MI->getOperand(0), OS); // pointer
            OS << ", " << FunSJID << ", esp)\n";
        } else if(IsSetJmp) {
            OS << "\tsjid = " << FunSJID << "; ";
        }
        if(!FunIsNaked && (MI->getDesc().isCall() || MI->getDesc().isReturn())) {
            OS << "\tESP = esp\n";    // set up ESP for calls/returns
        }
        if(FunHasAS3Sig && MI->getDesc().isReturn())
          OS << "\treturn _as3ReturnValue;\n";
        else
          printInstruction(MI, OS);
        if(IsSetJmp) {
	    if(TM.getSubtarget<AVM2Subtarget>().useInlineAsm())
            	OS << "\n\t__asm(lbl(\".SJ_" << FunSJID++ << "\"), label)";
	    else
            	OS << "\n\tSJ_" << FunSJID++ << "__:";
        }
        OutStreamer.EmitRawText(OS.str());
    }
    void printInstruction(const MachineInstr *MI, raw_ostream &OS);// autogen'd.
    static const char *getRegisterName(unsigned RegNo);

    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                         unsigned AsmVariant, const char *ExtraCode,
                         raw_ostream &O);
    bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                               unsigned AsmVariant, const char *ExtraCode,
                               raw_ostream &O);

    virtual bool isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB)
    const;

    std::string getPackageName(const Module &M) const {
        return "C_Run";
    }

    std::string getModulePackageName(const Module &M) const {
	// need to add a UUID to the module ident so it's really unique
	static std::map<const Module *, UUID> modUUIDs;

        std::string uuid = modUUIDs[&M].str();
        std::string EID = M.getModuleIdentifier();
        std::replace(EID.begin(), EID.end(), '@', '_'); // Mangler allows '@' but we don't.. should be only char Mangler thinks is ok that we don't like

        EID += ":";
        EID += uuid;

        Twine TMN = EID;
        SmallString<256> MN;
        Mang->getNameWithPrefix(MN, TMN);
        std::string R = MN.str();
        return getPackageName(M) + R;
    }

    // Following the behavior of llvm-2.9/tools/lto/LTOModule.cpp
    bool isPublic(const GlobalValue *GV) {
        return (GV->hasProtectedVisibility() || GV->hasHiddenVisibility() || 
                GV->hasExternalLinkage() || GV->hasWeakLinkage() ||
                GV->hasLinkOnceLinkage() || GV->hasCommonLinkage() ||
                GV->hasLinkerPrivateWeakLinkage());
    }

    // Following the behavior of llvm-2.9/tools/lto/LTOModule.cpp
    bool isWeak(const GlobalValue *GV) {
        return (GV->hasWeakLinkage() || GV->hasLinkOnceLinkage() ||
                GV->hasLinkerPrivateWeakLinkage());
    }

    bool UseSparseFun2Ptr;

    unsigned getFunctionAddr(const Function *Fun)
    {
      if(UseSparseFun2Ptr)
        return Fun2EntryLabelLineNo[Fun]; 
      else
        return getFunAlignment() * Fun2Num[Fun];
    }

    const Module *CurModule;

    void ChangePackage(PackageType PT) const
    {
	if(CurPackage != PT)
	{
            SmallString<128> Str;
            raw_svector_ostream OS(Str);

	    CurPackage = PT;
            if(PT == kPublic) {
                OS << "\n} // package end\n";
                OS << "package " << getPackageName(*CurModule) << " {\n";
                OS << "import " << getModulePackageName(*CurModule) << ".*\n";
                OS << "  import avm2.intrinsics.memory.*\n";
                OS << "import com.adobe.flascc.CModule\n";
            }
            else // kPrivate
            {
		assert(PT == kPrivate);
                OS << "\n} // package end\n";
                OS << "package " << getModulePackageName(*CurModule) << " {\n";
                OS << "import " << getPackageName(*CurModule) << ".*\n";
                OS << "  import avm2.intrinsics.memory.*\n";
                OS << "import com.adobe.flascc.CModule\n";
            }
            OutStreamer.EmitRawText(OS.str());
        }
    }

    void ChangePackage(const std::string &PN) const
    {
            SmallString<128> Str;
            raw_svector_ostream OS(Str);
            CurPackage = kCustom;
            OS << "\n} // package end\n";
            OS << "package " << PN << " {\n";
            OS << "import " << getPackageName(*CurModule) << ".*\n";
            OS << "import " << getModulePackageName(*CurModule) << ".*\n";
            OS << "  import avm2.intrinsics.memory.*\n";
            OS << "import com.adobe.flascc.CModule\n";
            OutStreamer.EmitRawText(OS.str());
    }

    virtual void EmitStartOfAsmFile(Module &M) {
	CurModule = &M;
        ((AVM2MCStreamer *)&OutStreamer)->setModulePackageName(getModulePackageName(M));
        ((AVM2MCStreamer *)&OutStreamer)->setEmitDebugLines(!TM.getSubtarget<AVM2Subtarget>().DisableDebugLines && TM.getSubtarget<AVM2Subtarget>().useInlineAsm());
        ((AVM2MCStreamer *)&OutStreamer)->setUseInlineAsm(TM.getSubtarget<AVM2Subtarget>().useInlineAsm());

        SmallString<128> Str;
        raw_svector_ostream OS(Str);

	   // weak emit registers into every module
        OS << "package " << getPackageName(M) << "{\n";

        if(InstrumentFlasccInitCode) OS << "    trace(\"packageInit(Code) Start: " << getModulePackageName(M) << "\");\n";
        // always import intrinsics (used w/ asc + concurrency or falcon for everything)
        OS << "  import avm2.intrinsics.memory.*\n";
	bool useInlineAsm = TM.getSubtarget<AVM2Subtarget>().useInlineAsm();
	const char *boilerPlate[] = {
"  import flash.utils.ByteArray;\n",
WeakMD, " public var ram:ByteArray = ram_init;\n",
WeakMD, " public var ESP:int = ESP_init;\n",
WeakMD, " public var eax:int;\n",
WeakMD, " public var edx:int;\n",
WeakMD, " public var st0:Number;\n",
#if CMP_HELPERS
WeakMD, "  public function ord(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a==a) && (b==b);\n",
"  }\n",

WeakMD, "  public function uno(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unoeq(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a==b) || (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unogt(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a>b) || (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unoge(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a>=b) || (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unolt(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a<b) || (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unole(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a<=b) || (a!=a) || (b!=b);\n",
"  }\n",

WeakMD, "  public function unone(a:Number, b:Number):Boolean\n",
"  {\n",
"    return (a!=b) || (a!=a) || (b!=b);\n",
"  }\n",
#endif

GlobalMethodMD, WeakMD, "[Csym(\"W\", \"abort\")]\n",
"public function Fabort():void { F_abort(); }\n",

GlobalMethodMD, WeakMD, "[Csym(\"W\", \"memcpy\")]\n",
"public function Fmemcpy():void { F_memcpy(); }\n",

GlobalMethodMD, WeakMD, "[Csym(\"W\", \"memmove\")]\n",
"public function Fmemmove():void { F_memmove(); }\n",

GlobalMethodMD, WeakMD, "[Csym(\"W\", \"memset\")]\n",
"public function Fmemset():void {\n",
"  var esp:int = ESP;\n",
  useInlineAsm ?
  "  var ptr:int = __xasm<int>(push(esp), op(0x37));\n" :
  "  var ptr:int = li32(esp);\n",
  useInlineAsm ?
  "  var c:int = __xasm<int>(push(esp+4), op(0x37));\n" :
  "  var c:int = li32(esp+4);\n",
  useInlineAsm ?
  "  var size:int = __xasm<int>(push(esp+8), op(0x37));\n" :
  "  var size:int = li32(esp+8);\n",
"  if(!c) {\n",
"    while(size >= 8) {\n",
  useInlineAsm ?
  "      __asm(push(0), push(ptr), op(0x3e));\n" :
  "      sf64(0, ptr);\n",
"      ptr += 8;\n",
"      size -= 8;\n",
"    }\n",
"  }\n",
"  while(size) {\n",
  useInlineAsm ?
  "    __asm(push(c), push(ptr), op(0x3a));\n" :
  "    si8(c, ptr);\n",
"    ptr++;\n",
"    size--;\n",
"  }\n",
"}\n",
		NULL
	};
        for(int bpi = 0; boilerPlate[bpi]; bpi++)
          OS << boilerPlate[bpi];
        if(InstrumentFlasccInitCode) OS << "    trace(\"packageInit(Setup) End: " << getModulePackageName(M) << "\");\n";
        OS << "} // end package\n";

        OS << "package " << getModulePackageName(M) << " {\n";
        OS << "import " << getPackageName(M) << ".*\n";
        OS << "import avm2.intrinsics.memory.*\n";
        OS << "import com.adobe.flascc.CModule\n";
        OutStreamer.EmitRawText(OS.str());

        // find functions with interesting annotations
        GlobalVariable *Annots = M.getGlobalVariable("llvm.global.annotations");
        ConstantArray *Arr = Annots ? dyn_cast_or_null<ConstantArray>(Annots->getInitializer()) : NULL;

        if(Arr) {
            unsigned Len = Arr->getNumOperands();

            for(unsigned I = 0; I < Len; I++) {
                ConstantStruct *Entry = dyn_cast_or_null<ConstantStruct>(Arr->getOperand(I));

                if(Entry && Entry->getNumOperands() >= 2) {
                    Function *Fun = dyn_cast_or_null<Function>(Entry->getOperand(0)->stripPointerCasts());
                    GlobalVariable *StrGV = dyn_cast_or_null<GlobalVariable>(Entry->getOperand(1)->stripPointerCasts());
                    ConstantDataSequential *StrV = StrGV ? dyn_cast_or_null<ConstantDataSequential>(StrGV->getInitializer()) : NULL;

                    if(Fun && StrV && StrV->isCString()) {
                        std::string Str = StrV->getAsString();

                        // record all explicit imports for functions
                        if(StringRef(Str).startswith("as3import:")) {
                            Fun2Import.insert(std::make_pair(Fun, Str.substr(10).c_str()));
                        }
                        // record all explicit signatures for functions
                        if(StringRef(Str).startswith("as3sig:")) {
                            Fun2Sig.insert(std::make_pair(Fun, Str.substr(7).c_str()));
                        }
                        // record all explicit packages for functions
                        if(StringRef(Str).startswith("as3package:")) {
                            Fun2Package.insert(std::make_pair(Fun, Str.substr(11).c_str()));
                        }
			// record explicit metadata!
                        if(StringRef(Str).startswith("as3md:")) {
                            Fun2MD.insert(std::make_pair(Fun, Str.substr(6).c_str()));
                        }
                    }
                }
            }
        }
	CurPackage = kPrivate;
    }

    virtual void EmitEndOfAsmFile(Module &M) {
        ((AVM2MCStreamer *)&OutStreamer)->setEmitFixupCalls(MMI->hasDebugInfo());

        UseSparseFun2Ptr = MMI->hasDebugInfo(); // use sparse function ptrs in debug

        ChangePackage(kPrivate);

        SmallString<128> Str;
        raw_svector_ostream OS(Str);

        OS << "// sections\n";
        {
            SectSet::const_iterator SI = Sections.begin(), SE = Sections.end();

            for(; SI != SE; SI++) {
		// TODO necessary?
		if((*SI)->getKind().isText() && *SI != getObjFileLowering().getTextSection())
			continue;
                OS << "public const " << getMangSectionName(**SI) << ":int = modSects[\"" << getSectionName(**SI) << "\"][0]\n";
            }
        }

	// map of synthetic symbols
	// copy the Sym2Num table and remove from it everything that's
	// actually in the module
	Sym2NumMap SynthSym2Num = Sym2Num;

        Module::const_global_iterator GI = M.global_begin(), GE = M.global_end();
        OS << "// private data\n";
        for(; GI != GE; GI++)
	{
            MCSymbol *Sym = Mang->getSymbol(GI);

	    SynthSym2Num.erase(Sym); // remove global vars

            if(!GI->isDeclaration()) {
                if(isPublic(GI)) {
                    continue;
                }

                Sym2NumMap::const_iterator SI = Sym2Num.find(Sym);
                if(SI == Sym2Num.end()) {
                    continue;
                }

                OS << ";";
                emitCsymMD(OS, false, StringRef("d"), Sym->getName(), getSectionName(SI->first->getSection()));

                OS << "public const " << *Sym << ":int = " << getMangSectionName(SI->first->getSection()) <<  " + " << SI->second << "\n";
                removeTE(*Sym);
                PrivateSyms.insert(Sym);
            }
	}

        OS << "// private functions\n";
        {
            Fun2NumMap::const_iterator FVI = Fun2Num.begin(), FVE = Fun2Num.end();
            for(; FVI != FVE; FVI++) {
                const MCSymbol *Sym = Mang->getSymbol(FVI->first);

                SynthSym2Num.erase(Sym); // remove functions

                const Function *Fun = dyn_cast_or_null<const Function>(FVI->first);

                if(Fun && !isPublic(Fun)) {
                    // we explicitly specify the "offset" (LineNo) for functions as it differs from the value that the constant will have... the constant is an index into ptr2fun
                    OS << ";";
                    emitCsymMD(OS, true, StringRef("t"), Sym->getName(), getSectionName(Sym->getSection()), Fun2EntryLabelLineNo[Fun]);
                    OS << "public const " << *Sym << ":int = " << getModulePackageName(M) << ".modFunStart + " << getFunctionAddr(Fun) << "\n";
                    PrivateSyms.insert(Sym);
                }

            }
        }

	OS << "// synthetic definitions\n";
        {
            Sym2NumMap::const_iterator SI = SynthSym2Num.begin(), SE = SynthSym2Num.end();
            for(; SI != SE; SI++) {
		const MCSymbol *Sym = SI->first;
		bool IsText = Sym->getSection().getKind().isText();
                OS << ";";
                emitCsymMD(OS, false, StringRef((IsText ? "t" : "d")), Sym->getName(), getSectionName(Sym->getSection()));
                OS << "public const " << *(SI->first) << ":int = " << getMangSectionName(SI->first->getSection()) <<  " + " << SI->second << "\n";
                PrivateSyms.insert(SI->first);
                removeTE(*SI->first);
            }
        }

        OS << ";\n"; // make metadata parsing happy
        OS << "// data imports\n";
        GI = M.global_begin();
        for(; GI != GE; GI++)
            if(GI->isDeclaration() && GI->hasNUsesOrMore(1)) {
                MCSymbol &Sym = *(Mang->getSymbol(GI));
                if(!isImplicitlyDefined(Sym)) {
                    emitCsymMD(OS, false, StringRef("U"), Sym.getName());
                }
                removeTE(Sym);
            }
        OS << "// function imports\n";
        Module::const_iterator FI = M.begin(), FE = M.end();
        for(; FI != FE; FI++)
            if(FI->isDeclaration() && FI->hasNUsesOrMore(1)) {
                MCSymbol &Sym = *(Mang->getSymbol(FI));
                if(!FI->getName().startswith("llvm.") && !isImplicitlyDefined(Sym)) {
                    emitCsymMD(OS, false, StringRef("U"), Sym.getName());
                }
                removeTE(Sym);
            }
        OS << "// external imports\n";
        TEMap::const_iterator TEI = TE.begin(), TEE = TE.end();
        for(; TEI != TEE; TEI++) {
            const char *Sym = TEI->c_str();
            if(!isImplicitlyDefined(Sym)) {
                emitCsymMD(OS, false, StringRef("U"), StringRef(Sym));
            }
        }
        
        // NOTE! all of the metadata generated above attaches to modWeaks!
        // it's all undefined stuff, so it doesn't matter what it's attached to,
        // but it needs to attach to something

        OS << "// weak reference resolution magic\n";
        {
            OS << LinkABCMD << " public var modWeaks:Object\n";
            OS << "public function resolveWeaks():Boolean\n";
            OS << "{\n";
            OS << "\tvar weaks:Object = {\n";
            OS << "\t\t1:1\n";
            SymRefMap::const_iterator WI = WeakRefs.begin(), WE = WeakRefs.end();
            for(; WI != WE; WI++)
            {
                const MCSymbol *Alias = WI->first;
                const MCSymbol *Target = WI->second;
                OS << "\t\t," << *Alias << " : " << *Target << "\n";
                if((Target->isInSection() && Target->getSection().getKind().isText()) ||
                	(Alias->isInSection() && Alias->getSection().getKind().isText()))
                {
                    OS << "\t\t,F" << *Alias << " : F" << *Target << "\n";
                }
            }
            OS << "\t}\n";
            OS << "\tfor each(var val in weaks) if(!val) { return false }\n";
            OS << "\tmodWeaks = weaks\n";
            OS << "\treturn true\n";
            OS << "}\n";
        }
        
        OS << ";" << INIT_MD << "public function modImplInit():void { /*dummy to cause script to run*/ }\n";
        OS << "} // end package\n";
        // back to global
        OS << "package " << getPackageName(M) << " {\n";
        OS << "import " << getModulePackageName(M) << ".*\n";
        OS << "  import avm2.intrinsics.memory.*\n";
        OS << "import com.adobe.flascc.CModule\n";
        OS << "// ptr2fun decl\n";
        if(UseSparseFun2Ptr)
          OS << WeakMD << " public const ptr2fun:* = ptr2funInit();\n";
        else
          OS << WeakMD << " public const ptr2fun:Vector.<Function> = ptr2funInit();\n";
        OS << "// public data\n";
        GI = M.global_begin();
        for(; GI != GE; GI++) {
            if(!GI->isDeclaration()) {
                if(!isPublic(GI)) {
                    continue;
                }

                MCSymbol *Sym = Mang->getSymbol(GI);
                Sym2NumMap::const_iterator SI = Sym2Num.find(Sym);
                if(SI == Sym2Num.end()) {
                    continue;
                }
                bool IsCommon = GI->hasCommonLinkage();
                bool IsWeak = isWeak(GI);

                OS << ";";
                emitCsymMD(OS, false, StringRef(IsCommon ? "C" : IsWeak ? "W" : "D"), Sym->getName(), getSectionName(Sym->getSection()));
                if(IsCommon || IsWeak)
                    OS << WeakMD << "\n";
                OS << "public const " << *Sym << ":int = " << getMangSectionName(SI->first->getSection()) <<  " + " << SI->second << "\n";
                removeTE(*Sym);
            }
        }
        OS << "// public functions\n";
        {
            Fun2NumMap::const_iterator FVI = Fun2Num.begin(), FVE = Fun2Num.end();
            for(; FVI != FVE; FVI++)
                if(isPublic(FVI->first)) {
                    const Function *Fun = FVI->first;

                    MCSymbol *Sym = Mang->getSymbol(Fun);
                    bool IsCommon = Fun->hasCommonLinkage();
                    bool IsWeak = isWeak(Fun);

                    // semicolon makes ASC happy
                    OS << ";";
                     emitCsymMD(OS, true, StringRef(IsCommon ? "C" : IsWeak ? "W" : "T"), Sym->getName(), getSectionName(CurrentFnSym->getSection()), Fun2EntryLabelLineNo[Fun]);
                    if(IsCommon || IsWeak)
                        OS << WeakMD << "\n";
                    OS << "public const " << *Sym << ":int = ";
                    OS << getModulePackageName(M) << ".modFunStart + " << getFunctionAddr(Fun);
                    OS << "\n";
                }
        }
        OS << "com.adobe.flascc.CModule.resolveWeaks(" << getModulePackageName(M) << ".resolveWeaks)\n";
        OS << "// weak references\n";
	{
    		SymRefMap::const_iterator WI = WeakRefs.begin(), WE = WeakRefs.end();
		for(; WI != WE; WI++)
		{
			const MCSymbol *Alias = WI->first;
			const MCSymbol *Target = WI->second;
			OS << ";";
			if(Target->isInSection())
				emitCsymMD(OS, false, StringRef("W"), Alias->getName(), getSectionName(Target->getSection()));
			else if(Alias->isInSection())
				emitCsymMD(OS, false, StringRef("W"), Alias->getName(), getSectionName(Alias->getSection()));
			else
				emitCsymMD(OS, false, StringRef("W"), Alias->getName());
			OS << WeakMD << "\n";
			OS << "public const " << *Alias << ":int = " << getModulePackageName(M) << ".modWeaks." << *Alias << "\n";
			// it's text, so a function... create a Function slot
			if(Target->isInSection() && Target->getSection().getKind().isText() ||
				Alias->isInSection() && Alias->getSection().getKind().isText())
			{
				OS << ";" << WeakMD << "\n";
				OS << "public const F" << *Alias << ":Function = " << getModulePackageName(M) << ".modWeaks.F" << *Alias << "\n";
			}
		}
	}
        OS << "} // package end\n";
        OS << "package " << getModulePackageName(M) << " {\n";
        OS << "import " << getPackageName(M) << ".*\n";
        OS << "import avm2.intrinsics.memory.*\n";
        OS << "import com.adobe.flascc.CModule\n";
        OS << "com.adobe.flascc.CModule.sendMetric(\"flascc.ns\", (function():String { namespace ns = \"" << getModulePackageName(M) << "\"; return ns; })());\n";
        OS << "com.adobe.flascc.CModule.sendMetric(\"flascc.ns\", (function():String { namespace ns = \"" << getPackageName(M) << "\"; return ns; })());\n";
        OS << "// static data init\n";
        OS << "\tmodSelf.once(modDataInit_" << getModulePackageName(M) << ", " << getModulePackageName(M) << ".modSects)\n";  // AVM2MCAsmStreamer creates modDataInit on Finish
        OS << "// function table init\n";
        {
            Fun2NumMap::const_iterator FVI = Fun2Num.begin(), FVE = Fun2Num.end();
            for(; FVI != FVE; FVI++) {
                const Function *Fun = dyn_cast<const Function>(FVI->first);

                if(Fun && Fun2Sig.find(Fun) == Fun2Sig.end()) { // skip funs w/ explicit sigs
                    OS << "com.adobe.flascc.CModule.regFun(" << *(Mang->getSymbol(FVI->first)) << ", ";
                    OS << (isPublic(Fun) ? getPackageName(M) : getModulePackageName(M)) << ".F" << *(Mang->getSymbol(FVI->first)) << ")\n";
                }
            }
        }

        int weakSymFuncCnt = 0;
        if(GenDbgSymTable) {
            const int MAX_WEAK_SYMS_PER_FUNC = 50;
            std::vector<CSymEntry> weaks;
            std::vector<CSymEntry>::const_iterator CSI = CSymData.begin(), CSE = CSymData.end();
            for (; CSI != CSE; CSI++) {
                if (CSI->type == "W" || CSI->type == "w") {
                    weaks.push_back(*CSI);
                }
            }

            std::vector<CSymEntry>::const_iterator WSI = weaks.begin(), WSE = weaks.end();
            int symsInThisFunc = 0;
            for(; WSI != WSE; WSI++) {
                if (!symsInThisFunc) {
                    OS << ";" << INIT_MD << "function addWeakSymbols_" << weakSymFuncCnt++ << "():void {\n";
                    OS << "\tnamespace C_RunNS = \"" << getPackageName(*CurModule) << "\";\n";
                }

                OS << "\ttry { ";
                // cause a ReferenceError if this weak symbol was defined in another module
                OS << "this.C_RunNS::[\"" << WSI->name << "\"]; ";
                OS << "modSyms.push([\"" << WSI->type << "\", \"" << WSI->name << "\"";
                if (WSI->num >= 3)
                    OS << ", \"" << WSI->section << "\"";
                if (WSI->num >= 4) 
                    OS << ", \"" << WSI->value << "\"";
                OS << "]); ";
                OS << "} catch(e:*) {}\n";

                symsInThisFunc++;

                if(symsInThisFunc == MAX_WEAK_SYMS_PER_FUNC || (WSI+1) == WSE) {
                    if ((WSI+1) != WSE) {
                        OS << "addWeakSymbols_" << weakSymFuncCnt << "();\n";
                    }
                    OS << "}\n";
                    symsInThisFunc = 0;
                }
            }
        }

        OS << ";" << INIT_MD << "function notifyInitHelper():void {\n";
        if(!TM.getSubtarget<AVM2Subtarget>().DisableDebugLines && TM.getSubtarget<AVM2Subtarget>().useInlineAsm()) {
            OS << "__asm(debugfile, str(\"" << getModulePackageName(M) << "\"))\n";
            OS << "__asm(debugline, u30(1))\n";
        }

        if(GenDbgSymTable) {
            OS << "// Weak Symbol debug info\n";
            if(weakSymFuncCnt) {
                OS << "addWeakSymbols_0()\n";
            }
        }

        OS << "modSelf.notifyInit(this)\n";
        OS << "}\n";
        OS << "notifyInitHelper()\n";
        OS << "} // package end\n";
        OutStreamer.EmitRawText(OS.str());
        Str.clear();

        // split into next file!
        if(!((AVM2MCStreamer*)&OutStreamer)->setUseSecondStream()) {
            OS << "\n\n#---SPLIT\n\n\n";
        }

        // some module specific stuff
        OS << "package " << getModulePackageName(M) << " {\n";
        OS << "import " << getPackageName(M) << ".*\n";
        if(InstrumentFlasccInitCode) OS << "    trace(\"packageInit(Setup) Start: " << getModulePackageName(M) << "\");\n";
        OS << "import avm2.intrinsics.memory.*\n";
        OS << "import com.adobe.flascc.CModule\n";
        OS << "import com.adobe.flascc.BinaryData\n";
        OS << "public const modPkgName:String = (function():String { namespace ns = \"" << getModulePackageName(M) << "\"; return ns; })();\n";
        OS << "public const modSects:Object = modAllocSects()\n";
        if(UseSparseFun2Ptr)
          OS << "public const modFunStart:int = modSects[\"" << getSectionName(*(getObjFileLowering().getTextSection())) << "\"][0]\n";
        else
          OS << "public const modFunStart:int = com.adobe.flascc.CModule.allocFunPtrs(modPkgName, " << (FunCount * getFunAlignment()) << ", " << getFunAlignment() << ")\n";
        
        if(GenDbgSymTable) {
            OS << "// debug symbol table\n";
            OS << "public var modSyms:Array = [\n";
            for(unsigned int i=0; i<CSymData.size(); i++) {
                if(CSymData[i].type == "W" || CSymData[i].type == "w")
                    continue;

                OS << "[\"" << CSymData[i].type << "\", \"" << CSymData[i].name << "\"";
                if (CSymData[i].num >= 3)
                    OS << ", \"" << CSymData[i].section << "\"";
                if (CSymData[i].num >= 4) 
                    OS << ", \"" << CSymData[i].value << "\"";
                OS << "],\n";
            }
            OS << "];\n";
        }
        
        OS << "// module reg\n";
        OS << "public const modSelf:com.adobe.flascc.CModule = com.adobe.flascc.CModule.regModule(modSects, function():void { modImplInit(); }, " << (GenDbgSymTable ? "modSyms" : "null") << ", modPkgName)\n";
        OutStreamer.EmitRawText(OS.str());
        CurModule = NULL;
    }

    std::map<const Function *, unsigned> Fun2EntryLabelLineNo;

    virtual void EmitFunctionEntryLabel()
    {
      AsmPrinter::EmitFunctionEntryLabel();
      // record line no for Csym metadata
      Fun2EntryLabelLineNo[MF->getFunction()] = LastLabelLineNo;
    }

    virtual void EmitFunctionBodyStart() {
        assert(MF->getAlignment() <= getFunAlignment());

        const Function *Fun = MF->getFunction();
        const Module &M = *Fun->getParent();

	SmallString<128> Str;
        raw_svector_ostream OS(Str);

        FunCallsSetJmp = MF->exposesReturnsTwice();

        // ugh... update "callSetJmp" if we used an sjlj setjmp
        MachineFunction::const_iterator I = MF->begin(), E = MF->end();

        for(; !FunCallsSetJmp && I != E; I++) {
            MachineBasicBlock::const_iterator BI = I->begin(), BE = I->end();

            for(; BI != BE; BI++)
                if(BI->getOpcode() == AVM2::eh_sjlj_setjmp) {
                    FunCallsSetJmp = true;
                    break;
                }
        }

        FunRCVRegCount.clear();
        FunVReg2RCRelativeIndex.clear();

        // select correct package
	{
            Fun2StrMap::const_iterator I = Fun2Package.find(Fun);
	    // TODO further consolidate package setting logic...
	    if(I != Fun2Package.end())
              ChangePackage(I->second);
            else
              ChangePackage(isPublic(Fun) ? kPublic : kPrivate);
	}

        OS << ";";

        // emit any required imports
	{
            Fun2StrMultiMap::const_iterator I = Fun2Import.find(Fun), E = Fun2Import.end();
	    for(; I != E && I->first == Fun; I++)
              OS << "import " << I->second << ";\n";
	}

	// emit weakness metadata
	if(isWeak(Fun))
		OS << WeakMD << "\n";
	// emit any custom metadata
	{
	    Fun2StrMultiMap::const_iterator I = Fun2MD.find(Fun), E = Fun2MD.end();

	    for(; I != E && I->first == Fun; I++)
		OS << "\n[" << I->second << "]\n";
	}
	// emit function signature
        {
            Fun2StrMap::const_iterator I = Fun2Sig.find(Fun);
            if(I != Fun2Sig.end()) {
                FunHasAS3Sig = true;
                OS << I->second;
            } else {
                FunHasAS3Sig = false;
		// mark all regular functions as "global methods"...
		// swfresolve ensures that global methods can statically
		// bind to symbols in their global/script object
		// TODO why doesn't GlobalMethodMD apply to funs w/ explicit signatures?
		OS << GlobalMethodMD << "\n";
                OS << "public function F" << *CurrentFnSym << "():void";
            }
        }

	// work around -ffuction-sections
	CurrentFnSym->setSection(*(getObjFileLowering().getTextSection())); 

        OS << " {\n";

        if(FunHasAS3Sig) // set up for return value
          OS << "    var _as3ReturnValue:*;\n";

        // Changed 27.04.2014.: was FunIsNaked = Fun->getFnAttributes().hasAttribute(Attributes::Naked);
        FunIsNaked = Fun->getAttributes().hasAttribute(AttributeSet::FunctionIndex, Attribute::Naked);

        if(!TM.getSubtarget<AVM2Subtarget>().DisableDebugLines && TM.getSubtarget<AVM2Subtarget>().useInlineAsm()) {
	  OS << "__asm(debugfile, str(\"" << getModulePackageName(M) << "\"))\n";
      OutStreamer.EmitRawText(OS.str());
	  Str.clear();
	  OS.resync();
	  // LastLabelLineNo will get set as a side-effect, but no debugline will be emitted as
	  // we haven't called EmitFnStart yet
	  OutStreamer.EmitLabel(GetTempSymbol(std::string("alcstart_"+CurrentFnSym->getName().str())));
	  OS << "__asm(debugline, u30(" << LastLabelLineNo << "))\n";
	  // local var info for ebp, esp
	  if(!FunIsNaked)
	  {
	    OS << "__asm(debug, byte(1), str(\"esp\"), byte(0), u30(0))\n";
	    OS << "__asm(debug, byte(1), str(\"ebp\"), byte(1), u30(0))\n";
	  }
	}


#if 0
        OS << "trace(\"" << *CurrentFnSym << ": \" + ESP/16)\n";
#endif
#if 0
        OS << "trace(\"\t\" + [0,4,8,12].map(function():*{return CModule.read32(ESP+arguments[0]);}))\n";
#endif

        if(!FunIsNaked) { // don't apply to naked functions
            OS << "\tvar esp:int = ESP\n";
            OS << "\tvar ebp:int\n";
        }
        OutStreamer.EmitRawText(OS.str());
        Fun2Num.insert(std::make_pair(Fun, FunCount++));
        if(TM.getSubtarget<AVM2Subtarget>().UseActivations) {
            OutStreamer.EmitRawText(StringRef("\t(function():void {\n"));
        }

        if(FunCallsSetJmp) {
            FunSJID = 1;
            SmallString<128> Str;
            raw_svector_ostream OS(Str);
            OS << "\tvar tsjid:int //0\n";
            OS << "\tfor(;;) {\n";
            OS << "\ttry {\n";
	    if(TM.getSubtarget<AVM2Subtarget>().useInlineAsm())
            	OS << "\t__asm(push(tsjid), iftrue, target(\".sj\"))\n";
	    else
            	OS << "\tif(tsjid) goto sj__\n";
            OutStreamer.EmitRawText(OS.str());
        }
        OutStreamer.EmitFnStart();
    }

    virtual void EmitFunctionBodyEnd() {
        OutStreamer.EmitFnEnd();
        if(FunCallsSetJmp) {
            SmallString<128> Str;
            raw_svector_ostream OS(Str);

            assert(FunSJID > 1);
	    if(TM.getSubtarget<AVM2Subtarget>().useInlineAsm())
	    {
            	OS << "\t__asm(lbl(\".sj\"))\n";
            	OS << "\t__asm(push(tsjid), switchjump(\n";
            	OS << "\t\".SJ_err\"\n";
            	OS << "\t,\".SJ_err\"\n";

            	for(unsigned n = 1; n < FunSJID; n++) {
                	OS << "\t,\".SJ_" << n << "\"\n";
            	}
            	OS << "\t))\n";
            	OS << "\t__asm(lbl(\".SJ_err\"))\n";
	    }
	    else
	    {
		OS << "\tsj__:\n";
		OS << "\tswitch(tsjid) {\n";
		OS << "\tdefault: goto SJ_err__\n";

            	for(unsigned n = 1; n < FunSJID; n++) {
                	OS << "\tcase " << n << ": goto SJ_" << n << "__\n";
            	}
		OS << "\t}\n";
		OS << "\tSJ_err__:\n";
	    }
            OS << "\tthrow \"bad longjmp\"\n";
            OS << "\t} catch(l:LongJmp) {\n";
            OS << "\tif(l.esp >= ebp) throw l\n";
            OS << "\tESP = esp = l.esp\n";
            OS << "\teax = l.retval\n";
            OS << "\ttsjid = l.sjid\n";
            OS << "\t}\n"; // catch
            OS << "\t}\n"; // for(;;)
            OutStreamer.EmitRawText(OS.str());
        }
        if(TM.getSubtarget<AVM2Subtarget>().UseActivations) {
            OutStreamer.EmitRawText(StringRef("\t})()\n"));
        }

        VReg2IndexMap::const_iterator I = FunVReg2RCRelativeIndex.begin(),
                                      E = FunVReg2RCRelativeIndex.end();
        const MachineRegisterInfo *RI = &(MF->getRegInfo());

        for(; I != E; I++) {
            const TargetRegisterClass *RC = AVM2RegisterInfo::getRegClass(RI, I->first);
            SmallString<128> Str;
            raw_svector_ostream OS(Str);
            OS << "\tvar ";
            printRegister(RI, I->first, OS);
            if(RC == &AVM2::IntRegsRegClass) {
                OS << ":int\n";
            } else if(RC == &AVM2::FPRegsRegClass || RC == &AVM2::SPRegsRegClass) {
                OS << ":Number\n";
            } else {
                assert(!"couldn't find reg class!");
            }
            OutStreamer.EmitRawText(OS.str());
        }
        StringRef Str = "} // function end\n";
        OutStreamer.EmitRawText(Str);
    }
};

} // end of anonymous namespace

#include "AVM2GenAsmWriter.inc"

// if the reg is always defined to be an immediate, return that MachineOperand
const MachineInstr *AVM2AsmPrinter::regIsTrivial(const MachineRegisterInfo *RI, unsigned Reg)
{
    const TargetRegisterClass *RC = AVM2RegisterInfo::getRegClass(RI, Reg);

    if(RC->contains(Reg)) {
        return NULL;    // never for phys regs
    }

    MachineRegisterInfo::def_iterator I = RI->def_begin(Reg), E = RI->def_end();

    if(I != E) { // at least one def
        const MachineOperand *Op = &(I.getOperand());
        I++;
        if(I == E) { // exactly one def
            int Opcode = Op->getParent()->getOpcode();

            // TODO are these safe? could in theory adjust ebp or whatever after...
            if(Opcode == AVM2::LDmri || Opcode == AVM2::LDmrr) {
                return NULL;
            }

            if(opcodeIsTrivial(Opcode)) {
                return Op->getParent();
            }
        }
    }
    return NULL;
}

void AVM2AsmPrinter::printRegister(const MachineInstr *MI, unsigned Reg, raw_ostream &O)
{
    const MachineRegisterInfo *RI = &(MI->getParent()->getParent()->getRegInfo());
    const MachineInstr *MIC = regIsTrivial(RI, Reg);

    if(MIC) {
        printInstruction(MIC, O);
    } else {
        printRegister(RI, Reg, O);
    }
}

void AVM2AsmPrinter::printRegister(const MachineRegisterInfo *RI, unsigned Reg, raw_ostream &O)
{
    const TargetRegisterClass *RC = AVM2RegisterInfo::getRegClass(RI, Reg);

    if(RC->contains(Reg)) {
        O << StringRef(getRegisterName(Reg)).lower();
    } else {
        if(RC == &AVM2::IntRegsRegClass) {
            O << "i";
        } else if(RC == &AVM2::FPRegsRegClass) {
            O << "f";
        } else if(RC == &AVM2::SPRegsRegClass) {
            O << "s";
        } else {
            assert(!"couldn't find reg class!");
        }


        VReg2IndexMap::const_iterator I = FunVReg2RCRelativeIndex.find(Reg);
        unsigned Index;

        if(I == FunVReg2RCRelativeIndex.end()) {
            Index = FunRCVRegCount[RC]++;
            FunVReg2RCRelativeIndex.insert(std::make_pair(Reg, Index));
        } else {
            Index = I->second;
        }

        O << Index;
    }
}

void AVM2AsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                  raw_ostream &O)
{
    const MachineOperand &MO = MI->getOperand (opNum);

    printOperand(MI, MO, O);
}

void AVM2AsmPrinter::printOperand(const MachineInstr *MI, const MachineOperand &MO, raw_ostream &O)
{
    switch (MO.getType()) {
    case MachineOperand::MO_Register:
        printRegister(MI, MO.getReg(), O);
        break;

    case MachineOperand::MO_Immediate:
        O << (int)MO.getImm();
        break;
    case MachineOperand::MO_MachineBasicBlock:
	if(TM.getSubtarget<AVM2Subtarget>().useInlineAsm())
            O << "target(\"" << *MO.getMBB()->getSymbol() << "\")";
        else
            O << *MO.getMBB()->getSymbol();
        LabelsSeen.insert(MO.getMBB()->getSymbol());
        return;
    case MachineOperand::MO_GlobalAddress:
      {
        const GlobalValue *GV = MO.getGlobal();
        if(MI->getDesc().isCall())
          O << "F"; // function prefix
        O << *Mang->getSymbol(GV);
        break;
      }
    case MachineOperand::MO_ExternalSymbol: {
      const char *Sym = MO.getSymbolName();
      //printf("\t%s:", Sym);
      if(MI->getDesc().isCall())
        O << "F"; // function prefix
      if(Sym[0] == 'L') {
        // Internal global.
        O << Sym;
        TE.insert(Sym);
      } else {
        SmallString<256> MN;
        Twine TMN;
        if(Sym[0] == '\2') {
          TMN = (Sym + 1);
        } else {
          TMN = Sym;
        }
        Mang->getNameWithPrefix(MN, TMN);
        O << MN;
        TE.insert(MN.str());
        //printf("%s", MN.str().data());
      }
    }
    break;
    case MachineOperand::MO_BlockAddress: {
        int blockaddr = 0;
        Function *f =  MO.getBlockAddress()->getFunction();

        for(Function::iterator blocks =f->begin(); blocks++; blocks != f->end()) {
            if(&(*blocks) == MO.getBlockAddress()->getBasicBlock())
                break;
            blockaddr++;
        }
        O << blockaddr;
    }
    break;
    case MachineOperand::MO_ConstantPoolIndex:
        O << MAI->getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << "_"
          << MO.getIndex();
        break;
    default:
        assert(!"<unknown operand type>");
    }
}

void AVM2AsmPrinter::printMemOperand(const MachineInstr *MI, int opNum,
                                     raw_ostream &O, const char *Modifier)
{
    printOperand(MI, opNum, O);
    const MachineOperand &Op1 = MI->getOperand(opNum+1);

    if (Op1.isImm())
    {
      int Imm = (int)Op1.getImm();
      if(Imm == 0)
        return;
      if(Imm < 0)
        O << "-" << -Imm;
      else
        O << "+" << Imm;
      return;
    }

    O << "+";
    printOperand(MI, opNum+1, O);
}

/// PrintAsmOperand - Print out an operand for an inline asm expression.
///
bool AVM2AsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                     unsigned AsmVariant,
                                     const char *ExtraCode,
                                     raw_ostream &O)
{
    if (ExtraCode && ExtraCode[0]) {
        if (ExtraCode[1] != 0) {
            return true;    // Unknown modifier.
        }

        switch (ExtraCode[0]) {
        default:
            return true;  // Unknown modifier.
        case 'r':
            break;
        }
    }

    printOperand(MI, OpNo, O);

    return false;
}

bool AVM2AsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
        unsigned OpNo, unsigned AsmVariant,
        const char *ExtraCode,
        raw_ostream &O)
{
    assert(false); // TODO?
    if (ExtraCode && ExtraCode[0]) {
        return true;    // Unknown modifier
    }

    O << '[';
    printMemOperand(MI, OpNo, O);
    O << ']';

    return false;
}

/// isBlockOnlyReachableByFallthough - Return true if the basic block has
/// exactly one predecessor and the control transfer mechanism between
/// the predecessor and this block is a fall-through.
///
/// This overrides AsmPrinter's implementation to handle delay slots.
bool AVM2AsmPrinter::
isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) const
{
    // If this is a landing pad, it isn't a fall through.  If it has no preds,
    // then nothing falls through to it.
    if (MBB->isLandingPad() || MBB->pred_empty()) {
        return false;
    }

    // If there isn't exactly one predecessor, it can't be a fall through.
    MachineBasicBlock::const_pred_iterator PI = MBB->pred_begin(), PI2 = PI;
    ++PI2;
    if (PI2 != MBB->pred_end()) {
        return false;
    }

    // The predecessor has to be immediately before this block.
    const MachineBasicBlock *Pred = *PI;

    if (!Pred->isLayoutSuccessor(MBB)) {
        return false;
    }

    // Check if the last terminator is an unconditional branch.
    MachineBasicBlock::const_iterator I = Pred->end();
    while (I != Pred->begin() && !(--I)->getDesc().isTerminator()) {
        ;    // Noop
    }
    return I == Pred->end() || !I->getDesc().isBarrier();
}

// Force static initialization.
extern "C" void LLVMInitializeAVM2AsmPrinter()
{
    RegisterAsmPrinter<AVM2AsmPrinter> X(TheAVM2Target);
}

#if !defined(__CYGWIN__) && !defined(__linux__)
extern "C" {
#ifdef MULTIPLE_THREADS
    #undef MULTIPLE_THREADS
#endif
#include "bsd-dtoa/gdtoa.c"
#include "bsd-dtoa/g_ddfmt.c"
#include "bsd-dtoa/g_dfmt.c"
#include "bsd-dtoa/g__fmt.c"
#include "bsd-dtoa/misc.c"
#include "bsd-dtoa/dmisc.c"
}
#endif
