/*
 ** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3, or (at your option)
 ** any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

/* -----------------------------------------------------------------------------
 * This file is part of SWIG, which is licensed as a whole under version 3
 * (or any later version) of the GNU General Public License. Some additional
 * terms also apply to certain portions of SWIG. The full details of the SWIG
 * license and copyrights can be found in the LICENSE and COPYRIGHT files
 * included with the SWIG source code as distributed by the SWIG developers
 * and at http://www.swig.org/legal.html.
 *
 * as3.cxx
 *
 * ActionScript3 language module for SWIG.
 * ----------------------------------------------------------------------------- */

#include <stdlib.h>
#include <ctype.h>

#include "llvm/Module.h"
#include "llvm/LLVMContext.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Casting.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "llvm/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetRegistry.h"

#include "clang/Frontend/CodeGenOptions.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/FileSystemOptions.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Basic/FileManager.h"
#include "clang/Lex/ModuleLoader.h"

#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Frontend/Utils.h"

#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"

#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Frontend/FrontendOptions.h"

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Builtins.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/Sema.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Ownership.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"

#include "CodeGen/CGRecordLayout.h"
#include "CodeGen/CodeGenModule.h"

#include "swigmod.h"

extern "C" char *flasccSDKLocation;

extern "C" void LLVMInitializeAVM2TargetInfo();
extern "C" void LLVMInitializeAVM2Target();

namespace {
  
  typedef enum {INT, UINT, FLOAT, COMPOSITE, ARRAY} TypeCategory;
  
  class TypeRegistry {
  public:
    virtual bool hasType(String *type_id) const = 0;
  };
  
  class TypeMemoryLayout;
  typedef llvm::StringMap<TypeMemoryLayout *> TypeMap;
  
  const String *AS_INT = "int";
  const String *AS_UINT = "uint";
  const String *AS_NUMBER = "Number";
  const String *AS_BYTE_ARRAY = "ByteArray";
  const String *BA_RBYTE = "readByte";
  const String *BA_RBYTES = "readBytes";
  const String *BA_RUBYTE = "readUnsignedByte";
  const String *BA_RSHORT = "readShort";
  const String *BA_RUSHORT = "readUnsignedShort";
  const String *BA_RINT = "readInt";
  const String *BA_RUINT = "readUnsignedInt";
  const String *BA_RFLOAT = "readFloat";
  const String *BA_RDOUBLE = "readDouble";
  const String *BA_WBYTE = "writeByte";
  const String *BA_WSHORT = "writeShort";
  const String *BA_WINT = "writeInt";
  const String *BA_WFLOAT = "writeFloat";
  const String *BA_WDOUBLE = "writeDouble";
  
  const String *getASTypeName(const String *type_id) {
    // FIXME: the returned value leaks
    String *ret = NewStringf("%sValue", type_id);
    *Char(ret) = toupper(*Char(ret));
    return ret;
  }
  
  struct MemberDescription {
    uint64_t size;
    unsigned offset;
    TypeCategory category;
    String *name;
    String *c_type_name;
    
    bool canCodeGen(const TypeRegistry &known_types) const {
      switch (category) {
        case UINT:
        case INT:
          return size == 1 || size == 2 || size == 4;
        case FLOAT:
          return size == 4 || size == 8;
        case COMPOSITE:
          return known_types.hasType(c_type_name);
        case ARRAY:
          return true;
        default:
          return false;
      }
    }
    
    const String *getASType() const {
      switch (category) {
        case UINT:
          return AS_UINT;
        case INT:
          return AS_INT;
        case FLOAT:
          return AS_NUMBER;
        case COMPOSITE:
          return getASTypeName(c_type_name);
        case ARRAY:
          return AS_BYTE_ARRAY;
        default:
          return NULL;
      }
    }
    
    bool isUnsigned() const {
      return category == UINT;
    }
    
    const String *getByteArrayReadMethod() const {
      switch (category) {
        case INT:
          switch(size) {
            case 1:
              return BA_RBYTE;
            case 2:
              return BA_RSHORT;
            case 4:
              return BA_RINT;
            default:
              return NULL;
          }
        case UINT:
          switch (size) {
            case 1:
              return BA_RUBYTE;
            case 2:
              return BA_RUSHORT;
            case 4:
              return BA_RUINT;
            default:
              return NULL;
          }
        case FLOAT:
          switch (size) {
            case 4:
              return BA_RFLOAT;
            case 8:
              return BA_RDOUBLE;
            default:
              return NULL;
          }
        default:
          return NULL;
      }
    }
    
    const String *getByteArrayWriteMethod() const {
      switch (category) {
        case INT:
        case UINT:
          switch (size) {
            case 1:
              return BA_WBYTE;
            case 2:
              return BA_WSHORT;
            case 4:
              return BA_WINT;
            default:
              return NULL;
          }
        case FLOAT:
          switch (size) {
            case 4:
              return BA_WFLOAT;
            case 8:
              return BA_WDOUBLE;
            default:
              return NULL;
          }
        default:
          return NULL;
      }
    }
  };
  
  class TypeMemoryLayout {
  private:
    unsigned size;
    String *name;
    llvm::SmallVector<MemberDescription, 8> members;
    
    llvm::SmallVector<unsigned, 8> llvm_fields; // XXX: get rid of this?
    llvm::SmallVector<String *, 8> names;
    llvm::SmallVector<unsigned, 8> offsets;
    llvm::SmallVector<unsigned, 8> sizes;
    llvm::SmallVector<bool, 8> signs;
    
  public:
    TypeMemoryLayout(const clang::RecordDecl &rd,
                     const clang::CodeGen::CGRecordLayout &rl,
                     const llvm::StructLayout &sl,
                     clang::CodeGen::CodeGenTypes &types) {
      size = sl.getSizeInBytes();
      name = NewString(rd.getDeclName().getAsString().c_str());
      //rl.dump();
      
      for (clang::RecordDecl::field_iterator i = rd.field_begin();
           i != rd.field_end(); i++) {
        MemberDescription mem;
        unsigned field = rl.getLLVMFieldNo(*i);
        //llvm_fields.push_back(field);
        //names.push_back(NewString(i->getNameAsString().c_str()));
        mem.name = NewString(i->getNameAsString().c_str());
        mem.offset = sl.getElementOffset(field);
        //offsets.push_back(sl.getElementOffset(field));
        members.push_back(mem);
      }
      
      // This can't be in the previous loop because calling
      // ConvertTypeForMem clobbers the StructLayout.
      int cnt = 0;
      for (clang::RecordDecl::field_iterator i = rd.field_begin();
           i != rd.field_end(); i++, cnt++) {
        // XXX: not sure if this is the best way!
        llvm::Type *memtype = const_cast<llvm::Type*>(types.ConvertTypeForMem(i->getType()));
        uint64_t size = types.getDataLayout().getTypeStoreSize(memtype);
        const clang::Type *type = i->getType().getTypePtr();
        
        if (type->isFloatingType()) {
          members[cnt].category = FLOAT;
        } else if (type->isPointerType()) {
          members[cnt].category = INT;
        } else if (type->isUnsignedIntegerType()) {
          members[cnt].category = UINT;
        } else if (type->isSignedIntegerType()) {
          members[cnt].category = INT;
        } else if (type->isStructureOrClassType() || type->isUnionType()) {
          members[cnt].category = COMPOSITE;
          // FIXME: clean up
          const clang::RecordType *rt = type->getAsStructureType();
          if (!rt) {
            rt = type->getAsUnionType();
          }
          if (rt) {
            members[cnt].c_type_name = NewString(
                                                 rt->getDecl()->getName().str().c_str());
          } else {
            abort();
          }
        } else if (type->isArrayType()) {
          members[cnt].category = ARRAY;
        }
        
        members[cnt].size = size;
      }
    }
    
    ~TypeMemoryLayout() {
      for (int i = 0; i < names.size(); i++) {
        Delete(names[i]);
      }
      Delete(name);
    }
    
    const String *getTypeName() const {
      return name;
    }
    
    unsigned getTypeSize() const {
      return size;
    }
    
    unsigned getNumMembers() const {
      return members.size();
    }
    
    const MemberDescription &getMember(unsigned i) const {
      return members[i];
    }
    
    void dump() const {
      Printf(stderr, "struct %s, %u bytes\n", name, size);
      for (int i = 0; i < getNumMembers(); i++) {
        const MemberDescription &mem = getMember(i);
        Printf(stderr, "\t%s, size %u, offset %u",
               mem.name, mem.size, mem.offset);
        if (mem.isUnsigned()) {
          Printf(stderr, " (unsigned) ");
        }
        Printf(stderr, "\n");
      }
    }
    
    bool canCodeGen(const TypeRegistry &known_types) const {
      for (int i = 0; i < getNumMembers(); i++) {
        if (!getMember(i).canCodeGen(known_types)) {
          return false;
        }
      }
      return true;
    }
    
  };
  
  
  class RecordLocator : public clang::ASTConsumer,
  public clang::RecursiveASTVisitor<RecordLocator>,
  public TypeRegistry, public clang::ModuleLoader
  {
  private:
    typedef clang::RecursiveASTVisitor<RecordLocator> super;
    
    void storeTypeLayout(clang::RecordDecl *rd) {
      clang::CodeGen::CodeGenTypes &cgtypes = cgModule->getTypes();
      clang::SourceLocation loc = rd->getLocStart();
      unsigned line = srcManager->getPresumedLineNumber(loc);
      const char *file = srcManager->getFileEntryForID(
                                                       srcManager->getFileID(loc))->getName();
      llvm::StringRef name = rd->getName();
      rd = rd->getDefinition();
      if (!rd) {
        Printf(stderr, "%s:%u: Can't find definition for %s\n",
               file, line, name.str().c_str());
        return;
      }
      
      const clang::CodeGen::CGRecordLayout *rl =
      &(cgtypes.getCGRecordLayout(rd));
      if (!rl) {
        // Replace any opaque types previously stored with the
        // full definition. The opaque types come from calling
        // ConvertTypeForMem on a type that has a forward
        // declared member of this type.
        cgtypes.UpdateCompletedType(rd);
        rl = &(cgtypes.getCGRecordLayout(rd));
        if (!rl) {
          Printf(stderr, "%s:%u: Insufficient type information for %s\n",
                 file, line, name.str().c_str());
          Printf(stderr, "def on line %u\n",
                 srcManager->getPresumedLineNumber(rd->getLocStart()));
          return;
        }
      }
      llvm::StructType *st = rl->getLLVMType();
      const llvm::StructLayout *sl = td->getStructLayout(st);
      
      TypeMemoryLayout *type_layout =
      new TypeMemoryLayout(*rd, *rl, *sl, cgtypes);
      composite_types[llvm::StringRef(Char(type_layout->getTypeName()))] =
      type_layout;
      //type_layout->dump();
    }
    
    void handleTypedef(clang::TypedefDecl *tpd) {
      const clang::Type *type = tpd->getUnderlyingType().getTypePtr();
      llvm::StringRef typedefname = tpd->getName();
      const clang::RecordType *rt = llvm::dyn_cast<clang::RecordType>(type);
      if (rt) {
        clang::RecordDecl *rd = rt->getDecl();
        llvm::StringRef name = rd->getName();
        TypeMap &map = composite_types;
        if (map.find(name) != map.end()) {
          /*
           std::cerr << "aliased " << typedefname.str() << " to "
           << name.str() << std::endl;
           */
          map[typedefname] = map[name];
        }
      }
    }
    
  public:
    RecordLocator() : clang::ASTConsumer() { }
    virtual ~RecordLocator() { }
    
    virtual bool HandleTopLevelDecl(clang::DeclGroupRef d) {
      clang::DeclGroupRef::iterator it;
      for(it = d.begin(); it != d.end(); it++) {
        clang::RecordDecl *rd = llvm::dyn_cast<clang::RecordDecl>(*it);
        clang::TypedefDecl *tpd = llvm::dyn_cast<clang::TypedefDecl>(*it);
        if (rd) {
          TraverseDecl(rd);
        }
        if (tpd) {
          handleTypedef(tpd);
        }
        
        /*
         clang::NamedDecl *nd = dyn_cast<clang::NamedDecl>(*it);
         if (nd) {
         Printf(stderr, "top decl: %s\n",
         nd->getNameAsString().c_str());
         Printf(stderr, "kind: %s\n",
         nd->getDeclKindName());
         if (!rd) {
         Printf(stderr, "not record\n");
         }
         }
         */
      }
      // TODO: check the ret value
      return true;
    }
    
    virtual bool TraverseRecordDecl(clang::RecordDecl *d) {
      storeTypeLayout(d);
      return super::TraverseRecordDecl(d);
    }
    
    virtual bool TraverseCXXRecordDecl(clang::CXXRecordDecl *d) {
      TraverseRecordDecl(d);
      return super::TraverseCXXRecordDecl(d);
    }
    
    virtual bool hasType(String *type_id) const {
      llvm::StringRef type_str(Char(type_id));
      return composite_types.find(type_str) != composite_types.end();
    }
    
    virtual clang::Module *loadModule(clang::SourceLocation ImportLoc,
                                      clang::ModuleIdPath Path,
                                      clang::Module::NameVisibilityKind Visibility,
                                      bool IsInclusionDirective);
    clang::CodeGen::CodeGenModule *cgModule;
    const llvm::DataLayout *td;
    const clang::SourceManager *srcManager;
    // Hash of typenames to TypeMemoryLayout instances.
    // One entry per struct/union/class in parsed C.
    TypeMap composite_types;
  };
  
  clang::Module *RecordLocator::loadModule(clang::SourceLocation ImportLoc,
                                    clang::ModuleIdPath Path,
                                    clang::Module::NameVisibilityKind Visibility,
                                    bool IsInclusionDirective) {
    // TODO: Need to implement?
    return NULL;
  }
  
  class CMemoryLayout {
  private:
    String *header_file;
    RecordLocator astConsumer;
    
  public:
    typedef enum {C, CXX} Language;
    
    CMemoryLayout(String *in_file) {
      header_file = in_file;
    }
    
    ~CMemoryLayout() {}
    
    int init(Language lang=C) {
      clang::DiagnosticOptions diagnosticOptions;
      clang::TextDiagnosticPrinter *pTextDiagnosticPrinter =
              new clang::TextDiagnosticPrinter(
                                       Verbose ? llvm::outs() : llvm::nulls(),
                                       &diagnosticOptions);
      pTextDiagnosticPrinter->setPrefix("(clang)");
      
      llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diagID(new clang::DiagnosticIDs);
      clang::DiagnosticsEngine *diagEng =
              new clang::DiagnosticsEngine(diagID, &diagnosticOptions);
      diagEng->setClient(pTextDiagnosticPrinter);
      
      clang::LangOptions langopt;
      if (lang == CXX) {
        clang::CompilerInvocation::setLangDefaults(langopt, clang::IK_CXX);
      }
      
      clang::FileSystemOptions fileSystemOptions;
      clang::FileManager fileManager(fileSystemOptions);
      
      clang::SourceManager sourceManager(*diagEng,fileManager);
      
      if (!flasccSDKLocation) {
        return EXIT_FAILURE;
      }
      
      llvm::StringRef sysroot(flasccSDKLocation);
      
      if (Verbose) {
        Printf(stdout, "Clang include directories:\n");
        Printf(stdout, "%s (sysroot)\n", flasccSDKLocation);
      }
      
      llvm::IntrusiveRefCntPtr<clang::HeaderSearchOptions> headerPtr(
          new clang::HeaderSearchOptions(sysroot));

      clang::HeaderSearchOptions headers = *headerPtr;
      List *swig_include_dirs = Swig_add_directory(NULL);
      Iterator i;
      for (i = First(swig_include_dirs); i.item; i = Next(i)) {
        String *path = i.item;
        if (Verbose) {
          Printf(stdout, "%s\n", path);
        }
        headers.AddPath(llvm::StringRef(Char(path)),
                        clang::frontend::Angled,
                        true, // user supplied
                        false, // not framework
                        false); // don't ignore sysroot
      }
      
      clang::TargetOptions targetOptions;
      targetOptions.Triple = "avm2-";
      //targetOptions.Triple = llvm::sys::getHostTriple();
           
      LLVMInitializeAVM2TargetInfo();
      LLVMInitializeAVM2Target();
      
      clang::TargetInfo *pTargetInfo =
      clang::TargetInfo::CreateTargetInfo(
                                          *diagEng,
                                          targetOptions);
      
      if (!pTargetInfo) {
        Printf(stderr, "Couldn't find LLVM target info \"%s\", wrapper "
               "types  unavailable.", targetOptions.Triple.c_str());
        return EXIT_FAILURE;
      }
/*
      std::string errorStr;
      const llvm::Target *target = llvm::TargetRegistry::lookupTarget(
                                        //targetOptions.Triple
                                        "avm2-",
                                        errorStr);
      if (!target) {
        Printf(stderr, "Couldn't find LLVM target \"%s\", wrapper types "
               " unavailable.", targetOptions.Triple.c_str());
        return EXIT_FAILURE;
      }
      llvm::TargetMachine *tm = target->createTargetMachine(
                                                            "avm2-", "");
      
      if (!tm) {
        Printf(stderr, "Couldn't find LLVM target machine \"%s\", wrapper "
               "types  unavailable.", targetOptions.Triple.c_str());
        return EXIT_FAILURE;
      }
      
      const llvm::DataLayout *td = tm->getDataLayout();

      if (!td) {
        Printf(stderr, "Couldn't find LLVM target data \"%s\", wrapper "
               "types  unavailable.", targetOptions.Triple.c_str());
        return EXIT_FAILURE;
      }
*/
      const llvm::DataLayout td(pTargetInfo->getTargetDescription());
      
      llvm::Module llvmModule("my module", llvm::getGlobalContext());
      
      clang::TargetCXXABI cxxabi = pTargetInfo->getCXXABI();
      
      clang::HeaderSearch *headerSearch =
            new clang::HeaderSearch(headerPtr, fileManager,
                                    *diagEng, langopt, pTargetInfo);
      clang::ApplyHeaderSearchOptions(
                                      *headerSearch,
                                      headers,
                                      langopt,
                                      pTargetInfo->getTriple());
      
      llvm::IntrusiveRefCntPtr<clang::PreprocessorOptions>
          preprocessorOptionsPtr(new clang::PreprocessorOptions());
      clang::Preprocessor preprocessor(preprocessorOptionsPtr,
                                       *diagEng,
                                       langopt,
                                       pTargetInfo,
                                       sourceManager,
                                       *headerSearch, astConsumer, /*token cache*/0,/*OwnsHeaderSearch=*/true);
      
      clang::PreprocessorOptions preprocessorOptions = *preprocessorOptionsPtr;
      clang::FrontendOptions frontendOptions;
      clang::InitializePreprocessor(
                                    preprocessor,
                                    preprocessorOptions,
                                    headers,
                                    frontendOptions);
      
      const clang::FileEntry *pFile = fileManager.getFile(Char(header_file));
      if (!pFile) {
        Printf(stderr, "Couldn't find source file \"%s\", wrapper "
               "types  unavailable.", header_file);
        return EXIT_FAILURE;
      }
      
      sourceManager.createMainFileID(pFile);
      
      const clang::TargetInfo &targetInfo = *pTargetInfo;
      clang::IdentifierTable identifierTable(langopt);
      clang::SelectorTable selectorTable;
      clang::Builtin::Context builtinContext;
      clang::ASTContext astContext(
                                   langopt,
                                   sourceManager,
                                   pTargetInfo,
                                   identifierTable,
                                   selectorTable,
                                   builtinContext,
                                   0);
      
      clang::CodeGenOptions codeGenOptions;
      clang::CodeGen::CodeGenModule cgModule(astContext, codeGenOptions,
                                             llvmModule, td, *diagEng);
      
      astConsumer.cgModule = &cgModule;
      astConsumer.td = &td;
      astConsumer.srcManager = &sourceManager;
      
//      clang::Sema sema(
//                       preprocessor,
//                       astContext,
//                       astConsumer);
//      sema.Initialize();
      
      pTextDiagnosticPrinter->BeginSourceFile(langopt, &preprocessor);
      clang::ParseAST(preprocessor, &astConsumer, astContext,
                      false, clang::TU_Complete, 0, true);
      pTextDiagnosticPrinter->EndSourceFile();
      
      return EXIT_SUCCESS;
    }
    
    const TypeMemoryLayout *getMemoryLayout(String *type_name) {
      llvm::StringRef type_name_str(Char(type_name));
      llvm::StringMap<TypeMemoryLayout *> &map =
      astConsumer.composite_types;
      if (map.find(type_name_str) == map.end()) {
        return NULL;
      } else {
        return map[type_name_str];
      }
    }
  };
  
}

// typedef-safe versions of swig's isfunctionpointer and
// functionpointer_decompose utility functions
static bool is_functionpointer(SwigType *t) {
  if (SwigType_istypedef(t)) {
    return is_functionpointer(SwigType_typedef_resolve_all(t));
  } else {
    return SwigType_isfunctionpointer(t);
  }
}

struct functionpointer {
  SwigType *ret_type;
  List *parms;
  // Note: ParmList is actually a hash, not a list. Therefore, this
  // member is not interchangeable with 'parms', above.
  ParmList *parmlist;
};

static struct functionpointer functionpointer_decompose(SwigType *t) {
  struct functionpointer ret;
  if (SwigType_istypedef(t)) {
    t = SwigType_typedef_resolve_all(t);
  }
  SwigType *args = SwigType_functionpointer_decompose(t);
  ret.parms = SwigType_parmlist(args);
  ret.parmlist = SwigType_function_parms(args, NIL);
  
  // normalize argument list: '(void') -> '()'
  if (Len(ret.parms) == 1 && SwigType_type(Getitem(ret.parms, 0)) == T_VOID) {
    assert(ParmList_len(ret.parmlist) == 1);
    Delete(ret.parmlist);
    ret.parmlist = NIL;
    Clear(ret.parms);
  }
  ret.ret_type = t;
  return ret;
}

static String *function_decl(SwigType *ret_type, String *id, String *args) {
  String *ret;
  if (SwigType_isfunctionpointer(ret_type)) {
    String *id_and_args = NewStringf("%s(%s)", id, args);
    ret = SwigType_lstr(ret_type, id_and_args);
    Delete(id_and_args);
  } else {
    String *decl = SwigType_lstr(ret_type, id);
    ret = NewStringf("%s (%s)", decl, args);
    Delete(decl);
  }
  return ret;
}

class AS3 : public Language {
private:
  String *package;
  
  bool validASName(String *p) {
    return p && Len(p) && !Strchr(p, ':');
  }
  
  String *getASOutput() {
    if (getClassName()) {
      return f_proxy_classes;
    } else {
      return f_func_class;
    }
  }
  
  void emitASClassForCType(const TypeMemoryLayout &type, File *outfile) {
    const String *class_name = getASTypeName(type.getTypeName());
    
    // Class, static, and instance variables
    Printf(outfile, "public class %s {\n", class_name);
    Printf(outfile, "\tpublic static const size:int = %u;\n\n",
           type.getTypeSize());
    Printf(outfile, "\tprivate const _ba:ByteArray;\n");
    Printf(outfile, "\tprivate const _offs:int;\n\n");
    
    // Constructor
    Printf(outfile,
           "\tpublic function %s(ba:ByteArray = null, offs:int = 0) {\n",
           class_name);
    Printf(outfile, "\t\tif (!ba) {\n");
    Printf(outfile, "\t\t\tba = new ByteArray();\n");
    Printf(outfile, "\t\t\tba.endian = \"littleEndian\";\n");
    Printf(outfile, "\t\t\tba.length = %u;\n", type.getTypeSize());
    Printf(outfile, "\t\t}\n");
    Printf(outfile, "\t\t_ba = ba;\n");
    Printf(outfile, "\t\t_offs = offs;\n");
    Printf(outfile, "\t}\n\n");
    
    // addressOf
    Printf(outfile, "\tpublic function addressOf():int {\n");
    Printf(outfile, "\t\timport C_Run.ram;\n");
    Printf(outfile, "\t\tif (_ba != ram) {\n");
    Printf(outfile, "\t\t\tthrow new Error(\"Cannot take address of object "
           "not in domainMemory\");\n");
    Printf(outfile, "\t\t}\n");
    Printf(outfile, "\t\treturn _offs;\n");
    Printf(outfile, "\t}\n\n");
    
    // Getters and setters
    int i;
    for (i = 0; i < type.getNumMembers(); i++) {
      const MemberDescription &mem = type.getMember(i);
      const String *memname = mem.name;
      unsigned offset = mem.offset;
      
      Printf(outfile, "\tpublic function get %s():%s {\n",
             memname, mem.getASType());
      if (mem.category == COMPOSITE) {
        Printf(outfile, "\t\treturn new %s(_ba, _offs + %u);\n",
               mem.getASType(), mem.offset);
      } else if (mem.category == ARRAY) {
        Printf(outfile, "\t\tvar ret = new ByteArray();\n");
        Printf(outfile, "\t\tret.endian = \"littleEndian\";\n");
        Printf(outfile, "\t\t_ba.position = _offs + %u;\n", mem.offset);
        Printf(outfile, "\t\t_ba.readBytes(ret, 0, %u);\n", mem.size);
        Printf(outfile, "\t\treturn ret;\n");
      } else {
        Printf(outfile, "\t\t_ba.position = _offs + %u;\n", offset);
        Printf(outfile, "\t\treturn _ba.%s();\n",
               mem.getByteArrayReadMethod());
      }
      Printf(outfile, "\t}\n\n");
      
      Printf(outfile, "\tpublic function set %s(v:%s):void {\n",
             memname, mem.getASType());
      if (mem.category == COMPOSITE) {
        Printf(outfile, "\t\tv.write(_offs + %u, _ba);\n", mem.offset);
      } else if (mem.category == ARRAY) {
        Printf(outfile, "\t\tv.readBytes(_ba, _offs + %u, %u);\n",
               mem.offset, mem.size);
      } else {
        Printf(outfile, "\t\t_ba.position = _offs + %u;\n", offset);
        Printf(outfile, "\t\t_ba.%s(v);\n",
               mem.getByteArrayWriteMethod());
      }
      
      Printf(outfile, "\t}\n\n");
    }
    
    // read and write member functions
    Printf(outfile, "\tpublic function read(ptr:int):void {\n");
    Printf(outfile, "\t\timport C_Run.ram;\n");
    Printf(outfile, "\t\t_ba.position = _offs;\n");
    Printf(outfile, "\t\t_ba.readBytes(ram, ptr, size);\n");
    Printf(outfile, "\t}\n\n");
    
    Printf(outfile,
           "\tpublic function write(ptr:int, ba:ByteArray = null):void {\n");
    Printf(outfile, "\t\timport C_Run.ram;\n");
    Printf(outfile, "\t\tif (!ba) {\n");
    Printf(outfile, "\t\t\tba = C_Run.ram;\n");
    Printf(outfile, "\t\t}\n");
    Printf(outfile, "\t\tba.position = ptr;\n");
    Printf(outfile, "\t\tba.writeBytes(_ba, _offs, size);\n");
    Printf(outfile, "\t}\n");
    
    Printf(outfile, "}\n\n");
  }
  
  String* MungeNSSeparator(String *s) {
    String *str = NewString(s);
    Replaceall(str, "::", "_");
    return str;
  }
  
  /*
   * Emit a C function that wraps an Actionscript Function and handles
   * passing of its arguments and return value on the stack. We use this for
   * creating C function pointers for Actionscript Functions.
   *
   * Adds the name of the wrapper function to the node as "wrap:funcptrwrap".
   */
  void emitFuncPtrWrapper(Node *n, Node *parent, File *outfile) {
    String *name = MungeNSSeparator(Getattr(n, "name"));
    List *parms;
    SwigType *type = Getattr(n, "type");
    SwigType *ret_type;
    
    ret_type = Copy(type);
    struct functionpointer fp = functionpointer_decompose(ret_type);
    ret_type = fp.ret_type;
    parms = fp.parms;
    
    if (!parms) {
      return;
    }
    
    String *wrap_func = NewStringf("_wrap_%s_%s_func_ptr",
                                   MungeNSSeparator(Getattr(parent, "name")), name);
    
    Setattr(n, "wrap:funcptrwrap", wrap_func);
    
    // Emit C wrapper
    Wrapper *f = NewWrapper();
    String *ctypestr = SwigType_str(type, 0);
    
    String *c_args = NewString("");
    String *as_args = NewString("");
    Iterator i;
    
    Parm *p;
    // Make sure that each C parameter has a name and the right type.
    int c = 0;
    for (p = fp.parmlist; p; p = nextSibling(p)) {
      String *arg_name = Swig_cparm_name(p, c++);
      SwigType *type = Getattr(p, "type");
      Append(c_args, SwigType_str(type, arg_name));
      Delete(arg_name);
      
      // Turn references into pointers, as the rest of swig expects
      if (SwigType_isreference(type)) {
        Push(arg_name, "&");
      }
      
      if (nextSibling(p)) {
        Append(c_args, ", ");
      }
    }
    
    Swig_typemap_attach_parms("out", fp.parmlist, f);
    // Emit conversion code for each Actionscript parameter
    for (p = fp.parmlist, c = 0; p; p = Getattr(p, "tmap:out:next"), c++) {
      bool next = Getattr(p, "tmap:out:next");
      if (checkAttribute(p, "tmap:out:numinputs", "0")) {
        continue;
      }
      SwigType *type = Getattr(p, "type");
      assert(type);
      
      String *arg_name = Getattr(p, "lname");
      String *as_arg_name = Swig_cparm_name(NIL, c);
      String *tp = Getattr(p, "tmap:out");
      
      if (is_functionpointer(Getattr(p, "type"))) {
        emitASFunctionWrapper(p, n, f_begin);
        Replaceall(tp, "$wrapper_name",
                   Getattr(p, "wrap:Functionwrap"));
      }
      
      Replaceall(tp, "$result", as_arg_name);
      Printf(f->code, "%s\n", tp);
      Append(as_args, as_arg_name);
      
      Delete(as_arg_name);
      Delete(arg_name);
      
      if (next) {
        Append(as_args, ", ");
      }
    }
    
    Printf(f->code, "inline_as3(\n\"");
    if (Cmp(ret_type, "void")) {
      Wrapper_add_localv(f, "result", SwigType_lstr(ret_type, "result"),
                         NIL);
      Printf(f->code, "var asresult = ");
    }
    Printf(f->code, "f(%s);\"\n", as_args);
    Printf(f->code, ");\n");
    Delete(as_args);
    
    if (Cmp(ret_type, "void")) {
      Swig_save("emitFuncPtrWrapper", n, "type", "tmap:in", NIL);
      Setattr(n, "type", ret_type);
      String *intm = Swig_typemap_lookup("in", n, "result", 0);
      if (is_functionpointer(ret_type)) {
        Node *retnode = NewHash();
        Setattr(retnode, "type", ret_type);
        emitFuncPtrWrapper(retnode, n, outfile);
        Replaceall(intm, "$wrapper_name",
                   Getattr(retnode, "wrap:funcptrwrap"));
        Delete(retnode);
      }
      Swig_restore(n);
      Replaceall(intm, "$input", "asresult");
      Printf(f->code, "%s\n", intm);
      Printf(f->code, "    return result;\n");
    }
    
    // Emit C wrapper
    String *assig = NewStringf("public function %s(f:Function):void",
                               wrap_func);
    
    String *sigattrib = NewStringf(
                                   "__attribute__((annotate(\"as3sig:%s\")))", assig);
    String *func_decl = function_decl(ret_type, wrap_func, c_args);
    Printf(f->def, "%s\n%s {\n", sigattrib, func_decl);
    Delete(func_decl);
    Delete(c_args);
    Delete(sigattrib);
    Delete(assig);
    
    Printf(f->code, "}\n");
    Wrapper_print(f, outfile);
    DelWrapper(f);
  }
  
  /*
   * Emit C code for an Actionscript function that wraps a C function
   * pointer so that it is callable from Actionscript.
   *
   * Adds the name of the wrapper function to the node as "wrap:Functionwrap".
   */
  void emitASFunctionWrapper(Node *n, Node *parent, File *outfile) {
    String *name = MungeNSSeparator(Getattr(n, "name"));
    ParmList *parms;
    SwigType *type = Getattr(n, "type");
    SwigType *ret_type;
    
    ret_type = Copy(type);
    struct functionpointer fp = functionpointer_decompose(ret_type);
    ret_type = fp.ret_type;
    parms = fp.parms;
    
    if (!parms) {
      return;
    }
    
    String *wrap_func = NewStringf("_wrap_%s_%s_Function",
                                   MungeNSSeparator(Getattr(parent, "name")), name);
    
    Setattr(n, "wrap:Functionwrap", wrap_func);
    
    // Emit C wrapper
    Wrapper *f = NewWrapper();
    
    // Emit conversion code (from Actionscript to C)
    Swig_save("as3", n, "parmstr", "parmnames",
              "cleanup_code", "omitted_self", NIL);
    emitWrapperParameters(n, f, fp.parmlist);
    String *as_args = Getattr(n, "parmstr");
    Swig_restore(n);
    
    if (Len(as_args) > 0) {
      Append(as_args, ", ");
    }
    Append(as_args, "funcPtrArg:int");
    
    Wrapper_add_localv(f, "funcPtr", SwigType_lstr(Getattr(n, "type"),
                                                   "funcPtr"), NIL);
    
    String *cresultline;
    String *ccall = Swig_cfunction_call("funcPtr", fp.parmlist);
    Swig_save("emitASFunctionWrapper", n, "type", "tmap:out", NIL);
    Setattr(n, "type", ret_type);
    
    if (Cmp(ret_type, "void")) {
      Wrapper_add_localv(f, "result", SwigType_lstr(ret_type, "result"),
                         NIL);
      Printf(f->code, "inline_nonreentrant_as3(\"%%0 = funcPtrArg;\""
             " : \"=r\"(funcPtr));\n");
      cresultline = Swig_cresult(ret_type, "result", ccall);
      String *outtm = Swig_typemap_lookup("out", n, "result", 0);
      Replaceall(outtm, "$result", "asresult");
      if (is_functionpointer(ret_type)) {
        Node *retnode = NewHash();
        Setattr(retnode, "type", ret_type);
        emitASFunctionWrapper(retnode, n, outfile);
        Replaceall(outtm, "$wrapper_name",
                   Getattr(retnode, "wrap:Functionwrap"));
        Delete(retnode);
      }
      Append(cresultline, "\n");
      Append(cresultline, outtm);
      Append(cresultline, "\nAS3_ReturnAS3Var(asresult);");
    } else {
      Printf(f->code, "__asm__ volatile(\"ESP = ebp; "
             "%%0 = funcPtrArg;\" : \"=r\"(funcPtr));\n");
      cresultline = Copy(ccall);
      Append(cresultline, ";");
    }
    Printf(f->code, "%s\n", cresultline);
    Delete(ccall);
    Delete(cresultline);
    
    String *as_ret_type;
    as_ret_type = Swig_typemap_lookup("astype", n, "", 0);
    
    // Emit AS wrapper
    String *assig = NewStringf("public function %s(%s):%s",
                               wrap_func, as_args, as_ret_type);
    
    Swig_restore(n);
    Delete(as_args);
    
    String *sigattrib = NewStringf(
                                   "__attribute__((annotate(\"as3sig:%s\")))", assig);
    Printf(f->def, "%s\nvoid %s() {\n", sigattrib, wrap_func);
    Delete(sigattrib);
    Delete(assig);
    
    Printf(f->code, "}\n");
    Wrapper_print(f, outfile);
    DelWrapper(f);
    
  }
  
  void emitWrapperParameters(Node *n, Wrapper *f, ParmList *parms) {
    bool in_class = getClassName();
    emit_parameter_variables(parms, f);
    emit_attach_parmmaps(parms, f);
    Swig_typemap_attach_parms("astype", parms, f);
    
    // Convert Actionscript parameters to C
    String *parmstr = NewString("");
    String *parmnames = NewString("");
    String *cleanup_code = NewString("");
    Parm *p;
    bool omitted_self = false;
    for (p = parms; p; p = Getattr(p, "tmap:in:next")) {
      bool next = Getattr(p, "tmap:in:next");
      if (checkAttribute(p, "tmap:in:numinputs", "0")) {
        continue;
      }
      
      String *parmname = Getattr(p, "name");
      parmname  = Swig_name_make(p, 0, Getattr(p, "name"), 0, 0);
      if (!validASName(parmname)) {
        parmname = Getattr(p, "lname");
      }
      
      // The 'this' pointer is special -- replace it with the
      // proxy class memeber that points to the C instance (swigCPtr).
      if (in_class && !is_static && !Cmp("self", parmname)) {
        Append(parmnames, "swigCPtr");
        omitted_self = true;
      } else {
        Append(parmstr, parmname);
        String *astype = Getattr(p, "tmap:astype");
        if (astype) {
          Append(parmstr, ":");
          Append(parmstr, astype);
        }
        if (next) {
          Append(parmstr, ", ");
        }
        Append(parmnames, parmname);
      }
      
      if (next) {
        Append(parmnames, ", ");
      }
      
      String *tp = Getattr(p, "tmap:in");
      Replaceall(tp, "$input", parmname);
      if (is_functionpointer(Getattr(p, "type"))) {
        emitFuncPtrWrapper(p, n, f_begin);
        Replaceall(tp, "$wrapper_name", Getattr(p, "wrap:funcptrwrap"));
      }
      Printf(f->code, "%s\n", tp);
      
      String *parm_cleanup = Getattr(p, "tmap:freearg");
      if (parm_cleanup) {
        Printf(cleanup_code, "%s\n", parm_cleanup);
      }
    }
    
    if (omitted_self) {
      Setattr(n, "omitted_self", "1");
    }
    Setattr(n, "parmstr", parmstr);
    Setattr(n, "parmnames", parmnames);
    Setattr(n, "cleanup_code", cleanup_code);
  }
  
  void emitASType(String *outtp, Node *n) {
    SwigType *type = Getattr(n, "type");
    String *resolved_typename = SwigType_typedef_resolve_all(type);
    Symtab *symtab = Getattr(n, "sym:symtab");
    Node *typenode = Swig_symbol_clookup(resolved_typename, symtab);
    if (typenode) {
      Replaceall(outtp, "$astype", Getattr(typenode, "sym:name"));
    }
  }
  
public:
  
  AS3():package(NIL) {}
  
  virtual void main(int argc, char **argv) {
    SWIG_library_directory("as3");
    for (int i = 1; i < argc; i++) {
      if (argv[i]) {
        if (strcmp(argv[i], "-package") == 0) {
          if (argv[i + 1]) {
            package = NewString("");
            Printf(package, argv[i + 1]);
            if (Len(package) == 0) {
              Delete(package);
              package = NIL;
            }
            Swig_mark_arg(i);
            Swig_mark_arg(i + 1);
            i++;
          } else {
            Swig_arg_error();
          }
        }
      }
    }
    Preprocessor_define("SWIGAS3 1", 0);
    SWIG_config_file("as3.swg");
    SWIG_typemap_lang("as3");
  }
  
  virtual int top(Node *n) {
    // TODO: Solve crash (#17)
    if (!CPlusPlus) {
      mem_layout = new CMemoryLayout(input_file);
      // Always C for the time being
      CMemoryLayout::Language lang =
      CPlusPlus ? CMemoryLayout::CXX : CMemoryLayout::C;
      mem_layout->init(lang);
    }
    
    is_variable = false;
    is_static = false;
    is_constructor = false;
    is_destructor = false;
    class_extends = false;
    
    String *module_name = NewStringf("%s", Getattr(n, "name"));
    String *as_class = NewStringf("%s%s.as", SWIG_output_directory(),
                                  module_name);
    
    String *outfile = Getattr(n, "outfile");
    if (!outfile) {
      Printf(stderr, "Unable to determine outfile\n");
      SWIG_exit(EXIT_FAILURE);
    }
    
    
    open_output_file(&f_module, as_class);
    open_output_file(&f_begin, outfile);
    Swig_register_filebyname("header", f_begin);
    Swig_register_filebyname("runtime", f_begin);
    Swig_register_filebyname("asruntime", f_module);
    
    Swig_banner_target_lang(f_module, "//");
    Swig_banner(f_begin);
    
    f_func_class = NewString("");
    Printf(f_func_class, "public class %s {\n", module_name);
    f_proxy_classes = NewString("");
    f_ctype_class = NewString("");
    
    Language::top(n);
    
    Printf(f_func_class, "}\n");
    if (package) {
      Printf(f_module, "package %s{\n", package);
    } else{
      Printf(f_module, "package {\n");
    }
    Printf(f_module, "import C_Run.*;\n");
    Printf(f_module, "import com.adobe.flascc.swig.*;\n");
    Printf(f_module, "import flash.utils.ByteArray;\n\n");
    Printf(f_module, "%s", f_ctype_class);
    Printf(f_module, "%s", f_func_class);
    Printf(f_module, "%s", f_proxy_classes);
    Printf(f_module, "} /* package */\n");
    
    //Close(f_module);
    //Close(f_begin);
    Delete(f_module);
    Delete(f_begin);
    
    Delete(f_func_class);
    Delete(module_name);
    Delete(as_class);
    
    return SWIG_OK;
  }
  
  virtual int functionWrapper(Node *n) {
    String *name = Getattr(n, "sym:name");
    ParmList *parms = Getattr(n, "parms");
    String *glue_func = Swig_name_wrapper(name);
    bool in_class = getClassName();
    
    Setattr(n, "wrap:name", glue_func);
    
    if (is_variable) {
      String *variable_name;
      if (getClassName()) {
        variable_name = Swig_name_member(
                                         0, getClassPrefix(), sym_name);
      } else { // variables that aren't class memebers
        variable_name = sym_name;
      }
      
      bool is_getter = Cmp(name,
                           Swig_name_set(0, variable_name)) != 0;
      // TODO: name leaks after this copy
      if (is_getter) {
        name = NewStringf("get %s", sym_name);
      } else {
        name = NewStringf("set %s", sym_name);
      }
    } else if (is_destructor) {
      name = NewString("destroy"); // TODO: leaks?
    } else if (is_constructor) {
      name = NewString("create"); // TODO: leaks?
    } else if (in_class) {
      // Don't prepend the class name to this method, just use the
      // the same name as in C.
      name = sym_name;
    }
    
    // Emit C wrapper
    Wrapper *f = NewWrapper();
    Swig_save("as3", n, "parmstr", "parmnames",
              "cleanup_code", "omitted_self", NIL);
    emitWrapperParameters(n, f, parms);
    String *parmstr = Getattr(n, "parmstr");
    String *parmnames = Getattr(n, "parmnames");
    String *cleanup_code = Getattr(n, "cleanup_code");
    String *omitted_self = Getattr(n, "omitted_self");
    Swig_restore(n);
    
    // TODO: remove this hack
    // Need the list of parameters both with and without the implied
    // 'this' pointer for different parts of code gen.
    String *parm_with_self_str;
    if (omitted_self) {
      parm_with_self_str = NewStringf("self%s%s",
                                      (Len(parmstr) > 0) ? ", " : "", parmstr);
    } else {
      parm_with_self_str = Copy(parmstr);
    }
    
    // Determine Actionscript return value from C return value
    SwigType *type = Getattr(n, "type");
    String *ctypestr = SwigType_str(type, 0);
    String *asretstr, *asretstat;
    if (Cmp(ctypestr, "void")) {
      Wrapper_add_localv(f, "result", SwigType_lstr(type, "result"),
                         NIL);
    }
    
    asretstr = Swig_typemap_lookup("astype", n, "", 0);
    if (is_constructor) {
      asretstr = NewStringf(":%s", class_sym_name);
    } else if (asretstr) {
      asretstr = NewStringf(":%s", asretstr);
    } else {
      asretstr = NewString(":*");
    }
    
    if (Cmp(":void", asretstr)) {
      asretstat = NewStringf("return ");
    } else {
      asretstat = NewString("");
    }
    
    // Emit Actionscript wrapper, which calls the C wrapper
    
    // The signature of the Actionscript function available to the user.
    String *static_str = NewString(
                                   (is_static || is_constructor || !in_class) ? "static " : "");
    String *override_str = NewString(
                                     !Getattr(n, "feature:as3:suppressoverride") && (Getattr(n, "override") || (is_destructor && class_extends)) ?
                                     "override " : "");
    String *assig = NewStringf("%spublic %sfunction %s(%s)%s",
                               override_str, static_str, name, parmstr, asretstr);
    Delete(static_str);
    Delete(override_str);
    
    // The Actionscript signature of the C wrapper function
    // (goes in as3sig attribute).
    String *wassig = NewStringf("public function %s(%s)%s",
                                glue_func, parm_with_self_str,
                                (is_constructor) ? ":int" : asretstr);
    Delete(asretstr);
    Delete(parm_with_self_str);
    
    String *as_out = getASOutput();
    
    Printf(as_out, "\t%s {\n", assig);
    if (is_constructor) {
      // TODO: should this go into a typemap?
      Printf(as_out, "\t\tvar obj = new %s();\n", class_sym_name);
      Printf(as_out, "\t\tobj.swigCPtr = %s(%s);\n", glue_func,
             parmnames);
      Printf(as_out, "\t\treturn obj;\n");
    } else {
      Printf(as_out, "\t\t%s%s(%s);\n", asretstat, glue_func, parmnames);
    }
    Printf(as_out, "\t}\n\n");
    
    Delete(asretstat);
    Delete(parmnames);
    Delete(parmstr);
    
    String *as3imports = Getattr(n, "feature:as3:import");
    std::string importstr;
    if(as3imports) {
      char *c = Char(as3imports);
      char *e = NULL;
      while((e = strchr(c, ',')) != NULL) {
        *e = 0;
        importstr += ", annotate(\"as3import:" + std::string(c) + "\")";
        c = e+1;
      }
      importstr += ", annotate(\"as3import:" + std::string(c) + "\")";
    }
    
    String *sigattrib = NewStringf(
                                   "__attribute__((annotate(\"as3sig:%s\")%s))", wassig, importstr.c_str());
    Printf(f->def, "%s\nvoid %s() {\n", sigattrib, glue_func);
    Delete(sigattrib);
    Delete(assig);
    Delete(wassig);
    
    Printf(f->code, "%s", emit_action(n));
    
    Parm *p;
    /* Insert argument output code */
    for (p = parms; p;) {
      String *tm;
      if ((tm = Getattr(p, "tmap:argout"))) {
        Replaceall(tm, "$result", "asresult");
        Replaceall(tm, "$input", Getattr(p, "emit:input"));
        Printv(f->code, tm, "\n", NIL);
        p = Getattr(p, "tmap:argout:next");
      } else {
        p = nextSibling(p);
      }
    }
    
    Printf(f->code, "%s", cleanup_code);
    Delete(cleanup_code);
    
    String *rettp = Swig_typemap_lookup("asreturn", n, "", f);
    String *ret_val;
    String *outtp = Swig_typemap_lookup("out", n, "result", f);
    if (is_functionpointer(type)) {
      emitASFunctionWrapper(n, n, f_begin);
      Replaceall(outtp, "$wrapper_name",
                 Getattr(n, "wrap:Functionwrap"));
    }
    emitASType(outtp, n);
    
    Replaceall(outtp, "$result", "asresult");
    Printf(f->code, "%s\n", outtp);
    ret_val = NewString("asresult");
    Replaceall(rettp, "$input", ret_val);
    Delete(ret_val);
    
    Printf(f->code, "%s\n", rettp);
    Printf(f->code, "}\n");
    Wrapper_print(f, f_begin);
    
    DelWrapper(f);
    Delete(ctypestr);
    Delete(glue_func);
    return SWIG_OK;
  }
  
  /*
   * Constants turn into Actionscript getters that return the constant
   * value from C.
   */
  virtual int constantWrapper(Node *n) {
    SwigType *type = Getattr(n, "type");
    String *astype = Swig_typemap_lookup("astype", n, "", 0);
    ParmList *parms = Getattr(n, "parms");
    
    
    Wrapper *f = NewWrapper();
    emit_parameter_variables(parms, f);
    emit_attach_parmmaps(parms, f);
    Swig_typemap_attach_parms("astype", parms, f);
    
    String *name;
    if (getClassName()) {
      name = sym_name;
    } else {
      name = Getattr(n, "sym:name");
    }
    
    String *glue_func = Swig_name_wrapper(name);
    Setattr(n, "wrap:name", glue_func);
    String *ctypestr = SwigType_str(type, 0);
    
    // Emit Actionscript getter
    String *as_out = getASOutput();
    
    Printf(as_out, "\tpublic static function get %s():%s{\n",
           name, astype);
    Printf(as_out, "\t\treturn %s();\n", glue_func);
    Printf(as_out, "\t}\n\n");
    
    // Emit C function (just returns the value of the constant)
    String *wassig = NewStringf("public function %s(%s):%s",
                                glue_func, "", astype);
    String *sigattrib = NewStringf(
                                   "__attribute__((annotate(\"as3sig:%s\")))", wassig);
    Printf(f->def, "%s\nvoid %s() {\n", sigattrib, glue_func);
    Wrapper_add_localv(f, "result", SwigType_lstr(type, "result"), NIL);
    
    // Add the stripped quotes back in
    String *new_value = NewString("");
    if (SwigType_type(type) == T_STRING) {
      Printf(new_value, "\"%s\"", Copy(Getattr(n, "value")));
      Setattr(n, "value", new_value);
    } else if (SwigType_type(type) == T_CHAR) {
      Printf(new_value, "\'%s\'", Copy(Getattr(n, "value")));
      Setattr(n, "value", new_value);
    }
    
    Printf(f->code, "  result = %s;\n", Getattr(n, "value"));
    
    String *outtp = Swig_typemap_lookup("out", n, "result", f);
    if (is_functionpointer(type)) {
      emitASFunctionWrapper(n, n, f_begin);
      Replaceall(outtp, "$wrapper_name",
                 Getattr(n, "wrap:Functionwrap"));
    }
    Replaceall(outtp, "$result", "asresult");
    emitASType(outtp, n);
    Printf(f->code, "%s\n", outtp);
    
    String *rettp = Swig_typemap_lookup("asreturn", n, "", f);
    Replaceall(rettp, "$input", "asresult");
    Printf(f->code, "%s\n", rettp);
    
    Printf(f->code, "}\n");
    Wrapper_print(f, f_begin);
    
    Delete(wassig);
    Delete(sigattrib);
    Delete(ctypestr);
    DelWrapper(f);
    Delete(glue_func);
    
    return SWIG_OK;
  }
  
  void swigClassHandler(Node *n) {
    class_sym_name = Copy(Getattr(n, "sym:name"));
    
    // Check for inheritance
    String *extend = NewString("");
    List *baselist = Getattr(n, "bases");
    if (baselist) {
      Iterator base = First(baselist);
      if (base.item) {
        Append(extend, " extends ");
        Append(extend, Getattr(base.item, "sym:name"));
        class_extends = true;
      }
      for (base = Next(base); base.item; base = Next(base)) {
        Printf(stderr, "Ignoring multiple inheritance of %s\n",
               Getattr(base.item, "name"));
      }
    }
    
    Printf(f_proxy_classes, "\npublic class %s%s {\n", class_sym_name,
           extend);
    Delete(extend);
    
    if (!class_extends) {
      Printf(f_proxy_classes, "\tpublic var swigCPtr:int;\n");
    }
    Language::classHandler(n);
    Printf(f_proxy_classes, "}\n\n");
    class_extends = false;
    Delete(class_sym_name);
    class_sym_name = NIL;
  }
  
  virtual int classHandler(Node *n) {
    String *name = Getattr(n, "name");
    
    if (CPlusPlus) {
      swigClassHandler(n);
    } else {
      const TypeMemoryLayout *type_layout =
      mem_layout->getMemoryLayout(name);
      if (type_layout) {
        emitASClassForCType(*type_layout, f_ctype_class);
      } else {
        Printf(stderr, "didn't find type info for %s\n", name);
        swigClassHandler(n);
      }
    }
    
    return SWIG_OK;
  }
  
  virtual int membervariableHandler(Node *n) {
    is_variable = true;
    sym_name = Getattr(n, "sym:name");
    Language::membervariableHandler(n);
    sym_name = NIL;
    is_variable = false;
    return SWIG_OK;
  }
  
  virtual int staticmembervariableHandler(Node *n) {
    is_variable = true;
    is_static = true;
    sym_name = Getattr(n, "sym:name");
    Language::staticmembervariableHandler(n);
    sym_name = NIL;
    is_static = false;
    is_variable = false;
    return SWIG_OK;
  }
  
  virtual int globalvariableHandler(Node *n) {
    is_variable = true;
    sym_name = Getattr(n, "sym:name");
    Language::globalvariableHandler(n);
    sym_name = NIL;
    is_variable = false;
    return SWIG_OK;
  }
  
  virtual int constructorHandler(Node *n) {
    is_constructor = true;
    Language::constructorHandler(n);
    is_constructor = false;
    return SWIG_OK;
  }
  
  virtual int destructorHandler(Node *n) {
    is_destructor = true;
    sym_name = Getattr(n, "sym:name");
    Language::destructorHandler(n);
    sym_name = NIL;
    is_destructor = false;
    return SWIG_OK;
  }
  
  virtual int enumvalueDeclaration(Node *n) {
    sym_name = Getattr(n, "sym:name");
    Language::enumvalueDeclaration(n);
    sym_name = NIL;
    return SWIG_OK;
  }
  
  virtual int staticmemberfunctionHandler(Node *n) {
    is_static = true;
    sym_name = Getattr(n, "sym:name");
    Language::staticmemberfunctionHandler(n);
    sym_name = NIL;
    is_static = false;
    return SWIG_OK;
  }
  
  virtual int memberfunctionHandler(Node *n) {
    sym_name = Getattr(n, "sym:name");
    Language::memberfunctionHandler(n);
    sym_name = NIL;
    return SWIG_OK;
  }
  
  // TODO: fix the variable naming scheme here
private:
  File *f_module;
  File *f_begin;
  String *f_func_class;
  String *f_proxy_classes;
  String *f_ctype_class;
  CMemoryLayout *mem_layout;
  String *sym_name;
  String *class_sym_name;
  bool is_variable;
  bool is_static;
  bool is_constructor;
  bool is_destructor;
  bool class_extends;
  
  void open_output_file(File **file, String *name) {
    *file = NewFile(name, "w", SWIG_output_files());
    if (!*file) {
      Printf(stderr, "couldn't open output file %s: ", name);
      perror("");
      SWIG_exit(EXIT_FAILURE);
    }
  }
};

extern "C" Language *
swig_as3(void) {
  return new AS3();
}

