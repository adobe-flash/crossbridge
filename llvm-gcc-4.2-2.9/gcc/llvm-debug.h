/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* Internal interfaces between the LLVM backend components
Copyright (C) 2006 Free Software Foundation, Inc.
Contributed by Jim Laskey  (jlaskey@apple.com)

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
// This is a C++ header file that defines the debug interfaces shared among
// the llvm-*.cpp files.
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUG_H
#define LLVM_DEBUG_H

#include "llvm-internal.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Support/ValueHandle.h"
#include "llvm/Support/Allocator.h"

extern "C" {
#include "llvm.h"
}  

#include <string>
#include <map>
#include <vector>

namespace llvm {

// Forward declarations
class AllocaInst;
class BasicBlock;
class CallInst;
class Function;
class Module;

/// DIFactory - This object assists with the construction of the various
/// descriptors.
class DIFactory {
  Module &M;
  LLVMContext& VMContext;
  
  Function *DeclareFn;     // llvm.dbg.declare
  Function *ValueFn;       // llvm.dbg.value
  
  DIFactory(const DIFactory &);     // DO NOT IMPLEMENT
  void operator=(const DIFactory&); // DO NOT IMPLEMENT
  public:
  enum ComplexAddrKind { OpPlus=1, OpDeref };
  
  explicit DIFactory(Module &m);
  
  /// GetOrCreateArray - Create an descriptor for an array of descriptors.
  /// This implicitly uniques the arrays created.
  DIArray GetOrCreateArray(DIDescriptor *Tys, unsigned NumTys);
  
  /// GetOrCreateSubrange - Create a descriptor for a value range.  This
  /// implicitly uniques the values returned.
  DISubrange GetOrCreateSubrange(int64_t Lo, int64_t Hi);
  
  /// CreateUnspecifiedParameter - Create unspeicified type descriptor
  /// for a subroutine type.
  DIDescriptor CreateUnspecifiedParameter();
  
  /// CreateCompileUnit - Create a new descriptor for the specified compile
  /// unit.
  DICompileUnit CreateCompileUnit(unsigned LangID,
                                  StringRef Filename,
                                  StringRef Directory,
                                  StringRef Producer,
                                  bool isMain = false,
                                  bool isOptimized = false,
                                  StringRef Flags = "",
                                  unsigned RunTimeVer = 0);
  
  /// CreateFile -  Create a new descriptor for the specified file.
  DIFile CreateFile(StringRef Filename, StringRef Directory,
                    DICompileUnit CU);
  
  /// CreateEnumerator - Create a single enumerator value.
  DIEnumerator CreateEnumerator(StringRef Name, uint64_t Val);
  
  /// CreateBasicType - Create a basic type like int, float, etc.
  DIBasicType CreateBasicType(DIDescriptor Context, StringRef Name,
                              DIFile F, unsigned LineNumber,
                              uint64_t SizeInBits, uint64_t AlignInBits,
                              uint64_t OffsetInBits, unsigned Flags,
                              unsigned Encoding);
  
  /// CreateBasicType - Create a basic type like int, float, etc.
  DIBasicType CreateBasicTypeEx(DIDescriptor Context, StringRef Name,
                                DIFile F, unsigned LineNumber,
                                Constant *SizeInBits, Constant *AlignInBits,
                                Constant *OffsetInBits, unsigned Flags,
                                unsigned Encoding);
  
  /// CreateDerivedType - Create a derived type like const qualified type,
  /// pointer, typedef, etc.
  DIDerivedType CreateDerivedType(unsigned Tag, DIDescriptor Context,
                                  StringRef Name,
                                  DIFile F,
                                  unsigned LineNumber,
                                  uint64_t SizeInBits, uint64_t AlignInBits,
                                  uint64_t OffsetInBits, unsigned Flags,
                                  DIType DerivedFrom);
  
  /// CreateDerivedType - Create a derived type like const qualified type,
  /// pointer, typedef, etc.
  DIDerivedType CreateDerivedTypeEx(unsigned Tag, DIDescriptor Context,
                                    StringRef Name,
                                    DIFile F,
                                    unsigned LineNumber,
                                    Constant *SizeInBits, 
                                    Constant *AlignInBits,
                                    Constant *OffsetInBits, unsigned Flags,
                                    DIType DerivedFrom);
  
  /// CreateCompositeType - Create a composite type like array, struct, etc.
  DICompositeType CreateCompositeType(unsigned Tag, DIDescriptor Context,
                                      StringRef Name,
                                      DIFile F,
                                      unsigned LineNumber,
                                      uint64_t SizeInBits,
                                      uint64_t AlignInBits,
                                      uint64_t OffsetInBits, unsigned Flags,
                                      DIType DerivedFrom,
                                      DIArray Elements,
                                      unsigned RunTimeLang = 0,
                                      MDNode *ContainingType = 0);
  
  /// CreateTemporaryType - Create a temporary forward-declared type.
  DIType CreateTemporaryType();
  DIType CreateTemporaryType(DIFile F);
  
  /// CreateArtificialType - Create a new DIType with "artificial" flag set.
  DIType CreateArtificialType(DIType Ty);
  
  /// CreateCompositeType - Create a composite type like array, struct, etc.
  DICompositeType CreateCompositeTypeEx(unsigned Tag, DIDescriptor Context,
                                        StringRef Name,
                                        DIFile F,
                                        unsigned LineNumber,
                                        Constant *SizeInBits,
                                        Constant *AlignInBits,
                                        Constant *OffsetInBits, 
                                        unsigned Flags,
                                        DIType DerivedFrom,
                                        DIArray Elements,
                                        unsigned RunTimeLang = 0,
                                        MDNode *ContainingType = 0);
  
  /// CreateSubprogram - Create a new descriptor for the specified subprogram.
  /// See comments in DISubprogram for descriptions of these fields.
  DISubprogram CreateSubprogram(DIDescriptor Context, StringRef Name,
                                StringRef DisplayName,
                                StringRef LinkageName,
                                DIFile F, unsigned LineNo,
                                DIType Ty, bool isLocalToUnit,
                                bool isDefinition,
                                unsigned VK = 0,
                                unsigned VIndex = 0,
                                DIType ContainingType = DIType(),
                                unsigned Flags = 0,
                                bool isOptimized = false,
                                Function *Fn = 0);
  
  /// CreateSubprogramDefinition - Create new subprogram descriptor for the
  /// given declaration. 
  DISubprogram CreateSubprogramDefinition(DISubprogram &SPDeclaration);
  
  /// CreateGlobalVariable - Create a new descriptor for the specified global.
  DIGlobalVariable
    CreateGlobalVariable(DIDescriptor Context, StringRef Name,
                         StringRef DisplayName,
                         StringRef LinkageName,
                         DIFile F,
                         unsigned LineNo, DIType Ty, bool isLocalToUnit,
                         bool isDefinition, llvm::GlobalVariable *GV);
  
  /// CreateGlobalVariable - Create a new descriptor for the specified constant.
  DIGlobalVariable
    CreateGlobalVariable(DIDescriptor Context, StringRef Name,
                         StringRef DisplayName,
                         StringRef LinkageName,
                         DIFile F,
                         unsigned LineNo, DIType Ty, bool isLocalToUnit,
                         bool isDefinition, llvm::Constant *C);
  
  /// CreateVariable - Create a new descriptor for the specified variable.
  DIVariable CreateVariable(unsigned Tag, DIDescriptor Context,
                            StringRef Name,
                            DIFile F, unsigned LineNo,
                            DIType Ty, bool AlwaysPreserve = false,
                            unsigned Flags = 0);
  
  /// CreateComplexVariable - Create a new descriptor for the specified
  /// variable which has a complex address expression for its address.
  DIVariable CreateComplexVariable(unsigned Tag, DIDescriptor Context,
                                   StringRef Name, DIFile F, unsigned LineNo,
                                   DIType Ty, Value *const *Addr,
                                   unsigned NumAddr);
  
  /// CreateLexicalBlock - This creates a descriptor for a lexical block
  /// with the specified parent context.
  DILexicalBlock CreateLexicalBlock(DIDescriptor Context, DIFile F,
                                    unsigned Line = 0, unsigned Col = 0);
  
  /// CreateNameSpace - This creates new descriptor for a namespace
  /// with the specified parent context.
  DINameSpace CreateNameSpace(DIDescriptor Context, StringRef Name,
                              DIFile F, unsigned LineNo);
  
  /// CreateLocation - Creates a debug info location.
  DILocation CreateLocation(unsigned LineNo, unsigned ColumnNo,
                            DIScope S, DILocation OrigLoc);
  
  /// CreateLocation - Creates a debug info location.
  DILocation CreateLocation(unsigned LineNo, unsigned ColumnNo,
                              DIScope S, MDNode *OrigLoc = 0);
  
  /// InsertDeclare - Insert a new llvm.dbg.declare intrinsic call.
  Instruction *InsertDeclare(llvm::Value *Storage, DIVariable D,
                             BasicBlock *InsertAtEnd);
  
  /// InsertDeclare - Insert a new llvm.dbg.declare intrinsic call.
  Instruction *InsertDeclare(llvm::Value *Storage, DIVariable D,
                             Instruction *InsertBefore);
  
  /// InsertDbgValueIntrinsic - Insert a new llvm.dbg.value intrinsic call.
  Instruction *InsertDbgValueIntrinsic(llvm::Value *V, uint64_t Offset,
                                       DIVariable D, BasicBlock *InsertAtEnd);
  
  /// InsertDbgValueIntrinsic - Insert a new llvm.dbg.value intrinsic call.
  Instruction *InsertDbgValueIntrinsic(llvm::Value *V, uint64_t Offset,
                                       DIVariable D, Instruction *InsertBefore);
  
  // RecordType - Record DIType in a module such that it is not lost even if
  // it is not referenced through debug info anchors.
  void RecordType(DIType T);
  
  private:
  Constant *GetTagConstant(unsigned TAG);
};

/// DebugInfo - This class gathers all debug information during compilation and
/// is responsible for emitting to llvm globals or pass directly to the backend.
class DebugInfo {
private:
  Module *M;                            // The current module.
  DIFactory DebugFactory;               
  const char *CurFullPath;              // Previous location file encountered.
  int CurLineNo;                        // Previous location line# encountered.
  const char *PrevFullPath;             // Previous location file encountered.
  int PrevLineNo;                       // Previous location line# encountered.
  BasicBlock *PrevBB;                   // Last basic block encountered.
  DICompileUnit TheCU;                  // The compile unit.

  // Current GCC lexical block (or enclosing FUNCTION_DECL).
  tree_node *CurrentGCCLexicalBlock;	
  
  std::map<tree_node *, WeakVH > TypeCache;
                                        // Cache of previously constructed 
                                        // Types.
  std::map<tree_node *, WeakVH > SPCache;
                                        // Cache of previously constructed 
                                        // Subprograms.
  std::map<tree_node *, WeakVH> NameSpaceCache;
                                        // Cache of previously constructed name 
                                        // spaces.

  SmallVector<WeakVH, 4> RegionStack;
                                        // Stack to track declarative scopes.
  
  std::map<tree_node *, WeakVH> RegionMap;

  // Starting at the 'desired' BLOCK, recursively walk back to the
  // 'grand' context, and return pushing regions to make 'desired' the
  // current context.  'desired' should be a GCC lexical BLOCK.
  // 'grand' may be a BLOCK or a FUNCTION_DECL, and it's presumed to
  // be an ancestor of 'desired'.
  void push_regions(tree_node *desired, tree_node *grand);

  /// FunctionNames - This is a storage for function names that are
  /// constructed on demand. For example, C++ destructors, C++ operators etc..
  llvm::BumpPtrAllocator FunctionNames;

 public:
  DebugInfo(Module *m);

  /// Initialize - Initialize debug info by creating compile unit for
  /// main_input_filename. This must be invoked after language dependent
  /// initialization is done.
  void Initialize();

  // Accessors.
  void setLocationFile(const char *FullPath) { CurFullPath = FullPath; }
  void setLocationLine(int LineNo)           { CurLineNo = LineNo; }
  
  tree_node *getCurrentLexicalBlock() { return CurrentGCCLexicalBlock; }
  void setCurrentLexicalBlock(tree_node *lb) { CurrentGCCLexicalBlock = lb; }

  // Pop the current region/lexical-block back to 'grand', then push
  // regions to arrive at 'desired'.  This was inspired (cribbed from)
  // by GCC's cfglayout.c:change_scope().
  void change_regions(tree_node *desired, tree_node *grand);

  /// CreateSubprogramFromFnDecl - Constructs the debug code for entering a function -
  /// "llvm.dbg.func.start."
  DISubprogram CreateSubprogramFromFnDecl(tree_node *FnDecl);

  /// EmitFunctionStart - Constructs the debug code for entering a function -
  /// "llvm.dbg.func.start", and pushes it onto the RegionStack.
  void EmitFunctionStart(tree_node *FnDecl);

  /// EmitFunctionEnd - Constructs the debug code for exiting a declarative
  /// region - "llvm.dbg.region.end."
  void EmitFunctionEnd(BasicBlock *CurBB, bool EndFunction);

  /// EmitDeclare - Constructs the debug code for allocation of a new variable.
  /// region - "llvm.dbg.declare."
  void EmitDeclare(tree_node *decl, unsigned Tag, const char *Name,
                   tree_node *type, Value *AI, LLVMBuilder &Builder);

  /// EmitStopPoint - Emit a call to llvm.dbg.stoppoint to indicate a change of 
  /// source line.
  void EmitStopPoint(Function *Fn, BasicBlock *CurBB, LLVMBuilder &Builder);
                     
  /// EmitGlobalVariable - Emit information about a global variable.
  ///
  void EmitGlobalVariable(GlobalVariable *GV, tree_node *decl);

  /// getOrCreateType - Get the type from the cache or create a new type if
  /// necessary.
  DIType getOrCreateType(tree_node *type);

  /// createBasicType - Create BasicType.
  DIType createBasicType(tree_node *type);

  /// createMethodType - Create MethodType.
  DIType createMethodType(tree_node *type);

  /// createPointerType - Create PointerType.
  DIType createPointerType(tree_node *type);

  /// createArrayType - Create ArrayType.
  DIType createArrayType(tree_node *type);

  /// createEnumType - Create EnumType.
  DIType createEnumType(tree_node *type);

  /// createStructType - Create StructType for struct or union or class.
  DIType createStructType(tree_node *type);

  /// createVarinatType - Create variant type or return MainTy.
  DIType createVariantType(tree_node *type, DIType MainTy);

  /// getOrCreateCompileUnit - Create a new compile unit.
  DICompileUnit getOrCreateCompileUnit(const char *FullPath,
                                       bool isMain = false);

  /// getOrCreateFile - Get DIFile descriptor.
  DIFile getOrCreateFile(const char *FullPath);

  /// findRegion - Find tree_node N's region.
  DIDescriptor findRegion(tree_node *n);
  
  /// getOrCreateNameSpace - Get name space descriptor for the tree node.
  DINameSpace getOrCreateNameSpace(tree_node *Node, DIDescriptor Context);

  /// getFunctionName - Get function name for the given FnDecl. If the
  /// name is constructred on demand (e.g. C++ destructor) then the name
  /// is stored on the side.
  StringRef getFunctionName(tree_node *FnDecl);

  /// getCU - Get Compile Unit.
  DICompileUnit getCU() { return TheCU;}

  /// replaceBasicTypesFromPCH - Replace basic type debug info received
  /// from PCH file.
  void replaceBasicTypesFromPCH();
};

} // end namespace llvm

#endif /* LLVM_DEBUG_H */
/* LLVM LOCAL end (ENTIRE FILE!)  */
