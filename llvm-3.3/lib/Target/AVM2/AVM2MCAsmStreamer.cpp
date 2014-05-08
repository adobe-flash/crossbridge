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
//===- lib/Target/AVM2/AVM2MCAsmStreamer.cpp - AVM2 Text Assembly Output -----===//

static const char AdobeInternalCode[] __attribute__((used)) = "This File contains Adobe internal code.";

#include "AVM2.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Timer.h"

#include <vector>
#include <map>
#include <stdio.h>

#if !defined(__MINGW32__) 
#include <sys/wait.h>
#endif

#include "SetAlchemySDKLocation.c"

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#define PATH_MAX 260 /* from windef.h */
#define chdir _chdir
#define getcwd _getcwd
#endif

// AS3 metadata to mark a class hidden from FlexBuilder's tree view
static const char *ExcludeClassMD = "[ExcludeClass]";

using namespace llvm;

extern cl::opt<bool> InstrumentFlasccInitCode;

static cl::list<std::string>
ASCOpts("ascopt",
  cl::desc("Extra options to pass to ASC when compiling ActionScript"),
  cl::value_desc("asc options"), cl::ZeroOrMore);

static cl::list<std::string>
JVMOpts("jvmopt",
        cl::desc("Extra options to pass to Java when compiling ActionScript"),
        cl::value_desc("jvm options"), cl::ZeroOrMore);

// TODO MCANNIZZ
static cl::opt<std::string> JVMPath("jvm", cl::desc("Path to the java vm executable (java)"),
	cl::value_desc("jvm executable path"), 
	cl::ZeroOrMore);

static cl::opt<bool> TargetPlayer(
  "target-player",
  cl::init(false),
  cl::desc("Target Flash Player (choose playerglobal.abc instead of shell_toplevel.abc)"),
  cl::Hidden);

static cl::opt<bool> FalconParallel(
  "falcon-parallel",
  cl::init(false),
  cl::desc("Compile AS3 function multi-threaded with Falcon)"),
  cl::Hidden);

static cl::opt<std::string>
AS3TmpFile1("as3tmp1", cl::Hidden, cl::init(""));

static cl::opt<std::string>
AS3TmpFile2("as3tmp2", cl::Hidden, cl::init(""));

namespace
{

bool checkForJVMError(const std::string& str)
{
    return str.find("Error: Could not create the Java Virtual Machine") != std::string::npos ||
            str.find("Error occurred during initialization of VM") != std::string::npos;
}

std::string runcmd(std::string cmd, std::vector<std::string> &args)
{
    std::string cmdname = "Run Command " + cmd;
    NamedRegionTimer RunCmdTimer(cmdname.c_str(), "AVM2 Backend");

    std::string appPath = sys::Program::FindProgramByName(cmd).c_str();
    sys::Path *ioredirects[3] = {NULL, NULL, NULL};
    ioredirects[0] = new sys::Path();
    ioredirects[1] = new sys::Path(sys::Path::GetTemporaryDirectory().str() + "alctmp");
    ioredirects[2] = new sys::Path(sys::Path::GetTemporaryDirectory().str() + "alctmp");

    // redirect stdout/stderr somewhere
    ioredirects[1]->createTemporaryFileOnDisk();
    ioredirects[2]->createTemporaryFileOnDisk();

    char **argptrs = new char*[args.size()+2];
    argptrs[0] = (char*)appPath.c_str();
    argptrs[args.size()+1] = NULL;
    for(unsigned int i=0; i<args.size(); i++) {
        argptrs[1+i] = (char*)args[i].c_str();
    }
    std::string ErrMsg;
    int result = sys::Program::ExecuteAndWait(sys::Path(appPath), (const char**)argptrs, NULL, (const llvm::sys::Path**)&ioredirects[0], 0, 0, &ErrMsg);

    OwningPtr<MemoryBuffer> stdoutbuffer,stderrbuffer;
    MemoryBuffer::getFile(ioredirects[1]->c_str(), stdoutbuffer);
    MemoryBuffer::getFile(ioredirects[2]->c_str(), stderrbuffer);
    std::string stdoutstring = stdoutbuffer->getBuffer();
    std::string stderrstring = stderrbuffer->getBuffer();
    ioredirects[1]->eraseFromDisk();
    ioredirects[2]->eraseFromDisk();

    // Yep, ASC sometimes fails to compile code but returns a 0 exit code...
    if(stderrstring.find("Error") != std::string::npos || stdoutstring.find("Error") != std::string::npos)
        result = -1;

    if(result != 0) {
        if(checkForJVMError(stderrstring) || checkForJVMError(stdoutstring)) {
            report_fatal_error("Error: Unable to launch the Java Virtual Machine. \n"
                                "This usually means you have a 32bit JVM installed or have set your Java heap size too large.\n"
                                "Try lowering the Java heap size by passing \"-jvmopt=-Xmx1G\" to gcc/g++.");
        }

        std::string argstring = "";
        for(unsigned int i=0; i<args.size(); i++)
            argstring += args[i] + " ";

        report_fatal_error("Failed to run " + appPath + " with args: " + argstring + "\nError: " + ErrMsg + "\n" + stdoutstring + stderrstring);
    }
    
    size_t p = stdoutstring.find_last_not_of(" \t\f\v\n\r");
    if (p != std::string::npos)
        stdoutstring.erase(p+1);
    else
        stdoutstring.clear();
    
    return stdoutstring;
}

inline std::string nativepath(std::string str)
{
#if defined(__CYGWIN__) || defined(__MINGW32__)
    std::vector<std::string> args;
    args.push_back("-at");
    args.push_back("mixed");
    args.push_back(str);
    return runcmd("cygpath", args);
#else
    return str;
#endif
}

class AVM2MCAsmStreamer : public AVM2MCStreamer
{
    // wrapper ostream that counts newlines
    // -- used to map labels to line numbers
    class line_counting_ostream : public raw_ostream
    {
        uint64_t CurrentPos;
        unsigned LineNo;
        raw_ostream &OS;
        void write_impl(const char *Ptr, size_t Size) {
            const char *NL = Ptr;

            while((NL = (const char *)memchr(NL, '\n', Size - (NL-Ptr)))) {
                LineNo++;
                NL++;
            }
            OS.write(Ptr, Size);
            CurrentPos += Size;
        }
        uint64_t current_pos() const {
            return CurrentPos;
        }
    public:
        unsigned getLineNo() const {
            return LineNo;
        }
        line_counting_ostream(raw_ostream &OS) : raw_ostream(true), CurrentPos(0), LineNo(0), OS(OS) {
            OS.SetUnbuffered();
        }
    };
    line_counting_ostream LC;
    formatted_raw_ostream OS;
    formatted_raw_ostream *OS2;
    std::string moduleName;
    const MCAsmInfo &MAI;
    OwningPtr<MCInstPrinter> InstPrinter;
    OwningPtr<MCCodeEmitter> Emitter;

    SmallString<128> CommentToEmit;
    raw_svector_ostream CommentStream;
    static const bool verboseAsm = true; // TODO MCANNIZZ

    unsigned IsVerboseAsm : 1;
    unsigned ShowInst : 1;

protected:
    bool UseInlineAsm;
    bool EmitDebugLines;
    bool EmitFixupCalls;

private:
    unsigned getCurrentLineNo() {
        OS.flush();
	unsigned LineNo = LC.getLineNo();
#define FAKELINEHACK 0 // keep the line # small to try to stay under 64k for the current debug protocol
#if FAKELINEHACK
	static unsigned LastLineNo = -1;
	static unsigned FakeLineNo = 0;
	if(LineNo != LastLineNo)
	{
		LastLineNo = LineNo;
		LineNo = FakeLineNo;
		FakeLineNo++;
	}
#endif
        return LineNo;
    }

public:
    AVM2MCAsmStreamer(MCContext &Context,
                      formatted_raw_ostream &os,
                      bool isVerboseAsm, bool useLoc,
                      MCInstPrinter *printer,
                      MCCodeEmitter *emitter,
                      bool showInst, formatted_raw_ostream *os2 = NULL)
        : AVM2MCStreamer(Context), LC(os), OS(LC), OS2(os2), MAI(Context.getAsmInfo()),
          InstPrinter(printer), Emitter(emitter), CommentStream(CommentToEmit),
          IsVerboseAsm(false), ShowInst(showInst), TextSection(NULL), Observer(NULL), LastSymbolWeakened(NULL), useSecondStream(false), InFn(false) {
        if (InstPrinter && IsVerboseAsm) {
            InstPrinter->setCommentStream(CommentStream);
        }
    }

    inline formatted_raw_ostream& GetOS() {
        return (OS2 && useSecondStream) ? *OS2 : OS;
    }
    
    virtual void setUseInlineAsm(bool use)
    {
	UseInlineAsm = use;
    }

    virtual void setEmitDebugLines(bool emit)
    {
	EmitDebugLines = emit;
    }

    virtual void setEmitFixupCalls(bool emit)
    {
	EmitFixupCalls = emit;
    }

    virtual void setModulePackageName(const std::string &mn)
    {
        moduleName = mn;
    }

    ~AVM2MCAsmStreamer() {}

    struct Section {
        std::string DataStr;
        unsigned DataAlign;

        Section() : DataAlign(0) {}
    };
    typedef std::map<const MCSection *, Section> SectionMap;
    SectionMap Sections;

    struct Fixup {
        const MCSection *Sect;
        const MCExpr *Expr;
        unsigned Offs;
        unsigned Size;
    };
    typedef std::vector<Fixup> FixupVector;
    FixupVector Fixups;

    typedef std::vector<const MCSymbol *> SymbolVector;
    SymbolVector Assignments;

    const MCSection *TextSection;

    typedef std::map<const MCSymbol *, unsigned> Symbol2ValueMap;
    Symbol2ValueMap Symbol2Value;

    typedef std::set<const MCSymbol *> SymbolSet;
    SymbolSet LateLabels;

    AVM2MCStreamerObserver *Observer;

    virtual void setObserver(AVM2MCStreamerObserver *O) {
        Observer = O;
    }

    inline void EmitEOL() {
        // If we don't have any comments, just emit a \n.
        if (!IsVerboseAsm) {
            GetOS() << '\n';
            return;
        }
        EmitCommentsAndEOL();
    }
    void EmitCommentsAndEOL();

    /// isVerboseAsm - Return true if this streamer supports verbose assembly at
    /// all.
    virtual bool isVerboseAsm() const {
        return IsVerboseAsm;
    }

    /// hasRawTextSupport - We support EmitRawText.
    virtual bool hasRawTextSupport() const {
        return true;
    }

    /// AddComment - Add a comment that can be emitted to the generated .s
    /// file if applicable as a QoI issue to make the output of the compiler
    /// more readable.  This only affects the MCAsmStreamer, and only when
    /// verbose assembly output is enabled.
    virtual void AddComment(const Twine &T);

    /// AddEncodingComment - Add a comment showing the encoding of an instruction.
    virtual void AddEncodingComment(const MCInst &Inst);

    /// GetCommentOS - Return a raw_ostream that comments can be written to.
    /// Unlike AddComment, you are required to terminate comments with \n if you
    /// use this method.
    virtual raw_ostream &GetCommentOS() {
        if (!IsVerboseAsm) {
            return nulls();    // Discard comments unless in verbose asm mode.
        }
        return CommentStream;
    }

    /// AddBlankLine - Emit a blank line to a .s file to pretty it up.
    virtual void AddBlankLine() {
        EmitEOL();
    }

    /// @name MCStreamer Interface
    /// @{

    virtual void ChangeSection(const MCSection *Section, const MCExpr *);
    
    virtual void InitSections() {}
    
    virtual void InitToTextSection() {}
    
    virtual void EmitBundleAlignMode(unsigned AlignPow2) {}

    virtual void EmitBundleLock(bool AlignToEnd) {}

    virtual void EmitBundleUnlock() {}

    virtual void EmitLabel(MCSymbol *Symbol);

    virtual void EmitAssemblerFlag(MCAssemblerFlag Flag);

    virtual void EmitThumbFunc(MCSymbol *Func);

    virtual void EmitAssignment(MCSymbol *Symbol, const MCExpr *Value);

    virtual void EmitWeakReference(MCSymbol *Alias, const MCSymbol *Symbol);

    virtual void EmitDwarfAdvanceLineAddr(int64_t LineDelta,
                                          const MCSymbol *LastLabel,
                                          const MCSymbol *Label,
                                          unsigned PointerSize);

    // symbol last weakened by EmitSymbolAttribute
    const MCSymbol *LastSymbolWeakened;

    virtual void EmitSymbolAttribute(MCSymbol *Symbol, MCSymbolAttr Attribute);

    virtual void EmitSymbolDesc(MCSymbol *Symbol, unsigned DescValue);
    virtual void BeginCOFFSymbolDef(const MCSymbol *Symbol);
    virtual void EmitCOFFSymbolStorageClass(int StorageClass);
    virtual void EmitCOFFSymbolType(int Type);
    virtual void EndCOFFSymbolDef();
    virtual void EmitELFSize(MCSymbol *Symbol, const MCExpr *Value);
    virtual void EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                                  unsigned ByteAlignment);

    /// EmitLocalCommonSymbol - Emit a local common (.lcomm) symbol.
    ///
    /// @param Symbol - The common symbol to emit.
    /// @param Size - The size of the common symbol.
    virtual void EmitLocalCommonSymbol(MCSymbol *Symbol, uint64_t Size,
                                        unsigned ByteAlignment);

    virtual void EmitZerofill(const MCSection *Section, MCSymbol *Symbol = 0,
                              uint64_t Size = 0, unsigned ByteAlignment = 0);

    virtual void EmitTBSSSymbol(const MCSection *Section, MCSymbol *Symbol,
                                uint64_t Size, unsigned ByteAlignment = 0);

    virtual void EmitBytes(StringRef Data, unsigned AddrSpace);

    virtual void EmitValueImpl(const MCExpr *Value, unsigned Size,
                               unsigned AddrSpace);
    virtual void EmitULEB128Value(const MCExpr *Value);
    virtual void EmitSLEB128Value(const MCExpr *Value);

    virtual void EmitFill(uint64_t NumBytes, uint8_t FillValue,
                          unsigned AddrSpace);

    virtual void EmitValueToAlignment(unsigned ByteAlignment, int64_t Value = 0,
                                      unsigned ValueSize = 1,
                                      unsigned MaxBytesToEmit = 0);

    virtual void EmitCodeAlignment(unsigned ByteAlignment,
                                   unsigned MaxBytesToEmit = 0);

    virtual bool EmitValueToOffset(const MCExpr *Offset,
                                   unsigned char Value = 0);

    virtual void EmitFileDirective(StringRef Filename);

    virtual void EmitInstruction(const MCInst &Inst);

    /// EmitRawText - If this file is backed by an assembly streamer, this dumps
    /// the specified string in the output .s file.  This capability is
    /// indicated by the hasRawTextSupport() predicate.
    virtual void EmitRawText(StringRef String);
    virtual void EmitRawTextAsm(StringRef String);

    virtual void FinishImpl(); 

    // Print MCExprs with fully qualified AS3 namespaces prepended to
    // any symbol reference
    void printFQExpr(formatted_raw_ostream &O, const MCExpr *e, AVM2MCStreamerObserver *Obs);

    bool useSecondStream;
    virtual bool setUseSecondStream() {
        OS.flush();
        useSecondStream = true;
        return OS2 != NULL;
    }

    bool InFn; // current emitting a function?

    virtual void EmitFnStart() {
        assert(!InFn);
        InFn = true;
    }

    virtual void EmitFnEnd() {
        assert(InFn);
        InFn = false;
    }

    /// @}

}; // class AVM2MCAsmStreamer

class AVM2MCABCStreamer : public AVM2MCAsmStreamer
{
    bool deleteTmps;
    raw_ostream &objout;
    formatted_raw_ostream *FOS1;
    std::string asout1;
    formatted_raw_ostream *FOS2;
    std::string asout2;
    

    
public:
    AVM2MCABCStreamer(MCContext &Context,
                      bool _deleteTmps,
                      raw_ostream &os,
                      formatted_raw_ostream *fos1,
                      std::string &tmp1,
                      formatted_raw_ostream *fos2,
                      std::string &tmp2,
                      bool isVerboseAsm, bool useLoc,
                      MCInstPrinter *printer,
                      MCCodeEmitter *emitter,
                      bool showInst)
        : AVM2MCAsmStreamer(Context, *fos1, isVerboseAsm, useLoc, printer, emitter, showInst, fos2)
        , deleteTmps(_deleteTmps)
        , objout(os)
        , FOS1(fos1)
        , asout1(tmp1)
        , FOS2(fos2)
        , asout2(tmp2) {
    }
    
    virtual void FinishImpl() {
      //AVM2MCAsmStreamer::Finish();
      AVM2MCAsmStreamer::FinishImpl();
        delete FOS1;
        delete FOS2;

        sys::Path tdir = sys::Path::GetTemporaryDirectory();
        std::string sdk = SetFlasccSDKLocation("/../../");
        
        // remember the current dir and cd into the temp dir
		char cwd[PATH_MAX+1];
        cwd[PATH_MAX] = 0;
        getcwd(cwd, PATH_MAX+1);
        chdir(tdir.c_str());

        std::vector<std::string> args;
        for(unsigned int i=0; i<JVMOpts.size(); i++) args.push_back(JVMOpts[i]);
        
        if(UseInlineAsm) {
            args.push_back("-classpath");
            args.push_back(nativepath(sdk + "/usr/lib/asc.jar"));
            args.push_back("macromedia.asc.embedding.ScriptCompiler");
        } else {
            args.push_back("-jar");
            args.push_back(nativepath(sdk + "/usr/lib/asc2.jar"));
            args.push_back("-merge");
            args.push_back("-md");
            if(FalconParallel)
                args.push_back("-parallel");
        }
        
        args.push_back("-abcfuture");
        args.push_back("-AS3");
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/builtin.abc"));
        args.push_back("-import");
        if(TargetPlayer)
            args.push_back(nativepath(sdk + "/usr/lib/playerglobal.abc"));
        else
            args.push_back(nativepath(sdk + "/usr/lib/shell_toplevel.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/BinaryData.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/Exit.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/LongJmp.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/ISpecialFile.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/IBackingStore.abc"));

        if(TargetPlayer) {
            args.push_back("-import");
            args.push_back(nativepath(sdk + "/usr/lib/InMemoryBackingStore.abc"));
        }

        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/IVFS.abc"));
        args.push_back("-import");
        args.push_back(nativepath(sdk + "/usr/lib/CModule.abc"));

        bool fixImportPath = false;
        for(unsigned int i=0; i<ASCOpts.size(); i++) {
            args.push_back(fixImportPath ? nativepath(ASCOpts[i]) : ASCOpts[i]);
            if (ASCOpts[i].find("-import") != std::string::npos)
                fixImportPath = true;
            else
                fixImportPath = false;
        }
        if(!EmitDebugLines)
            args.push_back("-d");
        args.push_back(nativepath(asout2));
        args.push_back(nativepath(asout1));
        args.push_back("-outdir");
        args.push_back(".");
        args.push_back("-out");
        args.push_back("output");
        
        sys::Path jvmPath(JVMPath);

        if(!jvmPath.canExecute())
        {
            std::string pathWithSuffix = (JVMPath + "." + sys::Path::GetEXESuffix()).str();
            jvmPath = StringRef(pathWithSuffix);
        }

        runcmd(nativepath(jvmPath.str()), args);

        OwningPtr<MemoryBuffer> mbuf;
        sys::Path output(tdir);
        output.appendComponent("output.abc");
        MemoryBuffer::getFile(output.c_str(), mbuf);
        if(mbuf.get()) {
            objout.write(mbuf.get()->getBufferStart(), mbuf.get()->getBufferSize());
            objout.flush();
        }
        chdir(cwd);

        if(deleteTmps) {
            sys::Path tmpfile1(asout1);
            tmpfile1.eraseFromDisk(true);
            sys::Path tmpfile2(asout2);
            tmpfile2.eraseFromDisk(true);
        }

        tdir.eraseFromDisk(true);
    }
};

}
/// TODO: Add appropriate implementation of Emit*() methods when needed

void AVM2MCAsmStreamer::AddComment(const Twine &T)
{
    if (!IsVerboseAsm) {
        return;
    }

    // Make sure that CommentStream is flushed.
    CommentStream.flush();

    T.toVector(CommentToEmit);
    // Each comment goes on its own line.
    CommentToEmit.push_back('\n');

    // Tell the comment stream that the vector changed underneath it.
    CommentStream.resync();
}

void AVM2MCAsmStreamer::EmitCommentsAndEOL()
{
    if (CommentToEmit.empty() && CommentStream.GetNumBytesInBuffer() == 0) {
        GetOS() << '\n';
        return;
    }

    CommentStream.flush();
    StringRef Comments = CommentToEmit.str();

    assert(Comments.back() == '\n' &&
           "Comment array not newline terminated");
    do {
        // Emit a line of comments.
        OS.PadToColumn(MAI.getCommentColumn());
        size_t Position = Comments.find('\n');
        GetOS() << MAI.getCommentString() << ' ' << Comments.substr(0, Position) << '\n';

        Comments = Comments.substr(Position+1);
    } while (!Comments.empty());

    CommentToEmit.clear();
    // Tell the comment stream that the vector changed underneath it.
    CommentStream.resync();
}

void AVM2MCAsmStreamer::ChangeSection(const MCSection *Section, const MCExpr *)
{
    assert(Section && "Cannot switch to a null section!");

}

void AVM2MCAsmStreamer::EmitLabel(MCSymbol *Symbol)
{
    assert(Symbol->isUndefined() && "Cannot define a symbol twice!");
    assert(!Symbol->isVariable() && "Cannot emit a variable symbol!");
    assert(getCurrentSection().first && "Cannot emit before setting section!");

    const MCSection *Sect = getCurrentSection().first;

    if(Sect->getKind().isText()) {
		unsigned lineNo = getCurrentLineNo();
		Symbol2Value[Symbol] = lineNo;
	    Symbol->setSection(*Sect);
        if(InFn) {
	    if(EmitDebugLines)
		GetOS() << "__asm(debugline, u30(" << ++lineNo << ")); "; // no nl
	    if(UseInlineAsm)
                GetOS() << "__asm(lbl(\"" << *Symbol << "\"))\n";
	    else
                GetOS() << *Symbol << ":;\n";
        }
	else // not "InFn" but text section -- probably entry label; align
	{
		while((lineNo & 3) != 3) // FIXME hardcoded align of 4....
                {
                        lineNo++; // FIXME this is broken w/; "secondstream"
			GetOS() << "\n";
#if FAKELINEHACK
			getCurrentLineNo(); // keep the line # incrementing
#endif
                }
		lineNo++;
                // line # here will be div by 4
                GetOS() << "/*\n" << *Symbol << "\n*/\n";
	}
        if(Observer) {
            Observer->notifyLabelEmitted(Symbol, lineNo, InFn);
        }
        else {
          assert(TextSection && "should have one by now, or at least no late labels in .text");
          Symbol->setSection(*TextSection);
          LateLabels.insert(Symbol);
        }
        if(!TextSection) {
            TextSection = &Symbol->getSection();
        } else
        {
            // TODO -- user facing error
            assert(TextSection == Sect && "only support one text section!");
        }
    } else {
        unsigned Offs = Sections[getCurrentSection().first].DataStr.length();
        Symbol->setSection(*Sect);
		Symbol2Value[Symbol] = Offs;
	    GetOS() << "// " << *Symbol << "\n";
        if(Observer)
          Observer->notifyDataSym(Symbol, Offs);
        else
          LateLabels.insert(Symbol);
    }
}

void AVM2MCAsmStreamer::EmitAssemblerFlag(MCAssemblerFlag Flag) {}

void AVM2MCAsmStreamer::EmitThumbFunc(MCSymbol *Func) {}

void AVM2MCAsmStreamer::EmitAssignment(MCSymbol *Symbol, const MCExpr *Value)
{
    if(Symbol == LastSymbolWeakened && Value->getKind() == MCExpr::SymbolRef)
    {
      const MCSymbol *Target = &(((MCSymbolRefExpr *)Value)->getSymbol());
      EmitWeakReference(Symbol, Target);
      return;
    }
    Symbol->setVariableValue(Value);
    Assignments.push_back(Symbol);
}

void AVM2MCAsmStreamer::EmitWeakReference(MCSymbol *Alias,
        const MCSymbol *Symbol)
{
    if(Observer)
      Observer->notifyWeakReference(Alias, Symbol);
}

void AVM2MCAsmStreamer::EmitDwarfAdvanceLineAddr(int64_t LineDelta,
        const MCSymbol *LastLabel,
        const MCSymbol *Label,
        unsigned PointerSize)
{
	// sort of copied from MCObjectStreamer.cpp
  if (!LastLabel) {
    EmitDwarfSetLineAddr(LineDelta, Label, PointerSize);
    return;
  }

  int64_t LastRes = Symbol2Value[LastLabel];
  int64_t LRes = Symbol2Value[Label];

  assert(LastRes && LRes);
  MCDwarfLineAddr::Emit(this, LineDelta, LRes-LastRes);
}

void AVM2MCAsmStreamer::EmitSymbolAttribute(MCSymbol *Symbol,
        MCSymbolAttr Attribute)
{
	if(Attribute == MCSA_WeakReference)
		LastSymbolWeakened = Symbol;
}

void AVM2MCAsmStreamer::EmitSymbolDesc(MCSymbol *Symbol, unsigned DescValue) {}

void AVM2MCAsmStreamer::BeginCOFFSymbolDef(const MCSymbol *Symbol) {}

void AVM2MCAsmStreamer::EmitCOFFSymbolStorageClass (int StorageClass) {}

void AVM2MCAsmStreamer::EmitCOFFSymbolType (int Type) {}

void AVM2MCAsmStreamer::EndCOFFSymbolDef() {}

void AVM2MCAsmStreamer::EmitELFSize(MCSymbol *Symbol, const MCExpr *Value) {}

void AVM2MCAsmStreamer::EmitCommonSymbol(MCSymbol *Symbol, uint64_t Size,
        unsigned ByteAlignment)
{
    if(!Symbol->isDefined()) { // already has a section? already defined
        if(Observer) {
            Observer->notifyCommonSym(Symbol, false);
        }
        assert(Symbol->isDefined());
        const MCSection *Sect = &(Symbol->getSection());
        Symbol->setUndefined();
        EmitZerofill(Sect, Symbol, Size, ByteAlignment);
    }
    if(verboseAsm)
        GetOS() << "// EmitCommonSymbol\n";
}

void AVM2MCAsmStreamer::EmitLocalCommonSymbol(MCSymbol *Symbol, uint64_t Size,
        unsigned ByteAlignment)
{
    if(!Symbol->isDefined()) { // already has a section? already defined
        if(Observer) {
            Observer->notifyCommonSym(Symbol, true);
        }
        assert(Symbol->isDefined());
        const MCSection *Sect = &(Symbol->getSection());
        Symbol->setUndefined();
        EmitZerofill(Sect, Symbol, Size, ByteAlignment); 
    }
    if(verboseAsm)
        GetOS() << "// EmitLocalCommonSymbol\n";
}

void AVM2MCAsmStreamer::EmitZerofill(const MCSection *Section, MCSymbol *Symbol,
                                     uint64_t Size, unsigned ByteAlignment)
{
    const MCSection *OldSect = getCurrentSection().first;

    if(Section != OldSect) {
        SwitchSection(Section);
    }
    EmitValueToAlignment(ByteAlignment);
    EmitLabel(Symbol);
    if(Size) {
        Sections[Section].DataStr.append(Size, (char)0);
    }
    if(verboseAsm)
        GetOS() << "// EmitZerofill " << Size << "\n";
    if(Section != OldSect) {
        SwitchSection(OldSect);
    }
}

void AVM2MCAsmStreamer::EmitTBSSSymbol(const MCSection *Section,
                                       MCSymbol *Symbol,
                                       uint64_t Size, unsigned ByteAlignment)
{
    assert(false);
}

void AVM2MCAsmStreamer::EmitBytes(StringRef Data, unsigned AddrSpace)
{
    assert(getCurrentSection().first && "Cannot emit contents before setting section!");
    if (Data.empty()) {
        return;
    }

    std::string str = Data.str();
    Sections[getCurrentSection().first].DataStr += str;
    if(verboseAsm)
    {
        GetOS() << "// EmitBytes\n";
        GetOS() << "// 0x";
        const unsigned char *data = (const unsigned char *)str.data();
        for(size_t n = str.length(); n; n--, data++) {
            GetOS() << hexdigit(*data >> 4) << hexdigit(*data & 0xf);
        }
        GetOS() << "\n";
    }
}

void AVM2MCAsmStreamer::printFQExpr(formatted_raw_ostream &O, const MCExpr *e, AVM2MCStreamerObserver *Obs) {
    if(!e) return;
    
    switch(e->getKind()) {
        case MCExpr::Target:
        case MCExpr::Constant:
        {
            O << *e;
            break;
        }
        case MCExpr::SymbolRef:
        {
            MCSymbolRefExpr *SE = (MCSymbolRefExpr*)e;
            const MCSymbol *Sym = &SE->getSymbol();

            if(LateLabels.find(Sym) == LateLabels.end() && std::find(Assignments.begin(), Assignments.end(), Sym) == Assignments.end()) {
                if(Obs->isPrivate(Sym))
                    O << moduleName << ".";
                else
                    O << "C_Run.";
            }
            O << *Sym;
            break;
        }
        case MCExpr::Unary:
        {
            MCUnaryExpr *UE = (MCUnaryExpr*)e;
            printFQExpr(O, UE->getSubExpr(), Obs);
            switch(UE->getOpcode()) {
                case MCUnaryExpr::LNot:  O << "!"; break;
                case MCUnaryExpr::Minus: O << "-"; break;
                case MCUnaryExpr::Not:   O << "~"; break;
                case MCUnaryExpr::Plus:  O << "+"; break;
                default: report_fatal_error("Unhandled MCUnaryExpr operation");
            }
            break;
        }
        case MCExpr::Binary:
        {
            MCBinaryExpr *BE = (MCBinaryExpr*)e;
            printFQExpr(O, BE->getLHS(), Obs);
            switch(BE->getOpcode()) {
                case MCBinaryExpr::Add:  O << "+"; break;
                case MCBinaryExpr::And:  O << "&"; break;
                case MCBinaryExpr::Div:  O << "/"; break;
                case MCBinaryExpr::EQ:   O << "=="; break;
                case MCBinaryExpr::GT :  O << ">"; break;
                case MCBinaryExpr::GTE:  O << ">="; break;
                case MCBinaryExpr::LAnd: O << "&&"; break;
                case MCBinaryExpr::LOr:  O << "||"; break;
                case MCBinaryExpr::LT:   O << "<"; break;
                case MCBinaryExpr::LTE:  O << "<="; break;
                case MCBinaryExpr::Mod:  O << "%"; break;
                case MCBinaryExpr::Mul:  O << "*"; break;
                case MCBinaryExpr::NE:   O << "!="; break;
                case MCBinaryExpr::Or:   O << "|"; break;
                case MCBinaryExpr::Shl:  O << "<<"; break;
                case MCBinaryExpr::Shr:  O << ">>"; break;
                case MCBinaryExpr::Sub:  O << "-"; break;
                case MCBinaryExpr::Xor:  O << "^"; break;
                default: report_fatal_error("Unhandled MCBinaryExpr operation");
            }
            printFQExpr(O, BE->getRHS(),Obs);
            break;
        }
        default: report_fatal_error("Unhandled MCExpr node type");
    }
}

void AVM2MCAsmStreamer::EmitValueImpl(const MCExpr *Value, unsigned Size,
                                      unsigned AddrSpace)
{
    assert(getCurrentSection().first && "Cannot emit contents before setting section!");
    assert(AddrSpace == 0);
    assert(getContext().getAsmInfo().isLittleEndian());

    if(verboseAsm)
    {
        GetOS() << "// EmitValueImpl\n";
        GetOS() << "/* value: (" << *Value << ") size: " << Size << " */\n";
    }

    std::string &DataStr = Sections[getCurrentSection().first].DataStr;
    int64_t IntValue;
    if (Value->EvaluateAsAbsolute(IntValue)) {
        if (verboseAsm) {
            GetOS() << "// Absolute value: " << IntValue << "\n";
        }
        while(Size--) {
            DataStr.append(1, (char)IntValue);
            IntValue >>= 8;
        }
        return;
    } else if (verboseAsm) {
        GetOS() << "// Not absolute\n";
    }
    switch (Size) {
    default:
        report_fatal_error("Don't know how to emit this value.");
        break;
    case 1:
        break;
    case 2:
        break;
    case 4:
        break;
    }

    Fixups.resize(Fixups.size()+1);
    Fixup &F = Fixups.back();
    F.Sect = getCurrentSection().first;
    F.Offs = DataStr.length();
    F.Size = Size;
    F.Expr = Value;
    DataStr.append(Size, '\0');
}

void AVM2MCAsmStreamer::EmitULEB128Value(const MCExpr *Value)
{
    assert(MAI.hasLEB128() && "Cannot print a .uleb");
    assert(false); // TODO
    GetOS() << ".uleb128 " << *Value;
    EmitEOL();
}

void AVM2MCAsmStreamer::EmitSLEB128Value(const MCExpr *Value)
{
    assert(MAI.hasLEB128() && "Cannot print a .sleb");
    assert(false); // TODO
    GetOS() << ".sleb128 " << *Value;
    EmitEOL();
}

/// EmitFill - Emit NumBytes bytes worth of the value specified by
/// FillValue.  This implements directives such as '.space'.
void AVM2MCAsmStreamer::EmitFill(uint64_t NumBytes, uint8_t FillValue,
                                 unsigned AddrSpace)
{
    if (NumBytes == 0) {
        return;
    }

    // Emit a byte at a time.
    MCStreamer::EmitFill(NumBytes, FillValue, AddrSpace);
}

void AVM2MCAsmStreamer::EmitValueToAlignment(unsigned ByteAlignment,
        int64_t Value,
        unsigned ValueSize,
        unsigned MaxBytesToEmit)
{
    unsigned &DataAlign = Sections[getCurrentSection().first].DataAlign;
    std::string &DataStr = Sections[getCurrentSection().first].DataStr;

    if(ByteAlignment > DataAlign) {
        DataAlign = ByteAlignment;
    }

    assert(ValueSize == 1);
    unsigned N = (ByteAlignment - (DataStr.length() % ByteAlignment)) % ByteAlignment;
    if(N) {
        DataStr.append(N, (char)Value);
    }
    if(verboseAsm)
    {
        GetOS() << "// EmitValueToAlignment\n";
        GetOS() << "// align: " << ByteAlignment << " fill: " << Value << " emitted: " << N << "\n";
    }
}

void AVM2MCAsmStreamer::EmitCodeAlignment(unsigned ByteAlignment,
        unsigned MaxBytesToEmit) {}

bool AVM2MCAsmStreamer::EmitValueToOffset(const MCExpr *Offset,
        unsigned char Value) { return false; }


void AVM2MCAsmStreamer::EmitFileDirective(StringRef Filename)
{
    return; // no-op
}

void AVM2MCAsmStreamer::AddEncodingComment(const MCInst &Inst) {}

void AVM2MCAsmStreamer::EmitInstruction(const MCInst &Inst)
{
	assert(false && "don't think this should get called for us...");
    assert(getCurrentSection().first && "Cannot emit contents before setting section!");

    // Show the encoding in a comment if we have a code emitter.
    if (Emitter) {
        AddEncodingComment(Inst);
    }

    // Show the MCInst if enabled.
    if (ShowInst) {
        Inst.dump_pretty(GetCommentOS(), &MAI, InstPrinter.get(), "\n ");
        GetCommentOS() << "\n";
    }

    // If we have an AsmPrinter, use that to print, otherwise print the MCInst.
    if (InstPrinter) {
        InstPrinter->printInst(&Inst, OS, StringRef());
    } else {
        Inst.print(OS, &MAI);
    }
    EmitEOL();
}

/// EmitRawText - If this file is backed by an assembly streamer, this dumps
/// the specified string in the output .s file.  This capability is
/// indicated by the hasRawTextSupport() predicate.
void AVM2MCAsmStreamer::EmitRawText(StringRef String)
{
    // trim trailing '\n' as we'll EmitEOL later
    if (!String.empty() && String.back() == '\n') {
        String = String.substr(0, String.size()-1);
    }
    GetOS() << String;
    EmitEOL();
}

void AVM2MCAsmStreamer::EmitRawTextAsm(StringRef String)
{
    // trim trailing '\n' as we'll EmitEOL later
    if (!String.empty() && String.back() == '\n') {
        String = String.substr(0, String.size()-1);
    }
    // search for package directive at the beginning
    if(String.startswith("#package "))
    {
      size_t nlPos = String.find('\n');

      if(nlPos == StringRef::npos)
          nlPos = String.size();
      std::string pkg = String.substr(9, nlPos-9);
      if(pkg == "public")
          Observer->notifyPackageDirective(NULL);
      else if(pkg == "private")
          Observer->notifyPackageDirective((const char *)-1);
      else
          Observer->notifyPackageDirective(pkg.c_str());
      String = String.substr(nlPos);
    }

    // split on package directives (on new lines)
    SmallVector<StringRef, 1> Packs;

    String.split(Packs, "\n#package ");

    SmallVector<StringRef, 1>::const_iterator I = Packs.begin(), E = Packs.end();

    if(I != E)
    {
      GetOS() << *I; // first one just gets printed
      I++;
      while(I != E)
      {
        size_t nlPos = I->find('\n');

        if(nlPos == StringRef::npos)
            nlPos = I->size();
        std::string pkg = I->substr(0, nlPos);
        if(pkg == "public")
            Observer->notifyPackageDirective(NULL);
        else if(pkg == "private")
            Observer->notifyPackageDirective((const char *)-1);
        else
            Observer->notifyPackageDirective(pkg.c_str());
        GetOS() << I->substr(nlPos, I->size());
        I++;
      }
    }
    EmitEOL();
}

struct MCSectNamePred {
    bool operator()(const MCSection *A, const MCSection *B) const {
        std::string NA = getSectionName(*A);
        std::string NB = getSectionName(*B);
        return NA < NB;
    }
};

void AVM2MCAsmStreamer::FinishImpl()
{
	AVM2MCStreamerObserver *FixupObserver = Observer; // Fixups still want to query the linkage of symbols, so we need to retain a ref
	Observer = NULL; // mostly shut down by now

	if (getContext().hasDwarfFiles())
		MCDwarfFileTable::Emit(this);

    GetOS() << ";" << INIT_MD << "function modAllocSects():Object {\n";
    if(InstrumentFlasccInitCode) GetOS() << "    trace(\"modAllocSects: " << moduleName << "\");\n";
    GetOS() << "\treturn {\n";
    {
        typedef std::vector<const MCSection *> SectVector;
        SectVector SV;

        {
            SectionMap::const_iterator SI = Sections.begin(), SE = Sections.end();
            for(; SI != SE; SI++) {
                assert(!SI->first->getKind().isText());
                SV.push_back(SI->first);
            }
        }

        MCSectNamePred Pred;

        std::sort(SV.begin(), SV.end(), Pred); // order by name

        SectVector::const_iterator SI = SV.begin(), SE = SV.end();

        while(SI != SE) {
            const Section &S = Sections[*SI];
            GetOS() << "\t\t\"" << getSectionName(**SI) << "\": [CModule.allocDataSect(modPkgName, \"" << getSectionName(**SI) << "\", " << S.DataStr.length() << ", " << S.DataAlign << "), " << S.DataStr.length() << "]";
            SI++;
            if(SI == SE && !TextSection) {
                GetOS() << "\n";
            } else {
                GetOS() << ",\n";
            }
        }
    }
    if(TextSection) {
        unsigned LineNo = getCurrentLineNo();
        GetOS() << "\t\t\"" << getSectionName(*TextSection) << "\": [CModule.allocTextSect(modPkgName, \"" << getSectionName(*TextSection) << "\", " << LineNo << "), " << LineNo << "]\n";
    }
    GetOS() << "\t}\n";
    GetOS() << "} // modAllocSects\n";
    typedef std::map<const MCSection *, unsigned> SectIdMap;
    SectIdMap SectIds;

    GetOS() << "// section binary data\n";
    {
        unsigned SectNum = 0;
        SectionMap::const_iterator SI = Sections.begin(), SE = Sections.end();
        for(; SI != SE; SI++) {
            const Section &S = SI->second;
            std::string::const_iterator DI = S.DataStr.begin(), DE = S.DataStr.end();

            bool allzeros = S.DataStr.find_first_not_of('\0') == std::string::npos;

            if(!allzeros) {
                GetOS() << ";[HexData(\"";
                for(; DI != DE; DI++) {
                    unsigned char ch = (unsigned char)*DI;
                    char ch1 = ch >> 4, ch2 = ch & 0xf;
                    ch1 = (ch1 < 10) ? ch1 + '0' : ch1 + 'a' - 10;
                    ch2 = (ch2 < 10) ? ch2 + '0' : ch2 + 'a' - 10;

                    GetOS() << ch1 << ch2;
                }
                GetOS() << "\")]\n";
            }

            GetOS() << ExcludeClassMD << " public class DS" << SectNum << " extends BinaryData { ";
            if(allzeros) {
                GetOS() << "public function DS" << SectNum << "() { length = " << S.DataStr.length() << "; super(); }";
            }
            GetOS() << " }\n";
            SectIds.insert(std::make_pair(SI->first, SectNum++));
        }
        // If there are no classes ASC may re-order this script such that it
        // isn't the last script in the ABC. 
        if(SectNum == 0) {
            GetOS() << ExcludeClassMD << " class __asc_workaround{}; // work around an ASC bug where it might re-order this script\n";
        }
    }
    GetOS() << ";" << INIT_MD << "function modDataInit_" << moduleName << "(s:Object):void {\n";
    if(InstrumentFlasccInitCode) GetOS() << "    trace(\"modDataInit_: " << moduleName << "\");\n";
    GetOS() << "// populate sections\n";
    {
        unsigned SectNum = 0;
        SectionMap::const_iterator SI = Sections.begin(), SE = Sections.end();
        for(; SI != SE; SI++) {
            const Section &S = SI->second;
            std::string::const_iterator DI = S.DataStr.begin(), DE = S.DataStr.end();

            GetOS() << "\tconst S" << SectNum << ":int = s[\"" << getSectionName(*(SI->first)) << "\"][0]\n";
            GetOS() << "\tram.position = S" << SectNum << "\n";
            GetOS() << "\tram.writeBytes(new DS" << SectNum << ")\n";
            SectNum++;
        }
    }
    GetOS() << "// late labels\n";
    SymbolSet::const_iterator SLI = LateLabels.begin(), SLE = LateLabels.end();
    for(; SLI != SLE; SLI++)
    {
      const MCSymbol *S = *SLI;
      GetOS() << "\tconst " << *S << ":int = S" << SectIds[&S->getSection()] << " + " << Symbol2Value[S] << "\n";
    }
    GetOS() << "// assignments\n";
    SymbolVector::const_iterator SYI = Assignments.begin(), SYE = Assignments.end();
    for(; SYI != SYE; SYI++) {
        const MCSymbol *S = *SYI;
        // TODO... theoretically can be 64 bits
        if(S->isUsed()) {
            GetOS() << "\tconst " << *S << ":int = ";
            printFQExpr(GetOS(), S->getVariableValue(), FixupObserver);
            GetOS() << "\n";
        }
    }
    GetOS() << "// fixups\n";
    FixupVector::const_iterator DI = Fixups.begin(), DE = Fixups.end();
    for(; DI != DE; DI++) {
      if(EmitFixupCalls) {
        GetOS() << "\tmodSelf.fixup" << (DI->Size * 8) << "(S" << SectIds[DI->Sect] << " + " << DI->Offs << ", ";
        printFQExpr(GetOS(), DI->Expr, FixupObserver);
        GetOS() << ")\n";
      } else {
	if(UseInlineAsm)
	{
		int StoreOp[] = { -1, 0x3a, 0x3b, -1, 0x3c };
        	GetOS() << "\t__asm(push(";
        	printFQExpr(GetOS(), DI->Expr, FixupObserver);
        	GetOS() << "), push(S" << SectIds[DI->Sect] << " + " << DI->Offs << "), op(" << StoreOp[DI->Size] << "))\n";
	} else {
        	GetOS() << "\tsi" << (DI->Size * 8) << "(";
        	printFQExpr(GetOS(), DI->Expr, FixupObserver);
        	GetOS() << ", S" << SectIds[DI->Sect] << " + " << DI->Offs << ")\n";
        }
      }
    }
    GetOS() << "} // modDataInit\n";
    GetOS() << "} // package end\n";
}

namespace llvm
{
MCStreamer *createAVM2AsmStreamer(MCContext &Context,
                                  formatted_raw_ostream &OS,
                                  bool isVerboseAsm, bool useLoc,
                                  bool useCFI, bool useDwarfDirectory,
                                  MCInstPrinter *IP,
                                  MCCodeEmitter *CE, MCAsmBackend *TAB,
                                  bool ShowInst)
{
    return new AVM2MCAsmStreamer(Context, OS, isVerboseAsm, useLoc, IP, CE, ShowInst);
}

MCStreamer *createAVM2ABCStreamer(const Target &T, StringRef TT,
                                  MCContext &Context, MCAsmBackend &TAB,
                                  raw_ostream &OS,
                                  MCCodeEmitter *_Emitter,
                                  bool RelaxAll,
                                  bool NoExecStack)
{
    std::string tmp1,tmp2;

    bool deleteTmps = false;
    sys::Path td = sys::Path::GetTemporaryDirectory();
    if(AS3TmpFile1 == "" || AS3TmpFile2 == "") {
        sys::Path tmpfile1(td.str() + "/alctmp"), tmpfile2(td.str() + "/alctmp");
        tmpfile1.createTemporaryFileOnDisk();
        tmpfile2.createTemporaryFileOnDisk();
        tmp1 = tmpfile1.str() + ".as";
        tmp2 = tmpfile2.str() + ".as";
        tmpfile1.eraseFromDisk();
        tmpfile2.eraseFromDisk();
        deleteTmps = true;
    } else {
        char cwd[PATH_MAX+1];
        cwd[PATH_MAX] = 0;
        getcwd(cwd, PATH_MAX+1);

        tmp1 = AS3TmpFile1[0] == '/' ? AS3TmpFile1 : (cwd + ("/" + AS3TmpFile1));
        tmp2 = AS3TmpFile2[0] == '/' ? AS3TmpFile2 : (cwd + ("/" + AS3TmpFile2));
    }
    
    std::string err;
    raw_fd_ostream *rfdos1 = new raw_fd_ostream(nativepath(tmp1).c_str(), err);
    raw_fd_ostream *rfdos2 = new raw_fd_ostream(nativepath(tmp2).c_str(), err);
    formatted_raw_ostream *fos1 = new formatted_raw_ostream(*rfdos1, true);
    formatted_raw_ostream *fos2 = new formatted_raw_ostream(*rfdos2, true);
    return new AVM2MCABCStreamer(Context, deleteTmps, OS, fos1, tmp1, fos2, tmp2, false, false, NULL, NULL, false);
}
}
