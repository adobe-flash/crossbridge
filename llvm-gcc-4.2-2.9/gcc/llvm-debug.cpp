/* LLVM LOCAL begin (ENTIRE FILE!)  */
/* High-level LLVM backend interface 
Copyright (C) 2005 Free Software Foundation, Inc.
Contributed by Jim Laskey (jlaskey@apple.com)

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
// This is a C++ source file that implements the debug information gathering.
//===----------------------------------------------------------------------===//

#include "llvm-debug.h"

#include "llvm-abi.h"
#include "llvm-internal.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"

extern "C" {
#include "langhooks.h"
#include "toplev.h"
#include "flags.h"
#include "tree.h"
#include "version.h"
#include "function.h"
}

using namespace llvm;
using namespace llvm::dwarf;

#ifndef LLVMTESTDEBUG
#define DEBUGASSERT(S) ((void)0)
#else
#define DEBUGASSERT(S) assert(S)
#endif


/// DirectoryAndFile - Extract the directory and file name from a path.  If no
/// directory is specified, then use the source working directory.
static void DirectoryAndFile(const std::string &FullPath,
                             std::string &Directory, std::string &FileName) {
  // Look for the directory slash.
  size_t Slash = FullPath.rfind('/');
  
  // If no slash
  if (Slash == std::string::npos) {
    // The entire path is the file name.
    Directory = "";
    FileName = FullPath;
  } else {
    // Separate the directory from the file name.
    Directory = FullPath.substr(0, Slash);
    FileName = FullPath.substr(Slash + 1);
  }
  
  // If no directory present then use source working directory.
  if (Directory.empty() || Directory[0] != '/') {
    Directory = std::string(get_src_pwd()) + "/" + Directory;
  }
}

/// NodeSizeInBits - Returns the size in bits stored in a tree node regardless
/// of whether the node is a TYPE or DECL.
static uint64_t NodeSizeInBits(tree Node) {
  if (TREE_CODE(Node) == ERROR_MARK) {
    return BITS_PER_WORD;
  } else if (TYPE_P(Node)) {
    if (TYPE_SIZE(Node) == NULL_TREE)
      return 0;
    else if (isInt64(TYPE_SIZE(Node), 1))
      return getINTEGER_CSTVal(TYPE_SIZE(Node));
    else
      return TYPE_ALIGN(Node);
  } else if (DECL_P(Node)) {
    if (DECL_SIZE(Node) == NULL_TREE)
      return 0;
    else if (isInt64(DECL_SIZE(Node), 1))
      return getINTEGER_CSTVal(DECL_SIZE(Node));
    else
      return DECL_ALIGN(Node);
  }
  
  return 0;
}

/// NodeAlignInBits - Returns the alignment in bits stored in a tree node
/// regardless of whether the node is a TYPE or DECL.
static uint64_t NodeAlignInBits(tree Node) {
  if (TREE_CODE(Node) == ERROR_MARK) return BITS_PER_WORD;
  if (TYPE_P(Node)) return TYPE_ALIGN(Node);
  if (DECL_P(Node)) return DECL_ALIGN(Node);
  return BITS_PER_WORD;
}

/// FieldType - Returns the type node of a structure member field.
///
static tree FieldType(tree Field) {
  if (TREE_CODE (Field) == ERROR_MARK) return integer_type_node;
  return getDeclaredType(Field);
}

/// GetNodeName - Returns the name stored in a node regardless of whether the
/// node is a TYPE or DECL.
static StringRef GetNodeName(tree Node) {
  tree Name = NULL;
  
  if (DECL_P(Node)) {
    Name = DECL_NAME(Node);
  } else if (TYPE_P(Node)) {
    Name = TYPE_NAME(Node);
  }

  if (Name) {
    if (TREE_CODE(Name) == IDENTIFIER_NODE) {
      return IDENTIFIER_POINTER(Name);
    } else if (TREE_CODE(Name) == TYPE_DECL && DECL_NAME(Name) &&
               !DECL_IGNORED_P(Name)) {
      return StringRef(IDENTIFIER_POINTER(DECL_NAME(Name)));
    }
  }
  
  return StringRef();
}

/// GetNodeLocation - Returns the location stored in a node  regardless of
/// whether the node is a TYPE or DECL.  UseStub is true if we should consider
/// the type stub as the actually location (ignored in struct/unions/enums.)
static expanded_location GetNodeLocation(tree Node, bool UseStub = true) {
  expanded_location Location = { NULL, 0 };

  if (Node == NULL_TREE)
    return Location;

  tree Name = NULL;
  
  if (DECL_P(Node)) {
    Name = DECL_NAME(Node);
  } else if (TYPE_P(Node)) {
    Name = TYPE_NAME(Node);
  }
  
  if (Name) {
    if (TYPE_STUB_DECL(Name)) {
      tree Stub = TYPE_STUB_DECL(Name);
      Location = expand_location(DECL_SOURCE_LOCATION(Stub));
    } else if (DECL_P(Name)) {
      Location = expand_location(DECL_SOURCE_LOCATION(Name));
    }
  }
  
  if (!Location.line) {
    if (UseStub && TYPE_STUB_DECL(Node)) {
      tree Stub = TYPE_STUB_DECL(Node);
      Location = expand_location(DECL_SOURCE_LOCATION(Stub));
    } else if (DECL_P(Node)) {
      Location = expand_location(DECL_SOURCE_LOCATION(Node));
    }
  }
  
  return Location;
}

static StringRef getLinkageName(tree Node) {

  // Use llvm value name as linkage name if it is available.
  if (DECL_LLVM_SET_P(Node)) {
    Value *V = DECL_LLVM(Node);
    return V->getName();
  }

  tree decl_name = DECL_NAME(Node);
  if (decl_name != NULL && IDENTIFIER_POINTER (decl_name) != NULL) {
    if (TREE_PUBLIC(Node) &&
        DECL_ASSEMBLER_NAME(Node) != DECL_NAME(Node) && 
        !DECL_ABSTRACT(Node)) {
      return StringRef(IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(Node)));
    } 
  }
  return StringRef();
}

DebugInfo::DebugInfo(Module *m)
: M(m)
, DebugFactory(*m)
, CurFullPath("")
, CurLineNo(0)
, PrevFullPath("")
, PrevLineNo(0)
, PrevBB(NULL)
, CurrentGCCLexicalBlock(NULL)
, RegionStack()
{}

/// isCopyOrDestroyHelper - Returns boolean indicating if FnDecl is for
/// one of the compiler-generated "helper" functions for Apple Blocks
/// (a copy helper or a destroy helper).  Such functions should not have
/// debug line table entries.
bool isCopyOrDestroyHelper (tree FnDecl) {
  const char *name = IDENTIFIER_POINTER(DECL_NAME(FnDecl));

  if (!BLOCK_SYNTHESIZED_FUNC(FnDecl))
    return false;

  if (strstr(name, "_copy_helper_block_")
      || strstr(name, "_destroy_helper_block_"))
    return true;
  else
    return false;
}

/// getFunctionName - Get function name for the given FnDecl. If the
/// name is constructred on demand (e.g. C++ destructor) then the name
/// is stored on the side.
StringRef DebugInfo::getFunctionName(tree FnDecl) {
  StringRef FnNodeName = GetNodeName(FnDecl);
  // Use dwarf_name to construct function names. In C++ this is used to
  // create human readable destructor names.
  StringRef FnName = lang_hooks.dwarf_name(FnDecl, 0);
  if (FnNodeName.equals(FnName))
    return FnNodeName;

  // Use name returned by dwarf_name. It is in a temp. storage so make a 
  // copy first.
  char *StrPtr = FunctionNames.Allocate<char>(FnName.size() + 1);
  strncpy(StrPtr, FnName.data(), FnName.size());
  StrPtr[FnName.size()] = 0;
  return StringRef(StrPtr);
}

// Starting at the 'desired' BLOCK, recursively walk back to the
// 'grand' context, and return pushing regions to make 'desired' the
// current context.  'desired' should be a GCC lexical BLOCK, and
// 'grand' should be an ancestor; it may be a BLOCK or a
// FUNCTION_DECL.
void DebugInfo::push_regions(tree desired, tree grand) {
  assert (grand && "'grand' BLOCK is NULL?");
  assert (desired &&
          "'desired' BLOCK is NULL; is grand BLOCK really a parent of desired BLOCK?");
  assert ((TREE_CODE(desired) == BLOCK || TREE_CODE(desired) == FUNCTION_DECL) &&
          "expected 'desired' to be a GCC BLOCK or FUNCTION_DECL");
  assert ((TREE_CODE(grand) == BLOCK || TREE_CODE(grand) == FUNCTION_DECL) &&
          "expected 'grand' to be a GCC BLOCK or FUNCTION_DECL");
  if (grand != desired)
    push_regions(BLOCK_SUPERCONTEXT(desired), grand);
  // FIXME: push_regions is currently never called with desired ==
  // grand, but it should be fixed so nothing weird happens if they're
  // equal.
  llvm::DIDescriptor D = findRegion(desired);
  llvm::MDNode *DN = D;
  RegionStack.push_back(DN);
}

// Pop the current region/lexical-block back to 'grand', then push
// regions to arrive at 'desired'.  This was inspired (cribbed from)
// by GCC's cfglayout.c:change_scope().
void DebugInfo::change_regions(tree desired, tree grand) {
  tree current_lexical_block = getCurrentLexicalBlock(), t;
  // FIXME: change_regions is currently never called with desired ==
  // grand, but it should be fixed so nothing weird happens if they're
  // equal.
  while (current_lexical_block != grand) {
    assert(BLOCK_SUPERCONTEXT(getCurrentLexicalBlock()) &&
           "lost BLOCK context!");
    current_lexical_block = BLOCK_SUPERCONTEXT(current_lexical_block);
    RegionStack.pop_back();
  }
  DebugInfo::push_regions(desired, grand);
  setCurrentLexicalBlock(desired);
}

/// functionPrototyped - Return true if function is prototyped.
static bool functionPrototyped(tree FnDecl, unsigned Lang) {
  if ((Lang == DW_LANG_C89 || Lang == DW_LANG_ObjC)
      || TYPE_ARG_TYPES (TREE_TYPE (FnDecl)) != NULL)
    return true;
  return false;
}
/// CreateSubprogramFromFnDecl - Constructs the debug code for
/// entering a function - "llvm.dbg.func.start."
DISubprogram DebugInfo::CreateSubprogramFromFnDecl(tree FnDecl) {
  DISubprogram SPDecl;
  bool SPDeclIsSet = false;
  // True if we're currently generating LLVM for this function.
  bool definition = llvm_set_decl_p(FnDecl);
  DIType FNType = getOrCreateType(TREE_TYPE(FnDecl));

  std::map<tree_node *, WeakVH >::iterator I = SPCache.find(FnDecl);
  if (I != SPCache.end()) {
    SPDecl = DISubprogram(cast<MDNode>(I->second));
    SPDeclIsSet = true;
    // If we've already created the defining instance, OR this
    // invocation won't create the defining instance, return what we
    // already have.
    if (SPDecl.isDefinition() || !definition)
      return SPDecl;
  }

  bool ArtificialFnWithAbstractOrigin = false;
  // If this artificial function has abstract origin then put this function
  // at module scope. The abstract copy will be placed in appropriate region.
  if (DECL_ARTIFICIAL (FnDecl)
      && DECL_ABSTRACT_ORIGIN (FnDecl)
      && DECL_ABSTRACT_ORIGIN (FnDecl) != FnDecl)
    ArtificialFnWithAbstractOrigin = true;

  DIDescriptor SPContext = ArtificialFnWithAbstractOrigin ?
    getOrCreateFile(main_input_filename) :
    findRegion (DECL_CONTEXT(FnDecl));

  // Declare block_invoke functions at file scope for GDB.
  if (BLOCK_SYNTHESIZED_FUNC(FnDecl))
    SPContext = findRegion(NULL_TREE);

  // Creating context may have triggered creation of this SP descriptor. So
  // check the cache again.
  if (!SPDeclIsSet) {
    I = SPCache.find(FnDecl);
    if (I != SPCache.end()) {
      SPDecl = DISubprogram(cast<MDNode>(I->second));
      DISubprogram SP = 
        DebugFactory.CreateSubprogramDefinition(SPDecl);
      SPDecl->replaceAllUsesWith(SP);

      // Push function on region stack.
      RegionStack.push_back(WeakVH(SP));
      RegionMap[FnDecl] = WeakVH(SP);
      return SP;
    }
  }
  // Gather location information.
  expanded_location Loc = GetNodeLocation(FnDecl, false);
  // If the name isn't public, omit the linkage name.  Adding a
  // linkage name to a class method can confuse GDB.
  StringRef LinkageName = TREE_PUBLIC(FnDecl) ?
    getLinkageName(FnDecl) : StringRef();

  unsigned lineno = LOCATION_LINE(Loc);
  if (isCopyOrDestroyHelper(FnDecl))
    lineno = 0;

  unsigned Virtuality = 0;
  unsigned VIndex = 0;
  DIType ContainingType;
  if (DECL_VINDEX (FnDecl)) {
    if (host_integerp (DECL_VINDEX (FnDecl), 0))
      VIndex = tree_low_cst (DECL_VINDEX (FnDecl), 0);
    Virtuality = dwarf::DW_VIRTUALITY_virtual;
    ContainingType = getOrCreateType(DECL_CONTEXT (FnDecl));
  }

  StringRef FnName = getFunctionName(FnDecl);
  // If the Function * hasn't been created yet, use a bogus value for
  // the debug internal linkage bit.
  bool hasInternalLinkage = true;
  Function *Fn = 0;
  if (GET_DECL_LLVM_INDEX(FnDecl)) {
    Fn = cast<Function>DECL_LLVM(FnDecl);
    if (Fn)
      hasInternalLinkage = Fn->hasInternalLinkage();
  }
  unsigned Flags = 0;
  if (DECL_ARTIFICIAL (FnDecl))
    Flags |= llvm::DIDescriptor::FlagArtificial;
  else if (functionPrototyped(FnDecl, TheCU.getLanguage()))
    Flags |= llvm::DIDescriptor::FlagPrototyped;

  DISubprogram SP = 
    DebugFactory.CreateSubprogram(SPContext,
                                  FnName, FnName,
                                  LinkageName,
                                  getOrCreateFile(Loc.file), lineno,
                                  FNType,
                                  hasInternalLinkage,
                                  definition,
                                  Virtuality, VIndex, ContainingType,
                                  Flags, optimize, Fn);

  SPCache[FnDecl] = WeakVH(SP);
  RegionMap[FnDecl] = WeakVH(SP);
  if (SPDeclIsSet && SPDecl != SP)
    SPDecl->replaceAllUsesWith(SP);
  return SP;
}

/// EmitFunctionStart - Constructs the debug code for entering a function.
void DebugInfo::EmitFunctionStart(tree FnDecl) {
  setCurrentLexicalBlock(FnDecl);
  DISubprogram SP = CreateSubprogramFromFnDecl(FnDecl);
  // Push function on region stack.
  RegionStack.push_back(WeakVH(SP));
}

/// getOrCreateNameSpace - Get name space descriptor for the tree node.
DINameSpace DebugInfo::getOrCreateNameSpace(tree Node, DIDescriptor Context) {
  std::map<tree_node *, WeakVH >::iterator I = 
    NameSpaceCache.find(Node);
  if (I != NameSpaceCache.end())
    return DINameSpace(cast<MDNode>(I->second));

  expanded_location Loc = GetNodeLocation(Node, false);
  DINameSpace DNS =
    DebugFactory.CreateNameSpace(Context, GetNodeName(Node),
                                 getOrCreateFile(Loc.file), Loc.line);

  NameSpaceCache[Node] = WeakVH(DNS);
  return DNS;
}

/// variablesDeclaredAtThisLevel - yield true if this block, or any
/// sibling thereof, declares any variables or types.  Expectes a
/// BLOCK or FUNCTION_DECL tree.
// GDB Kludge
static bool variablesDeclaredAtThisLevel(tree blk) {
  if (TREE_CODE(blk) == FUNCTION_DECL)
    return true;
  assert(TREE_CODE(blk) == BLOCK && "expected FUNCTION_DECL or BLOCK tree");
  tree supercontext = BLOCK_SUPERCONTEXT(blk);
  if (!supercontext)	// External scope.  Pretend it's the same as module scope.
    return false;
  tree step = TREE_CODE(supercontext) == BLOCK ?
    BLOCK_SUBBLOCKS(supercontext) : DECL_INITIAL(supercontext);
  for ( ; step && TREE_CODE(step) == BLOCK; step = TREE_CHAIN(step))
    if (BLOCK_VARS(step))
      return true;
  return false;
}

/// supercontextWithDecls - walk up the BLOCKS tree, return an
/// ancestor BLOCK that has vars, or sibling BLOCKs that do.  May
/// return the FUNCTION_DECL.
// GDB Kludge
static tree supercontextWithDecls(tree blk) {
  assert(TREE_CODE(blk) == BLOCK && "expected BLOCK");
  tree step = BLOCK_SUPERCONTEXT(blk);
  if (!step)    // No parent BLOCKs to search.
    return blk;
  while (step && TREE_CODE(step) == BLOCK && BLOCK_SUPERCONTEXT(step) &&
         !variablesDeclaredAtThisLevel(step))
    step = BLOCK_SUPERCONTEXT(step);
  return step;
}

/// nonemptySibling - true if the given BLOCK has a sibling that is
/// not empty (declares types and/or vars).
// GDB Kludge
static bool nonemptySibling(tree blk) {
  if (TREE_CODE(blk) == FUNCTION_DECL)
    return true;
  assert(TREE_CODE(blk) == BLOCK && "expected FUNCTION_DECL or BLOCK tree");
  tree supercontext = BLOCK_SUPERCONTEXT(blk);
  if (!supercontext)	// External scope.  Pretend it's the same as module scope.
    return false;
  tree step = TREE_CODE(supercontext) == BLOCK ?
    BLOCK_SUBBLOCKS(supercontext) : DECL_INITIAL(supercontext);
  for ( ; step; step = TREE_CHAIN(step))
    if (step != blk && BLOCK_VARS(step))
      return true;
  return false;
}

/// findRegion - Find the region (context) of a GCC tree.
DIDescriptor DebugInfo::findRegion(tree exp) {
  if (exp == NULL_TREE)
    return getOrCreateFile(main_input_filename);

  tree Node = exp;
  location_t *p_locus = 0;
  tree_code code = TREE_CODE(exp);
  enum tree_code_class tree_cc = TREE_CODE_CLASS(code);
  switch (tree_cc) {
  case tcc_declaration:  /* A decl node */
    p_locus = &DECL_SOURCE_LOCATION(exp);
    break;

  case tcc_expression:  /* an expression */
  case tcc_comparison:  /* a comparison expression */
  case tcc_unary:  /* a unary arithmetic expression */
  case tcc_binary:  /* a binary arithmetic expression */
    Node = TREE_BLOCK(exp);
    p_locus = EXPR_LOCUS(exp);
    break;

  case tcc_exceptional:
    switch (code) {
    case BLOCK:
      p_locus = &BLOCK_SOURCE_LOCATION(Node);
      break;
    default:
      gcc_unreachable ();
    }
    break;
  default:
    break;
  }

  std::map<tree_node *, WeakVH>::iterator I = RegionMap.find(Node);
  if (I != RegionMap.end())
    if (MDNode *R = dyn_cast_or_null<MDNode>(&*I->second))
      return DIDescriptor(R);

  if (TYPE_P (Node)) {
    DIType Ty = getOrCreateType(Node);
    return DIDescriptor(Ty);
  } else if (DECL_P (Node)) {
    switch (TREE_CODE(Node)) {
    default:
      /// What kind of DECL is this?
      return findRegion (DECL_CONTEXT (Node));
    case NAMESPACE_DECL: {
      DIDescriptor NSContext = findRegion(DECL_CONTEXT(Node));
      DINameSpace NS = getOrCreateNameSpace(Node, NSContext);
      return DIDescriptor(NS);
    }
    case FUNCTION_DECL: {
      DISubprogram SP = CreateSubprogramFromFnDecl(Node);
      return SP;
    }
    }
  } else if (TREE_CODE(Node) == BLOCK) {
    // Recursively establish ancestor scopes.
    DIDescriptor context = findRegion(BLOCK_SUPERCONTEXT(Node));
    // If we don't have a location, use the last-seen info.
    unsigned int line;
    const char *fullpath;
    if (LOCATION_FILE(*p_locus) == (char*)0) {
      fullpath = CurFullPath;
      line = CurLineNo;
    } else {
      fullpath = LOCATION_FILE(*p_locus);
      line = LOCATION_LINE(*p_locus);
    }
    DIFile F(getOrCreateFile(fullpath));
    DILexicalBlock lexical_block = 
      DebugFactory.CreateLexicalBlock(context, F, line, 0U);
    RegionMap[Node] = WeakVH(lexical_block);
    return DIDescriptor(lexical_block);
  }

  // Otherwise main compile unit covers everything.
  return getOrCreateFile(main_input_filename);
}

/// EmitFunctionEnd - Pop the region stack and reset current lexical block.
void DebugInfo::EmitFunctionEnd(BasicBlock *CurBB, bool EndFunction) {
  assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");
  RegionStack.pop_back();
  // Blocks get erased; clearing these is needed for determinism, and also
  // a good idea if the next function gets inlined.
  if (EndFunction) {
    PrevBB = NULL;
    PrevLineNo = 0;
    PrevFullPath = NULL;
  }
  setCurrentLexicalBlock(NULL_TREE);
}

/// EmitDeclare - Constructs the debug code for allocation of a new variable.
void DebugInfo::EmitDeclare(tree decl, unsigned Tag, const char *Name,
                            tree type, Value *AI, LLVMBuilder &Builder) {

  // Ignore compiler generated temporaries.
  if (DECL_IGNORED_P(decl))
    return;

  assert(!RegionStack.empty() && "Region stack mismatch, stack empty!");

  expanded_location Loc = GetNodeLocation(decl, false);

  // Construct variable.
  DIScope VarScope = DIScope(cast<MDNode>(RegionStack.back()));
  DIType Ty = getOrCreateType(type);
  if (!Ty && TREE_CODE(type) == OFFSET_TYPE)
    Ty = createPointerType(TREE_TYPE(type));
  if (DECL_ARTIFICIAL (decl))
      Ty = DebugFactory.CreateArtificialType(Ty);
  // If type info is not available then do not emit debug info for this var.
  if (!Ty)
    return;
  llvm::DIVariable D =
    DebugFactory.CreateVariable(Tag, VarScope,
                                Name, getOrCreateFile(Loc.file),
                                Loc.line, Ty, optimize);

  Instruction *Call = 
    DebugFactory.InsertDeclare(AI, D, Builder.GetInsertBlock());
  
  Call->setDebugLoc(DebugLoc::get(Loc.line, 0, VarScope));
}


/// isPartOfAppleBlockPrologue - Return boolean indicating if the line number
/// passed in is part of the prologue of an Apple Block function.  This assumes
/// the line number passed in belongs to the "current" function.
bool isPartOfAppleBlockPrologue (unsigned lineno) {
  if (!cfun  || !cfun->decl)
    return false;
  
  // In an earlier part of gcc, code that sets up Apple Block by-reference
  // variables at the beginning of the function (which should be part of the
  // prologue but isn't), is assigned a source location line of one before the
  // function decl.  We check for that here.
  if (BLOCK_SYNTHESIZED_FUNC(cfun->decl)) {
    int fn_decl_line = DECL_SOURCE_LINE(cfun->decl);
    if (lineno == (unsigned)(fn_decl_line - 1))
      return true;
    else
      return false;
  }
  
  return false;
}

/// EmitStopPoint - Set current source location. 
void DebugInfo::EmitStopPoint(Function *Fn, BasicBlock *CurBB,
                              LLVMBuilder &Builder) {
  // Don't bother if things are the same as last time.
  if (PrevLineNo == CurLineNo &&
      PrevBB == CurBB &&
      (PrevFullPath == CurFullPath ||
       !strcmp(PrevFullPath, CurFullPath))) return;
  if (!CurFullPath[0] || CurLineNo == 0) return;
  
  // Update last state.
  PrevFullPath = CurFullPath;
  PrevLineNo = CurLineNo;
  PrevBB = CurBB;

  // Don't set/allow source line breakpoints in Apple Block prologue code
  // or in Apple Block helper functions.
  if (!isPartOfAppleBlockPrologue(CurLineNo)
      && !isCopyOrDestroyHelper(cfun->decl)) {
    
    if (RegionStack.empty())
      return;
    MDNode *Scope = cast<MDNode>(RegionStack.back());
    Builder.SetCurrentDebugLocation(DebugLoc::get(CurLineNo,0/*col*/,Scope));
  }
}

/// EmitGlobalVariable - Emit information about a global variable.
///
void DebugInfo::EmitGlobalVariable(GlobalVariable *GV, tree decl) {
  if (DECL_ARTIFICIAL(decl) || DECL_IGNORED_P(decl))
    return;
  // Gather location information.
  expanded_location Loc = expand_location(DECL_SOURCE_LOCATION(decl));
  DIType TyD = getOrCreateType(TREE_TYPE(decl));
  StringRef DispName = GV->getName();
  if (DECL_NAME(decl)) {
    if (IDENTIFIER_POINTER(DECL_NAME(decl)))
      DispName = IDENTIFIER_POINTER(DECL_NAME(decl));
  }
  StringRef LinkageName;
  // The gdb does not expect linkage names for function local statics.
  if (DECL_CONTEXT (decl))
    if (TREE_CODE (DECL_CONTEXT (decl)) != FUNCTION_DECL)
      LinkageName = GV->getName();
  DIDescriptor Context;
  if (DECL_CONTEXT(decl)) 
    Context = findRegion(DECL_CONTEXT(decl));
  else
    Context = getOrCreateFile(Loc.file);
  DebugFactory.CreateGlobalVariable(Context,
                                    DispName, DispName, LinkageName,
                                    getOrCreateFile(Loc.file), Loc.line,
                                    TyD, GV->hasInternalLinkage(),
                                    true/*definition*/, GV);
}

/// createBasicType - Create BasicType.
DIType DebugInfo::createBasicType(tree type) {

  StringRef TypeName = GetNodeName(type);
  uint64_t Size = NodeSizeInBits(type);
  uint64_t Align = NodeAlignInBits(type);

  unsigned Encoding = 0;
  
  switch (TREE_CODE(type)) {
  case INTEGER_TYPE:
    if (TYPE_STRING_FLAG (type)) {
      if (TYPE_UNSIGNED (type))
        Encoding = DW_ATE_unsigned_char;
      else
        Encoding = DW_ATE_signed_char;
    }
    else if (TYPE_UNSIGNED (type))
      Encoding = DW_ATE_unsigned;
    else
      Encoding = DW_ATE_signed;
    break;
  case REAL_TYPE:
    Encoding = DW_ATE_float;
    break;
  case COMPLEX_TYPE:
    Encoding = TREE_CODE(TREE_TYPE(type)) == REAL_TYPE ?
      DW_ATE_complex_float : DW_ATE_lo_user;
    break;
  case BOOLEAN_TYPE:
    Encoding = DW_ATE_boolean;
    break;
  default: { 
    DEBUGASSERT(0 && "Basic type case missing");
    Encoding = DW_ATE_signed;
    Size = BITS_PER_WORD;
    Align = BITS_PER_WORD;
    break;
  }
  }

  DIBasicType BTy =  
    DebugFactory.CreateBasicType(getOrCreateFile(main_input_filename),
                                 TypeName, 
                                 getOrCreateFile(main_input_filename),
                                 0, Size, Align,
                                 0, 0, Encoding);

  if (TheDebugInfo && flag_pch_file) {
    NamedMDNode *NMD = TheModule->getOrInsertNamedMetadata("llvm.dbg.pch.bt");
    NMD->addOperand(BTy);
  }

  return BTy;
}

/// replaceBasicTypesFromPCH - Replace basic type debug info received
/// from PCH file.
void DebugInfo::replaceBasicTypesFromPCH() {
  NamedMDNode *NMD = TheModule->getOrInsertNamedMetadata("llvm.dbg.pch.bt");
  for (int i = 0, e = NMD->getNumOperands(); i != e; ++i) {
    DIBasicType HeaderBTy(NMD->getOperand(i));
    MDNode *NewBTy = NULL;
    NewBTy = DebugFactory.CreateBasicType(getOrCreateFile(main_input_filename),
                                          HeaderBTy.getName(),
                                          getOrCreateFile(main_input_filename),
                                          0, HeaderBTy.getSizeInBits(), 
                                          HeaderBTy.getAlignInBits(),
                                          0, 0, HeaderBTy.getEncoding());
    MDNode *HBTyNode = HeaderBTy;
    HBTyNode->replaceAllUsesWith(NewBTy);
  }
}

/// isArtificialArgumentType - Return true if arg_type represents artificial,
/// i.e. "this" in c++, argument.
static bool isArtificialArgumentType(tree arg_type, tree method_type) {
  if (TREE_CODE (method_type) != METHOD_TYPE) return false;
  if (TREE_CODE (arg_type) != POINTER_TYPE) return false;
  if (TREE_TYPE (arg_type) == TYPE_METHOD_BASETYPE (method_type))
    return true;
  if (TYPE_MAIN_VARIANT (TREE_TYPE (arg_type))
      && TYPE_MAIN_VARIANT (TREE_TYPE (arg_type)) != TREE_TYPE (arg_type)
      && (TYPE_MAIN_VARIANT (TREE_TYPE (arg_type))
          == TYPE_METHOD_BASETYPE (method_type)))
    return true;
  return false;
}

/// createMethodType - Create MethodType.
DIType DebugInfo::createMethodType(tree type) {

  // Create a  place holder type first. The may be used as a context
  // for the argument types.
  llvm::DIType FwdType = DebugFactory.CreateTemporaryType();
  llvm::MDNode *FTN = FwdType;
  llvm::TrackingVH<llvm::MDNode> FwdTypeNode = FTN;
  TypeCache[type] = WeakVH(FwdType);
  // Push the struct on region stack.
  RegionStack.push_back(WeakVH(FwdType));
  RegionMap[type] = WeakVH(FwdType);
  
  llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;
  
  // Add the result type at least.
  EltTys.push_back(getOrCreateType(TREE_TYPE(type)));
  
  // Set up remainder of arguments.
  bool ProcessedFirstArg = false;
  for (tree arg = TYPE_ARG_TYPES(type); arg; arg = TREE_CHAIN(arg)) {
    tree formal_type = TREE_VALUE(arg);
    if (formal_type == void_type_node) break;
    llvm::DIType FormalType = getOrCreateType(formal_type);
    if (!ProcessedFirstArg && isArtificialArgumentType(formal_type, type)) {
      DIType AFormalType = DebugFactory.CreateArtificialType(FormalType);
      EltTys.push_back(AFormalType);
    } else
      EltTys.push_back(FormalType);
    if (!ProcessedFirstArg)
      ProcessedFirstArg = true;
  }
  
  llvm::DIArray EltTypeArray =
    DebugFactory.GetOrCreateArray(EltTys.data(), EltTys.size());

  RegionStack.pop_back();
  std::map<tree_node *, WeakVH>::iterator RI = RegionMap.find(type);
  if (RI != RegionMap.end())
    RegionMap.erase(RI);

  llvm::DIType RealType =
    DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_subroutine_type,
                                     findRegion(TYPE_CONTEXT(type)), 
                                     StringRef(),
                                     getOrCreateFile(main_input_filename),
                                     0, 0, 0, 0, 0,
                                     llvm::DIType(), EltTypeArray);

  // Now that we have a real decl for the struct, replace anything using the
  // old decl with the new one.  This will recursively update the debug info.
  llvm::DIType(FwdTypeNode).replaceAllUsesWith(RealType);

  return RealType;
}

/// createPointerType - Create PointerType.
DIType DebugInfo::createPointerType(tree type) {

  DIType FromTy = getOrCreateType(TREE_TYPE(type));
  // type* and type&
  // FIXME: Should BLOCK_POINTER_TYP have its own DW_TAG?
  unsigned Tag = TREE_CODE(type) == REFERENCE_TYPE ?
    DW_TAG_reference_type: DW_TAG_pointer_type;
  unsigned Flags = 0;
  if (type_is_block_byref_struct(type))
    Flags |= llvm::DIDescriptor::FlagBlockByrefStruct;

  // Check if this pointer type has a name.
  if (tree TyName = TYPE_NAME(type)) 
    if (TREE_CODE(TyName) == TYPE_DECL && !DECL_ORIGINAL_TYPE(TyName)) {
      expanded_location TypeNameLoc = GetNodeLocation(TyName);
      DIType Ty = 
        DebugFactory.CreateDerivedType(Tag, findRegion(DECL_CONTEXT(TyName)),
                                       GetNodeName(TyName), 
                                       getOrCreateFile(TypeNameLoc.file),
                                       TypeNameLoc.line,
                                       0 /*size*/,
                                       0 /*align*/,
                                       0 /*offset */, 
                                       0 /*flags*/, 
                                       FromTy);
      TypeCache[TyName] = WeakVH(Ty);
      return Ty;
    }
  
  StringRef PName = FromTy.getName();
  DIType PTy = 
    DebugFactory.CreateDerivedType(Tag, findRegion(TYPE_CONTEXT(type)),
                                   Tag == DW_TAG_pointer_type ? 
                                   StringRef() : PName,
                                   getOrCreateFile(main_input_filename),
                                   0 /*line no*/, 
                                   NodeSizeInBits(type),
                                   NodeAlignInBits(type),
                                   0 /*offset */, 
                                   Flags, 
                                   FromTy);
  return PTy;
}

/// createArrayType - Create ArrayType.
DIType DebugInfo::createArrayType(tree type) {

  // type[n][m]...[p]
  if (TREE_CODE (type) == ARRAY_TYPE
      && TYPE_STRING_FLAG(type) && TREE_CODE(TREE_TYPE(type)) == INTEGER_TYPE){
    DEBUGASSERT(0 && "Don't support pascal strings");
    return DIType();
  }
  
  unsigned Tag = 0;
  
  if (TREE_CODE(type) == VECTOR_TYPE) {
    Tag = DW_TAG_vector_type;
    type = TREE_TYPE (TYPE_FIELDS (TYPE_DEBUG_REPRESENTATION_TYPE (type)));
  }
  else
    Tag = DW_TAG_array_type;
  
  // Add the dimensions of the array.  FIXME: This loses CV qualifiers from
  // interior arrays, do we care?  Why aren't nested arrays represented the
  // obvious/recursive way?
  llvm::SmallVector<llvm::DIDescriptor, 8> Subscripts;
  
  // There will be ARRAY_TYPE nodes for each rank.  Followed by the derived
  // type.
  tree atype = type;
  tree EltTy = TREE_TYPE(atype);
  for (; TREE_CODE(atype) == ARRAY_TYPE; 
       atype = TREE_TYPE(atype)) {
    tree Domain = TYPE_DOMAIN(atype);
    if (Domain) {
      // FIXME - handle dynamic ranges
      tree MinValue = TYPE_MIN_VALUE(Domain);
      tree MaxValue = TYPE_MAX_VALUE(Domain);
      uint64_t Low = 0;
      uint64_t Hi = 0;
      if (MinValue && isInt64(MinValue, 0))
        Low = getINTEGER_CSTVal(MinValue);
      if (MaxValue && isInt64(MaxValue, 0))
        Hi = getINTEGER_CSTVal(MaxValue);
      Subscripts.push_back(DebugFactory.GetOrCreateSubrange(Low, Hi));
    }
    EltTy = TREE_TYPE(atype);
  }
  
  llvm::DIArray SubscriptArray =
    DebugFactory.GetOrCreateArray(Subscripts.data(), Subscripts.size());
  expanded_location Loc = GetNodeLocation(type);
  return DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_array_type,
                                          findRegion(TYPE_CONTEXT(type)), 
                                          StringRef(),
                                          getOrCreateFile(Loc.file), 0, 
                                          NodeSizeInBits(type), 
                                          NodeAlignInBits(type), 0, 0,
                                          getOrCreateType(EltTy),
                                          SubscriptArray);
}

/// createEnumType - Create EnumType.
DIType DebugInfo::createEnumType(tree type) {
  // enum { a, b, ..., z };
  llvm::SmallVector<llvm::DIDescriptor, 32> Elements;
  
  if (TYPE_SIZE(type)) {
    for (tree Link = TYPE_VALUES(type); Link; Link = TREE_CHAIN(Link)) {
      tree EnumValue = TREE_VALUE(Link);
      int64_t Value = getINTEGER_CSTVal(EnumValue);
      const char *EnumName = IDENTIFIER_POINTER(TREE_PURPOSE(Link));
      Elements.push_back(DebugFactory.CreateEnumerator(EnumName, Value));
    }
  }
  
  llvm::DIArray EltArray =
    DebugFactory.GetOrCreateArray(Elements.data(), Elements.size());
  
  expanded_location Loc = { NULL, 0 };
  if (TYPE_SIZE(type)) 
    // Incomplete enums do not  have any location info.
    Loc = GetNodeLocation(TREE_CHAIN(type), false);

  return DebugFactory.CreateCompositeType(llvm::dwarf::DW_TAG_enumeration_type,
                                          findRegion(TYPE_CONTEXT(type)), 
                                          GetNodeName(type), 
                                          getOrCreateFile(Loc.file), 
                                          Loc.line,
                                          NodeSizeInBits(type), 
                                          NodeAlignInBits(type), 0, 0,
                                          llvm::DIType(), EltArray);
}

/// createStructType - Create StructType for struct or union or class.
DIType DebugInfo::createStructType(tree type) {

  // struct { a; b; ... z; }; | union { a; b; ... z; };
  unsigned Tag = TREE_CODE(type) == RECORD_TYPE ? DW_TAG_structure_type :
    DW_TAG_union_type;
  
  unsigned RunTimeLang = 0;
  if (TYPE_LANG_SPECIFIC (type)
      && lang_hooks.types.is_runtime_specific_type (type))
    {
      unsigned CULang = TheCU.getLanguage();
      switch (CULang) {
      case DW_LANG_ObjC_plus_plus :
        RunTimeLang = DW_LANG_ObjC_plus_plus;
        break;
      case DW_LANG_ObjC :
        RunTimeLang = DW_LANG_ObjC;
        break;
      case DW_LANG_C_plus_plus :
        RunTimeLang = DW_LANG_C_plus_plus;
        break;
      default:
        break;
      }
    }
    
  // Records and classes and unions can all be recursive.  To handle them,
  // we first generate a debug descriptor for the struct as a forward 
  // declaration. Then (if it is a definition) we go through and get debug 
  // info for all of its members.  Finally, we create a descriptor for the
  // complete type (which may refer to the forward decl if the struct is 
  // recursive) and replace all  uses of the forward declaration with the 
  // final definition. 
  expanded_location Loc = GetNodeLocation(TREE_CHAIN(type), false);
  unsigned SFlags = 0;
  if (TYPE_BLOCK_IMPL_STRUCT(type))
    SFlags |= llvm::DIDescriptor::FlagAppleBlock;
  if (type_is_block_byref_struct(type))
    SFlags |= llvm::DIDescriptor::FlagBlockByrefStruct;
  DIDescriptor TyContext =  findRegion(TYPE_CONTEXT(type));

  // Check if this type is created while creating context information 
  // descriptor. 
  std::map<tree_node *, WeakVH >::iterator I = TypeCache.find(type);
  if (I != TypeCache.end())
    if (MDNode *TN = dyn_cast_or_null<MDNode>(&*I->second))
      return DIType(TN);
  
  // forward declaration, 
  if (TYPE_SIZE(type) == 0) {
    llvm::DICompositeType FwdDecl =
      DebugFactory.CreateCompositeType(Tag, 
                                       TyContext,
                                       GetNodeName(type),
                                       getOrCreateFile(Loc.file), 
                                       Loc.line, 
                                       0, 0, 0,
                                       SFlags | llvm::DIDescriptor::FlagFwdDecl,
                                       llvm::DIType(), llvm::DIArray(),
                                       RunTimeLang);
    return FwdDecl;
  }
  
  llvm::DIType FwdDecl = DebugFactory.CreateTemporaryType();
  
  // Insert into the TypeCache so that recursive uses will find it.
  llvm::MDNode *FDN = FwdDecl;
  llvm::TrackingVH<llvm::MDNode> FwdDeclNode = FDN;
  TypeCache[type] =  WeakVH(FwdDecl);

  // Push the struct on region stack.
  RegionStack.push_back(WeakVH(FwdDecl));
  RegionMap[type] = WeakVH(FwdDecl);
  
  // Convert all the elements.
  llvm::SmallVector<llvm::DIDescriptor, 16> EltTys;
  
  if (tree binfo = TYPE_BINFO(type)) {
    VEC(tree,gc) *accesses = BINFO_BASE_ACCESSES (binfo);

    for (unsigned i = 0, e = BINFO_N_BASE_BINFOS(binfo); i != e; ++i) {
      tree BInfo = BINFO_BASE_BINFO(binfo, i);
      tree BInfoType = BINFO_TYPE (BInfo);
      DIType BaseClass = getOrCreateType(BInfoType);
      unsigned BFlags = 0;
      if (BINFO_VIRTUAL_P (BInfo))
        BFlags = llvm::DIDescriptor::FlagVirtual;
      if (accesses) {
        tree access = VEC_index (tree, accesses, i);
        if (access == access_protected_node)
          BFlags |= llvm::DIDescriptor::FlagProtected;
        else if (access == access_private_node)
          BFlags |= llvm::DIDescriptor::FlagPrivate;
      }

      // Check for zero BINFO_OFFSET. 
      // FIXME : Is this correct ?
      unsigned Offset = BINFO_OFFSET(BInfo) ? 
	getINTEGER_CSTVal(BINFO_OFFSET(BInfo))*8 : 0;

      if (BINFO_VIRTUAL_P (BInfo))
        Offset = 0 - getINTEGER_CSTVal(BINFO_VPTR_FIELD (BInfo));
      // FIXME : name, size, align etc...
      DIType DTy = 
        DebugFactory.CreateDerivedType(DW_TAG_inheritance, 
                                       findRegion(TYPE_CONTEXT(type)), StringRef(),
                                       getOrCreateFile(Loc.file), 0, 0, 0,
                                       Offset, BFlags, BaseClass);
      EltTys.push_back(DTy);
    }
  }
  
  // Now add members of this class.
  for (tree Member = TYPE_FIELDS(type); Member;
       Member = TREE_CHAIN(Member)) {
    // Should we skip.
    if (DECL_P(Member) && DECL_IGNORED_P(Member)) continue;

    // Get the location of the member.
    expanded_location MemLoc = GetNodeLocation(Member, false);

    if (TREE_CODE(Member) != FIELD_DECL)
      // otherwise is a static variable, whose debug info is emitted
      // when through EmitGlobalVariable().
      continue;
      
    if (DECL_FIELD_OFFSET(Member) == 0 ||
        TREE_CODE(DECL_FIELD_OFFSET(Member)) != INTEGER_CST)
      // FIXME: field with variable position, skip it for now.
      continue;
    
    /* Ignore nameless fields.  */
    if (DECL_NAME (Member) == NULL_TREE
        && !(TREE_CODE (TREE_TYPE (Member)) == UNION_TYPE
             || TREE_CODE (TREE_TYPE (Member)) == RECORD_TYPE))
      continue;
    
    // Field type is the declared type of the field.
    tree FieldNodeType = FieldType(Member);
    DIType MemberType = getOrCreateType(FieldNodeType);
    StringRef MemberName = GetNodeName(Member);
    unsigned MFlags = 0;
    if (TREE_PROTECTED(Member))
      MFlags = llvm::DIDescriptor::FlagProtected;
    else if (TREE_PRIVATE(Member))
      MFlags = llvm::DIDescriptor::FlagPrivate;
    
    DIType DTy =
      DebugFactory.CreateDerivedType(DW_TAG_member, 
                                     findRegion(DECL_CONTEXT(Member)),
                                     MemberName, 
                                     getOrCreateFile(MemLoc.file),
                                     MemLoc.line, NodeSizeInBits(Member),
                                     NodeAlignInBits(FieldNodeType),
                                     int_bit_position(Member), 
                                     MFlags, MemberType);
    EltTys.push_back(DTy);
  }
  
  for (tree Member = TYPE_METHODS(type); Member;
       Member = TREE_CHAIN(Member)) {
    
    if (DECL_ABSTRACT_ORIGIN (Member)) continue;
    // Ignore unused aritificial members.
    if (DECL_ARTIFICIAL (Member) && !TREE_USED (Member)) continue;
    // In C++, TEMPLATE_DECLs are marked Ignored, and should be.
    if (DECL_P (Member) && DECL_IGNORED_P (Member)) continue;

    std::map<tree_node *, WeakVH >::iterator I = SPCache.find(Member);
    if (I != SPCache.end())
      EltTys.push_back(DISubprogram(cast<MDNode>(I->second)));
    else {
      // Get the location of the member.
      expanded_location MemLoc = GetNodeLocation(Member, false);
      StringRef MemberName = getFunctionName(Member);
      StringRef LinkageName = getLinkageName(Member);
      DIType SPTy = getOrCreateType(TREE_TYPE(Member));
      unsigned Virtuality = 0;
      unsigned VIndex = 0;
      DIType ContainingType;
      if (DECL_VINDEX (Member)) {
        if (host_integerp (DECL_VINDEX (Member), 0))
          VIndex = tree_low_cst (DECL_VINDEX (Member), 0);
        Virtuality = dwarf::DW_VIRTUALITY_virtual;
        ContainingType = getOrCreateType(DECL_CONTEXT(Member));
      }
      unsigned Flags = 0;
      if (DECL_ARTIFICIAL (Member))
        Flags |= llvm::DIDescriptor::FlagArtificial;
      if (TREE_PROTECTED(Member))
        Flags |= llvm::DIDescriptor::FlagProtected;
      else if (TREE_PRIVATE(Member))
        Flags |= llvm::DIDescriptor::FlagPrivate;
      else if (functionPrototyped(Member, TheCU.getLanguage()))
        Flags |= llvm::DIDescriptor::FlagPrototyped;

      DISubprogram SP = 
        DebugFactory.CreateSubprogram(findRegion(DECL_CONTEXT(Member)), 
                                      MemberName, MemberName,
                                      LinkageName, 
                                      getOrCreateFile(MemLoc.file),
                                      MemLoc.line, SPTy, false, false,
                                      Virtuality, VIndex, ContainingType,
                                      Flags, optimize);
      EltTys.push_back(SP);
      SPCache[Member] = WeakVH(SP);
    }
  }
  
  llvm::DIArray Elements =
    DebugFactory.GetOrCreateArray(EltTys.data(), EltTys.size());

  RegionStack.pop_back();
  std::map<tree_node *, WeakVH>::iterator RI = RegionMap.find(type);
  if (RI != RegionMap.end())
    RegionMap.erase(RI);

  llvm::DIType ContainingType;
  if (TYPE_VFIELD (type)) {
    tree vtype = DECL_FCONTEXT (TYPE_VFIELD (type));
    ContainingType = getOrCreateType(vtype);
  }
  llvm::DICompositeType RealDecl =
    DebugFactory.CreateCompositeType(Tag, findRegion(TYPE_CONTEXT(type)),
                                     GetNodeName(type),
                                     getOrCreateFile(Loc.file),
                                     Loc.line, 
                                     NodeSizeInBits(type), NodeAlignInBits(type),
                                     0, SFlags, llvm::DIType(), Elements,
                                     RunTimeLang, ContainingType);
  RegionMap[type] = WeakVH(RealDecl);

  // Now that we have a real decl for the struct, replace anything using the
  // old decl with the new one.  This will recursively update the debug info.
  llvm::DIType(FwdDeclNode).replaceAllUsesWith(RealDecl);

  return RealDecl;
}

/// createVarinatType - Create variant type or return MainTy.
DIType DebugInfo::createVariantType(tree type, DIType MainTy) {
  
  DIType Ty;
  if (tree TyDef = TYPE_NAME(type)) {
      std::map<tree_node *, WeakVH >::iterator I = TypeCache.find(TyDef);
      if (I != TypeCache.end())
        if (Value *M = I->second)
          return DIType(cast<MDNode>(M));
    if (TREE_CODE(TyDef) == TYPE_DECL &&  DECL_ORIGINAL_TYPE(TyDef)) {
      expanded_location TypeDefLoc = GetNodeLocation(TyDef);
      DIDescriptor Context;
      if (DECL_CONTEXT(TyDef)) 
        Context = findRegion(DECL_CONTEXT(TyDef));
      else
        Context = getOrCreateFile(TypeDefLoc.file);
      Ty = DebugFactory.CreateDerivedType(DW_TAG_typedef, 
                                          Context,
                                          GetNodeName(TyDef), 
                                          getOrCreateFile(TypeDefLoc.file),
                                          TypeDefLoc.line,
                                          0 /*size*/,
                                          0 /*align*/,
                                          0 /*offset */, 
                                          0 /*flags*/, 
                                          MainTy);
      TypeCache[TyDef] = WeakVH(Ty);
      return Ty;
    }
  }

  if (TYPE_VOLATILE(type)) {
    Ty = DebugFactory.CreateDerivedType(DW_TAG_volatile_type, 
                                        findRegion(TYPE_CONTEXT(type)), 
                                        StringRef(),
                                        getOrCreateFile(main_input_filename),
                                        0 /*line no*/, 
                                        NodeSizeInBits(type),
                                        NodeAlignInBits(type),
                                        0 /*offset */, 
                                        0 /* flags */, 
                                        MainTy);
    MainTy = Ty;
  }

  if (TYPE_READONLY(type)) 
    Ty =  DebugFactory.CreateDerivedType(DW_TAG_const_type, 
                                         findRegion(TYPE_CONTEXT(type)), 
                                         StringRef(),
                                         getOrCreateFile(main_input_filename),
                                         0 /*line no*/, 
                                         NodeSizeInBits(type),
                                         NodeAlignInBits(type),
                                         0 /*offset */, 
                                         0 /* flags */, 
                                         MainTy);
  
  if (TYPE_VOLATILE(type) || TYPE_READONLY(type)) {
    TypeCache[type] = WeakVH(Ty);
    return Ty;
  }

  // If, for some reason, main type varaint type is seen then use it.
  return MainTy;
}

/// getOrCreateType - Get the type from the cache or create a new type if
/// necessary.
DIType DebugInfo::getOrCreateType(tree type) {
  DEBUGASSERT(type != NULL_TREE && type != error_mark_node &&
              "Not a type.");
  if (type == NULL_TREE || type == error_mark_node) return DIType();

  // Should only be void if a pointer/reference/return type.  Returning NULL
  // allows the caller to produce a non-derived type.
  if (TREE_CODE(type) == VOID_TYPE) return DIType();
  
  // Check to see if the compile unit already has created this type.
  std::map<tree_node *, WeakVH >::iterator I = TypeCache.find(type);
  if (I != TypeCache.end())
    if (Value *M = I->second)
      return DIType(cast<MDNode>(M));

  DIType MainTy;
  if (type != TYPE_MAIN_VARIANT(type) && TYPE_MAIN_VARIANT(type))
    MainTy = getOrCreateType(TYPE_MAIN_VARIANT(type));

  DIType Ty = createVariantType(type, MainTy);
  if (Ty.isValid())
    return Ty;

  // Work out details of type.
  switch (TREE_CODE(type)) {
    case ERROR_MARK:
    case LANG_TYPE:
    case TRANSLATION_UNIT_DECL:
    default: {
      DEBUGASSERT(0 && "Unsupported type");
      return DIType();
    }
    
    case POINTER_TYPE:
    case REFERENCE_TYPE:
      // Do not cache pointer type. The pointer may point to forward declared
      // struct.
      return createPointerType(type);
      break;

    case BLOCK_POINTER_TYPE: {
      DEBUGASSERT (generic_block_literal_struct_type && 
                   "Generic struct type for Blocks is missing!");
      tree tmp_type = build_pointer_type(generic_block_literal_struct_type);
      Ty = createPointerType(tmp_type);
      break;
    }

    case OFFSET_TYPE: {
      // gen_type_die(TYPE_OFFSET_BASETYPE(type), context_die);
      // gen_type_die(TREE_TYPE(type), context_die);
      // gen_ptr_to_mbr_type_die(type, context_die);
      // PR 7104
      break;
    }

    case FUNCTION_TYPE:
    case METHOD_TYPE: 
      Ty = createMethodType(type);
      break;
      
    case VECTOR_TYPE:
    case ARRAY_TYPE: 
      Ty = createArrayType(type);
      break;
    
    case ENUMERAL_TYPE: 
      Ty = createEnumType(type);
      break;
    
    case RECORD_TYPE:
    case QUAL_UNION_TYPE:
    case UNION_TYPE: 
      return createStructType(type);
      break;

    case INTEGER_TYPE:
    case REAL_TYPE:   
    case COMPLEX_TYPE:
    case BOOLEAN_TYPE:
      Ty = createBasicType(type);
      break;
  }
  TypeCache[type] = WeakVH(Ty);
  return Ty;
}

/// Initialize - Initialize debug info by creating compile unit for
/// main_input_filename. This must be invoked after language dependent
/// initialization is done.
void DebugInfo::Initialize() {

  // Each input file is encoded as a separate compile unit in LLVM
  // debugging information output. However, many target specific tool chains
  // prefer to encode only one compile unit in an object file. In this 
  // situation, the LLVM code generator will include  debugging information
  // entities in the compile unit that is marked as main compile unit. The 
  // code generator accepts maximum one main compile unit per module. If a
  // module does not contain any main compile unit then the code generator 
  // will emit multiple compile units in the output object file.
  if (!strcmp (main_input_filename, ""))
    TheCU = getOrCreateCompileUnit("<stdin>", true);
  else
    TheCU = getOrCreateCompileUnit(main_input_filename, true);
}

/// getOrCreateCompileUnit - Get the compile unit from the cache or 
/// create a new one if necessary.
DICompileUnit DebugInfo::getOrCreateCompileUnit(const char *FullPath,
                                                bool isMain) {
  if (!FullPath) {
    if (!strcmp (main_input_filename, ""))
      FullPath = "<stdin>";
    else
      FullPath = main_input_filename;
  }

  // Get source file information.
  std::string Directory;
  std::string FileName;
  DirectoryAndFile(FullPath, Directory, FileName);
  
  // Set up Language number.
  unsigned LangTag;
  const std::string LanguageName(lang_hooks.name);
  if (LanguageName == "GNU C")
    LangTag = DW_LANG_C89;
  else if (LanguageName == "GNU C++")
    LangTag = DW_LANG_C_plus_plus;
  else if (LanguageName == "GNU Ada")
    LangTag = DW_LANG_Ada95;
  else if (LanguageName == "GNU F77")
    LangTag = DW_LANG_Fortran77;
  else if (LanguageName == "GNU Pascal")
    LangTag = DW_LANG_Pascal83;
  else if (LanguageName == "GNU Java")
    LangTag = DW_LANG_Java;
  else if (LanguageName == "GNU Objective-C")
    LangTag = DW_LANG_ObjC;
  else if (LanguageName == "GNU Objective-C++") 
    LangTag = DW_LANG_ObjC_plus_plus;
  else
    LangTag = DW_LANG_C89;

  StringRef Flags;
  // Do this only when RC_DEBUG_OPTIONS environment variable is set to
  // a nonempty string. This is intended only for internal Apple use.
  char * debugopt = getenv("RC_DEBUG_OPTIONS");
  if (debugopt && debugopt[0])
    Flags = get_arguments();
  
  // flag_objc_abi represents Objective-C runtime version number. It is zero
  // for all other language.
  unsigned ObjcRunTimeVer = 0;
  if (flag_objc_abi != 0 && flag_objc_abi != -1)
    ObjcRunTimeVer = flag_objc_abi;
  return DebugFactory.CreateCompileUnit(LangTag, FileName.c_str(),
                                        Directory.c_str(),
                                        version_string, isMain,
                                        optimize, Flags,
                                        ObjcRunTimeVer);
}

/// getOrCreateFile - Get DIFile descriptor.
DIFile DebugInfo::getOrCreateFile(const char *FullPath) {
  if (!FullPath) {
    if (!strcmp (main_input_filename, ""))
      FullPath = "<stdin>";
    else
      FullPath = main_input_filename;
  }

  // Get source file information.
  std::string Directory;
  std::string FileName;
  DirectoryAndFile(FullPath, Directory, FileName);
  return DebugFactory.CreateFile(FileName, Directory, TheCU);
}

//===----------------------------------------------------------------------===//
// DIFactory: Basic Helpers
//===----------------------------------------------------------------------===//

DIFactory::DIFactory(Module &m)
  : M(m), VMContext(M.getContext()), DeclareFn(0), ValueFn(0) {}

Constant *DIFactory::GetTagConstant(unsigned TAG) {
  assert((TAG & LLVMDebugVersionMask) == 0 &&
         "Tag too large for debug encoding!");
  return ConstantInt::get(Type::getInt32Ty(VMContext), TAG | LLVMDebugVersion);
}

//===----------------------------------------------------------------------===//
// DIFactory: Primary Constructors
//===----------------------------------------------------------------------===//

/// GetOrCreateArray - Create an descriptor for an array of descriptors.
/// This implicitly uniques the arrays created.
DIArray DIFactory::GetOrCreateArray(DIDescriptor *Tys, unsigned NumTys) {
  if (NumTys == 0) {
    Value *Null = llvm::Constant::getNullValue(Type::getInt32Ty(VMContext));
    return DIArray(MDNode::get(VMContext, &Null, 1));
  }

  SmallVector<Value *, 16> Elts(Tys, Tys+NumTys);
  return DIArray(MDNode::get(VMContext, Elts.data(), Elts.size()));
}

/// GetOrCreateSubrange - Create a descriptor for a value range.  This
/// implicitly uniques the values returned.
DISubrange DIFactory::GetOrCreateSubrange(int64_t Lo, int64_t Hi) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_subrange_type),
    ConstantInt::get(Type::getInt64Ty(VMContext), Lo),
    ConstantInt::get(Type::getInt64Ty(VMContext), Hi)
  };

  return DISubrange(MDNode::get(VMContext, &Elts[0], 3));
}

/// CreateUnspecifiedParameter - Create unspeicified type descriptor
/// for the subroutine type.
DIDescriptor DIFactory::CreateUnspecifiedParameter() {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_unspecified_parameters)
  };
  return DIDescriptor(MDNode::get(VMContext, &Elts[0], 1));
}

/// CreateCompileUnit - Create a new descriptor for the specified compile
/// unit.  Note that this does not unique compile units within the module.
DICompileUnit DIFactory::CreateCompileUnit(unsigned LangID,
                                           StringRef Filename,
                                           StringRef Directory,
                                           StringRef Producer,
                                           bool isMain,
                                           bool isOptimized,
                                           StringRef Flags,
                                           unsigned RunTimeVer) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_compile_unit),
    llvm::Constant::getNullValue(Type::getInt32Ty(VMContext)),
    ConstantInt::get(Type::getInt32Ty(VMContext), LangID),
    MDString::get(VMContext, Filename),
    MDString::get(VMContext, Directory),
    MDString::get(VMContext, Producer),
    ConstantInt::get(Type::getInt1Ty(VMContext), isMain),
    ConstantInt::get(Type::getInt1Ty(VMContext), isOptimized),
    MDString::get(VMContext, Flags),
    ConstantInt::get(Type::getInt32Ty(VMContext), RunTimeVer)
  };

  return DICompileUnit(MDNode::get(VMContext, &Elts[0], 10));
}

/// CreateFile -  Create a new descriptor for the specified file.
DIFile DIFactory::CreateFile(StringRef Filename,
                             StringRef Directory,
                             DICompileUnit CU) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_file_type),
    MDString::get(VMContext, Filename),
    MDString::get(VMContext, Directory),
    CU
  };

  return DIFile(MDNode::get(VMContext, &Elts[0], 4));
}

/// CreateEnumerator - Create a single enumerator value.
DIEnumerator DIFactory::CreateEnumerator(StringRef Name, uint64_t Val){
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_enumerator),
    MDString::get(VMContext, Name),
    ConstantInt::get(Type::getInt64Ty(VMContext), Val)
  };
  return DIEnumerator(MDNode::get(VMContext, &Elts[0], 3));
}


/// CreateBasicType - Create a basic type like int, float, etc.
DIBasicType DIFactory::CreateBasicType(DIDescriptor Context,
                                       StringRef Name,
                                       DIFile F,
                                       unsigned LineNumber,
                                       uint64_t SizeInBits,
                                       uint64_t AlignInBits,
                                       uint64_t OffsetInBits, unsigned Flags,
                                       unsigned Encoding) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_base_type),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    ConstantInt::get(Type::getInt64Ty(VMContext), SizeInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), AlignInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), OffsetInBits),
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    ConstantInt::get(Type::getInt32Ty(VMContext), Encoding)
  };
  return DIBasicType(MDNode::get(VMContext, &Elts[0], 10));
}


/// CreateBasicType - Create a basic type like int, float, etc.
DIBasicType DIFactory::CreateBasicTypeEx(DIDescriptor Context,
                                         StringRef Name,
                                         DIFile F,
                                         unsigned LineNumber,
                                         Constant *SizeInBits,
                                         Constant *AlignInBits,
                                         Constant *OffsetInBits, unsigned Flags,
                                         unsigned Encoding) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_base_type),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    SizeInBits,
    AlignInBits,
    OffsetInBits,
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    ConstantInt::get(Type::getInt32Ty(VMContext), Encoding)
  };
  return DIBasicType(MDNode::get(VMContext, &Elts[0], 10));
}

/// CreateArtificialType - Create a new DIType with "artificial" flag set.
DIType DIFactory::CreateArtificialType(DIType Ty) {
  if (Ty.isArtificial())
    return Ty;

  SmallVector<Value *, 9> Elts;
  MDNode *N = Ty;
  assert (N && "Unexpected input DIType!");
  for (unsigned i = 0, e = N->getNumOperands(); i != e; ++i) {
    if (Value *V = N->getOperand(i))
      Elts.push_back(V);
    else
      Elts.push_back(Constant::getNullValue(Type::getInt32Ty(VMContext)));
  }

  unsigned CurFlags = Ty.getFlags();
  CurFlags = CurFlags | DIType::FlagArtificial;

  // Flags are stored at this slot.
  Elts[8] =  ConstantInt::get(Type::getInt32Ty(VMContext), CurFlags);

  return DIType(MDNode::get(VMContext, Elts.data(), Elts.size()));
}

/// CreateDerivedType - Create a derived type like const qualified type,
/// pointer, typedef, etc.
DIDerivedType DIFactory::CreateDerivedType(unsigned Tag,
                                           DIDescriptor Context,
                                           StringRef Name,
                                           DIFile F,
                                           unsigned LineNumber,
                                           uint64_t SizeInBits,
                                           uint64_t AlignInBits,
                                           uint64_t OffsetInBits,
                                           unsigned Flags,
                                           DIType DerivedFrom) {
  Value *Elts[] = {
    GetTagConstant(Tag),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    ConstantInt::get(Type::getInt64Ty(VMContext), SizeInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), AlignInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), OffsetInBits),
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    DerivedFrom,
  };
  return DIDerivedType(MDNode::get(VMContext, &Elts[0], 10));
}


/// CreateDerivedType - Create a derived type like const qualified type,
/// pointer, typedef, etc.
DIDerivedType DIFactory::CreateDerivedTypeEx(unsigned Tag,
                                             DIDescriptor Context,
                                             StringRef Name,
                                             DIFile F,
                                             unsigned LineNumber,
                                             Constant *SizeInBits,
                                             Constant *AlignInBits,
                                             Constant *OffsetInBits,
                                             unsigned Flags,
                                             DIType DerivedFrom) {
  Value *Elts[] = {
    GetTagConstant(Tag),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    SizeInBits,
    AlignInBits,
    OffsetInBits,
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    DerivedFrom,
  };
  return DIDerivedType(MDNode::get(VMContext, &Elts[0], 10));
}


/// CreateCompositeType - Create a composite type like array, struct, etc.
DICompositeType DIFactory::CreateCompositeType(unsigned Tag,
                                               DIDescriptor Context,
                                               StringRef Name,
                                               DIFile F,
                                               unsigned LineNumber,
                                               uint64_t SizeInBits,
                                               uint64_t AlignInBits,
                                               uint64_t OffsetInBits,
                                               unsigned Flags,
                                               DIType DerivedFrom,
                                               DIArray Elements,
                                               unsigned RuntimeLang,
                                               MDNode *ContainingType) {

  Value *Elts[] = {
    GetTagConstant(Tag),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    ConstantInt::get(Type::getInt64Ty(VMContext), SizeInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), AlignInBits),
    ConstantInt::get(Type::getInt64Ty(VMContext), OffsetInBits),
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    DerivedFrom,
    Elements,
    ConstantInt::get(Type::getInt32Ty(VMContext), RuntimeLang),
    ContainingType
  };

  MDNode *Node = MDNode::get(VMContext, &Elts[0], 13);
  // Create a named metadata so that we do not lose this enum info.
  if (Tag == dwarf::DW_TAG_enumeration_type) {
    NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.enum");
    NMD->addOperand(Node);
  }
  return DICompositeType(Node);
}

/// CreateTemporaryType - Create a temporary forward-declared type.
DIType DIFactory::CreateTemporaryType() {
  // Give the temporary MDNode a tag. It doesn't matter what tag we
  // use here as long as DIType accepts it.
  Value *Elts[] = {
    GetTagConstant(DW_TAG_base_type)
  };
  MDNode *Node = MDNode::getTemporary(VMContext, Elts, array_lengthof(Elts));
  return DIType(Node);
}

/// CreateTemporaryType - Create a temporary forward-declared type.
DIType DIFactory::CreateTemporaryType(DIFile F) {
  // Give the temporary MDNode a tag. It doesn't matter what tag we
  // use here as long as DIType accepts it.
  Value *Elts[] = {
    GetTagConstant(DW_TAG_base_type),
    F.getCompileUnit(),
    NULL,
    F
  };
  MDNode *Node = MDNode::getTemporary(VMContext, Elts, array_lengthof(Elts));
  return DIType(Node);
}

/// CreateCompositeType - Create a composite type like array, struct, etc.
DICompositeType DIFactory::CreateCompositeTypeEx(unsigned Tag,
                                                 DIDescriptor Context,
                                                 StringRef Name,
                                                 DIFile F,
                                                 unsigned LineNumber,
                                                 Constant *SizeInBits,
                                                 Constant *AlignInBits,
                                                 Constant *OffsetInBits,
                                                 unsigned Flags,
                                                 DIType DerivedFrom,
                                                 DIArray Elements,
                                                 unsigned RuntimeLang,
                                                 MDNode *ContainingType) {
  Value *Elts[] = {
    GetTagConstant(Tag),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNumber),
    SizeInBits,
    AlignInBits,
    OffsetInBits,
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    DerivedFrom,
    Elements,
    ConstantInt::get(Type::getInt32Ty(VMContext), RuntimeLang),
    ContainingType
  };
  MDNode *Node = MDNode::get(VMContext, &Elts[0], 13);
  // Create a named metadata so that we do not lose this enum info.
  if (Tag == dwarf::DW_TAG_enumeration_type) {
    NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.enum");
    NMD->addOperand(Node);
  }
  return DICompositeType(Node);
}


/// CreateSubprogram - Create a new descriptor for the specified subprogram.
/// See comments in DISubprogram for descriptions of these fields.  This
/// method does not unique the generated descriptors.
DISubprogram DIFactory::CreateSubprogram(DIDescriptor Context,
                                         StringRef Name,
                                         StringRef DisplayName,
                                         StringRef LinkageName,
                                         DIFile F,
                                         unsigned LineNo, DIType Ty,
                                         bool isLocalToUnit,
                                         bool isDefinition,
                                         unsigned VK, unsigned VIndex,
                                         DIType ContainingType,
                                         unsigned Flags,
                                         bool isOptimized,
                                         Function *Fn) {

  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_subprogram),
    llvm::Constant::getNullValue(Type::getInt32Ty(VMContext)),
    Context,
    MDString::get(VMContext, Name),
    MDString::get(VMContext, DisplayName),
    MDString::get(VMContext, LinkageName),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    Ty,
    ConstantInt::get(Type::getInt1Ty(VMContext), isLocalToUnit),
    ConstantInt::get(Type::getInt1Ty(VMContext), isDefinition),
    ConstantInt::get(Type::getInt32Ty(VMContext), (unsigned)VK),
    ConstantInt::get(Type::getInt32Ty(VMContext), VIndex),
    ContainingType,
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags),
    ConstantInt::get(Type::getInt1Ty(VMContext), isOptimized),
    Fn
  };
  MDNode *Node = MDNode::get(VMContext, &Elts[0], 17);

  // Create a named metadata so that we do not lose this mdnode.
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.sp");
  NMD->addOperand(Node);
  return DISubprogram(Node);
}

/// CreateSubprogramDefinition - Create new subprogram descriptor for the
/// given declaration.
DISubprogram DIFactory::CreateSubprogramDefinition(DISubprogram &SPDeclaration){
  if (SPDeclaration.isDefinition())
    return DISubprogram(SPDeclaration);

  MDNode *DeclNode = SPDeclaration;
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_subprogram),
    llvm::Constant::getNullValue(Type::getInt32Ty(VMContext)),
    DeclNode->getOperand(2), // Context
    DeclNode->getOperand(3), // Name
    DeclNode->getOperand(4), // DisplayName
    DeclNode->getOperand(5), // LinkageName
    DeclNode->getOperand(6), // CompileUnit
    DeclNode->getOperand(7), // LineNo
    DeclNode->getOperand(8), // Type
    DeclNode->getOperand(9), // isLocalToUnit
    ConstantInt::get(Type::getInt1Ty(VMContext), true),
    DeclNode->getOperand(11), // Virtuality
    DeclNode->getOperand(12), // VIndex
    DeclNode->getOperand(13), // Containting Type
    DeclNode->getOperand(14), // Flags
    DeclNode->getOperand(15), // isOptimized
    SPDeclaration.getFunction()
  };
  MDNode *Node =MDNode::get(VMContext, &Elts[0], 16);

  // Create a named metadata so that we do not lose this mdnode.
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.sp");
  NMD->addOperand(Node);
  return DISubprogram(Node);
}

/// CreateGlobalVariable - Create a new descriptor for the specified global.
DIGlobalVariable
DIFactory::CreateGlobalVariable(DIDescriptor Context, StringRef Name,
                                StringRef DisplayName,
                                StringRef LinkageName,
                                DIFile F,
                                unsigned LineNo, DIType Ty,bool isLocalToUnit,
                                bool isDefinition, llvm::GlobalVariable *Val) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_variable),
    llvm::Constant::getNullValue(Type::getInt32Ty(VMContext)),
    Context,
    MDString::get(VMContext, Name),
    MDString::get(VMContext, DisplayName),
    MDString::get(VMContext, LinkageName),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    Ty,
    ConstantInt::get(Type::getInt1Ty(VMContext), isLocalToUnit),
    ConstantInt::get(Type::getInt1Ty(VMContext), isDefinition),
    Val
  };

  Value *const *Vs = &Elts[0];
  MDNode *Node = MDNode::get(VMContext,Vs, 12);

  // Create a named metadata so that we do not lose this mdnode.
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.gv");
  NMD->addOperand(Node);

  return DIGlobalVariable(Node);
}

/// CreateGlobalVariable - Create a new descriptor for the specified constant.
DIGlobalVariable
DIFactory::CreateGlobalVariable(DIDescriptor Context, StringRef Name,
                                StringRef DisplayName,
                                StringRef LinkageName,
                                DIFile F,
                                unsigned LineNo, DIType Ty,bool isLocalToUnit,
                                bool isDefinition, llvm::Constant *Val) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_variable),
    llvm::Constant::getNullValue(Type::getInt32Ty(VMContext)),
    Context,
    MDString::get(VMContext, Name),
    MDString::get(VMContext, DisplayName),
    MDString::get(VMContext, LinkageName),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    Ty,
    ConstantInt::get(Type::getInt1Ty(VMContext), isLocalToUnit),
    ConstantInt::get(Type::getInt1Ty(VMContext), isDefinition),
    Val
  };

  Value *const *Vs = &Elts[0];
  MDNode *Node = MDNode::get(VMContext,Vs, 12);

  // Create a named metadata so that we do not lose this mdnode.
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.gv");
  NMD->addOperand(Node);

  return DIGlobalVariable(Node);
}

/// CreateVariable - Create a new descriptor for the specified variable.
DIVariable DIFactory::CreateVariable(unsigned Tag, DIDescriptor Context,
                                     StringRef Name,
                                     DIFile F,
                                     unsigned LineNo,
                                     DIType Ty, bool AlwaysPreserve,
                                     unsigned Flags) {
  Value *Elts[] = {
    GetTagConstant(Tag),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    Ty,
    ConstantInt::get(Type::getInt32Ty(VMContext), Flags)
  };
  MDNode *Node = MDNode::get(VMContext, &Elts[0], 7);
  if (AlwaysPreserve) {
    // The optimizer may remove local variable. If there is an interest
    // to preserve variable info in such situation then stash it in a
    // named mdnode.
    DISubprogram Fn(getDISubprogram(Context));
    StringRef FName = "fn";
    if (Fn.getFunction())
      FName = Fn.getFunction()->getName();
    char One = '\1';
    if (FName.startswith(StringRef(&One, 1)))
      FName = FName.substr(1);


    NamedMDNode *FnLocals = getOrInsertFnSpecificMDNode(M, FName);
    FnLocals->addOperand(Node);
  }
  return DIVariable(Node);
}


/// CreateComplexVariable - Create a new descriptor for the specified variable
/// which has a complex address expression for its address.
DIVariable DIFactory::CreateComplexVariable(unsigned Tag, DIDescriptor Context,
                                            StringRef Name, DIFile F,
                                            unsigned LineNo,
                                            DIType Ty, Value *const *Addr,
                                            unsigned NumAddr) {
  SmallVector<Value *, 15> Elts;
  Elts.push_back(GetTagConstant(Tag));
  Elts.push_back(Context);
  Elts.push_back(MDString::get(VMContext, Name));
  Elts.push_back(F);
  Elts.push_back(ConstantInt::get(Type::getInt32Ty(VMContext), LineNo));
  Elts.push_back(Ty);
  Elts.append(Addr, Addr+NumAddr);

  return DIVariable(MDNode::get(VMContext, Elts.data(), Elts.size()));
}


/// CreateBlock - This creates a descriptor for a lexical block with the
/// specified parent VMContext.
DILexicalBlock DIFactory::CreateLexicalBlock(DIDescriptor Context,
                                             DIFile F, unsigned LineNo,
                                             unsigned Col) {
  // Defeat MDNode uniqing for lexical blocks.
  static unsigned int unique_id = 0;
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_lexical_block),
    Context,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    ConstantInt::get(Type::getInt32Ty(VMContext), Col),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), unique_id++)
  };
  return DILexicalBlock(MDNode::get(VMContext, &Elts[0], 6));
}

/// CreateNameSpace - This creates new descriptor for a namespace
/// with the specified parent context.
DINameSpace DIFactory::CreateNameSpace(DIDescriptor Context, StringRef Name,
                                       DIFile F,
                                       unsigned LineNo) {
  Value *Elts[] = {
    GetTagConstant(dwarf::DW_TAG_namespace),
    Context,
    MDString::get(VMContext, Name),
    F,
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo)
  };
  return DINameSpace(MDNode::get(VMContext, &Elts[0], 5));
}

/// CreateLocation - Creates a debug info location.
DILocation DIFactory::CreateLocation(unsigned LineNo, unsigned ColumnNo,
                                     DIScope S, DILocation OrigLoc) {
  Value *Elts[] = {
    ConstantInt::get(Type::getInt32Ty(VMContext), LineNo),
    ConstantInt::get(Type::getInt32Ty(VMContext), ColumnNo),
    S,
    OrigLoc,
  };
  return DILocation(MDNode::get(VMContext, &Elts[0], 4));
}

//===----------------------------------------------------------------------===//
// DIFactory: Routines for inserting code into a function
//===----------------------------------------------------------------------===//

/// InsertDeclare - Insert a new llvm.dbg.declare intrinsic call.
Instruction *DIFactory::InsertDeclare(Value *Storage, DIVariable D,
                                      Instruction *InsertBefore) {
  assert(Storage && "no storage passed to dbg.declare");
  assert(D.Verify() && "empty DIVariable passed to dbg.declare");
  if (!DeclareFn)
    DeclareFn = Intrinsic::getDeclaration(&M, Intrinsic::dbg_declare);

  Value *Args[] = { MDNode::get(Storage->getContext(), &Storage, 1),
                    D };
  return CallInst::Create(DeclareFn, Args, Args+2, "", InsertBefore);
}

/// InsertDeclare - Insert a new llvm.dbg.declare intrinsic call.
Instruction *DIFactory::InsertDeclare(Value *Storage, DIVariable D,
                                      BasicBlock *InsertAtEnd) {
  assert(Storage && "no storage passed to dbg.declare");
  assert(D.Verify() && "invalid DIVariable passed to dbg.declare");
  if (!DeclareFn)
    DeclareFn = Intrinsic::getDeclaration(&M, Intrinsic::dbg_declare);

  Value *Args[] = { MDNode::get(Storage->getContext(), &Storage, 1),
                    D };

  // If this block already has a terminator then insert this intrinsic
  // before the terminator.
  if (TerminatorInst *T = InsertAtEnd->getTerminator())
    return CallInst::Create(DeclareFn, Args, Args+2, "", T);
  else
    return CallInst::Create(DeclareFn, Args, Args+2, "", InsertAtEnd);}

/// InsertDbgValueIntrinsic - Insert a new llvm.dbg.value intrinsic call.
Instruction *DIFactory::InsertDbgValueIntrinsic(Value *V, uint64_t Offset,
                                                DIVariable D,
                                                Instruction *InsertBefore) {
  assert(V && "no value passed to dbg.value");
  assert(D.Verify() && "invalid DIVariable passed to dbg.value");
  if (!ValueFn)
    ValueFn = Intrinsic::getDeclaration(&M, Intrinsic::dbg_value);

  Value *Args[] = { MDNode::get(V->getContext(), &V, 1),
                    ConstantInt::get(Type::getInt64Ty(V->getContext()), Offset),
                    D };
  return CallInst::Create(ValueFn, Args, Args+3, "", InsertBefore);
}

/// InsertDbgValueIntrinsic - Insert a new llvm.dbg.value intrinsic call.
Instruction *DIFactory::InsertDbgValueIntrinsic(Value *V, uint64_t Offset,
                                                DIVariable D,
                                                BasicBlock *InsertAtEnd) {
  assert(V && "no value passed to dbg.value");
  assert(D.Verify() && "invalid DIVariable passed to dbg.value");
  if (!ValueFn)
    ValueFn = Intrinsic::getDeclaration(&M, Intrinsic::dbg_value);

  Value *Args[] = { MDNode::get(V->getContext(), &V, 1),
                    ConstantInt::get(Type::getInt64Ty(V->getContext()), Offset),
                    D };
  return CallInst::Create(ValueFn, Args, Args+3, "", InsertAtEnd);
}

// RecordType - Record DIType in a module such that it is not lost even if
// it is not referenced through debug info anchors.
void DIFactory::RecordType(DIType T) {
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.dbg.ty");
  NMD->addOperand(T);
}

/* LLVM LOCAL end (ENTIRE FILE!)  */
