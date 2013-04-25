/* LLVM LOCAL begin (ENTIRE FILE!) */
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

//===----------------------------------------------------------------------===//
// This is the code that converts GCC AST nodes into LLVM code.
//===----------------------------------------------------------------------===//

#include "llvm/ValueSymbolTable.h"
#include "llvm/CallingConv.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InlineAsm.h"
#include "llvm/Instructions.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/DenseMap.h"

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm-debug.h"

extern "C" {
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tm_p.h"
#include "tree.h"
#include "c-tree.h"  // FIXME: eliminate.
#include "tree-iterator.h"
#include "output.h"
#include "diagnostic.h"
#include "real.h"
#include "langhooks.h"
#include "function.h"
#include "toplev.h"
#include "flags.h"
#include "target.h"
#include "hard-reg-set.h"
#include "except.h"
#include "rtl.h"
#include "libfuncs.h"
#include "tree-flow.h"
#include "tree-gimple.h"
extern int get_pointer_alignment (tree exp, unsigned int max_align);
extern enum machine_mode reg_raw_mode[FIRST_PSEUDO_REGISTER];
}

static LLVMContext &Context = getGlobalContext();

extern std::vector<tree> Thunks;

// Check for GCC bug 17347: C++ FE sometimes creates bogus ctor trees
// which we should throw out
#define BOGUS_CTOR(exp)                                                \
  (DECL_INITIAL(exp) &&                                                \
   TREE_CODE(DECL_INITIAL(exp)) == CONSTRUCTOR &&                      \
   !TREE_TYPE(DECL_INITIAL(exp)))

/// isGimpleTemporary - Return true if this is a gimple temporary that we can
/// directly compile into an LLVM temporary.  This saves us from creating an
/// alloca and creating loads/stores of that alloca (a compile-time win).  We
/// can only do this if the value is a first class llvm value and if it's a
/// "gimple_formal_tmp_reg".
static bool isGimpleTemporary(tree decl) {
  return is_gimple_formal_tmp_reg(decl) &&
        !isAggregateTreeType(TREE_TYPE(decl));
}

/// getINTEGER_CSTVal - Return the specified INTEGER_CST value as a uint64_t.
///
uint64_t getINTEGER_CSTVal(tree exp) {
  unsigned HOST_WIDE_INT HI = (unsigned HOST_WIDE_INT)TREE_INT_CST_HIGH(exp);
  unsigned HOST_WIDE_INT LO = (unsigned HOST_WIDE_INT)TREE_INT_CST_LOW(exp);
  if (HOST_BITS_PER_WIDE_INT == 64) {
    return (uint64_t)LO;
  } else {
    assert(HOST_BITS_PER_WIDE_INT == 32 &&
           "Only 32- and 64-bit hosts supported!");
    return ((uint64_t)HI << 32) | (uint64_t)LO;
  }
}

/// isInt64 - Return true if t is an INTEGER_CST that fits in a 64 bit integer.
/// If Unsigned is false, returns whether it fits in a int64_t.  If Unsigned is
/// true, returns whether the value is non-negative and fits in a uint64_t.
/// Always returns false for overflowed constants.
bool isInt64(tree t, bool Unsigned) {
  if (HOST_BITS_PER_WIDE_INT == 64)
    return host_integerp(t, Unsigned) && !TREE_OVERFLOW (t);
  else {
    assert(HOST_BITS_PER_WIDE_INT == 32 &&
           "Only 32- and 64-bit hosts supported!");
    return
      (TREE_CODE (t) == INTEGER_CST && !TREE_OVERFLOW (t))
      && ((TYPE_UNSIGNED(TREE_TYPE(t)) == Unsigned) ||
          // If the constant is signed and we want an unsigned result, check
          // that the value is non-negative.  If the constant is unsigned and
          // we want a signed result, check it fits in 63 bits.
          (HOST_WIDE_INT)TREE_INT_CST_HIGH(t) >= 0);
  }
}

/// getInt64 - Extract the value of an INTEGER_CST as a 64 bit integer.  If
/// Unsigned is false, the value must fit in a int64_t.  If Unsigned is true,
/// the value must be non-negative and fit in a uint64_t.  Must not be used on
/// overflowed constants.  These conditions can be checked by calling isInt64.
uint64_t getInt64(tree t, bool Unsigned) {
  assert(isInt64(t, Unsigned) && "invalid constant!");
  return getINTEGER_CSTVal(t);
}

/// getPointerAlignment - Return the alignment in bytes of exp, a pointer valued
/// expression, or 1 if the alignment is not known.
unsigned int getPointerAlignment(tree exp) {
  assert(POINTER_TYPE_P (TREE_TYPE (exp)) && "Expected a pointer type!");
  unsigned int align = get_pointer_alignment(exp, BIGGEST_ALIGNMENT) / 8;
  return align ? align : 1;
}

//===----------------------------------------------------------------------===//
//                         ... High-Level Methods ...
//===----------------------------------------------------------------------===//

/// TheTreeToLLVM - Keep track of the current function being compiled.
TreeToLLVM *TheTreeToLLVM = 0;

const TargetData &getTargetData() {
  return *TheTarget->getTargetData();
}

/// EmitDebugInfo - Return true if debug info is to be emitted for current 
/// function.
bool TreeToLLVM::EmitDebugInfo() {
  if (TheDebugInfo && getFUNCTION_DECL() && !DECL_IGNORED_P(getFUNCTION_DECL()))
    return true;
  return false;
}

TreeToLLVM::TreeToLLVM(tree fndecl) :
    TD(getTargetData()), Builder(Context, *TheFolder) {
  FnDecl = fndecl;
  Fn = 0;
  ReturnBB = UnwindBB = 0;
  ReturnOffset = 0;

  if (EmitDebugInfo()) {
    expanded_location Location = expand_location(DECL_SOURCE_LOCATION (fndecl));

    if (Location.file) {
      TheDebugInfo->setLocationFile(Location.file);
      TheDebugInfo->setLocationLine(Location.line);
    } else {
      TheDebugInfo->setLocationFile("<unknown file>");
      TheDebugInfo->setLocationLine(0);
    }
  }

  AllocaInsertionPoint = 0;
  GreatestAlignment = TheTarget->getFrameLowering()->getStackAlignment();
  SeenVLA = NULL;

  CatchAll = 0;
  ExceptionValue = 0;
  ExceptionSelectorValue = 0;
  FuncEHException = 0;
  FuncEHSelector = 0;
  FuncEHGetTypeID = 0;

  assert(TheTreeToLLVM == 0 && "Reentering function creation?");
  TheTreeToLLVM = this;
}

TreeToLLVM::~TreeToLLVM() {
  TheTreeToLLVM = 0;
}

/// getLabelDeclBlock - Lazily get and create a basic block for the specified
/// label.
static BasicBlock *getLabelDeclBlock(tree LabelDecl) {
  assert(TREE_CODE(LabelDecl) == LABEL_DECL && "Isn't a label!?");
  if (DECL_LLVM_SET_P(LabelDecl))
    return cast<BasicBlock>(DECL_LLVM(LabelDecl));

  const char *Name = "bb";
  if (DECL_NAME(LabelDecl))
    Name = IDENTIFIER_POINTER(DECL_NAME(LabelDecl));

  BasicBlock *NewBB = BasicBlock::Create(Context, Name);
  SET_DECL_LLVM(LabelDecl, NewBB);
  return NewBB;
}

/// llvm_store_scalar_argument - Store scalar argument ARGVAL of type
/// LLVMTY at location LOC.
static void llvm_store_scalar_argument(Value *Loc, Value *ArgVal,
                                       const llvm::Type *LLVMTy,
                                       unsigned RealSize,
                                       LLVMBuilder &Builder) {
  if (RealSize) {
    // Not clear what this is supposed to do on big endian machines...
    assert(!BYTES_BIG_ENDIAN && "Unsupported case - please report");
    // Do byte wise store because actual argument type does not match LLVMTy.
    assert(ArgVal->getType()->isIntegerTy() && "Expected an integer value!");
    const Type *StoreType = IntegerType::get(Context, RealSize * 8);
    Loc = Builder.CreateBitCast(Loc, StoreType->getPointerTo());
    if (ArgVal->getType()->getPrimitiveSizeInBits() >=
        StoreType->getPrimitiveSizeInBits())
      ArgVal = Builder.CreateTrunc(ArgVal, StoreType);
    else
      ArgVal = Builder.CreateZExt(ArgVal, StoreType);
    Builder.CreateStore(ArgVal, Loc);
  } else {
    // This cast only involves pointers, therefore BitCast.
    Loc = Builder.CreateBitCast(Loc, LLVMTy->getPointerTo());
    Builder.CreateStore(ArgVal, Loc);
  }
}

#ifndef LLVM_STORE_SCALAR_ARGUMENT
#define LLVM_STORE_SCALAR_ARGUMENT(LOC,ARG,TYPE,SIZE,BUILDER)   \
  llvm_store_scalar_argument((LOC),(ARG),(TYPE),(SIZE),(BUILDER))
#endif

// This is true for types whose alignment when passed on the stack is less
// than the alignment of the type.
#define LLVM_BYVAL_ALIGNMENT_TOO_SMALL(T) \
   (LLVM_BYVAL_ALIGNMENT(T) && LLVM_BYVAL_ALIGNMENT(T) < TYPE_ALIGN_UNIT(T))

namespace {
  /// FunctionPrologArgumentConversion - This helper class is driven by the ABI
  /// definition for this target to figure out how to retrieve arguments from
  /// the stack/regs coming into a function and store them into an appropriate
  /// alloca for the argument.
  struct FunctionPrologArgumentConversion : public DefaultABIClient {
    tree FunctionDecl;
    Function::arg_iterator &AI;
    LLVMBuilder Builder;
    std::vector<Value*> LocStack;
    std::vector<std::string> NameStack;
    unsigned Offset;
    CallingConv::ID &CallingConv;
    bool isShadowRet;
    FunctionPrologArgumentConversion(tree FnDecl,
                                     Function::arg_iterator &ai,
                                     const LLVMBuilder &B, CallingConv::ID &CC)
      : FunctionDecl(FnDecl), AI(ai), Builder(B), Offset(0), CallingConv(CC),
        isShadowRet(false) {}

    /// getCallingConv - This provides the desired CallingConv for the function.
    CallingConv::ID& getCallingConv(void) { return CallingConv; }

    void HandlePad(const llvm::Type *LLVMTy) {
      ++AI;
    }

    bool isShadowReturn() const {
      return isShadowRet;
    }
    void setName(const std::string &Name) {
      NameStack.push_back(Name);
    }
    void setLocation(Value *Loc) {
      LocStack.push_back(Loc);
    }
    void clear() {
      assert(NameStack.size() == 1 && LocStack.size() == 1 && "Imbalance!");
      NameStack.clear();
      LocStack.clear();
    }

    void HandleAggregateShadowResult(const PointerType *PtrArgTy,
                                       bool RetPtr) {
      // If the function returns a structure by value, we transform the function
      // to take a pointer to the result as the first argument of the function
      // instead.
      assert(AI != Builder.GetInsertBlock()->getParent()->arg_end() &&
             "No explicit return value?");
      AI->setName("agg.result");

      isShadowRet = true;
      tree ResultDecl = DECL_RESULT(FunctionDecl);
      tree RetTy = TREE_TYPE(TREE_TYPE(FunctionDecl));
      if (TREE_CODE(RetTy) == TREE_CODE(TREE_TYPE(ResultDecl))) {
        SET_DECL_LLVM(ResultDecl, AI);
        ++AI;
        return;
      }

      // Otherwise, this must be something returned with NRVO.
      assert(TREE_CODE(TREE_TYPE(ResultDecl)) == REFERENCE_TYPE &&
             "Not type match and not passing by reference?");
      // Create an alloca for the ResultDecl.
      Value *Tmp = TheTreeToLLVM->CreateTemporary(AI->getType());
      Builder.CreateStore(AI, Tmp);

      SET_DECL_LLVM(ResultDecl, Tmp);
      if (TheDebugInfo && !DECL_IGNORED_P(FunctionDecl)) {
        TheDebugInfo->EmitDeclare(ResultDecl,
                                  dwarf::DW_TAG_return_variable,
                                  "agg.result", RetTy, Tmp,
                                  Builder);
      }
      ++AI;
    }

    void HandleScalarShadowResult(const PointerType *PtrArgTy, bool RetPtr) {
      assert(AI != Builder.GetInsertBlock()->getParent()->arg_end() &&
             "No explicit return value?");
      AI->setName("scalar.result");
      isShadowRet = true;
      SET_DECL_LLVM(DECL_RESULT(FunctionDecl), AI);
      ++AI;
    }

    void HandleScalarArgument(const llvm::Type *LLVMTy, tree type,
                              unsigned RealSize = 0) {
      Value *ArgVal = AI;
      LLVMTy = LLVM_ADJUST_MMX_PARAMETER_TYPE(LLVMTy);
      if (ArgVal->getType() != LLVMTy) {
        if (ArgVal->getType()->isPointerTy() && LLVMTy->isPointerTy()) {
          // If this is GCC being sloppy about pointer types, insert a bitcast.
          // See PR1083 for an example.
          ArgVal = Builder.CreateBitCast(ArgVal, LLVMTy);
        } else if (ArgVal->getType()->isDoubleTy()) {
          // If this is a K&R float parameter, it got promoted to double. Insert
          // the truncation to float now.
          ArgVal = Builder.CreateFPTrunc(ArgVal, LLVMTy,
                                         NameStack.back().c_str());
        } else {
          // If this is just a mismatch between integer types, this is due
          // to K&R prototypes, where the forward proto defines the arg as int
          // and the actual impls is a short or char.
          assert(ArgVal->getType()->isIntegerTy(32) && LLVMTy->isIntegerTy() &&
                 "Lowerings don't match?");
          ArgVal = Builder.CreateTrunc(ArgVal, LLVMTy,NameStack.back().c_str());
        }
      }
      assert(!LocStack.empty());
      Value *Loc = LocStack.back();
      LLVM_STORE_SCALAR_ARGUMENT(Loc,ArgVal,LLVMTy,RealSize,Builder);
      AI->setName(NameStack.back());
      ++AI;
    }

    void HandleByValArgument(const llvm::Type *LLVMTy, tree type) {
      if (LLVM_BYVAL_ALIGNMENT_TOO_SMALL(type)) {
        // Incoming object on stack is insufficiently aligned for the type.
        // Make a correctly aligned copy.
        assert(!LocStack.empty());
        Value *Loc = LocStack.back();
        // We cannot use field-by-field copy here; x86 long double is 16
        // bytes, but only 10 are copied.  If the object is really a union
        // we might need the other bytes.  We must also be careful to use
        // the smaller alignment.
        const Type *SBP = Type::getInt8PtrTy(Context);
        const Type *IntPtr = getTargetData().getIntPtrType(Context);
        Value *Ops[5] = {
          Builder.CreateCast(Instruction::BitCast, Loc, SBP),
          Builder.CreateCast(Instruction::BitCast, AI, SBP),
          ConstantInt::get(IntPtr,
                           TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type))),
          ConstantInt::get(Type::getInt32Ty(Context), 
                           LLVM_BYVAL_ALIGNMENT(type)),
          ConstantInt::get(Type::getInt1Ty(Context), false)
        };
        const Type *ArgTypes[3] = {SBP, SBP, IntPtr };
        Builder.CreateCall(Intrinsic::getDeclaration(TheModule, 
                                                     Intrinsic::memcpy,
                                                     ArgTypes, 3), Ops, Ops+5);

        AI->setName(NameStack.back());
      }
      ++AI;
    }

    void HandleFCAArgument(const llvm::Type *LLVMTy, tree /*type*/) {
      // Store the FCA argument into alloca.
      assert(!LocStack.empty());
      Value *Loc = LocStack.back();
      Builder.CreateStore(AI, Loc);
      AI->setName(NameStack.back());
      ++AI;
    }

    void HandleAggregateResultAsScalar(const Type *ScalarTy, unsigned Offset=0){
      this->Offset = Offset;
    }

    void EnterField(unsigned FieldNo, const llvm::Type *StructTy) {
      NameStack.push_back(NameStack.back()+"."+utostr(FieldNo));

      Value *Loc = LocStack.back();
      // This cast only involves pointers, therefore BitCast.
      Loc = Builder.CreateBitCast(Loc, StructTy->getPointerTo());

      Loc = Builder.CreateStructGEP(Loc, FieldNo);
      LocStack.push_back(Loc);
    }
    void ExitField() {
      NameStack.pop_back();
      LocStack.pop_back();
    }
  };
}

// isPassedByVal - Return true if an aggregate of the specified type will be
// passed in memory byval.
static bool isPassedByVal(tree type, const Type *Ty,
                          std::vector<const Type*> &ScalarArgs,
                          CallingConv::ID &CC) {
  if (LLVM_SHOULD_PASS_AGGREGATE_USING_BYVAL_ATTR(type, Ty))
    return true;

  std::vector<const Type*> Args;
  if (LLVM_SHOULD_PASS_AGGREGATE_IN_MIXED_REGS(type, Ty, CC, Args) &&
      LLVM_AGGREGATE_PARTIALLY_PASSED_IN_REGS(Args, ScalarArgs,
                                              CC))
    // We want to pass the whole aggregate in registers but only some of the
    // registers are available.
    return true;
  return false;
}

/// LanguageIsC - Return true if we are compiling C or Objective-C.
static bool LanguageIsC() {
  // If we've already determined this, return it.
  static unsigned Val = 2;
  if (Val != 2) return (bool)Val;

  StringRef LanguageName = lang_hooks.name;

  if (LanguageName == "GNU C" || LanguageName == "GNU Objective-C")
    return (Val = true);
  return (Val = false);
}

// Walk the GCC BLOCK() tree.  Set BLOCK_NUMBER() to the depth of each
// block; this is necessary for lexical block debug info.  Visit all
// the BLOCK_VARS(), and add them to the set s.  Since the
// unexpanded_var_list seems to be a superset of all the scoped
// variables in every lexical BLOCK(), this facilitates allocating the
// scoped variables in their blocks, and the rest at the outermost
// scope of the function.
void TreeToLLVM::setLexicalBlockDepths(tree t, treeset &s, unsigned level) {
  tree bstep, step;
  switch (TREE_CODE(t)) {
  default:
    abort();
  case BLOCK:
    for (bstep = t; bstep; bstep = TREE_CHAIN(bstep)) {
      BLOCK_NUMBER(bstep) = level;
      for (step = BLOCK_VARS(t); step; step = TREE_CHAIN(step))
        s.insert(step);
    }
    for (bstep = BLOCK_SUBBLOCKS(t); bstep; bstep = TREE_CHAIN(bstep))
      setLexicalBlockDepths(bstep, s, level+1);
    return;
  case FUNCTION_DECL:
    return setLexicalBlockDepths(DECL_INITIAL(t), s, level);
  }
}

void TreeToLLVM::StartFunctionBody() {
  const char *Name = "";
  // Get the name of the function.
  if (tree ID = DECL_ASSEMBLER_NAME(FnDecl))
    Name = IDENTIFIER_POINTER(ID);

  // Determine the FunctionType and calling convention for this function.
  tree static_chain = cfun->static_chain_decl;
  const FunctionType *FTy;
  CallingConv::ID CallingConv;
  AttrListPtr PAL;

  // If the function has no arguments and is varargs (...), turn it into a
  // non-varargs function by scanning the param list for the function.  This
  // allows C functions declared as "T foo() {}" to be treated like
  // "T foo(void) {}" and allows us to handle functions with K&R-style
  // definitions correctly.
  //
  // Note that we only do this in C/Objective-C.  Doing this in C++ for
  // functions explicitly declared as taking (...) is bad.
  if (TYPE_ARG_TYPES(TREE_TYPE(FnDecl)) == 0 && LanguageIsC()) {
    FTy = TheTypeConverter->ConvertArgListToFnType(TREE_TYPE(FnDecl),
                                                   DECL_ARGUMENTS(FnDecl),
                                                   static_chain,
                                                   CallingConv, PAL);
  } else {
    // Otherwise, just get the type from the function itself.
    FTy = TheTypeConverter->ConvertFunctionType(TREE_TYPE(FnDecl),
                                                FnDecl,
                                                static_chain,
                                                CallingConv, PAL);
  }

  // If we've already seen this function and created a prototype, and if the
  // proto has the right LLVM type, just use it.
  if (DECL_LLVM_SET_P(FnDecl) &&
      cast<PointerType>(DECL_LLVM(FnDecl)->getType())->getElementType() == FTy){
    Fn = cast<Function>(DECL_LLVM(FnDecl));
    assert(Fn->getCallingConv() == CallingConv &&
           "Calling convention disagreement between prototype and impl!");
    // The visibility can be changed from the last time we've seen this
    // function. Set to current.
    handleVisibility(FnDecl, Fn);
  } else {
    Function *FnEntry = TheModule->getFunction(Name);
    if (FnEntry) {
      assert(FnEntry->getName() == Name && "Same entry, different name?");
      assert((FnEntry->isDeclaration() ||
              FnEntry->getLinkage() == Function::AvailableExternallyLinkage) &&
             "Multiple fns with same name and neither are external!");
      FnEntry->setName("");  // Clear name to avoid conflicts.
      assert(FnEntry->getCallingConv() == CallingConv &&
             "Calling convention disagreement between prototype and impl!");
    }

    // Otherwise, either it exists with the wrong type or it doesn't exist.  In
    // either case create a new function.
    Fn = Function::Create(FTy, Function::ExternalLinkage, Name, TheModule);
    assert(Fn->getName() == Name && "Preexisting fn with the same name!");
    Fn->setCallingConv(CallingConv);
    Fn->setAttributes(PAL);

    // If a previous proto existed with the wrong type, replace any uses of it
    // with the actual function and delete the proto.
    if (FnEntry) {
      FnEntry->replaceAllUsesWith(
        Builder.getFolder().CreateBitCast(Fn, FnEntry->getType())
      );
      changeLLVMConstant(FnEntry, Fn);
      FnEntry->eraseFromParent();
    }
    SET_DECL_LLVM(FnDecl, Fn);
  }

  // The function should not already have a body.
  assert(Fn->empty() && "Function expanded multiple times!");

  if (!lang_hooks.function_is_thunk_p (FnDecl)) {
    // Compute the linkage that the function should get.
    if (DECL_LLVM_PRIVATE(FnDecl)) {
      Fn->setLinkage(Function::PrivateLinkage);
    } else if (DECL_LLVM_LINKER_PRIVATE(FnDecl)) {
      Fn->setLinkage(Function::LinkerPrivateLinkage);
    } else if (!TREE_PUBLIC(FnDecl) /*|| lang_hooks.llvm_is_in_anon(subr)*/) {
      Fn->setLinkage(Function::InternalLinkage);
    } else if (DECL_EXTERNAL(FnDecl) && 
               lookup_attribute ("always_inline", DECL_ATTRIBUTES (FnDecl))) {
      Fn->setLinkage(Function::AvailableExternallyLinkage);
    } else if (DECL_COMDAT(FnDecl)) {
      Fn->setLinkage(Function::getLinkOnceLinkage(flag_odr));
    } else if (DECL_WEAK(FnDecl)) {
      // The user may have explicitly asked for weak linkage - ignore flag_odr.
      Fn->setLinkage(Function::WeakAnyLinkage);
    } else if (DECL_ONE_ONLY(FnDecl) || 
               lang_hooks.function_is_thunk_p (FnDecl)) {
      Fn->setLinkage(Function::getWeakLinkage(flag_odr));
    } else if (IS_EXTERN_INLINE(FnDecl)) {
      // gcc "extern inline", C99 "inline"
      Fn->setLinkage(Function::AvailableExternallyLinkage);
    }

#ifdef TARGET_ADJUST_LLVM_LINKAGE
    TARGET_ADJUST_LLVM_LINKAGE(Fn, FnDecl);
#endif /* TARGET_ADJUST_LLVM_LINKAGE */

    // Handle visibility style.
    handleVisibility(FnDecl, Fn);
  } else {
    // A thunk should get its visibility and linkage from the function being
    // thunked. Set the information after all functions have been processed
    // through here.
    Thunks.push_back(FnDecl);
  }

  // Handle attribute "aligned".
  if (DECL_ALIGN (FnDecl) != FUNCTION_BOUNDARY)
    Fn->setAlignment(DECL_ALIGN (FnDecl) / 8);

  // Handle functions in specified sections.
  if (DECL_SECTION_NAME(FnDecl))
    Fn->setSection(TREE_STRING_POINTER(DECL_SECTION_NAME(FnDecl)));

  // Handle used Functions
  if (lookup_attribute ("used", DECL_ATTRIBUTES (FnDecl)))
    AttributeUsedGlobals.insert(Fn);

  // Handle noinline Functions
  if (lookup_attribute ("noinline", DECL_ATTRIBUTES (FnDecl)))
    Fn->addFnAttr(Attribute::NoInline);

  // Handle always_inline attribute
  if (lookup_attribute ("always_inline", DECL_ATTRIBUTES (FnDecl)))
    Fn->addFnAttr(Attribute::AlwaysInline);

  // asm functions are implicitly naked
  if (DECL_IASM_ASM_FUNCTION (FnDecl) == 1)
    Fn->addFnAttr(Attribute::Naked);

  // Pass inline keyword to optimizer.
  if (DECL_EXPLICIT_INLINE_P (FnDecl))
    Fn->addFnAttr(Attribute::InlineHint);

  if (optimize_size)
    Fn->addFnAttr(Attribute::OptimizeForSize);

  // Handle stack smashing protection.
  if (flag_stack_protect == 1)
    Fn->addFnAttr(Attribute::StackProtect);
  else if (flag_stack_protect == 2)
    Fn->addFnAttr(Attribute::StackProtectReq);

  // Handle naked attribute
  if (lookup_attribute ("naked", DECL_ATTRIBUTES (FnDecl)))
    Fn->addFnAttr(Attribute::Naked);

  // Handle annotate attributes
  if (DECL_ATTRIBUTES(FnDecl))
    AddAnnotateAttrsToGlobal(Fn, FnDecl);

  // Mark the function "nounwind" if not doing exception handling.
  if (!flag_exceptions)
    Fn->setDoesNotThrow();

  // Create a new basic block for the function.
  Builder.SetInsertPoint(BasicBlock::Create(Context, "entry", Fn));

  treeset block_declared_vars;
  // Set the BLOCK_NUMBER()s to the depth of each lexical block.
  setLexicalBlockDepths(FnDecl, block_declared_vars, 1);

  SeenBlocks.clear();

  if (EmitDebugInfo())
    TheDebugInfo->EmitFunctionStart(FnDecl);

  // Loop over all of the arguments to the function, setting Argument names and
  // creating argument alloca's for the PARM_DECLs in case their address is
  // exposed.
  Function::arg_iterator AI = Fn->arg_begin();

  // Rename and alloca'ify real arguments.
  FunctionPrologArgumentConversion Client(FnDecl, AI, Builder, CallingConv);
  DefaultABI ABIConverter(Client);

  // Scalar arguments processed so far.
  std::vector<const Type*> ScalarArgs;

  // Handle the DECL_RESULT.
  ABIConverter.HandleReturnType(TREE_TYPE(TREE_TYPE(FnDecl)), FnDecl,
                                DECL_BUILT_IN(FnDecl),
                                ScalarArgs);
  // Remember this for use by FinishFunctionBody.
  ReturnOffset = Client.Offset;

  // Prepend the static chain (if any) to the list of arguments.
  tree Args = static_chain ? static_chain : DECL_ARGUMENTS(FnDecl);

  while (Args) {
    const char *Name = "unnamed_arg";
    if (DECL_NAME(Args)) Name = IDENTIFIER_POINTER(DECL_NAME(Args));

    const Type *ArgTy = ConvertType(TREE_TYPE(Args));
    bool isInvRef = isPassedByInvisibleReference(TREE_TYPE(Args));
    if (isInvRef ||
        (ArgTy->isVectorTy() &&
         LLVM_SHOULD_PASS_VECTOR_USING_BYVAL_ATTR(TREE_TYPE(Args)) &&
         !LLVM_BYVAL_ALIGNMENT_TOO_SMALL(TREE_TYPE(Args))) ||
        (!ArgTy->isSingleValueType() &&
         isPassedByVal(TREE_TYPE(Args), ArgTy, ScalarArgs,
                       CallingConv) &&
         !LLVM_BYVAL_ALIGNMENT_TOO_SMALL(TREE_TYPE(Args)))) {
      // If the value is passed by 'invisible reference' or 'byval reference',
      // the l-value for the argument IS the argument itself.  But for byval
      // arguments whose alignment as an argument is less than the normal
      // alignment of the type (examples are x86-32 aggregates containing long
      // double and large x86-64 vectors), we need to make the copy.
      AI->setName(Name);
      SET_DECL_LLVM(Args, AI);
      if (!isInvRef && EmitDebugInfo())
        TheDebugInfo->EmitDeclare(Args, dwarf::DW_TAG_arg_variable,
                                  Name, TREE_TYPE(Args),
                                  AI, Builder);
      ABIConverter.HandleArgument(TREE_TYPE(Args), ScalarArgs);
    } else {
      // Otherwise, we create an alloca to hold the argument value and provide
      // an l-value.  On entry to the function, we copy formal argument values
      // into the alloca.
      Value *Tmp = CreateTemporary(ArgTy, TYPE_ALIGN_UNIT(TREE_TYPE(Args)));
      Tmp->setName(std::string(Name)+"_addr");
      SET_DECL_LLVM(Args, Tmp);
      if (EmitDebugInfo())
        TheDebugInfo->EmitDeclare(Args, dwarf::DW_TAG_arg_variable,
                                  Name, TREE_TYPE(Args), Tmp,
                                  Builder);

      // Emit annotate intrinsic if arg has annotate attr
      if (DECL_ATTRIBUTES(Args))
        EmitAnnotateIntrinsic(Tmp, Args);

      // Emit gcroot intrinsic if arg has attribute
      if (POINTER_TYPE_P(TREE_TYPE(Args))
          && lookup_attribute ("gcroot", TYPE_ATTRIBUTES(TREE_TYPE(Args))))
        EmitTypeGcroot(Tmp, Args);

      Client.setName(Name);
      Client.setLocation(Tmp);
      ABIConverter.HandleArgument(TREE_TYPE(Args), ScalarArgs);
      Client.clear();
    }

    Args = Args == static_chain ? DECL_ARGUMENTS(FnDecl) : TREE_CHAIN(Args);
  }

  // If this is not a void-returning function, initialize the RESULT_DECL.
  if (DECL_RESULT(FnDecl) && !VOID_TYPE_P(TREE_TYPE(DECL_RESULT(FnDecl))) &&
      !DECL_LLVM_SET_P(DECL_RESULT(FnDecl)))
    EmitAutomaticVariableDecl(DECL_RESULT(FnDecl));

  // If this function has nested functions, we should handle a potential
  // nonlocal_goto_save_area.
  if (cfun->nonlocal_goto_save_area) {
    // Not supported yet.
  }

  if (EmitDebugInfo())
    TheDebugInfo->EmitStopPoint(Fn, Builder.GetInsertBlock(), Builder);

  // As it turns out, not all temporaries are associated with blocks.  For those
  // that aren't, emit them now.
  for (tree t = cfun->unexpanded_var_list; t; t = TREE_CHAIN(t)) {
    // If this variable hasn't been emitted into LLVM yet, AND it is
    // not part of any BLOCK, emit it now.
    if (!DECL_LLVM_SET_P(TREE_VALUE(t)) &&
        block_declared_vars.count(TREE_VALUE(t)) == 0)
      EmitAutomaticVariableDecl(TREE_VALUE(t));
  }

  // Push the outermost lexical block onto the RegionStack.
  switchLexicalBlock(DECL_INITIAL(FnDecl));

  // Create a new block for the return node, but don't insert it yet.
  ReturnBB = BasicBlock::Create(Context, "return");
}

Function *TreeToLLVM::FinishFunctionBody() {
  // Insert the return block at the end of the function.
  EmitBlock(ReturnBB);

  SmallVector <Value *, 4> RetVals;

  // If the function returns a value, get it into a register and return it now.
  if (Fn->getReturnType() != Type::getVoidTy(Context)) {
    if (!isAggregateTreeType(TREE_TYPE(DECL_RESULT(FnDecl)))) {
      // If the DECL_RESULT is a scalar type, just load out the return value
      // and return it.
      tree TreeRetVal = DECL_RESULT(FnDecl);
      Value *RetVal = Builder.CreateLoad(DECL_LLVM(TreeRetVal), "retval");
      bool RetValSigned = !TYPE_UNSIGNED(TREE_TYPE(TreeRetVal));
      Instruction::CastOps opcode = CastInst::getCastOpcode(
          RetVal, RetValSigned, Fn->getReturnType(), RetValSigned);
      RetVal = CastToType(opcode, RetVal, Fn->getReturnType());
      RetVals.push_back(RetVal);
    } else {
      Value *RetVal = DECL_LLVM(DECL_RESULT(FnDecl));
      if (const StructType *STy = dyn_cast<StructType>(Fn->getReturnType())) {
        Value *R1 = BitCastToType(RetVal, STy->getPointerTo());

        llvm::Value *Idxs[2];
        Idxs[0] = ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);
        for (unsigned ri = 0; ri < STy->getNumElements(); ++ri) {
          Idxs[1] = ConstantInt::get(llvm::Type::getInt32Ty(Context), ri);
          Value *GEP = Builder.CreateGEP(R1, Idxs, Idxs+2, "mrv_gep");
          Value *E = Builder.CreateLoad(GEP, "mrv");
          RetVals.push_back(E);
        }
      } else {
        // Otherwise, this aggregate result must be something that is returned
        // in a scalar register for this target.  We must bit convert the
        // aggregate to the specified scalar type, which we do by casting the
        // pointer and loading.  The load does not necessarily start at the
        // beginning of the aggregate (x86-64).
        if (ReturnOffset) {
          RetVal = BitCastToType(RetVal,
                                 Type::getInt8PtrTy(Context));
          RetVal = Builder.CreateGEP(RetVal,
                    ConstantInt::get(TD.getIntPtrType(Context), ReturnOffset));
        }
        RetVal = BitCastToType(RetVal, Fn->getReturnType()->getPointerTo());
        RetVal = Builder.CreateLoad(RetVal, "retval");
        RetVals.push_back(RetVal);
      }
    }
  }
  if (EmitDebugInfo()) {
    TheDebugInfo->EmitStopPoint(Fn, Builder.GetInsertBlock(), Builder);
    TheDebugInfo->EmitFunctionEnd(Builder.GetInsertBlock(), true);
  }
  if (RetVals.empty())
    Builder.CreateRetVoid();
  else if (!Fn->getReturnType()->isAggregateType()) {
    assert(RetVals.size() == 1 && "Non-aggregate return has multiple values!");
    Builder.CreateRet(RetVals[0]);
  } else
    Builder.CreateAggregateRet(RetVals.data(), RetVals.size());

  // Emit pending exception handling code.
  EmitLandingPads();
  EmitPostPads();
  EmitUnwindBlock();

  // Remove any cached LLVM values that are local to this function.  Such values
  // may be deleted when the optimizers run, so would be dangerous to keep.
  eraseLocalLLVMValues();

  // Simplify any values that were uniqued using a no-op bitcast.
  for (std::vector<BitCastInst *>::iterator I = UniquedValues.begin(),
       E = UniquedValues.end(); I != E; ++I) {
    BitCastInst *BI = *I;
    assert(BI->getSrcTy() == BI->getDestTy() && "Not a no-op bitcast!");
    BI->replaceAllUsesWith(BI->getOperand(0));
    // Safe to erase because after the call to eraseLocalLLVMValues.
    BI->eraseFromParent();
  }
  UniquedValues.clear();

#if !defined(TARGET_ARM)
  // ARM supports VLAs + dynamic realignment. Others don't.

  // If we've seen a vla in this function and we'll possibly need to
  // either dynamically realign or this is greater than the maximum stack
  // alignment, output a warning.  This is here so we don't warn every time
  // we see a variable.
  if (SeenVLA &&
      GreatestAlignment > TheTarget->getFrameLowering()->getStackAlignment())
      warning (0, "alignment for %q+D conflicts with a dynamically realigned "
                  "stack", SeenVLA);
#endif
  return Fn;
}

Function *TreeToLLVM::EmitFunction() {
  // Set up parameters and prepare for return, for the function.
  StartFunctionBody();

  // Emit the body of the function iterating over all BBs
  basic_block bb;
  edge e;
  edge_iterator ei;
  tree stmt = NULL_TREE;
  FOR_EACH_BB (bb) {
    for (block_stmt_iterator bsi = bsi_start (bb); !bsi_end_p (bsi);
         bsi_next (&bsi)) {
      MemRef DestLoc;
      stmt = bsi_stmt (bsi);

      // If this stmt returns an aggregate value (e.g. a call whose result is
      // ignored), create a temporary to receive the value.  Note that we don't
      // do this for MODIFY_EXPRs as an efficiency hack.
      if (isAggregateTreeType(TREE_TYPE(stmt)) &&
          TREE_CODE(stmt)!= MODIFY_EXPR && TREE_CODE(stmt)!=INIT_EXPR)
        DestLoc = CreateTempLoc(ConvertType(TREE_TYPE(stmt)));

      Emit(stmt, DestLoc.Ptr ? &DestLoc : NULL);
    }

    // If we didn't see any statements, the current bb is an empty
    // basic block.  But an empty block must have one outgoing edge,
    // and there might be some location info there; grab it.
    if (!stmt && EmitDebugInfo()) {
      assert(EDGE_COUNT(bb->succs) == 1 && 
             "empty basic block with multiple successors?");
      e = EDGE_I(bb->succs, 0);
      source_locus locus = e->goto_locus;
      if (locus) {
        // Set new location on the way up the tree.
        TheDebugInfo->setLocationFile(LOCATION_FILE(*locus));
        TheDebugInfo->setLocationLine(LOCATION_LINE(*locus));
      }
      TheDebugInfo->EmitStopPoint(Fn, Builder.GetInsertBlock(), Builder);
    }

    FOR_EACH_EDGE (e, ei, bb->succs)
      if (e->flags & EDGE_FALLTHRU)
        break;
    if (e && e->dest != bb->next_bb) {
      Builder.CreateBr(getLabelDeclBlock(tree_block_label (e->dest)));
      EmitBlock(BasicBlock::Create(Context, ""));
    }
  }

  // Wrap things up.
  return FinishFunctionBody();
}

// Given a GCC lexical context (BLOCK or FUNCTION_DECL), make it the
// new current BLOCK/context/scope.  Emit any local variables found
// in the new context.  Note that the variable emission order must be
// consistent with and without debug info; otherwise, the register
// allocation would change with -g, and users dislike that.
void TreeToLLVM::switchLexicalBlock(tree exp) {
  if (exp == NULL_TREE || TREE_CODE(exp) == FUNCTION_DECL) {
    // assert(RegionStack empty);
    if (EmitDebugInfo())
      TheDebugInfo->setCurrentLexicalBlock(exp);
    return;
  }

  // Only EXPR_P nodes have a TREE_BLOCK() field.
  if (!EXPR_P(exp) && (TREE_CODE(exp) != BLOCK))
    return;

  tree new_block = EXPR_P(exp) ? TREE_BLOCK(exp) : exp;

  if (!new_block)
    return;

  // Have we seen this BLOCK before?
  bool previously_visited = !SeenBlocks.insert(new_block);

  // If new_block is nested inside another block, and we haven't
  // processed either block, insure the outer block(s) get processed
  // first.
  if (!previously_visited)
    switchLexicalBlock(BLOCK_SUPERCONTEXT(new_block));

  if (EmitDebugInfo()) {
    tree current_block = TheDebugInfo->getCurrentLexicalBlock();
    if (new_block && current_block && new_block != current_block) {
      tree new_climber = new_block, current_climber = current_block;
      unsigned new_climber_depth, current_climber_depth;
      // Climb the GCC BLOCK hierarchy, looking for a common parent
      // BLOCK or FUNCTION_DECL.  Assume BLOCK_NUMBER() fields set to
      // depth by setLexicalBlockDepths().  If the climber depths are
      // equal, but the climbers differ, they are siblings: both
      // climbers rise to the next level.
      while (new_climber != current_climber) {
        current_climber_depth = DECL_P(current_climber) ? 
                                        0 : BLOCK_NUMBER(current_climber);
        new_climber_depth = DECL_P(new_climber) ? 0 : BLOCK_NUMBER(new_climber);
        if (new_climber_depth <= current_climber_depth)
          current_climber = BLOCK_SUPERCONTEXT(current_climber);
        if (new_climber_depth >= current_climber_depth)
          new_climber = BLOCK_SUPERCONTEXT(new_climber);
      }
      assert(new_climber == current_climber &&
             "missed common TREE_BLOCK parent");
      // Pop and push lexical blocks to arrive at the new context.
      TheDebugInfo->change_regions(new_block, new_climber);
    }
  }

  // If we've seen this lexical BLOCK before, we're done.
  if (previously_visited)
    return;

  // Finally, allocate any BLOCK_VARS we find.
  tree step;
  for (step = BLOCK_VARS(new_block); step; step = TREE_CHAIN(step))
    switch (TREE_CODE_CLASS(TREE_CODE(step))) {
    default:
      assert(0 && "non-var, non-type node hanging from a GCC BLOCK?");
      break;
    case tcc_type:
      // GCC has recorded a lexical block scope for this type, but
      // ConvertType() doesn't currently handle scopes for types.
      break;
    case tcc_declaration:
      if (!DECL_LLVM_SET_P(step))
        EmitAutomaticVariableDecl(step);
      break;
    }

  // Kludge! The Objective-C++ front-end doesn't always point to the
  // right BLOCK. :-( Test case g++.apple/block-assigncglobal.C has a
  // block (closure) where the first statement (a MODIFY_EXPR) points
  // to a lexical BLOCK that has no BLOCK_VARS, but it has a subblock
  // with the decl for the target of the MODIFY.  Ergo, if we
  // encounter a BLOCK that allocates no variables, check for a
  // subblock.  A better fix would be in the front-end.
  if (!BLOCK_VARS(new_block) && BLOCK_SUBBLOCKS(new_block))
    switchLexicalBlock(BLOCK_SUBBLOCKS(new_block));
}

Value *TreeToLLVM::Emit(tree exp, const MemRef *DestLoc) {
  assert((isAggregateTreeType(TREE_TYPE(exp)) == (DestLoc != 0) ||
          TREE_CODE(exp) == MODIFY_EXPR || TREE_CODE(exp) == INIT_EXPR) &&
         "Didn't pass DestLoc to an aggregate expr, or passed it to scalar!");

  Value *Result = 0;

  bool emitdebuginfo = EmitDebugInfo();

  if (emitdebuginfo && EXPR_HAS_LOCATION(exp)) {
    // Set new location on the way up the tree.
    TheDebugInfo->setLocationFile(EXPR_FILENAME(exp));
    TheDebugInfo->setLocationLine(EXPR_LINENO(exp));
  }

  // If we've just changed lexical blocks, emit any local variables
  // declared in the new block.
  TreeToLLVM::switchLexicalBlock(exp);

  if (emitdebuginfo)
    TheDebugInfo->EmitStopPoint(Fn, Builder.GetInsertBlock(), Builder);

  switch (TREE_CODE(exp)) {
  default:
    errs() << "Unhandled expression!\n"
           << "TREE_CODE: " << TREE_CODE(exp) << "\n";
    debug_tree(exp);
    abort();

  // Control flow
  case LABEL_EXPR:     Result = EmitLABEL_EXPR(exp); break;
  case GOTO_EXPR:      Result = EmitGOTO_EXPR(exp); break;
  case RETURN_EXPR:    Result = EmitRETURN_EXPR(exp, DestLoc); break;
  case COND_EXPR:      Result = EmitCOND_EXPR(exp); break;
  case SWITCH_EXPR:    Result = EmitSWITCH_EXPR(exp); break;

  // Exception handling.
  case EXC_PTR_EXPR:   Result = EmitEXC_PTR_EXPR(exp); break;
  case FILTER_EXPR:    Result = EmitFILTER_EXPR(exp); break;
  case RESX_EXPR:      Result = EmitRESX_EXPR(exp); break;

  // Expressions
  case VAR_DECL:
  case PARM_DECL:
  case RESULT_DECL:
  case INDIRECT_REF:
  case ARRAY_REF:
  case ARRAY_RANGE_REF:
  case COMPONENT_REF:
  case BIT_FIELD_REF:
  case STRING_CST:
  case REALPART_EXPR:
  case IMAGPART_EXPR:
    Result = EmitLoadOfLValue(exp, DestLoc);
    break;
  case OBJ_TYPE_REF:   Result = EmitOBJ_TYPE_REF(exp); break;
  case ADDR_EXPR:      Result = EmitADDR_EXPR(exp); break;
  case CALL_EXPR:      Result = EmitCALL_EXPR(exp, DestLoc); break;
  case INIT_EXPR:
  case MODIFY_EXPR:    Result = EmitMODIFY_EXPR(exp, DestLoc); break;
  case ASM_EXPR:       Result = EmitASM_EXPR(exp); break;
  case NON_LVALUE_EXPR: Result = Emit(TREE_OPERAND(exp, 0), DestLoc); break;

    // Unary Operators
  case NOP_EXPR:       Result = EmitNOP_EXPR(exp, DestLoc); break;
  case FIX_TRUNC_EXPR:
  case FLOAT_EXPR:
  case CONVERT_EXPR:   Result = EmitCONVERT_EXPR(exp, DestLoc); break;
  case VIEW_CONVERT_EXPR: Result = EmitVIEW_CONVERT_EXPR(exp, DestLoc); break;
  case NEGATE_EXPR:    Result = EmitNEGATE_EXPR(exp, DestLoc); break;
  case CONJ_EXPR:      Result = EmitCONJ_EXPR(exp, DestLoc); break;
  case ABS_EXPR:       Result = EmitABS_EXPR(exp); break;
  case BIT_NOT_EXPR:   Result = EmitBIT_NOT_EXPR(exp); break;
  case TRUTH_NOT_EXPR: Result = EmitTRUTH_NOT_EXPR(exp); break;

  // Binary Operators
  case LT_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_ULT, ICmpInst::ICMP_SLT,
                         FCmpInst::FCMP_OLT);
    break;
  case LE_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_ULE, ICmpInst::ICMP_SLE,
                         FCmpInst::FCMP_OLE);
    break;
  case GT_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_UGT, ICmpInst::ICMP_SGT,
                         FCmpInst::FCMP_OGT);
    break;
  case GE_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_UGE, ICmpInst::ICMP_SGE,
                         FCmpInst::FCMP_OGE);
    break;
  case EQ_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_EQ, ICmpInst::ICMP_EQ,
                         FCmpInst::FCMP_OEQ);
    break;
  case NE_EXPR:
    Result = EmitCompare(exp, ICmpInst::ICMP_NE, ICmpInst::ICMP_NE,
                         FCmpInst::FCMP_UNE);
    break;
  case UNORDERED_EXPR:
    Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_UNO);
    break;
  case ORDERED_EXPR:
    Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_ORD);
    break;
  case UNLT_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_ULT); break;
  case UNLE_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_ULE); break;
  case UNGT_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_UGT); break;
  case UNGE_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_UGE); break;
  case UNEQ_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_UEQ); break;
  case LTGT_EXPR: Result = EmitCompare(exp, 0, 0, FCmpInst::FCMP_ONE); break;
  case PLUS_EXPR:
    Result = EmitBinOp(exp, DestLoc,
                       FLOAT_TYPE_P(TREE_TYPE(exp)) ?
                         Instruction::FAdd :
                         Instruction::Add);
    break;
  case MINUS_EXPR:
    Result = EmitBinOp(exp, DestLoc,
                       FLOAT_TYPE_P(TREE_TYPE(exp)) ?
                         Instruction::FSub :
                         Instruction::Sub);
    break;
  case MULT_EXPR:
    Result = EmitBinOp(exp, DestLoc,
                       FLOAT_TYPE_P(TREE_TYPE(exp)) ?
                         Instruction::FMul :
                         Instruction::Mul);
    break;
  case EXACT_DIV_EXPR: Result = EmitEXACT_DIV_EXPR(exp, DestLoc); break;
  case TRUNC_DIV_EXPR:
    if (TYPE_UNSIGNED(TREE_TYPE(exp)))
      Result = EmitBinOp(exp, DestLoc, Instruction::UDiv);
    else
      Result = EmitBinOp(exp, DestLoc, Instruction::SDiv);
    break;
  case RDIV_EXPR: Result = EmitBinOp(exp, DestLoc, Instruction::FDiv); break;
  case CEIL_DIV_EXPR: Result = EmitCEIL_DIV_EXPR(exp); break;
  case FLOOR_DIV_EXPR: Result = EmitFLOOR_DIV_EXPR(exp); break;
  case ROUND_DIV_EXPR: Result = EmitROUND_DIV_EXPR(exp); break;
  case TRUNC_MOD_EXPR:
    if (TYPE_UNSIGNED(TREE_TYPE(exp)))
      Result = EmitBinOp(exp, DestLoc, Instruction::URem);
    else
      Result = EmitBinOp(exp, DestLoc, Instruction::SRem);
    break;
  case FLOOR_MOD_EXPR: Result = EmitFLOOR_MOD_EXPR(exp, DestLoc); break;
  case BIT_AND_EXPR:   Result = EmitBinOp(exp, DestLoc, Instruction::And);break;
  case BIT_IOR_EXPR:   Result = EmitBinOp(exp, DestLoc, Instruction::Or );break;
  case BIT_XOR_EXPR:   Result = EmitBinOp(exp, DestLoc, Instruction::Xor);break;
  case TRUTH_AND_EXPR: Result = EmitTruthOp(exp, Instruction::And); break;
  case TRUTH_OR_EXPR:  Result = EmitTruthOp(exp, Instruction::Or); break;
  case TRUTH_XOR_EXPR: Result = EmitTruthOp(exp, Instruction::Xor); break;
  case RSHIFT_EXPR:
    Result = EmitShiftOp(exp,DestLoc,
       TYPE_UNSIGNED(TREE_TYPE(exp)) ? Instruction::LShr : Instruction::AShr);
    break;
  case LSHIFT_EXPR:    Result = EmitShiftOp(exp,DestLoc,Instruction::Shl);break;
  case RROTATE_EXPR:
    Result = EmitRotateOp(exp, Instruction::LShr, Instruction::Shl);
    break;
  case LROTATE_EXPR:
    Result = EmitRotateOp(exp, Instruction::Shl, Instruction::LShr);
    break;
  case MIN_EXPR:
    Result = EmitMinMaxExpr(exp, ICmpInst::ICMP_ULE, ICmpInst::ICMP_SLE,
                            FCmpInst::FCMP_OLE);
    break;
  case MAX_EXPR:
    Result = EmitMinMaxExpr(exp, ICmpInst::ICMP_UGE, ICmpInst::ICMP_SGE,
                            FCmpInst::FCMP_OGE);
    break;
  case CONSTRUCTOR:    Result = EmitCONSTRUCTOR(exp, DestLoc); break;

  // Complex Math Expressions.
  case COMPLEX_CST:    EmitCOMPLEX_CST (exp, DestLoc); break;
  case COMPLEX_EXPR:   EmitCOMPLEX_EXPR(exp, DestLoc); break;

  // Constant Expressions
  case INTEGER_CST:
    Result = TreeConstantToLLVM::ConvertINTEGER_CST(exp);
    break;
  case REAL_CST:
    Result = TreeConstantToLLVM::ConvertREAL_CST(exp);
    break;
  case VECTOR_CST:
    Result = TreeConstantToLLVM::ConvertVECTOR_CST(exp);
    break;
  }

  if (EmitDebugInfo() && EXPR_HAS_LOCATION(exp)) {
    // Restore location back down the tree.
    TheDebugInfo->setLocationFile(EXPR_FILENAME(exp));
    TheDebugInfo->setLocationLine(EXPR_LINENO(exp));
  }

  assert(((DestLoc && Result == 0) || DestLoc == 0) &&
         "Expected a scalar or aggregate but got the wrong thing!");
  // Check that the type of the result matches that of the tree node.  If the
  // result is not used then GCC sometimes sets the tree type to VOID_TYPE, so
  // don't take VOID_TYPE too seriously here.
  assert((Result == 0 || VOID_TYPE_P(TREE_TYPE(exp)) ||
          // FIXME: The vector stuff isn't straight-forward. Sometimes X86 can
          // pass it back as a scalar value. Disable checking if it's a
          // vector. This should be made better, though.
          ConvertType(TREE_TYPE(exp))->isVectorTy() ||
          // FIXME: The handling of MODIFY_EXPR doesn't always produce results
          // that pass this check; the return type might be the LHS type or
          // the RHS type, neither of which is guaranteed to be the
          // same as the expression type.
          TREE_CODE(exp) == MODIFY_EXPR ||
          Result->getType() == ConvertType(TREE_TYPE(exp))) &&
          "Value has wrong type!");
  return Result;
}

/// get_constant_alignment - Return the alignment of constant EXP in bits.
///
static unsigned int
get_constant_alignment (tree exp)
{
  unsigned int align = TYPE_ALIGN (TREE_TYPE (exp));
#ifdef CONSTANT_ALIGNMENT
  align = CONSTANT_ALIGNMENT (exp, align);
#endif
  return align;
}

/// EmitLV - Convert the specified l-value tree node to LLVM code, returning
/// the address of the result.
LValue TreeToLLVM::EmitLV(tree exp) {
  // Needs to be in sync with EmitVIEW_CONVERT_EXPR.
  LValue LV;

  switch (TREE_CODE(exp)) {
  default:
    errs() << "Unhandled lvalue expression!\n";
    debug_tree(exp);
    abort();

  case PARM_DECL:
  case VAR_DECL:
  case FUNCTION_DECL:
  case CONST_DECL:
  case RESULT_DECL:
    LV = EmitLV_DECL(exp);
    break;
  case ARRAY_RANGE_REF:
  case ARRAY_REF:
    LV = EmitLV_ARRAY_REF(exp);
    break;
  case COMPONENT_REF:
    LV = EmitLV_COMPONENT_REF(exp);
    break;
  case BIT_FIELD_REF:
    LV = EmitLV_BIT_FIELD_REF(exp);
    break;
  case REALPART_EXPR:
    LV = EmitLV_XXXXPART_EXPR(exp, 0);
    break;
  case IMAGPART_EXPR:
    LV = EmitLV_XXXXPART_EXPR(exp, 1);
    break;

  // Constants.
  case LABEL_DECL: {
    LV = LValue(EmitLV_LABEL_DECL(exp), 1);
    break;
  }
  case COMPLEX_CST: {
    Value *Ptr = TreeConstantToLLVM::EmitLV_COMPLEX_CST(exp);
    LV = LValue(Ptr, get_constant_alignment(exp) / 8);
    break;
  }
  case STRING_CST: {
    Value *Ptr = TreeConstantToLLVM::EmitLV_STRING_CST(exp);
    LV = LValue(Ptr, get_constant_alignment(exp) / 8);
    break;
  }

  // Type Conversion.
  case VIEW_CONVERT_EXPR:
    LV = EmitLV_VIEW_CONVERT_EXPR(exp);
    break;

  // Exception Handling.
  case EXC_PTR_EXPR:
    LV = EmitLV_EXC_PTR_EXPR(exp);
    break;
  case FILTER_EXPR:
    LV = EmitLV_FILTER_EXPR(exp);
    break;

  // Trivial Cases.
  case WITH_SIZE_EXPR:
    LV = EmitLV_WITH_SIZE_EXPR(exp);
    break;
  case INDIRECT_REF:
    LV = EmitLV_INDIRECT_REF(exp);
    break;
  }

  // Check that the type of the lvalue is indeed that of a pointer to the tree
  // node.  This may not hold for bitfields because the type of a bitfield need
  // not match the type of the value being loaded out of it.  Since LLVM has no
  // void* type, don't insist that void* be converted to a specific LLVM type.
  // For MMX registers, we deliberately changed the type to x86mmx here.
  assert((LV.isBitfield() || VOID_TYPE_P(TREE_TYPE(exp)) ||
          LLVM_IS_DECL_MMX_REGISTER(exp) ||
          LV.Ptr->getType() == ConvertType(TREE_TYPE(exp))->getPointerTo()) &&
         "LValue has wrong type!");

  return LV;
}

//===----------------------------------------------------------------------===//
//                         ... Utility Functions ...
//===----------------------------------------------------------------------===//

void TreeToLLVM::TODO(tree exp) {
  errs() << "Unhandled tree node\n";
  if (exp) debug_tree(exp);
  abort();
}

/// CastToType - Cast the specified value to the specified type if it is
/// not already that type.
Value *TreeToLLVM::CastToType(unsigned opcode, Value *V, const Type* Ty) {
  // Handle 'trunc (zext i1 X to T2) to i1' as X, because this occurs all over
  // the place.
  if (ZExtInst *CI = dyn_cast<ZExtInst>(V))
    if (Ty->isIntegerTy(1) && CI->getOperand(0)->getType()->isIntegerTy(1))
      return CI->getOperand(0);

  return Builder.CreateCast(Instruction::CastOps(opcode), V, Ty,
                            V->getName().data());
}

/// CastToAnyType - Cast the specified value to the specified type making no
/// assumptions about the types of the arguments. This creates an inferred cast.
Value *TreeToLLVM::CastToAnyType(Value *V, bool VisSigned,
                                 const Type* Ty, bool TyIsSigned) {
  // Eliminate useless casts of a type to itself.
  if (V->getType() == Ty)
    return V;

  // The types are different so we must cast. Use getCastOpcode to create an
  // inferred cast opcode.
  Instruction::CastOps opc =
    CastInst::getCastOpcode(V, VisSigned, Ty, TyIsSigned);

  // Generate the cast and return it.
  return CastToType(opc, V, Ty);
}

/// CastToUIntType - Cast the specified value to the specified type assuming
/// that the value and type are unsigned integer types.
Value *TreeToLLVM::CastToUIntType(Value *V, const Type* Ty) {
  // Eliminate useless casts of a type to itself.
  if (V->getType() == Ty)
    return V;

  unsigned SrcBits = V->getType()->getPrimitiveSizeInBits();
  unsigned DstBits = Ty->getPrimitiveSizeInBits();
  assert(SrcBits != DstBits && "Types are different but have same #bits?");

  Instruction::CastOps opcode =
    (SrcBits > DstBits ? Instruction::Trunc : Instruction::ZExt);
  return CastToType(opcode, V, Ty);
}

/// CastToSIntType - Cast the specified value to the specified type assuming
/// that the value and type are signed integer types.
Value *TreeToLLVM::CastToSIntType(Value *V, const Type* Ty) {
  // Eliminate useless casts of a type to itself.
  if (V->getType() == Ty)
    return V;

  unsigned SrcBits = V->getType()->getPrimitiveSizeInBits();
  unsigned DstBits = Ty->getPrimitiveSizeInBits();
  assert(SrcBits != DstBits && "Types are different but have same #bits?");

  Instruction::CastOps opcode =
    (SrcBits > DstBits ? Instruction::Trunc : Instruction::SExt);
  return CastToType(opcode, V, Ty);
}

/// CastToFPType - Cast the specified value to the specified type assuming
/// that the value and type are floating point.
Value *TreeToLLVM::CastToFPType(Value *V, const Type* Ty) {
  unsigned SrcBits = V->getType()->getPrimitiveSizeInBits();
  unsigned DstBits = Ty->getPrimitiveSizeInBits();
  if (SrcBits == DstBits)
    return V;
  Instruction::CastOps opcode = (SrcBits > DstBits ?
      Instruction::FPTrunc : Instruction::FPExt);
  return CastToType(opcode, V, Ty);
}

/// BitCastToType - Insert a BitCast from V to Ty if needed. This is just a
/// shorthand convenience function for CastToType(Instruction::BitCast,V,Ty).
Value *TreeToLLVM::BitCastToType(Value *V, const Type *Ty) {
  return CastToType(Instruction::BitCast, V, Ty);
}

/// CreateTemporary - Create a new alloca instruction of the specified type,
/// inserting it into the entry block and returning it.  The resulting
/// instruction's type is a pointer to the specified type.
AllocaInst *TreeToLLVM::CreateTemporary(const Type *Ty, unsigned align) {
  if (AllocaInsertionPoint == 0) {
    // Create a dummy instruction in the entry block as a marker to insert new
    // alloc instructions before.  It doesn't matter what this instruction is,
    // it is dead.  This allows us to insert allocas in order without having to
    // scan for an insertion point. Use BitCast for int -> int
    AllocaInsertionPoint = CastInst::Create(Instruction::BitCast,
      Constant::getNullValue(Type::getInt32Ty(Context)),
      Type::getInt32Ty(Context), "alloca point");
    // Insert it as the first instruction in the entry block.
    Fn->begin()->getInstList().insert(Fn->begin()->begin(),
                                      AllocaInsertionPoint);
  }
  if (align)
    return new AllocaInst(Ty, 0, align, "memtmp", AllocaInsertionPoint);
  else
    return new AllocaInst(Ty, 0, "memtmp", AllocaInsertionPoint);
}

/// CreateTempLoc - Like CreateTemporary, but returns a MemRef.
MemRef TreeToLLVM::CreateTempLoc(const Type *Ty) {
  AllocaInst *AI = CreateTemporary(Ty);
  // MemRefs do not allow alignment 0.
  if (!AI->getAlignment())
    AI->setAlignment(TD.getPrefTypeAlignment(Ty));
  return MemRef(AI, AI->getAlignment(), false);
}

/// EmitBlock - Add the specified basic block to the end of the function.  If
/// the previous block falls through into it, add an explicit branch.
void TreeToLLVM::EmitBlock(BasicBlock *BB) {
  BasicBlock *CurBB = Builder.GetInsertBlock();
  // If the previous block falls through to BB, add an explicit branch.
  if (CurBB->getTerminator() == 0) {
    // If the previous block has no label and is empty, remove it: it is a
    // post-terminator block.
    if (CurBB->getName().empty() && CurBB->begin() == CurBB->end())
      CurBB->eraseFromParent();
    else
      // Otherwise, fall through to this block.
      Builder.CreateBr(BB);
  }

  // Add this block.
  Fn->getBasicBlockList().push_back(BB);
  Builder.SetInsertPoint(BB);  // It is now the current block.
}

/// CopyAggregate - Recursively traverse the potientially aggregate src/dest
/// ptrs, copying all of the elements.
static void CopyAggregate(MemRef DestLoc, MemRef SrcLoc,
                          LLVMBuilder &Builder, tree gccType){
  assert(DestLoc.Ptr->getType() == SrcLoc.Ptr->getType() &&
         "Cannot copy between two pointers of different type!");
  const Type *ElTy =
    cast<PointerType>(DestLoc.Ptr->getType())->getElementType();

  unsigned Alignment = std::min(DestLoc.getAlignment(), SrcLoc.getAlignment());

  if (ElTy->isSingleValueType()) {
    LoadInst *V = Builder.CreateLoad(SrcLoc.Ptr, SrcLoc.Volatile);
    StoreInst *S = Builder.CreateStore(V, DestLoc.Ptr, DestLoc.Volatile);
    V->setAlignment(Alignment);
    S->setAlignment(Alignment);
  } else if (const StructType *STy = dyn_cast<StructType>(ElTy)) {
    const StructLayout *SL = getTargetData().getStructLayout(STy);
    for (unsigned i = 0, e = STy->getNumElements(); i != e; ++i) {
      if (gccType && isPaddingElement(gccType, i))
        continue;
      Value *DElPtr = Builder.CreateStructGEP(DestLoc.Ptr, i);
      Value *SElPtr = Builder.CreateStructGEP(SrcLoc.Ptr, i);
      unsigned Align = MinAlign(Alignment, SL->getElementOffset(i));
      CopyAggregate(MemRef(DElPtr, Align, DestLoc.Volatile),
                    MemRef(SElPtr, Align, SrcLoc.Volatile),
                    Builder, 0);
    }
  } else {
    const ArrayType *ATy = cast<ArrayType>(ElTy);
    unsigned EltSize = getTargetData().getTypeAllocSize(ATy->getElementType());
    for (unsigned i = 0, e = ATy->getNumElements(); i != e; ++i) {
      Value *DElPtr = Builder.CreateStructGEP(DestLoc.Ptr, i);
      Value *SElPtr = Builder.CreateStructGEP(SrcLoc.Ptr, i);
      unsigned Align = MinAlign(Alignment, i * EltSize);
      CopyAggregate(MemRef(DElPtr, Align, DestLoc.Volatile),
                    MemRef(SElPtr, Align, SrcLoc.Volatile),
                    Builder, 0);
    }
  }
}

/// CountAggregateElements - Return the number of elements in the specified type
/// that will need to be loaded/stored if we copy this by explicit accesses.
static unsigned CountAggregateElements(const Type *Ty) {
  if (Ty->isSingleValueType()) return 1;

  if (const StructType *STy = dyn_cast<StructType>(Ty)) {
    unsigned NumElts = 0;
    for (unsigned i = 0, e = STy->getNumElements(); i != e; ++i)
      NumElts += CountAggregateElements(STy->getElementType(i));
    return NumElts;
  } else {
    const ArrayType *ATy = cast<ArrayType>(Ty);
    return ATy->getNumElements()*CountAggregateElements(ATy->getElementType());
  }
}

/// containsFPField - indicates whether the given LLVM type
/// contains any floating point elements.

static bool containsFPField(const Type *LLVMTy) {
  if (LLVMTy->isFloatingPointTy())
    return true;
  const StructType* STy = dyn_cast<StructType>(LLVMTy);
  if (STy) {
    for (StructType::element_iterator I = STy->element_begin(),
                                      E = STy->element_end(); I != E; I++) {
      const Type *Ty = *I;
      if (Ty->isFloatingPointTy())
        return true;
      if (Ty->isStructTy() && containsFPField(Ty))
        return true;
      const ArrayType *ATy = dyn_cast<ArrayType>(Ty);
      if (ATy && containsFPField(ATy->getElementType()))
        return true;
      const VectorType *VTy = dyn_cast<VectorType>(Ty);
      if (VTy && containsFPField(VTy->getElementType()))
        return true;
    }
  }
  return false;
}

#ifndef TARGET_LLVM_MIN_BYTES_COPY_BY_MEMCPY
#define TARGET_LLVM_MIN_BYTES_COPY_BY_MEMCPY 64
#endif

/// EmitAggregateCopy - Copy the elements from SrcLoc to DestLoc, using the
/// GCC type specified by GCCType to know which elements to copy.
void TreeToLLVM::EmitAggregateCopy(MemRef DestLoc, MemRef SrcLoc, tree type) {
  if (DestLoc.Ptr == SrcLoc.Ptr && !DestLoc.Volatile && !SrcLoc.Volatile)
    return;  // noop copy.

  // If the type is small, copy the elements instead of using a block copy.
  const Type *LLVMTy = ConvertType(type);
  unsigned NumElts = CountAggregateElements(LLVMTy);
  if (TREE_CODE(TYPE_SIZE(type)) == INTEGER_CST &&
      (NumElts == 1 ||
       TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type)) <
       TARGET_LLVM_MIN_BYTES_COPY_BY_MEMCPY)) {

    // Some targets (x87) cannot pass non-floating-point values using FP
    // instructions.  The LLVM type for a union may include FP elements,
    // even if some of the union fields do not; it is unsafe to pass such
    // converted types element by element.  PR 2680.

    // If the GCC type is not fully covered by the LLVM type, use memcpy. This
    // can occur with unions etc.
    if ((TREE_CODE(type) != UNION_TYPE || !containsFPField(LLVMTy)) &&
        !TheTypeConverter->GCCTypeOverlapsWithLLVMTypePadding(type, LLVMTy) &&
        // Don't copy tons of tiny elements.
        NumElts <= 8) {
      DestLoc.Ptr = BitCastToType(DestLoc.Ptr,
                                  LLVMTy->getPointerTo());
      SrcLoc.Ptr = BitCastToType(SrcLoc.Ptr,
                                 LLVMTy->getPointerTo());
      CopyAggregate(DestLoc, SrcLoc, Builder, type);
      return;
    }
  }

  Value *TypeSize = Emit(TYPE_SIZE_UNIT(type), 0);
  EmitMemCpy(DestLoc.Ptr, SrcLoc.Ptr, TypeSize,
             std::min(DestLoc.getAlignment(), SrcLoc.getAlignment()));
}

/// ZeroAggregate - Recursively traverse the potentially aggregate DestLoc,
/// zero'ing all of the elements.
static void ZeroAggregate(MemRef DestLoc, LLVMBuilder &Builder) {
  const Type *ElTy =
    cast<PointerType>(DestLoc.Ptr->getType())->getElementType();
  if (ElTy->isSingleValueType()) {
    StoreInst *St = Builder.CreateStore(Constant::getNullValue(ElTy),
                                        DestLoc.Ptr, DestLoc.Volatile);
    St->setAlignment(DestLoc.getAlignment());
  } else if (const StructType *STy = dyn_cast<StructType>(ElTy)) {
    const StructLayout *SL = getTargetData().getStructLayout(STy);
    for (unsigned i = 0, e = STy->getNumElements(); i != e; ++i) {
      Value *Ptr = Builder.CreateStructGEP(DestLoc.Ptr, i);
      unsigned Alignment = MinAlign(DestLoc.getAlignment(),
                                    SL->getElementOffset(i));
      ZeroAggregate(MemRef(Ptr, Alignment, DestLoc.Volatile), Builder);
    }
  } else {
    const ArrayType *ATy = cast<ArrayType>(ElTy);
    unsigned EltSize = getTargetData().getTypeAllocSize(ATy->getElementType());
    for (unsigned i = 0, e = ATy->getNumElements(); i != e; ++i) {
      Value *Ptr = Builder.CreateStructGEP(DestLoc.Ptr, i);
      unsigned Alignment = MinAlign(DestLoc.getAlignment(), i * EltSize);
      ZeroAggregate(MemRef(Ptr, Alignment, DestLoc.Volatile), Builder);
    }
  }
}

/// EmitAggregateZero - Zero the elements of DestLoc.
///
void TreeToLLVM::EmitAggregateZero(MemRef DestLoc, tree type) {
  // If the type is small, copy the elements instead of using a block copy.
  if (TREE_CODE(TYPE_SIZE(type)) == INTEGER_CST &&
      TREE_INT_CST_LOW(TYPE_SIZE_UNIT(type)) < 128) {
    const Type *LLVMTy = ConvertType(type);

    // If the GCC type is not fully covered by the LLVM type, use memset. This
    // can occur with unions etc.
    if (!TheTypeConverter->GCCTypeOverlapsWithLLVMTypePadding(type, LLVMTy) &&
        // Don't zero tons of tiny elements.
        CountAggregateElements(LLVMTy) <= 8) {
      DestLoc.Ptr = BitCastToType(DestLoc.Ptr,
                                  LLVMTy->getPointerTo());
      ZeroAggregate(DestLoc, Builder);
      return;
    }
  }

  EmitMemSet(DestLoc.Ptr, ConstantInt::get(Type::getInt8Ty(Context), 0),
             Emit(TYPE_SIZE_UNIT(type), 0), DestLoc.getAlignment());
}

Value *TreeToLLVM::EmitMemCpy(Value *DestPtr, Value *SrcPtr, Value *Size,
                              unsigned Align) {
  const Type *SBP = Type::getInt8PtrTy(Context);
  const Type *IntPtr = TD.getIntPtrType(Context);
  Value *Ops[5] = {
    BitCastToType(DestPtr, SBP),
    BitCastToType(SrcPtr, SBP),
    CastToSIntType(Size, IntPtr),
    ConstantInt::get(Type::getInt32Ty(Context), Align),
    ConstantInt::get(Type::getInt1Ty(Context), false)
  };

  const Type *ArgTypes[3] = {SBP, SBP, IntPtr };
  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::memcpy,
                                               ArgTypes, 3), Ops, Ops+5);
  return Ops[0];
}

Value *TreeToLLVM::EmitMemMove(Value *DestPtr, Value *SrcPtr, Value *Size,
                               unsigned Align) {
  const Type *SBP = Type::getInt8PtrTy(Context);
  const Type *IntPtr = TD.getIntPtrType(Context);
  Value *Ops[5] = {
    BitCastToType(DestPtr, SBP),
    BitCastToType(SrcPtr, SBP),
    CastToSIntType(Size, IntPtr),
    ConstantInt::get(Type::getInt32Ty(Context), Align),
    ConstantInt::get(Type::getInt1Ty(Context), false)
  };
  const Type *ArgTypes[3] = {SBP, SBP, IntPtr };

  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::memmove,
                                               ArgTypes, 3), Ops, Ops+5);
  return Ops[0];
}

Value *TreeToLLVM::EmitMemSet(Value *DestPtr, Value *SrcVal, Value *Size,
                              unsigned Align) {
  const Type *SBP = Type::getInt8PtrTy(Context);
  const Type *IntPtr = TD.getIntPtrType(Context);
  Value *Ops[5] = {
    BitCastToType(DestPtr, SBP),
    CastToSIntType(SrcVal, Type::getInt8Ty(Context)),
    CastToSIntType(Size, IntPtr),
    ConstantInt::get(Type::getInt32Ty(Context), Align),
    ConstantInt::get(Type::getInt1Ty(Context), false)
  };

  const Type *ArgTypes[2] = {SBP, IntPtr };
  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::memset,
                                               ArgTypes, 2), Ops, Ops+5);
  return Ops[0];
}


// Emits code to do something for a type attribute
void TreeToLLVM::EmitTypeGcroot(Value *V, tree decl) {
  // GC intrinsics can only be used in functions which specify a collector.
  Fn->setGC("shadow-stack");

  Function *gcrootFun = Intrinsic::getDeclaration(TheModule,
                                                  Intrinsic::gcroot);

  // The idea is that it's a pointer to type "Value"
  // which is opaque* but the routine expects i8** and i8*.
  const PointerType *Ty = Type::getInt8PtrTy(Context);
  V = Builder.CreateBitCast(V, Ty->getPointerTo());

  Value *Ops[2] = {
    V,
    ConstantPointerNull::get(Ty)
  };

  Builder.CreateCall(gcrootFun, Ops, Ops+2);
}

// Emits annotate intrinsic if the decl has the annotate attribute set.
void TreeToLLVM::EmitAnnotateIntrinsic(Value *V, tree decl) {

  // Handle annotate attribute on global.
  tree annotateAttr = lookup_attribute("annotate", DECL_ATTRIBUTES (decl));

  if (!annotateAttr)
    return;

  Function *annotateFun = Intrinsic::getDeclaration(TheModule,
                                                    Intrinsic::var_annotation);

  // Get file and line number
  Constant *lineNo =
    ConstantInt::get(Type::getInt32Ty(Context), DECL_SOURCE_LINE(decl));
  Constant *file = ConvertMetadataStringToGV(DECL_SOURCE_FILE(decl));
  const Type *SBP= Type::getInt8PtrTy(Context);
  file = Builder.getFolder().CreateBitCast(file, SBP);

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
      const Type *SBP = Type::getInt8PtrTy(Context);
      Constant *strGV = TreeConstantToLLVM::EmitLV_STRING_CST(val);
      Value *Ops[4] = {
        BitCastToType(V, SBP),
        BitCastToType(strGV, SBP),
        file,
        lineNo
      };

      Builder.CreateCall(annotateFun, Ops, Ops+4);
    }

    // Get next annotate attribute.
    annotateAttr = TREE_CHAIN(annotateAttr);
    if (annotateAttr)
      annotateAttr = lookup_attribute("annotate", annotateAttr);
  }
}

//===----------------------------------------------------------------------===//
//                  ... Basic Lists and Binding Scopes ...
//===----------------------------------------------------------------------===//

/// EmitAutomaticVariableDecl - Emit the function-local decl to the current
/// function and set DECL_LLVM for the decl to the right pointer.
void TreeToLLVM::EmitAutomaticVariableDecl(tree decl) {
  tree type = TREE_TYPE(decl);

  // An LLVM value pointer for this decl may already be set, for example, if the
  // named return value optimization is being applied to this function, and
  // this variable is the one being returned.
  assert(!DECL_LLVM_SET_P(decl) && "Shouldn't call this on an emitted var!");

  // For a CONST_DECL, set mode, alignment, and sizes from those of the
  // type in case this node is used in a reference.
  if (TREE_CODE(decl) == CONST_DECL) {
    DECL_MODE(decl)      = TYPE_MODE(type);
    DECL_ALIGN(decl)     = TYPE_ALIGN(type);
    DECL_SIZE(decl)      = TYPE_SIZE(type);
    DECL_SIZE_UNIT(decl) = TYPE_SIZE_UNIT(type);
    return;
  }

  // Otherwise, only automatic (and result) variables need any expansion done.
  // Static and external variables, and external functions, will be handled by
  // `assemble_variable' (called from finish_decl).  TYPE_DECL requires nothing.
  // PARM_DECLs are handled in `llvm_expand_function_start'.
  if ((TREE_CODE(decl) != VAR_DECL && TREE_CODE(decl) != RESULT_DECL) ||
      TREE_STATIC(decl) || DECL_EXTERNAL(decl) || type == error_mark_node)
    return;

  // If we've seen a VLA remember it in case we try to dynamically realign
  // the stack or request an alignment greater than the default stack
  // alignment.
  // We're basing the VLA on an expression that is an array that doesn't have
  // a constant decl size as below.  
  if (TREE_CODE(type) == ARRAY_TYPE &&
      DECL_SIZE(decl) != 0 && TREE_CODE(DECL_SIZE_UNIT(decl)) != INTEGER_CST)
    SeenVLA = decl;
      
  // Gimple temporaries are handled specially: their DECL_LLVM is set when the
  // definition is encountered.
  if (isGimpleTemporary(decl))
    return;

  const Type *Ty;  // Type to allocate
  Value *Size = 0; // Amount to alloca (null for 1)

  if (DECL_SIZE(decl) == 0) {    // Variable with incomplete type.
    if (DECL_INITIAL(decl) == 0)
      return; // Error message was already done; now avoid a crash.
    else {
      // "An initializer is going to decide the size of this array."??
      TODO(decl);
      abort();
    }
  } else if (TREE_CODE(DECL_SIZE_UNIT(decl)) == INTEGER_CST
             || (TREE_CODE(decl) == VAR_DECL && DECL_VALUE_EXPR(decl))) {
    // Variable of fixed size that goes on the stack or the type for a
    // variable we're not going to emit anyways, but need later for alignment
    // calculations.
    Ty = ConvertType(type);
  } else {
    // Dynamic-size object: must push space on the stack.
    if (TREE_CODE(type) == ARRAY_TYPE
        && isSequentialCompatible(type)
        && TYPE_SIZE(type) == DECL_SIZE(decl)) {
      Ty = ConvertType(TREE_TYPE(type));  // Get array element type.
      // Compute the number of elements in the array.
      Size = Emit(DECL_SIZE(decl), 0);
      assert(!integer_zerop(TYPE_SIZE(TREE_TYPE(type)))
             && "Array of positive size with elements of zero size!");
      Value *EltSize = Emit(TYPE_SIZE(TREE_TYPE(type)), 0);
      Size = Builder.CreateUDiv(Size, EltSize, "len");
    } else {
      // Compute the variable's size in bytes.
      Size = Emit(DECL_SIZE_UNIT(decl), 0);
      Ty = Type::getInt8Ty(Context);
    }
  }

  if (LLVM_IS_DECL_MMX_REGISTER(decl))
    Ty = Type::getX86_MMXTy(Context);

  unsigned Alignment = 0; // Alignment in bytes.

  // Set the alignment for the local if one of the following condition is met
  // 1) DECL_ALIGN is better than the alignment as per ABI specification
  // 2) DECL_ALIGN is set by user.
  if (DECL_ALIGN(decl)) {
    unsigned TargetAlign = getTargetData().getABITypeAlignment(Ty);
    if (DECL_USER_ALIGN(decl) || 8 * TargetAlign < (unsigned)DECL_ALIGN(decl))
      Alignment = DECL_ALIGN(decl) / 8;
  }

  // Record the alignment if it's the largest we've seen and is explicitly
  // requested.
  if (DECL_USER_ALIGN(decl) && Alignment > GreatestAlignment)
    GreatestAlignment = Alignment;

  // If this is just the rotten husk of a variable that the gimplifier
  // eliminated all uses of, but is preserving for debug info, ignore it.
  // We're doing this late so that we can get the alignment information from
  // the code.
  if (TREE_CODE(decl) == VAR_DECL && DECL_VALUE_EXPR(decl))
    return;
      
  const char *Name;      // Name of variable
  if (DECL_NAME(decl))
    Name = IDENTIFIER_POINTER(DECL_NAME(decl));
  else if (TREE_CODE(decl) == RESULT_DECL)
    Name = "retval";
  else
    Name = "";

  // Insert an alloca for this variable.
  AllocaInst *AI;
  if (!Size) {                           // Fixed size alloca -> entry block.
    AI = CreateTemporary(Ty);
    AI->setName(Name);
  } else {
    AI = Builder.CreateAlloca(Ty, Size, Name);
  }

  AI->setAlignment(Alignment);
  
  SET_DECL_LLVM(decl, AI);

  // Handle annotate attributes
  if (DECL_ATTRIBUTES(decl))
    EmitAnnotateIntrinsic(AI, decl);

  // Handle gcroot attribute
  if (POINTER_TYPE_P(TREE_TYPE (decl))
      && lookup_attribute("gcroot", TYPE_ATTRIBUTES(TREE_TYPE (decl))))
    {
      // We should null out local variables so that a stack crawl
      // before initialization doesn't get garbage results to follow.
      const Type *T = cast<PointerType>(AI->getType())->getElementType();
      EmitTypeGcroot(AI, decl);
      Builder.CreateStore(Constant::getNullValue(T), AI);
    }

  if (EmitDebugInfo()) {
    if (DECL_NAME(decl)) {
      TheDebugInfo->EmitDeclare(decl, dwarf::DW_TAG_auto_variable,
                                Name, TREE_TYPE(decl), AI,
                                Builder);
    } else if (TREE_CODE(decl) == RESULT_DECL) {
      TheDebugInfo->EmitDeclare(decl, dwarf::DW_TAG_return_variable,
                                Name, TREE_TYPE(decl), AI,
                                Builder);
    }
  }
}

//===----------------------------------------------------------------------===//
//                           ... Control Flow ...
//===----------------------------------------------------------------------===//

/// EmitLABEL_EXPR - Emit the basic block corresponding to the specified label.
///
Value *TreeToLLVM::EmitLABEL_EXPR(tree exp) {
  EmitBlock(getLabelDeclBlock(TREE_OPERAND(exp, 0)));
  return 0;
}

Value *TreeToLLVM::EmitGOTO_EXPR(tree exp) {
  tree dest = GOTO_DESTINATION(exp);
  if (TREE_CODE(dest) == LABEL_DECL) {
    // Direct branch.
    Builder.CreateBr(getLabelDeclBlock(dest));
  } else {
    // Indirect branch.
    basic_block bb = bb_for_stmt(exp);
    Value *V = Emit(dest, 0);
    IndirectBrInst *Br = Builder.CreateIndirectBr(V, EDGE_COUNT(bb->succs));

    // Add the list of possible destinations.
    edge e;
    edge_iterator ei;
    FOR_EACH_EDGE (e, ei, bb->succs)
      Br->addDestination(getLabelDeclBlock(tree_block_label(e->dest)));
  }
  EmitBlock(BasicBlock::Create(Context, ""));
  return 0;
}


Value *TreeToLLVM::EmitRETURN_EXPR(tree exp, const MemRef *DestLoc) {
  assert(DestLoc == 0 && "Does not return a value!");
  tree retval = TREE_OPERAND(exp, 0);

  assert((!retval || TREE_CODE(retval) == RESULT_DECL ||
          ((TREE_CODE(retval) == MODIFY_EXPR
             || TREE_CODE(retval) == INIT_EXPR) &&
           TREE_CODE(TREE_OPERAND(retval, 0)) == RESULT_DECL)) &&
         "RETURN_EXPR not gimple!");

  if (retval && TREE_CODE(retval) != RESULT_DECL)
    // Emit the assignment to RESULT_DECL.
    Emit(retval, 0);

  // Emit a branch to the exit label.
  Builder.CreateBr(ReturnBB);
  EmitBlock(BasicBlock::Create(Context, ""));
  return 0;
}

Value *TreeToLLVM::EmitCOND_EXPR(tree exp) {
  tree exp_cond = COND_EXPR_COND(exp);

  // Emit the conditional expression.  Special case comparisons since they are
  // very common and we want to avoid an extension to 'int' of the intermediate
  // result.
  unsigned UIPred = 0, SIPred = 0, FPPred = ~0;
  Value *Cond;
  switch (TREE_CODE(exp_cond)) {
  default: break;
  case LT_EXPR:
    UIPred = ICmpInst::ICMP_ULT;
    SIPred = ICmpInst::ICMP_SLT;
    FPPred = FCmpInst::FCMP_OLT;
    break;
  case LE_EXPR:
    UIPred = ICmpInst::ICMP_ULE;
    SIPred = ICmpInst::ICMP_SLE;
    FPPred = FCmpInst::FCMP_OLE;
    break;
  case GT_EXPR:
    UIPred = ICmpInst::ICMP_UGT;
    SIPred = ICmpInst::ICMP_SGT;
    FPPred = FCmpInst::FCMP_OGT;
    break;
  case GE_EXPR:
    UIPred = ICmpInst::ICMP_UGE;
    SIPred = ICmpInst::ICMP_SGE;
    FPPred = FCmpInst::FCMP_OGE;
    break;
  case EQ_EXPR:
    UIPred = SIPred = ICmpInst::ICMP_EQ;
    FPPred = FCmpInst::FCMP_OEQ;
    break;
  case NE_EXPR:
    UIPred = SIPred = ICmpInst::ICMP_NE;
    FPPred = FCmpInst::FCMP_UNE;
    break;
  case UNORDERED_EXPR: FPPred = FCmpInst::FCMP_UNO; break;
  case ORDERED_EXPR:   FPPred = FCmpInst::FCMP_ORD; break;
  case UNLT_EXPR:      FPPred = FCmpInst::FCMP_ULT; break;
  case UNLE_EXPR:      FPPred = FCmpInst::FCMP_ULE; break;
  case UNGT_EXPR:      FPPred = FCmpInst::FCMP_UGT; break;
  case UNGE_EXPR:      FPPred = FCmpInst::FCMP_UGE; break;
  case UNEQ_EXPR:      FPPred = FCmpInst::FCMP_UEQ; break;
  case LTGT_EXPR:      FPPred = FCmpInst::FCMP_ONE; break;
  }

  // If the operand wasn't a compare, emit it fully generally.  If it was, emit
  // it with EmitCompare to get the result as an i1.
  if (FPPred == ~0U) {
    Cond = Emit(exp_cond, 0);
    // Comparison against zero to convert the result to i1.
    if (!Cond->getType()->isIntegerTy(1))
      Cond = Builder.CreateIsNotNull(Cond, "toBool");
  } else {
    Cond = EmitCompare(exp_cond, UIPred, SIPred, FPPred,
                       Type::getInt1Ty(Context));
    assert(Cond->getType() == Type::getInt1Ty(Context));
  }

  tree Then = COND_EXPR_THEN(exp);
  tree Else = COND_EXPR_ELSE(exp);
  assert(TREE_CODE(Then) == GOTO_EXPR && TREE_CODE(Else) == GOTO_EXPR
         && "Not a gimple if?");

  BasicBlock *ThenDest = getLabelDeclBlock(TREE_OPERAND(Then, 0));
  BasicBlock *ElseDest = getLabelDeclBlock(TREE_OPERAND(Else, 0));
  Builder.CreateCondBr(Cond, ThenDest, ElseDest);
  EmitBlock(BasicBlock::Create(Context, ""));
  return 0;
}

Value *TreeToLLVM::EmitSWITCH_EXPR(tree exp) {
  tree Cases = SWITCH_LABELS(exp);

  // Emit the condition.
  Value *SwitchExp = Emit(SWITCH_COND(exp), 0);
  bool ExpIsSigned = !TYPE_UNSIGNED(TREE_TYPE(SWITCH_COND(exp)));

  // Emit the switch instruction.
  SwitchInst *SI = Builder.CreateSwitch(SwitchExp, Builder.GetInsertBlock(),
                                        TREE_VEC_LENGTH(Cases));
  EmitBlock(BasicBlock::Create(Context, ""));
  // Default location starts out as fall-through
  SI->setSuccessor(0, Builder.GetInsertBlock());

  assert(!SWITCH_BODY(exp) && "not a gimple switch?");

  BasicBlock *DefaultDest = NULL;
  for (unsigned i = 0, e = TREE_VEC_LENGTH(Cases); i != e; ++i) {
    BasicBlock *Dest = getLabelDeclBlock(CASE_LABEL(TREE_VEC_ELT(Cases, i)));

    tree low = CASE_LOW(TREE_VEC_ELT(Cases, i));
    if (!low) {
      DefaultDest = Dest;
      continue;
    }

    // Convert the integer to the right type.
    Value *Val = Emit(low, 0);
    Val = CastToAnyType(Val, !TYPE_UNSIGNED(TREE_TYPE(low)),
                        SwitchExp->getType(), ExpIsSigned);
    ConstantInt *LowC = cast<ConstantInt>(Val);

    tree high = CASE_HIGH(TREE_VEC_ELT(Cases, i));
    if (!high) {
      SI->addCase(LowC, Dest); // Single destination.
      continue;
    }

    // Otherwise, we have a range, like 'case 1 ... 17'.
    Val = Emit(high, 0);
    // Make sure the case value is the same type as the switch expression
    Val = CastToAnyType(Val, !TYPE_UNSIGNED(TREE_TYPE(high)),
                        SwitchExp->getType(), ExpIsSigned);
    ConstantInt *HighC = cast<ConstantInt>(Val);

    APInt Range = HighC->getValue() - LowC->getValue();
    if (Range.ult(APInt(Range.getBitWidth(), 64))) {
      // Add all of the necessary successors to the switch.
      APInt CurrentValue = LowC->getValue();
      while (1) {
        SI->addCase(LowC, Dest);
        if (LowC == HighC) break;  // Emitted the last one.
        CurrentValue++;
        LowC = ConstantInt::get(Context, CurrentValue);
      }
    } else {
      // The range is too big to add to the switch - emit an "if".
      Value *Diff = Builder.CreateSub(SwitchExp, LowC);
      Value *Cond = Builder.CreateICmpULE(Diff,
                                          ConstantInt::get(Context, Range));
      BasicBlock *False_Block = BasicBlock::Create(Context, "case_false");
      Builder.CreateCondBr(Cond, Dest, False_Block);
      EmitBlock(False_Block);
    }
  }

  if (DefaultDest) {
    if (SI->getSuccessor(0) == Builder.GetInsertBlock())
      SI->setSuccessor(0, DefaultDest);
    else {
      Builder.CreateBr(DefaultDest);
      // Emit a "fallthrough" block, which is almost certainly dead.
      EmitBlock(BasicBlock::Create(Context, ""));
    }
  }

  return 0;
}


/// CreateExceptionValues - Create values used internally by exception handling.
void TreeToLLVM::CreateExceptionValues() {
  // Check to see if the exception values have been constructed.
  if (ExceptionValue) return;

  ExceptionValue = CreateTemporary(Type::getInt8PtrTy(Context));
  ExceptionValue->setName("eh_exception");

  ExceptionSelectorValue = CreateTemporary(Type::getInt32Ty(Context));
  ExceptionSelectorValue->setName("eh_selector");

  FuncEHException = Intrinsic::getDeclaration(TheModule,
                                              Intrinsic::eh_exception);
  FuncEHSelector  = Intrinsic::getDeclaration(TheModule,
                                              Intrinsic::eh_selector);
  FuncEHGetTypeID = Intrinsic::getDeclaration(TheModule,
                                              Intrinsic::eh_typeid_for);

  CatchAll = TheModule->getGlobalVariable("llvm.eh.catch.all.value");
  if (!CatchAll && lang_eh_catch_all) {
    Constant *Init = 0;
    tree catch_all_type = lang_eh_catch_all();
    if (catch_all_type == NULL_TREE)
      // Use a C++ style null catch-all object.
      Init = Constant::getNullValue(Type::getInt8PtrTy(Context));
    else
      // This language has a type that catches all others.
      Init = cast<Constant>(Emit(catch_all_type, 0));

    CatchAll = new GlobalVariable(*TheModule, Init->getType(), true,
                                  GlobalVariable::LinkOnceAnyLinkage,
                                  Init, "llvm.eh.catch.all.value");
    CatchAll->setUnnamedAddr(true);
    CatchAll->setSection("llvm.metadata");
    AttributeUsedGlobals.insert(CatchAll);
  }
}

/// getPostPad - Return the post landing pad for the given exception handling
/// region, creating it if necessary.
BasicBlock *TreeToLLVM::getPostPad(unsigned RegionNo) {
  PostPads.grow(RegionNo);
  BasicBlock *&PostPad = PostPads[RegionNo];

  if (!PostPad)
    PostPad = BasicBlock::Create(Context, "ppad");

  return PostPad;
}

/// AddHandler - Append the given region to a vector of exception handlers.
/// A callback passed to foreach_reachable_handler.
static void AddHandler (struct eh_region *region, void *data) {
  ((std::vector<struct eh_region *> *)data)->push_back(region);
}

/// EmitLandingPads - Emit EH landing pads.
void TreeToLLVM::EmitLandingPads() {
  std::vector<Value*> Args;
  std::vector<struct eh_region *> Handlers;

  for (unsigned i = 1; i < LandingPads.size(); ++i) {
    BasicBlock *LandingPad = LandingPads[i];

    if (!LandingPad)
      continue;

    CreateExceptionValues();

    EmitBlock(LandingPad);

    // Fetch and store the exception.
    Value *Ex = Builder.CreateCall(FuncEHException, "eh_ptr");
    Builder.CreateStore(Ex, ExceptionValue);

    // Fetch and store the exception selector.

    // The exception and the personality function.
    Args.push_back(Builder.CreateLoad(ExceptionValue, "eh_ptr"));
    assert(llvm_eh_personality_libfunc
           && "no exception handling personality function!");
    Args.push_back(BitCastToType(DECL_LLVM(llvm_eh_personality_libfunc),
                                 Type::getInt8PtrTy(Context)));

    // Add selections for each handler.
    foreach_reachable_handler(i, false, AddHandler, &Handlers);

    bool HasCleanup = false;
    bool HasCatchAll = false;

    for (std::vector<struct eh_region *>::iterator I = Handlers.begin(),
         E = Handlers.end(); I != E; ++I) {
      struct eh_region *region = *I;

      // Create a post landing pad for the handler.
      getPostPad(get_eh_region_number(region));

      int RegionKind = classify_eh_handler(region);
      if (RegionKind < 0) {
        // Filter - note the length.
        tree TypeList = get_eh_type_list(region);
        unsigned Length = list_length(TypeList);
        Args.reserve(Args.size() + Length + 1);
        Args.push_back(ConstantInt::get(Type::getInt32Ty(Context), Length + 1));

        // Add the type infos.
        for (; TypeList; TypeList = TREE_CHAIN(TypeList)) {
          tree TType = lookup_type_for_runtime(TREE_VALUE(TypeList));
          Args.push_back(Emit(TType, 0));
        }
      } else if (RegionKind > 0) {
        // Catch.
        tree TypeList = get_eh_type_list(region);

        if (!TypeList) {
          // Catch-all - push the catch-all object.
          assert(CatchAll && "Language did not define lang_eh_catch_all?");
          Args.push_back(CatchAll);
          HasCatchAll = true;
        } else {
          // Add the type infos.
          for (; TypeList; TypeList = TREE_CHAIN(TypeList)) {
            tree TType = lookup_type_for_runtime(TREE_VALUE(TypeList));
            Args.push_back(Emit(TType, 0));
          }
        }
      } else {
        // Cleanup.
        HasCleanup = true;
      }
    }

    if (can_throw_external_1(i, false)) {
      if (HasCleanup) {
        if (Args.size() == 2 || USING_SJLJ_EXCEPTIONS || !lang_eh_catch_all) {
          // Insert the sentinal indicating that this is a cleanup-only
          // selector.  It may also be the representation of a catch-all for
          // some languages.
          Args.push_back(ConstantInt::get(Type::getInt32Ty(Context), 0));
        } else if (!HasCatchAll) {
          // Some exceptions from this region may not be caught by any handler.
          // Since invokes are required to branch to the unwind label no matter
          // what exception is being unwound, append a catch-all.
          assert(CatchAll && "Language did not define lang_eh_catch_all?");
          Args.push_back(CatchAll);
        }
      }
    }

    // Emit the selector call.
    Value *Select = Builder.CreateCall(FuncEHSelector, Args.begin(), Args.end(),
                                       "eh_select");
    Builder.CreateStore(Select, ExceptionSelectorValue);
    // Branch to the post landing pad for the first reachable handler.
    assert(!Handlers.empty() && "Landing pad but no handler?");
    Builder.CreateBr(getPostPad(get_eh_region_number(*Handlers.begin())));

    Handlers.clear();
    Args.clear();
  }
}

/// EmitPostPads - Emit EH post landing pads.
void TreeToLLVM::EmitPostPads() {
  std::vector<struct eh_region *> Handlers;

  for (unsigned i = 1; i < PostPads.size(); ++i) {
    BasicBlock *PostPad = PostPads[i];

    if (!PostPad)
      continue;

    CreateExceptionValues();

    EmitBlock(PostPad);

    struct eh_region *region = get_eh_region(i);
    BasicBlock *Dest = getLabelDeclBlock(get_eh_region_tree_label(region));

    int RegionKind = classify_eh_handler(region);
    if (!RegionKind || !get_eh_type_list(region)) {
      // Cleanup, catch-all or empty filter - no testing required.
      Builder.CreateBr(Dest);
      continue;
    } else if (RegionKind < 0) {
      // Filter - the result of a filter selection will be a negative index if
      // there is a match.
      Value *Select = Builder.CreateLoad(ExceptionSelectorValue);

      // Compare with the filter action value.
      Value *Zero = ConstantInt::get(Select->getType(), 0);
      Value *Compare = Builder.CreateICmpSLT(Select, Zero);

      // Branch on the compare.
      BasicBlock *NoFilterBB = BasicBlock::Create(Context, "nofilter");
      Builder.CreateCondBr(Compare, Dest, NoFilterBB);
      EmitBlock(NoFilterBB);
    } else if (RegionKind > 0) {
      // Catch
      tree TypeList = get_eh_type_list(region);

      Value *Cond = NULL;
      for (; TypeList; TypeList = TREE_CHAIN (TypeList)) {
        Value *TType = Emit(lookup_type_for_runtime(TREE_VALUE(TypeList)), 0);
        TType = BitCastToType(TType,
                              Type::getInt8PtrTy(Context));

        // Call get eh type id.
        Value *TypeID = Builder.CreateCall(FuncEHGetTypeID, TType, "eh_typeid");
        Value *Select = Builder.CreateLoad(ExceptionSelectorValue);

        // Compare with the exception selector.
        Value *Compare = Builder.CreateICmpEQ(Select, TypeID);

        Cond = Cond ? Builder.CreateOr(Cond, Compare) : Compare;
      }

      BasicBlock *NoCatchBB = NULL;

      // If the comparion fails, branch to the next catch that has a
      // post landing pad.
      struct eh_region *next_catch = get_eh_next_catch(region);
      for (; next_catch; next_catch = get_eh_next_catch(next_catch)) {
        unsigned CatchNo = get_eh_region_number(next_catch);

        if (CatchNo < PostPads.size())
          NoCatchBB = PostPads[CatchNo];

        if (NoCatchBB)
          break;
      }

      if (NoCatchBB) {
        // Branch on the compare.
        Builder.CreateCondBr(Cond, Dest, NoCatchBB);
        continue;
      }

      // If there is no such catch, execute a RESX if the comparison fails.
      NoCatchBB = BasicBlock::Create(Context, "nocatch");
      // Branch on the compare.
      Builder.CreateCondBr(Cond, Dest, NoCatchBB);
      EmitBlock(NoCatchBB);
    }

    // Emit a RESX_EXPR which skips handlers with no post landing pad.
    foreach_reachable_handler(i, true, AddHandler, &Handlers);

    BasicBlock *TargetBB = NULL;

    for (std::vector<struct eh_region *>::iterator I = Handlers.begin(),
         E = Handlers.end(); I != E; ++I) {
      unsigned UnwindNo = get_eh_region_number(*I);

      if (UnwindNo < PostPads.size())
        TargetBB = PostPads[UnwindNo];

      if (TargetBB)
        break;
    }

    if (TargetBB) {
      Builder.CreateBr(TargetBB);
    } else {
      assert(can_throw_external_1(i, true) &&
             "Must-not-throw region handled by runtime?");
      // Unwinding continues in the caller.
      if (!UnwindBB)
        UnwindBB = BasicBlock::Create(Context, "Unwind");
      Builder.CreateBr(UnwindBB);
    }

    Handlers.clear();
  }
}

/// EmitUnwindBlock - Emit the lazily created EH unwind block.
void TreeToLLVM::EmitUnwindBlock() {
  if (UnwindBB) {
    CreateExceptionValues();
    EmitBlock(UnwindBB);
    // Fetch and store exception handler.
    Value *Arg = Builder.CreateLoad(ExceptionValue, "eh_ptr");
    assert(llvm_unwind_resume_libfunc && "no unwind resume function!");

    // As we're emitting a naked call (not an expression) going through
    // EmitCallOf would be wasteful and incorrect. Manually adjust
    // the calling convention for this call here if necessary.
#ifdef TARGET_ADJUST_LLVM_CC
    tree fntype = TREE_TYPE(llvm_unwind_resume_libfunc);
    CallingConv::ID CallingConvention = CallingConv::C;

    TARGET_ADJUST_LLVM_CC(CallingConvention, fntype);
    CallInst *Call = Builder.CreateCall(DECL_LLVM(llvm_unwind_resume_libfunc),
                                        Arg);
    Call->setCallingConv(CallingConvention);
#else
    Builder.CreateCall(DECL_LLVM(llvm_unwind_resume_libfunc), Arg);
#endif
    Builder.CreateUnreachable();
  }
}

//===----------------------------------------------------------------------===//
//                           ... Expressions ...
//===----------------------------------------------------------------------===//

static bool canEmitLocalRegisterVariable(tree exp) {
  // Only variables can be marked as 'register'.
  if (TREE_CODE(exp) != VAR_DECL || !DECL_REGISTER(exp))
    return false;

  // Global register variables are not accepted here.
  if (TREE_STATIC(exp) || DECL_EXTERNAL(exp) || TREE_PUBLIC(exp))
    return false;

  // Asm block register usage is not accepted here.
  if (DECL_ASM_BLOCK_REGISTER (exp))
    return false;

  // Emit inline asm if this is local variable with assembler name on it.
  if (DECL_ASSEMBLER_NAME_SET_P(exp))
    return true;

  // Otherwise - it's normal automatic variable.
  return false;
}

static bool canEmitGlobalRegisterVariable(tree exp) {
  // Only variables can be marked as 'register'.
  if (TREE_CODE(exp) != VAR_DECL || !DECL_REGISTER(exp))
    return false;

  // Treat register usage in asm blocks as global.
  if (DECL_ASM_BLOCK_REGISTER (exp))
    return true;

  // Local register variables are not accepted here.
  if (TREE_STATIC(exp) || DECL_EXTERNAL(exp) || TREE_PUBLIC(exp))
    return true;

  // Otherwise - it's normal automatic variable, or local register variable.
  return false;
}


/// EmitLoadOfLValue - When an l-value expression is used in a context that
/// requires an r-value, this method emits the lvalue computation, then loads
/// the result.
Value *TreeToLLVM::EmitLoadOfLValue(tree exp, const MemRef *DestLoc) {
  // If this is a gimple temporary, don't emit a load, just use the result.
  if (isGimpleTemporary(exp)) {
    if (DECL_LLVM_SET_P(exp))
      return DECL_LLVM(exp);
    // Since basic blocks are output in no particular order, it is perfectly
    // possible to encounter a use of a gimple temporary before encountering
    // its definition, which is what has happened here.  This happens rarely
    // in practice, so there's no point in trying to do anything clever: just
    // demote to an ordinary variable and create an alloca to hold its value.
    DECL_GIMPLE_FORMAL_TEMP_P(exp) = 0;
    EmitAutomaticVariableDecl(exp);
    // Fall through.
  } else if (canEmitGlobalRegisterVariable(exp)) {
    // If this is a global register variable, EmitLV can't handle it (there is
    // no l-value of a global register variable).  Emit an inline asm node that
    // copies the value out of the specified register.
    return EmitReadOfRegisterVariable(exp, DestLoc);
  }

  LValue LV = EmitLV(exp);
  bool isVolatile = TREE_THIS_VOLATILE(exp);
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  if (LLVM_IS_DECL_MMX_REGISTER(exp))
    Ty = Type::getX86_MMXTy(Context);
  unsigned Alignment = LV.getAlignment();

  if (!LV.isBitfield()) {
    if (!DestLoc) {
      // Scalar value: emit a load.
      Value *Ptr = BitCastToType(LV.Ptr, Ty->getPointerTo());
      LoadInst *LI = Builder.CreateLoad(Ptr, isVolatile);
      LI->setAlignment(Alignment);
      if (canEmitLocalRegisterVariable(exp)) {
        // For register variable, move the loaded variable into the right reg.
        return EmitMoveOfRegVariableToRightReg(LI, exp);
      } else
        return LI;
    } else {
      EmitAggregateCopy(*DestLoc, MemRef(LV.Ptr, Alignment, isVolatile),
                        TREE_TYPE(exp));
      return 0;
    }
  } else {
    // This is a bitfield reference.
    if (!LV.BitSize)
      return Constant::getNullValue(Ty);

    const Type *ValTy = cast<PointerType>(LV.Ptr->getType())->getElementType();
    unsigned ValSizeInBits = ValTy->getPrimitiveSizeInBits();

    // The number of loads needed to read the entire bitfield.
    unsigned Strides = 1 + (LV.BitStart + LV.BitSize - 1) / ValSizeInBits;

    assert(ValTy->isIntegerTy() && "Invalid bitfield lvalue!");
    assert(ValSizeInBits > LV.BitStart && "Bad bitfield lvalue!");
    assert(ValSizeInBits >= LV.BitSize && "Bad bitfield lvalue!");
    assert(2*ValSizeInBits > LV.BitSize+LV.BitStart && "Bad bitfield lvalue!");

    Value *Result = NULL;

    for (unsigned I = 0; I < Strides; I++) {
      unsigned Index = BYTES_BIG_ENDIAN ? I : Strides - I - 1; // MSB first
      unsigned ThisFirstBit = Index * ValSizeInBits;
      unsigned ThisLastBitPlusOne = ThisFirstBit + ValSizeInBits;
      if (ThisFirstBit < LV.BitStart)
        ThisFirstBit = LV.BitStart;
      if (ThisLastBitPlusOne > LV.BitStart+LV.BitSize)
        ThisLastBitPlusOne = LV.BitStart+LV.BitSize;

      Value *Ptr = Index ?
        Builder.CreateGEP(LV.Ptr,
                          ConstantInt::get(Type::getInt32Ty(Context), Index)) :
        LV.Ptr;
      LoadInst *LI = Builder.CreateLoad(Ptr, isVolatile);
      LI->setAlignment(Alignment);
      Value *Val = LI;

      unsigned BitsInVal = ThisLastBitPlusOne - ThisFirstBit;
      unsigned FirstBitInVal = ThisFirstBit % ValSizeInBits;

      if (BYTES_BIG_ENDIAN)
        FirstBitInVal = ValSizeInBits-FirstBitInVal-BitsInVal;

      // Mask the bits out by shifting left first, then shifting right.  The
      // LLVM optimizer will turn this into an AND if this is an unsigned
      // expression.

      if (FirstBitInVal+BitsInVal != ValSizeInBits) {
        Value *ShAmt = ConstantInt::get(ValTy, ValSizeInBits -
                                        (FirstBitInVal+BitsInVal));
        Val = Builder.CreateShl(Val, ShAmt);
      }

      // Shift right required?
      if (ValSizeInBits != BitsInVal) {
        bool AddSignBits = !TYPE_UNSIGNED(TREE_TYPE(exp)) && !Result;
        Value *ShAmt = ConstantInt::get(ValTy, ValSizeInBits-BitsInVal);
        Val = AddSignBits ?
          Builder.CreateAShr(Val, ShAmt) : Builder.CreateLShr(Val, ShAmt);
      }

      if (Result) {
        Value *ShAmt = ConstantInt::get(ValTy, BitsInVal);
        Result = Builder.CreateShl(Result, ShAmt);
        Result = Builder.CreateOr(Result, Val);
      } else {
        Result = Val;
      }
    }

    if (TYPE_UNSIGNED(TREE_TYPE(exp)))
      return CastToUIntType(Result, Ty);
    else
      return CastToSIntType(Result, Ty);
  }
}

Value *TreeToLLVM::EmitADDR_EXPR(tree exp) {
  LValue LV = EmitLV(TREE_OPERAND(exp, 0));
  assert((!LV.isBitfield() || LV.BitStart == 0) &&
         "It is illegal to take the address of a bitfield");
  // Perform a cast here if necessary.  For example, GCC sometimes forms an
  // ADDR_EXPR where the operand is an array, and the ADDR_EXPR type is a
  // pointer to the first element.
  return BitCastToType(LV.Ptr, ConvertType(TREE_TYPE(exp)));
}

Value *TreeToLLVM::EmitOBJ_TYPE_REF(tree exp) {
  return BitCastToType(Emit(OBJ_TYPE_REF_EXPR(exp), 0),
                       ConvertType(TREE_TYPE(exp)));
}

Value *TreeToLLVM::EmitCALL_EXPR(tree exp, const MemRef *DestLoc) {
  // Check for a built-in function call.  If we can lower it directly, do so
  // now.
  tree fndecl = get_callee_fndecl(exp);
  if (fndecl && DECL_BUILT_IN(fndecl) &&
      DECL_BUILT_IN_CLASS(fndecl) != BUILT_IN_FRONTEND) {
    Value *Res = 0;
    if (EmitBuiltinCall(exp, fndecl, DestLoc, Res))
      return Res;
  }

  Value *Callee = Emit(TREE_OPERAND(exp, 0), 0);

  assert(TREE_TYPE (TREE_OPERAND (exp, 0)) &&
         (TREE_CODE(TREE_TYPE (TREE_OPERAND (exp, 0))) == POINTER_TYPE ||
          TREE_CODE(TREE_TYPE (TREE_OPERAND (exp, 0))) == REFERENCE_TYPE ||
          TREE_CODE(TREE_TYPE (TREE_OPERAND (exp, 0))) == BLOCK_POINTER_TYPE)
         && "Not calling a function pointer?");
  tree function_type = TREE_TYPE(TREE_TYPE (TREE_OPERAND (exp, 0)));
  CallingConv::ID CallingConv;
  AttrListPtr PAL;

  const Type *Ty = TheTypeConverter->ConvertFunctionType(function_type,
                                                         fndecl,
                                                         TREE_OPERAND(exp, 2),
                                                         CallingConv, PAL);

  // If this is a direct call to a function using a static chain then we need
  // to ensure the function type is the one just calculated: it has an extra
  // parameter for the chain.
  Callee = BitCastToType(Callee, Ty->getPointerTo());

  // EmitCall(exp, DestLoc);
  Value *Result = EmitCallOf(Callee, exp, DestLoc, PAL);

  // If the function has the volatile bit set, then it is a "noreturn" function.
  // Output an unreachable instruction right after the function to prevent LLVM
  // from thinking that control flow will fall into the subsequent block.
  //
  if (fndecl && TREE_THIS_VOLATILE(fndecl)) {
    // LLVM LOCAL - begin radar 7885482
    /*
      Consider this pseudo-ObjC code:

      // locking using some "lock" variable
      @try {
        // ...
      } @catch (...) {
        // ...
        @throw;
      } @finally {
        // unlocking using the "lock" variable
      }

      The "lock" variable has live intervals from the top of the function
      through the @try block and in the @finally block. On 32-bit x86, it
      doesn't have a live interval in the @catch block. This is because in
      32-bit mode Objective-C uses setjmp/longjmp for exception handling and not
      the invoke/DWARF method.  The @throw is implemented as an
      "objc_exception_throw" call marked with NORETURN. The upshot is that if
      the "lock" variable is placed into a stack slot, there won't be an
      indication that the "lock" can be used after the "objc_exception_throw"
      executes. With the invoke/DWARF method, the unwind edge of the invoke
      points to the @finally block, so the "lock" variable will have a live
      interval leading to there.

      The solution is to have the "objc_exception_throw" behave in a similar
      manner to the invoke/DWARF method. That is remove the "NORETURN"
      attribute, allowing it to have an edge from the call to the @finally
      block.  */
    if (flag_objc_abi == 1 && 
        Callee->getName() == "objc_exception_throw")
      cast<Function>(Callee)->removeFnAttr(Attribute::NoReturn);
    else {
      Builder.CreateUnreachable();
      EmitBlock(BasicBlock::Create(Context, ""));
    }
    // LLVM LOCAL - end radar 7885482
  }
  return Result;
}

/// llvm_load_scalar_argument - Load value located at LOC.
static Value *llvm_load_scalar_argument(Value *L,
                                        const llvm::Type *LLVMTy,
                                        unsigned RealSize,
                                        LLVMBuilder &Builder) {
  if (!RealSize)
    return UndefValue::get(LLVMTy);

  // Not clear what this is supposed to do on big endian machines...
  assert(!BYTES_BIG_ENDIAN && "Unsupported case - please report");
  assert(LLVMTy->isIntegerTy() && "Expected an integer value!");
  const Type *LoadType = IntegerType::get(Context, RealSize * 8);
  L = Builder.CreateBitCast(L, LoadType->getPointerTo());
  Value *Val = Builder.CreateLoad(L);
  if (LoadType->getPrimitiveSizeInBits() >= LLVMTy->getPrimitiveSizeInBits())
    Val = Builder.CreateTrunc(Val, LLVMTy);
  else
    Val = Builder.CreateZExt(Val, LLVMTy);
  return Val;
}

#ifndef LLVM_LOAD_SCALAR_ARGUMENT
#define LLVM_LOAD_SCALAR_ARGUMENT(LOC,TY,SIZE,BUILDER) \
  llvm_load_scalar_argument((LOC),(TY),(SIZE),(BUILDER))
#endif

namespace {
  /// FunctionCallArgumentConversion - This helper class is driven by the ABI
  /// definition for this target to figure out how to pass arguments into the
  /// stack/regs for a function call.
  struct FunctionCallArgumentConversion : public DefaultABIClient {
    SmallVector<Value*, 16> &CallOperands;
    SmallVector<Value*, 2> LocStack;
    const FunctionType *FTy;
    const MemRef *DestLoc;
    bool useReturnSlot;
    LLVMBuilder &Builder;
    Value *TheValue;
    MemRef RetBuf;
    CallingConv::ID &CallingConv;
    bool isShadowRet;
    bool isAggrRet;
    unsigned Offset;

    FunctionCallArgumentConversion(SmallVector<Value*, 16> &ops,
                                   const FunctionType *FnTy,
                                   const MemRef *destloc,
                                   bool ReturnSlotOpt,
                                   LLVMBuilder &b,
                                   CallingConv::ID &CC)
      : CallOperands(ops), FTy(FnTy), DestLoc(destloc),
        useReturnSlot(ReturnSlotOpt), Builder(b), CallingConv(CC),
        isShadowRet(false), isAggrRet(false), Offset(0) { }

    /// getCallingConv - This provides the desired CallingConv for the function.
    CallingConv::ID& getCallingConv(void) { return CallingConv; }

    // Push the address of an argument.
    void pushAddress(Value *Loc) {
      assert(Loc && "Invalid location!");
      LocStack.push_back(Loc);
    }

    // Push the value of an argument.
    void pushValue(Value *V) {
      assert(LocStack.empty() && "Value only allowed at top level!");
      LocStack.push_back(NULL);
      TheValue = V;
    }

    // Get the address of the current location.
    Value *getAddress(void) {
      assert(!LocStack.empty());
      Value *&Loc = LocStack.back();
      if (!Loc) {
        // A value.  Store to a temporary, and return the temporary's address.
        // Any future access to this argument will reuse the same address.
        Loc = TheTreeToLLVM->CreateTemporary(TheValue->getType());
        Builder.CreateStore(TheValue, Loc);
      }
      return Loc;
    }

    // Get the value of the current location (of type Ty).
    Value *getValue(const Type *Ty) {
      assert(!LocStack.empty());
      Value *Loc = LocStack.back();
      if (Loc) {
        // An address.  Convert to the right type and load the value out.
        Loc = Builder.CreateBitCast(Loc, Ty->getPointerTo());
        return Builder.CreateLoad(Loc, "val");
      } else {
        // A value - just return it.
        assert(TheValue->getType() == Ty && "Value not of expected type!");
        return TheValue;
      }
    }

    void clear() {
      assert(LocStack.size() == 1 && "Imbalance!");
      LocStack.clear();
    }

    bool isShadowReturn() const { return isShadowRet; }
    bool isAggrReturn() { return isAggrRet; }

    // EmitShadowResult - If the return result was redirected to a buffer,
    // emit it now.
    Value *EmitShadowResult(tree type, const MemRef *DestLoc) {
      if (!RetBuf.Ptr)
        return 0;

      if (DestLoc) {
        // Copy out the aggregate return value now.
        assert(ConvertType(type) ==
               cast<PointerType>(RetBuf.Ptr->getType())->getElementType() &&
               "Inconsistent result types!");
        TheTreeToLLVM->EmitAggregateCopy(*DestLoc, RetBuf, type);
        return 0;
      } else {
        // Read out the scalar return value now.
        return Builder.CreateLoad(RetBuf.Ptr, "result");
      }
    }

    /// HandleScalarResult - This callback is invoked if the function returns a
    /// simple scalar result value.
    void HandleScalarResult(const Type *RetTy) {
      // There is nothing to do here if we return a scalar or void.
      assert(DestLoc == 0 &&
             "Call returns a scalar but caller expects aggregate!");
    }

    /// HandleAggregateResultAsScalar - This callback is invoked if the function
    /// returns an aggregate value by bit converting it to the specified scalar
    /// type and returning that.
    void HandleAggregateResultAsScalar(const Type *ScalarTy,
                                       unsigned Offset = 0) {
      this->Offset = Offset;
    }

    /// HandleAggregateResultAsAggregate - This callback is invoked if the
    /// function returns an aggregate value using multiple return values.
    void HandleAggregateResultAsAggregate(const Type *AggrTy) {
      // There is nothing to do here.
      isAggrRet = true;
    }

    /// HandleAggregateShadowResult - This callback is invoked if the function
    /// returns an aggregate value by using a "shadow" first parameter.  If
    /// RetPtr is set to true, the pointer argument itself is returned from the
    /// function.
    void HandleAggregateShadowResult(const PointerType *PtrArgTy,
                                       bool RetPtr) {
      // We need to pass memory to write the return value into.
      // FIXME: alignment and volatility are being ignored!
      assert(!DestLoc || PtrArgTy == DestLoc->Ptr->getType());

      if (DestLoc == 0) {
        // The result is unused, but still needs to be stored somewhere.
        Value *Buf = TheTreeToLLVM->CreateTemporary(PtrArgTy->getElementType());
        CallOperands.push_back(Buf);
      } else if (useReturnSlot) {
        // Letting the call write directly to the final destination is safe and
        // may be required.  Do not use a buffer.
        CallOperands.push_back(DestLoc->Ptr);
      } else {
        // Letting the call write directly to the final destination may not be
        // safe (eg: if DestLoc aliases a parameter) and is not required - pass
        // a buffer and copy it to DestLoc after the call.
        RetBuf = TheTreeToLLVM->CreateTempLoc(PtrArgTy->getElementType());
        CallOperands.push_back(RetBuf.Ptr);
      }

      // Note the use of a shadow argument.
      isShadowRet = true;
    }

    void HandlePad(const llvm::Type *LLVMTy) {
      CallOperands.push_back(UndefValue::get(LLVMTy));
    }

    /// HandleScalarShadowResult - This callback is invoked if the function
    /// returns a scalar value by using a "shadow" first parameter, which is a
    /// pointer to the scalar, of type PtrArgTy.  If RetPtr is set to true,
    /// the pointer argument itself is returned from the function.
    void HandleScalarShadowResult(const PointerType *PtrArgTy, bool RetPtr) {
      assert(DestLoc == 0 &&
             "Call returns a scalar but caller expects aggregate!");
      // Create a buffer to hold the result.  The result will be loaded out of
      // it after the call.
      RetBuf = TheTreeToLLVM->CreateTempLoc(PtrArgTy->getElementType());
      CallOperands.push_back(RetBuf.Ptr);

      // Note the use of a shadow argument.
      isShadowRet = true;
    }

    /// HandleScalarArgument - This is the primary callback that specifies an
    /// LLVM argument to pass.  It is only used for first class types.
    void HandleScalarArgument(const llvm::Type *LLVMTy, tree type,
                              unsigned RealSize = 0) {
      Value *Loc = NULL;
      if (RealSize) {
        Value *L = getAddress();
        Loc = LLVM_LOAD_SCALAR_ARGUMENT(L,LLVMTy,RealSize,Builder);
      } else
        Loc = getValue(LLVMTy);

      // Perform any implicit type conversions.
      if (CallOperands.size() < FTy->getNumParams()) {
        const Type *CalledTy= FTy->getParamType(CallOperands.size());
        if (Loc->getType() != CalledTy) {
          assert(type && "Inconsistent parameter types?");
          bool isSigned = !TYPE_UNSIGNED(type);
          Loc = TheTreeToLLVM->CastToAnyType(Loc, isSigned, CalledTy, false);
        }
      }

      CallOperands.push_back(Loc);
    }

    /// HandleByInvisibleReferenceArgument - This callback is invoked if a
    /// pointer (of type PtrTy) to the argument is passed rather than the
    /// argument itself.
    void HandleByInvisibleReferenceArgument(const llvm::Type *PtrTy, tree type){
      Value *Loc = getAddress();
      Loc = Builder.CreateBitCast(Loc, PtrTy);
      CallOperands.push_back(Loc);
    }

    /// HandleByValArgument - This callback is invoked if the aggregate function
    /// argument is passed by value. It is lowered to a parameter passed by
    /// reference with an additional parameter attribute "ByVal".
    void HandleByValArgument(const llvm::Type *LLVMTy, tree type) {
      Value *Loc = getAddress();
      assert(LLVMTy->getPointerTo() == Loc->getType());
      CallOperands.push_back(Loc);
    }

    /// HandleFCAArgument - This callback is invoked if the aggregate function
    /// argument is passed as a first class aggregate.
    void HandleFCAArgument(const llvm::Type *LLVMTy, tree /*type*/) {
      Value *Loc = getAddress();
      assert(LLVMTy->getPointerTo() == Loc->getType());
      CallOperands.push_back(Builder.CreateLoad(Loc));
    }

    /// EnterField - Called when we're about the enter the field of a struct
    /// or union.  FieldNo is the number of the element we are entering in the
    /// LLVM Struct, StructTy is the LLVM type of the struct we are entering.
    void EnterField(unsigned FieldNo, const llvm::Type *StructTy) {
      Value *Loc = getAddress();
      Loc = Builder.CreateBitCast(Loc, StructTy->getPointerTo());
      pushAddress(Builder.CreateStructGEP(Loc, FieldNo, "elt"));
    }
    void ExitField() {
      assert(!LocStack.empty());
      LocStack.pop_back();
    }
  };
}

/// EmitCallOf - Emit a call to the specified callee with the operands specified
/// in the CALL_EXP 'exp'.  If the result of the call is a scalar, return the
/// result, otherwise store it in DestLoc.
Value *TreeToLLVM::EmitCallOf(Value *Callee, tree exp, const MemRef *DestLoc,
                              const AttrListPtr &InPAL) {
  BasicBlock *LandingPad = 0; // Non-zero indicates an invoke.

  AttrListPtr PAL = InPAL;
  if (PAL.isEmpty() && isa<Function>(Callee))
    PAL = cast<Function>(Callee)->getAttributes();

  // Work out whether to use an invoke or an ordinary call.
  if (!tree_could_throw_p(exp))
    // This call does not throw - mark it 'nounwind'.
    PAL = PAL.addAttr(~0, Attribute::NoUnwind);

  if (!PAL.paramHasAttr(~0, Attribute::NoUnwind)) {
    // This call may throw.  Determine if we need to generate
    // an invoke rather than a simple call.
    int RegionNo = lookup_stmt_eh_region(exp);

    // Is the call contained in an exception handling region?
    if (RegionNo > 0) {
      // Are there any exception handlers for this region?
      if (can_throw_internal_1(RegionNo, false)) {
        // There are - turn the call into an invoke.
        LandingPads.grow(RegionNo);
        BasicBlock *&ThisPad = LandingPads[RegionNo];

        // Create a landing pad if one didn't exist already.
        if (!ThisPad)
          ThisPad = BasicBlock::Create(Context, "lpad");

        LandingPad = ThisPad;
      } else {
        assert(can_throw_external_1(RegionNo, false) &&
               "Must-not-throw region handled by runtime?");
      }
    }
  }

  tree fndecl = get_callee_fndecl(exp);
  tree fntype = fndecl ?
    TREE_TYPE(fndecl) : TREE_TYPE (TREE_TYPE(TREE_OPERAND (exp, 0)));

  // Determine the calling convention.
  CallingConv::ID CallingConvention = CallingConv::C;
#ifdef TARGET_ADJUST_LLVM_CC
  TARGET_ADJUST_LLVM_CC(CallingConvention, fntype);
#endif

  SmallVector<Value*, 16> CallOperands;
  const PointerType *PFTy = cast<PointerType>(Callee->getType());
  const FunctionType *FTy = cast<FunctionType>(PFTy->getElementType());
  FunctionCallArgumentConversion Client(CallOperands, FTy, DestLoc,
                                        CALL_EXPR_RETURN_SLOT_OPT(exp),
                                        Builder, CallingConvention);
  DefaultABI ABIConverter(Client);

  // Handle the result, including struct returns.
  std::vector<const Type*> ScalarArgs;
  ABIConverter.HandleReturnType(TREE_TYPE(exp),
                                fndecl ? fndecl : exp,
                                fndecl ? DECL_BUILT_IN(fndecl) : false,
                                ScalarArgs);

  // Pass the static chain, if any, as the first parameter.
  if (TREE_OPERAND(exp, 2))
    CallOperands.push_back(Emit(TREE_OPERAND(exp, 2), 0));

  // Loop over the arguments, expanding them and adding them to the op list.
  for (tree arg = TREE_OPERAND(exp, 1); arg; arg = TREE_CHAIN(arg)) {
    tree type = TREE_TYPE(TREE_VALUE(arg));
    const Type *ArgTy = ConvertType(type);

    // Push the argument.
    if (ArgTy->isSingleValueType()) {
      // A scalar - push the value.
      Client.pushValue(Emit(TREE_VALUE(arg), 0));
    } else if (LLVM_SHOULD_PASS_AGGREGATE_AS_FCA(type, ArgTy)) {
      // A first class aggregate - push the value.
      LValue ArgVal = EmitLV(TREE_VALUE(arg));
      Client.pushValue(Builder.CreateLoad(ArgVal.Ptr));
    } else {
      // An aggregate - push the address.
      LValue ArgVal = EmitLV(TREE_VALUE(arg));
      assert(!ArgVal.isBitfield() && "Bitfields are first-class types!");
      Client.pushAddress(ArgVal.Ptr);
    }

    Attributes Attrs = Attribute::None;

    unsigned OldSize = CallOperands.size();

    ABIConverter.HandleArgument(type, ScalarArgs, &Attrs);

    if (Attrs != Attribute::None) {
      // If the argument is split into multiple scalars, assign the
      // attributes to all scalars of the aggregate.
      for (unsigned i = OldSize + 1; i <= CallOperands.size(); ++i) {
        PAL = PAL.addAttr(i, Attrs);
      }
    }

    Client.clear();
  }

  // Compile stuff like:
  //   %tmp = call float (...)* bitcast (float ()* @foo to float (...)*)( )
  // to:
  //   %tmp = call float @foo( )
  // This commonly occurs due to C "implicit ..." semantics.
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(Callee)) {
    if (CallOperands.empty() && CE->getOpcode() == Instruction::BitCast) {
      Constant *RealCallee = CE->getOperand(0);
      assert(RealCallee->getType()->isPointerTy() &&
             "Bitcast to ptr not from ptr?");
      const PointerType *RealPT = cast<PointerType>(RealCallee->getType());
      if (const FunctionType *RealFT =
          dyn_cast<FunctionType>(RealPT->getElementType())) {
        const PointerType *ActualPT = cast<PointerType>(Callee->getType());
        const FunctionType *ActualFT =
          cast<FunctionType>(ActualPT->getElementType());
        if (RealFT->getReturnType() == ActualFT->getReturnType() &&
            RealFT->getNumParams() == 0)
          Callee = RealCallee;
      }
    }
  }

  Value *Call;
  if (!LandingPad) {
    Call = Builder.CreateCall(Callee, CallOperands.begin(), CallOperands.end());
    cast<CallInst>(Call)->setCallingConv(CallingConvention);
    cast<CallInst>(Call)->setAttributes(PAL);

    // LLVM LOCAL - begin radar 7885482
    /*
      Consider this pseudo-ObjC code:

      // locking using some "lock" variable
      @try {
        // ...
      } @catch (...) {
        // ...
        @throw;
      } @finally {
        // unlocking using the "lock" variable
      }

      The "lock" variable has live intervals from the top of the function
      through the @try block and in the @finally block. On 32-bit x86, it
      doesn't have a live interval in the @catch block. This is because in
      32-bit mode Objective-C uses setjmp/longjmp for exception handling and not
      the invoke/DWARF method.  The @throw is implemented as an
      "objc_exception_throw" call marked with NORETURN. The upshot is that if
      the "lock" variable is placed into a stack slot, there won't be an
      indication that the "lock" can be used after the "objc_exception_throw"
      executes. With the invoke/DWARF method, the unwind edge of the invoke
      points to the @finally block, so the "lock" variable will have a live
      interval leading to there.

      The solution is to have the "objc_exception_throw" behave in a similar
      manner to the invoke/DWARF method. That is remove the "NORETURN"
      attribute, allowing it to have an edge from the call to the @finally
      block.  */
    if (flag_objc_abi == 1 && 
        Callee->getName() == "objc_exception_throw")
      cast<CallInst>(Call)->removeAttribute(~0U, Attribute::NoReturn);
    // LLVM LOCAL - end radar 7885482
  } else {
    BasicBlock *NextBlock = BasicBlock::Create(Context, "invcont");
    Call = Builder.CreateInvoke(Callee, NextBlock, LandingPad,
                                CallOperands.begin(), CallOperands.end());
    cast<InvokeInst>(Call)->setCallingConv(CallingConvention);
    cast<InvokeInst>(Call)->setAttributes(PAL);
    EmitBlock(NextBlock);
  }

  if (Client.isShadowReturn())
    return Client.EmitShadowResult(TREE_TYPE(exp), DestLoc);

  if (Call->getType()->isVoidTy())
    return 0;

  if (Client.isAggrReturn()) {
    if (TD.getTypeAllocSize(Call->getType()) <= 
        TD.getTypeAllocSize(cast<PointerType>(DestLoc->Ptr->getType())
                                             ->getElementType())) {
      Value *Dest = BitCastToType(DestLoc->Ptr, Call->getType()->getPointerTo());
      LLVM_EXTRACT_MULTIPLE_RETURN_VALUE(Call,Dest,DestLoc->Volatile,Builder);
    } else {
      // The call will return an aggregate value in registers, but
      // those registers are bigger than DestLoc.  Allocate a
      // temporary to match the registers, store the registers there,
      // cast the temporary into the correct (smaller) type, and using
      // the correct type, copy the value into DestLoc.  Assume the
      // optimizer will delete the temporary and clean this up.
      AllocaInst *biggerTmp = CreateTemporary(Call->getType());
      LLVM_EXTRACT_MULTIPLE_RETURN_VALUE(Call,biggerTmp,/*Volatile=*/false,
                                         Builder);
      EmitAggregateCopy(*DestLoc,
                        MemRef(BitCastToType(biggerTmp,Call->getType()->
                                             getPointerTo()),
                               DestLoc->getAlignment(),
                               DestLoc->Volatile),
                        TREE_TYPE(exp));
    }
    return 0;
  }

  // If the caller expects an aggregate, we have a situation where the ABI for
  // the current target specifies that the aggregate be returned in scalar
  // registers even though it is an aggregate.  We must bitconvert the scalar
  // to the destination aggregate type.  We do this by casting the DestLoc
  // pointer and storing into it.  The store does not necessarily start at the
  // beginning of the aggregate (x86-64).
  if (!DestLoc)
    return Call;   // Normal scalar return.

  Value *Ptr = DestLoc->Ptr;
  if (Client.Offset) {
    Ptr = BitCastToType(Ptr, Type::getInt8PtrTy(Context));
    Ptr = Builder.CreateGEP(Ptr,
                    ConstantInt::get(TD.getIntPtrType(Context), Client.Offset));
  }
  Ptr = BitCastToType(Ptr, Call->getType()->getPointerTo());
  StoreInst *St = Builder.CreateStore(Call, Ptr, DestLoc->Volatile);
  St->setAlignment(DestLoc->getAlignment());
  return 0;
}

/// HandleMultiplyDefinedGimpleTemporary - Gimple temporaries *mostly* have a
/// single definition, in which case all uses are dominated by the definition.
/// This routine exists to handle the rare case of a gimple temporary with
/// multiple definitions.  It turns the temporary into an ordinary automatic
/// variable by creating an alloca for it, initializing the alloca with the
/// first definition that was seen, and fixing up any existing uses to load
/// the alloca instead.
///
void TreeToLLVM::HandleMultiplyDefinedGimpleTemporary(tree Var) {
  Value *UniqVal = DECL_LLVM(Var);
  assert(isa<CastInst>(UniqVal) && "Invalid value for gimple temporary!");
  Value *FirstVal = cast<CastInst>(UniqVal)->getOperand(0);

  // Create a new temporary and set the VAR_DECL to use it as the llvm location.
  Value *NewTmp = CreateTemporary(FirstVal->getType());
  SET_DECL_LLVM(Var, NewTmp);

  // Store the already existing initial value into the alloca.  If the value
  // being stored is an instruction, emit the store right after the instruction,
  // otherwise, emit it into the entry block.
  StoreInst *SI = new StoreInst(FirstVal, NewTmp);

  BasicBlock::iterator InsertPt;
  if (Instruction *I = dyn_cast<Instruction>(FirstVal)) {
    InsertPt = I;                      // Insert after the init instruction.

    bool InsertPtFinal = false;
    // If the instruction is an alloca in the entry block, the insert point
    // will be before the alloca.  Advance to the AllocaInsertionPoint if we are
    // before it.
    if (I->getParent() == &Fn->front()) {
      for (BasicBlock::iterator CI = InsertPt, E = Fn->begin()->end();
           CI != E; ++CI) {
        if (&*CI == AllocaInsertionPoint) {
          InsertPt = AllocaInsertionPoint;
          ++InsertPt;
          InsertPtFinal = true;  // This is the spot; stop searching.
          break;
        }
      }
    }

    if (!InsertPtFinal) {
      // If the instruction is an invoke, the init is inserted on the normal
      // edge.
      if (InvokeInst *II = dyn_cast<InvokeInst>(InsertPt)) {
        InsertPt = II->getNormalDest()->begin();
        while (isa<PHINode>(InsertPt))
          ++InsertPt;
      }
      else
        ++InsertPt; // Insert after the init instruction.
    }
  } else {
    InsertPt = AllocaInsertionPoint;   // Insert after the allocas.
    ++InsertPt;
  }
  BasicBlock *BB = InsertPt->getParent();
  BB->getInstList().insert(InsertPt, SI);

  // Replace any uses of the original value with a load of the alloca.
  for (Value::use_iterator U = UniqVal->use_begin(), E = UniqVal->use_end();
       U != E; ++U)
    U.getUse().set(new LoadInst(NewTmp, "mtmp", cast<Instruction>(*U)));

  // Finally, This is no longer a GCC temporary.
  DECL_GIMPLE_FORMAL_TEMP_P(Var) = 0;
}

/// EmitMODIFY_EXPR - Note that MODIFY_EXPRs are rvalues only!
/// We also handle INIT_EXPRs here; these are built by the C++ FE on rare
/// occasions, and have slightly different semantics that don't affect us here.
///
Value *TreeToLLVM::EmitMODIFY_EXPR(tree exp, const MemRef *DestLoc) {
  tree lhs = TREE_OPERAND (exp, 0);
  tree rhs = TREE_OPERAND (exp, 1);

  // Avoid generating stores of empty types.  This is rare, but necessary
  // to support the compressed_pair template.
  if (lang_hooks.empty_type_p(TREE_TYPE(exp))) {
    if (TREE_SIDE_EFFECTS(rhs)) {
      // There's a side-effect; alloc a temporary to receive the
      // value, if any.  Do not store into lhs; we must not
      // reference it.
      const Type *RHSTy = ConvertType(TREE_TYPE(rhs));
      MemRef dest = CreateTempLoc(RHSTy);
      return Emit(rhs, &dest);
    } else
      return (Value *)0;
  }

  // If this is the definition of a gimple temporary, set its DECL_LLVM to the
  // RHS.
  bool LHSSigned = !TYPE_UNSIGNED(TREE_TYPE(lhs));
  bool RHSSigned = !TYPE_UNSIGNED(TREE_TYPE(rhs));
  if (isGimpleTemporary(lhs)) {
    // If DECL_LLVM is already set, this is a multiply defined gimple temporary.
    if (DECL_LLVM_SET_P(lhs)) {
      HandleMultiplyDefinedGimpleTemporary(lhs);
      return EmitMODIFY_EXPR(exp, DestLoc);
    }
    Value *RHS = Emit(rhs, 0);
    const Type *LHSTy = ConvertType(TREE_TYPE(lhs));
    // The value may need to be replaced later if this temporary is multiply
    // defined - ensure it can be uniquely identified by not folding the cast.
    Instruction::CastOps opc = CastInst::getCastOpcode(RHS, RHSSigned,
                                                       LHSTy, LHSSigned);
    CastInst *Cast = CastInst::Create(opc, RHS, LHSTy, RHS->getName());
    if (opc == Instruction::BitCast && RHS->getType() == LHSTy)
      // Simplify this no-op bitcast once the function is emitted.
      UniquedValues.push_back(cast<BitCastInst>(Cast));
    Builder.Insert(Cast);
    SET_DECL_LLVM(lhs, Cast);
    return Cast;
  } else if (canEmitGlobalRegisterVariable(lhs)) {
    // If this is a store to a global register variable, EmitLV can't handle the
    // dest (there is no l-value of a global register variable).  Emit an inline
    // asm node that copies the value into the specified register.
    Value *RHS = Emit(rhs, 0);
    RHS = CastToAnyType(RHS, RHSSigned, ConvertType(TREE_TYPE(lhs)), LHSSigned);
    EmitModifyOfRegisterVariable(lhs, RHS);
    return RHS;
  }

  LValue LV = EmitLV(lhs);
  bool isVolatile = TREE_THIS_VOLATILE(lhs);
  unsigned Alignment = LV.getAlignment();

  if (!LV.isBitfield()) {
    const Type *ValTy = ConvertType(TREE_TYPE(rhs));
    if (ValTy->isSingleValueType()) {
      // Non-bitfield, scalar value.  Just emit a store.
      Value *RHS = Emit(rhs, 0);
      // Convert RHS to the right type if we can.  If LHS is bigger than RHS
      // we must convert; all the bits of LHS must be stored into.  Otherwise
      // convert the pointer.
      const PointerType *PT = cast<PointerType>(LV.Ptr->getType());
      if (PT->getElementType()->canLosslesslyBitCastTo(RHS->getType()) ||
          (PT->getElementType()->getPrimitiveSizeInBits() >
           RHS->getType()->getPrimitiveSizeInBits()))
        RHS = CastToAnyType(RHS, RHSSigned, PT->getElementType(), LHSSigned);
      else
        LV.Ptr = BitCastToType(LV.Ptr, RHS->getType()->getPointerTo());
      StoreInst *SI = Builder.CreateStore(RHS, LV.Ptr, isVolatile);
      SI->setAlignment(Alignment);
      return RHS;
    }

    // Non-bitfield aggregate value.
    MemRef NewLoc(LV.Ptr, Alignment, isVolatile);
    Emit(rhs, &NewLoc);

    if (DestLoc)
      EmitAggregateCopy(*DestLoc, NewLoc, TREE_TYPE(exp));

    return 0;
  }

  // Last case, this is a store to a bitfield, so we have to emit a
  // read/modify/write sequence.

  Value *RHS = Emit(rhs, 0);

  if (!LV.BitSize)
    return RHS;

  const Type *ValTy = cast<PointerType>(LV.Ptr->getType())->getElementType();
  unsigned ValSizeInBits = ValTy->getPrimitiveSizeInBits();

  // The number of stores needed to write the entire bitfield.
  unsigned Strides = 1 + (LV.BitStart + LV.BitSize - 1) / ValSizeInBits;

  assert(ValTy->isIntegerTy() && "Invalid bitfield lvalue!");
  assert(ValSizeInBits > LV.BitStart && "Bad bitfield lvalue!");
  assert(ValSizeInBits >= LV.BitSize && "Bad bitfield lvalue!");
  assert(2*ValSizeInBits > LV.BitSize+LV.BitStart && "Bad bitfield lvalue!");

  Value *BitSource = CastToAnyType(RHS, RHSSigned, ValTy, LHSSigned);

  for (unsigned I = 0; I < Strides; I++) {
    unsigned Index = BYTES_BIG_ENDIAN ? Strides - I - 1 : I; // LSB first
    unsigned ThisFirstBit = Index * ValSizeInBits;
    unsigned ThisLastBitPlusOne = ThisFirstBit + ValSizeInBits;
    if (ThisFirstBit < LV.BitStart)
      ThisFirstBit = LV.BitStart;
    if (ThisLastBitPlusOne > LV.BitStart+LV.BitSize)
      ThisLastBitPlusOne = LV.BitStart+LV.BitSize;

    Value *Ptr = Index ?
      Builder.CreateGEP(LV.Ptr, 
                        ConstantInt::get(Type::getInt32Ty(Context), Index)) :
      LV.Ptr;
    LoadInst *LI = Builder.CreateLoad(Ptr, isVolatile);
    LI->setAlignment(Alignment);
    Value *OldVal = LI;
    Value *NewVal = BitSource;

    unsigned BitsInVal = ThisLastBitPlusOne - ThisFirstBit;
    unsigned FirstBitInVal = ThisFirstBit % ValSizeInBits;

    if (BYTES_BIG_ENDIAN)
      FirstBitInVal = ValSizeInBits-FirstBitInVal-BitsInVal;

    // If not storing into the zero'th bit, shift the Src value to the left.
    if (FirstBitInVal) {
      Value *ShAmt = ConstantInt::get(ValTy, FirstBitInVal);
      NewVal = Builder.CreateShl(NewVal, ShAmt);
    }

    // Next, if this doesn't touch the top bit, mask out any bits that shouldn't
    // be set in the result.
    uint64_t MaskVal = ((1ULL << BitsInVal)-1) << FirstBitInVal;
    Constant *Mask = ConstantInt::get(Type::getInt64Ty(Context), MaskVal);
    Mask = Builder.getFolder().CreateTruncOrBitCast(Mask, ValTy);

    if (FirstBitInVal+BitsInVal != ValSizeInBits)
      NewVal = Builder.CreateAnd(NewVal, Mask);

    // Next, mask out the bits this bit-field should include from the old value.
    Mask = Builder.getFolder().CreateNot(Mask);
    OldVal = Builder.CreateAnd(OldVal, Mask);

    // Finally, merge the two together and store it.
    NewVal = Builder.CreateOr(OldVal, NewVal);

    StoreInst *SI = Builder.CreateStore(NewVal, Ptr, isVolatile);
    SI->setAlignment(Alignment);

    if (I + 1 < Strides) {
      Value *ShAmt = ConstantInt::get(ValTy, BitsInVal);
      BitSource = Builder.CreateLShr(BitSource, ShAmt);
    }
  }

  return RHS;
}

Value *TreeToLLVM::EmitFP16Conversion(tree fromty, tree toty, Value *V) {
  bool OpIsSigned = !TYPE_UNSIGNED(fromty);
  bool ExpIsSigned = !TYPE_UNSIGNED(toty);

  if (SCALAR_FLOAT_TYPE_P (fromty) && TYPE_PRECISION (fromty) == 16) {
    // HF -> something cast is performed as HF -> SF -> something
    Value *Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
						   Intrinsic::convert_from_fp16),
			 &V, &V+1);
    return CastToAnyType(Result, false, ConvertType(toty), ExpIsSigned);
  } else if (SCALAR_FLOAT_TYPE_P (toty) && TYPE_PRECISION (toty) == 16) {
    // something -> HF cast is performed as something -> SF -> HF
    V = CastToAnyType(V, OpIsSigned, Type::getFloatTy(Context), false);
    return Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
						Intrinsic::convert_to_fp16),
			      &V, &V+1);
  }

  return 0;
}


Value *TreeToLLVM::EmitNOP_EXPR(tree exp, const MemRef *DestLoc) {
  if (TREE_CODE(TREE_TYPE(exp)) == VOID_TYPE &&    // deleted statement.
      TREE_CODE(TREE_OPERAND(exp, 0)) == INTEGER_CST)
    return 0;
  tree Op = TREE_OPERAND(exp, 0);
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  bool OpIsSigned = !TYPE_UNSIGNED(TREE_TYPE(Op));
  bool ExpIsSigned = !TYPE_UNSIGNED(TREE_TYPE(exp));
  if (DestLoc == 0) {
    // Scalar to scalar copy.
    assert(!isAggregateTreeType(TREE_TYPE(Op))
	   && "Aggregate to scalar nop_expr!");
    Value *OpVal = Emit(Op, DestLoc);
    if (Ty->isVoidTy()) return 0;

    // Special case: something <-> HF casts are made via special intrinsics.
    if (Value *V = EmitFP16Conversion(TREE_TYPE(Op), TREE_TYPE(exp), OpVal))
      return V;

    return CastToAnyType(OpVal, OpIsSigned, Ty, ExpIsSigned);
  } else if (isAggregateTreeType(TREE_TYPE(Op))) {
    // Aggregate to aggregate copy.
    MemRef NewLoc = *DestLoc;
    NewLoc.Ptr = BitCastToType(DestLoc->Ptr, Ty->getPointerTo());
    Value *OpVal = Emit(Op, &NewLoc);
    assert(OpVal == 0 && "Shouldn't cast scalar to aggregate!");
    return 0;
  }

  // Scalar to aggregate copy.
  Value *OpVal = Emit(Op, 0);
  Value *Ptr = BitCastToType(DestLoc->Ptr, OpVal->getType()->getPointerTo());
  StoreInst *St = Builder.CreateStore(OpVal, Ptr, DestLoc->Volatile);
  St->setAlignment(DestLoc->getAlignment());
  return 0;
}

Value *TreeToLLVM::EmitCONVERT_EXPR(tree exp, const MemRef *DestLoc) {
  assert(!DestLoc && "Cannot handle aggregate casts!");
  tree op0 = TREE_OPERAND(exp, 0);
  tree opty = TREE_TYPE(op0), expty = TREE_TYPE(exp);
  Value *Op = Emit(op0, 0);

  // Special case: something <-> HF casts are made via special intrinsics.
  if (Value *V = EmitFP16Conversion(opty, expty, Op))
    return V;

  bool OpIsSigned = !TYPE_UNSIGNED(opty), ExpIsSigned = !TYPE_UNSIGNED(expty);
  return CastToAnyType(Op, OpIsSigned, ConvertType(expty), ExpIsSigned);
}

Value *TreeToLLVM::EmitVIEW_CONVERT_EXPR(tree exp, const MemRef *DestLoc) {
  tree Op = TREE_OPERAND(exp, 0);

  if (isAggregateTreeType(TREE_TYPE(Op))) {
    MemRef Target;
    if (DestLoc)
      // This is an aggregate-to-agg VIEW_CONVERT_EXPR, just evaluate in place.
      Target = *DestLoc;
    else
      // This is an aggregate-to-scalar VIEW_CONVERT_EXPR, evaluate, then load.
      Target = CreateTempLoc(ConvertType(TREE_TYPE(exp)));

    // Make the destination look like the source type.
    const Type *OpTy = ConvertType(TREE_TYPE(Op));
    Target.Ptr = BitCastToType(Target.Ptr, OpTy->getPointerTo());

    // Needs to be in sync with EmitLV.
    switch (TREE_CODE(Op)) {
    default: {
      Value *OpVal = Emit(Op, &Target);
      assert(OpVal == 0 && "Expected an aggregate operand!");
      break;
    }

    // Lvalues
    case VAR_DECL:
    case PARM_DECL:
    case RESULT_DECL:
    case INDIRECT_REF:
    case ARRAY_REF:
    case ARRAY_RANGE_REF:
    case COMPONENT_REF:
    case BIT_FIELD_REF:
    case STRING_CST:
    case REALPART_EXPR:
    case IMAGPART_EXPR:
      // Same as EmitLoadOfLValue but taking the size from TREE_TYPE(exp), since
      // the size of TREE_TYPE(Op) may not be available.
      LValue LV = EmitLV(Op);
      assert(!LV.isBitfield() && "Expected an aggregate operand!");
      bool isVolatile = TREE_THIS_VOLATILE(Op);
      unsigned Alignment = LV.getAlignment();

      EmitAggregateCopy(Target, MemRef(LV.Ptr, Alignment, isVolatile),
                        TREE_TYPE(exp));
      break;
    }

    if (DestLoc)
      return 0;

    // Target holds the temporary created above.
    const Type *ExpTy = ConvertType(TREE_TYPE(exp));
    return Builder.CreateLoad(BitCastToType(Target.Ptr,
                                          ExpTy->getPointerTo()));
  }

  if (DestLoc) {
    // The input is a scalar the output is an aggregate, just eval the input,
    // then store into DestLoc.
    Value *OpVal = Emit(Op, 0);
    assert(OpVal && "Expected a scalar result!");
    Value *Ptr = BitCastToType(DestLoc->Ptr, OpVal->getType()->getPointerTo());
    StoreInst *St = Builder.CreateStore(OpVal, Ptr, DestLoc->Volatile);
    St->setAlignment(DestLoc->getAlignment());
    return 0;
  }

  // Otherwise, this is a scalar to scalar conversion.
  Value *OpVal = Emit(Op, 0);
  assert(OpVal && "Expected a scalar result!");
  const Type *DestTy = ConvertType(TREE_TYPE(exp));

  // If the source is a pointer, use ptrtoint to get it to something
  // bitcast'able.  This supports things like v_c_e(foo*, float).
  if (OpVal->getType()->isPointerTy()) {
    if (DestTy->isPointerTy())   // ptr->ptr is a simple bitcast.
      return Builder.CreateBitCast(OpVal, DestTy);
    // Otherwise, ptrtoint to intptr_t first.
    OpVal = Builder.CreatePtrToInt(OpVal, TD.getIntPtrType(Context));
  }

  // If the destination type is a pointer, use inttoptr.
  if (DestTy->isPointerTy())
    return Builder.CreateIntToPtr(OpVal, DestTy);

  // Otherwise, use a bitcast.
  return Builder.CreateBitCast(OpVal, DestTy);
}

Value *TreeToLLVM::EmitNEGATE_EXPR(tree exp, const MemRef *DestLoc) {
  if (!DestLoc) {
    Value *V = Emit(TREE_OPERAND(exp, 0), 0);
    if (V->getType()->isFPOrFPVectorTy())
      return Builder.CreateFNeg(V);
    if (!V->getType()->isPointerTy()) {
      bool HasNSW = !TYPE_OVERFLOW_WRAPS(TREE_TYPE(exp));
      return HasNSW ? Builder.CreateNSWNeg(V) : Builder.CreateNeg(V);
    }

    // GCC allows NEGATE_EXPR on pointers as well.  Cast to int, negate, cast
    // back.
    V = CastToAnyType(V, false, TD.getIntPtrType(Context), false);
    V = Builder.CreateNeg(V);
    return CastToType(Instruction::IntToPtr, V, ConvertType(TREE_TYPE(exp)));
  }

  // Emit the operand to a temporary.
  const Type *ComplexTy =
    cast<PointerType>(DestLoc->Ptr->getType())->getElementType();
  MemRef Tmp = CreateTempLoc(ComplexTy);
  Emit(TREE_OPERAND(exp, 0), &Tmp);

  // Handle complex numbers: -(a+ib) = -a + i*-b
  Value *R, *I;
  EmitLoadFromComplex(R, I, Tmp);
  if (R->getType()->isFloatingPointTy()) {
    R = Builder.CreateFNeg(R);
    I = Builder.CreateFNeg(I);
  } else {
    R = Builder.CreateNeg(R);
    I = Builder.CreateNeg(I);
  }
  EmitStoreToComplex(*DestLoc, R, I);
  return 0;
}

Value *TreeToLLVM::EmitCONJ_EXPR(tree exp, const MemRef *DestLoc) {
  assert(DestLoc && "CONJ_EXPR only applies to complex numbers.");
  // Emit the operand to a temporary.
  const Type *ComplexTy =
    cast<PointerType>(DestLoc->Ptr->getType())->getElementType();
  MemRef Tmp = CreateTempLoc(ComplexTy);
  Emit(TREE_OPERAND(exp, 0), &Tmp);

  // Handle complex numbers: ~(a+ib) = a + i*-b
  Value *R, *I;
  EmitLoadFromComplex(R, I, Tmp);
  if (I->getType()->isFloatingPointTy())
    I = Builder.CreateFNeg(I);
  else
    I = Builder.CreateNeg(I);
  EmitStoreToComplex(*DestLoc, R, I);
  return 0;
}

Value *TreeToLLVM::EmitABS_EXPR(tree exp) {
  Value *Op = Emit(TREE_OPERAND(exp, 0), 0);
  if (!Op->getType()->isFloatingPointTy()) {
    Value *OpN = Builder.CreateNeg(Op, (Op->getNameStr()+"neg").c_str());
    ICmpInst::Predicate pred = TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 0))) ?
      ICmpInst::ICMP_UGE : ICmpInst::ICMP_SGE;
    Value *Cmp = Builder.CreateICmp(pred, Op,
                    Constant::getNullValue(Op->getType()), "abscond");
    return Builder.CreateSelect(Cmp, Op, OpN, "abs");
  }

  // Turn FP abs into fabs/fabsf.
  const char *Name = 0;

  tree ArgType;
  switch (Op->getType()->getTypeID()) {
  default: assert(0 && "Unknown FP type!");
  case Type::FloatTyID:
    Name = "fabsf";
    ArgType = float_type_node;
    break;
  case Type::DoubleTyID:
    Name = "fabs";
    ArgType = double_type_node;
    break;
  case Type::X86_FP80TyID:
  case Type::PPC_FP128TyID:
  case Type::FP128TyID:
    Name = "fabsl";
    ArgType = long_double_type_node;
    break;
  }

  Value *V = TheModule->getOrInsertFunction(Name, Op->getType(), Op->getType(),
                                            NULL);
  // Determine the calling convention.
  CallingConv::ID CallingConvention = CallingConv::C;
#ifdef TARGET_ADJUST_LLVM_CC
  tree FunctionType = build_function_type_list(ArgType, ArgType ,NULL);
  TARGET_ADJUST_LLVM_CC(CallingConvention, FunctionType);
#endif

  Function *F = cast<Function>(V);
  F->setCallingConv(CallingConvention);
  CallInst *Call = Builder.CreateCall(V, Op);
  Call->setDoesNotThrow();
  Call->setDoesNotAccessMemory();
  Call->setCallingConv(CallingConvention);
  return Call;
}

/// getSuitableBitCastIntType - Given Ty is a floating point type or a vector
/// type with floating point elements, return an integer type to bitcast to.
/// e.g. 4 x float -> 4 x i32
static const Type *getSuitableBitCastIntType(const Type *Ty) {
  if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
    unsigned NumElements = VTy->getNumElements();
    const Type *EltTy = VTy->getElementType();
    return VectorType::get(
      IntegerType::get(Context, EltTy->getPrimitiveSizeInBits()), NumElements);
  }
  return IntegerType::get(Context, Ty->getPrimitiveSizeInBits());
}

Value *TreeToLLVM::EmitBIT_NOT_EXPR(tree exp) {
  Value *Op = Emit(TREE_OPERAND(exp, 0), 0);
  const Type *Ty = Op->getType();
  if (Ty->isPointerTy()) {
    assert (TREE_CODE(TREE_TYPE(exp)) == INTEGER_TYPE &&
            "Expected integer type here");
    Ty = ConvertType(TREE_TYPE(exp));
    Op = CastToType(Instruction::PtrToInt, Op, Ty);
  } else if (Ty->isFloatingPointTy() ||
             (Ty->isVectorTy() &&
              cast<VectorType>(Ty)->getElementType()->isFloatingPointTy())) {
    Op = BitCastToType(Op, getSuitableBitCastIntType(Ty));
  }
  return BitCastToType(Builder.CreateNot(Op,
                                         (Op->getNameStr()+"not").c_str()),Ty);
}

Value *TreeToLLVM::EmitTRUTH_NOT_EXPR(tree exp) {
  Value *V = Emit(TREE_OPERAND(exp, 0), 0);
  if (V->getType() != Type::getInt1Ty(Context))
    V = Builder.CreateICmpNE(V,
          Constant::getNullValue(V->getType()), "toBool");
  V = Builder.CreateNot(V, (V->getNameStr()+"not").c_str());
  return CastToUIntType(V, ConvertType(TREE_TYPE(exp)));
}

/// EmitCompare - 'exp' is a comparison of two values.  Opc is the base LLVM
/// comparison to use.  isUnord is true if this is a floating point comparison
/// that should also be true if either operand is a NaN.  Note that Opc can be
/// set to zero for special cases.
///
/// If DestTy is specified, make sure to return the result with the specified
/// integer type.  Otherwise, return the expression as whatever TREE_TYPE(exp)
/// corresponds to.
Value *TreeToLLVM::EmitCompare(tree exp, unsigned UIOpc, unsigned SIOpc,
                               unsigned FPPred, const Type *DestTy) {
  // Get the type of the operands
  tree lhsty = TREE_TYPE(TREE_OPERAND(exp, 0));
  tree rhsty = TREE_TYPE(TREE_OPERAND(exp, 1));

  Value *Result;

  // Deal with complex types
  if (TREE_CODE(lhsty) == COMPLEX_TYPE) {
    Result = EmitComplexBinOp(exp, 0);  // Complex ==/!=
  } else {
    // Get the compare operands, in the right type. Comparison of struct is not
    // allowed, so this is safe as we already handled complex (struct) type.
    Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
    Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

    // FIXME: This should be generalized for other storage-only types, if any.
    if (Value *V = EmitFP16Conversion(lhsty, float_type_node, LHS)) {
      LHS = V; lhsty = float_type_node;
    }
    if (Value *V = EmitFP16Conversion(rhsty, float_type_node, RHS)) {
      RHS = V; rhsty = float_type_node;
    }

    bool LHSIsSigned = !TYPE_UNSIGNED(lhsty);
    bool RHSIsSigned = !TYPE_UNSIGNED(rhsty);
    RHS = CastToAnyType(RHS, RHSIsSigned, LHS->getType(), LHSIsSigned);
    assert(LHS->getType() == RHS->getType() && "Binop type equality failure!");

    if (FLOAT_TYPE_P(lhsty)) {
      // Handle floating point comparisons, if we get here.
      Result = Builder.CreateFCmp(FCmpInst::Predicate(FPPred), LHS, RHS);
    } else {
      // Handle the integer/pointer cases.  Determine which predicate to use 
      // based on signedness.
      ICmpInst::Predicate pred = ICmpInst::Predicate(LHSIsSigned ? SIOpc:UIOpc);

      // Get the compare instructions
      Result = Builder.CreateICmp(pred, LHS, RHS);
    }
  }
  assert(Result->getType() == Type::getInt1Ty(Context) && 
         "Expected i1 result for compare");

  if (DestTy == 0)
    DestTy = ConvertType(TREE_TYPE(exp));

  // The GCC type is probably an int, not a bool.  ZExt to the right size.
  if (Result->getType() == DestTy)
    return Result;
  return Builder.CreateZExt(Result, DestTy);
}

/// EmitBinOp - 'exp' is a binary operator.
///
Value *TreeToLLVM::EmitBinOp(tree exp, const MemRef *DestLoc, unsigned Opc) {
  tree expty = TREE_TYPE(exp);
  const Type *Ty = ConvertType(expty);
  if (Ty->isPointerTy())
    return EmitPtrBinOp(exp, Opc);   // Pointer arithmetic!
  if (Ty->isStructTy())
    return EmitComplexBinOp(exp, DestLoc);
  assert(Ty->isSingleValueType() && DestLoc == 0 &&
	 "Bad binary operation!");

  tree lhsty = TREE_TYPE(TREE_OPERAND(exp, 0));
  tree rhsty = TREE_TYPE(TREE_OPERAND(exp, 1));

  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  // FIXME: This should be generalized for other storage-only types, if any.
  if (Value *V = EmitFP16Conversion(lhsty, float_type_node, LHS)) {
    LHS = V; lhsty = float_type_node; Ty = Type::getFloatTy(Context);
  }
  if (Value *V = EmitFP16Conversion(rhsty, float_type_node, RHS)) {
    RHS = V; rhsty = float_type_node; Ty = Type::getFloatTy(Context);
  }

  // GCC has no problem with things like "xor uint X, int 17", and X-Y, where
  // X and Y are pointer types, but the result is an integer.  As such, convert
  // everything to the result type.
  bool LHSIsSigned = !TYPE_UNSIGNED(lhsty);
  bool RHSIsSigned = !TYPE_UNSIGNED(rhsty);
  bool TyIsSigned  = !TYPE_UNSIGNED(expty);
  bool IsExactDiv  = TREE_CODE(exp) == EXACT_DIV_EXPR;

  LHS = CastToAnyType(LHS, LHSIsSigned, Ty, TyIsSigned);
  RHS = CastToAnyType(RHS, RHSIsSigned, Ty, TyIsSigned);

  // If it's And, Or, or Xor, make sure the operands are casted to the right
  // integer types first.
  bool isLogicalOp = Opc == Instruction::And || Opc == Instruction::Or ||
    Opc == Instruction::Xor;
  const Type *ResTy = Ty;
  if (isLogicalOp &&
      (Ty->isFloatingPointTy() ||
       (Ty->isVectorTy() &&
	cast<VectorType>(Ty)->getElementType()->isFloatingPointTy()))) {
    Ty = getSuitableBitCastIntType(Ty);
    LHS = BitCastToType(LHS, Ty);
    RHS = BitCastToType(RHS, Ty);
  }

  Value *V;
  if (Opc == Instruction::SDiv && IsExactDiv)
    V = Builder.CreateExactSDiv(LHS, RHS);
  else if (Opc == Instruction::Add && !TYPE_OVERFLOW_WRAPS(expty))
    V = Builder.CreateNSWAdd(LHS, RHS);
  else if (Opc == Instruction::Sub && !TYPE_OVERFLOW_WRAPS(expty))
    V = Builder.CreateNSWSub(LHS, RHS);
  else if (Opc == Instruction::Mul && !TYPE_OVERFLOW_WRAPS(expty))
    V = Builder.CreateNSWMul(LHS, RHS);
  else
    V = Builder.CreateBinOp((Instruction::BinaryOps)Opc, LHS, RHS);
  if (ResTy != Ty)
    V = BitCastToType(V, ResTy);

  // FIXME: This should be generalized for other storage-only types, if any.
  if (Value *CV = EmitFP16Conversion(float_type_node, expty, V))
    V = CV;

  return V;
}

/// EmitPtrBinOp - Handle binary expressions involving pointers, e.g. "P+4".
///
Value *TreeToLLVM::EmitPtrBinOp(tree exp, unsigned Opc) {
  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);

  // If this is an expression like (P+4), try to turn this into
  // "getelementptr P, 1".
  if ((Opc == Instruction::Add || Opc == Instruction::Sub) &&
      TREE_CODE(TREE_OPERAND(exp, 1)) == INTEGER_CST) {
    int64_t Offset = getINTEGER_CSTVal(TREE_OPERAND(exp, 1));

    // If POINTER_SIZE is 32-bits and the offset is signed, sign extend it.
    if (POINTER_SIZE == 32 && !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 1))))
      Offset = (Offset << 32) >> 32;

    // Figure out how large the element pointed to is.
    const Type *ElTy = cast<PointerType>(LHS->getType())->getElementType();
    // We can't get the type size (and thus convert to using a GEP instr) from
    // pointers to opaque structs if the type isn't abstract.
    if (ElTy->isSized()) {
      int64_t EltSize = TD.getTypeAllocSize(ElTy);

      // If EltSize exactly divides Offset, then we know that we can turn this
      // into a getelementptr instruction.
      int64_t EltOffset = EltSize ? Offset/EltSize : 0;
      if (EltOffset*EltSize == Offset) {
        // If this is a subtract, we want to step backwards.
        if (Opc == Instruction::Sub)
          EltOffset = -EltOffset;
        Constant *C = ConstantInt::get(Type::getInt64Ty(Context), EltOffset);
        Value *V = flag_wrapv ?
                   Builder.CreateGEP(LHS, C) :
                   Builder.CreateInBoundsGEP(LHS, C);
        return BitCastToType(V, ConvertType(TREE_TYPE(exp)));
      }
    }
  }


  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  const Type *IntPtrTy = TD.getIntPtrType(Context);
  bool LHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 0)));
  bool RHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 1)));
  LHS = CastToAnyType(LHS, LHSIsSigned, IntPtrTy, false);
  RHS = CastToAnyType(RHS, RHSIsSigned, IntPtrTy, false);
  Value *V = Builder.CreateBinOp((Instruction::BinaryOps)Opc, LHS, RHS);
  return CastToType(Instruction::IntToPtr, V, ConvertType(TREE_TYPE(exp)));
}


Value *TreeToLLVM::EmitTruthOp(tree exp, unsigned Opc) {
  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  // This is a truth operation like the strict &&,||,^^.  Convert to bool as
  // a test against zero
  LHS = Builder.CreateICmpNE(LHS,
                             Constant::getNullValue(LHS->getType()),
                             "toBool");
  RHS = Builder.CreateICmpNE(RHS,
                             Constant::getNullValue(RHS->getType()),
                             "toBool");

  Value *Res = Builder.CreateBinOp((Instruction::BinaryOps)Opc, LHS, RHS);
  return CastToType(Instruction::ZExt, Res, ConvertType(TREE_TYPE(exp)));
}


Value *TreeToLLVM::EmitShiftOp(tree exp, const MemRef *DestLoc, unsigned Opc) {
  assert(DestLoc == 0 && "aggregate shift?");
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  assert(!Ty->isPointerTy() && "Pointer arithmetic!?");

  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);
  if (RHS->getType() != LHS->getType())
    RHS = Builder.CreateIntCast(RHS, LHS->getType(), false,
                                (RHS->getNameStr()+".cast").c_str());

  return Builder.CreateBinOp((Instruction::BinaryOps)Opc, LHS, RHS);
}

Value *TreeToLLVM::EmitRotateOp(tree exp, unsigned Opc1, unsigned Opc2) {
  Value *In  = Emit(TREE_OPERAND(exp, 0), 0);
  Value *Amt = Emit(TREE_OPERAND(exp, 1), 0);

  if (In->getType()->isPointerTy()) {
    const Type *Ty =
      IntegerType::get(Context,
                       TYPE_PRECISION(TREE_TYPE (TREE_OPERAND (exp, 0))));
    In = Builder.CreatePtrToInt(In, Ty,
                                (In->getNameStr()+".cast").c_str());
  }

  if (Amt->getType() != In->getType())
    Amt = Builder.CreateIntCast(Amt, In->getType(), false,
                                (Amt->getNameStr()+".cast").c_str());

  Value *TypeSize =
    ConstantInt::get(In->getType(),
                     In->getType()->getPrimitiveSizeInBits());

  // Do the two shifts.
  Value *V1 = Builder.CreateBinOp((Instruction::BinaryOps)Opc1, In, Amt);
  Value *OtherShift = Builder.CreateSub(TypeSize, Amt);
  Value *V2 = Builder.CreateBinOp((Instruction::BinaryOps)Opc2, In, OtherShift);

  // Or the two together to return them.
  Value *Merge = Builder.CreateOr(V1, V2);
  return CastToUIntType(Merge, ConvertType(TREE_TYPE(exp)));
}

Value *TreeToLLVM::EmitMinMaxExpr(tree exp, unsigned UIPred, unsigned SIPred,
                                  unsigned FPPred) {
  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  const Type *Ty = ConvertType(TREE_TYPE(exp));

  // The LHS, RHS and Ty could be integer, floating or pointer typed. We need
  // to convert the LHS and RHS into the destination type before doing the
  // comparison. Use CastInst::getCastOpcode to get this right.
  bool TyIsSigned  = !TYPE_UNSIGNED(TREE_TYPE(exp));
  bool LHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 0)));
  bool RHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 1)));
  Instruction::CastOps opcode =
    CastInst::getCastOpcode(LHS, LHSIsSigned, Ty, TyIsSigned);
  LHS = CastToType(opcode, LHS, Ty);
  opcode = CastInst::getCastOpcode(RHS, RHSIsSigned, Ty, TyIsSigned);
  RHS = CastToType(opcode, RHS, Ty);

  Value *Compare;
  if (LHS->getType()->isFloatingPointTy())
    Compare = Builder.CreateFCmp(FCmpInst::Predicate(FPPred), LHS, RHS);
  else if (TYPE_UNSIGNED(TREE_TYPE(exp)))
    Compare = Builder.CreateICmp(ICmpInst::Predicate(UIPred), LHS, RHS);
  else
    Compare = Builder.CreateICmp(ICmpInst::Predicate(SIPred), LHS, RHS);

  return Builder.CreateSelect(Compare, LHS, RHS,
                              TREE_CODE(exp) == MAX_EXPR ? "max" : "min");
}

Value *TreeToLLVM::EmitEXACT_DIV_EXPR(tree exp, const MemRef *DestLoc) {
  // Unsigned EXACT_DIV_EXPR -> normal udiv.
  if (TYPE_UNSIGNED(TREE_TYPE(exp)))
    return EmitBinOp(exp, DestLoc, Instruction::UDiv);

  // Otherwise, emit this as a normal signed divide.
  return EmitBinOp(exp, DestLoc, Instruction::SDiv);
}

Value *TreeToLLVM::EmitFLOOR_MOD_EXPR(tree exp, const MemRef *DestLoc) {
  // Notation: FLOOR_MOD_EXPR <-> Mod, TRUNC_MOD_EXPR <-> Rem.

  // We express Mod in terms of Rem as follows: if RHS exactly divides LHS,
  // or the values of LHS and RHS have the same sign, then Mod equals Rem.
  // Otherwise Mod equals Rem + RHS.  This means that LHS Mod RHS traps iff
  // LHS Rem RHS traps.
  if (TYPE_UNSIGNED(TREE_TYPE(exp)))
    // LHS and RHS values must have the same sign if their type is unsigned.
    return EmitBinOp(exp, DestLoc, Instruction::URem);

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  Constant *Zero = ConstantInt::get(Ty, 0);

  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  // The two possible values for Mod.
  Value *Rem = Builder.CreateSRem(LHS, RHS, "rem");
  Value *RemPlusRHS = Builder.CreateAdd(Rem, RHS);

  // HaveSameSign: (LHS >= 0) == (RHS >= 0).
  Value *LHSIsPositive = Builder.CreateICmpSGE(LHS, Zero);
  Value *RHSIsPositive = Builder.CreateICmpSGE(RHS, Zero);
  Value *HaveSameSign = Builder.CreateICmpEQ(LHSIsPositive,RHSIsPositive);

  // RHS exactly divides LHS iff Rem is zero.
  Value *RemIsZero = Builder.CreateICmpEQ(Rem, Zero);

  Value *SameAsRem = Builder.CreateOr(HaveSameSign, RemIsZero);
  return Builder.CreateSelect(SameAsRem, Rem, RemPlusRHS, "mod");
}

Value *TreeToLLVM::EmitCEIL_DIV_EXPR(tree exp) {
  // Notation: CEIL_DIV_EXPR <-> CDiv, TRUNC_DIV_EXPR <-> Div.

  // CDiv calculates LHS/RHS by rounding up to the nearest integer.  In terms
  // of Div this means if the values of LHS and RHS have opposite signs or if
  // LHS is zero, then CDiv necessarily equals Div; and
  //   LHS CDiv RHS = (LHS - Sign(RHS)) Div RHS + 1
  // otherwise.

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  Constant *Zero = ConstantInt::get(Ty, 0);
  Constant *One = ConstantInt::get(Ty, 1);
  Constant *MinusOne = Constant::getAllOnesValue(Ty);

  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  if (!TYPE_UNSIGNED(TREE_TYPE(exp))) {
    // In the case of signed arithmetic, we calculate CDiv as follows:
    //   LHS CDiv RHS = (LHS - Sign(RHS) * Offset) Div RHS + Offset,
    // where Offset is 1 if LHS and RHS have the same sign and LHS is
    // not zero, and 0 otherwise.

    // On some machines INT_MIN Div -1 traps.  You might expect a trap for
    // INT_MIN CDiv -1 too, but this implementation will not generate one.
    // Quick quiz question: what value is returned for INT_MIN CDiv -1?

    // Determine the signs of LHS and RHS, and whether they have the same sign.
    Value *LHSIsPositive = Builder.CreateICmpSGE(LHS, Zero);
    Value *RHSIsPositive = Builder.CreateICmpSGE(RHS, Zero);
    Value *HaveSameSign = Builder.CreateICmpEQ(LHSIsPositive, RHSIsPositive);

    // Offset equals 1 if LHS and RHS have the same sign and LHS is not zero.
    Value *LHSNotZero = Builder.CreateICmpNE(LHS, Zero);
    Value *OffsetOne = Builder.CreateAnd(HaveSameSign, LHSNotZero);
    // ... otherwise it is 0.
    Value *Offset = Builder.CreateSelect(OffsetOne, One, Zero);

    // Calculate Sign(RHS) ...
    Value *SignRHS = Builder.CreateSelect(RHSIsPositive, One, MinusOne);
    // ... and Sign(RHS) * Offset
    Value *SignedOffset = CastToType(Instruction::SExt, OffsetOne, Ty);
    SignedOffset = Builder.CreateAnd(SignRHS, SignedOffset);

    // Return CDiv = (LHS - Sign(RHS) * Offset) Div RHS + Offset.
    Value *CDiv = Builder.CreateSub(LHS, SignedOffset);
    CDiv = Builder.CreateSDiv(CDiv, RHS);
    return Builder.CreateAdd(CDiv, Offset, "cdiv");
  }

  // In the case of unsigned arithmetic, LHS and RHS necessarily have the
  // same sign, so we can use
  //   LHS CDiv RHS = (LHS - 1) Div RHS + 1
  // as long as LHS is non-zero.

  // Offset is 1 if LHS is non-zero, 0 otherwise.
  Value *LHSNotZero = Builder.CreateICmpNE(LHS, Zero);
  Value *Offset = Builder.CreateSelect(LHSNotZero, One, Zero);

  // Return CDiv = (LHS - Offset) Div RHS + Offset.
  Value *CDiv = Builder.CreateSub(LHS, Offset);
  CDiv = Builder.CreateUDiv(CDiv, RHS);
  return Builder.CreateAdd(CDiv, Offset, "cdiv");
}

Value *TreeToLLVM::EmitFLOOR_DIV_EXPR(tree exp) {
  // Notation: FLOOR_DIV_EXPR <-> FDiv, TRUNC_DIV_EXPR <-> Div.
  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  // FDiv calculates LHS/RHS by rounding down to the nearest integer.  In terms
  // of Div this means if the values of LHS and RHS have the same sign or if LHS
  // is zero, then FDiv necessarily equals Div; and
  //   LHS FDiv RHS = (LHS + Sign(RHS)) Div RHS - 1
  // otherwise.

  if (TYPE_UNSIGNED(TREE_TYPE(exp)))
    // In the case of unsigned arithmetic, LHS and RHS necessarily have the
    // same sign, so FDiv is the same as Div.
    return Builder.CreateUDiv(LHS, RHS, "fdiv");

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  Constant *Zero = ConstantInt::get(Ty, 0);
  Constant *One = ConstantInt::get(Ty, 1);
  Constant *MinusOne = Constant::getAllOnesValue(Ty);

  // In the case of signed arithmetic, we calculate FDiv as follows:
  //   LHS FDiv RHS = (LHS + Sign(RHS) * Offset) Div RHS - Offset,
  // where Offset is 1 if LHS and RHS have opposite signs and LHS is
  // not zero, and 0 otherwise.

  // Determine the signs of LHS and RHS, and whether they have the same sign.
  Value *LHSIsPositive = Builder.CreateICmpSGE(LHS, Zero);
  Value *RHSIsPositive = Builder.CreateICmpSGE(RHS, Zero);
  Value *SignsDiffer = Builder.CreateICmpNE(LHSIsPositive, RHSIsPositive);

  // Offset equals 1 if LHS and RHS have opposite signs and LHS is not zero.
  Value *LHSNotZero = Builder.CreateICmpNE(LHS, Zero);
  Value *OffsetOne = Builder.CreateAnd(SignsDiffer, LHSNotZero);
  // ... otherwise it is 0.
  Value *Offset = Builder.CreateSelect(OffsetOne, One, Zero);

  // Calculate Sign(RHS) ...
  Value *SignRHS = Builder.CreateSelect(RHSIsPositive, One, MinusOne);
  // ... and Sign(RHS) * Offset
  Value *SignedOffset = CastToType(Instruction::SExt, OffsetOne, Ty);
  SignedOffset = Builder.CreateAnd(SignRHS, SignedOffset);

  // Return FDiv = (LHS + Sign(RHS) * Offset) Div RHS - Offset.
  Value *FDiv = Builder.CreateAdd(LHS, SignedOffset);
  FDiv = Builder.CreateSDiv(FDiv, RHS);
  return Builder.CreateSub(FDiv, Offset, "fdiv");
}

Value *TreeToLLVM::EmitROUND_DIV_EXPR(tree exp) {
  // Notation: ROUND_DIV_EXPR <-> RDiv, TRUNC_DIV_EXPR <-> Div.

  // RDiv calculates LHS/RHS by rounding to the nearest integer.  Ties
  // are broken by rounding away from zero.  In terms of Div this means:
  //   LHS RDiv RHS = (LHS + (RHS Div 2)) Div RHS
  // if the values of LHS and RHS have the same sign; and
  //   LHS RDiv RHS = (LHS - (RHS Div 2)) Div RHS
  // if the values of LHS and RHS differ in sign.  The intermediate
  // expressions in these formulae can overflow, so some tweaking is
  // required to ensure correct results.  The details depend on whether
  // we are doing signed or unsigned arithmetic.

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  Constant *Zero = ConstantInt::get(Ty, 0);
  Constant *Two = ConstantInt::get(Ty, 2);

  Value *LHS = Emit(TREE_OPERAND(exp, 0), 0);
  Value *RHS = Emit(TREE_OPERAND(exp, 1), 0);

  if (!TYPE_UNSIGNED(TREE_TYPE(exp))) {
    // In the case of signed arithmetic, we calculate RDiv as follows:
    //   LHS RDiv RHS = (sign) ( (|LHS| + (|RHS| UDiv 2)) UDiv |RHS| ),
    // where sign is +1 if LHS and RHS have the same sign, -1 if their
    // signs differ.  Doing the computation unsigned ensures that there
    // is no overflow.

    // On some machines INT_MIN Div -1 traps.  You might expect a trap for
    // INT_MIN RDiv -1 too, but this implementation will not generate one.
    // Quick quiz question: what value is returned for INT_MIN RDiv -1?

    // Determine the signs of LHS and RHS, and whether they have the same sign.
    Value *LHSIsPositive = Builder.CreateICmpSGE(LHS, Zero);
    Value *RHSIsPositive = Builder.CreateICmpSGE(RHS, Zero);
    Value *HaveSameSign = Builder.CreateICmpEQ(LHSIsPositive, RHSIsPositive);

    // Calculate |LHS| ...
    Value *MinusLHS = Builder.CreateNeg(LHS);
    Value *AbsLHS = Builder.CreateSelect(LHSIsPositive, LHS, MinusLHS,
                                         (LHS->getNameStr()+".abs").c_str());
    // ... and |RHS|
    Value *MinusRHS = Builder.CreateNeg(RHS);
    Value *AbsRHS = Builder.CreateSelect(RHSIsPositive, RHS, MinusRHS,
                                         (RHS->getNameStr()+".abs").c_str());

    // Calculate AbsRDiv = (|LHS| + (|RHS| UDiv 2)) UDiv |RHS|.
    Value *HalfAbsRHS = Builder.CreateUDiv(AbsRHS, Two);
    Value *Numerator = Builder.CreateAdd(AbsLHS, HalfAbsRHS);
    Value *AbsRDiv = Builder.CreateUDiv(Numerator, AbsRHS);

    // Return AbsRDiv or -AbsRDiv according to whether LHS and RHS have the
    // same sign or not.
    Value *MinusAbsRDiv = Builder.CreateNeg(AbsRDiv);
    return Builder.CreateSelect(HaveSameSign, AbsRDiv, MinusAbsRDiv, "rdiv");
  }

  // In the case of unsigned arithmetic, LHS and RHS necessarily have the
  // same sign, however overflow is a problem.  We want to use the formula
  //   LHS RDiv RHS = (LHS + (RHS Div 2)) Div RHS,
  // but if LHS + (RHS Div 2) overflows then we get the wrong result.  Since
  // the use of a conditional branch seems to be unavoidable, we choose the
  // simple solution of explicitly checking for overflow, and using
  //   LHS RDiv RHS = ((LHS + (RHS Div 2)) - RHS) Div RHS + 1
  // if it occurred.

  // Usually the numerator is LHS + (RHS Div 2); calculate this.
  Value *HalfRHS = Builder.CreateUDiv(RHS, Two);
  Value *Numerator = Builder.CreateAdd(LHS, HalfRHS);

  // Did the calculation overflow?
  Value *Overflowed = Builder.CreateICmpULT(Numerator, HalfRHS);

  // If so, use (LHS + (RHS Div 2)) - RHS for the numerator instead.
  Value *AltNumerator = Builder.CreateSub(Numerator, RHS);
  Numerator = Builder.CreateSelect(Overflowed, AltNumerator, Numerator);

  // Quotient = Numerator / RHS.
  Value *Quotient = Builder.CreateUDiv(Numerator, RHS);

  // Return Quotient unless we overflowed, in which case return Quotient + 1.
  return Builder.CreateAdd(Quotient, CastToUIntType(Overflowed, Ty), "rdiv");
}

//===----------------------------------------------------------------------===//
//                        ... Exception Handling ...
//===----------------------------------------------------------------------===//


/// EmitEXC_PTR_EXPR - Handle EXC_PTR_EXPR.
Value *TreeToLLVM::EmitEXC_PTR_EXPR(tree exp) {
  CreateExceptionValues();
  // Load exception address.
  Value *V = Builder.CreateLoad(ExceptionValue, "eh_value");
  // Cast the address to the right pointer type.
  return BitCastToType(V, ConvertType(TREE_TYPE(exp)));
}

/// EmitFILTER_EXPR - Handle FILTER_EXPR.
Value *TreeToLLVM::EmitFILTER_EXPR(tree exp) {
  CreateExceptionValues();
  // Load exception selector.
  return Builder.CreateLoad(ExceptionSelectorValue, "eh_select");
}

/// EmitRESX_EXPR - Handle RESX_EXPR.
Value *TreeToLLVM::EmitRESX_EXPR(tree exp) {
  unsigned RegionNo = TREE_INT_CST_LOW(TREE_OPERAND (exp, 0));
  std::vector<struct eh_region *> Handlers;

  foreach_reachable_handler(RegionNo, true, AddHandler, &Handlers);

  if (!Handlers.empty()) {
    for (std::vector<struct eh_region *>::iterator I = Handlers.begin(),
         E = Handlers.end(); I != E; ++I)
      // Create a post landing pad for the handler.
      getPostPad(get_eh_region_number(*I));

    Builder.CreateBr(getPostPad(get_eh_region_number(*Handlers.begin())));
  } else {
    assert(can_throw_external_1(RegionNo, true) &&
           "Must-not-throw region handled by runtime?");
    // Unwinding continues in the caller.
    if (!UnwindBB)
      UnwindBB = BasicBlock::Create(Context, "Unwind");
    Builder.CreateBr(UnwindBB);
  }

  EmitBlock(BasicBlock::Create(Context, ""));
  return 0;
}

//===----------------------------------------------------------------------===//
//               ... Inline Assembly and Register Variables ...
//===----------------------------------------------------------------------===//

// LLVM_GET_REG_NAME - Default to use GCC's register names.  Targets may
// override this to use different names for some registers.  The REG_NAME is
// the name before it was decoded; it may be null in some contexts.
#ifndef LLVM_GET_REG_NAME
#define LLVM_GET_REG_NAME(REG_NAME, REG_NUM) reg_names[REG_NUM]
#endif

// LLVM_CANONICAL_ADDRESS_CONSTRAINTS - GCC defines the "p" constraint to
// allow a valid memory address, but targets differ widely on what is allowed
// as an address.  This macro is a string containing the canonical constraint
// characters that are conservatively valid addresses.  Default to allowing an
// address in a register, since that works for many targets.
#ifndef LLVM_CANONICAL_ADDRESS_CONSTRAINTS
#define LLVM_CANONICAL_ADDRESS_CONSTRAINTS "r"
#endif

 /// Reads from global register variables are handled by emitting an inline
 /// asm node that copies the value out of the specified register.
Value *TreeToLLVM::EmitReadOfRegisterVariable(tree decl,
                                              const MemRef *DestLoc) {
  const Type *Ty = ConvertType(TREE_TYPE(decl));
  if (LLVM_IS_DECL_MMX_REGISTER(decl))
    Ty = Type::getX86_MMXTy(Context);

  // If there was an error, return something bogus.
  if (ValidateRegisterVariable(decl)) {
    if (Ty->isSingleValueType())
      return UndefValue::get(Ty);
    return 0;   // Just don't copy something into DestLoc.
  }

  // Turn this into a 'tmp = call Ty asm "", "={reg}"()'.
  FunctionType *FTy = FunctionType::get(Ty, std::vector<const Type*>(),false);

  const char *Name = extractRegisterName(decl);
  int RegNum = decode_reg_name(Name);
  Name = LLVM_GET_REG_NAME(Name, RegNum);

  InlineAsm *IA = InlineAsm::get(FTy, "", "={"+std::string(Name)+"}", true);
  CallInst *Call = Builder.CreateCall(IA);
  Call->setDoesNotThrow();
  return Call;
}

/// Reads from register variables are handled by emitting an inline asm node
/// that copies the value out of the specified register.
Value *TreeToLLVM::EmitMoveOfRegVariableToRightReg(Instruction *I, tree var) {
  // Create a 'call void asm sideeffect "", "{reg}"(Ty %RHS)'.
  const Type *Ty = I->getType();

  // If there was an error, return something bogus.
  if (ValidateRegisterVariable(var)) {
    if (Ty->isSingleValueType())
      return UndefValue::get(Ty);
    return 0;   // Just don't copy something into DestLoc.
  }

  std::vector<const Type*> ArgTys;
  ArgTys.push_back(Ty);
  FunctionType *FTy = FunctionType::get(Type::getVoidTy(Context), 
                                        ArgTys, false);
  const char *Name = extractRegisterName(var);
  int RegNum = decode_reg_name(Name);
  Name = LLVM_GET_REG_NAME(Name, RegNum);
  InlineAsm *IA = InlineAsm::get(FTy, "", "{"+std::string(Name)+"}", 
                                    true);
  CallInst *Call = Builder.CreateCall(IA, I);
  Call->setDoesNotThrow();
  // Create another asm with the same reg, this time producing an output.
  // Turn this into a 'tmp = call Ty asm "", "={reg}"()'.
  FunctionType *FTy2 = FunctionType::get(Ty, std::vector<const Type*>(),
                                        false);
  InlineAsm *IA2 = InlineAsm::get(FTy2, "", "={"+std::string(Name)+"}",
                                 true);
  CallInst *Call2 = Builder.CreateCall(IA2);
  Call2->setDoesNotThrow();
  return Call2;
}

/// Stores to global register variables are handled by emitting an inline asm
/// node that copies the value into the specified register.
void TreeToLLVM::EmitModifyOfRegisterVariable(tree decl, Value *RHS) {
  // If there was an error, bail out.
  if (ValidateRegisterVariable(decl))
    return;

  // Turn this into a 'call void asm sideeffect "", "{reg}"(Ty %RHS)'.
  std::vector<const Type*> ArgTys;
  const Type* Ty = ConvertType(TREE_TYPE(decl));
  if (LLVM_IS_DECL_MMX_REGISTER(decl))
    Ty = Type::getX86_MMXTy(Context);
  ArgTys.push_back(Ty);
  FunctionType *FTy = FunctionType::get(Type::getVoidTy(Context), ArgTys, false);

  const char *Name = extractRegisterName(decl);
  int RegNum = decode_reg_name(Name);
  Name = LLVM_GET_REG_NAME(Name, RegNum);

  InlineAsm *IA = InlineAsm::get(FTy, "", "{"+std::string(Name)+"}", true);
  CallInst *Call = Builder.CreateCall(IA, RHS);
  Call->setDoesNotThrow();
}

/// ConvertInlineAsmStr - Convert the specified inline asm string to an LLVM
/// InlineAsm string.  The GNU style inline asm template string has the
/// following format:
///   %N (for N a digit) means print operand N in usual manner.
///   %=  means a unique number for the inline asm.
///   %lN means require operand N to be a CODE_LABEL or LABEL_REF
///       and print the label name with no punctuation.
///   %cN means require operand N to be a constant
///       and print the constant expression with no punctuation.
///   %aN means expect operand N to be a memory address
///       (not a memory reference!) and print a reference to that address.
///   %nN means expect operand N to be a constant and print a constant
///       expression for minus the value of the operand, with no other
///       punctuation.
/// Other %xN expressions are turned into LLVM ${N:x} operands.
///
static std::string ConvertInlineAsmStr(tree exp, unsigned NumOperands) {

  tree str = ASM_STRING(exp);
  if (TREE_CODE(str) == ADDR_EXPR) str = TREE_OPERAND(str, 0);

  // ASM_INPUT_P - This flag is set if this is a non-extended ASM, which means
  // that the asm string should not be interpreted, other than to escape $'s.
  if (ASM_INPUT_P(exp)) {
    const char *InStr = TREE_STRING_POINTER(str);
    std::string Result;
    while (1) {
      switch (*InStr++) {
      case 0: return Result;                 // End of string.
      default: Result += InStr[-1]; break;   // Normal character.
      case '$': Result += "$$"; break;       // Escape '$' characters.
      }
    }
  }

  // Expand [name] symbolic operand names.
  str = resolve_asm_operand_names(str, ASM_OUTPUTS(exp), ASM_INPUTS(exp));

  const char *InStr = TREE_STRING_POINTER(str);

  std::string Result;
  while (1) {
    switch (*InStr++) {
    case 0: return Result;                 // End of string.
    default: Result += InStr[-1]; break;   // Normal character.
    case '$': Result += "$$"; break;       // Escape '$' characters.
#ifdef ASSEMBLER_DIALECT
    // Note that we can't escape to ${, because that is the syntax for vars.
    case '{': Result += "$("; break;       // Escape '{' character.
    case '}': Result += "$)"; break;       // Escape '}' character.
    case '|': Result += "$|"; break;       // Escape '|' character.
#endif
    case '%':                              // GCC escape character.
      char EscapedChar = *InStr++;
      if (EscapedChar == '%') {            // Escaped '%' character
        Result += '%';
      } else if (EscapedChar == '=') {     // Unique ID for the asm instance.
        Result += "${:uid}";
      }
#ifdef LLVM_ASM_EXTENSIONS
      LLVM_ASM_EXTENSIONS(EscapedChar, InStr, Result)
#endif
      else if (ISALPHA(EscapedChar)) {
        // % followed by a letter and some digits. This outputs an operand in a
        // special way depending on the letter.  We turn this into LLVM ${N:o}
        // syntax.
        char *EndPtr;
        unsigned long OpNum = strtoul(InStr, &EndPtr, 10);

        if (InStr == EndPtr) {
          error("%Hoperand number missing after %%-letter",&EXPR_LOCATION(exp));
          return Result;
        } else if (OpNum >= NumOperands) {
          error("%Hoperand number out of range", &EXPR_LOCATION(exp));
          return Result;
        }
        Result += "${" + utostr(OpNum) + ":" + EscapedChar + "}";
        InStr = EndPtr;
      } else if (ISDIGIT(EscapedChar)) {
        char *EndPtr;
        unsigned long OpNum = strtoul(InStr-1, &EndPtr, 10);
        InStr = EndPtr;
        Result += "$" + utostr(OpNum);
#ifdef PRINT_OPERAND_PUNCT_VALID_P
      } else if (PRINT_OPERAND_PUNCT_VALID_P((unsigned char)EscapedChar)) {
        Result += "${:";
        Result += EscapedChar;
        Result += "}";
#endif
      } else {
        output_operand_lossage("invalid %%-code");
      }
      break;
    }
  }
}

/// CanonicalizeConstraint - If we can canonicalize the constraint into
/// something simpler, do so now.  This turns register classes with a single
/// register into the register itself, expands builtin constraints to multiple
/// alternatives, etc.
static std::string CanonicalizeConstraint(const char *Constraint) {
  std::string Result;

  // Skip over modifier characters.
  bool DoneModifiers = false;
  while (!DoneModifiers) {
    switch (*Constraint) {
    default: DoneModifiers = true; break;
    case '=': assert(0 && "Should be after '='s");
    case '+': assert(0 && "'+' should already be expanded");
    case '*':
    case '?':
    case '!':
      ++Constraint;
      break;
    case '&':     // Pass earlyclobber to LLVM.
    case '%':     // Pass commutative to LLVM.
      Result += *Constraint++;
      break;
    case '#':  // No constraint letters left.
      return Result;
    }
  }

  while (*Constraint) {
    char ConstraintChar = *Constraint++;

    // 'g' is just short-hand for 'imr'.
    if (ConstraintChar == 'g') {
      Result += "imr";
      continue;
    }

    // Translate 'p' to a target-specific set of constraints that
    // conservatively allow a valid memory address.  For inline assembly there
    // is no way to know the mode of the data being addressed, so this is only
    // a rough approximation of how GCC handles this constraint.
    if (ConstraintChar == 'p') {
      Result += LLVM_CANONICAL_ADDRESS_CONSTRAINTS;
      continue;
    }

    // See if this is a regclass constraint.
    unsigned RegClass;
    if (ConstraintChar == 'r')
      // REG_CLASS_FROM_CONSTRAINT doesn't support 'r' for some reason.
      RegClass = GENERAL_REGS;
    else
      RegClass = REG_CLASS_FROM_CONSTRAINT(Constraint[-1], Constraint-1);

    if (RegClass == NO_REGS) {  // not a reg class.
      Result += ConstraintChar;
      continue;
    }

    // Look to see if the specified regclass has exactly one member, and if so,
    // what it is.  Cache this information in AnalyzedRegClasses once computed.
    static std::map<unsigned, int> AnalyzedRegClasses;

    std::map<unsigned, int>::iterator I =
      AnalyzedRegClasses.lower_bound(RegClass);

    int RegMember;
    if (I != AnalyzedRegClasses.end() && I->first == RegClass) {
      // We've already computed this, reuse value.
      RegMember = I->second;
    } else {
      // Otherwise, scan the regclass, looking for exactly one member.
      RegMember = -1;  // -1 => not a single-register class.
      for (unsigned j = 0; j != FIRST_PSEUDO_REGISTER; ++j)
        if (TEST_HARD_REG_BIT(reg_class_contents[RegClass], j)) {
          if (RegMember == -1) {
            RegMember = j;
          } else {
            RegMember = -1;
            break;
          }
        }
      // Remember this answer for the next query of this regclass.
      AnalyzedRegClasses.insert(I, std::make_pair(RegClass, RegMember));
    }

    // If we found a single register register class, return the register.
    if (RegMember != -1) {
      Result += '{';
      Result += LLVM_GET_REG_NAME(0, RegMember);
      Result += '}';
    } else {
      Result += ConstraintChar;
    }
  }

  return Result;
}

/// See if operand "exp" can use the indicated Constraint (which is
/// terminated by a null or a comma).
/// Returns:  -1=no, 0=yes but auxiliary instructions needed, 1=yes and free
static int MatchWeight(const char *Constraint, tree Operand) {
  const char *p = Constraint;
  int RetVal = 0;
  // Look for hard register operand.  This matches only a constraint of a
  // register class that includes that hard register, and it matches that
  // perfectly, so we never return 0 in this case.
  if (TREE_CODE(Operand) == VAR_DECL && DECL_HARD_REGISTER(Operand)) {
    int RegNum = decode_reg_name(extractRegisterName(Operand));
    RetVal = -1;
    if (RegNum >= 0) {
      do {
        unsigned RegClass;
        if (*p == 'r')
          RegClass = GENERAL_REGS;
        else
          RegClass = REG_CLASS_FROM_CONSTRAINT(*p, p);
        if (RegClass != NO_REGS &&
            TEST_HARD_REG_BIT(reg_class_contents[RegClass], RegNum)) {
          RetVal = 1;
          break;
        }
        ++p;
      } while (*p != ',' && *p != 0);
    }
  }
  // Look for integer constant operand.  This cannot match "m", and "i" is
  // better than "r".  FIXME target-dependent immediate letters are not handled
  // yet; in general they require looking at the value.
  if (TREE_CODE(Operand) == INTEGER_CST) {
    do {
      RetVal = -1;
      if (*p == 'i' || *p == 'n') {     // integer constant
        RetVal = 1;
        break;
      }
      if (*p != 'm' && *p != 'o' && *p != 'V')    // not memory
        RetVal = 0;
      ++p;
    } while (*p != ',' && *p != 0);
  }
  /// TEMPORARY.  This has the effect that alternative 0 is always chosen,
  /// except in the cases handled above.
  return RetVal;
}

/// ChooseConstraintTuple: we know each of the NumInputs+NumOutputs strings
/// in Constraints[] is a comma-separated list of NumChoices different
/// constraints.  Look through the operands and constraint possibilities
/// and pick a tuple where all the operands match.  Replace the strings
/// in Constraints[] with the shorter strings from that tuple (malloc'ed,
/// caller is responsible for cleaning it up).  Later processing can alter what
/// Constraints points to, so to make sure we delete everything, the addresses
/// of everything we allocated also are returned in ReplacementStrings.
/// Casting back and forth from char* to const char* is Ugly, but we have to
/// interface with C code that expects const char*.
///
/// gcc's algorithm for picking "the best" tuple is quite complicated, and
/// is performed after things like SROA, not before.  At the moment we are
/// just trying to pick one that will work.  This may get refined.
static void
ChooseConstraintTuple (const char **Constraints, tree exp, unsigned NumInputs,
                      unsigned NumOutputs, unsigned NumChoices,
                      const char **ReplacementStrings)
{
  int MaxWeight = -1;
  unsigned int CommasToSkip = 0;
  int *Weights = (int *)alloca(NumChoices * sizeof(int));
  // RunningConstraints is pointers into the Constraints strings which
  // are incremented as we go to point to the beginning of each
  // comma-separated alternative.
  const char** RunningConstraints =
    (const char**)alloca((NumInputs+NumOutputs)*sizeof(const char*));
  memcpy(RunningConstraints, Constraints,
         (NumInputs+NumOutputs) * sizeof(const char*));
  // The entire point of this loop is to compute CommasToSkip.
  for (unsigned int i=0; i<NumChoices; i++) {
    Weights[i] = 0;
    unsigned int j = 0;
    for (tree Output = ASM_OUTPUTS(exp); j<NumOutputs;
         j++, Output = TREE_CHAIN(Output)) {
      if (i==0)
        RunningConstraints[j]++;    // skip leading =
      const char* p = RunningConstraints[j];
      while (*p=='*' || *p=='&' || *p=='%')   // skip modifiers
        p++;
      if (Weights[i] != -1) {
        int w = MatchWeight(p, TREE_VALUE(Output));
        // Nonmatch means the entire tuple doesn't match.  However, we
        // keep scanning to set up RunningConstraints correctly for the
        // next tuple.
        if (w < 0)
          Weights[i] = -1;
        else
          Weights[i] += w;
      }
      while (*p!=0 && *p!=',')
        p++;
      if (*p!=0) {
        p++;      // skip comma
        while (*p=='*' || *p=='&' || *p=='%')
          p++;    // skip modifiers
      }
      RunningConstraints[j] = p;
    }
    assert(j==NumOutputs);
    for (tree Input = ASM_INPUTS(exp); j<NumInputs+NumOutputs;
         j++, Input = TREE_CHAIN(Input)) {
      const char* p = RunningConstraints[j];
      if (Weights[i] != -1) {
        int w = MatchWeight(p, TREE_VALUE(Input));
        if (w < 0)
          Weights[i] = -1;    // As above.
        else
          Weights[i] += w;
      }
      while (*p!=0 && *p!=',')
        p++;
      if (*p!=0)
        p++;
      RunningConstraints[j] = p;
    }
    if (Weights[i]>MaxWeight) {
      CommasToSkip = i;
      MaxWeight = Weights[i];
    }
  }
  // We have picked an alternative (the CommasToSkip'th one).
  // Change Constraints to point to malloc'd copies of the appropriate
  // constraints picked out of the original strings.
  for (unsigned int i=0; i<NumInputs+NumOutputs; i++) {
    assert(*(RunningConstraints[i])==0);   // sanity check
    const char* start = Constraints[i];
    if (i<NumOutputs)
      start++;          // skip '=' or '+'
    const char* end = start;
    while (*end != ',' && *end != 0)
      end++;
    for (unsigned int j=0; j<CommasToSkip; j++) {
      start = end+1;
      end = start;
      while (*end != ',' && *end != 0)
        end++;
    }
    // String we want is at start..end-1 inclusive.
    // For outputs, copy the leading = or +.
    char *newstring;
    if (i<NumOutputs) {
      newstring = (char *)xmalloc(end-start+1+1);
      newstring[0] = *(Constraints[i]);
      strncpy(newstring+1, start, end-start);
      newstring[end-start+1] = 0;
    } else {
      newstring = (char *)xmalloc(end-start+1);
      strncpy(newstring, start, end-start);
      newstring[end-start] = 0;
    }
    Constraints[i] = (const char *)newstring;
    ReplacementStrings[i] = (const char*)newstring;
  }
}

static void FreeConstTupleStrings(const char **ReplacementStrings,
                                  unsigned int Size) {
  for (unsigned int i=0; i<Size; i++)
    free((char *)ReplacementStrings[i]);
}

Value *TreeToLLVM::EmitASM_EXPR(tree exp) {
  unsigned NumInputs = list_length(ASM_INPUTS(exp));
  unsigned NumOutputs = list_length(ASM_OUTPUTS(exp));
  unsigned NumInOut = 0;

  // Look for multiple alternative constraints: multiple alternatives separated
  // by commas.
  unsigned NumChoices = 0;    // sentinal; real value is always at least 1.
  const char* p;
  for (tree t = ASM_INPUTS(exp); t; t = TREE_CHAIN(t)) {
    unsigned NumInputChoices = 1;
    for (p = TREE_STRING_POINTER(TREE_VALUE(TREE_PURPOSE(t))); *p; p++) {
      if (*p == ',')
        NumInputChoices++;
    }
    if (NumChoices==0)
      NumChoices = NumInputChoices;
    else if (NumChoices != NumInputChoices)
      abort();      // invalid constraints
  }
  for (tree t = ASM_OUTPUTS(exp); t; t = TREE_CHAIN(t)) {
    unsigned NumOutputChoices = 1;
    for (p = TREE_STRING_POINTER(TREE_VALUE(TREE_PURPOSE(t))); *p; p++) {
      if (*p == ',')
        NumOutputChoices++;
    }
    if (NumChoices==0)
      NumChoices = NumOutputChoices;
    else if (NumChoices != NumOutputChoices)
      abort();      // invalid constraints
  }

  /// Constraints - The output/input constraints, concatenated together in array
  /// form instead of list form.
  const char **Constraints =
    (const char **)alloca((NumOutputs + NumInputs) * sizeof(const char *));

  // Process outputs.
  int ValNum = 0;
  for (tree Output = ASM_OUTPUTS(exp); Output;
       Output = TREE_CHAIN(Output), ++ValNum) {
    tree Operand = TREE_VALUE(Output);
    tree type = TREE_TYPE(Operand);
    // If there's an erroneous arg, emit no insn.
    if (type == error_mark_node) return 0;

    // Parse the output constraint.
    const char *Constraint =
      TREE_STRING_POINTER(TREE_VALUE(TREE_PURPOSE(Output)));
    Constraints[ValNum] = Constraint;
  }
  // Process inputs.
  for (tree Input = ASM_INPUTS(exp); Input; Input = TREE_CHAIN(Input),++ValNum){
    tree Val = TREE_VALUE(Input);
    tree type = TREE_TYPE(Val);
    // If there's an erroneous arg, emit no insn.
    if (type == error_mark_node) return 0;

    const char *Constraint =
      TREE_STRING_POINTER(TREE_VALUE(TREE_PURPOSE(Input)));
    Constraints[ValNum] = Constraint;
  }

  // If there are multiple constraint tuples, pick one.  Constraints is
  // altered to point to shorter strings (which are malloc'ed), and everything
  // below Just Works as in the NumChoices==1 case.
  const char** ReplacementStrings = 0;
  if (NumChoices>1) {
    ReplacementStrings =
      (const char **)alloca((NumOutputs + NumInputs) * sizeof(const char *));
    ChooseConstraintTuple(Constraints, exp, NumInputs, NumOutputs, NumChoices,
                          ReplacementStrings);
  }

  std::vector<Value*> CallOps;
  std::vector<const Type*> CallArgTypes;
  std::string NewAsmStr = ConvertInlineAsmStr(exp, NumOutputs+NumInputs);
  std::string ConstraintStr;
  bool HasSideEffects = ASM_VOLATILE_P(exp) || !ASM_OUTPUTS(exp);

  // StoreCallResultAddr - The pointer to store the result of the call through.
  SmallVector<Value *, 4> StoreCallResultAddrs;
  SmallVector<const Type *, 4> CallResultTypes;
  SmallVector<bool, 4> CallResultIsSigned;
  SmallVector<std::pair<bool, unsigned>, 4> OutputLocations;

  // Process outputs.
  ValNum = 0;
  for (tree Output = ASM_OUTPUTS(exp); Output;
       Output = TREE_CHAIN(Output), ++ValNum) {
    tree Operand = TREE_VALUE(Output);

    // Parse the output constraint.
    const char *Constraint = Constraints[ValNum];
    bool IsInOut, AllowsReg, AllowsMem;
    if (!parse_output_constraint(&Constraint, ValNum, NumInputs, NumOutputs,
                                 &AllowsMem, &AllowsReg, &IsInOut)) {
      if (NumChoices>1)
        FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
      return 0;
    }
    assert(Constraint[0] == '=' && "Not an output constraint?");

    // Output constraints must be addressable if they aren't simple register
    // constraints (this emits "address of register var" errors, etc).
    if (!AllowsReg && (AllowsMem || IsInOut))
      lang_hooks.mark_addressable(Operand);

    // Count the number of "+" constraints.
    if (IsInOut)
      ++NumInOut, ++NumInputs;

    std::string SimplifiedConstraint;
    // If this output register is pinned to a machine register, use that machine
    // register instead of the specified constraint.
    if (TREE_CODE(Operand) == VAR_DECL && DECL_HARD_REGISTER(Operand)) {
      const char* RegName = extractRegisterName(Operand);
      int RegNum = decode_reg_name(RegName);
      if (RegNum >= 0) {
        RegName = LLVM_GET_REG_NAME(RegName, RegNum);
        unsigned RegNameLen = strlen(RegName);
        char *NewConstraint = (char*)alloca(RegNameLen+3);
        NewConstraint[0] = '{';
        memcpy(NewConstraint+1, RegName, RegNameLen);
        NewConstraint[RegNameLen+1] = '}';
        NewConstraint[RegNameLen+2] = 0;
        SimplifiedConstraint = NewConstraint;
        // This output will now be implicit; set the sideffect flag on the asm.
        HasSideEffects = true;
        // We should no longer consider mem constraints.
        AllowsMem = false;
      } else {
        // If we can simplify the constraint into something else, do so now.
        // This avoids LLVM having to know about all the (redundant) GCC
        // constraints.
        SimplifiedConstraint = CanonicalizeConstraint(Constraint+1);
      }
    } else {
      SimplifiedConstraint = CanonicalizeConstraint(Constraint+1);
    }

    LValue Dest = EmitLV(Operand);
    const Type *DestValTy =
      cast<PointerType>(Dest.Ptr->getType())->getElementType();

    assert(!Dest.isBitfield() && "Cannot assign into a bitfield!");
    if (!AllowsMem && DestValTy->isSingleValueType()) {// Reg dest -> asm return
      StoreCallResultAddrs.push_back(Dest.Ptr);
      ConstraintStr += ",=";
      ConstraintStr += SimplifiedConstraint;
      CallResultTypes.push_back(LLVM_ADJUST_MMX_INLINE_PARAMETER_TYPE(
                                SimplifiedConstraint, DestValTy));
      CallResultIsSigned.push_back(!TYPE_UNSIGNED(TREE_TYPE(Operand)));
      OutputLocations.push_back(std::make_pair(true, CallResultTypes.size()-1));
    } else {
      ConstraintStr += ",=*";
      ConstraintStr += SimplifiedConstraint;
      CallOps.push_back(Dest.Ptr);
      CallArgTypes.push_back(LLVM_ADJUST_MMX_INLINE_PARAMETER_TYPE(
                                SimplifiedConstraint, Dest.Ptr->getType()));
      OutputLocations.push_back(std::make_pair(false, CallArgTypes.size()-1));
    }
  }

  // Process inputs.
  for (tree Input = ASM_INPUTS(exp); Input; Input = TREE_CHAIN(Input),++ValNum){
    tree Val = TREE_VALUE(Input);
    tree type = TREE_TYPE(Val);

    const char *Constraint = Constraints[ValNum];

    bool AllowsReg, AllowsMem;
    if (!parse_input_constraint(Constraints+ValNum, ValNum-NumOutputs,
                                NumInputs, NumOutputs, NumInOut,
                                Constraints, &AllowsMem, &AllowsReg)) {
      if (NumChoices>1)
        FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
      return 0;
    }
    bool isIndirect = false;
    if (AllowsReg || !AllowsMem) {    // Register operand.
      const Type *LLVMTy = ConvertType(type);

      Value *Op = 0;
      if (LLVM_IS_DECL_MMX_REGISTER(Val))
        LLVMTy = Type::getX86_MMXTy(Context);
      const Type *OpTy = LLVMTy;
      if (LLVMTy->isSingleValueType()) {
        if (TREE_CODE(Val)==ADDR_EXPR &&
            TREE_CODE(TREE_OPERAND(Val,0))==LABEL_DECL) {
          // Emit the label, but do not assume it is going to be the target
          // of an indirect branch.  Having this logic here is a hack; there
          // should be a bit in the label identifying it as in an asm.
          Op = getLabelDeclBlock(TREE_OPERAND(Val, 0));
        } else
          Op = Emit(Val, 0);
      } else {
        LValue LV = EmitLV(Val);
        assert(!LV.isBitfield() && "Inline asm can't have bitfield operand");

        // Small structs and unions can be treated as integers.
        uint64_t TySize = TD.getTypeSizeInBits(LLVMTy);
        if (TySize == 1 || TySize == 8 || TySize == 16 ||
            TySize == 32 || TySize == 64 || (TySize == 128 && !AllowsMem)) {
          LLVMTy = IntegerType::get(Context, TySize);
          Op = Builder.CreateLoad(BitCastToType(LV.Ptr,
                                                LLVMTy->getPointerTo()));
        } else {
          // Codegen only supports indirect operands with mem constraints.
          if (!AllowsMem)
            error("%Haggregate does not match inline asm register constraint",
                  &EXPR_LOCATION(exp));
          // Otherwise, emit our value as a lvalue.
          isIndirect = true;
          Op = LV.Ptr;
          OpTy = Op->getType();
        }
      }

      // If this input operand is matching an output operand, e.g. '0', check if
      // this is something that llvm supports. If the operand types are
      // different, then emit an error if 1) one of the types is not integer or
      // pointer, 2) if size of input type is larger than the output type. If
      // the size of the integer input size is smaller than the integer output
      // type, then cast it to the larger type and shift the value if the target
      // is big endian.
      if (ISDIGIT(Constraint[0])) {
        unsigned Match = atoi(Constraint);
        // This output might have gotten put in either CallResult or CallArg
        // depending whether it's a register or not.  Find its type.
        const Type *OTy = 0;
        if (Match < OutputLocations.size()) {
          // Indices here known to be within range.
          if (OutputLocations[Match].first)
            OTy = CallResultTypes[OutputLocations[Match].second];
          else {
            OTy = CallArgTypes[OutputLocations[Match].second];
            assert(OTy->isPointerTy() && "Expected pointer type!");
            OTy = cast<PointerType>(OTy)->getElementType();
          }
        }
        if (OTy && OTy != OpTy) {
          if (!(OTy->isIntegerTy() || OTy->isPointerTy()) ||
              !(OpTy->isIntegerTy() || OpTy->isPointerTy())) {
            error("%Hunsupported inline asm: input constraint with a matching "
                  "output constraint of incompatible type!",
                  &EXPR_LOCATION(exp));
            if (NumChoices>1)
              FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
            return 0;
          }
          unsigned OTyBits = TD.getTypeSizeInBits(OTy);
          unsigned OpTyBits = TD.getTypeSizeInBits(OpTy);
          if (OTyBits == 0 || OpTyBits == 0 || OTyBits < OpTyBits) {
            // It's tempting to implement the OTyBits < OpTyBits case by 
            // truncating Op down to OTy, however that breaks in the case of an 
            // inline asm constraint that corresponds to a single register, 
            // because the user can write code that assumes the whole register 
            // is defined, despite the output operand being only a subset of the
            // register. For example:
            //
            //   asm ("sarl $10, %%eax" : "=a"(c) : "0"(1000000));
            //
            // The expected behavior is for %eax to be fully defined with the 
            // value 1000000 immediately before the asm.
            error("%Hunsupported inline asm: input constraint with a matching "
                  "output constraint of incompatible type!",
                  &EXPR_LOCATION(exp));
            return 0;
          } else if (OTyBits > OpTyBits) {
            Op = CastToAnyType(Op, !TYPE_UNSIGNED(type),
                               OTy, CallResultIsSigned[Match]);
            if (BYTES_BIG_ENDIAN) {
              Constant *ShAmt = ConstantInt::get(Op->getType(),
                                                 OTyBits-OpTyBits);
              Op = Builder.CreateLShr(Op, ShAmt);
            }
          }
        }
      }

      const Type* AdjTy = LLVM_ADJUST_MMX_INLINE_PARAMETER_TYPE(
                              Constraint, Op->getType());
      if (AdjTy != Op->getType())
        Op = BitCastToType(Op, AdjTy);
      CallOps.push_back(Op);
      CallArgTypes.push_back(AdjTy);
    } else {                          // Memory operand.
      lang_hooks.mark_addressable(TREE_VALUE(Input));
      isIndirect = true;
      LValue Src = EmitLV(Val);
      assert(!Src.isBitfield() && "Cannot read from a bitfield!");
      CallOps.push_back(Src.Ptr);
      CallArgTypes.push_back(Src.Ptr->getType());
    }

    ConstraintStr += ',';
    if (isIndirect)
      ConstraintStr += '*';

    // If this input register is pinned to a machine register, use that machine
    // register instead of the specified constraint.
    if (TREE_CODE(Val) == VAR_DECL && DECL_HARD_REGISTER(Val)) {
      const char *RegName = extractRegisterName(Val);
      int RegNum = decode_reg_name(RegName);
      if (RegNum >= 0) {
        RegName = LLVM_GET_REG_NAME(RegName, RegNum);
        ConstraintStr += '{';
        ConstraintStr += RegName;
        ConstraintStr += '}';
        continue;
      }
    }

    // If there is a simpler form for the register constraint, use it.
    std::string Simplified = CanonicalizeConstraint(Constraint);
    ConstraintStr += Simplified;
  }

  // ASM_USES contains info about certain hard regs which are used as inputs.
  // gcc represents the xH registers on x86 this way because of deficiencies
  // in the way gcc can represent registers internally.  llvm-gcc can represent
  // these as normal inputs, so we aren't using ASM_USES.
  assert(ASM_USES(exp)==0);

  // Process clobbers.

  // Some targets automatically clobber registers across an asm.
  tree Clobbers = targetm.md_asm_clobbers(ASM_OUTPUTS(exp), ASM_INPUTS(exp),
                                          ASM_CLOBBERS(exp));
  for (; Clobbers; Clobbers = TREE_CHAIN(Clobbers)) {
    const char *RegName = TREE_STRING_POINTER(TREE_VALUE(Clobbers));
    int RegCode = decode_reg_name(RegName);

    switch (RegCode) {
    case -1:     // Nothing specified?
    case -2:     // Invalid.
      error("%Hunknown register name %qs in %<asm%>", &EXPR_LOCATION(exp),
            RegName);
      if (NumChoices>1)
        FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
      return 0;
    case -3:     // cc
      ConstraintStr += ",~{cc}";
      break;
    case -4:     // memory
      ConstraintStr += ",~{memory}";
      break;
    default:     // Normal register name.
      RegName = LLVM_GET_REG_NAME(RegName, RegCode);
      ConstraintStr += ",~{";
      ConstraintStr += RegName;
      ConstraintStr += "}";
      break;
    }
  }

  const Type *CallResultType;
  switch (CallResultTypes.size()) {
  case 0: CallResultType = Type::getVoidTy(Context); break;
  case 1: CallResultType = CallResultTypes[0]; break;
  default:
    std::vector<const Type*> TmpVec(CallResultTypes.begin(),
                                    CallResultTypes.end());
    CallResultType = StructType::get(Context, TmpVec);
    break;
  }

  const FunctionType *FTy =
    FunctionType::get(CallResultType, CallArgTypes, false);

  // Remove the leading comma if we have operands.
  if (!ConstraintStr.empty())
    ConstraintStr.erase(ConstraintStr.begin());

  // Make sure we're created a valid inline asm expression.
  if (!InlineAsm::Verify(FTy, ConstraintStr)) {
    error("%HInvalid or unsupported inline assembly!", &EXPR_LOCATION(exp));
    if (NumChoices>1)
      FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
    return 0;
  }

  Value *Asm = InlineAsm::get(FTy, NewAsmStr, ConstraintStr,
                              HasSideEffects, ASM_ASM_BLOCK(exp));
  CallInst *CV = Builder.CreateCall(Asm, CallOps.begin(), CallOps.end(),
                                    CallResultTypes.empty() ? "" : "asmtmp");
  CV->setDoesNotThrow();

  // If the call produces a value, store it into the destination.
  if (StoreCallResultAddrs.size() == 1) {
    Value *V = CV;
    const Type *DestValTy =
      cast<PointerType>(StoreCallResultAddrs[0]->getType())->getElementType();
    if (CV->getType() != DestValTy)
      V = BitCastToType(CV, DestValTy);
    Builder.CreateStore(V, StoreCallResultAddrs[0]);
  } else if (unsigned NumResults = StoreCallResultAddrs.size()) {
    for (unsigned i = 0; i != NumResults; ++i) {
      const Type *DestValTy =
        cast<PointerType>(StoreCallResultAddrs[i]->getType())->getElementType();
      Value *ValI = Builder.CreateExtractValue(CV, i, "asmresult");
      if (ValI->getType() != DestValTy)
        ValI = BitCastToType(ValI, DestValTy);
      Builder.CreateStore(ValI, StoreCallResultAddrs[i]);
    }
  }

  // Give the backend a chance to upgrade the inline asm to LLVM code.  This
  // handles some common cases that LLVM has intrinsics for, e.g. x86 bswap ->
  // llvm.bswap.
  if (const TargetLowering *TLI = TheTarget->getTargetLowering())
    TLI->ExpandInlineAsm(CV);

  if (NumChoices>1)
    FreeConstTupleStrings(ReplacementStrings, NumInputs+NumOutputs);
  return 0;
}

//===----------------------------------------------------------------------===//
//               ... Helpers for Builtin Function Expansion ...
//===----------------------------------------------------------------------===//

Value *TreeToLLVM::BuildVector(const std::vector<Value*> &Ops) {
  assert((Ops.size() & (Ops.size()-1)) == 0 &&
         "Not a power-of-two sized vector!");
  bool AllConstants = true;
  for (unsigned i = 0, e = Ops.size(); i != e && AllConstants; ++i)
    AllConstants &= isa<Constant>(Ops[i]);

  // If this is a constant vector, create a ConstantVector.
  if (AllConstants) {
    std::vector<Constant*> CstOps;
    for (unsigned i = 0, e = Ops.size(); i != e; ++i)
      CstOps.push_back(cast<Constant>(Ops[i]));
    return ConstantVector::get(CstOps);
  }

  // Otherwise, insertelement the values to build the vector.
  Value *Result =
    UndefValue::get(VectorType::get(Ops[0]->getType(), Ops.size()));

  for (unsigned i = 0, e = Ops.size(); i != e; ++i)
    Result = Builder.CreateInsertElement(Result, Ops[i],
                                ConstantInt::get(Type::getInt32Ty(Context), i));

  return Result;
}

/// BuildVector - This varargs function builds a literal vector ({} syntax) with
/// the specified null-terminated list of elements.  The elements must be all
/// the same element type and there must be a power of two of them.
Value *TreeToLLVM::BuildVector(Value *Elt, ...) {
  std::vector<Value*> Ops;
  va_list VA;
  va_start(VA, Elt);

  Ops.push_back(Elt);
  while (Value *Arg = va_arg(VA, Value *))
    Ops.push_back(Arg);
  va_end(VA);

  return BuildVector(Ops);
}

/// BuildVectorShuffle - Given two vectors and a variable length list of int
/// constants, create a shuffle of the elements of the inputs, where each dest
/// is specified by the indexes.  The int constant list must be as long as the
/// number of elements in the input vector.
///
/// Undef values may be specified by passing in -1 as the result value.
///
Value *TreeToLLVM::BuildVectorShuffle(Value *InVec1, Value *InVec2, ...) {
  assert(InVec1->getType()->isVectorTy() &&
         InVec1->getType() == InVec2->getType() && "Invalid shuffle!");
  unsigned NumElements = cast<VectorType>(InVec1->getType())->getNumElements();

  // Get all the indexes from varargs.
  std::vector<Constant*> Idxs;
  va_list VA;
  va_start(VA, InVec2);
  for (unsigned i = 0; i != NumElements; ++i) {
    int idx = va_arg(VA, int);
    if (idx == -1)
      Idxs.push_back(UndefValue::get(Type::getInt32Ty(Context)));
    else {
      assert((unsigned)idx < 2*NumElements && "Element index out of range!");
      Idxs.push_back(ConstantInt::get(Type::getInt32Ty(Context), idx));
    }
  }
  va_end(VA);

  // Turn this into the appropriate shuffle operation.
  return Builder.CreateShuffleVector(InVec1, InVec2,
                                     ConstantVector::get(Idxs));
}

//===----------------------------------------------------------------------===//
//                     ... Builtin Function Expansion ...
//===----------------------------------------------------------------------===//

/// EmitFrontendExpandedBuiltinCall - We allow the target to do some amount
/// of lowering.  This allows us to avoid having intrinsics for operations that
/// directly correspond to LLVM constructs.
///
/// This method returns true if the builtin is handled, otherwise false.
///
bool TreeToLLVM::EmitFrontendExpandedBuiltinCall(tree exp, tree fndecl,
                                                 const MemRef *DestLoc,
                                                 Value *&Result) {
#ifdef LLVM_TARGET_INTRINSIC_LOWER
  // Get the result type and operand line in an easy to consume format.
  const Type *ResultType = ConvertType(TREE_TYPE(TREE_TYPE(fndecl)));
  std::vector<Value*> Operands;
  for (tree Op = TREE_OPERAND(exp, 1); Op; Op = TREE_CHAIN(Op)) {
    tree OpVal = TREE_VALUE(Op);
    if (isAggregateTreeType(TREE_TYPE(OpVal))) {
      MemRef OpLoc = CreateTempLoc(ConvertType(TREE_TYPE(OpVal)));
      Emit(OpVal, &OpLoc);
      Operands.push_back(Builder.CreateLoad(OpLoc.Ptr));
    } else {
      Operands.push_back(Emit(OpVal, NULL));
    }
  }

  unsigned FnCode = DECL_FUNCTION_CODE(fndecl);
  return LLVM_TARGET_INTRINSIC_LOWER(exp, FnCode, DestLoc, Result, ResultType,
                                     Operands);
#endif
  return false;
}

/// TargetBuiltinCache - A cache of builtin intrinsics indexed by the GCC
/// builtin number.
static std::vector<Constant*> TargetBuiltinCache;

void clearTargetBuiltinCache() {
  TargetBuiltinCache.clear();
}

void TreeToLLVM::EmitMemoryBarrier(bool ll, bool ls, bool sl, bool ss,
                                   bool device) {
  Value* C[5];
  C[0] = ConstantInt::get(Type::getInt1Ty(Context), ll);
  C[1] = ConstantInt::get(Type::getInt1Ty(Context), ls);
  C[2] = ConstantInt::get(Type::getInt1Ty(Context), sl);
  C[3] = ConstantInt::get(Type::getInt1Ty(Context), ss);
  C[4] = ConstantInt::get(Type::getInt1Ty(Context), device);

  Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                               Intrinsic::memory_barrier),
                     C, C + 5);
}

Value *
TreeToLLVM::BuildBinaryAtomicBuiltin(tree exp, Intrinsic::ID id) {
  const Type *ResultTy = ConvertType(TREE_TYPE(exp));
  tree arglist = TREE_OPERAND(exp, 1);
  Value* C[2] = {
    Emit(TREE_VALUE(arglist), 0),
    Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
  };
  const Type* Ty[2];
  Ty[0] = ResultTy;
  Ty[1] = ResultTy->getPointerTo();
  C[0] = Builder.CreateBitCast(C[0], Ty[1]);
  C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                               "cast");
  // The gcc builtins are also full memory barriers.
  // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
  EmitMemoryBarrier(false, false, true, true, false);
#else
  EmitMemoryBarrier(true, true, true, true, true);
#endif

  Value *Result =
    Builder.CreateCall(Intrinsic::getDeclaration(TheModule,  id,
                                                 Ty, 2),
    C, C + 2);

  // The gcc builtins are also full memory barriers.
  // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
  EmitMemoryBarrier(true, true, true, true, false);
#else
  EmitMemoryBarrier(true, true, true, true, true);
#endif

  Result = Builder.CreateIntToPtr(Result, ResultTy);
  return Result;
}

Value *
TreeToLLVM::BuildCmpAndSwapAtomicBuiltin(tree exp, tree type, bool isBool) {
  const Type *ResultTy = ConvertType(type);
  tree arglist = TREE_OPERAND(exp, 1);
  Value* C[3] = {
    Emit(TREE_VALUE(arglist), 0),
    Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0),
    Emit(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(arglist))), 0)
  };
  const Type* Ty[2];
  Ty[0] = ResultTy;
  Ty[1] = ResultTy->getPointerTo();
  C[0] = Builder.CreateBitCast(C[0], Ty[1]);
  C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                               "cast");
  C[2] = Builder.CreateIntCast(C[2], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                               "cast");

  // The gcc builtins are also full memory barriers.
  // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
  EmitMemoryBarrier(false, false, true, true, false);
#else
  EmitMemoryBarrier(true, true, true, true, true);
#endif

  Value *Result =
    Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                 Intrinsic::atomic_cmp_swap,
                                                 Ty, 2),
    C, C + 3);

  // The gcc builtins are also full memory barriers.
  // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
  EmitMemoryBarrier(true, true, true, true, false);
#else
  EmitMemoryBarrier(true, true, true, true, true);
#endif

  if (isBool)
    Result = CastToUIntType(Builder.CreateICmpEQ(Result, C[1]),
                            ConvertType(boolean_type_node));
  else
    Result = Builder.CreateIntToPtr(Result, ResultTy);
  return Result;
}

/// EmitBuiltinCall - exp is a call to fndecl, a builtin function.  Try to emit
/// the call in a special way, setting Result to the scalar result if necessary.
/// If we can't handle the builtin, return false, otherwise return true.
bool TreeToLLVM::EmitBuiltinCall(tree exp, tree fndecl,
                                 const MemRef *DestLoc, Value *&Result) {
  if (DECL_BUILT_IN_CLASS(fndecl) == BUILT_IN_MD) {
    unsigned FnCode = DECL_FUNCTION_CODE(fndecl);
    if (TargetBuiltinCache.size() <= FnCode)
      TargetBuiltinCache.resize(FnCode+1);

    // If we haven't converted this intrinsic over yet, do so now.
    if (TargetBuiltinCache[FnCode] == 0) {
      const char *TargetPrefix = "";
#ifdef LLVM_TARGET_INTRINSIC_PREFIX
      TargetPrefix = LLVM_TARGET_INTRINSIC_PREFIX;
#endif
      // If the backend has some special code to lower, go ahead and try to
      // do that first.
      if (EmitFrontendExpandedBuiltinCall(exp, fndecl, DestLoc, Result))
        return true;
      
      // If this builtin directly corresponds to an LLVM intrinsic, get the
      // IntrinsicID now.
      const char *BuiltinName = IDENTIFIER_POINTER(DECL_NAME(fndecl));
      Intrinsic::ID IntrinsicID =
        Intrinsic::getIntrinsicForGCCBuiltin(TargetPrefix, BuiltinName);
      if (IntrinsicID == Intrinsic::not_intrinsic) {

        error("%Hunsupported target builtin %<%s%> used", &EXPR_LOCATION(exp),
              BuiltinName);
        const Type *ResTy = ConvertType(TREE_TYPE(exp));
        if (ResTy->isSingleValueType())
          Result = UndefValue::get(ResTy);
        return true;
      }

      // Finally, map the intrinsic ID back to a name.
      TargetBuiltinCache[FnCode] =
        Intrinsic::getDeclaration(TheModule, IntrinsicID);
    }

    Result = EmitCallOf(TargetBuiltinCache[FnCode], exp, DestLoc,
                        AttrListPtr());
    return true;
  }

  enum built_in_function fcode = DECL_FUNCTION_CODE(fndecl);
  switch (fcode) {
  default: return false;
  // Varargs builtins.
  case BUILT_IN_VA_START:
  case BUILT_IN_STDARG_START:   return EmitBuiltinVAStart(exp);
  case BUILT_IN_VA_END:         return EmitBuiltinVAEnd(exp);
  case BUILT_IN_VA_COPY:        return EmitBuiltinVACopy(exp);
  case BUILT_IN_CONSTANT_P:     return EmitBuiltinConstantP(exp, Result);
  case BUILT_IN_ALLOCA:         return EmitBuiltinAlloca(exp, Result);
  case BUILT_IN_EXTEND_POINTER: return EmitBuiltinExtendPointer(exp, Result);
  case BUILT_IN_EXPECT:         return EmitBuiltinExpect(exp, DestLoc, Result);
  case BUILT_IN_MEMCPY:         return EmitBuiltinMemCopy(exp, Result,
                                                          false, false);
  case BUILT_IN_MEMCPY_CHK:     return EmitBuiltinMemCopy(exp, Result,
                                                          false, true);
  case BUILT_IN_MEMMOVE:        return EmitBuiltinMemCopy(exp, Result,
                                                          true, false);
  case BUILT_IN_MEMMOVE_CHK:    return EmitBuiltinMemCopy(exp, Result,
                                                          true, true);
  case BUILT_IN_MEMSET:         return EmitBuiltinMemSet(exp, Result, false);
  case BUILT_IN_MEMSET_CHK:     return EmitBuiltinMemSet(exp, Result, true);
  case BUILT_IN_BZERO:          return EmitBuiltinBZero(exp, Result);
  case BUILT_IN_PREFETCH:       return EmitBuiltinPrefetch(exp);
  case BUILT_IN_FRAME_ADDRESS:  return EmitBuiltinReturnAddr(exp, Result,true);
  case BUILT_IN_RETURN_ADDRESS: return EmitBuiltinReturnAddr(exp, Result,false);
  case BUILT_IN_STACK_SAVE:     return EmitBuiltinStackSave(exp, Result);
  case BUILT_IN_STACK_RESTORE:  return EmitBuiltinStackRestore(exp);
  case BUILT_IN_EXTRACT_RETURN_ADDR:
   return EmitBuiltinExtractReturnAddr(exp, Result);
  case BUILT_IN_FROB_RETURN_ADDR:
   return EmitBuiltinFrobReturnAddr(exp, Result);
  case BUILT_IN_INIT_TRAMPOLINE:
    return EmitBuiltinInitTrampoline(exp, Result);

  // Builtins used by the exception handling runtime.
  case BUILT_IN_DWARF_CFA:
    return EmitBuiltinDwarfCFA(exp, Result);
#ifdef DWARF2_UNWIND_INFO
  case BUILT_IN_DWARF_SP_COLUMN:
    return EmitBuiltinDwarfSPColumn(exp, Result);
  case BUILT_IN_INIT_DWARF_REG_SIZES:
    return EmitBuiltinInitDwarfRegSizes(exp, Result);
#endif
  case BUILT_IN_EH_RETURN:
    return EmitBuiltinEHReturn(exp, Result);
#ifdef EH_RETURN_DATA_REGNO
  case BUILT_IN_EH_RETURN_DATA_REGNO:
    return EmitBuiltinEHReturnDataRegno(exp, Result);
#endif
  case BUILT_IN_UNWIND_INIT:
    return EmitBuiltinUnwindInit(exp, Result);

  case BUILT_IN_OBJECT_SIZE: {
    tree arglist = TREE_OPERAND (exp, 1);
    if (!validate_arglist(arglist, POINTER_TYPE, INTEGER_TYPE, VOID_TYPE)) {
      error("Invalid builtin_object_size argument types");
      return false;
    }
    tree ObjSizeTree = TREE_VALUE (TREE_CHAIN (arglist));
    STRIP_NOPS (ObjSizeTree);
    if (TREE_CODE (ObjSizeTree) != INTEGER_CST
        || tree_int_cst_sgn (ObjSizeTree) < 0
        || compare_tree_int (ObjSizeTree, 3) > 0) {
      error("Invalid second builtin_object_size argument");
      return false;
    }

    tree Object = TREE_VALUE(arglist);
    tree ObjTy = TREE_VALUE(TREE_CHAIN(arglist));

    // LLVM doesn't handle type 1 or type 3. Deal with that here.
    Value *Tmp = Emit(ObjTy, 0);
    
    ConstantInt *CI = cast<ConstantInt>(Tmp);
    
    // Clear the bottom bit since we only handle whole objects and shift to turn
    // the second bit into our boolean.
    uint64_t val = (CI->getZExtValue() & 0x2) >> 1;
    
    Value *NewTy = ConstantInt::get(Tmp->getType(), val);
    
    Value* Args[] = {
      Emit(Object, 0),
      NewTy
    };

    // Grab the current return type.
    const Type* Ty;
    Ty = ConvertType(TREE_TYPE(exp));

    // Manually coerce the arg to the correct pointer type.
    Args[0] = Builder.CreateBitCast(Args[0], Type::getInt8PtrTy(Context));
    Args[1] = Builder.CreateIntCast(Args[1], Type::getInt1Ty(Context),
                                    false);

    Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                          Intrinsic::objectsize,
                                                          &Ty,
                                                          1),
                                Args, Args + 2);
    return true;
  }
  // Unary bit counting intrinsics.
  // NOTE: do not merge these case statements.  That will cause the memoized
  // Function* to be incorrectly shared across the different typed functions.
  case BUILT_IN_CLZ:       // These GCC builtins always return int.
  case BUILT_IN_CLZL:
  case BUILT_IN_CLZLL: {
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::ctlz);
    const Type *DestTy = ConvertType(TREE_TYPE(exp));
    Result = Builder.CreateIntCast(Result, DestTy, 
                                   !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                   "cast");
    return true;
  }
  case BUILT_IN_CTZ:       // These GCC builtins always return int.
  case BUILT_IN_CTZL:
  case BUILT_IN_CTZLL: {
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::cttz);
    const Type *DestTy = ConvertType(TREE_TYPE(exp));
    Result = Builder.CreateIntCast(Result, DestTy,
                                   !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                   "cast");
    return true;
  }
  case BUILT_IN_PARITYLL:
  case BUILT_IN_PARITYL:
  case BUILT_IN_PARITY: {
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::ctpop);
    Result = Builder.CreateBinOp(Instruction::And, Result,
                                 ConstantInt::get(Result->getType(), 1));
    const Type *DestTy = ConvertType(TREE_TYPE(exp));
    Result = Builder.CreateIntCast(Result, DestTy,
                                   !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                   "cast");
    return true;
  }
  case BUILT_IN_POPCOUNT:  // These GCC builtins always return int.
  case BUILT_IN_POPCOUNTL:
  case BUILT_IN_POPCOUNTLL: {
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::ctpop);
    const Type *DestTy = ConvertType(TREE_TYPE(exp));
    Result = Builder.CreateIntCast(Result, DestTy,
                                   !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                   "cast");
    return true;
  }
  case BUILT_IN_BSWAP32:
  case BUILT_IN_BSWAP64: {
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::bswap);
    const Type *DestTy = ConvertType(TREE_TYPE(exp));
    Result = Builder.CreateIntCast(Result, DestTy,
                                   !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                   "cast");
    return true;
  }

  case BUILT_IN_SQRT:
  case BUILT_IN_SQRTF:
  case BUILT_IN_SQRTL:
    // The result of sqrt(negative) is implementation-defined, but follows
    // IEEE754 in most current implementations. llvm.sqrt, which has undefined
    // behavior for such inputs, is an inappropriate substitute.
    break;
  case BUILT_IN_POWI:
  case BUILT_IN_POWIF:
  case BUILT_IN_POWIL:
    Result = EmitBuiltinPOWI(exp);
    return true;
  case BUILT_IN_POW:
  case BUILT_IN_POWF:
  case BUILT_IN_POWL:
    // If errno math has been disabled, expand these to llvm.pow calls.
    if (!flag_errno_math) {
      Result = EmitBuiltinPOW(exp);
      return true;
    }
    break;
  case BUILT_IN_LOG:
  case BUILT_IN_LOGF:
  case BUILT_IN_LOGL:
    // If errno math has been disabled, expand these to llvm.log calls.
    if (!flag_errno_math) {
      Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
      EmitBuiltinUnaryOp(Amt, Result, Intrinsic::log);
      Result = CastToFPType(Result, ConvertType(TREE_TYPE(exp)));
      return true;
    }
    break;
  case BUILT_IN_LOG2:
  case BUILT_IN_LOG2F:
  case BUILT_IN_LOG2L:
    // If errno math has been disabled, expand these to llvm.log2 calls.
    if (!flag_errno_math) {
      Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
      EmitBuiltinUnaryOp(Amt, Result, Intrinsic::log2);
      Result = CastToFPType(Result, ConvertType(TREE_TYPE(exp)));
      return true;
    }
    break;
  case BUILT_IN_LOG10:
  case BUILT_IN_LOG10F:
  case BUILT_IN_LOG10L:
    // If errno math has been disabled, expand these to llvm.log10 calls.
    if (!flag_errno_math) {
      Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
      EmitBuiltinUnaryOp(Amt, Result, Intrinsic::log10);
      Result = CastToFPType(Result, ConvertType(TREE_TYPE(exp)));
      return true;
    }
    break;
  case BUILT_IN_EXP:
  case BUILT_IN_EXPF:
  case BUILT_IN_EXPL:
    // If errno math has been disabled, expand these to llvm.exp calls.
    if (!flag_errno_math) {
      Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
      EmitBuiltinUnaryOp(Amt, Result, Intrinsic::exp);
      Result = CastToFPType(Result, ConvertType(TREE_TYPE(exp)));
      return true;
    }
    break;
  case BUILT_IN_EXP2:
  case BUILT_IN_EXP2F:
  case BUILT_IN_EXP2L:
    // If errno math has been disabled, expand these to llvm.exp2 calls.
    if (!flag_errno_math) {
      Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
      EmitBuiltinUnaryOp(Amt, Result, Intrinsic::exp2);
      Result = CastToFPType(Result, ConvertType(TREE_TYPE(exp)));
      return true;
    }
    break;
  case BUILT_IN_FFS:  // These GCC builtins always return int.
  case BUILT_IN_FFSL:
  case BUILT_IN_FFSLL: {      // FFS(X) -> (x == 0 ? 0 : CTTZ(x)+1)
    // The argument and return type of cttz should match the argument type of
    // the ffs, but should ignore the return type of ffs.
    Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
    EmitBuiltinUnaryOp(Amt, Result, Intrinsic::cttz);
    Result = Builder.CreateAdd(Result,
      ConstantInt::get(Result->getType(), 1));
    Result = CastToUIntType(Result, ConvertType(TREE_TYPE(exp)));
    Value *Cond =
      Builder.CreateICmpEQ(Amt,
                           Constant::getNullValue(Amt->getType()));
    Result = Builder.CreateSelect(Cond,
                           Constant::getNullValue(Result->getType()),
                                  Result);
    return true;
  }
  case BUILT_IN_FLT_ROUNDS: {
    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::flt_rounds));
    Result = BitCastToType(Result, ConvertType(TREE_TYPE(exp)));
    return true;
  }
  case BUILT_IN_TRAP:
    Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::trap));
    // Emit an explicit unreachable instruction.
    Builder.CreateUnreachable();
    EmitBlock(BasicBlock::Create(Context, ""));
    return true;

  // Convert annotation built-in to llvm.annotation intrinsic.
  case BUILT_IN_ANNOTATION: {

    // Get file and line number
    location_t locus = EXPR_LOCATION (exp);
    Constant *lineNo = ConstantInt::get(Type::getInt32Ty(Context), locus.line);
    Constant *file = ConvertMetadataStringToGV(locus.file);
    const Type *SBP= Type::getInt8PtrTy(Context);
    file = Builder.getFolder().CreateBitCast(file, SBP);

    // Get arguments.
    tree arglist = TREE_OPERAND(exp, 1);
    Value *ExprVal = Emit(TREE_VALUE(arglist), 0);
    const Type *Ty = ExprVal->getType();
    Value *StrVal = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);

    SmallVector<Value *, 4> Args;
    Args.push_back(ExprVal);
    Args.push_back(StrVal);
    Args.push_back(file);
    Args.push_back(lineNo);

    assert(Ty && "llvm.annotation arg type may not be null");
    Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                          Intrinsic::annotation,
                                                          &Ty,
                                                          1),
                                Args.begin(), Args.end());
    return true;
  }

  case BUILT_IN_SYNCHRONIZE: {
    // We assume like gcc appears to, that this only applies to cached memory.
    Value* C[5];
    C[0] = C[1] = C[2] = C[3] = ConstantInt::get(Type::getInt1Ty(Context), 1);
    C[4] = ConstantInt::get(Type::getInt1Ty(Context), 0);

    Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                 Intrinsic::memory_barrier),
                       C, C + 5);
    return true;
  }
#if defined(TARGET_ALPHA) || defined(TARGET_386) || defined(TARGET_POWERPC) \
    || defined(TARGET_ARM)
    // gcc uses many names for the sync intrinsics
    // The type of the first argument is not reliable for choosing the
    // right llvm function; if the original type is not volatile, gcc has
    // helpfully changed it to "volatile void *" at this point.  The
    // original type can be recovered from the function type in most cases.
    // For lock_release and bool_compare_and_swap even that is not good
    // enough, we have to key off the opcode.
    // Note that Intrinsic::getDeclaration expects the type list in reversed
    // order, while CreateCall expects the parameter list in normal order.
  case BUILT_IN_BOOL_COMPARE_AND_SWAP_1: {
    Result = BuildCmpAndSwapAtomicBuiltin(exp, unsigned_char_type_node, true);
    return true;
  }
  case BUILT_IN_BOOL_COMPARE_AND_SWAP_2: {
    Result = BuildCmpAndSwapAtomicBuiltin(exp, short_unsigned_type_node, true);
    return true;
  }
  case BUILT_IN_BOOL_COMPARE_AND_SWAP_4: {
    Result = BuildCmpAndSwapAtomicBuiltin(exp, unsigned_type_node, true);
    return true;
  }
  case BUILT_IN_BOOL_COMPARE_AND_SWAP_8: {
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
    Result = BuildCmpAndSwapAtomicBuiltin(exp, long_long_unsigned_type_node,
                                          true);
    return true;
  }

  case BUILT_IN_VAL_COMPARE_AND_SWAP_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_VAL_COMPARE_AND_SWAP_1:
  case BUILT_IN_VAL_COMPARE_AND_SWAP_2:
  case BUILT_IN_VAL_COMPARE_AND_SWAP_4: {
    tree type = TREE_TYPE(exp);
    Result = BuildCmpAndSwapAtomicBuiltin(exp, type, false);
    return true;
  }
  case BUILT_IN_FETCH_AND_ADD_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_ADD_1:
  case BUILT_IN_FETCH_AND_ADD_2:
  case BUILT_IN_FETCH_AND_ADD_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_add);
    return true;
  }
  case BUILT_IN_FETCH_AND_SUB_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_SUB_1:
  case BUILT_IN_FETCH_AND_SUB_2:
  case BUILT_IN_FETCH_AND_SUB_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_sub);
    return true;
  }
  case BUILT_IN_FETCH_AND_OR_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_OR_1:
  case BUILT_IN_FETCH_AND_OR_2:
  case BUILT_IN_FETCH_AND_OR_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_or);
    return true;
  }
  case BUILT_IN_FETCH_AND_AND_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_AND_1:
  case BUILT_IN_FETCH_AND_AND_2:
  case BUILT_IN_FETCH_AND_AND_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_and);
    return true;
  }
  case BUILT_IN_FETCH_AND_XOR_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_XOR_1:
  case BUILT_IN_FETCH_AND_XOR_2:
  case BUILT_IN_FETCH_AND_XOR_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_xor);
    return true;
  }
  case BUILT_IN_FETCH_AND_NAND_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_FETCH_AND_NAND_1:
  case BUILT_IN_FETCH_AND_NAND_2:
  case BUILT_IN_FETCH_AND_NAND_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_load_nand);
    return true;
  }
  case BUILT_IN_LOCK_TEST_AND_SET_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_LOCK_TEST_AND_SET_1:
  case BUILT_IN_LOCK_TEST_AND_SET_2:
  case BUILT_IN_LOCK_TEST_AND_SET_4: {
    Result = BuildBinaryAtomicBuiltin(exp, Intrinsic::atomic_swap);
    return true;
  }

  case BUILT_IN_ADD_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_ADD_AND_FETCH_1:
  case BUILT_IN_ADD_AND_FETCH_2:
  case BUILT_IN_ADD_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_add,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateAdd(Result, C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }
  case BUILT_IN_SUB_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_SUB_AND_FETCH_1:
  case BUILT_IN_SUB_AND_FETCH_2:
  case BUILT_IN_SUB_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_sub,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateSub(Result, C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }
  case BUILT_IN_OR_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_OR_AND_FETCH_1:
  case BUILT_IN_OR_AND_FETCH_2:
  case BUILT_IN_OR_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_or,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateOr(Result, C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }
  case BUILT_IN_AND_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_AND_AND_FETCH_1:
  case BUILT_IN_AND_AND_FETCH_2:
  case BUILT_IN_AND_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_and,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateAnd(Result, C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }
  case BUILT_IN_XOR_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_XOR_AND_FETCH_1:
  case BUILT_IN_XOR_AND_FETCH_2:
  case BUILT_IN_XOR_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_xor,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateXor(Result, C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }
  case BUILT_IN_NAND_AND_FETCH_8:
#if defined(TARGET_POWERPC)
    if (!TARGET_64BIT)
      return false;
#endif
  case BUILT_IN_NAND_AND_FETCH_1:
  case BUILT_IN_NAND_AND_FETCH_2:
  case BUILT_IN_NAND_AND_FETCH_4: {
    const Type *ResultTy = ConvertType(TREE_TYPE(exp));
    tree arglist = TREE_OPERAND(exp, 1);
    Value* C[2] = {
      Emit(TREE_VALUE(arglist), 0),
      Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0)
    };
    const Type* Ty[2];
    Ty[0] = ResultTy;
    Ty[1] = ResultTy->getPointerTo();
    C[0] = Builder.CreateBitCast(C[0], Ty[1]);
    C[1] = Builder.CreateIntCast(C[1], Ty[0], !TYPE_UNSIGNED(TREE_TYPE(exp)),
                                 "cast");

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(false, false, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result =
      Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                   Intrinsic::atomic_load_nand,
                                                   Ty, 2),
                         C, C + 2);

    // The gcc builtins are also full memory barriers.
    // FIXME: __sync_lock_test_and_set and __sync_lock_release require less.
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    EmitMemoryBarrier(true, true, true, true, false);
#else
    EmitMemoryBarrier(true, true, true, true, true);
#endif

    Result = Builder.CreateAnd(Builder.CreateNot(Result), C[1]);
    Result = Builder.CreateIntToPtr(Result, ResultTy);
    return true;
  }

  case BUILT_IN_LOCK_RELEASE_1:
  case BUILT_IN_LOCK_RELEASE_2:
  case BUILT_IN_LOCK_RELEASE_4:
  case BUILT_IN_LOCK_RELEASE_8:
  case BUILT_IN_LOCK_RELEASE_16: {
    // This is effectively a volatile store of 0, and has no return value.
    // The argument has typically been coerced to "volatile void*"; the
    // only way to find the size of the operation is from the builtin
    // opcode.
    tree type;
    switch(DECL_FUNCTION_CODE(fndecl)) {
      case BUILT_IN_LOCK_RELEASE_1:
        type = unsigned_char_type_node; break;
      case BUILT_IN_LOCK_RELEASE_2:
        type = short_unsigned_type_node; break;
      case BUILT_IN_LOCK_RELEASE_4:
        type = unsigned_type_node; break;
      case BUILT_IN_LOCK_RELEASE_8:
        type = long_long_unsigned_type_node; break;
      case BUILT_IN_LOCK_RELEASE_16:    // not handled; should use SSE on x86
      default:
        abort();
    }
    tree arglist = TREE_OPERAND(exp, 1);
    tree t1 = build1 (INDIRECT_REF, type, TREE_VALUE (arglist));
    TREE_THIS_VOLATILE(t1) = 1;
    tree t = build2 (MODIFY_EXPR, type, t1,
                     build_int_cst (type, (HOST_WIDE_INT)0));
    EmitMODIFY_EXPR(t, 0);
    Result = 0;
    return true;
  }

#endif //FIXME: these break the build for backends that haven't implemented them


#if 1  // FIXME: Should handle these GCC extensions eventually.
  case BUILT_IN_LONGJMP: {
    tree arglist = TREE_OPERAND(exp, 1);

    if (validate_arglist(arglist, POINTER_TYPE, INTEGER_TYPE, VOID_TYPE)) {
      tree value = TREE_VALUE(TREE_CHAIN(arglist));

      if (TREE_CODE(value) != INTEGER_CST ||
          cast<ConstantInt>(Emit(value, 0))->getValue() != 1) {
        error ("%<__builtin_longjmp%> second argument must be 1");
        return false;
      }
    }
#if defined(TARGET_ARM) && defined(CONFIG_DARWIN_H)
    Value *Buf = Emit(TREE_VALUE(arglist), 0);
    Buf = Builder.CreateBitCast(Buf, Type::getInt8Ty(Context)->getPointerTo());
    Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                 Intrinsic::eh_sjlj_longjmp),
                      Buf);
    Result = 0;
    return true;
#endif
    // Fall-through
  }
  case BUILT_IN_APPLY_ARGS:
  case BUILT_IN_APPLY:
  case BUILT_IN_RETURN:
  case BUILT_IN_SAVEREGS:
  case BUILT_IN_ARGS_INFO:
  case BUILT_IN_NEXT_ARG:
  case BUILT_IN_CLASSIFY_TYPE:
  case BUILT_IN_AGGREGATE_INCOMING_ADDRESS:
  case BUILT_IN_SETJMP_SETUP:
  case BUILT_IN_SETJMP_DISPATCHER:
  case BUILT_IN_SETJMP_RECEIVER:
  case BUILT_IN_UPDATE_SETJMP_BUF:

    // FIXME: HACK: Just ignore these.
    {
      const Type *Ty = ConvertType(TREE_TYPE(exp));
      if (Ty != Type::getVoidTy(Context))
        Result = Constant::getNullValue(Ty);
      return true;
    }
#endif  // FIXME: Should handle these GCC extensions eventually.
  }
  return false;
}

bool TreeToLLVM::EmitBuiltinUnaryOp(Value *InVal, Value *&Result,
                                    Intrinsic::ID Id) {
  // The intrinsic might be overloaded in which case the argument is of
  // varying type. Make sure that we specify the actual type for "iAny"
  // by passing it as the 3rd and 4th parameters. This isn't needed for
  // most intrinsics, but is needed for ctpop, cttz, ctlz.
  const Type *Ty = InVal->getType();
  Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Id, &Ty, 1),
                              InVal);
  return true;
}

Value *TreeToLLVM::EmitBuiltinSQRT(tree exp) {
  Value *Amt = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
  const Type* Ty = Amt->getType();

  return Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                      Intrinsic::sqrt, &Ty, 1),
                            Amt);
}

Value *TreeToLLVM::EmitBuiltinPOWI(tree exp) {
  tree ArgList = TREE_OPERAND (exp, 1);
  if (!validate_arglist(ArgList, REAL_TYPE, INTEGER_TYPE, VOID_TYPE))
    return 0;

  Value *Val = Emit(TREE_VALUE(ArgList), 0);
  Value *Pow = Emit(TREE_VALUE(TREE_CHAIN(ArgList)), 0);
  const Type *Ty = Val->getType();
  Pow = CastToSIntType(Pow, Type::getInt32Ty(Context));

  SmallVector<Value *,2> Args;
  Args.push_back(Val);
  Args.push_back(Pow);
  return Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                      Intrinsic::powi, &Ty, 1),
                            Args.begin(), Args.end());
}

Value *TreeToLLVM::EmitBuiltinPOW(tree exp) {
  tree ArgList = TREE_OPERAND (exp, 1);
  if (!validate_arglist(ArgList, REAL_TYPE, REAL_TYPE, VOID_TYPE))
    return 0;

  Value *Val = Emit(TREE_VALUE(ArgList), 0);
  Value *Pow = Emit(TREE_VALUE(TREE_CHAIN(ArgList)), 0);
  const Type *Ty = Val->getType();

  SmallVector<Value *,2> Args;
  Args.push_back(Val);
  Args.push_back(Pow);
  return Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                      Intrinsic::pow, &Ty, 1),
                            Args.begin(), Args.end());
}

bool TreeToLLVM::EmitBuiltinConstantP(tree exp, Value *&Result) {
  Result = Constant::getNullValue(ConvertType(TREE_TYPE(exp)));
  return true;
}

bool TreeToLLVM::EmitBuiltinExtendPointer(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);
  Value *Amt = Emit(TREE_VALUE(arglist), 0);
  bool AmtIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_VALUE(arglist)));
  bool ExpIsSigned = !TYPE_UNSIGNED(TREE_TYPE(exp));
  Result = CastToAnyType(Amt, AmtIsSigned, ConvertType(TREE_TYPE(exp)),
                         ExpIsSigned);
  return true;
}

/// OptimizeIntoPlainBuiltIn - Return true if it's safe to lower the object
/// size checking builtin calls (e.g. __builtin___memcpy_chk into the
/// plain non-checking calls. If the size of the argument is either -1 (unknown)
/// or large enough to ensure no overflow (> len), then it's safe to do so.
static bool OptimizeIntoPlainBuiltIn(tree exp, Value *Len, Value *Size) {
  if (BitCastInst *SizeBC = dyn_cast<BitCastInst>(Size))
    Size = SizeBC->getOperand(0);
  ConstantInt *SizeCI = dyn_cast<ConstantInt>(Size);
  if (!SizeCI)
    return false;
  if (SizeCI->isAllOnesValue())
    // If size is -1, convert to plain memcpy, etc.
    return true;

  if (BitCastInst *LenBC = dyn_cast<BitCastInst>(Len))
    Len = LenBC->getOperand(0);
  ConstantInt *LenCI = dyn_cast<ConstantInt>(Len);
  if (!LenCI)
    return false;
  if (SizeCI->getValue().ult(LenCI->getValue())) {
    location_t locus = EXPR_LOCATION(exp);
    warning (0, "%Hcall to %D will always overflow destination buffer",
             &locus, get_callee_fndecl(exp));
    return false;
  }
  return true;
}

/// EmitBuiltinMemCopy - Emit an llvm.memcpy or llvm.memmove intrinsic,
/// depending on the value of isMemMove.
bool TreeToLLVM::EmitBuiltinMemCopy(tree exp, Value *&Result, bool isMemMove,
                                    bool SizeCheck) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (SizeCheck) {
    if (!validate_arglist(arglist, POINTER_TYPE, POINTER_TYPE,
                          INTEGER_TYPE, INTEGER_TYPE, VOID_TYPE))
      return false;
  } else {
    if (!validate_arglist(arglist, POINTER_TYPE, POINTER_TYPE,
                          INTEGER_TYPE, VOID_TYPE))
      return false;
  }

  tree Dst = TREE_VALUE(arglist);
  tree Src = TREE_VALUE(TREE_CHAIN(arglist));
  unsigned SrcAlign = getPointerAlignment(Src);
  unsigned DstAlign = getPointerAlignment(Dst);

  Value *DstV = Emit(Dst, 0);
  Value *SrcV = Emit(Src, 0);
  Value *Len = Emit(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(arglist))), 0);
  if (SizeCheck) {
    tree SizeArg = TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(arglist))));
    Value *Size = Emit(SizeArg, 0);
    if (!OptimizeIntoPlainBuiltIn(exp, Len, Size))
      return false;
  }

  Result = isMemMove ?
    EmitMemMove(DstV, SrcV, Len, std::min(SrcAlign, DstAlign)) :
    EmitMemCpy(DstV, SrcV, Len, std::min(SrcAlign, DstAlign));
  return true;
}

bool TreeToLLVM::EmitBuiltinMemSet(tree exp, Value *&Result, bool SizeCheck) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (SizeCheck) {
    if (!validate_arglist(arglist, POINTER_TYPE, INTEGER_TYPE,
                          INTEGER_TYPE, INTEGER_TYPE, VOID_TYPE))
      return false;
  } else {
    if (!validate_arglist(arglist, POINTER_TYPE, INTEGER_TYPE,
                          INTEGER_TYPE, VOID_TYPE))
      return false;
  }

  tree Dst = TREE_VALUE(arglist);
  unsigned DstAlign = getPointerAlignment(Dst);

  Value *DstV = Emit(Dst, 0);
  Value *Val = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);
  Value *Len = Emit(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(arglist))), 0);
  if (SizeCheck) {
    tree SizeArg = TREE_VALUE(TREE_CHAIN(TREE_CHAIN(TREE_CHAIN(arglist))));
    Value *Size = Emit(SizeArg, 0);
    if (!OptimizeIntoPlainBuiltIn(exp, Len, Size))
      return false;
  }
  Result = EmitMemSet(DstV, Val, Len, DstAlign);
  return true;
}

bool TreeToLLVM::EmitBuiltinBZero(tree exp, Value *&/*Result*/) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, POINTER_TYPE, INTEGER_TYPE, VOID_TYPE))
    return false;

  tree Dst = TREE_VALUE(arglist);
  unsigned DstAlign = getPointerAlignment(Dst);

  Value *DstV = Emit(Dst, 0);
  Value *Val = Constant::getNullValue(Type::getInt32Ty(Context));
  Value *Len = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);
  EmitMemSet(DstV, Val, Len, DstAlign);
  return true;
}

bool TreeToLLVM::EmitBuiltinPrefetch(tree exp) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, POINTER_TYPE, 0))
    return false;

  Value *Ptr = Emit(TREE_VALUE(arglist), 0);
  Value *ReadWrite = 0;
  Value *Locality = 0;

  if (TREE_CHAIN(arglist)) { // Args 1/2 are optional
    ReadWrite = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);
    if (!isa<ConstantInt>(ReadWrite)) {
      error("second argument to %<__builtin_prefetch%> must be a constant");
      ReadWrite = 0;
    } else if (cast<ConstantInt>(ReadWrite)->getZExtValue() > 1) {
      warning (0, "invalid second argument to %<__builtin_prefetch%>;"
               " using zero");
      ReadWrite = 0;
    } else {
      ReadWrite = Builder.getFolder().CreateIntCast(cast<Constant>(ReadWrite),
                                                    Type::getInt32Ty(Context),
                                                    false);
    }

    if (TREE_CHAIN(TREE_CHAIN(arglist))) {
      Locality = Emit(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(arglist))), 0);
      if (!isa<ConstantInt>(Locality)) {
        error("third argument to %<__builtin_prefetch%> must be a constant");
        Locality = 0;
      } else if (cast<ConstantInt>(Locality)->getZExtValue() > 3) {
        warning(0, "invalid third argument to %<__builtin_prefetch%>; using 3");
        Locality = 0;
      } else {
        Locality = Builder.getFolder().CreateIntCast(cast<Constant>(Locality),
                                                     Type::getInt32Ty(Context),
                                                     false);
      }
    }
  }

  // Default to highly local read.
  if (ReadWrite == 0)
    ReadWrite = Constant::getNullValue(Type::getInt32Ty(Context));
  if (Locality == 0)
    Locality = ConstantInt::get(Type::getInt32Ty(Context), 3);

  Ptr = BitCastToType(Ptr, Type::getInt8PtrTy(Context));

  Value *Ops[3] = { Ptr, ReadWrite, Locality };
  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::prefetch),
                     Ops, Ops+3);
  return true;
}

/// EmitBuiltinReturnAddr - Emit an llvm.returnaddress or llvm.frameaddress
/// instruction, depending on whether isFrame is true or not.
bool TreeToLLVM::EmitBuiltinReturnAddr(tree exp, Value *&Result, bool isFrame) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, INTEGER_TYPE, VOID_TYPE))
    return false;

  ConstantInt *Level = dyn_cast<ConstantInt>(Emit(TREE_VALUE(arglist), 0));
  if (!Level) {
    if (isFrame)
      error("invalid argument to %<__builtin_frame_address%>");
    else
      error("invalid argument to %<__builtin_return_address%>");
    return false;
  }

  Intrinsic::ID IID =
    !isFrame ? Intrinsic::returnaddress : Intrinsic::frameaddress;
  Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule, IID), Level);
  Result = BitCastToType(Result, ConvertType(TREE_TYPE(exp)));
  return true;
}

bool TreeToLLVM::EmitBuiltinExtractReturnAddr(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);

  Value *Ptr = Emit(TREE_VALUE(arglist), 0);

  // FIXME: Actually we should do something like this:
  //
  // Result = (Ptr & MASK_RETURN_ADDR) + RETURN_ADDR_OFFSET, if mask and
  // offset are defined. This seems to be needed for: ARM, MIPS, Sparc.
  // Unfortunately, these constants are defined as RTL expressions and
  // should be handled separately.

  Result = BitCastToType(Ptr, Type::getInt8PtrTy(Context));

  return true;
}

bool TreeToLLVM::EmitBuiltinFrobReturnAddr(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);

  Value *Ptr = Emit(TREE_VALUE(arglist), 0);

  // FIXME: Actually we should do something like this:
  //
  // Result = Ptr - RETURN_ADDR_OFFSET, if offset is defined. This seems to be
  // needed for: MIPS, Sparc.  Unfortunately, these constants are defined
  // as RTL expressions and should be handled separately.

  Result = BitCastToType(Ptr, Type::getInt8PtrTy(Context));

  return true;
}

bool TreeToLLVM::EmitBuiltinStackSave(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, VOID_TYPE))
    return false;

  Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                        Intrinsic::stacksave));
  return true;
}


// Builtins used by the exception handling runtime.

// On most machines, the CFA coincides with the first incoming parm.
#ifndef ARG_POINTER_CFA_OFFSET
#define ARG_POINTER_CFA_OFFSET(FNDECL) FIRST_PARM_OFFSET (FNDECL)
#endif

// The mapping from gcc register number to DWARF 2 CFA column number.  By
// default, we just provide columns for all registers.
#ifndef DWARF_FRAME_REGNUM
#define DWARF_FRAME_REGNUM(REG) DBX_REGISTER_NUMBER (REG)
#endif

// Map register numbers held in the call frame info that gcc has
// collected using DWARF_FRAME_REGNUM to those that should be output in
// .debug_frame and .eh_frame.
#ifndef DWARF2_FRAME_REG_OUT
#define DWARF2_FRAME_REG_OUT(REGNO, FOR_EH) (REGNO)
#endif

/* Registers that get partially clobbered by a call in a given mode.
   These must not be call used registers.  */
#ifndef HARD_REGNO_CALL_PART_CLOBBERED
#define HARD_REGNO_CALL_PART_CLOBBERED(REGNO, MODE) 0
#endif

bool TreeToLLVM::EmitBuiltinDwarfCFA(tree exp, Value *&Result) {
  if (!validate_arglist(TREE_OPERAND(exp, 1), VOID_TYPE))
    return false;

  int cfa_offset = ARG_POINTER_CFA_OFFSET(exp);

  // FIXME: is i32 always enough here?
  Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                        Intrinsic::eh_dwarf_cfa),
                                      ConstantInt::get(Type::getInt32Ty(Context),
                                                       cfa_offset));

  return true;
}

bool TreeToLLVM::EmitBuiltinDwarfSPColumn(tree exp, Value *&Result) {
  if (!validate_arglist(TREE_OPERAND(exp, 1), VOID_TYPE))
    return false;

  unsigned int dwarf_regnum = DWARF_FRAME_REGNUM(STACK_POINTER_REGNUM);
  Result = ConstantInt::get(ConvertType(TREE_TYPE(exp)), dwarf_regnum);

  return true;
}

bool TreeToLLVM::EmitBuiltinEHReturnDataRegno(tree exp, Value *&Result) {
#ifdef EH_RETURN_DATA_REGNO
  tree arglist = TREE_OPERAND(exp, 1);

  if (!validate_arglist(arglist, INTEGER_TYPE, VOID_TYPE))
    return false;

  tree which = TREE_VALUE (arglist);
  unsigned HOST_WIDE_INT iwhich;

  if (TREE_CODE (which) != INTEGER_CST) {
    error ("argument of %<__builtin_eh_return_regno%> must be constant");
    return false;
  }

  iwhich = tree_low_cst (which, 1);
  iwhich = EH_RETURN_DATA_REGNO (iwhich);
  if (iwhich == INVALID_REGNUM)
    return false;

  iwhich = DWARF_FRAME_REGNUM (iwhich);

  Result = ConstantInt::get(ConvertType(TREE_TYPE(exp)), iwhich);
#endif

  return true;
}

bool TreeToLLVM::EmitBuiltinEHReturn(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);

  if (!validate_arglist(arglist, INTEGER_TYPE, POINTER_TYPE, VOID_TYPE))
    return false;

  const Type *IntPtr = TD.getIntPtrType(Context);
  Value *Offset = Emit(TREE_VALUE(arglist), 0);
  Value *Handler = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);

  Intrinsic::ID IID = IntPtr->isIntegerTy(32) ?
    Intrinsic::eh_return_i32 : Intrinsic::eh_return_i64;

  Offset = Builder.CreateIntCast(Offset, IntPtr, true);
  Handler = BitCastToType(Handler, Type::getInt8PtrTy(Context));

  SmallVector<Value *, 2> Args;
  Args.push_back(Offset);
  Args.push_back(Handler);
  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, IID),
                     Args.begin(), Args.end());
  Result = Builder.CreateUnreachable();
  EmitBlock(BasicBlock::Create(Context, ""));

  return true;
}

bool TreeToLLVM::EmitBuiltinInitDwarfRegSizes(tree exp, Value *&/*Result*/) {
#ifdef DWARF2_UNWIND_INFO
  unsigned int i;
  bool wrote_return_column = false;
  static bool reg_modes_initialized = false;

  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, POINTER_TYPE, VOID_TYPE))
    return false;

  if (!reg_modes_initialized) {
    init_reg_modes_once();
    reg_modes_initialized = true;
  }

  Value *Addr = BitCastToType(Emit(TREE_VALUE(arglist), 0),
                              Type::getInt8PtrTy(Context));
  Constant *Size, *Idx;

  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++) {
    int rnum = DWARF2_FRAME_REG_OUT (DWARF_FRAME_REGNUM (i), 1);

    if (rnum < DWARF_FRAME_REGISTERS) {
      enum machine_mode save_mode = reg_raw_mode[i];
      HOST_WIDE_INT size;

      if (HARD_REGNO_CALL_PART_CLOBBERED (i, save_mode))
        save_mode = choose_hard_reg_mode (i, 1, true);
      if (DWARF_FRAME_REGNUM (i) == DWARF_FRAME_RETURN_COLUMN) {
        if (save_mode == VOIDmode)
          continue;
        wrote_return_column = true;
      }
      size = GET_MODE_SIZE (save_mode);
      if (rnum < 0)
        continue;

      Size = ConstantInt::get(Type::getInt8Ty(Context), size);
      Idx  = ConstantInt::get(Type::getInt32Ty(Context), rnum);
      Builder.CreateStore(Size, Builder.CreateGEP(Addr, Idx), false);
    }
  }

  if (!wrote_return_column) {
    Size = ConstantInt::get(Type::getInt8Ty(Context), GET_MODE_SIZE (Pmode));
    Idx  = ConstantInt::get(Type::getInt32Ty(Context),
                            DWARF_FRAME_RETURN_COLUMN);
    Builder.CreateStore(Size, Builder.CreateGEP(Addr, Idx), false);
  }

#ifdef DWARF_ALT_FRAME_RETURN_COLUMN
  Size = ConstantInt::get(Type::getInt8Ty(Context), GET_MODE_SIZE (Pmode));
  Idx  = ConstantInt::get(Type::getInt32Ty(Context),
                          DWARF_ALT_FRAME_RETURN_COLUMN);
  Builder.CreateStore(Size, Builder.CreateGEP(Addr, Idx), false);
#endif

#endif /* DWARF2_UNWIND_INFO */

  // TODO: the RS6000 target needs extra initialization [gcc changeset 122468].

  return true;
}

bool TreeToLLVM::EmitBuiltinUnwindInit(tree exp, Value *&Result) {
  if (!validate_arglist(TREE_OPERAND(exp, 1), VOID_TYPE))
    return false;

  Result = Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                                    Intrinsic::eh_unwind_init));

  return true;
}

bool TreeToLLVM::EmitBuiltinStackRestore(tree exp) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, POINTER_TYPE, VOID_TYPE))
    return false;

  Value *Ptr = Emit(TREE_VALUE(arglist), 0);
  Ptr = BitCastToType(Ptr, Type::getInt8PtrTy(Context));

  Builder.CreateCall(Intrinsic::getDeclaration(TheModule,
                                               Intrinsic::stackrestore), Ptr);
  return true;
}


bool TreeToLLVM::EmitBuiltinAlloca(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist(arglist, INTEGER_TYPE, VOID_TYPE))
    return false;
  Value *Amt = Emit(TREE_VALUE(arglist), 0);
  AllocaInst *AI = Builder.CreateAlloca(Type::getInt8Ty(Context), Amt);
  
  // If this was originally a vla alloca find the alignment and set it
  // on our alloca.
  tree fndecl = get_callee_fndecl(exp);
  unsigned align = DECL_ALIGN(fndecl) ? DECL_ALIGN(fndecl)/8 : 1;
  AI->setAlignment(align);
  Result = AI;

  return true;
}

bool TreeToLLVM::EmitBuiltinExpect(tree exp, const MemRef *DestLoc,
                                   Value *&Result) {
  // Ignore the hint for now, just expand the expr.  This is safe, but not
  // optimal.
  tree arglist = TREE_OPERAND(exp, 1);
  if (arglist == NULL_TREE || TREE_CHAIN(arglist) == NULL_TREE)
    return true;
  Result = Emit(TREE_VALUE(arglist), DestLoc);
  return true;
}

bool TreeToLLVM::EmitBuiltinVAStart(tree exp) {
  tree arglist = TREE_OPERAND(exp, 1);
  tree fntype = TREE_TYPE(current_function_decl);

  if (TYPE_ARG_TYPES(fntype) == 0 ||
      (TREE_VALUE(tree_last(TYPE_ARG_TYPES(fntype))) == void_type_node)) {
    error("`va_start' used in function with fixed args");
    return true;
  }

  tree chain = TREE_CHAIN(arglist);

  // Check for errors.
  if (fold_builtin_next_arg (chain))
    return true;

  Value *ArgVal = Emit(TREE_VALUE(arglist), 0);

  Constant *llvm_va_start_fn = Intrinsic::getDeclaration(TheModule,
                                                         Intrinsic::vastart);
  ArgVal = BitCastToType(ArgVal, Type::getInt8PtrTy(Context));
  Builder.CreateCall(llvm_va_start_fn, ArgVal);
  return true;
}

bool TreeToLLVM::EmitBuiltinVAEnd(tree exp) {
  Value *Arg = Emit(TREE_VALUE(TREE_OPERAND(exp, 1)), 0);
  Arg = BitCastToType(Arg, Type::getInt8PtrTy(Context));
  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::vaend),
                     Arg);
  return true;
}

bool TreeToLLVM::EmitBuiltinVACopy(tree exp) {
  tree Arg1T = TREE_VALUE(TREE_OPERAND(exp, 1));
  tree Arg2T = TREE_VALUE(TREE_CHAIN(TREE_OPERAND(exp, 1)));

  Value *Arg1 = Emit(Arg1T, 0);   // Emit the address of the destination.
  // The second arg of llvm.va_copy is a pointer to a valist.
  Value *Arg2;
  if (!isAggregateTreeType(va_list_type_node)) {
    // Emit it as a value, then store it to a temporary slot.
    Value *V2 = Emit(Arg2T, 0);
    Arg2 = CreateTemporary(V2->getType());
    Builder.CreateStore(V2, Arg2);
  } else {
    // If the target has aggregate valists, then the second argument
    // from GCC is the address of the source valist and we don't
    // need to do anything special.
    Arg2 = Emit(Arg2T, 0);
  }

  static const Type *VPTy = Type::getInt8PtrTy(Context);

  // FIXME: This ignores alignment and volatility of the arguments.
  SmallVector<Value *, 2> Args;
  Args.push_back(BitCastToType(Arg1, VPTy));
  Args.push_back(BitCastToType(Arg2, VPTy));

  Builder.CreateCall(Intrinsic::getDeclaration(TheModule, Intrinsic::vacopy),
                     Args.begin(), Args.end());
  return true;
}

bool TreeToLLVM::EmitBuiltinInitTrampoline(tree exp, Value *&Result) {
  tree arglist = TREE_OPERAND(exp, 1);
  if (!validate_arglist (arglist, POINTER_TYPE, POINTER_TYPE, POINTER_TYPE,
                         VOID_TYPE))
    return false;

  static const Type *VPTy = Type::getInt8PtrTy(Context);

  Value *Tramp = Emit(TREE_VALUE(arglist), 0);
  Tramp = BitCastToType(Tramp, VPTy);

  Value *Func = Emit(TREE_VALUE(TREE_CHAIN(arglist)), 0);
  Func = BitCastToType(Func, VPTy);

  Value *Chain = Emit(TREE_VALUE(TREE_CHAIN(TREE_CHAIN(arglist))), 0);
  Chain = BitCastToType(Chain, VPTy);

  Value *Ops[3] = { Tramp, Func, Chain };

  Function *Intr = Intrinsic::getDeclaration(TheModule,
                                             Intrinsic::init_trampoline);
  Result = Builder.CreateCall(Intr, Ops, Ops+3, "tramp");
  return true;
}

//===----------------------------------------------------------------------===//
//                      ... Complex Math Expressions ...
//===----------------------------------------------------------------------===//

void TreeToLLVM::EmitLoadFromComplex(Value *&Real, Value *&Imag,
                                     MemRef SrcComplex) {
  Value *RealPtr = Builder.CreateStructGEP(SrcComplex.Ptr, 0, "real");
  Real = Builder.CreateLoad(RealPtr, SrcComplex.Volatile, "real");
  cast<LoadInst>(Real)->setAlignment(SrcComplex.getAlignment());

  Value *ImagPtr = Builder.CreateStructGEP(SrcComplex.Ptr, 1, "imag");
  Imag = Builder.CreateLoad(ImagPtr, SrcComplex.Volatile, "imag");
  cast<LoadInst>(Imag)->setAlignment(
    MinAlign(SrcComplex.getAlignment(), TD.getTypeAllocSize(Real->getType()))
  );
}

void TreeToLLVM::EmitStoreToComplex(MemRef DestComplex, Value *Real,
                                    Value *Imag) {
  StoreInst *St;

  Value *RealPtr = Builder.CreateStructGEP(DestComplex.Ptr, 0, "real");
  St = Builder.CreateStore(Real, RealPtr, DestComplex.Volatile);
  St->setAlignment(DestComplex.getAlignment());

  Value *ImagPtr = Builder.CreateStructGEP(DestComplex.Ptr, 1, "imag");
  St = Builder.CreateStore(Imag, ImagPtr, DestComplex.Volatile);
  St->setAlignment(
    MinAlign(DestComplex.getAlignment(), TD.getTypeAllocSize(Real->getType()))
  );
}


void TreeToLLVM::EmitCOMPLEX_EXPR(tree exp, const MemRef *DestLoc) {
  Value *Real = Emit(TREE_OPERAND(exp, 0), 0);
  Value *Imag = Emit(TREE_OPERAND(exp, 1), 0);
  EmitStoreToComplex(*DestLoc, Real, Imag);
}

void TreeToLLVM::EmitCOMPLEX_CST(tree exp, const MemRef *DestLoc) {
  Value *Real = Emit(TREE_REALPART(exp), 0);
  Value *Imag = Emit(TREE_IMAGPART(exp), 0);
  EmitStoreToComplex(*DestLoc, Real, Imag);
}

// EmitComplexBinOp - Note that this operates on binops like ==/!=, which return
// a bool, not a complex value.
Value *TreeToLLVM::EmitComplexBinOp(tree exp, const MemRef *DestLoc) {
  const Type *ComplexTy = ConvertType(TREE_TYPE(TREE_OPERAND(exp, 0)));

  MemRef LHSTmp = CreateTempLoc(ComplexTy);
  MemRef RHSTmp = CreateTempLoc(ComplexTy);
  Emit(TREE_OPERAND(exp, 0), &LHSTmp);
  Emit(TREE_OPERAND(exp, 1), &RHSTmp);

  Value *LHSr, *LHSi;
  EmitLoadFromComplex(LHSr, LHSi, LHSTmp);
  Value *RHSr, *RHSi;
  EmitLoadFromComplex(RHSr, RHSi, RHSTmp);

  Value *DSTr, *DSTi;
  switch (TREE_CODE(exp)) {
  default: TODO(exp);
  case PLUS_EXPR: // (a+ib) + (c+id) = (a+c) + i(b+d)
    if (LHSr->getType()->isFloatingPointTy()) {
      DSTr = Builder.CreateFAdd(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateFAdd(LHSi, RHSi, "tmpi");
    } else {
      DSTr = Builder.CreateAdd(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateAdd(LHSi, RHSi, "tmpi");
    }
    break;
  case MINUS_EXPR: // (a+ib) - (c+id) = (a-c) + i(b-d)
    if (LHSr->getType()->isFloatingPointTy()) {
      DSTr = Builder.CreateFSub(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateFSub(LHSi, RHSi, "tmpi");
    } else {
      DSTr = Builder.CreateSub(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateSub(LHSi, RHSi, "tmpi");
    }
    break;
  case MULT_EXPR: { // (a+ib) * (c+id) = (ac-bd) + i(ad+cb)
    if (LHSr->getType()->isFloatingPointTy()) {
      Value *Tmp1 = Builder.CreateFMul(LHSr, RHSr); // a*c
      Value *Tmp2 = Builder.CreateFMul(LHSi, RHSi); // b*d
      DSTr = Builder.CreateFSub(Tmp1, Tmp2);        // ac-bd

      Value *Tmp3 = Builder.CreateFMul(LHSr, RHSi); // a*d
      Value *Tmp4 = Builder.CreateFMul(RHSr, LHSi); // c*b
      DSTi = Builder.CreateFAdd(Tmp3, Tmp4);        // ad+cb
    } else {
      Value *Tmp1 = Builder.CreateMul(LHSr, RHSr); // a*c
      Value *Tmp2 = Builder.CreateMul(LHSi, RHSi); // b*d
      DSTr = Builder.CreateSub(Tmp1, Tmp2);        // ac-bd

      Value *Tmp3 = Builder.CreateMul(LHSr, RHSi); // a*d
      Value *Tmp4 = Builder.CreateMul(RHSr, LHSi); // c*b
      DSTi = Builder.CreateAdd(Tmp3, Tmp4);        // ad+cb
    }
    break;
  }
  case RDIV_EXPR: { // (a+ib) / (c+id) = ((ac+bd)/(cc+dd)) + i((bc-ad)/(cc+dd))
    // RDIV_EXPR should always be floating point.
    assert (LHSr->getType()->isFloatingPointTy());
    Value *Tmp1 = Builder.CreateFMul(LHSr, RHSr); // a*c
    Value *Tmp2 = Builder.CreateFMul(LHSi, RHSi); // b*d
    Value *Tmp3 = Builder.CreateFAdd(Tmp1, Tmp2); // ac+bd

    Value *Tmp4 = Builder.CreateFMul(RHSr, RHSr); // c*c
    Value *Tmp5 = Builder.CreateFMul(RHSi, RHSi); // d*d
    Value *Tmp6 = Builder.CreateFAdd(Tmp4, Tmp5); // cc+dd
    DSTr = Builder.CreateFDiv(Tmp3, Tmp6);

    Value *Tmp7 = Builder.CreateFMul(LHSi, RHSr); // b*c
    Value *Tmp8 = Builder.CreateFMul(LHSr, RHSi); // a*d
    Value *Tmp9 = Builder.CreateFSub(Tmp7, Tmp8); // bc-ad
    DSTi = Builder.CreateFDiv(Tmp9, Tmp6);
    break;
  }
  case EQ_EXPR:   // (a+ib) == (c+id) = (a == c) & (b == d)
    if (LHSr->getType()->isFloatingPointTy()) {
      DSTr = Builder.CreateFCmpOEQ(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateFCmpOEQ(LHSi, RHSi, "tmpi");
    } else {
      DSTr = Builder.CreateICmpEQ(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateICmpEQ(LHSi, RHSi, "tmpi");
    }
    return Builder.CreateAnd(DSTr, DSTi);
  case NE_EXPR:   // (a+ib) != (c+id) = (a != c) | (b != d)
    if (LHSr->getType()->isFloatingPointTy()) {
      DSTr = Builder.CreateFCmpUNE(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateFCmpUNE(LHSi, RHSi, "tmpi");
    } else {
      DSTr = Builder.CreateICmpNE(LHSr, RHSr, "tmpr");
      DSTi = Builder.CreateICmpNE(LHSi, RHSi, "tmpi");
    }
    return Builder.CreateOr(DSTr, DSTi);
  }

  EmitStoreToComplex(*DestLoc, DSTr, DSTi);
  return 0;
}


//===----------------------------------------------------------------------===//
//                         ... L-Value Expressions ...
//===----------------------------------------------------------------------===//

/// getFieldOffsetInBits - Return the offset (in bits) of a FIELD_DECL in a
/// structure.
static unsigned getFieldOffsetInBits(tree Field) {
  assert(DECL_FIELD_BIT_OFFSET(Field) != 0 && DECL_FIELD_OFFSET(Field) != 0);
  unsigned Result = TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(Field));
  if (TREE_CODE(DECL_FIELD_OFFSET(Field)) == INTEGER_CST)
    Result += TREE_INT_CST_LOW(DECL_FIELD_OFFSET(Field))*8;
  return Result;
}

/// getComponentRefOffsetInBits - Return the offset (in bits) of the field
/// referenced in a COMPONENT_REF exp.
static unsigned getComponentRefOffsetInBits(tree exp) {
  assert(TREE_CODE(exp) == COMPONENT_REF && "not a COMPONENT_REF!");
  tree field = TREE_OPERAND(exp, 1);
  assert(TREE_CODE(field) == FIELD_DECL && "not a FIELD_DECL!");
  tree field_offset = component_ref_field_offset (exp);
  assert(DECL_FIELD_BIT_OFFSET(field) && field_offset);
  unsigned Result = TREE_INT_CST_LOW(DECL_FIELD_BIT_OFFSET(field));
  if (TREE_CODE(field_offset) == INTEGER_CST)
    Result += TREE_INT_CST_LOW(field_offset)*8;
  return Result;
}

Value *TreeToLLVM::EmitFieldAnnotation(Value *FieldPtr, tree FieldDecl) {
  tree AnnotateAttr = lookup_attribute("annotate", DECL_ATTRIBUTES(FieldDecl));

  const Type *SBP = Type::getInt8PtrTy(Context);

  Function *Fn = Intrinsic::getDeclaration(TheModule,
                                           Intrinsic::ptr_annotation,
                                           &SBP, 1);

  // Get file and line number.  FIXME: Should this be for the decl or the
  // use.  Is there a location info for the use?
  Constant *LineNo = ConstantInt::get(Type::getInt32Ty(Context),
                                      DECL_SOURCE_LINE(FieldDecl));
  Constant *File = ConvertMetadataStringToGV(DECL_SOURCE_FILE(FieldDecl));

  File = TheFolder->CreateBitCast(File, SBP);

  // There may be multiple annotate attributes. Pass return of lookup_attr
  //  to successive lookups.
  while (AnnotateAttr) {
    // Each annotate attribute is a tree list.
    // Get value of list which is our linked list of args.
    tree args = TREE_VALUE(AnnotateAttr);

    // Each annotate attribute may have multiple args.
    // Treat each arg as if it were a separate annotate attribute.
    for (tree a = args; a; a = TREE_CHAIN(a)) {
      // Each element of the arg list is a tree list, so get value
      tree val = TREE_VALUE(a);

      // Assert its a string, and then get that string.
      assert(TREE_CODE(val) == STRING_CST &&
             "Annotate attribute arg should always be a string");

      Constant *strGV = TreeConstantToLLVM::EmitLV_STRING_CST(val);

      // We can not use the IRBuilder because it will constant fold away
      // the GEP that is critical to distinguish between an annotate
      // attribute on a whole struct from one on the first element of the
      // struct.
      BitCastInst *CastFieldPtr = new BitCastInst(FieldPtr,  SBP,
                                                  FieldPtr->getName());
      Builder.Insert(CastFieldPtr);

      Value *Ops[4] = {
        CastFieldPtr, BitCastToType(strGV, SBP),
        File,  LineNo
      };

      const Type* FieldPtrType = FieldPtr->getType();
      FieldPtr = Builder.CreateCall(Fn, Ops, Ops+4);
      FieldPtr = BitCastToType(FieldPtr, FieldPtrType);
    }

    // Get next annotate attribute.
    AnnotateAttr = TREE_CHAIN(AnnotateAttr);
    if (AnnotateAttr)
      AnnotateAttr = lookup_attribute("annotate", AnnotateAttr);
  }
  return FieldPtr;
}

LValue TreeToLLVM::EmitLV_ARRAY_REF(tree exp) {
  // The result type is an ElementTy* in the case of an ARRAY_REF, an array
  // of ElementTy in the case of ARRAY_RANGE_REF.

  tree Array = TREE_OPERAND(exp, 0);
  tree ArrayTreeType = TREE_TYPE(Array);
  tree Index = TREE_OPERAND(exp, 1);
  tree IndexType = TREE_TYPE(Index);
  tree ElementType = TREE_TYPE(ArrayTreeType);

  assert((TREE_CODE (ArrayTreeType) == ARRAY_TYPE ||
          TREE_CODE (ArrayTreeType) == POINTER_TYPE ||
          TREE_CODE (ArrayTreeType) == REFERENCE_TYPE ||
          TREE_CODE (ArrayTreeType) == BLOCK_POINTER_TYPE) &&
         "Unknown ARRAY_REF!");

  // As an LLVM extension, we allow ARRAY_REF with a pointer as the first
  // operand.  This construct maps directly to a getelementptr instruction.
  Value *ArrayAddr;
  unsigned ArrayAlign;

  if (TREE_CODE(ArrayTreeType) == ARRAY_TYPE) {
    // First subtract the lower bound, if any, in the type of the index.
    tree LowerBound = array_ref_low_bound(exp);
    if (!integer_zerop(LowerBound))
      Index = fold(build2(MINUS_EXPR, IndexType, Index, LowerBound));

    LValue ArrayAddrLV = EmitLV(Array);
    assert(!ArrayAddrLV.isBitfield() && "Arrays cannot be bitfields!");
    ArrayAddr = ArrayAddrLV.Ptr;
    ArrayAlign = ArrayAddrLV.getAlignment();
  } else {
    ArrayAddr = Emit(Array, 0);
    if (TREE_CODE (ArrayTreeType) == POINTER_TYPE)
      ArrayAlign = getPointerAlignment(Array);
    else
      ArrayAlign = 1;
  }

  Value *IndexVal = Emit(Index, 0);

  const Type *IntPtrTy = getTargetData().getIntPtrType(Context);
  if (TYPE_UNSIGNED(IndexType)) // if the index is unsigned
    // ZExt it to retain its value in the larger type
    IndexVal = CastToUIntType(IndexVal, IntPtrTy);
  else
    // SExt it to retain its value in the larger type
    IndexVal = CastToSIntType(IndexVal, IntPtrTy);

  // If we are indexing over a fixed-size type, just use a GEP.
  if (isSequentialCompatible(ArrayTreeType)) {
    SmallVector<Value*, 2> Idx;
    if (TREE_CODE(ArrayTreeType) == ARRAY_TYPE)
      Idx.push_back(ConstantInt::get(IntPtrTy, 0));
    Idx.push_back(IndexVal);
    Value *Ptr = flag_wrapv ?
      Builder.CreateGEP(ArrayAddr, Idx.begin(), Idx.end()) :
      Builder.CreateInBoundsGEP(ArrayAddr, Idx.begin(), Idx.end());

    const Type *ElementTy = ConvertType(ElementType);
    unsigned Alignment = MinAlign(ArrayAlign, TD.getABITypeAlignment(ElementTy));
    return LValue(BitCastToType(Ptr,
                                ConvertType(TREE_TYPE(exp))->getPointerTo()),
                  Alignment);
  }

  // Otherwise, just do raw, low-level pointer arithmetic.  FIXME: this could be
  // much nicer in cases like:
  //   float foo(int w, float A[][w], int g) { return A[g][0]; }

  ArrayAddr = BitCastToType(ArrayAddr,
                            Type::getInt8PtrTy(Context));
  if (VOID_TYPE_P(TREE_TYPE(ArrayTreeType)))
    return LValue(Builder.CreateGEP(ArrayAddr, IndexVal), 1);

  tree EltSize = array_ref_element_size(exp);
  if (EltSize == 0)  // Force emit an error on invalid case.
    EltSize = size_in_bytes(ElementType);
  
  Value *TypeSize = Emit(EltSize, 0);
  TypeSize = CastToUIntType(TypeSize, IntPtrTy);
  IndexVal = Builder.CreateMul(IndexVal, TypeSize);
  unsigned Alignment = 1;
  if (isa<ConstantInt>(IndexVal))
    Alignment = MinAlign(ArrayAlign,
                         cast<ConstantInt>(IndexVal)->getZExtValue());
  Value *Ptr = flag_wrapv ?
    Builder.CreateGEP(ArrayAddr, IndexVal) :
    Builder.CreateInBoundsGEP(ArrayAddr, IndexVal);
  return LValue(BitCastToType(Ptr, ConvertType(TREE_TYPE(exp))->getPointerTo()),
                Alignment);
}

LValue TreeToLLVM::EmitLV_BIT_FIELD_REF(tree exp) {
  LValue Ptr = EmitLV(TREE_OPERAND(exp, 0));
  assert(!Ptr.isBitfield() && "BIT_FIELD_REF operands cannot be bitfields!");

  unsigned BitStart = (unsigned)TREE_INT_CST_LOW(TREE_OPERAND(exp, 2));
  unsigned BitSize = (unsigned)TREE_INT_CST_LOW(TREE_OPERAND(exp, 1));
  const Type *ValTy = ConvertType(TREE_TYPE(exp));

  unsigned ValueSizeInBits = TD.getTypeSizeInBits(ValTy);
  assert(BitSize <= ValueSizeInBits &&
         "ValTy isn't large enough to hold the value loaded!");

  assert(ValueSizeInBits == TD.getTypeAllocSizeInBits(ValTy) &&
         "FIXME: BIT_FIELD_REF logic is broken for non-round types");

  // BIT_FIELD_REF values can have BitStart values that are quite large.  We
  // know that the thing we are loading is ValueSizeInBits large.  If BitStart
  // is larger than ValueSizeInBits, bump the pointer over to where it should
  // be.
  if (unsigned UnitOffset = BitStart / ValueSizeInBits) {
    // TODO: If Ptr.Ptr is a struct type or something, we can do much better
    // than this.  e.g. check out when compiling unwind-dw2-fde-darwin.c.
    Ptr.Ptr = BitCastToType(Ptr.Ptr, ValTy->getPointerTo());
    Ptr.Ptr = Builder.CreateGEP(Ptr.Ptr,
                            ConstantInt::get(Type::getInt32Ty(Context),
                                             UnitOffset));
    BitStart -= UnitOffset*ValueSizeInBits;
  }

  // If this is referring to the whole field, return the whole thing.
  if (BitStart == 0 && BitSize == ValueSizeInBits) {
    return LValue(BitCastToType(Ptr.Ptr, ValTy->getPointerTo()),
                  Ptr.getAlignment());
  }

  return LValue(BitCastToType(Ptr.Ptr, ValTy->getPointerTo()), 1,
                BitStart, BitSize);
}

LValue TreeToLLVM::EmitLV_COMPONENT_REF(tree exp) {
  LValue StructAddrLV = EmitLV(TREE_OPERAND(exp, 0));
  tree FieldDecl = TREE_OPERAND(exp, 1);
  unsigned LVAlign = StructAddrLV.getAlignment();

  assert((TREE_CODE(DECL_CONTEXT(FieldDecl)) == RECORD_TYPE ||
          TREE_CODE(DECL_CONTEXT(FieldDecl)) == UNION_TYPE  ||
          TREE_CODE(DECL_CONTEXT(FieldDecl)) == QUAL_UNION_TYPE));

  // Ensure that the struct type has been converted, so that the fielddecls
  // are laid out.  Note that we convert to the context of the Field, not to the
  // type of Operand #0, because GCC doesn't always have the field match up with
  // operand #0's type.
  const Type *StructTy = ConvertType(DECL_CONTEXT(FieldDecl));

  assert((!StructAddrLV.isBitfield() ||
          StructAddrLV.BitStart == 0) && "structs cannot be bitfields!");

  StructAddrLV.Ptr = BitCastToType(StructAddrLV.Ptr,
                                   StructTy->getPointerTo());
  const Type *FieldTy = ConvertType(getDeclaredType(FieldDecl));

  // BitStart - This is the actual offset of the field from the start of the
  // struct, in bits.  For bitfields this may be on a non-byte boundary.
  unsigned BitStart = getComponentRefOffsetInBits(exp);
  Value *FieldPtr;
  unsigned ByteOffset = 0;

  tree field_offset = component_ref_field_offset (exp);
  // If this is a normal field at a fixed offset from the start, handle it.
  if (TREE_CODE(field_offset) == INTEGER_CST) {
    unsigned int MemberIndex = GET_LLVM_FIELD_INDEX(FieldDecl);

    // If the LLVM struct has zero field, don't try to index into it, just use
    // the current pointer.
    FieldPtr = StructAddrLV.Ptr;
    if (StructTy->getNumContainedTypes() != 0) {
      assert(MemberIndex < StructTy->getNumContainedTypes() &&
             "Field Idx out of range!");
      FieldPtr = Builder.CreateStructGEP(FieldPtr, MemberIndex);
    }

    // Now that we did an offset from the start of the struct, subtract off
    // the offset from BitStart.
    if (MemberIndex) {
      const StructLayout *SL = TD.getStructLayout(cast<StructType>(StructTy));
      ByteOffset = SL->getElementOffset(MemberIndex);
      BitStart -= ByteOffset * 8;

      // If the base is known to be 8-byte aligned, and we're adding a 4-byte
      // offset, the field is known to be 4-byte aligned.
      LVAlign = MinAlign(LVAlign, ByteOffset);
    }

    // There is debate about whether this is really safe or not, be conservative
    // in the meantime.
#if 0
    // If this field is at a constant offset, if the LLVM pointer really points
    // to it, then we know that the pointer is at least as aligned as the field
    // is required to be.  Try to round up our alignment info.
    if (BitStart == 0 && // llvm pointer points to it.
        !isBitfield(FieldDecl) &&  // bitfield computation might offset pointer.
        DECL_ALIGN(FieldDecl))
      LVAlign = std::max(LVAlign, unsigned(DECL_ALIGN(FieldDecl)) / 8);
#endif

    // If the FIELD_DECL has an annotate attribute on it, emit it.
    if (lookup_attribute("annotate", DECL_ATTRIBUTES(FieldDecl)))
      FieldPtr = EmitFieldAnnotation(FieldPtr, FieldDecl);
  } else {
    Value *Offset = Emit(field_offset, 0);

    // For ObjC2, the offset of the field is loaded from memory (it can
    // change at runtime), and the initial value in memory includes the
    // value that would normally be computed at compile time; we don't
    // want to add this in twice.  The ObjC FE figures out the value we
    // actually should add at compile time (usually 0).
    tree field_bit_offset = objc_v2_bitfield_ivar_bitpos(exp);
    if (field_bit_offset) {
      BitStart = (unsigned)getINTEGER_CSTVal(field_bit_offset);
    }
    // Here BitStart gives the offset of the field in bits from field_offset.
    // Incorporate as much of it as possible into the pointer computation.
    ByteOffset = BitStart/8;
    if (ByteOffset > 0) {
      Offset = Builder.CreateAdd(Offset,
        ConstantInt::get(Offset->getType(), ByteOffset));
      BitStart -= ByteOffset*8;
      // If the base is known to be 8-byte aligned, and we're adding a 4-byte
      // offset, the field is known to be 4-byte aligned.
      LVAlign = MinAlign(LVAlign, ByteOffset);
    }

    // Since we're using GCC's offset, we're obliged to use GCC's
    // "shrink-wrapped" type for the reference, lest we reference
    // memory outside of this struct.
    FieldTy = ConvertType(TREE_TYPE(FieldDecl));

    Value *Ptr = CastToType(Instruction::PtrToInt, StructAddrLV.Ptr,
                            Offset->getType());
    Ptr = Builder.CreateAdd(Ptr, Offset);
    FieldPtr = CastToType(Instruction::IntToPtr, Ptr,
                          FieldTy->getPointerTo());
  }

  if (isBitfield(FieldDecl)) {
    // If this is a bitfield, the declared type must be an integral type.
    assert(FieldTy->isIntegerTy() && "Invalid bitfield");

    assert(DECL_SIZE(FieldDecl) &&
           TREE_CODE(DECL_SIZE(FieldDecl)) == INTEGER_CST &&
           "Variable sized bitfield?");
    unsigned BitfieldSize = TREE_INT_CST_LOW(DECL_SIZE(FieldDecl));

    const Type *LLVMFieldTy =
      cast<PointerType>(FieldPtr->getType())->getElementType();

    // 'piecemeal' will be true if the fetch-type we wish to use will
    // reference memory outside of the struct.  (That's not good.)
    // Note this check is too simplistic: if this field is part of a
    // struct within another struct, it's probably O.K. to fetch data
    // outside of the bitfield, unless the inadvertently-referenced
    // data is volatile.  But we're too lazy to check for that; choose
    // the slow-but-conservative-and-always-correct path.
    tree gccContext = DECL_FIELD_CONTEXT(FieldDecl);
    tree gccSize = TYPE_SIZE(gccContext);
    // If the size isn't constant, assume the worst (a one byte
    // struct).
    unsigned int gccStructSize = TREE_CODE(gccSize) == INTEGER_CST ?
      TREE_INT_CST_LOW(gccSize) : 1;
    // piecemeal == true means we fetch the bitfield in pieces and
    // reassemble in a register.
    bool piecemeal = false;

    if (ByteOffset * 8 + TD.getTypeAllocSizeInBits(FieldTy) > gccStructSize) {
      // If the LLVM notion of the type would reference memory outside
      // of the enclosing struct, punt and fetch the bitfield
      // piecemeal.  Round the bitfield size up to the nearest byte;
      // use existing size if it's already a multiple of 8:
      unsigned int byteAlignedBitfieldSize = (BitfieldSize % 8) ?
        ((BitfieldSize / 8) + 1) * 8 : BitfieldSize;
      FieldTy = Type::getIntNTy(Context, byteAlignedBitfieldSize);
      piecemeal = true;
    } else if (LLVMFieldTy->isIntegerTy() &&
             LLVMFieldTy->getPrimitiveSizeInBits() >= BitStart + BitfieldSize &&
             LLVMFieldTy->getPrimitiveSizeInBits() ==
             TD.getTypeAllocSizeInBits(LLVMFieldTy))
      // If the LLVM notion of the field type contains the entire bitfield being
      // accessed, use the LLVM type.  This avoids pointer casts and other bad
      // things that are difficult to clean up later.  This occurs in cases like
      // "struct X{ unsigned long long x:50; unsigned y:2; }" when accessing y.
      // We want to access the field as a ulong, not as a uint with an offset.
      FieldTy = LLVMFieldTy;
    else
      // If the field result type T is a bool or some other curiously sized
      // integer type, then not all bits may be accessible by advancing a T*
      // and loading through it.  For example, if the result type is i1 then
      // only the first bit in each byte would be loaded.  Even if T is byte
      // sized like an i24 there may be trouble: incrementing a T* will move
      // the position by 32 bits not 24, leaving the upper 8 of those 32 bits
      // inaccessible.  Avoid this by rounding up the size appropriately.
      FieldTy = IntegerType::get(Context, TD.getTypeAllocSizeInBits(FieldTy));

    assert((piecemeal || (FieldTy->getPrimitiveSizeInBits() ==
                          TD.getTypeAllocSizeInBits(FieldTy))) &&
           "Field type not sequential!");

    // If this is a bitfield, the field may span multiple fields in the LLVM
    // type.  As such, cast the pointer to be a pointer to the declared type.
    FieldPtr = BitCastToType(FieldPtr, FieldTy->getPointerTo());

    unsigned LLVMValueBitSize = FieldTy->getPrimitiveSizeInBits();
    // Finally, because bitfields can span LLVM fields, and because the start
    // of the first LLVM field (where FieldPtr currently points) may be up to
    // 63 bits away from the start of the bitfield), it is possible that
    // *FieldPtr doesn't contain any of the bits for this bitfield. If needed,
    // adjust FieldPtr so that it is close enough to the bitfield that
    // *FieldPtr contains the first needed bit.  Be careful to make sure that
    // the pointer remains appropriately aligned.
    if (BitStart >= LLVMValueBitSize) {
      // In this case, we know that the alignment of the field is less than
      // the size of the field.  To get the pointer close enough, add some
      // number of alignment units to the pointer.
      unsigned ByteAlignment = TD.getABITypeAlignment(FieldTy);
      // It is possible that an individual field is Packed. This information is
      // not reflected in FieldTy. Check DECL_PACKED here.
      if (DECL_PACKED(FieldDecl))
        ByteAlignment = 1;
      assert(ByteAlignment*8 <= LLVMValueBitSize && "Unknown overlap case!");
      unsigned NumAlignmentUnits = BitStart/(ByteAlignment*8);
      assert(NumAlignmentUnits && "Not adjusting pointer?");

      // Compute the byte offset, and add it to the pointer.
      unsigned ByteOffset = NumAlignmentUnits*ByteAlignment;
      LVAlign = MinAlign(LVAlign, ByteOffset);

      Constant *Offset = ConstantInt::get(TD.getIntPtrType(Context), ByteOffset);
      FieldPtr = CastToType(Instruction::PtrToInt, FieldPtr,
                            Offset->getType());
      FieldPtr = Builder.CreateAdd(FieldPtr, Offset);
      FieldPtr = CastToType(Instruction::IntToPtr, FieldPtr,
                            FieldTy->getPointerTo());

      // Adjust bitstart to account for the pointer movement.
      BitStart -= ByteOffset*8;

      // Check that this worked.  Note that the bitfield may extend beyond
      // the end of *FieldPtr, for example because BitfieldSize is the same
      // as LLVMValueBitSize but BitStart > 0.
      assert(BitStart < LLVMValueBitSize &&
             BitStart+BitfieldSize < 2*LLVMValueBitSize &&
             "Couldn't get bitfield into value!");
    }

    // Okay, everything is good.  Return this as a bitfield if we can't
    // return it as a normal l-value. (e.g. "struct X { int X : 32 };" ).
    if (BitfieldSize != LLVMValueBitSize || BitStart != 0)
      return LValue(FieldPtr, LVAlign, BitStart, BitfieldSize);
    
  } else {
    // Make sure we return a pointer to the right type.
    const Type *EltTy = ConvertType(TREE_TYPE(exp));
    FieldPtr = BitCastToType(FieldPtr, EltTy->getPointerTo());
  }

  assert(BitStart == 0 &&
         "It's a bitfield reference or we didn't get to the field!");
  return LValue(FieldPtr, LVAlign);
}

LValue TreeToLLVM::EmitLV_DECL(tree exp) {
  if (TREE_CODE(exp) == PARM_DECL || TREE_CODE(exp) == VAR_DECL ||
      TREE_CODE(exp) == CONST_DECL) {
    // If a static var's type was incomplete when the decl was written,
    // but the type is complete now, lay out the decl now.
    if (DECL_SIZE(exp) == 0 && COMPLETE_OR_UNBOUND_ARRAY_TYPE_P(TREE_TYPE(exp))
        && (TREE_STATIC(exp) || DECL_EXTERNAL(exp))) {
      layout_decl(exp, 0);

#if 0
      // This mirrors code in layout_decl for munging the RTL.  Here we actually
      // emit a NEW declaration for the global variable, now that it has been
      // laid out.  We then tell the compiler to "forward" any uses of the old
      // global to this new one.
      if (Value *Val = DECL_LLVM_IF_SET(exp)) {
        //fprintf(stderr, "***\n*** SHOULD HANDLE GLOBAL VARIABLES!\n***\n");
        //assert(0 && "Reimplement this with replace all uses!");
        SET_DECL_LLVM(exp, 0);
        // Create a new global variable declaration
        llvm_assemble_external(exp);
        V2GV(Val)->ForwardedGlobal = V2GV(DECL_LLVM(exp));
      }
#endif
    }
  }

  assert(!isGimpleTemporary(exp) &&
         "Cannot use a gimple temporary as an l-value");

  Value *Decl = DECL_LLVM(exp);
  if (Decl == 0) {
    if (errorcount || sorrycount) {
      const Type *Ty = ConvertType(TREE_TYPE(exp));
      const PointerType *PTy = Ty->getPointerTo();
      LValue LV(ConstantPointerNull::get(PTy), 1);
      return LV;
    }
    assert(0 && "INTERNAL ERROR: Referencing decl that hasn't been laid out");
    abort();
  }

  // Ensure variable marked as used even if it doesn't go through a parser.  If
  // it hasn't been used yet, write out an external definition.
  if (!TREE_USED(exp)) {
    assemble_external(exp);
    TREE_USED(exp) = 1;
    Decl = DECL_LLVM(exp);
  }

  if (GlobalValue *GV = dyn_cast<GlobalValue>(Decl)) {
    // If this is an aggregate, emit it to LLVM now.  GCC happens to
    // get this case right by forcing the initializer into memory.
    if (TREE_CODE(exp) == CONST_DECL || TREE_CODE(exp) == VAR_DECL) {
      if ((DECL_INITIAL(exp) || !TREE_PUBLIC(exp)) && !DECL_EXTERNAL(exp) &&
          GV->isDeclaration() &&
          !BOGUS_CTOR(exp)) {
        emit_global_to_llvm(exp);
        Decl = DECL_LLVM(exp);     // Decl could have change if it changed type.
      }
    } else {
      // Otherwise, inform cgraph that we used the global.
      mark_decl_referenced(exp);
      if (tree ID = DECL_ASSEMBLER_NAME(exp))
        mark_referenced(ID);
    }
  }

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  // If we have "extern void foo", make the global have type {} instead of
  // type void.
  if (Ty->isVoidTy()) Ty = StructType::get(Context);
  if (LLVM_IS_DECL_MMX_REGISTER(exp))
    Ty = Type::getX86_MMXTy(Context);
  const PointerType *PTy = Ty->getPointerTo();
  unsigned Alignment = Ty->isSized() ? TD.getABITypeAlignment(Ty) : 1;
  if (DECL_ALIGN(exp)) {
    if (DECL_USER_ALIGN(exp) || 8 * Alignment < (unsigned)DECL_ALIGN(exp))
      Alignment = DECL_ALIGN(exp) / 8;
  }

  // Function arguments may have extra alignment attributes.
  if (Argument *Arg = dyn_cast<Argument>(Decl)) {
    unsigned pa = Arg->getParent()->getParamAlignment(Arg->getArgNo()+1);
    if (pa && pa < Alignment)
      Alignment = pa;
  }

  return LValue(BitCastToType(Decl, PTy), Alignment);
}

LValue TreeToLLVM::EmitLV_EXC_PTR_EXPR(tree exp) {
  CreateExceptionValues();
  // Cast the address pointer to the expected type.
  unsigned Alignment = TD.getABITypeAlignment(cast<PointerType>(ExceptionValue->
                                                  getType())->getElementType());
  return LValue(BitCastToType(ExceptionValue,
                              ConvertType(TREE_TYPE(exp))->getPointerTo()),
                Alignment);
}

LValue TreeToLLVM::EmitLV_FILTER_EXPR(tree exp) {
  CreateExceptionValues();
  unsigned Alignment =
    TD.getABITypeAlignment(cast<PointerType>(ExceptionSelectorValue->
                                             getType())->getElementType());
  return LValue(ExceptionSelectorValue, Alignment);
}

LValue TreeToLLVM::EmitLV_INDIRECT_REF(tree exp) {
  // The lvalue is just the address.
  LValue LV = LValue(Emit(TREE_OPERAND(exp, 0), 0), expr_align(exp) / 8);
  // Correct for implicit type conversion: INDIRECT_REF can be applied to a
  // void*, resulting in a non-void type.
  LV.Ptr = BitCastToType(LV.Ptr, ConvertType(TREE_TYPE(exp))->getPointerTo());
  return LV;
}

LValue TreeToLLVM::EmitLV_VIEW_CONVERT_EXPR(tree exp) {
  tree Op = TREE_OPERAND(exp, 0);

  if (isAggregateTreeType(TREE_TYPE(Op))) {
    // If the input is an aggregate, the address is the address of the operand.
    LValue LV = EmitLV(Op);
    // The type is the type of the expression.
    LV.Ptr = BitCastToType(LV.Ptr, ConvertType(TREE_TYPE(exp))->getPointerTo());
    return LV;
  } else {
    // If the input is a scalar, emit to a temporary.
    Value *Dest = CreateTemporary(ConvertType(TREE_TYPE(Op)));
    Builder.CreateStore(Emit(Op, 0), Dest);
    // The type is the type of the expression.
    Dest = BitCastToType(Dest, ConvertType(TREE_TYPE(exp))->getPointerTo());
    return LValue(Dest, 1);
  }
}

LValue TreeToLLVM::EmitLV_WITH_SIZE_EXPR(tree exp) {
  // The address is the address of the operand.
  return EmitLV(TREE_OPERAND(exp, 0));
}

LValue TreeToLLVM::EmitLV_XXXXPART_EXPR(tree exp, unsigned Idx) {
  LValue Ptr = EmitLV(TREE_OPERAND(exp, 0));
  assert(!Ptr.isBitfield() &&
         "REALPART_EXPR / IMAGPART_EXPR operands cannot be bitfields!");
  unsigned Alignment;
  if (Idx == 0)
    // REALPART alignment is same as the complex operand.
    Alignment = Ptr.getAlignment();
  else
    // IMAGPART alignment = MinAlign(Ptr.Alignment, sizeof field);
    Alignment = MinAlign(Ptr.getAlignment(),
                         TD.getTypeAllocSize(Ptr.Ptr->getType()));
  return LValue(Builder.CreateStructGEP(Ptr.Ptr, Idx), Alignment);
}

Constant *TreeToLLVM::EmitLV_LABEL_DECL(tree exp) {
  return BlockAddress::get(Fn, getLabelDeclBlock(exp));
}

//===----------------------------------------------------------------------===//
//                       ... Constant Expressions ...
//===----------------------------------------------------------------------===//

/// EmitCONSTRUCTOR - emit the constructor into the location specified by
/// DestLoc.
Value *TreeToLLVM::EmitCONSTRUCTOR(tree exp, const MemRef *DestLoc) {
  tree type = TREE_TYPE(exp);
  const Type *Ty = ConvertType(type);
  if (const VectorType *PTy = dyn_cast<VectorType>(Ty)) {
    assert(DestLoc == 0 && "Dest location for packed value?");

    std::vector<Value *> BuildVecOps;

    // Insert zero initializers for any uninitialized values.
    Constant *Zero = Constant::getNullValue(PTy->getElementType());
    BuildVecOps.resize(cast<VectorType>(Ty)->getNumElements(), Zero);

    // Insert all of the elements here.
    unsigned HOST_WIDE_INT ix;
    tree purpose, value;
    FOR_EACH_CONSTRUCTOR_ELT (CONSTRUCTOR_ELTS (exp), ix, purpose, value) {
      if (!purpose) continue;  // Not actually initialized?

      unsigned FieldNo = TREE_INT_CST_LOW(purpose);

      // Update the element.
      if (FieldNo < BuildVecOps.size())
        BuildVecOps[FieldNo] = Emit(value, 0);
    }

    return BuildVector(BuildVecOps);
  }

  assert(!Ty->isSingleValueType() && "Constructor for scalar type??");

  // Start out with the value zero'd out.
  EmitAggregateZero(*DestLoc, type);

  VEC(constructor_elt, gc) *elt = CONSTRUCTOR_ELTS(exp);
  switch (TREE_CODE(TREE_TYPE(exp))) {
  case ARRAY_TYPE:
  case RECORD_TYPE:
  default:
    if (elt && VEC_length(constructor_elt, elt)) {
      // We don't handle elements yet.

      TODO(exp);
    }
    return 0;
  case QUAL_UNION_TYPE:
  case UNION_TYPE:
    // Store each element of the constructor into the corresponding field of
    // DEST.
    if (!elt || VEC_empty(constructor_elt, elt)) return 0;  // no elements
    assert(VEC_length(constructor_elt, elt) == 1
           && "Union CONSTRUCTOR should have one element!");
    tree tree_purpose = VEC_index(constructor_elt, elt, 0)->index;
    tree tree_value   = VEC_index(constructor_elt, elt, 0)->value;
    if (!tree_purpose)
      return 0;  // Not actually initialized?

    if (!ConvertType(TREE_TYPE(tree_purpose))->isSingleValueType()) {
      Value *V = Emit(tree_value, DestLoc);
      assert(V == 0 && "Aggregate value returned in a register?");
    } else {
      // Scalar value.  Evaluate to a register, then do the store.
      Value *V = Emit(tree_value, 0);
      Value *Ptr = BitCastToType(DestLoc->Ptr, V->getType()->getPointerTo());
      StoreInst *St = Builder.CreateStore(V, Ptr, DestLoc->Volatile);
      St->setAlignment(DestLoc->getAlignment());
    }
    break;
  }
  return 0;
}

Constant *TreeConstantToLLVM::Convert(tree exp) {
  // Some front-ends use constants other than the standard language-independent
  // varieties, but which may still be output directly.  Give the front-end a
  // chance to convert EXP to a language-independent representation.
  exp = lang_hooks.expand_constant (exp);

  assert((TREE_CONSTANT(exp) || TREE_CODE(exp) == STRING_CST) &&
         "Isn't a constant!");
  switch (TREE_CODE(exp)) {
  case FDESC_EXPR:    // Needed on itanium
  default:
    debug_tree(exp);
    assert(0 && "Unknown constant to convert!");
    abort();
  case INTEGER_CST:   return ConvertINTEGER_CST(exp);
  case REAL_CST:      return ConvertREAL_CST(exp);
  case VECTOR_CST:    return ConvertVECTOR_CST(exp);
  case STRING_CST:    return ConvertSTRING_CST(exp);
  case COMPLEX_CST:   return ConvertCOMPLEX_CST(exp);
  case NOP_EXPR:      return ConvertNOP_EXPR(exp);
  case CONVERT_EXPR:  return ConvertCONVERT_EXPR(exp);
  case PLUS_EXPR:
  case MINUS_EXPR:    return ConvertBinOp_CST(exp);
  case CONSTRUCTOR:   return ConvertCONSTRUCTOR(exp);
  case VIEW_CONVERT_EXPR: return Convert(TREE_OPERAND(exp, 0));
  case ADDR_EXPR:
    return TheFolder->CreateBitCast(EmitLV(TREE_OPERAND(exp, 0)),
                                    ConvertType(TREE_TYPE(exp)));
  }
}

Constant *TreeConstantToLLVM::ConvertINTEGER_CST(tree exp) {
  const Type *Ty = ConvertType(TREE_TYPE(exp));

  // Handle i128 specially.
  if (const IntegerType *IT = dyn_cast<IntegerType>(Ty)) {
    if (IT->getBitWidth() == 128) {
      // GCC only supports i128 on 64-bit systems.
      assert(HOST_BITS_PER_WIDE_INT == 64 &&
             "i128 only supported on 64-bit system");
      uint64_t Bits[] = { TREE_INT_CST_LOW(exp), TREE_INT_CST_HIGH(exp) };
      return ConstantInt::get(Context, APInt(128, 2, Bits));
    }
  }

  // Build the value as a ulong constant, then constant fold it to the right
  // type.  This handles overflow and other things appropriately.
  uint64_t IntValue = getINTEGER_CSTVal(exp);
  ConstantInt *C = ConstantInt::get(Type::getInt64Ty(Context), IntValue);
  // The destination type can be a pointer, integer or floating point
  // so we need a generalized cast here
  Instruction::CastOps opcode = CastInst::getCastOpcode(C, false, Ty,
      !TYPE_UNSIGNED(TREE_TYPE(exp)));
  return TheFolder->CreateCast(opcode, C, Ty);
}

Constant *TreeConstantToLLVM::ConvertREAL_CST(tree exp) {
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  assert((Ty->isFloatingPointTy() ||
	  Ty->isIntegerTy(16)) && "Integer REAL_CST?");
  long RealArr[2];
  union {
    int UArr[2];
    double V;
  };
  if (Ty->isFloatTy() || Ty->isDoubleTy()) {
    REAL_VALUE_TO_TARGET_DOUBLE(TREE_REAL_CST(exp), RealArr);

    // Here's how this works:
    // REAL_VALUE_TO_TARGET_DOUBLE() will generate the floating point number
    // as an array of integers in the target's representation.  Each integer
    // in the array will hold 32 bits of the result REGARDLESS OF THE HOST'S
    // INTEGER SIZE.
    //
    // This, then, makes the conversion pretty simple.  The tricky part is
    // getting the byte ordering correct and make sure you don't print any
    // more than 32 bits per integer on platforms with ints > 32 bits.
    //
    // We want to switch the words of UArr if host and target endianness
    // do not match.  FLOAT_WORDS_BIG_ENDIAN describes the target endianness.
    // The host's used to be available in HOST_WORDS_BIG_ENDIAN, but the gcc
    // maintainers removed this in a fit of cleanliness between 4.0
    // and 4.2. llvm::sys has a substitute.

    UArr[0] = RealArr[0];   // Long -> int convert
    UArr[1] = RealArr[1];

    if (llvm::sys::isBigEndianHost() != FLOAT_WORDS_BIG_ENDIAN)
      std::swap(UArr[0], UArr[1]);

    return
      ConstantFP::get(Context, Ty->isFloatTy() ?
                      APFloat((float)V) : APFloat(V));
  } else if (Ty->isX86_FP80Ty()) {
    long RealArr[4];
    uint64_t UArr[2];
    REAL_VALUE_TO_TARGET_LONG_DOUBLE(TREE_REAL_CST(exp), RealArr);
    UArr[0] = ((uint64_t)((uint32_t)RealArr[0])) |
              ((uint64_t)((uint32_t)RealArr[1]) << 32);
    UArr[1] = (uint16_t)RealArr[2];
    return ConstantFP::get(Context, APFloat(APInt(80, 2, UArr)));
  } else if (Ty->isPPC_FP128Ty() || Ty->isFP128Ty()) {
    long RealArr[4];
    uint64_t UArr[2];
    REAL_VALUE_TO_TARGET_LONG_DOUBLE(TREE_REAL_CST(exp), RealArr);

    UArr[0] = ((uint64_t)((uint32_t)RealArr[0]) << 32) |
              ((uint64_t)((uint32_t)RealArr[1]));
    UArr[1] = ((uint64_t)((uint32_t)RealArr[2]) << 32) |
              ((uint64_t)((uint32_t)RealArr[3]));
    return ConstantFP::get(Context,
                           APFloat(APInt(128, 2, UArr),
                                   /*isIEEE*/ Ty->isFP128Ty()));
  } else if (Ty->isIntegerTy(16)) {
    long RealVal;
    REAL_VALUE_TO_TARGET_HALF(TREE_REAL_CST(exp), RealVal);
    return ConstantInt::get(Context, APInt(16, RealVal));
  }
  assert(0 && "Floating point type not handled yet");
  return 0;   // outwit compiler warning
}

Constant *TreeConstantToLLVM::ConvertVECTOR_CST(tree exp) {
  if (!TREE_VECTOR_CST_ELTS(exp))
    return Constant::getNullValue(ConvertType(TREE_TYPE(exp)));

  std::vector<Constant*> Elts;
  for (tree elt = TREE_VECTOR_CST_ELTS(exp); elt; elt = TREE_CHAIN(elt))
    Elts.push_back(Convert(TREE_VALUE(elt)));

  // The vector should be zero filled if insufficient elements are provided.
  if (Elts.size() < TYPE_VECTOR_SUBPARTS(TREE_TYPE(exp))) {
    tree EltType = TREE_TYPE(TREE_TYPE(exp));
    Constant *Zero = Constant::getNullValue(ConvertType(EltType));
    while (Elts.size() < TYPE_VECTOR_SUBPARTS(TREE_TYPE(exp)))
      Elts.push_back(Zero);
  }

  return ConstantVector::get(Elts);
}

Constant *TreeConstantToLLVM::ConvertSTRING_CST(tree exp) {
  const ArrayType *StrTy = cast<ArrayType>(ConvertType(TREE_TYPE(exp)));
  const Type *ElTy = StrTy->getElementType();

  unsigned Len = (unsigned)TREE_STRING_LENGTH(exp);

  std::vector<Constant*> Elts;
  if (ElTy->isIntegerTy(8)) {
    const unsigned char *InStr =(const unsigned char *)TREE_STRING_POINTER(exp);
    for (unsigned i = 0; i != Len; ++i)
      Elts.push_back(ConstantInt::get(Type::getInt8Ty(Context), InStr[i]));
  } else if (ElTy->isIntegerTy(16)) {
    assert((Len&1) == 0 &&
           "Length in bytes should be a multiple of element size");
    const uint16_t *InStr =
      (const unsigned short *)TREE_STRING_POINTER(exp);
    for (unsigned i = 0; i != Len/2; ++i) {
      // gcc has constructed the initializer elements in the target endianness,
      // but we're going to treat them as ordinary shorts from here, with
      // host endianness.  Adjust if necessary.
      if (llvm::sys::isBigEndianHost() == BYTES_BIG_ENDIAN)
        Elts.push_back(ConstantInt::get(Type::getInt16Ty(Context), InStr[i]));
      else
        Elts.push_back(ConstantInt::get(Type::getInt16Ty(Context),
                                        ByteSwap_16(InStr[i])));
    }
  } else if (ElTy->isIntegerTy(32)) {
    assert((Len&3) == 0 &&
           "Length in bytes should be a multiple of element size");
    const uint32_t *InStr = (const uint32_t *)TREE_STRING_POINTER(exp);
    for (unsigned i = 0; i != Len/4; ++i) {
      // gcc has constructed the initializer elements in the target endianness,
      // but we're going to treat them as ordinary ints from here, with
      // host endianness.  Adjust if necessary.
      if (llvm::sys::isBigEndianHost() == BYTES_BIG_ENDIAN)
        Elts.push_back(ConstantInt::get(Type::getInt32Ty(Context), InStr[i]));
      else
        Elts.push_back(ConstantInt::get(Type::getInt32Ty(Context),
                                        ByteSwap_32(InStr[i])));
    }
  } else {
    assert(0 && "Unknown character type!");
  }

  unsigned LenInElts = Len /
          TREE_INT_CST_LOW(TYPE_SIZE_UNIT(TREE_TYPE(TREE_TYPE(exp))));
  unsigned ConstantSize = StrTy->getNumElements();

  if (LenInElts != ConstantSize) {
    // If this is a variable sized array type, set the length to LenInElts.
    if (ConstantSize == 0) {
      tree Domain = TYPE_DOMAIN(TREE_TYPE(exp));
      if (!Domain || !TYPE_MAX_VALUE(Domain)) {
        ConstantSize = LenInElts;
        StrTy = ArrayType::get(ElTy, LenInElts);
      }
    }

    if (ConstantSize < LenInElts) {
      // Only some chars are being used, truncate the string: char X[2] = "foo";
      Elts.resize(ConstantSize);
    } else {
      // Fill the end of the string with nulls.
      Constant *C = Constant::getNullValue(ElTy);
      for (; LenInElts != ConstantSize; ++LenInElts)
        Elts.push_back(C);
    }
  }
  return ConstantArray::get(StrTy, Elts);
}

Constant *TreeConstantToLLVM::ConvertCOMPLEX_CST(tree exp) {
  std::vector<Constant*> Elts;
  Elts.push_back(Convert(TREE_REALPART(exp)));
  Elts.push_back(Convert(TREE_IMAGPART(exp)));
  return ConstantStruct::get(Context, Elts, false);
}

Constant *TreeConstantToLLVM::ConvertNOP_EXPR(tree exp) {
  Constant *Elt = Convert(TREE_OPERAND(exp, 0));
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  bool EltIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 0)));
  bool TyIsSigned = !TYPE_UNSIGNED(TREE_TYPE(exp));

  // If this is a structure-to-structure cast, just return the uncasted value.
  if (!Elt->getType()->isSingleValueType() || !Ty->isSingleValueType())
    return Elt;

  // Elt and Ty can be integer, float or pointer here: need generalized cast
  Instruction::CastOps opcode = CastInst::getCastOpcode(Elt, EltIsSigned,
                                                        Ty, TyIsSigned);
  return TheFolder->CreateCast(opcode, Elt, Ty);
}

Constant *TreeConstantToLLVM::ConvertCONVERT_EXPR(tree exp) {
  Constant *Elt = Convert(TREE_OPERAND(exp, 0));
  bool EltIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp, 0)));
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  bool TyIsSigned = !TYPE_UNSIGNED(TREE_TYPE(exp));
  Instruction::CastOps opcode = CastInst::getCastOpcode(Elt, EltIsSigned, Ty,
                                                        TyIsSigned);
  return TheFolder->CreateCast(opcode, Elt, Ty);
}

Constant *TreeConstantToLLVM::ConvertBinOp_CST(tree exp) {
  Constant *LHS = Convert(TREE_OPERAND(exp, 0));
  bool LHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp,0)));
  Constant *RHS = Convert(TREE_OPERAND(exp, 1));
  bool RHSIsSigned = !TYPE_UNSIGNED(TREE_TYPE(TREE_OPERAND(exp,1)));
  Instruction::CastOps opcode;
  if (LHS->getType()->isPointerTy()) {
    const Type *IntPtrTy = getTargetData().getIntPtrType(Context);
    opcode = CastInst::getCastOpcode(LHS, LHSIsSigned, IntPtrTy, false);
    LHS = TheFolder->CreateCast(opcode, LHS, IntPtrTy);
    opcode = CastInst::getCastOpcode(RHS, RHSIsSigned, IntPtrTy, false);
    RHS = TheFolder->CreateCast(opcode, RHS, IntPtrTy);
  }

  Constant *Result;
  switch (TREE_CODE(exp)) {
  default: assert(0 && "Unexpected case!");
  case PLUS_EXPR:   Result = TheFolder->CreateAdd(LHS, RHS); break;
  case MINUS_EXPR:  Result = TheFolder->CreateSub(LHS, RHS); break;
  }

  const Type *Ty = ConvertType(TREE_TYPE(exp));
  bool TyIsSigned = !TYPE_UNSIGNED(TREE_TYPE(exp));
  opcode = CastInst::getCastOpcode(Result, LHSIsSigned, Ty, TyIsSigned);
  return TheFolder->CreateCast(opcode, Result, Ty);
}

Constant *TreeConstantToLLVM::ConvertCONSTRUCTOR(tree exp) {
  // Please note, that we can have empty ctor, even if array is non-trivial (has
  // nonzero number of entries). This situation is typical for static ctors,
  // when array is filled during program initialization.
  if (CONSTRUCTOR_ELTS(exp) == 0 ||
      VEC_length(constructor_elt, CONSTRUCTOR_ELTS(exp)) == 0)  // All zeros?
    return Constant::getNullValue(ConvertType(TREE_TYPE(exp)));

  switch (TREE_CODE(TREE_TYPE(exp))) {
  default:
    debug_tree(exp);
    assert(0 && "Unknown ctor!");
  case VECTOR_TYPE:
  case ARRAY_TYPE:  return ConvertArrayCONSTRUCTOR(exp);
  case RECORD_TYPE: return ConvertRecordCONSTRUCTOR(exp);
  case QUAL_UNION_TYPE:
  case UNION_TYPE:  return ConvertUnionCONSTRUCTOR(exp);
  }
}

Constant *TreeConstantToLLVM::ConvertArrayCONSTRUCTOR(tree exp) {
  // Vectors are like arrays, but the domain is stored via an array
  // type indirectly.

  // If we have a lower bound for the range of the type, get it.
  tree InitType = TREE_TYPE(exp);
  tree min_element = size_zero_node;
  std::vector<Constant*> ResultElts;

  if (TREE_CODE(InitType) == VECTOR_TYPE) {
    ResultElts.resize(TYPE_VECTOR_SUBPARTS(InitType));
  } else {
    assert(TREE_CODE(InitType) == ARRAY_TYPE && "Unknown type for init");
    tree Domain = TYPE_DOMAIN(InitType);
    if (Domain && TYPE_MIN_VALUE(Domain))
      min_element = fold_convert(sizetype, TYPE_MIN_VALUE(Domain));

    if (Domain && TYPE_MAX_VALUE(Domain)) {
      tree max_element = fold_convert(sizetype, TYPE_MAX_VALUE(Domain));
      tree size = size_binop (MINUS_EXPR, max_element, min_element);
      size = size_binop (PLUS_EXPR, size, size_one_node);

      if (host_integerp(size, 1))
        ResultElts.resize(tree_low_cst(size, 1));
    }
  }

  unsigned NextFieldToFill = 0;
  unsigned HOST_WIDE_INT ix;
  tree elt_index, elt_value;
  Constant *SomeVal = 0;
  FOR_EACH_CONSTRUCTOR_ELT (CONSTRUCTOR_ELTS (exp), ix, elt_index, elt_value) {
    // Find and decode the constructor's value.
    Constant *Val = Convert(elt_value);
    SomeVal = Val;

    // Get the index position of the element within the array.  Note that this
    // can be NULL_TREE, which means that it belongs in the next available slot.
    tree index = elt_index;

    // The first and last field to fill in, inclusive.
    unsigned FieldOffset, FieldLastOffset;
    if (index && TREE_CODE(index) == RANGE_EXPR) {
      tree first = fold_convert (sizetype, TREE_OPERAND(index, 0));
      tree last  = fold_convert (sizetype, TREE_OPERAND(index, 1));

      first = size_binop (MINUS_EXPR, first, min_element);
      last  = size_binop (MINUS_EXPR, last, min_element);

      assert(host_integerp(first, 1) && host_integerp(last, 1) &&
             "Unknown range_expr!");
      FieldOffset     = tree_low_cst(first, 1);
      FieldLastOffset = tree_low_cst(last, 1);
    } else if (index) {
      index = size_binop (MINUS_EXPR, fold_convert (sizetype, index),
                          min_element);
      assert(host_integerp(index, 1));
      FieldOffset = tree_low_cst(index, 1);
      FieldLastOffset = FieldOffset;
    } else {
      FieldOffset = NextFieldToFill;
      FieldLastOffset = FieldOffset;
    }

    // Process all of the elements in the range.
    for (--FieldOffset; FieldOffset != FieldLastOffset; ) {
      ++FieldOffset;
      if (FieldOffset == ResultElts.size())
        ResultElts.push_back(Val);
      else {
        if (FieldOffset >= ResultElts.size())
          ResultElts.resize(FieldOffset+1);
        ResultElts[FieldOffset] = Val;
      }

      NextFieldToFill = FieldOffset+1;
    }
  }

  // Zero length array.
  if (ResultElts.empty())
    return ConstantArray::get(
      cast<ArrayType>(ConvertType(TREE_TYPE(exp))), ResultElts);
  assert(SomeVal && "If we had some initializer, we should have some value!");

  // Do a post-pass over all of the elements.  We're taking care of two things
  // here:
  //   #1. If any elements did not have initializers specified, provide them
  //       with a null init.
  //   #2. If any of the elements have different types, return a struct instead
  //       of an array.  This can occur in cases where we have an array of
  //       unions, and the various unions had different pieces init'd.
  const Type *ElTy = SomeVal->getType();
  Constant *Filler = Constant::getNullValue(ElTy);
  bool AllEltsSameType = true;
  for (unsigned i = 0, e = ResultElts.size(); i != e; ++i) {
    if (ResultElts[i] == 0)
      ResultElts[i] = Filler;
    else if (ResultElts[i]->getType() != ElTy)
      AllEltsSameType = false;
  }

  if (TREE_CODE(InitType) == VECTOR_TYPE) {
    assert(AllEltsSameType && "Vector of heterogeneous element types?");
    return ConstantVector::get(ResultElts);
  }

  if (AllEltsSameType)
    return ConstantArray::get(
      ArrayType::get(ElTy, ResultElts.size()), ResultElts);
  return ConstantStruct::get(Context, ResultElts, false);
}


namespace {
/// ConstantLayoutInfo - A helper class used by ConvertRecordCONSTRUCTOR to
/// lay out struct inits.
struct ConstantLayoutInfo {
  const TargetData &TD;

  /// ResultElts - The initializer elements so far.
  std::vector<Constant*> ResultElts;

  /// StructIsPacked - This is set to true if we find out that we have to emit
  /// the ConstantStruct as a Packed LLVM struct type (because the LLVM
  /// alignment rules would prevent laying out the struct correctly).
  bool StructIsPacked;

  /// NextFieldByteStart - This field indicates the *byte* that the next field
  /// will start at.  Put another way, this is the size of the struct as
  /// currently laid out, but without any tail padding considered.
  uint64_t NextFieldByteStart;

  /// MaxLLVMFieldAlignment - This is the largest alignment of any IR field,
  /// which is the alignment that the ConstantStruct will get.
  unsigned MaxLLVMFieldAlignment;


  ConstantLayoutInfo(const TargetData &TD) : TD(TD) {
    StructIsPacked = false;
    NextFieldByteStart = 0;
    MaxLLVMFieldAlignment = 1;
  }

  void ConvertToPacked();
  void AddFieldToRecordConstant(Constant *Val, uint64_t GCCFieldOffsetInBits);
  void AddBitFieldToRecordConstant(ConstantInt *Val,
                                   uint64_t GCCFieldOffsetInBits);
  void HandleTailPadding(uint64_t GCCStructBitSize);
};

}

/// ConvertToPacked - Given a partially constructed initializer for a LLVM
/// struct constant, change it to make all the implicit padding between elements
/// be fully explicit.
void ConstantLayoutInfo::ConvertToPacked() {
  assert(!StructIsPacked && "Struct is already packed");
  uint64_t EltOffs = 0;
  for (unsigned i = 0, e = ResultElts.size(); i != e; ++i) {
    Constant *Val = ResultElts[i];

    // Check to see if this element has an alignment that would cause it to get
    // offset.  If so, insert explicit padding for the offset.
    unsigned ValAlign = TD.getABITypeAlignment(Val->getType());
    uint64_t AlignedEltOffs = TargetData::RoundUpAlignment(EltOffs, ValAlign);

    // If the alignment doesn't affect the element offset, then the value is ok.
    // Accept the field and keep moving.
    if (AlignedEltOffs == EltOffs) {
      EltOffs += TD.getTypeAllocSize(Val->getType());
      continue;
    }

    // Otherwise, there is padding here.  Insert explicit zeros.
    const Type *PadTy = Type::getInt8Ty(Context);
    if (AlignedEltOffs-EltOffs != 1)
      PadTy = ArrayType::get(PadTy, AlignedEltOffs-EltOffs);
    ResultElts.insert(ResultElts.begin()+i,
                      Constant::getNullValue(PadTy));

    // The padding is now element "i" and just bumped us up to "AlignedEltOffs".
    EltOffs = AlignedEltOffs;
    ++e;  // One extra element to scan.
  }

  // Packed now!
  MaxLLVMFieldAlignment = 1;
  StructIsPacked = true;
}


/// AddFieldToRecordConstant - As ConvertRecordCONSTRUCTOR builds up an LLVM
/// constant to represent a GCC CONSTRUCTOR node, it calls this method to add
/// fields.  The design of this is that it adds leading/trailing padding as
/// needed to make the piece fit together and honor the GCC layout.  This does
/// not handle bitfields.
///
/// The arguments are:
///   Val: The value to add to the struct, with a size that matches the size of
///        the corresponding GCC field.
///   GCCFieldOffsetInBits: The offset that we have to put Val in the result.
///
void ConstantLayoutInfo::
AddFieldToRecordConstant(Constant *Val, uint64_t GCCFieldOffsetInBits) {
  // Figure out how to add this non-bitfield value to our constant struct so
  // that it ends up at the right offset.  There are four cases we have to
  // think about:
  //   1. We may be able to just slap it onto the end of our struct and have
  //      everything be ok.
  //   2. We may have to insert explicit padding into the LLVM struct to get
  //      the initializer over into the right space.  This is needed when the
  //      GCC field has a larger alignment than the LLVM field.
  //   3. The LLVM field may be too far over and we may be forced to convert
  //      this to an LLVM packed struct.  This is required when the LLVM
  //      alignment is larger than the GCC alignment.
  //   4. We may have a bitfield that needs to be merged into a previous
  //      field.
  // Start by determining which case we have by looking at where LLVM and GCC
  // would place the field.

  // Verified that we haven't already laid out bytes that will overlap with
  // this new field.
  assert(NextFieldByteStart*8 <= GCCFieldOffsetInBits &&
         "Overlapping LLVM fields!");

  // Compute the offset the field would get if we just stuck 'Val' onto the
  // end of our structure right now.  It is NextFieldByteStart rounded up to
  // the LLVM alignment of Val's type.
  unsigned ValLLVMAlign = 1;

  if (!StructIsPacked) { // Packed structs ignore the alignment of members.
    ValLLVMAlign = TD.getABITypeAlignment(Val->getType());
    MaxLLVMFieldAlignment = std::max(MaxLLVMFieldAlignment, ValLLVMAlign);
  }

  // LLVMNaturalByteOffset - This is where LLVM would drop the field if we
  // slap it onto the end of the struct.
  uint64_t LLVMNaturalByteOffset
    = TargetData::RoundUpAlignment(NextFieldByteStart, ValLLVMAlign);

  // If adding the LLVM field would push it over too far, then we must have a
  // case that requires the LLVM struct to be packed.  Do it now if so.
  if (LLVMNaturalByteOffset*8 > GCCFieldOffsetInBits) {
    // Switch to packed.
    ConvertToPacked();
    assert(NextFieldByteStart*8 <= GCCFieldOffsetInBits &&
           "Packing didn't fix the problem!");

    // Recurse to add the field after converting to packed.
    return AddFieldToRecordConstant(Val, GCCFieldOffsetInBits);
  }

  // If the LLVM offset is not large enough, we need to insert explicit
  // padding in the LLVM struct between the fields.
  if (LLVMNaturalByteOffset*8 < GCCFieldOffsetInBits) {
    // Insert enough padding to fully fill in the hole.  Insert padding from
    // NextFieldByteStart (not LLVMNaturalByteOffset) because the padding will
    // not get the same alignment as "Val".
    const Type *FillTy = Type::getInt8Ty(Context);
    if (GCCFieldOffsetInBits/8-NextFieldByteStart != 1)
      FillTy = ArrayType::get(FillTy,
                              GCCFieldOffsetInBits/8-NextFieldByteStart);
    ResultElts.push_back(Constant::getNullValue(FillTy));

    NextFieldByteStart = GCCFieldOffsetInBits/8;

    // Recurse to add the field.  This handles the case when the LLVM struct
    // needs to be converted to packed after inserting tail padding.
    return AddFieldToRecordConstant(Val, GCCFieldOffsetInBits);
  }

  // Slap 'Val' onto the end of our ConstantStruct, it must be known to land
  // at the right offset now.
  assert(LLVMNaturalByteOffset*8 == GCCFieldOffsetInBits);
  ResultElts.push_back(Val);
  NextFieldByteStart = LLVMNaturalByteOffset;
  NextFieldByteStart += TD.getTypeAllocSize(Val->getType());
}

/// AddBitFieldToRecordConstant - Bitfields can span multiple LLVM fields and
/// have other annoying properties, thus requiring extra layout rules.  This
/// routine handles the extra complexity and then forwards to
/// AddFieldToRecordConstant.
void ConstantLayoutInfo::
AddBitFieldToRecordConstant(ConstantInt *ValC, uint64_t GCCFieldOffsetInBits) {
  // If the GCC field starts after our current LLVM field then there must have
  // been an anonymous bitfield or other thing that shoved it over.  No matter,
  // just insert some i8 padding until there are bits to fill in.
  while (GCCFieldOffsetInBits > NextFieldByteStart*8) {
    ResultElts.push_back(ConstantInt::get(Type::getInt8Ty(Context), 0));
    ++NextFieldByteStart;
  }

  // If the field is a bitfield, it could partially go in a previously
  // laid out structure member, and may add elements to the end of the currently
  // laid out structure.
  //
  // Since bitfields can only partially overlap other bitfields, because we
  // always emit components of bitfields as i8, and because we never emit tail
  // padding until we know it exists, this boils down to merging pieces of the
  // bitfield values into i8's.  This is also simplified by the fact that
  // bitfields can only be initialized by ConstantInts.  An interesting case is
  // sharing of tail padding in C++ structures.  Because this can only happen
  // in inheritance cases, and those are non-POD, we should never see them here.

  // First handle any part of Val that overlaps an already laid out field by
  // merging it into it.  By the above invariants, we know that it is an i8 that
  // we are merging into.  Note that we may be inserting *all* of Val into the
  // previous field.
  if (GCCFieldOffsetInBits < NextFieldByteStart*8) {
    unsigned ValBitSize = ValC->getBitWidth();
    assert(!ResultElts.empty() && "Bitfield starts before first element?");
    assert(ResultElts.back()->getType()->isIntegerTy(8) &&
           isa<ConstantInt>(ResultElts.back()) &&
           "Merging bitfield with non-bitfield value?");
    assert(NextFieldByteStart*8 - GCCFieldOffsetInBits < 8 &&
           "Bitfield overlaps backwards more than one field?");

    // Figure out how many bits can fit into the previous field given the
    // starting point in that field.
    unsigned BitsInPreviousField =
      unsigned(NextFieldByteStart*8 - GCCFieldOffsetInBits);
    assert(BitsInPreviousField != 0 && "Previous field should not be null!");

    // Split the bits that will be inserted into the previous element out of
    // Val into a new constant.  If Val is completely contained in the previous
    // element, this sets Val to null, otherwise we shrink Val to contain the
    // bits to insert in the next element.
    APInt ValForPrevField(ValC->getValue());
    if (BitsInPreviousField >= ValBitSize) {
      // The whole field fits into the previous field.
      ValC = 0;
    } else if (!BYTES_BIG_ENDIAN) {
      // Little endian, take bits from the bottom of the field value.
      ValForPrevField = ValForPrevField.trunc(BitsInPreviousField);
      APInt Tmp = ValC->getValue();
      Tmp = Tmp.lshr(BitsInPreviousField);
      Tmp = Tmp.trunc(ValBitSize-BitsInPreviousField);
      ValC = ConstantInt::get(Context, Tmp);
    } else {
      // Big endian, take bits from the top of the field value.
      ValForPrevField = ValForPrevField.lshr(ValBitSize-BitsInPreviousField);
      ValForPrevField = ValForPrevField.trunc(BitsInPreviousField);

      APInt Tmp = ValC->getValue();
      Tmp = Tmp.trunc(ValBitSize-BitsInPreviousField);
      ValC = ConstantInt::get(Context, Tmp);
    }

    // Okay, we're going to insert ValForPrevField into the previous i8, extend
    // it and shift into place.
    ValForPrevField = ValForPrevField.zext(8);
    if (!BYTES_BIG_ENDIAN) {
      ValForPrevField = ValForPrevField.shl(8-BitsInPreviousField);
    } else {
      // On big endian, if the entire field fits into the remaining space, shift
      // over to not take part of the next field's bits.
      if (BitsInPreviousField > ValBitSize)
        ValForPrevField = ValForPrevField.shl(BitsInPreviousField-ValBitSize);
    }

    // "or" in the previous value and install it.
    const APInt &LastElt = cast<ConstantInt>(ResultElts.back())->getValue();
    ResultElts.back() = ConstantInt::get(Context, ValForPrevField | LastElt);

    // If the whole bit-field fit into the previous field, we're done.
    if (ValC == 0) return;
    GCCFieldOffsetInBits = NextFieldByteStart*8;
  }

  APInt Val = ValC->getValue();

  // Okay, we know that we're plopping bytes onto the end of the struct.
  // Iterate while there is stuff to do.
  while (1) {
    ConstantInt *ValToAppend;
    if (Val.getBitWidth() > 8) {
      if (!BYTES_BIG_ENDIAN) {
        // Little endian lays out low bits first.
        APInt Tmp = Val.trunc(8);
        ValToAppend = ConstantInt::get(Context, Tmp);

        Val = Val.lshr(8);
      } else {
        // Big endian lays out high bits first.
        APInt Tmp = Val.lshr(Val.getBitWidth()-8).trunc(8);
        ValToAppend = ConstantInt::get(Context, Tmp);
      }
    } else if (Val.getBitWidth() == 8) {
      ValToAppend = ConstantInt::get(Context, Val);
    } else {
      APInt Tmp = Val.zext(8);

      if (BYTES_BIG_ENDIAN)
        Tmp = Tmp << 8-Val.getBitWidth();
      ValToAppend = ConstantInt::get(Context, Tmp);
    }

    ResultElts.push_back(ValToAppend);
    ++NextFieldByteStart;

    if (Val.getBitWidth() <= 8)
      break;
    Val = Val.trunc(Val.getBitWidth()-8);
  }
}


/// HandleTailPadding - Check to see if the struct fields, as laid out so far,
/// will be large enough to make the generated constant struct have the right
/// size.  If not, add explicit tail padding.  If rounding up based on the LLVM
/// IR alignment would make the struct too large, convert it to a packed LLVM
/// struct.
void ConstantLayoutInfo::HandleTailPadding(uint64_t GCCStructBitSize) {
  uint64_t GCCStructSize = (GCCStructBitSize+7)/8;
  uint64_t LLVMNaturalSize =
    TargetData::RoundUpAlignment(NextFieldByteStart, MaxLLVMFieldAlignment);

  // If the total size of the laid out data is within the size of the GCC type
  // but the rounded-up size (including the tail padding induced by LLVM
  // alignment) is too big, convert to a packed struct type.  We don't do this
  // if the size of the laid out fields is too large because initializers like
  //
  //    struct X { int A; char C[]; } x = { 4, "foo" };
  //
  // can occur and no amount of packing will help.
  if (NextFieldByteStart <= GCCStructSize &&   // Not flexible init case.
      LLVMNaturalSize > GCCStructSize) {       // Tail pad will overflow type.
    assert(!StructIsPacked && "LLVM Struct type overflow!");

    // Switch to packed.
    ConvertToPacked();
    LLVMNaturalSize = NextFieldByteStart;

    // Verify that packing solved the problem.
    assert(LLVMNaturalSize <= GCCStructSize &&
           "Oversized should be handled by packing");
  }

  // If the LLVM Size is too small, add some tail padding to fill it in.
  if (LLVMNaturalSize < GCCStructSize) {
    const Type *FillTy = Type::getInt8Ty(Context);
    if (GCCStructSize - NextFieldByteStart != 1)
      FillTy = ArrayType::get(FillTy, GCCStructSize - NextFieldByteStart);
    ResultElts.push_back(Constant::getNullValue(FillTy));
    NextFieldByteStart = GCCStructSize;

    // At this point, we know that our struct should have the right size.
    // However, if the size of the struct is not a multiple of the largest
    // element alignment, the rounding could bump up the struct more.  In this
    // case, we have to convert the struct to being packed.
    LLVMNaturalSize =
      TargetData::RoundUpAlignment(NextFieldByteStart, MaxLLVMFieldAlignment);

    // If the alignment will make the struct too big, convert it to being
    // packed.
    if (LLVMNaturalSize > GCCStructSize) {
      assert(!StructIsPacked && "LLVM Struct type overflow!");
      ConvertToPacked();
    }
  }
}

Constant *TreeConstantToLLVM::ConvertRecordCONSTRUCTOR(tree exp) {
  ConstantLayoutInfo LayoutInfo(getTargetData());

  tree NextField = TYPE_FIELDS(TREE_TYPE(exp));
  unsigned HOST_WIDE_INT CtorIndex;
  tree FieldValue;
  tree Field; // The FIELD_DECL for the field.

  FOR_EACH_CONSTRUCTOR_ELT(CONSTRUCTOR_ELTS(exp), CtorIndex, Field, FieldValue){
    // If an explicit field is specified, use it.
    if (Field == 0) {
      Field = NextField;
      // Advance to the next FIELD_DECL, skipping over other structure members
      // (e.g. enums).
      while (1) {
        assert(Field && "Fell off end of record!");
        if (TREE_CODE(Field) == FIELD_DECL) break;
        Field = TREE_CHAIN(Field);
      }
    }

    // GCCFieldOffsetInBits is where GCC is telling us to put the current field.
    uint64_t GCCFieldOffsetInBits = getFieldOffsetInBits(Field);
    NextField = TREE_CHAIN(Field);

    uint64_t FieldSizeInBits = 0;
    uint64_t ValueSizeInBits = 0;
    Constant *Val = 0;
    ConstantInt *ValC = 0;
    
    // Zero-sized bitfields upset the type converter.  If it's not a
    // bit-field, or it is a bit-field but it has a non-zero precision
    // type, go ahead and convert it.
    if (!isBitfield(Field) || TYPE_PRECISION(TREE_TYPE(Field)))
      Val = Convert(FieldValue);        // Decode the field's value.

    if (DECL_SIZE(Field)) {
      FieldSizeInBits = getInt64(DECL_SIZE(Field), true);
      if (FieldSizeInBits == 0)
        continue;       // Skip zero-sized fields.
      ValueSizeInBits = Val->getType()->getPrimitiveSizeInBits();
      ValC = dyn_cast<ConstantInt>(Val);
      if (ValC && ValC->isZero()) {
        // G++ has various bugs handling {} initializers where it doesn't
        // synthesize a zero node of the right type. Instead of figuring out G++,
        // just hack around it by special casing zero and allowing it to be the
        // wrong size.
        if (ValueSizeInBits != FieldSizeInBits) {
          APInt ValAsInt = ValC->getValue();
          ValC = ConstantInt::get(Context, ValueSizeInBits < FieldSizeInBits ?
                                  ValAsInt.zext(FieldSizeInBits) :
                                  ValAsInt.trunc(FieldSizeInBits));
          ValueSizeInBits = FieldSizeInBits;
          Val = ValC;
        }
      }
    }

    // If this is a non-bitfield value, just slap it onto the end of the struct
    // with the appropriate padding etc.  If it is a bitfield, we have more
    // processing to do.
    if (!isBitfield(Field))
      LayoutInfo.AddFieldToRecordConstant(Val, GCCFieldOffsetInBits);
    else {
      // Bitfields can only be initialized with constants (integer constant
      // expressions).
      assert(ValC);
      assert(DECL_SIZE(Field));
      assert(ValueSizeInBits >= FieldSizeInBits &&
             "disagreement between LLVM and GCC on bitfield size");
      if (ValueSizeInBits != FieldSizeInBits) {
        // Fields are allowed to be smaller than their type.  Simply discard
        // the unwanted upper bits in the field value.
        APInt ValAsInt = ValC->getValue();
        ValC = ConstantInt::get(Context, ValAsInt.trunc(FieldSizeInBits));
      }
      LayoutInfo.AddBitFieldToRecordConstant(ValC, GCCFieldOffsetInBits);
    }
  }

  // Check to see if the struct fields, as laid out so far, will be large enough
  // to make the generated constant struct have the right size.  If not, add
  // explicit tail padding.  If rounding up based on the LLVM IR alignment would
  // make the struct too large, convert it to a packed LLVM struct.
  tree StructTypeSizeTree = TYPE_SIZE(TREE_TYPE(exp));
  if (StructTypeSizeTree && TREE_CODE(StructTypeSizeTree) == INTEGER_CST)
    LayoutInfo.HandleTailPadding(getInt64(StructTypeSizeTree, true));

  // Okay, we're done, return the computed elements.
  return ConstantStruct::get(Context, LayoutInfo.ResultElts,
                             LayoutInfo.StructIsPacked);
}

Constant *TreeConstantToLLVM::ConvertUnionCONSTRUCTOR(tree exp) {
  assert(!VEC_empty(constructor_elt, CONSTRUCTOR_ELTS(exp))
         && "Union CONSTRUCTOR has no elements? Zero?");

  VEC(constructor_elt, gc) *elt = CONSTRUCTOR_ELTS(exp);
  assert(VEC_length(constructor_elt, elt) == 1
         && "Union CONSTRUCTOR with multiple elements?");

  ConstantLayoutInfo LayoutInfo(getTargetData());

  // Convert the constant itself.
  Constant *Val = Convert(VEC_index(constructor_elt, elt, 0)->value);

  // Unions are initialized using the first member field.  Find it.
  tree Field = TYPE_FIELDS(TREE_TYPE(exp));
  assert(Field && "cannot initialize union with no fields");
  while (TREE_CODE(Field) != FIELD_DECL) {
    Field = TREE_CHAIN(Field);
    assert(Field && "cannot initialize union with no fields");
  }

  // If this is a non-bitfield value, just slap it onto the end of the struct
  // with the appropriate padding etc.  If it is a bitfield, we have more
  // processing to do.
  if (!isBitfield(Field))
    LayoutInfo.AddFieldToRecordConstant(Val, 0);
  else {
    // Bitfields can only be initialized with constants (integer constant
    // expressions).
    ConstantInt *ValC = cast<ConstantInt>(Val);
    uint64_t FieldSizeInBits = getInt64(DECL_SIZE(Field), true);
    uint64_t ValueSizeInBits = Val->getType()->getPrimitiveSizeInBits();

    assert(ValueSizeInBits >= FieldSizeInBits &&
           "disagreement between LLVM and GCC on bitfield size");
    if (ValueSizeInBits != FieldSizeInBits) {
      // Fields are allowed to be smaller than their type.  Simply discard
      // the unwanted upper bits in the field value.
      APInt ValAsInt = ValC->getValue();
      ValC = ConstantInt::get(Context, ValAsInt.trunc(FieldSizeInBits));
    }
    LayoutInfo.AddBitFieldToRecordConstant(ValC, 0);
  }

  // If the union has a fixed size, and if the value we converted isn't large
  // enough to fill all the bits, add a zero initialized array at the end to pad
  // it out.
  tree UnionTypeSizeTree = TYPE_SIZE(TREE_TYPE(exp));
  if (UnionTypeSizeTree && TREE_CODE(UnionTypeSizeTree) == INTEGER_CST)
    LayoutInfo.HandleTailPadding(getInt64(UnionTypeSizeTree, true));

  return ConstantStruct::get(Context, LayoutInfo.ResultElts,
                             LayoutInfo.StructIsPacked);
}

//===----------------------------------------------------------------------===//
//                  ... Constant Expressions L-Values ...
//===----------------------------------------------------------------------===//

Constant *TreeConstantToLLVM::EmitLV(tree exp) {
  Constant *LV;

  switch (TREE_CODE(exp)) {
  default:
    debug_tree(exp);
    assert(0 && "Unknown constant lvalue to convert!");
    abort();
  case FUNCTION_DECL:
  case CONST_DECL:
  case VAR_DECL:
    LV = EmitLV_Decl(exp);
    break;
  case LABEL_DECL:
    LV = EmitLV_LABEL_DECL(exp);
    break;
  case COMPLEX_CST:
    LV = EmitLV_COMPLEX_CST(exp);
    break;
  case STRING_CST:
    LV = EmitLV_STRING_CST(exp);
    break;
  case COMPONENT_REF:
    LV = EmitLV_COMPONENT_REF(exp);
    break;
  case ARRAY_RANGE_REF:
  case ARRAY_REF:
    LV = EmitLV_ARRAY_REF(exp);
    break;
  case INDIRECT_REF:
    // The lvalue is just the address.
    LV = Convert(TREE_OPERAND(exp, 0));
    break;
  case COMPOUND_LITERAL_EXPR: // FIXME: not gimple - defined by C front-end
    /* This used to read
       return EmitLV(COMPOUND_LITERAL_EXPR_DECL(exp));
       but gcc warns about that and there doesn't seem to be any way to stop it
       with casts or the like.  The following is equivalent with no checking
       (since we know TREE_CODE(exp) is COMPOUND_LITERAL_EXPR the checking
       doesn't accomplish anything anyway). */
    LV = EmitLV(DECL_EXPR_DECL (TREE_OPERAND (exp, 0)));
    break;
  }

  // Check that the type of the lvalue is indeed that of a pointer to the tree
  // node.  Since LLVM has no void* type, don't insist that void* be converted
  // to a specific LLVM type.
  assert((VOID_TYPE_P(TREE_TYPE(exp)) ||
          LV->getType() == ConvertType(TREE_TYPE(exp))->getPointerTo()) &&
         "LValue of constant has wrong type!");

  return LV;
}

Constant *TreeConstantToLLVM::EmitLV_Decl(tree exp) {
  GlobalValue *Val = cast<GlobalValue>(DECL_LLVM(exp));

  // Ensure variable marked as used even if it doesn't go through a parser.  If
  // it hasn't been used yet, write out an external definition.
  if (!TREE_USED(exp)) {
    assemble_external(exp);
    TREE_USED(exp) = 1;
    Val = cast<GlobalValue>(DECL_LLVM(exp));
  }

  // If this is an aggregate, emit it to LLVM now.  GCC happens to
  // get this case right by forcing the initializer into memory.
  if (TREE_CODE(exp) == CONST_DECL || TREE_CODE(exp) == VAR_DECL) {
    if ((DECL_INITIAL(exp) || !TREE_PUBLIC(exp)) && !DECL_EXTERNAL(exp) &&
        Val->isDeclaration() &&
        !BOGUS_CTOR(exp)) {
      emit_global_to_llvm(exp);
      // Decl could have change if it changed type.
      Val = cast<GlobalValue>(DECL_LLVM(exp));
    }
  } else {
    // Otherwise, inform cgraph that we used the global.
    mark_decl_referenced(exp);
    if (tree ID = DECL_ASSEMBLER_NAME(exp))
      mark_referenced(ID);
  }

  // The type of the global value output for exp need not match that of exp.
  // For example if the global's initializer has a different type to the global
  // itself (allowed in GCC but not in LLVM) then the global is changed to have
  // the type of the initializer.  Correct for this now.
  const Type *Ty = ConvertType(TREE_TYPE(exp));
  if (Ty->isVoidTy()) Ty = Type::getInt8Ty(Context);  // void* -> i8*.

  return TheFolder->CreateBitCast(Val, Ty->getPointerTo());
}

/// EmitLV_LABEL_DECL - Someone took the address of a label.
Constant *TreeConstantToLLVM::EmitLV_LABEL_DECL(tree exp) {
  assert(TheTreeToLLVM &&
         "taking the address of a label while not compiling the function!");

  // Figure out which function this is for, verify it's the one we're compiling.
  if (DECL_CONTEXT(exp)) {
    assert(TREE_CODE(DECL_CONTEXT(exp)) == FUNCTION_DECL &&
           "Address of label in nested function?");
    assert(TheTreeToLLVM->getFUNCTION_DECL() == DECL_CONTEXT(exp) &&
           "Taking the address of a label that isn't in the current fn!?");
  }

  return TheTreeToLLVM->EmitLV_LABEL_DECL(exp);
}

Constant *TreeConstantToLLVM::EmitLV_COMPLEX_CST(tree exp) {
  Constant *Init = TreeConstantToLLVM::ConvertCOMPLEX_CST(exp);

  // Cache the constants to avoid making obvious duplicates that have to be
  // folded by the optimizer.
  static std::map<Constant*, GlobalVariable*> ComplexCSTCache;
  GlobalVariable *&Slot = ComplexCSTCache[Init];
  if (Slot) return Slot;

  // Create a new complex global.
  Slot = new GlobalVariable(*TheModule, Init->getType(), true,
                            GlobalVariable::PrivateLinkage, Init, ".cpx");
  Slot->setUnnamedAddr(true);
  return Slot;
}

Constant *TreeConstantToLLVM::EmitLV_STRING_CST(tree exp) {
  Constant *Init = TreeConstantToLLVM::ConvertSTRING_CST(exp);

  // Support -fwritable-strings.
  bool StringIsConstant = !flag_writable_strings;
  bool IsConstCFString = false;
#ifdef CONFIG_DARWIN_H
  // However, ignores it if exp is a CFString and
  // -fconstant-cfstring (default) is set.
  IsConstCFString = darwin_constant_cfstring_p(exp);
  StringIsConstant |= IsConstCFString;
#endif

  // Literal cstrings in data section needs a label the linker can
  // see to prevent it from being merged into its previous label.
  GlobalValue::LinkageTypes Linkage = StringIsConstant
    ? GlobalValue::PrivateLinkage
    : GlobalValue::InternalLinkage;

  GlobalVariable **SlotP = 0;
  if (StringIsConstant) {
    // Cache the string constants to avoid making obvious duplicate strings that
    // have to be folded by the optimizer.
    static std::map<Constant*, GlobalVariable*> StringCSTCache;
    GlobalVariable *&Slot = StringCSTCache[Init];
    if (Slot) return Slot;
    SlotP = &Slot;
  }

  // Create a new string global.
  GlobalVariable *GV = new GlobalVariable(*TheModule, Init->getType(),
                                          StringIsConstant, Linkage, Init,
                                          ".str");
  GV->setUnnamedAddr(true);
  if (!IsConstCFString) {
    GV->setAlignment(get_constant_alignment(exp) / 8);
  } else {
    // CFStrings don't need to be over-aligned. Align them to 1.
    // <rdar://problem/8961909>
    GV->setAlignment(1);
    GV->setSection("__TEXT,__cstring,cstring_literals");
  }

  if (SlotP) *SlotP = GV;
  return GV;
}

Constant *TreeConstantToLLVM::EmitLV_ARRAY_REF(tree exp) {
  tree Array = TREE_OPERAND(exp, 0);
  tree ArrayType = TREE_TYPE(Array);
  tree Index = TREE_OPERAND(exp, 1);
  tree IndexType = TREE_TYPE(Index);
  assert((TREE_CODE(ArrayType) == ARRAY_TYPE ||
          TREE_CODE(ArrayType) == POINTER_TYPE ||
          TREE_CODE(ArrayType) == REFERENCE_TYPE ||
          TREE_CODE(ArrayType) == BLOCK_POINTER_TYPE) &&
         "Unknown ARRAY_REF!");

  // Check for variable sized reference.
  // FIXME: add support for array types where the size doesn't fit into 64 bits
  assert(isSequentialCompatible(ArrayType) && "Global with variable size?");

  // As an LLVM extension, we allow ARRAY_REF with a pointer as the first
  // operand.  This construct maps directly to a getelementptr instruction.
  Constant *ArrayAddr;
  if (TREE_CODE(ArrayType) == ARRAY_TYPE) {
    // First subtract the lower bound, if any, in the type of the index.
    tree LowerBound = array_ref_low_bound(exp);
    if (!integer_zerop(LowerBound))
      Index = fold(build2(MINUS_EXPR, IndexType, Index, LowerBound));
    ArrayAddr = EmitLV(Array);
  } else {
    ArrayAddr = Convert(Array);
  }

  Constant *IndexVal = Convert(Index);

  const Type *IntPtrTy = getTargetData().getIntPtrType(Context);
  if (IndexVal->getType() != IntPtrTy)
    IndexVal = TheFolder->CreateIntCast(IndexVal, IntPtrTy,
                                        !TYPE_UNSIGNED(IndexType));

  std::vector<Value*> Idx;
  if (TREE_CODE(ArrayType) == ARRAY_TYPE)
    Idx.push_back(ConstantInt::get(IntPtrTy, 0));
  Idx.push_back(IndexVal);

  return TheFolder->CreateGetElementPtr(ArrayAddr, &Idx[0], Idx.size());
}

Constant *TreeConstantToLLVM::EmitLV_COMPONENT_REF(tree exp) {
  Constant *StructAddrLV = EmitLV(TREE_OPERAND(exp, 0));

  // Ensure that the struct type has been converted, so that the fielddecls
  // are laid out.
  const Type *StructTy = ConvertType(TREE_TYPE(TREE_OPERAND(exp, 0)));

  tree FieldDecl = TREE_OPERAND(exp, 1);

  StructAddrLV = TheFolder->CreateBitCast(StructAddrLV,
                                      StructTy->getPointerTo());
  const Type *FieldTy = ConvertType(getDeclaredType(FieldDecl));

  // BitStart - This is the actual offset of the field from the start of the
  // struct, in bits.  For bitfields this may be on a non-byte boundary.
  unsigned BitStart = getComponentRefOffsetInBits(exp);
  Constant *FieldPtr;
  const TargetData &TD = getTargetData();

  tree field_offset = component_ref_field_offset (exp);
  // If this is a normal field at a fixed offset from the start, handle it.
  if (TREE_CODE(field_offset) == INTEGER_CST) {
    unsigned int MemberIndex = GET_LLVM_FIELD_INDEX(FieldDecl);

    Constant *Ops[] = {
      StructAddrLV,
      Constant::getNullValue(Type::getInt32Ty(Context)),
      ConstantInt::get(Type::getInt32Ty(Context), MemberIndex)
    };
    FieldPtr = TheFolder->CreateGetElementPtr(StructAddrLV, Ops+1, 2);

    FieldPtr = ConstantFoldInstOperands(Instruction::GetElementPtr,
                                        FieldPtr->getType(), Ops, 3, &TD);

    // Now that we did an offset from the start of the struct, subtract off
    // the offset from BitStart.
    if (MemberIndex) {
      const StructLayout *SL = TD.getStructLayout(cast<StructType>(StructTy));
      BitStart -= SL->getElementOffset(MemberIndex) * 8;
    }

  } else {
    Constant *Offset = Convert(field_offset);
    Constant *Ptr = TheFolder->CreatePtrToInt(StructAddrLV, Offset->getType());
    Ptr = TheFolder->CreateAdd(Ptr, Offset);
    FieldPtr = TheFolder->CreateIntToPtr(Ptr,
                                         FieldTy->getPointerTo());
  }

  // Make sure we return a result of the right type.
  if (FieldTy->getPointerTo() != FieldPtr->getType())
    FieldPtr = TheFolder->CreateBitCast(FieldPtr, FieldTy->getPointerTo());

  assert(BitStart == 0 &&
         "It's a bitfield reference or we didn't get to the field!");
  return FieldPtr;
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
