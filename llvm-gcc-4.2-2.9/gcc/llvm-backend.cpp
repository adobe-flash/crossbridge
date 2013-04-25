/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
Contributed by Chris Lattner (sabre@nondot.org)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include "llvm-internal.h"
#include "llvm-debug.h"
#include "llvm-file-ostream.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/ValueSymbolTable.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/Target/SubtargetFeature.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/StandardPasses.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Program.h"

#include <cassert>
extern "C" {
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "except.h"
#include "flags.h"
#include "tree.h"
#include "diagnostic.h"
#include "output.h"
#include "target.h"
#include "toplev.h"
#include "timevar.h"
#include "function.h"
#include "tree-inline.h"
#include "langhooks.h"
#include "cgraph.h"
#include "params.h"
}

// Non-zero if bytecode from PCH is successfully read.
int flag_llvm_pch_read;

// Non-zero if libcalls should not be simplified.
int flag_no_simplify_libcalls;

// Non-zero if red-zone is disabled.
static int flag_disable_red_zone = 0;

// Non-zero if implicit floating point instructions are disabled.
static int flag_no_implicit_float = 0;

// Global state for the LLVM backend.
Module *TheModule = 0;
DebugInfo *TheDebugInfo = 0;
TargetMachine *TheTarget = 0;
TargetFolder *TheFolder = 0;
TypeConverter *TheTypeConverter = 0;

// A list of thunks to post-process.
std::vector<tree> Thunks;

/// DisableLLVMOptimizations - Allow the user to specify:
/// "-mllvm -disable-llvm-optzns" on the llvm-gcc command line to force llvm
/// optimizations off.
static cl::opt<bool> DisableLLVMOptimizations("disable-llvm-optzns");

static cl::list<std::string>
MAttrs("mattr",
  cl::CommaSeparated,
  cl::desc("Target specific attributes (-mattr=help for details)"),
  cl::value_desc("a1,+a2,-a3,..."));

std::vector<std::pair<Constant*, int> > StaticCtors, StaticDtors;
SmallSetVector<Constant*, 32> AttributeUsedGlobals;
SmallSetVector<Constant*, 32> AttributeCompilerUsedGlobals;
std::vector<Constant*> AttributeAnnotateGlobals;

/// PerFunctionPasses - This is the list of cleanup passes run per-function
/// as each is compiled.  In cases where we are not doing IPO, it includes the 
/// code generator.
static FunctionPassManager *PerFunctionPasses = 0;
static PassManager *PerModulePasses = 0;
static FunctionPassManager *CodeGenPasses = 0;

static void createPerFunctionOptimizationPasses();
static void createPerModuleOptimizationPasses();
static void destroyOptimizationPasses();

//===----------------------------------------------------------------------===//
//                   Matching LLVM Values with GCC DECL trees
//===----------------------------------------------------------------------===//
//
// LLVMValues is a vector of LLVM Values. GCC tree nodes keep track of LLVM
// Values using this vector's index. It is easier to save and restore the index
// than the LLVM Value pointer while using PCH.

// Collection of LLVM Values
static std::vector<Value*> LLVMValues;
typedef DenseMap<Value*, unsigned> LLVMValuesMapTy;
static LLVMValuesMapTy LLVMValuesMap;

/// LocalLLVMValueIDs - This is the set of local IDs we have in our mapping,
/// this allows us to efficiently identify and remove them.  Local IDs are IDs
/// for values that are local to the current function being processed.  These do
/// not need to go into the PCH file, but DECL_LLVM still needs a valid index
/// while converting the function.  Using "Local IDs" allows the IDs for
/// function-local decls to be recycled after the function is done.
static std::vector<unsigned> LocalLLVMValueIDs;

/// llvm_set_decl - Remember the LLVM value for GCC tree node.
void llvm_set_decl(tree Tr, Value *V) {

  // If there is not any value then do not add new LLVMValues entry.
  // However clear Tr index if it is non zero.
  if (!V) {
    if (GET_DECL_LLVM_INDEX(Tr))
      SET_DECL_LLVM_INDEX(Tr, 0);
    return;
  }

  unsigned &ValueSlot = LLVMValuesMap[V];
  if (ValueSlot) {
    // Already in map
    SET_DECL_LLVM_INDEX(Tr, ValueSlot);
    return;
  }

  LLVMValues.push_back(V);
  unsigned Index = LLVMValues.size();
  SET_DECL_LLVM_INDEX(Tr, Index);
  LLVMValuesMap[V] = Index;

  // Remember local values.
  if (!isa<Constant>(V))
    LocalLLVMValueIDs.push_back(Index);
}

/// llvm_set_decl_p - Return TRUE if there is a LLVM Value associate with GCC
/// tree node.
bool llvm_set_decl_p(tree Tr) {
  unsigned Index = GET_DECL_LLVM_INDEX(Tr);
  if (Index == 0)
    return false;

  return LLVMValues[Index - 1] != 0;
}

/// llvm_get_decl - Get LLVM Value for the GCC tree node based on LLVMValues
/// vector index.  If there is not any value associated then use
/// make_decl_llvm() to make LLVM value. When GCC tree node is initialized, it
/// has 0 as the index value. This is why all recorded indices are offset by 1.
Value *llvm_get_decl(tree Tr) {

  unsigned Index = GET_DECL_LLVM_INDEX(Tr);
  if (Index == 0) {
    make_decl_llvm(Tr);
    Index = GET_DECL_LLVM_INDEX(Tr);

    // If there was an error, we may have disabled creating LLVM values.
    if (Index == 0) return 0;
  }
  assert((Index - 1) < LLVMValues.size() && "Invalid LLVM value index");
  assert(LLVMValues[Index - 1] && "Trying to use deleted LLVM value!");

  return LLVMValues[Index - 1];
}

/// changeLLVMConstant - Replace Old with New everywhere, updating all maps
/// (except for AttributeAnnotateGlobals, which is a different kind of animal).
/// At this point we know that New is not in any of these maps.
void changeLLVMConstant(Constant *Old, Constant *New) {
  assert(Old->use_empty() && "Old value has uses!");

  if (AttributeUsedGlobals.count(Old)) {
    AttributeUsedGlobals.remove(Old);
    AttributeUsedGlobals.insert(New);
  }

  if (AttributeCompilerUsedGlobals.count(Old)) {
    AttributeCompilerUsedGlobals.remove(Old);
    AttributeCompilerUsedGlobals.insert(New);
  }

  for (unsigned i = 0, e = StaticCtors.size(); i != e; ++i) {
    if (StaticCtors[i].first == Old)
      StaticCtors[i].first = New;
  }

  for (unsigned i = 0, e = StaticDtors.size(); i != e; ++i) {
    if (StaticDtors[i].first == Old)
      StaticDtors[i].first = New;
  }

  assert(!LLVMValuesMap.count(New) && "New cannot be in the LLVMValues map!");

  // Find Old in the table.
  LLVMValuesMapTy::iterator I = LLVMValuesMap.find(Old);
  if (I == LLVMValuesMap.end()) return;

  unsigned Idx = I->second-1;
  assert(Idx < LLVMValues.size() && "Out of range index!");
  assert(LLVMValues[Idx] == Old && "Inconsistent LLVMValues mapping!");

  LLVMValues[Idx] = New;

  // Remove the old value from the value map.
  LLVMValuesMap.erase(I);

  // Insert the new value into the value map.  We know that it can't already
  // exist in the mapping.
  if (New)
    LLVMValuesMap[New] = Idx+1;
}

/// readLLVMValues - Read LLVM Types string table
void readLLVMValues() {
  GlobalValue *V = TheModule->getNamedGlobal("llvm.pch.values");
  if (!V)
    return;

  GlobalVariable *GV = cast<GlobalVariable>(V);
  ConstantStruct *ValuesFromPCH = cast<ConstantStruct>(GV->getOperand(0));

  for (unsigned i = 0; i < ValuesFromPCH->getNumOperands(); ++i) {
    Value *Va = ValuesFromPCH->getOperand(i);

    if (!Va) {
      // If V is empty then insert NULL to represent empty entries.
      LLVMValues.push_back(Va);
      continue;
    }
    if (ConstantArray *CA = dyn_cast<ConstantArray>(Va)) {
      std::string Str = CA->getAsString();
      Va = TheModule->getValueSymbolTable().lookup(Str);
    }
    assert (Va != NULL && "Invalid Value in LLVMValues string table");
    LLVMValues.push_back(Va);
  }

  // Now, llvm.pch.values is not required so remove it from the symbol table.
  GV->eraseFromParent();

  if (TheDebugInfo) {
    // Replace compile unit received from the PCH file.
    NamedMDNode *NMD = TheModule->getOrInsertNamedMetadata("llvm.dbg.pch.cu");
    if (NMD->getNumOperands() == 1) {
      MDNode *HeaderCUNode = NMD->getOperand(0);
      Value *CUNode = TheDebugInfo->getCU();
      HeaderCUNode->replaceAllUsesWith(CUNode);
    }
    TheDebugInfo->replaceBasicTypesFromPCH();
  }
}

/// writeLLVMValues - GCC tree's uses LLVMValues vector's index to reach LLVM
/// Values.  Create a string table to hold these LLVM Values' names. This string
/// table will be used to recreate LTypes vector after loading PCH.
void writeLLVMValues() {
  if (LLVMValues.empty())
    return;

  LLVMContext &Context = getGlobalContext();

  std::vector<Constant *> ValuesForPCH;
  for (std::vector<Value *>::iterator I = LLVMValues.begin(),
         E = LLVMValues.end(); I != E; ++I)  {
    if (Constant *C = dyn_cast_or_null<Constant>(*I))
      ValuesForPCH.push_back(C);
    else
      // Non constant values, e.g. arguments, are not at global scope.
      // When PCH is read, only global scope values are used.
      ValuesForPCH.push_back(Constant::getNullValue(Type::getInt32Ty(Context)));
  }

  // Create string table.
  Constant *LLVMValuesTable = ConstantStruct::get(Context, ValuesForPCH, false);

  // Create variable to hold this string table.
  GlobalVariable *GV = new GlobalVariable(*TheModule, LLVMValuesTable->getType(),
					  true, GlobalValue::ExternalLinkage,
					  LLVMValuesTable,
					  "llvm.pch.values");
  GV->setUnnamedAddr(true);

  if (TheDebugInfo && TheDebugInfo->getCU().Verify()) {
    NamedMDNode *NMD = TheModule->getOrInsertNamedMetadata("llvm.dbg.pch.cu");
    NMD->addOperand(TheDebugInfo->getCU());
  }
}

/// eraseLocalLLVMValues - drop all non-global values from the LLVM values map.
void eraseLocalLLVMValues() {
  // Erase all the local values, these are stored in LocalLLVMValueIDs.
  while (!LocalLLVMValueIDs.empty()) {
    unsigned Idx = LocalLLVMValueIDs.back()-1;
    LocalLLVMValueIDs.pop_back();

    if (Value *V = LLVMValues[Idx]) {
      assert(!isa<Constant>(V) && "Found global value");
      LLVMValuesMap.erase(V);
    }

    if (Idx == LLVMValues.size()-1)
      LLVMValues.pop_back();
    else
      LLVMValues[Idx] = 0;
  }
}

/// handleVisibility - Forward decl visibility style to global.
void handleVisibility(tree decl, GlobalValue *GV) {
  // If decl has visibility specified explicitely (via attribute) - honour
  // it. Otherwise (e.g. visibility specified via -fvisibility=hidden) honour
  // only if symbol is local.
  if (TREE_PUBLIC(decl) &&
      (DECL_VISIBILITY_SPECIFIED(decl) || !DECL_EXTERNAL(decl))) {
    if (DECL_VISIBILITY(decl) == VISIBILITY_HIDDEN)
      GV->setVisibility(GlobalValue::HiddenVisibility);
    else if (DECL_VISIBILITY(decl) == VISIBILITY_PROTECTED)
      GV->setVisibility(GlobalValue::ProtectedVisibility);
    else if (DECL_VISIBILITY(decl) == VISIBILITY_DEFAULT)
      GV->setVisibility(Function::DefaultVisibility);
  }
}

#ifndef LLVM_TARGET_NAME
#error LLVM_TARGET_NAME macro not specified by GCC backend
#endif

namespace llvm {
#define Declare2(TARG, MOD)   extern "C" void LLVMInitialize ## TARG ## MOD()
#define Declare(T, M) Declare2(T, M)
  Declare(LLVM_TARGET_NAME, TargetInfo);
  Declare(LLVM_TARGET_NAME, Target);
  Declare(LLVM_TARGET_NAME, AsmPrinter);
#undef Declare
#undef Declare2
}

// GuessAtInliningThreshold - Figure out a reasonable threshold to pass llvm's
// inliner.  gcc has many options that control inlining, but we have decided
// not to support anything like that for llvm-gcc.
static unsigned GuessAtInliningThreshold() {
  if (optimize_size)
    // Reduce inline limit.
    return 75;
  
  if (optimize >= 3)
    return 275;
  return 225;
}

void llvm_initialize_backend(void) {
  // Initialize the LLVM backend.
#define DoInit2(TARG, MOD)  LLVMInitialize ## TARG ## MOD()
#define DoInit(T, M) DoInit2(T, M)
  DoInit(LLVM_TARGET_NAME, TargetInfo);
  DoInit(LLVM_TARGET_NAME, Target);
  DoInit(LLVM_TARGET_NAME, AsmPrinter);
#undef DoInit
#undef DoInit2
  
  // Initialize LLVM options.
  std::vector<const char*> Args;
  Args.push_back(progname); // program name

  // Allow targets to specify PIC options and other stuff to the corresponding
  // LLVM backends.
#ifdef LLVM_SET_RED_ZONE_FLAG
  LLVM_SET_RED_ZONE_FLAG(flag_disable_red_zone)
#endif
#ifdef LLVM_SET_TARGET_OPTIONS
  LLVM_SET_TARGET_OPTIONS(Args);
#endif
#ifdef LLVM_SET_MACHINE_OPTIONS
  LLVM_SET_MACHINE_OPTIONS(Args);
#endif
#ifdef LLVM_SET_IMPLICIT_FLOAT
  LLVM_SET_IMPLICIT_FLOAT(flag_no_implicit_float)
#endif
  
  if (time_report)
    Args.push_back("--time-passes");
  if (fast_math_flags_set_p())
    Args.push_back("--enable-unsafe-fp-math");
  if (!flag_honor_infinites)
    Args.push_back("--enable-no-infs-fp-math");
  if (!flag_honor_nans)
    Args.push_back("--enable-no-nans-fp-math");
  if (!flag_omit_frame_pointer)
    Args.push_back("--disable-fp-elim");
  if (!flag_zero_initialized_in_bss)
    Args.push_back("--nozero-initialized-in-bss");
  if (flag_verbose_asm)
    Args.push_back("--asm-verbose");
  if (flag_debug_pass_structure)
    Args.push_back("--debug-pass=Structure");
  if (flag_debug_pass_arguments)
    Args.push_back("--debug-pass=Arguments");
  if (flag_unwind_tables)
    Args.push_back("--unwind-tables");
  if (!flag_schedule_insns)
    Args.push_back("--pre-RA-sched=source");
  if (flag_function_sections)
    Args.push_back("--ffunction-sections");
  if (flag_data_sections)
    Args.push_back("--fdata-sections");
  if (flag_disable_debug_info_print)
    Args.push_back("--disable-debug-info-print");

  // If there are options that should be passed through to the LLVM backend
  // directly from the command line, do so now.  This is mainly for debugging
  // purposes, and shouldn't really be for general use.
  std::vector<std::string> ArgStrings;

  if (flag_inline_trees > 1) {
    unsigned threshold = GuessAtInliningThreshold();
    std::string Arg("--inline-threshold="+utostr(threshold));
    ArgStrings.push_back(Arg);
  }

  if (flag_limited_precision > 0) {
    std::string Arg("--limit-float-precision="+utostr(flag_limited_precision));
    ArgStrings.push_back(Arg);
  }

  if (flag_stack_protect > 0) {
    std::string Arg("--stack-protector-buffer-size=" +
                    utostr(PARAM_VALUE(PARAM_SSP_BUFFER_SIZE)));
    ArgStrings.push_back(Arg);
  }

  for (unsigned i = 0, e = ArgStrings.size(); i != e; ++i)
    Args.push_back(ArgStrings[i].c_str());

  std::vector<std::string> LLVM_Optns; // Avoid deallocation before opts parsed!
  if (llvm_optns) {
    llvm::SmallVector<llvm::StringRef, 16> Buf;
    SplitString(llvm_optns, Buf);
    for(unsigned i = 0, e = Buf.size(); i != e; ++i) {
      LLVM_Optns.push_back(Buf[i]);
      Args.push_back(LLVM_Optns.back().c_str());
    }
  }
 
  Args.push_back(0);  // Null terminator.
  int pseudo_argc = Args.size()-1;
  cl::ParseCommandLineOptions(pseudo_argc, (char**)&Args[0]);

  TheModule = new Module("", getGlobalContext());

  // If the target wants to override the architecture, e.g. turning
  // powerpc-darwin-... into powerpc64-darwin-... when -m64 is enabled, do so
  // now.
  std::string TargetTriple = TARGET_CANONICAL_NAME;
#ifdef LLVM_OVERRIDE_TARGET_ARCH
  std::string Arch = LLVM_OVERRIDE_TARGET_ARCH();
  if (!Arch.empty()) {
    std::string::size_type DashPos = TargetTriple.find('-');
    if (DashPos != std::string::npos)// If we have a sane t-t, replace the arch.
      TargetTriple = Arch + TargetTriple.substr(DashPos);
  }
#endif
#ifdef LLVM_OVERRIDE_TARGET_VERSION
  char *NewTriple;
  bool OverRidden = LLVM_OVERRIDE_TARGET_VERSION(TargetTriple.c_str(),
                                                 &NewTriple);
  if (OverRidden)
    TargetTriple = std::string(NewTriple);
#endif
  TheModule->setTargetTriple(TargetTriple);
  
  TheTypeConverter = new TypeConverter();

  // Create the TargetMachine we will be generating code with.
  // FIXME: Figure out how to select the target and pass down subtarget info.
  std::string Err;
  const Target *TME =
    TargetRegistry::lookupTarget(TargetTriple, Err);
  if (!TME) {
    errs() << "Did not get a target machine! Triplet is " << TargetTriple<<'\n';
    exit(1);
  }

  // Figure out the subtarget feature string we pass to the target.
  std::string FeatureStr;
  SubtargetFeatures Features;
  // The target can set LLVM_SET_SUBTARGET_FEATURES to configure the LLVM
  // backend.
#ifdef LLVM_SET_SUBTARGET_FEATURES
  LLVM_SET_SUBTARGET_FEATURES(Features);
#endif
  
  // Handle -mattr options passed into llvm
  for (unsigned i = 0; i != MAttrs.size(); ++i)
    Features.AddFeature(MAttrs[i]);
  FeatureStr = Features.getString();

  TheTarget = TME->createTargetMachine(TargetTriple, FeatureStr);
  TheTarget->setMCUseLoc(false);
  assert(TheTarget->getTargetData()->isBigEndian() == BYTES_BIG_ENDIAN);

  TheFolder = new TargetFolder(TheTarget->getTargetData());

  // Install information about target datalayout stuff into the module for
  // optimizer use.
  TheModule->setDataLayout(TheTarget->getTargetData()->
                           getStringRepresentation());

  if (debug_info_level > DINFO_LEVEL_NONE)
    TheDebugInfo = new DebugInfo(TheModule);
  else
    TheDebugInfo = 0;
}

/// performLateBackendInitialization - Set backend options that may only be
/// known at codegen time.
void performLateBackendInitialization(void) {
  for (Module::iterator I = TheModule->begin(), E = TheModule->end();
       I != E; ++I)
    if (!I->isDeclaration()) {
      if (flag_disable_red_zone)
        I->addFnAttr(Attribute::NoRedZone);
      if (flag_no_implicit_float)
        I->addFnAttr(Attribute::NoImplicitFloat);
    }
}

void llvm_lang_dependent_init(const char *Name) {
  if (TheDebugInfo)
    TheDebugInfo->Initialize();
  if (Name)
    TheModule->setModuleIdentifier(Name);
}

oFILEstream *AsmOutStream = 0;
static formatted_raw_ostream *AsmOutRawStream = 0;
oFILEstream *AsmIntermediateOutStream = 0;

/// llvm_pch_read - Read bytecode from PCH file. Initialize TheModule and setup
/// LTypes vector.
void llvm_pch_read(const unsigned char *Buffer, unsigned Size) {
  std::string ModuleName = TheModule->getModuleIdentifier();

  delete TheModule;
  delete TheDebugInfo;

  clearTargetBuiltinCache();

  MemoryBuffer *MB = MemoryBuffer::getNewMemBuffer(Size, ModuleName.c_str());
  memcpy((char*)MB->getBufferStart(), Buffer, Size);

  std::string ErrMsg;
  TheModule = ParseBitcodeFile(MB, getGlobalContext(), &ErrMsg);
  delete MB;

  if (debug_info_level > DINFO_LEVEL_NONE) {
    TheDebugInfo = new DebugInfo(TheModule);
    TheDebugInfo->Initialize();
  }

  if (!TheModule) {
    errs() << "Error reading bytecodes from PCH file\n";
    errs() << ErrMsg << "\n";
    exit(1);
  }

  if (PerFunctionPasses || PerModulePasses) {
    destroyOptimizationPasses();

    // Don't run codegen, when we should output PCH
    if (flag_pch_file)
      llvm_pch_write_init();
  }

  // Read LLVM Types string table
  readLLVMTypesStringTable();
  readLLVMValues();

  flag_llvm_pch_read = 1;
}

/// llvm_pch_write_init - Initialize PCH writing. 
void llvm_pch_write_init(void) {
  timevar_push(TV_LLVM_INIT);
  AsmOutStream = new oFILEstream(asm_out_file);
  // FIXME: disentangle ostream madness here.  Kill off ostream and FILE.
  AsmOutRawStream =
    new formatted_raw_ostream(*new raw_os_ostream(*AsmOutStream),
                              formatted_raw_ostream::DELETE_STREAM);

  PerModulePasses = new PassManager();
  PerModulePasses->add(new TargetData(*TheTarget->getTargetData()));

  // If writing to stdout, set binary mode.
  if (asm_out_file == stdout)
    sys::Program::ChangeStdoutToBinary();

  // Emit an LLVM .bc file to the output.  This is used when passed
  // -emit-llvm -c to the GCC driver.
  PerModulePasses->add(createBitcodeWriterPass(*AsmOutRawStream));
  
  // Disable emission of .ident into the output file... which is completely
  // wrong for llvm/.bc emission cases.
  flag_no_ident = 1;

  flag_llvm_pch_read = 0;

  timevar_pop(TV_LLVM_INIT);
}

static void destroyOptimizationPasses() {
  delete PerFunctionPasses;
  delete PerModulePasses;
  delete CodeGenPasses;

  PerFunctionPasses = 0;
  PerModulePasses   = 0;
  CodeGenPasses     = 0;
}

static void createPerFunctionOptimizationPasses() {
  if (PerFunctionPasses) 
    return;

  // Create and set up the per-function pass manager.
  // FIXME: Move the code generator to be function-at-a-time.
  PerFunctionPasses =
    new FunctionPassManager(TheModule);
  PerFunctionPasses->add(new TargetData(*TheTarget->getTargetData()));

  // In -O0 if checking is disabled, we don't even have per-function passes.
  bool HasPerFunctionPasses = false;
#ifdef ENABLE_CHECKING
  PerFunctionPasses->add(createVerifierPass());
  HasPerFunctionPasses = true;
#endif

  if (optimize > 0 && !DisableLLVMOptimizations) {
    HasPerFunctionPasses = true;
    PerFunctionPasses->add(createCFGSimplificationPass());
    if (optimize == 1)
      PerFunctionPasses->add(createPromoteMemoryToRegisterPass());
    else
      PerFunctionPasses->add(createScalarReplAggregatesPass());
    PerFunctionPasses->add(createInstructionCombiningPass());
  }

  // If there are no module-level passes that have to be run, we codegen as
  // each function is parsed.
  // FIXME: We can't figure this out until we know there are no always-inline
  // functions.
  // FIXME: This is disabled right now until bugs can be worked out.  Reenable
  // this for fast -O0 compiles!
  if (!emit_llvm_bc && !emit_llvm && 0) {
    FunctionPassManager *PM = PerFunctionPasses;    
    HasPerFunctionPasses = true;

    CodeGenOpt::Level OptLevel = CodeGenOpt::Default;  // -O2, -Os, and -Oz
    if (optimize == 0)
      OptLevel = CodeGenOpt::None;
    else if (optimize == 1)
      OptLevel = CodeGenOpt::Less;
    else if (optimize == 3)
      // -O3 and above.
      OptLevel = CodeGenOpt::Aggressive;

    // Request that addPassesToEmitFile run the Verifier after running
    // passes which modify the IR.
#ifndef NDEBUG
    bool DisableVerify = false;
#else
    bool DisableVerify = true;
#endif

    // Normal mode, emit a .s file by running the code generator.
    // Note, this also adds codegenerator level optimization passes.
    if (TheTarget->addPassesToEmitFile(*PM, *AsmOutRawStream,
                                       TargetMachine::CGFT_AssemblyFile,
                                       OptLevel, DisableVerify)) {
      errs() << "Error interfacing to target machine!\n";
      exit(1);
    }
  }
  
  if (HasPerFunctionPasses) {
    PerFunctionPasses->doInitialization();
  } else {
    delete PerFunctionPasses;
    PerFunctionPasses = 0;
  }
}

static void createPerModuleOptimizationPasses() {
  if (PerModulePasses)
    // llvm_pch_write_init has already created the per module passes.
    return;

  // FIXME: AT -O0/O1, we should stream out functions at a time.
  PerModulePasses = new PassManager();
  PerModulePasses->add(new TargetData(*TheTarget->getTargetData()));
  bool HasPerModulePasses = false;

  if (!DisableLLVMOptimizations) {
    bool NeedAlwaysInliner = false;
    llvm::Pass *InliningPass = 0;
    if (flag_inline_trees > 1) {                // respect -fno-inline-functions
      InliningPass = createFunctionInliningPass();    // Inline small functions
    } else {
      // If full inliner is not run, check if always-inline is needed to handle
      // functions that are  marked as always_inline.
      for (Module::iterator I = TheModule->begin(), E = TheModule->end();
           I != E; ++I)
        if (I->hasFnAttr(Attribute::AlwaysInline)) {
          NeedAlwaysInliner = true;
          break;
        }

      if (NeedAlwaysInliner)
        InliningPass = createAlwaysInlinerPass();  // Inline always_inline funcs
    }

    HasPerModulePasses = true;
    createStandardModulePasses(PerModulePasses, optimize,
                               optimize_size,
                               flag_unit_at_a_time, flag_unroll_loops,
                               !flag_no_simplify_libcalls, flag_exceptions,
                               InliningPass);
  }

  if (emit_llvm_bc) {
    // Emit an LLVM .bc file to the output.  This is used when passed
    // -emit-llvm -c to the GCC driver.
    PerModulePasses->add(createBitcodeWriterPass(*AsmOutRawStream));
    HasPerModulePasses = true;
  } else if (emit_llvm) {
    // Emit an LLVM .ll file to the output.  This is used when passed 
    // -emit-llvm -S to the GCC driver.
    PerModulePasses->add(createPrintModulePass(AsmOutRawStream));
    HasPerModulePasses = true;
  } else {
    // If there are passes we have to run on the entire module, we do codegen
    // as a separate "pass" after that happens.
    // However if there are no module-level passes that have to be run, we
    // codegen as each function is parsed.
    // FIXME: This is disabled right now until bugs can be worked out.  Reenable
    // this for fast -O0 compiles!
    if (PerModulePasses || 1) {
      FunctionPassManager *PM = CodeGenPasses =
        new FunctionPassManager(TheModule);
      PM->add(new TargetData(*TheTarget->getTargetData()));

      CodeGenOpt::Level OptLevel = CodeGenOpt::Default;

      switch (optimize) {
      default: break;
      case 0: OptLevel = CodeGenOpt::None; break;
      case 3: OptLevel = CodeGenOpt::Aggressive; break;
      }

      // Request that addPassesToEmitFile run the Verifier after running
      // passes which modify the IR.
#ifndef NDEBUG
      bool DisableVerify = false;
#else
      bool DisableVerify = true;
#endif

      // Normal mode, emit a .s file by running the code generator.
      // Note, this also adds codegenerator level optimization passes.
      if (TheTarget->addPassesToEmitFile(*PM, *AsmOutRawStream,
                                         TargetMachine::CGFT_AssemblyFile,
                                         OptLevel, DisableVerify)) {
        errs() << "Error interfacing to target machine!\n";
        exit(1);
      }
    }
  }

  if (!HasPerModulePasses) {
    delete PerModulePasses;
    PerModulePasses = 0;
  }
}

/// llvm_asm_file_start - Start the .s file.
void llvm_asm_file_start(void) {
  timevar_push(TV_LLVM_INIT);
  AsmOutStream = new oFILEstream(asm_out_file);
  // FIXME: disentangle ostream madness here.  Kill off ostream and FILE.
  AsmOutRawStream =
    new formatted_raw_ostream(*new raw_os_ostream(*AsmOutStream),
                              formatted_raw_ostream::DELETE_STREAM);
  flag_llvm_pch_read = 0;

  if (emit_llvm_bc || emit_llvm)
    // Disable emission of .ident into the output file... which is completely
    // wrong for llvm/.bc emission cases.
    flag_no_ident = 1;

  // If writing to stdout, set binary mode.
  if (asm_out_file == stdout)
    sys::Program::ChangeStdoutToBinary();

  AttributeUsedGlobals.clear();
  AttributeCompilerUsedGlobals.clear();
  timevar_pop(TV_LLVM_INIT);
}

/// ConvertStructorsList - Convert a list of static ctors/dtors to an
/// initializer suitable for the llvm.global_[cd]tors globals.
static void CreateStructorsList(std::vector<std::pair<Constant*, int> > &Tors,
                                const char *Name) {
  std::vector<Constant*> InitList;
  std::vector<Constant*> StructInit;
  StructInit.resize(2);
  
  LLVMContext &Context = getGlobalContext();
  
  const Type *FPTy =
    FunctionType::get(Type::getVoidTy(Context),
                      std::vector<const Type*>(), false);
  FPTy = FPTy->getPointerTo();
  
  for (unsigned i = 0, e = Tors.size(); i != e; ++i) {
    StructInit[0] = ConstantInt::get(Type::getInt32Ty(Context), Tors[i].second);
    
    // __attribute__(constructor) can be on a function with any type.  Make sure
    // the pointer is void()*.
    StructInit[1] = TheFolder->CreateBitCast(Tors[i].first, FPTy);
    InitList.push_back(ConstantStruct::get(Context, StructInit, false));
  }
  Constant *Array = ConstantArray::get(
    ArrayType::get(InitList[0]->getType(), InitList.size()), InitList);
  GlobalVariable *GV = new GlobalVariable(*TheModule, Array->getType(), false,
					  GlobalValue::AppendingLinkage,
					  Array, Name);
  GV->setUnnamedAddr(true);
}

/// llvm_asm_file_end - Finish the .s file.
void llvm_asm_file_end(void) {
  timevar_push(TV_LLVM_PERFILE);
  LLVMContext &Context = getGlobalContext();

  // Assign the correct linkage to the thunks now that we've set the linkage and
  // visibility to their targets.
  SmallPtrSet<tree, 4> ThunkOfThunk;

  for (std::vector<tree>::iterator
         I = Thunks.begin(), E = Thunks.end(); I != E; ++I) {
    tree thunk = *I;
    tree thunk_target = lang_hooks.thunk_target(thunk);

    if (lang_hooks.function_is_thunk_p (thunk_target)) {
      ThunkOfThunk.insert(thunk);
      continue;
    }

    Function *Thunk = cast<Function>(DECL_LLVM(thunk));
    const Function *ThunkTarget = cast<Function>(DECL_LLVM(thunk_target));

    Thunk->setLinkage(ThunkTarget->getLinkage());
    Thunk->setVisibility(ThunkTarget->getVisibility());
  }

  // There's a situation where a thunk calls another thunk. In that case, we
  // want to process first the thunk that calls a non-thunk. Then we process
  // each thunk in turn until all thunks have been processed.
  while (!ThunkOfThunk.empty())
    for (SmallPtrSet<tree, 4>::iterator
           I = ThunkOfThunk.begin(), E = ThunkOfThunk.end(); I != E; ++I) {
      tree thunk = *I;
      tree thunk_target = lang_hooks.thunk_target(thunk);

      if (!ThunkOfThunk.count(thunk_target)) {
        Function *Thunk = cast<Function>(DECL_LLVM(thunk));
        const Function *ThunkTarget = cast<Function>(DECL_LLVM(thunk_target));

        Thunk->setLinkage(ThunkTarget->getLinkage());
        Thunk->setVisibility(ThunkTarget->getVisibility());
        ThunkOfThunk.erase(thunk);
        break;
      }
    }

  performLateBackendInitialization();
  createPerFunctionOptimizationPasses();

  if (flag_pch_file) {
    writeLLVMTypesStringTable();
    writeLLVMValues();
    writeLLVMTypeUsers();
  }

  // Add an llvm.global_ctors global if needed.
  if (!StaticCtors.empty())
    CreateStructorsList(StaticCtors, "llvm.global_ctors");
  // Add an llvm.global_dtors global if needed.
  if (!StaticDtors.empty())
    CreateStructorsList(StaticDtors, "llvm.global_dtors");

  if (!AttributeUsedGlobals.empty()) {
    std::vector<Constant *> AUGs;
    const Type *SBP= Type::getInt8PtrTy(Context);
    for (SmallSetVector<Constant *,32>::iterator
           AI = AttributeUsedGlobals.begin(),
           AE = AttributeUsedGlobals.end(); AI != AE; ++AI) {
      Constant *C = *AI;
      AUGs.push_back(TheFolder->CreateBitCast(C, SBP));
    }

    ArrayType *AT = ArrayType::get(SBP, AUGs.size());
    Constant *Init = ConstantArray::get(AT, AUGs);
    GlobalValue *gv = new GlobalVariable(*TheModule, AT, false,
                                         GlobalValue::AppendingLinkage, Init,
                                         "llvm.used");
    gv->setUnnamedAddr(true);
    gv->setSection("llvm.metadata");
    AttributeUsedGlobals.clear();
  }

  if (!AttributeCompilerUsedGlobals.empty()) {
    std::vector<Constant *> ACUGs;
    const Type *SBP= Type::getInt8PtrTy(Context);
    for (SmallSetVector<Constant *,32>::iterator
           AI = AttributeCompilerUsedGlobals.begin(),
           AE = AttributeCompilerUsedGlobals.end(); AI != AE; ++AI) {
      Constant *C = *AI;
      ACUGs.push_back(TheFolder->CreateBitCast(C, SBP));
    }

    ArrayType *AT = ArrayType::get(SBP, ACUGs.size());
    Constant *Init = ConstantArray::get(AT, ACUGs);
    GlobalValue *gv = new GlobalVariable(*TheModule, AT, false,
                                         GlobalValue::AppendingLinkage, Init,
                                         "llvm.compiler.used");
    gv->setUnnamedAddr(true);
    gv->setSection("llvm.metadata");
    AttributeCompilerUsedGlobals.clear();
  }

  // Add llvm.global.annotations
  if (!AttributeAnnotateGlobals.empty()) {
    Constant *Array = ConstantArray::get(
      ArrayType::get(AttributeAnnotateGlobals[0]->getType(),
                                      AttributeAnnotateGlobals.size()),
                       AttributeAnnotateGlobals);
    GlobalValue *gv = new GlobalVariable(*TheModule, Array->getType(), false,
                                         GlobalValue::AppendingLinkage, Array,
                                         "llvm.global.annotations");
    gv->setUnnamedAddr(true);
    gv->setSection("llvm.metadata");
    AttributeAnnotateGlobals.clear();
  }

  // Finish off the per-function pass.
  if (PerFunctionPasses)
    PerFunctionPasses->doFinalization();

  // Emit intermediate file before module level optimization passes are run.
  if (flag_debug_llvm_module_opt) {
    
    static PassManager *IntermediatePM = new PassManager();
    IntermediatePM->add(new TargetData(*TheTarget->getTargetData()));

    char asm_intermediate_out_filename[MAXPATHLEN];
    strcpy(&asm_intermediate_out_filename[0], asm_file_name);
    strcat(&asm_intermediate_out_filename[0],".0");
    FILE *asm_intermediate_out_file = fopen(asm_intermediate_out_filename, "w+b");
    AsmIntermediateOutStream = new oFILEstream(asm_intermediate_out_file);
    raw_ostream *AsmIntermediateRawOutStream = 
      new raw_os_ostream(*AsmIntermediateOutStream);
    if (emit_llvm_bc)
     IntermediatePM->add(createBitcodeWriterPass(*AsmIntermediateRawOutStream));
    if (emit_llvm)
      IntermediatePM->add(createPrintModulePass(AsmIntermediateRawOutStream));
    IntermediatePM->run(*TheModule);
    AsmIntermediateRawOutStream->flush();
    delete AsmIntermediateRawOutStream;
    AsmIntermediateRawOutStream = 0;
    AsmIntermediateOutStream->flush();
    fflush(asm_intermediate_out_file);
    delete AsmIntermediateOutStream;
    AsmIntermediateOutStream = 0;
  }

  // Run module-level optimizers, if any are present.
  createPerModuleOptimizationPasses();
  if (PerModulePasses)
    PerModulePasses->run(*TheModule);
  
  // Run the code generator, if present.
  if (CodeGenPasses) {
    CodeGenPasses->doInitialization();
    for (Module::iterator I = TheModule->begin(), E = TheModule->end();
         I != E; ++I)
      if (!I->isDeclaration())
        CodeGenPasses->run(*I);
    CodeGenPasses->doFinalization();
  }

  AsmOutRawStream->flush();
  AsmOutStream->flush();
  fflush(asm_out_file);
  delete AsmOutRawStream;
  AsmOutRawStream = 0;
  delete AsmOutStream;
  AsmOutStream = 0;
  timevar_pop(TV_LLVM_PERFILE);
}

// llvm_call_llvm_shutdown - Release LLVM global state.
void llvm_call_llvm_shutdown(void) {
#ifndef NDEBUG
  delete PerModulePasses;
  delete PerFunctionPasses;
  delete CodeGenPasses;
  delete TheModule;
  llvm_shutdown();
#endif
}

/// llvm_emit_code_for_current_function - Top level interface for emitting a
/// function to the .s file.
void llvm_emit_code_for_current_function(tree fndecl) {
  if (cfun->nonlocal_goto_save_area)
    sorry("%Jnon-local gotos not supported by LLVM", fndecl);

  if (errorcount || sorrycount) {
    TREE_ASM_WRITTEN(fndecl) = 1;
    return;  // Do not process broken code.
  }

  // Initial fill of TypeRefinementDatabase::TypeUsers[] if we're
  // using a PCH.  Won't work until the GCC PCH has been read in and
  // digested.
  {
    static bool done = false;
    if (!done && flag_llvm_pch_read) {
      readLLVMTypeUsers();
      done = true;
    }
  }

  timevar_push(TV_LLVM_FUNCS);

  // Convert the AST to raw/ugly LLVM code.
  Function *Fn;
  {
    TreeToLLVM *Emitter = new TreeToLLVM(fndecl);
    // FIXME: should we store TheTreeToLLVM right here (current in constructor)?
    enum symbol_visibility vis = DECL_VISIBILITY (fndecl);

    if (vis != VISIBILITY_DEFAULT)
      // "asm_out.visibility" emits an important warning if we're using a
      // visibility that's not supported by the target.
      targetm.asm_out.visibility(fndecl, vis);

    Fn = TheTreeToLLVM->EmitFunction();
    Emitter->~TreeToLLVM();
  }

#if 0
  if (dump_file) {
    fprintf (dump_file,
             "\n\n;;\n;; Full LLVM generated for this function:\n;;\n");
    Fn->dump();
  }
#endif

  performLateBackendInitialization();
  createPerFunctionOptimizationPasses();

  if (PerFunctionPasses)
    PerFunctionPasses->run(*Fn);
  
  // TODO: Nuke the .ll code for the function at -O[01] if we don't want to
  // inline it or something else.
  
  // There's no need to defer outputting this function any more; we
  // know we want to output it.
  DECL_DEFER_OUTPUT(fndecl) = 0;
  
  // Finally, we have written out this function!
  TREE_ASM_WRITTEN(fndecl) = 1;
  timevar_pop(TV_LLVM_FUNCS);
}

/// emit_alias_to_llvm - Given decl and target emit alias to target.
void emit_alias_to_llvm(tree decl, tree target, tree target_decl) {
  if (errorcount || sorrycount) {
    TREE_ASM_WRITTEN(decl) = 1;
    return;  // Do not process broken code.
  }

  timevar_push(TV_LLVM_GLOBALS);

  // Get or create LLVM global for our alias.
  GlobalValue *V = cast<GlobalValue>(DECL_LLVM(decl));
  
  GlobalValue *Aliasee = NULL;
  
  if (target_decl)
    Aliasee = cast<GlobalValue>(DECL_LLVM(target_decl));
  else {
    // This is something insane. Probably only LTHUNKs can be here
    // Try to grab decl from IDENTIFIER_NODE

    // Query SymTab for aliasee
    const char* AliaseeName = IDENTIFIER_POINTER(target);
    Aliasee =
      dyn_cast_or_null<GlobalValue>(TheModule->
                                    getValueSymbolTable().lookup(AliaseeName));

    // Last resort. Query for name set via __asm__
    if (!Aliasee) {
      std::string starred = std::string("\001") + AliaseeName;
      Aliasee =
        dyn_cast_or_null<GlobalValue>(TheModule->
                                      getValueSymbolTable().lookup(starred));
    }
    
    if (!Aliasee) {
      if (lookup_attribute ("weakref", DECL_ATTRIBUTES (decl))) {
        if (GlobalVariable *GV = dyn_cast<GlobalVariable>(V))
          Aliasee = new GlobalVariable(*TheModule, GV->getType(),
                                       GV->isConstant(),
                                       GlobalVariable::ExternalWeakLinkage,
                                       NULL, AliaseeName);
        else if (Function *F = dyn_cast<Function>(V))
          Aliasee = Function::Create(F->getFunctionType(),
                                     Function::ExternalWeakLinkage,
                                     AliaseeName, TheModule);
        else
          assert(0 && "Unsuported global value");
        Aliasee->setUnnamedAddr(true);
      } else {
        error ("%J%qD aliased to undefined symbol %qs", decl, decl, AliaseeName);
        timevar_pop(TV_LLVM_GLOBALS);
        return;
      }
    }
  }

  GlobalValue::LinkageTypes Linkage;

  // A weak alias has TREE_PUBLIC set but not the other bits.
  if (DECL_LLVM_PRIVATE(decl))
    Linkage = GlobalValue::PrivateLinkage;
  else if (DECL_LLVM_LINKER_PRIVATE(decl))
    Linkage = GlobalValue::LinkerPrivateLinkage;
  else if (DECL_LLVM_LINKER_PRIVATE_WEAK(decl))
    Linkage = GlobalValue::LinkerPrivateWeakLinkage;
  else if (DECL_WEAK(decl))
    // The user may have explicitly asked for weak linkage - ignore flag_odr.
    Linkage = GlobalValue::WeakAnyLinkage;
  else if (!TREE_PUBLIC(decl))
    Linkage = GlobalValue::InternalLinkage;
  else
    Linkage = GlobalValue::ExternalLinkage;

  GlobalAlias* GA = new GlobalAlias(Aliasee->getType(), Linkage, "",
                                    Aliasee, TheModule);

  handleVisibility(decl, GA);

  if (GA->getType()->canLosslesslyBitCastTo(V->getType()))
    V->replaceAllUsesWith(ConstantExpr::getBitCast(GA, V->getType()));
  else if (!V->use_empty()) {
    error ("%J Alias %qD used with invalid type!", decl, decl);
    timevar_pop(TV_LLVM_GLOBALS);
    return;
  }
    
  changeLLVMConstant(V, GA);
  GA->takeName(V);
  if (GlobalVariable *GV = dyn_cast<GlobalVariable>(V))
    GV->eraseFromParent();
  else if (GlobalAlias *GA = dyn_cast<GlobalAlias>(V))
    GA->eraseFromParent();
  else if (Function *F = dyn_cast<Function>(V))
    F->eraseFromParent();
  else
    assert(0 && "Unsuported global value");

  TREE_ASM_WRITTEN(decl) = 1;
  
  timevar_pop(TV_LLVM_GLOBALS);
  return;
}

/// ConvertMetadataStringToGV - Convert string to global value. Use existing
/// global if possible.
Constant* ConvertMetadataStringToGV(const char *str) {
  
  Constant *Init = ConstantArray::get(getGlobalContext(), std::string(str));

  // Use cached string if it exists.
  static std::map<Constant*, GlobalVariable*> StringCSTCache;
  GlobalVariable *&Slot = StringCSTCache[Init];
  if (Slot) return Slot;
  
  // Create a new string global.
  GlobalVariable *GV = new GlobalVariable(*TheModule, Init->getType(), true,
                                          GlobalVariable::PrivateLinkage,
                                          Init, ".str");
  GV->setSection("llvm.metadata");
  GV->setUnnamedAddr(true);
  Slot = GV;
  return GV;
  
}

/// AddAnnotateAttrsToGlobal - Adds decls that have a annotate attribute to a
/// vector to be emitted later.
void AddAnnotateAttrsToGlobal(GlobalValue *GV, tree decl) {
  LLVMContext &Context = getGlobalContext();
  
  // Handle annotate attribute on global.
  tree annotateAttr = lookup_attribute("annotate", DECL_ATTRIBUTES (decl));
  if (annotateAttr == 0)
    return;
  
  // Get file and line number
  Constant *lineNo = ConstantInt::get(Type::getInt32Ty(Context),
                                      DECL_SOURCE_LINE(decl));
  Constant *file = ConvertMetadataStringToGV(DECL_SOURCE_FILE(decl));
  const Type *SBP= Type::getInt8PtrTy(Context);
  file = TheFolder->CreateBitCast(file, SBP);
 
  // There may be multiple annotate attributes. Pass return of lookup_attr 
  //  to successive lookups.
  while (annotateAttr) {
    
    // Each annotate attribute is a tree list.
    // Get value of list which is our linked list of args.
    tree args = TREE_VALUE(annotateAttr);
    
    // Each annotate attribute may have multiple args.
    // Treat each arg as if it were a separate annotate attribute.
    for (tree a = args; a; a = TREE_CHAIN(a)) {
      // Each element of the arg list is a tree list, so get value
      tree val = TREE_VALUE(a);
      
      // Assert its a string, and then get that string.
      assert(TREE_CODE(val) == STRING_CST && 
             "Annotate attribute arg should always be a string");
      Constant *strGV = TreeConstantToLLVM::EmitLV_STRING_CST(val);
      Constant *Element[4] = {
        TheFolder->CreateBitCast(GV,SBP),
        TheFolder->CreateBitCast(strGV,SBP),
        file,
        lineNo
      };
 
      AttributeAnnotateGlobals.push_back(
        ConstantStruct::get(Context, Element, 4, false));
    }
      
    // Get next annotate attribute.
    annotateAttr = TREE_CHAIN(annotateAttr);
    if (annotateAttr)
      annotateAttr = lookup_attribute("annotate", annotateAttr);
  }
}

/// reset_initializer_llvm - Change the initializer for a global variable.
void reset_initializer_llvm(tree decl) {
  // If there were earlier errors we can get here when DECL_LLVM has not
  // been set.  Don't crash.
  // We can also get here when DECL_LLVM has not been set for some object
  // referenced in the initializer.  Don't crash then either.
  if (errorcount || sorrycount)
    return;

  // Get or create the global variable now.
  GlobalVariable *GV = cast<GlobalVariable>(DECL_LLVM(decl));
  
  // Visibility may also have changed.
  handleVisibility(decl, GV);

  // Convert the initializer over.
  Constant *Init = TreeConstantToLLVM::Convert(DECL_INITIAL(decl));

  // Set the initializer.
  GV->setInitializer(Init);
  GV->setUnnamedAddr(true);
}
  
/// reset_type_and_initializer_llvm - Change the type and initializer for 
/// a global variable.
void reset_type_and_initializer_llvm(tree decl) {
  // If there were earlier errors we can get here when DECL_LLVM has not
  // been set.  Don't crash.
  // We can also get here when DECL_LLVM has not been set for some object
  // referenced in the initializer.  Don't crash then either.
  if (errorcount || sorrycount)
    return;

  // Get or create the global variable now.
  GlobalVariable *GV = cast<GlobalVariable>(DECL_LLVM(decl));
  
  // Visibility may also have changed.
  handleVisibility(decl, GV);

  // Temporary to avoid infinite recursion (see comments emit_global_to_llvm)
  GV->setInitializer(UndefValue::get(GV->getType()->getElementType()));
  GV->setUnnamedAddr(true);

  // Convert the initializer over.
  Constant *Init = TreeConstantToLLVM::Convert(DECL_INITIAL(decl));

  // If we had a forward definition that has a type that disagrees with our
  // initializer, insert a cast now.  This sort of thing occurs when we have a
  // global union, and the LLVM type followed a union initializer that is
  // different from the union element used for the type.
  if (GV->getType()->getElementType() != Init->getType()) {
    GV->removeFromParent();
    GlobalVariable *NGV = new GlobalVariable(*TheModule, Init->getType(), 
                                             GV->isConstant(),
                                             GV->getLinkage(), 0,
                                             GV->getName());
    NGV->setUnnamedAddr(true);
    NGV->setVisibility(GV->getVisibility());
    NGV->setSection(GV->getSection());
    NGV->setAlignment(GV->getAlignment());
    NGV->setLinkage(GV->getLinkage());
    GV->replaceAllUsesWith(TheFolder->CreateBitCast(NGV, GV->getType()));
    changeLLVMConstant(GV, NGV);
    delete GV;
    SET_DECL_LLVM(decl, NGV);
    GV = NGV;
  }

  // Set the initializer.
  GV->setInitializer(Init);
  GV->setUnnamedAddr(true);
}
  
/// emit_global_to_llvm - Emit the specified VAR_DECL or aggregate CONST_DECL to
/// LLVM as a global variable.  This function implements the end of
/// assemble_variable.
void emit_global_to_llvm(tree decl) {
  if (errorcount || sorrycount) {
    TREE_ASM_WRITTEN(decl) = 1;
    return;  // Do not process broken code.
  }

  // FIXME: Support alignment on globals: DECL_ALIGN.
  // FIXME: DECL_PRESERVE_P indicates the var is marked with attribute 'used'.

  // Global register variables don't turn into LLVM GlobalVariables.
  if (TREE_CODE(decl) == VAR_DECL && DECL_REGISTER(decl))
    return;

  // If tree nodes says defer output then do not emit global yet.
  if (CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS) 
      && (DECL_DEFER_OUTPUT(decl)))
      return;

  // If we encounter a forward declaration then do not emit the global yet.
  if (!TYPE_SIZE(TREE_TYPE(decl)))
    return;

  timevar_push(TV_LLVM_GLOBALS);

  // Get or create the global variable now.
  GlobalVariable *GV = cast<GlobalVariable>(DECL_LLVM(decl));
  
  // Convert the initializer over.
  Constant *Init;
  if (DECL_INITIAL(decl) == 0 || DECL_INITIAL(decl) == error_mark_node) {
    // This global should be zero initialized.  Reconvert the type in case the
    // forward def of the global and the real def differ in type (e.g. declared
    // as 'int A[]', and defined as 'int A[100]').
    Init = Constant::getNullValue(ConvertType(TREE_TYPE(decl)));
  } else {
    assert((TREE_CONSTANT(DECL_INITIAL(decl)) || 
            TREE_CODE(DECL_INITIAL(decl)) == STRING_CST) &&
           "Global initializer should be constant!");
    
    // Temporarily set an initializer for the global, so we don't infinitely
    // recurse.  If we don't do this, we can hit cases where we see "oh a global
    // with an initializer hasn't been initialized yet, call emit_global_to_llvm
    // on it".  When constructing the initializer it might refer to itself.
    // this can happen for things like void *G = &G;
    //
    GV->setInitializer(UndefValue::get(GV->getType()->getElementType()));
    GV->setUnnamedAddr(true);
    Init = TreeConstantToLLVM::Convert(DECL_INITIAL(decl));
  }

  // If we had a forward definition that has a type that disagrees with our
  // initializer, insert a cast now.  This sort of thing occurs when we have a
  // global union, and the LLVM type followed a union initializer that is
  // different from the union element used for the type.
  if (GV->getType()->getElementType() != Init->getType()) {
    GV->removeFromParent();
    GlobalVariable *NGV = new GlobalVariable(*TheModule, Init->getType(),
                                             GV->isConstant(),
                                             GlobalValue::ExternalLinkage, 0,
                                             GV->getName());
    NGV->setUnnamedAddr(true);
    GV->replaceAllUsesWith(TheFolder->CreateBitCast(NGV, GV->getType()));
    changeLLVMConstant(GV, NGV);
    delete GV;
    SET_DECL_LLVM(decl, NGV);
    GV = NGV;
  }
 
  // Set the initializer.
  GV->setInitializer(Init);
  GV->setUnnamedAddr(true);

  // Set thread local (TLS)
  if (TREE_CODE(decl) == VAR_DECL && DECL_THREAD_LOCAL_P(decl))
    GV->setThreadLocal(true);

  // Set the linkage.
  GlobalValue::LinkageTypes Linkage;

  if (CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS)
      && DECL_LLVM_PRIVATE(decl)) {
    Linkage = GlobalValue::PrivateLinkage;
  } else if (CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS)
             && DECL_LLVM_LINKER_PRIVATE(decl)) {
    Linkage = GlobalValue::LinkerPrivateLinkage;
  } else if (CODE_CONTAINS_STRUCT (TREE_CODE (decl), TS_DECL_WITH_VIS)
             && DECL_LLVM_LINKER_PRIVATE_WEAK(decl)) {
    Linkage = GlobalValue::LinkerPrivateWeakLinkage;
  } else if (!TREE_PUBLIC(decl)) {
    Linkage = GlobalValue::InternalLinkage;
  } else if (DECL_WEAK(decl)) {
    // The user may have explicitly asked for weak linkage - ignore flag_odr.
    Linkage = GlobalValue::WeakAnyLinkage;
  } else if (DECL_ONE_ONLY(decl)) {
    Linkage = GlobalValue::getWeakLinkage(flag_odr);
  } else if (DECL_COMMON(decl) &&  // DECL_COMMON is only meaningful if no init
             (!DECL_INITIAL(decl) || DECL_INITIAL(decl) == error_mark_node)) {
    // llvm-gcc also includes DECL_VIRTUAL_P here.
    Linkage = GlobalValue::CommonLinkage;
    GV->setConstant(false);
  } else if (DECL_COMDAT(decl)) {
    Linkage = GlobalValue::getLinkOnceLinkage(flag_odr);
  } else {
    Linkage = GV->getLinkage();
  }

  // Allow loads from constants to be folded even if the constant has weak
  // linkage.  Do this by giving the constant weak_odr linkage rather than
  // weak linkage.  It is not clear whether this optimization is valid (see
  // gcc bug 36685), but mainline gcc chooses to do it, and fold may already
  // have done it, so we might as well join in with gusto.
  if (GV->isConstant()) {
    if (Linkage == GlobalValue::WeakAnyLinkage)
      Linkage = GlobalValue::WeakODRLinkage;
    else if (Linkage == GlobalValue::LinkOnceAnyLinkage)
      Linkage = GlobalValue::LinkOnceODRLinkage;
  }
  GV->setLinkage(Linkage);

#ifdef TARGET_ADJUST_LLVM_LINKAGE
  TARGET_ADJUST_LLVM_LINKAGE(GV, decl);
#endif /* TARGET_ADJUST_LLVM_LINKAGE */

  handleVisibility(decl, GV);

  // Set the section for the global.
  if (TREE_CODE(decl) == VAR_DECL) {
    if (DECL_SECTION_NAME(decl)) {
      GV->setSection(TREE_STRING_POINTER(DECL_SECTION_NAME(decl)));
#ifdef LLVM_IMPLICIT_TARGET_GLOBAL_VAR_SECTION
    } else if (const char *Section = 
                LLVM_IMPLICIT_TARGET_GLOBAL_VAR_SECTION(decl)) {
      GV->setSection(Section);
#endif
    }
    
    // Set the alignment for the global if one of the following condition is met
    // 1) DECL_ALIGN is better than the alignment as per ABI specification
    // 2) DECL_ALIGN is set by user.
    if (DECL_ALIGN(decl)) {
      unsigned TargetAlign =
        getTargetData().getABITypeAlignment(GV->getType()->getElementType());
      if (DECL_USER_ALIGN(decl) ||
          8 * TargetAlign < (unsigned)DECL_ALIGN(decl)) {
        GV->setAlignment(DECL_ALIGN(decl) / 8);
      }
#ifdef TARGET_ADJUST_CSTRING_ALIGN
      else if (DECL_INITIAL(decl) != error_mark_node && // uninitialized?
               DECL_INITIAL(decl) &&
               TREE_CODE(DECL_INITIAL(decl)) == STRING_CST) {
        TARGET_ADJUST_CSTRING_ALIGN(GV);
      }
#endif
    }

    // Handle used decls
    if (DECL_PRESERVE_P (decl)) {
      if (DECL_LLVM_LINKER_PRIVATE (decl) ||
          DECL_LLVM_LINKER_PRIVATE_WEAK (decl))
        AttributeCompilerUsedGlobals.insert(GV);
      else
        AttributeUsedGlobals.insert(GV);
    }
  
    // Add annotate attributes for globals
    if (DECL_ATTRIBUTES(decl))
      AddAnnotateAttrsToGlobal(GV, decl);
  
#ifdef LLVM_IMPLICIT_TARGET_GLOBAL_VAR_SECTION
  } else if (TREE_CODE(decl) == CONST_DECL) {
    if (const char *Section = 
        LLVM_IMPLICIT_TARGET_GLOBAL_VAR_SECTION(decl)) {
      GV->setSection(Section);

      /* LLVM LOCAL - begin radar 6389998 */
#ifdef TARGET_ADJUST_CFSTRING_NAME
      TARGET_ADJUST_CFSTRING_NAME(GV, Section);
#endif
      /* LLVM LOCAL - end radar 6389998 */
    }
#endif
  }

  if (TheDebugInfo) {
    std::string Name = GV->getNameStr();
    const char LPrefix[] = "\01L_OBJC_";
    const char lPrefix[] = "\01l_OBJC_";

    if (flag_objc_abi == -1 || flag_objc_abi == 0 ||
        (strncmp(Name.c_str(), LPrefix, sizeof(LPrefix) - 1) != 0 &&
         strncmp(Name.c_str(), lPrefix, sizeof(lPrefix) - 1) != 0))
      TheDebugInfo->EmitGlobalVariable(GV, decl);
  }

  TREE_ASM_WRITTEN(decl) = 1;
  timevar_pop(TV_LLVM_GLOBALS);
}


/// ValidateRegisterVariable - Check that a static "asm" variable is
/// well-formed.  If not, emit error messages and return true.  If so, return
/// false.
bool ValidateRegisterVariable(tree decl) {
  int RegNumber = decode_reg_name(extractRegisterName(decl));
  const Type *Ty = ConvertType(TREE_TYPE(decl));

  if (errorcount || sorrycount)
    return true;  // Do not process broken code.

  /* Detect errors in declaring global registers.  */
  if (RegNumber == -1)
    error("%Jregister name not specified for %qD", decl, decl);
  else if (RegNumber < 0)
    error("%Jinvalid register name for %qD", decl, decl);
  else if (TYPE_MODE(TREE_TYPE(decl)) == BLKmode)
    error("%Jdata type of %qD isn%'t suitable for a register", decl, decl);
#if 0 // FIXME: enable this.
  else if (!HARD_REGNO_MODE_OK(RegNumber, TYPE_MODE(TREE_TYPE(decl))))
    error("%Jregister specified for %qD isn%'t suitable for data type",
          decl, decl);
#endif
  else if (DECL_INITIAL(decl) != 0 && TREE_STATIC(decl))
    error("global register variable has initial value");
  else if (!Ty->isSingleValueType())
    sorry("%JLLVM cannot handle register variable %qD, report a bug",
          decl, decl);
  else {
    if (TREE_THIS_VOLATILE(decl))
      warning(0, "volatile register variables don%'t work as you might wish");

    return false;  // Everything ok.
  }

  return true;
}


/// make_decl_llvm - Create the DECL_RTL for a VAR_DECL or FUNCTION_DECL.  DECL
/// should have static storage duration.  In other words, it should not be an
/// automatic variable, including PARM_DECLs.
///
/// There is, however, one exception: this function handles variables explicitly
/// placed in a particular register by the user.
///
/// This function corresponds to make_decl_rtl in varasm.c, and is implicitly
/// called by DECL_LLVM if a decl doesn't have an LLVM set.
void make_decl_llvm(tree decl) {
#ifdef ENABLE_CHECKING
  // Check that we are not being given an automatic variable.
  // A weak alias has TREE_PUBLIC set but not the other bits.
  if (TREE_CODE(decl) == PARM_DECL || TREE_CODE(decl) == RESULT_DECL
      || (TREE_CODE(decl) == VAR_DECL && !TREE_STATIC(decl) &&
          !TREE_PUBLIC(decl) && !DECL_EXTERNAL(decl) && !DECL_REGISTER(decl)))
    abort();
  // And that we were not given a type or a label.  */
  else if (TREE_CODE(decl) == TYPE_DECL || TREE_CODE(decl) == LABEL_DECL)
    abort ();
#endif

  LLVMContext &Context = getGlobalContext();
  
  // For a duplicate declaration, we can be called twice on the
  // same DECL node.  Don't discard the LLVM already made.
  if (DECL_LLVM_SET_P(decl)) return;

  if (errorcount || sorrycount)
    return;  // Do not process broken code.
  
  
  // Global register variable with asm name, e.g.:
  // register unsigned long esp __asm__("ebp");
  if (TREE_CODE(decl) != FUNCTION_DECL && DECL_REGISTER(decl)) {
    // This  just verifies that the variable is ok.  The actual "load/store"
    // code paths handle accesses to the variable.
    ValidateRegisterVariable(decl);
    return;
  }
  
  timevar_push(TV_LLVM_GLOBALS);

  const char *Name = "";
  if (DECL_NAME(decl))
    if (tree AssemblerName = DECL_ASSEMBLER_NAME(decl))
      Name = IDENTIFIER_POINTER(AssemblerName);
  
  // Now handle ordinary static variables and functions (in memory).
  // Also handle vars declared register invalidly.
  if (Name[0] == 1) {
#ifdef REGISTER_PREFIX
    if (strlen (REGISTER_PREFIX) != 0) {
      int reg_number = decode_reg_name(Name);
      if (reg_number >= 0 || reg_number == -3)
        error("%Jregister name given for non-register variable %qD",
              decl, decl);
    }
#endif
  }
  
  // Specifying a section attribute on a variable forces it into a
  // non-.bss section, and thus it cannot be common.
  if (TREE_CODE(decl) == VAR_DECL && DECL_SECTION_NAME(decl) != NULL_TREE &&
      DECL_INITIAL(decl) == NULL_TREE && DECL_COMMON(decl))
    DECL_COMMON(decl) = 0;
  
  // Variables can't be both common and weak.
  if (TREE_CODE(decl) == VAR_DECL && DECL_WEAK(decl))
    DECL_COMMON(decl) = 0;
  
  // Okay, now we need to create an LLVM global variable or function for this
  // object.  Note that this is quite possibly a forward reference to the
  // object, so its type may change later.
  if (TREE_CODE(decl) == FUNCTION_DECL) {
    assert(Name[0] && "Function with empty name!");
    // If this function has already been created, reuse the decl.  This happens
    // when we have something like __builtin_memset and memset in the same file.
    Function *FnEntry = TheModule->getFunction(Name);
    if (FnEntry == 0) {
      CallingConv::ID CC;
      AttrListPtr PAL;
      const FunctionType *Ty = 
        TheTypeConverter->ConvertFunctionType(TREE_TYPE(decl), decl, NULL,
                                              CC, PAL);
      FnEntry = Function::Create(Ty, Function::ExternalLinkage, Name, TheModule);
      FnEntry->setCallingConv(CC);
      FnEntry->setAttributes(PAL);

      // Check for external weak linkage.
      if (DECL_EXTERNAL(decl) && DECL_WEAK(decl))
        FnEntry->setLinkage(Function::ExternalWeakLinkage);

#ifdef TARGET_ADJUST_LLVM_LINKAGE
      TARGET_ADJUST_LLVM_LINKAGE(FnEntry,decl);
#endif /* TARGET_ADJUST_LLVM_LINKAGE */

      handleVisibility(decl, FnEntry);

      // If FnEntry got renamed, then there is already an object with this name
      // in the symbol table.  If this happens, the old one must be a forward
      // decl, just replace it with a cast of the new one.
      if (FnEntry->getName() != Name) {
        GlobalVariable *G = TheModule->getGlobalVariable(Name, true);
        assert(G && G->isDeclaration() && "A global turned into a function?");

        // Replace any uses of "G" with uses of FnEntry.
        Constant *GInNewType = TheFolder->CreateBitCast(FnEntry, G->getType());
        G->replaceAllUsesWith(GInNewType);

        // Update the decl that points to G.
        changeLLVMConstant(G, GInNewType);

        // Now we can give GV the proper name.
        FnEntry->takeName(G);
        
        // G is now dead, nuke it.
        G->eraseFromParent();
      }
    }
    SET_DECL_LLVM(decl, FnEntry);
  } else {
    assert((TREE_CODE(decl) == VAR_DECL ||
            TREE_CODE(decl) == CONST_DECL) && "Not a function or var decl?");
    const Type *Ty = ConvertType(TREE_TYPE(decl));
    GlobalVariable *GV ;

    // If we have "extern void foo", make the global have type {} instead of
    // type void.
    if (Ty->isVoidTy())
      Ty = StructType::get(Context);

    if (Name[0] == 0) {   // Global has no name.
      GV = new GlobalVariable(*TheModule, Ty, false, 
                              GlobalValue::ExternalLinkage, 0, "");
      GV->setUnnamedAddr(true);

      // Check for external weak linkage.
      if (DECL_EXTERNAL(decl) && DECL_WEAK(decl))
        GV->setLinkage(GlobalValue::ExternalWeakLinkage);

#ifdef TARGET_ADJUST_LLVM_LINKAGE
      TARGET_ADJUST_LLVM_LINKAGE(GV,decl);
#endif /* TARGET_ADJUST_LLVM_LINKAGE */

      handleVisibility(decl, GV);
    } else {
      // If the global has a name, prevent multiple vars with the same name from
      // being created.
      GlobalVariable *GVE = TheModule->getGlobalVariable(Name, true);
    
      if (GVE == 0) {
        GV = new GlobalVariable(*TheModule, Ty, false,
                                GlobalValue::ExternalLinkage, 0, Name);

        GV->setUnnamedAddr(true);
        // Check for external weak linkage.
        if (DECL_EXTERNAL(decl) && DECL_WEAK(decl))
          GV->setLinkage(GlobalValue::ExternalWeakLinkage);

#ifdef TARGET_ADJUST_LLVM_LINKAGE
        TARGET_ADJUST_LLVM_LINKAGE(GV,decl);
#endif /* TARGET_ADJUST_LLVM_LINKAGE */

	handleVisibility(decl, GV);

        // If GV got renamed, then there is already an object with this name in
        // the symbol table.  If this happens, the old one must be a forward
        // decl, just replace it with a cast of the new one.
        if (GV->getName() != Name) {
          Function *F = TheModule->getFunction(Name);
          assert(F && F->isDeclaration() && "A function turned into a global?");
          
          // Replace any uses of "F" with uses of GV.
          Constant *FInNewType = TheFolder->CreateBitCast(GV, F->getType());
          F->replaceAllUsesWith(FInNewType);

          // Update the decl that points to F.
          changeLLVMConstant(F, FInNewType);

          // Now we can give GV the proper name.
          GV->takeName(F);
          
          // F is now dead, nuke it.
          F->eraseFromParent();
        }
        
      } else {
        GV = GVE;  // Global already created, reuse it.
      }
    }

    if ((TREE_READONLY(decl) && !TREE_SIDE_EFFECTS(decl)) ||
        TREE_CODE(decl) == CONST_DECL) {
      if (DECL_EXTERNAL(decl)) {
        // Mark external globals constant even though they could be marked
        // non-constant in the defining translation unit.  The definition of the
        // global determines whether the global is ultimately constant or not,
        // marking this constant will allow us to do some extra (legal)
        // optimizations that we would otherwise not be able to do.  (In C++,
        // any global that is 'C++ const' may not be readonly: it could have a
        // dynamic initializer.
        //
        GV->setConstant(true);
      } else {
        // Mark readonly globals with constant initializers constant.
        if (DECL_INITIAL(decl) != error_mark_node && // uninitialized?
            DECL_INITIAL(decl) &&
            (TREE_CONSTANT(DECL_INITIAL(decl)) ||
             TREE_CODE(DECL_INITIAL(decl)) == STRING_CST))
          GV->setConstant(true);
      }
    }

    // Set thread local (TLS)
    if (TREE_CODE(decl) == VAR_DECL && DECL_THREAD_LOCAL_P(decl))
      GV->setThreadLocal(true);

    SET_DECL_LLVM(decl, GV);
  }
  timevar_pop(TV_LLVM_GLOBALS);
}

/// llvm_get_decl_name - Used by varasm.c, returns the specified declaration's
/// name.
const char *llvm_get_decl_name(void *LLVM) {
  if (LLVM)
    if (const ValueName *VN = ((Value*)LLVM)->getValueName())
      return VN->getKeyData();
  return "";
}

/// llvm_mark_decl_weak - Used by varasm.c, called when a decl is found to be
/// weak, but it already had an llvm object created for it. This marks the LLVM
/// object weak as well.
void llvm_mark_decl_weak(tree decl) {
  assert(DECL_LLVM_SET_P(decl) && DECL_WEAK(decl) &&
         isa<GlobalValue>(DECL_LLVM(decl)) && "Decl isn't marked weak!");
  GlobalValue *GV = cast<GlobalValue>(DECL_LLVM(decl));

  // Do not mark something that is already known to be linkonce or internal.
  // The user may have explicitly asked for weak linkage - ignore flag_odr.
  if (GV->hasExternalLinkage()) {
    GlobalValue::LinkageTypes Linkage;
    if (GV->isDeclaration()) {
      Linkage = GlobalValue::ExternalWeakLinkage;
    } else {
      Linkage = GlobalValue::WeakAnyLinkage;
      // Allow loads from constants to be folded even if the constant has weak
      // linkage.  Do this by giving the constant weak_odr linkage rather than
      // weak linkage.  It is not clear whether this optimization is valid (see
      // gcc bug 36685), but mainline gcc chooses to do it, and fold may already
      // have done it, so we might as well join in with gusto.
      if (GlobalVariable *GVar = dyn_cast<GlobalVariable>(GV))
        if (GVar->isConstant())
          Linkage = GlobalValue::WeakODRLinkage;
    }
    GV->setLinkage(Linkage);
  }
}

/// llvm_emit_ctor_dtor - Called to emit static ctors/dtors to LLVM code.
/// fndecl is a 'void()' FUNCTION_DECL for the code, initprio is the init
/// priority, and isCtor indicates whether this is a ctor or dtor.
void llvm_emit_ctor_dtor(tree FnDecl, int InitPrio, int isCtor) {
  mark_decl_referenced(FnDecl);  // Inform cgraph that we used the global.

  if (errorcount || sorrycount) return;

  Constant *C = cast<Constant>(DECL_LLVM(FnDecl));
  (isCtor ? &StaticCtors:&StaticDtors)->push_back(std::make_pair(C, InitPrio));
}

void llvm_emit_typedef(tree decl) {
  // Need hooks for debug info?
  return;
}

/// llvm_emit_file_scope_asm - Emit the specified string as a file-scope inline
/// asm block.
void llvm_emit_file_scope_asm(const char *string) {
  if (TheModule->getModuleInlineAsm().empty())
    TheModule->setModuleInlineAsm(string);
  else
    TheModule->setModuleInlineAsm(TheModule->getModuleInlineAsm() + "\n" +
                                  string);
}

/// print_llvm - Print the specified LLVM chunk like an operand, called by
/// print-tree.c for tree dumps.
void print_llvm(FILE *file, void *LLVM) {
  // FIXME: oFILEstream can probably be removed in favor of a new raw_ostream
  // adaptor which would be simpler and more efficient.  In the meantime, just
  // adapt the adaptor.
  oFILEstream FS(file);
  raw_os_ostream FSS(FS);
  FSS << "LLVM: ";
  WriteAsOperand(FSS, (Value*)LLVM, true, TheModule);
}

/// print_llvm_type - Print the specified LLVM type symbolically, called by
/// print-tree.c for tree dumps.
void print_llvm_type(FILE *file, void *LLVM) {
  oFILEstream FS(file);
  FS << "LLVM: ";
  
  // FIXME: oFILEstream can probably be removed in favor of a new raw_ostream
  // adaptor which would be simpler and more efficient.  In the meantime, just
  // adapt the adaptor.
  raw_os_ostream RO(FS);
  WriteTypeSymbolic(RO, (const Type*)LLVM, TheModule);
}

/// extractRegisterName - Get a register name given its decl. In 4.2 unlike 4.0
/// these names have been run through set_user_assembler_name which means they
/// may have a leading \1 at this point; compensate.
const char* extractRegisterName(tree decl) {
  const char* Name = IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(decl));
  return (*Name == 1) ? Name + 1 : Name;
}
/* LLVM LOCAL end (ENTIRE FILE!)  */
