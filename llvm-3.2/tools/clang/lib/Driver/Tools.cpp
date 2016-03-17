//===--- Tools.cpp - Tools Implementations --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Tools.h"

#include "clang/Driver/Action.h"
#include "clang/Driver/Arg.h"
#include "clang/Driver/ArgList.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Job.h"
#include "clang/Driver/Option.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Util.h"
#include "clang/Basic/ObjCRuntime.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/ErrorHandling.h"

#include "InputInfo.h"
#include "SanitizerArgs.h"
#include "ToolChains.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;

/// CheckPreprocessingOptions - Perform some validation of preprocessing
/// arguments that is shared with gcc.
static void CheckPreprocessingOptions(const Driver &D, const ArgList &Args) {
  if (Arg *A = Args.getLastArg(options::OPT_C, options::OPT_CC))
    if (!Args.hasArg(options::OPT_E) && !D.CCCIsCPP)
      D.Diag(diag::err_drv_argument_only_allowed_with)
        << A->getAsString(Args) << "-E";
}

/// CheckCodeGenerationOptions - Perform some validation of code generation
/// arguments that is shared with gcc.
static void CheckCodeGenerationOptions(const Driver &D, const ArgList &Args) {
  // In gcc, only ARM checks this, but it seems reasonable to check universally.
  if (Args.hasArg(options::OPT_static))
    if (const Arg *A = Args.getLastArg(options::OPT_dynamic,
                                       options::OPT_mdynamic_no_pic))
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << A->getAsString(Args) << "-static";
}

// Quote target names for inclusion in GNU Make dependency files.
// Only the characters '$', '#', ' ', '\t' are quoted.
static void QuoteTarget(StringRef Target,
                        SmallVectorImpl<char> &Res) {
  for (unsigned i = 0, e = Target.size(); i != e; ++i) {
    switch (Target[i]) {
    case ' ':
    case '\t':
      // Escape the preceding backslashes
      for (int j = i - 1; j >= 0 && Target[j] == '\\'; --j)
        Res.push_back('\\');

      // Escape the space/tab
      Res.push_back('\\');
      break;
    case '$':
      Res.push_back('$');
      break;
    case '#':
      Res.push_back('\\');
      break;
    default:
      break;
    }

    Res.push_back(Target[i]);
  }
}

static void addDirectoryList(const ArgList &Args,
                             ArgStringList &CmdArgs,
                             const char *ArgName,
                             const char *EnvVar) {
  const char *DirList = ::getenv(EnvVar);
  bool CombinedArg = false;

  if (!DirList)
    return; // Nothing to do.

  StringRef Name(ArgName);
  if (Name.equals("-I") || Name.equals("-L"))
    CombinedArg = true;

  StringRef Dirs(DirList);
  if (Dirs.empty()) // Empty string should not add '.'.
    return;

  StringRef::size_type Delim;
  while ((Delim = Dirs.find(llvm::sys::PathSeparator)) != StringRef::npos) {
    if (Delim == 0) { // Leading colon.
      if (CombinedArg) {
        CmdArgs.push_back(Args.MakeArgString(std::string(ArgName) + "."));
      } else {
        CmdArgs.push_back(ArgName);
        CmdArgs.push_back(".");
      }
    } else {
      if (CombinedArg) {
        CmdArgs.push_back(Args.MakeArgString(std::string(ArgName) + Dirs.substr(0, Delim)));
      } else {
        CmdArgs.push_back(ArgName);
        CmdArgs.push_back(Args.MakeArgString(Dirs.substr(0, Delim)));
      }
    }
    Dirs = Dirs.substr(Delim + 1);
  }

  if (Dirs.empty()) { // Trailing colon.
    if (CombinedArg) {
      CmdArgs.push_back(Args.MakeArgString(std::string(ArgName) + "."));
    } else {
      CmdArgs.push_back(ArgName);
      CmdArgs.push_back(".");
    }
  } else { // Add the last path.
    if (CombinedArg) {
      CmdArgs.push_back(Args.MakeArgString(std::string(ArgName) + Dirs));
    } else {
      CmdArgs.push_back(ArgName);
      CmdArgs.push_back(Args.MakeArgString(Dirs));
    }
  }
}

static void AddLinkerInputs(const ToolChain &TC,
                            const InputInfoList &Inputs, const ArgList &Args,
                            ArgStringList &CmdArgs) {
  const Driver &D = TC.getDriver();

  // Add extra linker input arguments which are not treated as inputs
  // (constructed via -Xarch_).
  Args.AddAllArgValues(CmdArgs, options::OPT_Zlinker_input);

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;

    if (!TC.HasNativeLLVMSupport()) {
      // Don't try to pass LLVM inputs unless we have native support.
      if (II.getType() == types::TY_LLVM_IR ||
          II.getType() == types::TY_LTO_IR ||
          II.getType() == types::TY_LLVM_BC ||
          II.getType() == types::TY_LTO_BC)
        D.Diag(diag::err_drv_no_linker_llvm_support)
          << TC.getTripleString();
    }

    // Add filenames immediately.
    if (II.isFilename()) {
      CmdArgs.push_back(II.getFilename());
      continue;
    }

    // Otherwise, this is a linker input argument.
    const Arg &A = II.getInputArg();

    // Handle reserved library options.
    if (A.getOption().matches(options::OPT_Z_reserved_lib_stdcxx)) {
      TC.AddCXXStdlibLibArgs(Args, CmdArgs);
    } else if (A.getOption().matches(options::OPT_Z_reserved_lib_cckext)) {
      TC.AddCCKextLibArgs(Args, CmdArgs);
    } else
      A.renderAsInput(Args, CmdArgs);
  }

  // LIBRARY_PATH - included following the user specified library paths.
  addDirectoryList(Args, CmdArgs, "-L", "LIBRARY_PATH");
}

/// \brief Determine whether Objective-C automated reference counting is
/// enabled.
static bool isObjCAutoRefCount(const ArgList &Args) {
  return Args.hasFlag(options::OPT_fobjc_arc, options::OPT_fno_objc_arc, false);
}

/// \brief Determine whether we are linking the ObjC runtime.
static bool isObjCRuntimeLinked(const ArgList &Args) {
  if (isObjCAutoRefCount(Args)) {
    Args.ClaimAllArgs(options::OPT_fobjc_link_runtime);
    return true;
  }
  return Args.hasArg(options::OPT_fobjc_link_runtime);
}

static void addProfileRT(const ToolChain &TC, const ArgList &Args,
                         ArgStringList &CmdArgs,
                         llvm::Triple Triple) {
  if (!(Args.hasArg(options::OPT_fprofile_arcs) ||
        Args.hasArg(options::OPT_fprofile_generate) ||
        Args.hasArg(options::OPT_fcreate_profile) ||
        Args.hasArg(options::OPT_coverage)))
    return;

  // GCC links libgcov.a by adding -L<inst>/gcc/lib/gcc/<triple>/<ver> -lgcov to
  // the link line. We cannot do the same thing because unlike gcov there is a
  // libprofile_rt.so. We used to use the -l:libprofile_rt.a syntax, but that is
  // not supported by old linkers.
  std::string ProfileRT =
    std::string(TC.getDriver().Dir) + "/../lib/libprofile_rt.a";

  CmdArgs.push_back(Args.MakeArgString(ProfileRT));
}

static bool forwardToGCC(const Option &O) {
  return !O.hasFlag(options::NoForward) &&
         !O.hasFlag(options::DriverOption) &&
         !O.hasFlag(options::LinkerInput);
}

void Clang::AddPreprocessingOptions(Compilation &C,
                                    const Driver &D,
                                    const ArgList &Args,
                                    ArgStringList &CmdArgs,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs) const {
  Arg *A;

  CheckPreprocessingOptions(D, Args);

  Args.AddLastArg(CmdArgs, options::OPT_C);
  Args.AddLastArg(CmdArgs, options::OPT_CC);

  // Handle dependency file generation.
  if ((A = Args.getLastArg(options::OPT_M, options::OPT_MM)) ||
      (A = Args.getLastArg(options::OPT_MD)) ||
      (A = Args.getLastArg(options::OPT_MMD))) {
    // Determine the output location.
    const char *DepFile;
    if (Arg *MF = Args.getLastArg(options::OPT_MF)) {
      DepFile = MF->getValue();
      C.addFailureResultFile(DepFile);
    } else if (Output.getType() == types::TY_Dependencies) {
      DepFile = Output.getFilename();
    } else if (A->getOption().matches(options::OPT_M) ||
               A->getOption().matches(options::OPT_MM)) {
      DepFile = "-";
    } else {
      DepFile = darwin::CC1::getDependencyFileName(Args, Inputs);
      C.addFailureResultFile(DepFile);
    }
    CmdArgs.push_back("-dependency-file");
    CmdArgs.push_back(DepFile);

    // Add a default target if one wasn't specified.
    if (!Args.hasArg(options::OPT_MT) && !Args.hasArg(options::OPT_MQ)) {
      const char *DepTarget;

      // If user provided -o, that is the dependency target, except
      // when we are only generating a dependency file.
      Arg *OutputOpt = Args.getLastArg(options::OPT_o);
      if (OutputOpt && Output.getType() != types::TY_Dependencies) {
        DepTarget = OutputOpt->getValue();
      } else {
        // Otherwise derive from the base input.
        //
        // FIXME: This should use the computed output file location.
        SmallString<128> P(Inputs[0].getBaseInput());
        llvm::sys::path::replace_extension(P, "o");
        DepTarget = Args.MakeArgString(llvm::sys::path::filename(P));
      }

      CmdArgs.push_back("-MT");
      SmallString<128> Quoted;
      QuoteTarget(DepTarget, Quoted);
      CmdArgs.push_back(Args.MakeArgString(Quoted));
    }

    if (A->getOption().matches(options::OPT_M) ||
        A->getOption().matches(options::OPT_MD))
      CmdArgs.push_back("-sys-header-deps");
  }

  if (Args.hasArg(options::OPT_MG)) {
    if (!A || A->getOption().matches(options::OPT_MD) ||
              A->getOption().matches(options::OPT_MMD))
      D.Diag(diag::err_drv_mg_requires_m_or_mm);
    CmdArgs.push_back("-MG");
  }

  Args.AddLastArg(CmdArgs, options::OPT_MP);

  // Convert all -MQ <target> args to -MT <quoted target>
  for (arg_iterator it = Args.filtered_begin(options::OPT_MT,
                                             options::OPT_MQ),
         ie = Args.filtered_end(); it != ie; ++it) {
    const Arg *A = *it;
    A->claim();

    if (A->getOption().matches(options::OPT_MQ)) {
      CmdArgs.push_back("-MT");
      SmallString<128> Quoted;
      QuoteTarget(A->getValue(), Quoted);
      CmdArgs.push_back(Args.MakeArgString(Quoted));

    // -MT flag - no change
    } else {
      A->render(Args, CmdArgs);
    }
  }

  // Add -i* options, and automatically translate to
  // -include-pch/-include-pth for transparent PCH support. It's
  // wonky, but we include looking for .gch so we can support seamless
  // replacement into a build system already set up to be generating
  // .gch files.
  bool RenderedImplicitInclude = false;
  for (arg_iterator it = Args.filtered_begin(options::OPT_clang_i_Group),
         ie = Args.filtered_end(); it != ie; ++it) {
    const Arg *A = it;

    if (A->getOption().matches(options::OPT_include)) {
      bool IsFirstImplicitInclude = !RenderedImplicitInclude;
      RenderedImplicitInclude = true;

      // Use PCH if the user requested it.
      bool UsePCH = D.CCCUsePCH;

      bool FoundPTH = false;
      bool FoundPCH = false;
      llvm::sys::Path P(A->getValue());
      bool Exists;
      if (UsePCH) {
        P.appendSuffix("pch");
        if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
          FoundPCH = true;
        else
          P.eraseSuffix();
      }

      if (!FoundPCH) {
        P.appendSuffix("pth");
        if (!llvm::sys::fs::exists(P.str(), Exists) && Exists)
          FoundPTH = true;
        else
          P.eraseSuffix();
      }

      if (!FoundPCH && !FoundPTH) {
        P.appendSuffix("gch");
        if (!llvm::sys::fs::exists(P.str(), Exists) && Exists) {
          FoundPCH = UsePCH;
          FoundPTH = !UsePCH;
        }
        else
          P.eraseSuffix();
      }

      if (FoundPCH || FoundPTH) {
        if (IsFirstImplicitInclude) {
          A->claim();
          if (UsePCH)
            CmdArgs.push_back("-include-pch");
          else
            CmdArgs.push_back("-include-pth");
          CmdArgs.push_back(Args.MakeArgString(P.str()));
          continue;
        } else {
          // Ignore the PCH if not first on command line and emit warning.
          D.Diag(diag::warn_drv_pch_not_first_include)
              << P.str() << A->getAsString(Args);
        }
      }
    }

    // Not translated, render as usual.
    A->claim();
    A->render(Args, CmdArgs);
  }

  Args.AddAllArgs(CmdArgs, options::OPT_D, options::OPT_U);
  Args.AddAllArgs(CmdArgs, options::OPT_I_Group, options::OPT_F,
                  options::OPT_index_header_map);

  // Add -Wp, and -Xassembler if using the preprocessor.

  // FIXME: There is a very unfortunate problem here, some troubled
  // souls abuse -Wp, to pass preprocessor options in gcc syntax. To
  // really support that we would have to parse and then translate
  // those options. :(
  Args.AddAllArgValues(CmdArgs, options::OPT_Wp_COMMA,
                       options::OPT_Xpreprocessor);

  // -I- is a deprecated GCC feature, reject it.
  if (Arg *A = Args.getLastArg(options::OPT_I_))
    D.Diag(diag::err_drv_I_dash_not_supported) << A->getAsString(Args);

  // If we have a --sysroot, and don't have an explicit -isysroot flag, add an
  // -isysroot to the CC1 invocation.
  StringRef sysroot = C.getSysRoot();
  if (sysroot != "") {
    if (!Args.hasArg(options::OPT_isysroot)) {
      CmdArgs.push_back("-isysroot");
      CmdArgs.push_back(C.getArgs().MakeArgString(sysroot));
    }
  }
  
  // If a module path was provided, pass it along. Otherwise, use a temporary
  // directory.
  if (Arg *A = Args.getLastArg(options::OPT_fmodule_cache_path)) {
    A->claim();
    A->render(Args, CmdArgs);
  } else {
    SmallString<128> DefaultModuleCache;
    llvm::sys::path::system_temp_directory(/*erasedOnReboot=*/false, 
                                           DefaultModuleCache);
    llvm::sys::path::append(DefaultModuleCache, "clang-module-cache");
    CmdArgs.push_back("-fmodule-cache-path");
    CmdArgs.push_back(Args.MakeArgString(DefaultModuleCache));
  }
  
  // Parse additional include paths from environment variables.
  // FIXME: We should probably sink the logic for handling these from the
  // frontend into the driver. It will allow deleting 4 otherwise unused flags.
  // CPATH - included following the user specified includes (but prior to
  // builtin and standard includes).
  addDirectoryList(Args, CmdArgs, "-I", "CPATH");
  // C_INCLUDE_PATH - system includes enabled when compiling C.
  addDirectoryList(Args, CmdArgs, "-c-isystem", "C_INCLUDE_PATH");
  // CPLUS_INCLUDE_PATH - system includes enabled when compiling C++.
  addDirectoryList(Args, CmdArgs, "-cxx-isystem", "CPLUS_INCLUDE_PATH");
  // OBJC_INCLUDE_PATH - system includes enabled when compiling ObjC.
  addDirectoryList(Args, CmdArgs, "-objc-isystem", "OBJC_INCLUDE_PATH");
  // OBJCPLUS_INCLUDE_PATH - system includes enabled when compiling ObjC++.
  addDirectoryList(Args, CmdArgs, "-objcxx-isystem", "OBJCPLUS_INCLUDE_PATH");

  // Add C++ include arguments, if needed.
  if (types::isCXX(Inputs[0].getType()))
    getToolChain().AddClangCXXStdlibIncludeArgs(Args, CmdArgs);

  // Add system include arguments.
  getToolChain().AddClangSystemIncludeArgs(Args, CmdArgs);
}

/// getLLVMArchSuffixForARM - Get the LLVM arch name to use for a particular
/// CPU.
//
// FIXME: This is redundant with -mcpu, why does LLVM use this.
// FIXME: tblgen this, or kill it!
static const char *getLLVMArchSuffixForARM(StringRef CPU) {
  return llvm::StringSwitch<const char *>(CPU)
    .Cases("arm7tdmi", "arm7tdmi-s", "arm710t", "v4t")
    .Cases("arm720t", "arm9", "arm9tdmi", "v4t")
    .Cases("arm920", "arm920t", "arm922t", "v4t")
    .Cases("arm940t", "ep9312","v4t")
    .Cases("arm10tdmi",  "arm1020t", "v5")
    .Cases("arm9e",  "arm926ej-s",  "arm946e-s", "v5e")
    .Cases("arm966e-s",  "arm968e-s",  "arm10e", "v5e")
    .Cases("arm1020e",  "arm1022e",  "xscale", "iwmmxt", "v5e")
    .Cases("arm1136j-s",  "arm1136jf-s",  "arm1176jz-s", "v6")
    .Cases("arm1176jzf-s",  "mpcorenovfp",  "mpcore", "v6")
    .Cases("arm1156t2-s",  "arm1156t2f-s", "v6t2")
    .Cases("cortex-a8", "cortex-a9", "cortex-a15", "v7")
    .Case("cortex-m3", "v7m")
    .Case("cortex-m4", "v7m")
    .Case("cortex-m0", "v6m")
    .Case("cortex-a9-mp", "v7f")
    .Case("swift", "v7s")
    .Default("");
}

/// getARMTargetCPU - Get the (LLVM) name of the ARM cpu we are targeting.
//
// FIXME: tblgen this.
static std::string getARMTargetCPU(const ArgList &Args,
                                   const llvm::Triple &Triple) {
  // FIXME: Warn on inconsistent use of -mcpu and -march.

  // If we have -mcpu=, use that.
  if (Arg *A = Args.getLastArg(options::OPT_mcpu_EQ)) {
    StringRef MCPU = A->getValue();
    // Handle -mcpu=native.
    if (MCPU == "native")
      return llvm::sys::getHostCPUName();
    else
      return MCPU;
  }

  StringRef MArch;
  if (Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    // Otherwise, if we have -march= choose the base CPU for that arch.
    MArch = A->getValue();
  } else {
    // Otherwise, use the Arch from the triple.
    MArch = Triple.getArchName();
  }

  // Handle -march=native.
  std::string NativeMArch;
  if (MArch == "native") {
    std::string CPU = llvm::sys::getHostCPUName();
    if (CPU != "generic") {
      // Translate the native cpu into the architecture. The switch below will
      // then chose the minimum cpu for that arch.
      NativeMArch = std::string("arm") + getLLVMArchSuffixForARM(CPU);
      MArch = NativeMArch;
    }
  }

  return llvm::StringSwitch<const char *>(MArch)
    .Cases("armv2", "armv2a","arm2")
    .Case("armv3", "arm6")
    .Case("armv3m", "arm7m")
    .Cases("armv4", "armv4t", "arm7tdmi")
    .Cases("armv5", "armv5t", "arm10tdmi")
    .Cases("armv5e", "armv5te", "arm1022e")
    .Case("armv5tej", "arm926ej-s")
    .Cases("armv6", "armv6k", "arm1136jf-s")
    .Case("armv6j", "arm1136j-s")
    .Cases("armv6z", "armv6zk", "arm1176jzf-s")
    .Case("armv6t2", "arm1156t2-s")
    .Cases("armv7", "armv7a", "armv7-a", "cortex-a8")
    .Cases("armv7f", "armv7-f", "cortex-a9-mp")
    .Cases("armv7s", "armv7-s", "swift")
    .Cases("armv7r", "armv7-r", "cortex-r4")
    .Cases("armv7m", "armv7-m", "cortex-m3")
    .Case("ep9312", "ep9312")
    .Case("iwmmxt", "iwmmxt")
    .Case("xscale", "xscale")
    .Cases("armv6m", "armv6-m", "cortex-m0")
    // If all else failed, return the most base CPU LLVM supports.
    .Default("arm7tdmi");
}

// FIXME: Move to target hook.
static bool isSignedCharDefault(const llvm::Triple &Triple) {
  switch (Triple.getArch()) {
  default:
    return true;

  case llvm::Triple::arm:
  case llvm::Triple::ppc:
  case llvm::Triple::ppc64:
    if (Triple.isOSDarwin())
      return true;
    return false;
  }
}

// Handle -mfpu=.
//
// FIXME: Centralize feature selection, defaulting shouldn't be also in the
// frontend target.
static void addFPUArgs(const Driver &D, const Arg *A, const ArgList &Args,
                       ArgStringList &CmdArgs) {
  StringRef FPU = A->getValue();

  // Set the target features based on the FPU.
  if (FPU == "fpa" || FPU == "fpe2" || FPU == "fpe3" || FPU == "maverick") {
    // Disable any default FPU support.
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-vfp2");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-vfp3");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neon");
  } else if (FPU == "vfp3-d16" || FPU == "vfpv3-d16") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+vfp3");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+d16");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neon");
  } else if (FPU == "vfp") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+vfp2");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neon");
  } else if (FPU == "vfp3" || FPU == "vfpv3") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+vfp3");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neon");
  } else if (FPU == "neon") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+neon");
  } else
    D.Diag(diag::err_drv_clang_unsupported) << A->getAsString(Args);
}

// Handle -mfpmath=.
static void addFPMathArgs(const Driver &D, const Arg *A, const ArgList &Args,
                          ArgStringList &CmdArgs, StringRef CPU) {
  StringRef FPMath = A->getValue();
  
  // Set the target features based on the FPMath.
  if (FPMath == "neon") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+neonfp");
    
    if (CPU != "cortex-a8" && CPU != "cortex-a9" && CPU != "cortex-a9-mp" &&
        CPU != "cortex-a15")
      D.Diag(diag::err_drv_invalid_feature) << "-mfpmath=neon" << CPU;
    
  } else if (FPMath == "vfp" || FPMath == "vfp2" || FPMath == "vfp3" ||
             FPMath == "vfp4") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neonfp");

    // FIXME: Add warnings when disabling a feature not present for a given CPU.    
  } else
    D.Diag(diag::err_drv_clang_unsupported) << A->getAsString(Args);
}

// Select the float ABI as determined by -msoft-float, -mhard-float, and
// -mfloat-abi=.
static StringRef getARMFloatABI(const Driver &D,
                                const ArgList &Args,
                                const llvm::Triple &Triple) {
  StringRef FloatABI;
  if (Arg *A = Args.getLastArg(options::OPT_msoft_float,
                               options::OPT_mhard_float,
                               options::OPT_mfloat_abi_EQ)) {
    if (A->getOption().matches(options::OPT_msoft_float))
      FloatABI = "soft";
    else if (A->getOption().matches(options::OPT_mhard_float))
      FloatABI = "hard";
    else {
      FloatABI = A->getValue();
      if (FloatABI != "soft" && FloatABI != "softfp" && FloatABI != "hard") {
        D.Diag(diag::err_drv_invalid_mfloat_abi)
          << A->getAsString(Args);
        FloatABI = "soft";
      }
    }
  }

  // If unspecified, choose the default based on the platform.
  if (FloatABI.empty()) {
    switch (Triple.getOS()) {
    case llvm::Triple::Darwin:
    case llvm::Triple::MacOSX:
    case llvm::Triple::IOS: {
      // Darwin defaults to "softfp" for v6 and v7.
      //
      // FIXME: Factor out an ARM class so we can cache the arch somewhere.
      std::string ArchName =
        getLLVMArchSuffixForARM(getARMTargetCPU(Args, Triple));
      if (StringRef(ArchName).startswith("v6") ||
          StringRef(ArchName).startswith("v7"))
        FloatABI = "softfp";
      else
        FloatABI = "soft";
      break;
    }

    default:
      switch(Triple.getEnvironment()) {
      case llvm::Triple::GNUEABIHF:
        FloatABI = "hard";
        break;
      case llvm::Triple::GNUEABI:
        FloatABI = "softfp";
        break;
      case llvm::Triple::EABI:
        // EABI is always AAPCS, and if it was not marked 'hard', it's softfp
        FloatABI = "softfp";
        break;
      case llvm::Triple::Android: {
        std::string ArchName =
          getLLVMArchSuffixForARM(getARMTargetCPU(Args, Triple));
        if (StringRef(ArchName).startswith("v7"))
          FloatABI = "softfp";
        else
          FloatABI = "soft";
        break;
      }
      default:
        // Assume "soft", but warn the user we are guessing.
        FloatABI = "soft";
        D.Diag(diag::warn_drv_assuming_mfloat_abi_is) << "soft";
        break;
      }
    }
  }

  return FloatABI;
}


void Clang::AddARMTargetArgs(const ArgList &Args,
                             ArgStringList &CmdArgs,
                             bool KernelOrKext) const {
  const Driver &D = getToolChain().getDriver();
  // Get the effective triple, which takes into account the deployment target.
  std::string TripleStr = getToolChain().ComputeEffectiveClangTriple(Args);
  llvm::Triple Triple(TripleStr);
  std::string CPUName = getARMTargetCPU(Args, Triple);

  // Select the ABI to use.
  //
  // FIXME: Support -meabi.
  const char *ABIName = 0;
  if (Arg *A = Args.getLastArg(options::OPT_mabi_EQ)) {
    ABIName = A->getValue();
  } else if (Triple.isOSDarwin()) {
    // The backend is hardwired to assume AAPCS for M-class processors, ensure
    // the frontend matches that.
    if (StringRef(CPUName).startswith("cortex-m")) {
      ABIName = "aapcs";
    } else {
      ABIName = "apcs-gnu";
    }
  } else {
    // Select the default based on the platform.
    switch(Triple.getEnvironment()) {
    case llvm::Triple::Android:
    case llvm::Triple::GNUEABI:
    case llvm::Triple::GNUEABIHF:
      ABIName = "aapcs-linux";
      break;
    case llvm::Triple::EABI:
      ABIName = "aapcs";
      break;
    default:
      ABIName = "apcs-gnu";
    }
  }
  CmdArgs.push_back("-target-abi");
  CmdArgs.push_back(ABIName);

  // Set the CPU based on -march= and -mcpu=.
  CmdArgs.push_back("-target-cpu");
  CmdArgs.push_back(Args.MakeArgString(CPUName));

  // Determine floating point ABI from the options & target defaults.
  StringRef FloatABI = getARMFloatABI(D, Args, Triple);
  if (FloatABI == "soft") {
    // Floating point operations and argument passing are soft.
    //
    // FIXME: This changes CPP defines, we need -target-soft-float.
    CmdArgs.push_back("-msoft-float");
    CmdArgs.push_back("-mfloat-abi");
    CmdArgs.push_back("soft");
  } else if (FloatABI == "softfp") {
    // Floating point operations are hard, but argument passing is soft.
    CmdArgs.push_back("-mfloat-abi");
    CmdArgs.push_back("soft");
  } else {
    // Floating point operations and argument passing are hard.
    assert(FloatABI == "hard" && "Invalid float abi!");
    CmdArgs.push_back("-mfloat-abi");
    CmdArgs.push_back("hard");
  }

  // Set appropriate target features for floating point mode.
  //
  // FIXME: Note, this is a hack, the LLVM backend doesn't actually use these
  // yet (it uses the -mfloat-abi and -msoft-float options above), and it is
  // stripped out by the ARM target.

  // Use software floating point operations?
  if (FloatABI == "soft") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+soft-float");
  }

  // Use software floating point argument passing?
  if (FloatABI != "hard") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+soft-float-abi");
  }

  // Honor -mfpu=.
  if (const Arg *A = Args.getLastArg(options::OPT_mfpu_EQ))
    addFPUArgs(D, A, Args, CmdArgs);

  // Honor -mfpmath=.
  if (const Arg *A = Args.getLastArg(options::OPT_mfpmath_EQ))
    addFPMathArgs(D, A, Args, CmdArgs, getARMTargetCPU(Args, Triple));

  // Setting -msoft-float effectively disables NEON because of the GCC
  // implementation, although the same isn't true of VFP or VFP3.
  if (FloatABI == "soft") {
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("-neon");
  }

  // Kernel code has more strict alignment requirements.
  if (KernelOrKext) {
    if (Triple.getOS() != llvm::Triple::IOS || Triple.isOSVersionLT(6)) {
      CmdArgs.push_back("-backend-option");
      CmdArgs.push_back("-arm-long-calls");
    }

    CmdArgs.push_back("-backend-option");
    CmdArgs.push_back("-arm-strict-align");

    // The kext linker doesn't know how to deal with movw/movt.
    CmdArgs.push_back("-backend-option");
    CmdArgs.push_back("-arm-darwin-use-movt=0");
  }

  // Setting -mno-global-merge disables the codegen global merge pass. Setting 
  // -mglobal-merge has no effect as the pass is enabled by default.
  if (Arg *A = Args.getLastArg(options::OPT_mglobal_merge,
                               options::OPT_mno_global_merge)) {
    if (A->getOption().matches(options::OPT_mno_global_merge))
      CmdArgs.push_back("-mno-global-merge");
  }

  if (Args.hasArg(options::OPT_mno_implicit_float))
    CmdArgs.push_back("-no-implicit-float");
}

// Translate MIPS CPU name alias option to CPU name.
static StringRef getMipsCPUFromAlias(const Arg &A) {
  if (A.getOption().matches(options::OPT_mips32))
    return "mips32";
  if (A.getOption().matches(options::OPT_mips32r2))
    return "mips32r2";
  if (A.getOption().matches(options::OPT_mips64))
    return "mips64";
  if (A.getOption().matches(options::OPT_mips64r2))
    return "mips64r2";
  llvm_unreachable("Unexpected option");
  return "";
}

// Get CPU and ABI names. They are not independent
// so we have to calculate them together.
static void getMipsCPUAndABI(const ArgList &Args,
                             const ToolChain &TC,
                             StringRef &CPUName,
                             StringRef &ABIName) {
  const char *DefMips32CPU = "mips32";
  const char *DefMips64CPU = "mips64";

  if (Arg *A = Args.getLastArg(options::OPT_march_EQ,
                               options::OPT_mcpu_EQ,
                               options::OPT_mips_CPUs_Group)) {
    if (A->getOption().matches(options::OPT_mips_CPUs_Group))
      CPUName = getMipsCPUFromAlias(*A);
    else
      CPUName = A->getValue();
  }

  if (Arg *A = Args.getLastArg(options::OPT_mabi_EQ))
    ABIName = A->getValue();

  // Setup default CPU and ABI names.
  if (CPUName.empty() && ABIName.empty()) {
    switch (TC.getTriple().getArch()) {
    default:
      llvm_unreachable("Unexpected triple arch name");
    case llvm::Triple::mips:
    case llvm::Triple::mipsel:
      CPUName = DefMips32CPU;
      break;
    case llvm::Triple::mips64:
    case llvm::Triple::mips64el:
      CPUName = DefMips64CPU;
      break;
    }
  }

  if (!ABIName.empty()) {
    // Deduce CPU name from ABI name.
    CPUName = llvm::StringSwitch<const char *>(ABIName)
      .Cases("o32", "eabi", DefMips32CPU)
      .Cases("n32", "n64", DefMips64CPU)
      .Default("");
  }
  else if (!CPUName.empty()) {
    // Deduce ABI name from CPU name.
    ABIName = llvm::StringSwitch<const char *>(CPUName)
      .Cases("mips32", "mips32r2", "o32")
      .Cases("mips64", "mips64r2", "n64")
      .Default("");
  }

  // FIXME: Warn on inconsistent cpu and abi usage.
}

// Select the MIPS float ABI as determined by -msoft-float, -mhard-float,
// and -mfloat-abi=.
static StringRef getMipsFloatABI(const Driver &D, const ArgList &Args) {
  // Select the float ABI as determined by -msoft-float, -mhard-float,
  // and -mfloat-abi=.
  StringRef FloatABI;
  if (Arg *A = Args.getLastArg(options::OPT_msoft_float,
                               options::OPT_mhard_float,
                               options::OPT_mfloat_abi_EQ)) {
    if (A->getOption().matches(options::OPT_msoft_float))
      FloatABI = "soft";
    else if (A->getOption().matches(options::OPT_mhard_float))
      FloatABI = "hard";
    else {
      FloatABI = A->getValue();
      if (FloatABI != "soft" && FloatABI != "single" && FloatABI != "hard") {
        D.Diag(diag::err_drv_invalid_mfloat_abi) << A->getAsString(Args);
        FloatABI = "hard";
      }
    }
  }

  // If unspecified, choose the default based on the platform.
  if (FloatABI.empty()) {
    // Assume "hard", because it's a default value used by gcc.
    // When we start to recognize specific target MIPS processors,
    // we will be able to select the default more correctly.
    FloatABI = "hard";
  }

  return FloatABI;
}

static void AddTargetFeature(const ArgList &Args,
                             ArgStringList &CmdArgs,
                             OptSpecifier OnOpt,
                             OptSpecifier OffOpt,
                             StringRef FeatureName) {
  if (Arg *A = Args.getLastArg(OnOpt, OffOpt)) {
    CmdArgs.push_back("-target-feature");
    if (A->getOption().matches(OnOpt))
      CmdArgs.push_back(Args.MakeArgString("+" + FeatureName));
    else
      CmdArgs.push_back(Args.MakeArgString("-" + FeatureName));
  }
}

void Clang::AddMIPSTargetArgs(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  const Driver &D = getToolChain().getDriver();
  StringRef CPUName;
  StringRef ABIName;
  getMipsCPUAndABI(Args, getToolChain(), CPUName, ABIName);

  CmdArgs.push_back("-target-cpu");
  CmdArgs.push_back(CPUName.data());

  CmdArgs.push_back("-target-abi");
  CmdArgs.push_back(ABIName.data());

  StringRef FloatABI = getMipsFloatABI(D, Args);

  if (FloatABI == "soft") {
    // Floating point operations and argument passing are soft.
    CmdArgs.push_back("-msoft-float");
    CmdArgs.push_back("-mfloat-abi");
    CmdArgs.push_back("soft");

    // FIXME: Note, this is a hack. We need to pass the selected float
    // mode to the MipsTargetInfoBase to define appropriate macros there.
    // Now it is the only method.
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+soft-float");
  }
  else if (FloatABI == "single") {
    // Restrict the use of hardware floating-point
    // instructions to 32-bit operations.
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+single-float");
  }
  else {
    // Floating point operations and argument passing are hard.
    assert(FloatABI == "hard" && "Invalid float abi!");
    CmdArgs.push_back("-mfloat-abi");
    CmdArgs.push_back("hard");
  }

  AddTargetFeature(Args, CmdArgs,
                   options::OPT_mips16, options::OPT_mno_mips16,
                   "mips16");
  AddTargetFeature(Args, CmdArgs,
                   options::OPT_mdsp, options::OPT_mno_dsp,
                   "dsp");
  AddTargetFeature(Args, CmdArgs,
                   options::OPT_mdspr2, options::OPT_mno_dspr2,
                   "dspr2");

  if (Arg *A = Args.getLastArg(options::OPT_G)) {
    StringRef v = A->getValue();
    CmdArgs.push_back("-mllvm");
    CmdArgs.push_back(Args.MakeArgString("-mips-ssection-threshold=" + v));
    A->claim();
  }
}

/// getPPCTargetCPU - Get the (LLVM) name of the PowerPC cpu we are targeting.
static std::string getPPCTargetCPU(const ArgList &Args) {
  if (Arg *A = Args.getLastArg(options::OPT_mcpu_EQ)) {
    StringRef CPUName = A->getValue();

    if (CPUName == "native") {
      std::string CPU = llvm::sys::getHostCPUName();
      if (!CPU.empty() && CPU != "generic")
        return CPU;
      else
        return "";
    }

    return llvm::StringSwitch<const char *>(CPUName)
      .Case("common", "generic")
      .Case("440", "440")
      .Case("440fp", "440")
      .Case("450", "450")
      .Case("601", "601")
      .Case("602", "602")
      .Case("603", "603")
      .Case("603e", "603e")
      .Case("603ev", "603ev")
      .Case("604", "604")
      .Case("604e", "604e")
      .Case("620", "620")
      .Case("G3", "g3")
      .Case("7400", "7400")
      .Case("G4", "g4")
      .Case("7450", "7450")
      .Case("G4+", "g4+")
      .Case("750", "750")
      .Case("970", "970")
      .Case("G5", "g5")
      .Case("a2", "a2")
      .Case("e500mc", "e500mc")
      .Case("e5500", "e5500")
      .Case("power6", "pwr6")
      .Case("power7", "pwr7")
      .Case("powerpc", "ppc")
      .Case("powerpc64", "ppc64")
      .Default("");
  }

  return "";
}

void Clang::AddPPCTargetArgs(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  std::string TargetCPUName = getPPCTargetCPU(Args);

  // LLVM may default to generating code for the native CPU,
  // but, like gcc, we default to a more generic option for
  // each architecture. (except on Darwin)
  llvm::Triple Triple = getToolChain().getTriple();
  if (TargetCPUName.empty() && !Triple.isOSDarwin()) {
    if (Triple.getArch() == llvm::Triple::ppc64)
      TargetCPUName = "ppc64";
    else
      TargetCPUName = "ppc";
  }

  if (!TargetCPUName.empty()) {
    CmdArgs.push_back("-target-cpu");
    CmdArgs.push_back(Args.MakeArgString(TargetCPUName.c_str()));
  }
}

void Clang::AddSparcTargetArgs(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  const Driver &D = getToolChain().getDriver();

  if (const Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    CmdArgs.push_back("-target-cpu");
    CmdArgs.push_back(A->getValue());
  }

  // Select the float ABI as determined by -msoft-float, -mhard-float, and
  StringRef FloatABI;
  if (Arg *A = Args.getLastArg(options::OPT_msoft_float,
                               options::OPT_mhard_float)) {
    if (A->getOption().matches(options::OPT_msoft_float))
      FloatABI = "soft";
    else if (A->getOption().matches(options::OPT_mhard_float))
      FloatABI = "hard";
  }

  // If unspecified, choose the default based on the platform.
  if (FloatABI.empty()) {
    switch (getToolChain().getTriple().getOS()) {
    default:
      // Assume "soft", but warn the user we are guessing.
      FloatABI = "soft";
      D.Diag(diag::warn_drv_assuming_mfloat_abi_is) << "soft";
      break;
    }
  }

  if (FloatABI == "soft") {
    // Floating point operations and argument passing are soft.
    //
    // FIXME: This changes CPP defines, we need -target-soft-float.
    CmdArgs.push_back("-msoft-float");
    CmdArgs.push_back("-target-feature");
    CmdArgs.push_back("+soft-float");
  } else {
    assert(FloatABI == "hard" && "Invalid float abi!");
    CmdArgs.push_back("-mhard-float");
  }
}

void Clang::AddX86TargetArgs(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  const bool isAndroid =
    getToolChain().getTriple().getEnvironment() == llvm::Triple::Android;
  if (!Args.hasFlag(options::OPT_mred_zone,
                    options::OPT_mno_red_zone,
                    true) ||
      Args.hasArg(options::OPT_mkernel) ||
      Args.hasArg(options::OPT_fapple_kext))
    CmdArgs.push_back("-disable-red-zone");

  if (Args.hasFlag(options::OPT_msoft_float,
                   options::OPT_mno_soft_float,
                   false))
    CmdArgs.push_back("-no-implicit-float");

  const char *CPUName = 0;
  if (const Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    if (StringRef(A->getValue()) == "native") {
      // FIXME: Reject attempts to use -march=native unless the target matches
      // the host.
      //
      // FIXME: We should also incorporate the detected target features for use
      // with -native.
      std::string CPU = llvm::sys::getHostCPUName();
      if (!CPU.empty() && CPU != "generic")
        CPUName = Args.MakeArgString(CPU);
    } else
      CPUName = A->getValue();
  }

  // Select the default CPU if none was given (or detection failed).
  if (!CPUName) {
    // FIXME: Need target hooks.
    if (getToolChain().getTriple().isOSDarwin()) {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "core2";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "yonah";
    } else if (getToolChain().getOS().startswith("haiku"))  {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "i586";
    } else if (getToolChain().getOS().startswith("openbsd"))  {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "i486";
    } else if (getToolChain().getOS().startswith("bitrig"))  {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "i686";
    } else if (getToolChain().getOS().startswith("freebsd"))  {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "i486";
    } else if (getToolChain().getOS().startswith("netbsd"))  {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        CPUName = "i486";
    } else {
      if (getToolChain().getArch() == llvm::Triple::x86_64)
        CPUName = "x86-64";
      else if (getToolChain().getArch() == llvm::Triple::x86)
        // All x86 devices running Android have core2 as their common
        // denominator. This makes a better choice than pentium4.
        CPUName = isAndroid ? "core2" : "pentium4";
    }
  }

  if (CPUName) {
    CmdArgs.push_back("-target-cpu");
    CmdArgs.push_back(CPUName);
  }

  // The required algorithm here is slightly strange: the options are applied
  // in order (so -mno-sse -msse2 disables SSE3), but any option that gets
  // directly overridden later is ignored (so "-mno-sse -msse2 -mno-sse2 -msse"
  // is equivalent to "-mno-sse2 -msse"). The -cc1 handling deals with the
  // former correctly, but not the latter; handle directly-overridden
  // attributes here.
  llvm::StringMap<unsigned> PrevFeature;
  std::vector<const char*> Features;
  for (arg_iterator it = Args.filtered_begin(options::OPT_m_x86_Features_Group),
         ie = Args.filtered_end(); it != ie; ++it) {
    StringRef Name = (*it)->getOption().getName();
    (*it)->claim();

    // Skip over "-m".
    assert(Name.startswith("m") && "Invalid feature name.");
    Name = Name.substr(1);

    bool IsNegative = Name.startswith("no-");
    if (IsNegative)
      Name = Name.substr(3);

    unsigned& Prev = PrevFeature[Name];
    if (Prev)
      Features[Prev - 1] = 0;
    Prev = Features.size() + 1;
    Features.push_back(Args.MakeArgString((IsNegative ? "-" : "+") + Name));
  }
  for (unsigned i = 0; i < Features.size(); i++) {
    if (Features[i]) {
      CmdArgs.push_back("-target-feature");
      CmdArgs.push_back(Features[i]);
    }
  }
}

static Arg* getLastHexagonArchArg (const ArgList &Args)
{
  Arg * A = NULL;

  for (ArgList::const_iterator it = Args.begin(), ie = Args.end();
       it != ie; ++it) {
    if ((*it)->getOption().matches(options::OPT_march_EQ) ||
        (*it)->getOption().matches(options::OPT_mcpu_EQ)) {
      A = *it;
      A->claim();
    }
    else if ((*it)->getOption().matches(options::OPT_m_Joined)){
      StringRef Value = (*it)->getValue(0);
      if (Value.startswith("v")) {
        A = *it;
        A->claim();
      }
    }
  }
  return A;
}

static StringRef getHexagonTargetCPU(const ArgList &Args)
{
  Arg *A;
  llvm::StringRef WhichHexagon;

  // Select the default CPU (v4) if none was given or detection failed.
  if ((A = getLastHexagonArchArg (Args))) {
    WhichHexagon = A->getValue();
    if (WhichHexagon == "")
      return "v4";
    else
      return WhichHexagon;
  }
  else
    return "v4";
}

void Clang::AddHexagonTargetArgs(const ArgList &Args,
                                 ArgStringList &CmdArgs) const {
  llvm::Triple Triple = getToolChain().getTriple();

  CmdArgs.push_back("-target-cpu");
  CmdArgs.push_back(Args.MakeArgString("hexagon" + getHexagonTargetCPU(Args)));
  CmdArgs.push_back("-fno-signed-char");
  CmdArgs.push_back("-nobuiltininc");

  if (Args.hasArg(options::OPT_mqdsp6_compat))
    CmdArgs.push_back("-mqdsp6-compat");

  if (Arg *A = Args.getLastArg(options::OPT_G,
                               options::OPT_msmall_data_threshold_EQ)) {
    std::string SmallDataThreshold="-small-data-threshold=";
    SmallDataThreshold += A->getValue();
    CmdArgs.push_back ("-mllvm");
    CmdArgs.push_back(Args.MakeArgString(SmallDataThreshold));
    A->claim();
  }

  if (!Args.hasArg(options::OPT_fno_short_enums))
    CmdArgs.push_back("-fshort-enums");
  if (Args.getLastArg(options::OPT_mieee_rnd_near)) {
    CmdArgs.push_back ("-mllvm");
    CmdArgs.push_back ("-enable-hexagon-ieee-rnd-near");
  }
  CmdArgs.push_back ("-mllvm");
  CmdArgs.push_back ("-machine-sink-split=0");
}

static bool
shouldUseExceptionTablesForObjCExceptions(const ObjCRuntime &runtime,
                                          const llvm::Triple &Triple) {
  // We use the zero-cost exception tables for Objective-C if the non-fragile
  // ABI is enabled or when compiling for x86_64 and ARM on Snow Leopard and
  // later.
  if (runtime.isNonFragile())
    return true;

  if (!Triple.isOSDarwin())
    return false;

  return (!Triple.isMacOSXVersionLT(10,5) &&
          (Triple.getArch() == llvm::Triple::x86_64 ||
           Triple.getArch() == llvm::Triple::arm));
}

/// addExceptionArgs - Adds exception related arguments to the driver command
/// arguments. There's a master flag, -fexceptions and also language specific
/// flags to enable/disable C++ and Objective-C exceptions.
/// This makes it possible to for example disable C++ exceptions but enable
/// Objective-C exceptions.
static void addExceptionArgs(const ArgList &Args, types::ID InputType,
                             const llvm::Triple &Triple,
                             bool KernelOrKext,
                             const ObjCRuntime &objcRuntime,
                             ArgStringList &CmdArgs) {
  if (KernelOrKext) {
    // -mkernel and -fapple-kext imply no exceptions, so claim exception related
    // arguments now to avoid warnings about unused arguments.
    Args.ClaimAllArgs(options::OPT_fexceptions);
    Args.ClaimAllArgs(options::OPT_fno_exceptions);
    Args.ClaimAllArgs(options::OPT_fobjc_exceptions);
    Args.ClaimAllArgs(options::OPT_fno_objc_exceptions);
    Args.ClaimAllArgs(options::OPT_fcxx_exceptions);
    Args.ClaimAllArgs(options::OPT_fno_cxx_exceptions);
    return;
  }

  // Exceptions are enabled by default.
  bool ExceptionsEnabled = true;

  // This keeps track of whether exceptions were explicitly turned on or off.
  bool DidHaveExplicitExceptionFlag = false;

  if (Arg *A = Args.getLastArg(options::OPT_fexceptions,
                               options::OPT_fno_exceptions)) {
    if (A->getOption().matches(options::OPT_fexceptions))
      ExceptionsEnabled = true;
    else
      ExceptionsEnabled = false;

    DidHaveExplicitExceptionFlag = true;
  }

  bool ShouldUseExceptionTables = false;

  // Exception tables and cleanups can be enabled with -fexceptions even if the
  // language itself doesn't support exceptions.
  if (ExceptionsEnabled && DidHaveExplicitExceptionFlag)
    ShouldUseExceptionTables = true;

  // Obj-C exceptions are enabled by default, regardless of -fexceptions. This
  // is not necessarily sensible, but follows GCC.
  if (types::isObjC(InputType) &&
      Args.hasFlag(options::OPT_fobjc_exceptions,
                   options::OPT_fno_objc_exceptions,
                   true)) {
    CmdArgs.push_back("-fobjc-exceptions");

    ShouldUseExceptionTables |=
      shouldUseExceptionTablesForObjCExceptions(objcRuntime, Triple);
  }

  if (types::isCXX(InputType)) {
    bool CXXExceptionsEnabled = ExceptionsEnabled;

    if (Arg *A = Args.getLastArg(options::OPT_fcxx_exceptions,
                                 options::OPT_fno_cxx_exceptions,
                                 options::OPT_fexceptions,
                                 options::OPT_fno_exceptions)) {
      if (A->getOption().matches(options::OPT_fcxx_exceptions))
        CXXExceptionsEnabled = true;
      else if (A->getOption().matches(options::OPT_fno_cxx_exceptions))
        CXXExceptionsEnabled = false;
    }

    if (CXXExceptionsEnabled) {
      CmdArgs.push_back("-fcxx-exceptions");

      ShouldUseExceptionTables = true;
    }
  }

  if (ShouldUseExceptionTables)
    CmdArgs.push_back("-fexceptions");
}

static bool ShouldDisableCFI(const ArgList &Args,
                             const ToolChain &TC) {
  bool Default = true;
  if (TC.getTriple().isOSDarwin()) {
    // The native darwin assembler doesn't support cfi directives, so
    // we disable them if we think the .s file will be passed to it.
    Default = Args.hasFlag(options::OPT_integrated_as,
			   options::OPT_no_integrated_as,
			   TC.IsIntegratedAssemblerDefault());
  }
  return !Args.hasFlag(options::OPT_fdwarf2_cfi_asm,
		       options::OPT_fno_dwarf2_cfi_asm,
		       Default);
}

static bool ShouldDisableDwarfDirectory(const ArgList &Args,
                                        const ToolChain &TC) {
  bool IsIADefault = TC.IsIntegratedAssemblerDefault();
  bool UseIntegratedAs = Args.hasFlag(options::OPT_integrated_as,
                                      options::OPT_no_integrated_as,
                                      IsIADefault);
  bool UseDwarfDirectory = Args.hasFlag(options::OPT_fdwarf_directory_asm,
                                        options::OPT_fno_dwarf_directory_asm,
                                        UseIntegratedAs);
  return !UseDwarfDirectory;
}

/// \brief Check whether the given input tree contains any compilation actions.
static bool ContainsCompileAction(const Action *A) {
  if (isa<CompileJobAction>(A))
    return true;

  for (Action::const_iterator it = A->begin(), ie = A->end(); it != ie; ++it)
    if (ContainsCompileAction(*it))
      return true;

  return false;
}

/// \brief Check if -relax-all should be passed to the internal assembler.
/// This is done by default when compiling non-assembler source with -O0.
static bool UseRelaxAll(Compilation &C, const ArgList &Args) {
  bool RelaxDefault = true;

  if (Arg *A = Args.getLastArg(options::OPT_O_Group))
    RelaxDefault = A->getOption().matches(options::OPT_O0);

  if (RelaxDefault) {
    RelaxDefault = false;
    for (ActionList::const_iterator it = C.getActions().begin(),
           ie = C.getActions().end(); it != ie; ++it) {
      if (ContainsCompileAction(*it)) {
        RelaxDefault = true;
        break;
      }
    }
  }

  return Args.hasFlag(options::OPT_mrelax_all, options::OPT_mno_relax_all,
    RelaxDefault);
}

SanitizerArgs::SanitizerArgs(const Driver &D, const ArgList &Args) {
  Kind = 0;

  const Arg *AsanArg, *TsanArg, *UbsanArg;
  for (ArgList::const_iterator I = Args.begin(), E = Args.end(); I != E; ++I) {
    unsigned Add = 0, Remove = 0;
    const char *DeprecatedReplacement = 0;
    if ((*I)->getOption().matches(options::OPT_faddress_sanitizer)) {
      Add = Address;
      DeprecatedReplacement = "-fsanitize=address";
    } else if ((*I)->getOption().matches(options::OPT_fno_address_sanitizer)) {
      Remove = Address;
      DeprecatedReplacement = "-fno-sanitize=address";
    } else if ((*I)->getOption().matches(options::OPT_fthread_sanitizer)) {
      Add = Thread;
      DeprecatedReplacement = "-fsanitize=thread";
    } else if ((*I)->getOption().matches(options::OPT_fno_thread_sanitizer)) {
      Remove = Thread;
      DeprecatedReplacement = "-fno-sanitize=thread";
    } else if ((*I)->getOption().matches(options::OPT_fcatch_undefined_behavior)) {
      Add = Undefined;
      DeprecatedReplacement = "-fsanitize=undefined";
    } else if ((*I)->getOption().matches(options::OPT_fsanitize_EQ)) {
      Add = parse(D, *I);
    } else if ((*I)->getOption().matches(options::OPT_fno_sanitize_EQ)) {
      Remove = parse(D, *I);
    } else {
      continue;
    }

    (*I)->claim();

    Kind |= Add;
    Kind &= ~Remove;

    if (Add & NeedsAsanRt) AsanArg = *I;
    if (Add & NeedsTsanRt) TsanArg = *I;
    if (Add & NeedsUbsanRt) UbsanArg = *I;

    // If this is a deprecated synonym, produce a warning directing users
    // towards the new spelling.
    if (DeprecatedReplacement)
      D.Diag(diag::warn_drv_deprecated_arg)
        << (*I)->getAsString(Args) << DeprecatedReplacement;
  }

  // Only one runtime library can be used at once.
  // FIXME: Allow Ubsan to be combined with the other two.
  bool NeedsAsan = needsAsanRt();
  bool NeedsTsan = needsTsanRt();
  bool NeedsUbsan = needsUbsanRt();
  if (NeedsAsan + NeedsTsan + NeedsUbsan > 1)
    D.Diag(diag::err_drv_argument_not_allowed_with)
      << describeSanitizeArg(Args, NeedsAsan ? AsanArg : TsanArg,
                             NeedsAsan ? NeedsAsanRt : NeedsTsanRt)
      << describeSanitizeArg(Args, NeedsUbsan ? UbsanArg : TsanArg,
                             NeedsUbsan ? NeedsUbsanRt : NeedsTsanRt);
}

/// If AddressSanitizer is enabled, add appropriate linker flags (Linux).
/// This needs to be called before we add the C run-time (malloc, etc).
static void addAsanRTLinux(const ToolChain &TC, const ArgList &Args,
                           ArgStringList &CmdArgs) {
  if(TC.getTriple().getEnvironment() == llvm::Triple::Android) {
    if (!Args.hasArg(options::OPT_shared)) {
      if (!Args.hasArg(options::OPT_pie))
        TC.getDriver().Diag(diag::err_drv_asan_android_requires_pie);
    }

    SmallString<128> LibAsan(TC.getDriver().ResourceDir);
    llvm::sys::path::append(LibAsan, "lib", "linux",
        (Twine("libclang_rt.asan-") +
            TC.getArchName() + "-android.so"));
    CmdArgs.push_back(Args.MakeArgString(LibAsan));
  } else {
    if (!Args.hasArg(options::OPT_shared)) {
      // LibAsan is "libclang_rt.asan-<ArchName>.a" in the Linux library
      // resource directory.
      SmallString<128> LibAsan(TC.getDriver().ResourceDir);
      llvm::sys::path::append(LibAsan, "lib", "linux",
                              (Twine("libclang_rt.asan-") +
                               TC.getArchName() + ".a"));
      CmdArgs.push_back(Args.MakeArgString(LibAsan));
      CmdArgs.push_back("-lpthread");
      CmdArgs.push_back("-ldl");
      CmdArgs.push_back("-export-dynamic");
    }
  }
}

/// If ThreadSanitizer is enabled, add appropriate linker flags (Linux).
/// This needs to be called before we add the C run-time (malloc, etc).
static void addTsanRTLinux(const ToolChain &TC, const ArgList &Args,
                           ArgStringList &CmdArgs) {
  if (!Args.hasArg(options::OPT_shared)) {
    // LibTsan is "libclang_rt.tsan-<ArchName>.a" in the Linux library
    // resource directory.
    SmallString<128> LibTsan(TC.getDriver().ResourceDir);
    llvm::sys::path::append(LibTsan, "lib", "linux",
                            (Twine("libclang_rt.tsan-") +
                             TC.getArchName() + ".a"));
    CmdArgs.push_back(Args.MakeArgString(LibTsan));
    CmdArgs.push_back("-lpthread");
    CmdArgs.push_back("-ldl");
    CmdArgs.push_back("-export-dynamic");
  }
}

/// If UndefinedBehaviorSanitizer is enabled, add appropriate linker flags
/// (Linux).
static void addUbsanRTLinux(const ToolChain &TC, const ArgList &Args,
                            ArgStringList &CmdArgs) {
  if (!Args.hasArg(options::OPT_shared)) {
    // LibUbsan is "libclang_rt.ubsan-<ArchName>.a" in the Linux library
    // resource directory.
    SmallString<128> LibUbsan(TC.getDriver().ResourceDir);
    llvm::sys::path::append(LibUbsan, "lib", "linux",
                            (Twine("libclang_rt.ubsan-") +
                             TC.getArchName() + ".a"));
    CmdArgs.push_back(Args.MakeArgString(LibUbsan));
    CmdArgs.push_back("-lpthread");
  }
}

static bool shouldUseFramePointer(const ArgList &Args,
                                  const llvm::Triple &Triple) {
  if (Arg *A = Args.getLastArg(options::OPT_fno_omit_frame_pointer,
                               options::OPT_fomit_frame_pointer))
    return A->getOption().matches(options::OPT_fno_omit_frame_pointer);

  // Don't use a frame pointer on linux x86 and x86_64 if optimizing.
  if ((Triple.getArch() == llvm::Triple::x86_64 ||
       Triple.getArch() == llvm::Triple::x86) &&
      Triple.getOS() == llvm::Triple::Linux) {
    if (Arg *A = Args.getLastArg(options::OPT_O_Group))
      if (!A->getOption().matches(options::OPT_O0))
        return false;
  }

  return true;
}

void Clang::ConstructJob(Compilation &C, const JobAction &JA,
                         const InputInfo &Output,
                         const InputInfoList &Inputs,
                         const ArgList &Args,
                         const char *LinkingOutput) const {
  bool KernelOrKext = Args.hasArg(options::OPT_mkernel,
                                  options::OPT_fapple_kext);
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  assert(Inputs.size() == 1 && "Unable to handle multiple inputs.");

  // Invoke ourselves in -cc1 mode.
  //
  // FIXME: Implement custom jobs for internal actions.
  CmdArgs.push_back("-cc1");

  // Add the "effective" target triple.
  CmdArgs.push_back("-triple");
  std::string TripleStr = getToolChain().ComputeEffectiveClangTriple(Args);
  CmdArgs.push_back(Args.MakeArgString(TripleStr));

  // Select the appropriate action.
  RewriteKind rewriteKind = RK_None;
  
  if (isa<AnalyzeJobAction>(JA)) {
    assert(JA.getType() == types::TY_Plist && "Invalid output type.");
    CmdArgs.push_back("-analyze");
  } else if (isa<MigrateJobAction>(JA)) {
    CmdArgs.push_back("-migrate");
  } else if (isa<PreprocessJobAction>(JA)) {
    if (Output.getType() == types::TY_Dependencies)
      CmdArgs.push_back("-Eonly");
    else
      CmdArgs.push_back("-E");
  } else if (isa<AssembleJobAction>(JA)) {
    CmdArgs.push_back("-emit-obj");

    if (UseRelaxAll(C, Args))
      CmdArgs.push_back("-mrelax-all");

    // When using an integrated assembler, translate -Wa, and -Xassembler
    // options.
    for (arg_iterator it = Args.filtered_begin(options::OPT_Wa_COMMA,
                                               options::OPT_Xassembler),
           ie = Args.filtered_end(); it != ie; ++it) {
      const Arg *A = *it;
      A->claim();

      for (unsigned i = 0, e = A->getNumValues(); i != e; ++i) {
        StringRef Value = A->getValue(i);

        if (Value == "-force_cpusubtype_ALL") {
          // Do nothing, this is the default and we don't support anything else.
        } else if (Value == "-L") {
          CmdArgs.push_back("-msave-temp-labels");
        } else if (Value == "--fatal-warnings") {
          CmdArgs.push_back("-mllvm");
          CmdArgs.push_back("-fatal-assembler-warnings");
        } else if (Value == "--noexecstack") {
          CmdArgs.push_back("-mnoexecstack");
        } else {
          D.Diag(diag::err_drv_unsupported_option_argument)
            << A->getOption().getName() << Value;
        }
      }
    }

    // Also ignore explicit -force_cpusubtype_ALL option.
    (void) Args.hasArg(options::OPT_force__cpusubtype__ALL);
  } else if (isa<PrecompileJobAction>(JA)) {
    // Use PCH if the user requested it.
    bool UsePCH = D.CCCUsePCH;

    if (JA.getType() == types::TY_Nothing)
      CmdArgs.push_back("-fsyntax-only");
    else if (UsePCH)
      CmdArgs.push_back("-emit-pch");
    else
      CmdArgs.push_back("-emit-pth");
  } else {
    assert(isa<CompileJobAction>(JA) && "Invalid action for clang tool.");

    if (JA.getType() == types::TY_Nothing) {
      CmdArgs.push_back("-fsyntax-only");
    } else if (JA.getType() == types::TY_LLVM_IR ||
               JA.getType() == types::TY_LTO_IR) {
      CmdArgs.push_back("-emit-llvm");
    } else if (JA.getType() == types::TY_LLVM_BC ||
               JA.getType() == types::TY_LTO_BC) {
      CmdArgs.push_back("-emit-llvm-bc");
    } else if (JA.getType() == types::TY_PP_Asm) {
      CmdArgs.push_back("-S");
    } else if (JA.getType() == types::TY_AST) {
      CmdArgs.push_back("-emit-pch");
    } else if (JA.getType() == types::TY_RewrittenObjC) {
      CmdArgs.push_back("-rewrite-objc");
      rewriteKind = RK_NonFragile;
    } else if (JA.getType() == types::TY_RewrittenLegacyObjC) {
      CmdArgs.push_back("-rewrite-objc");
      rewriteKind = RK_Fragile;
    } else {
      assert(JA.getType() == types::TY_PP_Asm &&
             "Unexpected output type!");
    }
  }

  // The make clang go fast button.
  CmdArgs.push_back("-disable-free");

  // Disable the verification pass in -asserts builds.
#ifdef NDEBUG
  CmdArgs.push_back("-disable-llvm-verifier");
#endif

  // Set the main file name, so that debug info works even with
  // -save-temps.
  CmdArgs.push_back("-main-file-name");
  CmdArgs.push_back(darwin::CC1::getBaseInputName(Args, Inputs));

  // Some flags which affect the language (via preprocessor
  // defines). See darwin::CC1::AddCPPArgs.
  if (Args.hasArg(options::OPT_static))
    CmdArgs.push_back("-static-define");

  if (isa<AnalyzeJobAction>(JA)) {
    // Enable region store model by default.
    CmdArgs.push_back("-analyzer-store=region");

    // Treat blocks as analysis entry points.
    CmdArgs.push_back("-analyzer-opt-analyze-nested-blocks");

    CmdArgs.push_back("-analyzer-eagerly-assume");

    // Add default argument set.
    if (!Args.hasArg(options::OPT__analyzer_no_default_checks)) {
      CmdArgs.push_back("-analyzer-checker=core");

      if (getToolChain().getTriple().getOS() != llvm::Triple::Win32)
        CmdArgs.push_back("-analyzer-checker=unix");

      if (getToolChain().getTriple().getVendor() == llvm::Triple::Apple)
        CmdArgs.push_back("-analyzer-checker=osx");
      
      CmdArgs.push_back("-analyzer-checker=deadcode");
      
      // Enable the following experimental checkers for testing. 
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.UncheckedReturn");
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.getpw");
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.gets");
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.mktemp");      
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.mkstemp");
      CmdArgs.push_back("-analyzer-checker=security.insecureAPI.vfork");
    }

    // Set the output format. The default is plist, for (lame) historical
    // reasons.
    CmdArgs.push_back("-analyzer-output");
    if (Arg *A = Args.getLastArg(options::OPT__analyzer_output))
      CmdArgs.push_back(A->getValue());
    else
      CmdArgs.push_back("plist");

    // Disable the presentation of standard compiler warnings when
    // using --analyze.  We only want to show static analyzer diagnostics
    // or frontend errors.
    CmdArgs.push_back("-w");

    // Add -Xanalyzer arguments when running as analyzer.
    Args.AddAllArgValues(CmdArgs, options::OPT_Xanalyzer);
  }

  CheckCodeGenerationOptions(D, Args);

  // For the PIC and PIE flag options, this logic is different from the legacy
  // logic in very old versions of GCC, as that logic was just a bug no one had
  // ever fixed. This logic is both more rational and consistent with GCC's new
  // logic now that the bugs are fixed. The last argument relating to either
  // PIC or PIE wins, and no other argument is used. If the last argument is
  // any flavor of the '-fno-...' arguments, both PIC and PIE are disabled. Any
  // PIE option implicitly enables PIC at the same level.
  bool PIE = false;
  bool PIC = getToolChain().isPICDefault();
  bool IsPICLevelTwo = PIC;
  if (Arg *A = Args.getLastArg(options::OPT_fPIC, options::OPT_fno_PIC,
                               options::OPT_fpic, options::OPT_fno_pic,
                               options::OPT_fPIE, options::OPT_fno_PIE,
                               options::OPT_fpie, options::OPT_fno_pie)) {
    Option O = A->getOption();
    if (O.matches(options::OPT_fPIC) || O.matches(options::OPT_fpic) ||
        O.matches(options::OPT_fPIE) || O.matches(options::OPT_fpie)) {
      PIE = O.matches(options::OPT_fPIE) || O.matches(options::OPT_fpie);
      PIC = PIE || O.matches(options::OPT_fPIC) || O.matches(options::OPT_fpic);
      IsPICLevelTwo = O.matches(options::OPT_fPIE) ||
                      O.matches(options::OPT_fPIC);
    } else {
      PIE = PIC = false;
    }
  }
  // Check whether the tool chain trumps the PIC-ness decision. If the PIC-ness
  // is forced, then neither PIC nor PIE flags will have no effect.
  if (getToolChain().isPICDefaultForced()) {
    PIE = false;
    PIC = getToolChain().isPICDefault();
    IsPICLevelTwo = PIC;
  }

  // Inroduce a Darwin-specific hack. If the default is PIC but the flags
  // specified while enabling PIC enabled level 1 PIC, just force it back to
  // level 2 PIC instead. This matches the behavior of Darwin GCC (based on my
  // informal testing).
  if (PIC && getToolChain().getTriple().isOSDarwin())
    IsPICLevelTwo |= getToolChain().isPICDefault();

  // Note that these flags are trump-cards. Regardless of the order w.r.t. the
  // PIC or PIE options above, if these show up, PIC is disabled.
  llvm::Triple Triple(TripleStr);
  if ((Args.hasArg(options::OPT_mkernel) ||
       Args.hasArg(options::OPT_fapple_kext)) &&
      (Triple.getOS() != llvm::Triple::IOS ||
       Triple.isOSVersionLT(6)))
    PIC = PIE = false;
  if (Args.hasArg(options::OPT_static))
    PIC = PIE = false;

  if (Arg *A = Args.getLastArg(options::OPT_mdynamic_no_pic)) {
    // This is a very special mode. It trumps the other modes, almost no one
    // uses it, and it isn't even valid on any OS but Darwin.
    if (!getToolChain().getTriple().isOSDarwin())
      D.Diag(diag::err_drv_unsupported_opt_for_target)
        << A->getSpelling() << getToolChain().getTriple().str();

    // FIXME: Warn when this flag trumps some other PIC or PIE flag.

    CmdArgs.push_back("-mrelocation-model");
    CmdArgs.push_back("dynamic-no-pic");

    // Only a forced PIC mode can cause the actual compile to have PIC defines
    // etc., no flags are sufficient. This behavior was selected to closely
    // match that of llvm-gcc and Apple GCC before that.
    if (getToolChain().isPICDefault() && getToolChain().isPICDefaultForced()) {
      CmdArgs.push_back("-pic-level");
      CmdArgs.push_back("2");
    }
  } else {
    // Currently, LLVM only knows about PIC vs. static; the PIE differences are
    // handled in Clang's IRGen by the -pie-level flag.
    CmdArgs.push_back("-mrelocation-model");
    CmdArgs.push_back(PIC ? "pic" : "static");

    if (PIC) {
      CmdArgs.push_back("-pic-level");
      CmdArgs.push_back(IsPICLevelTwo ? "2" : "1");
      if (PIE) {
        CmdArgs.push_back("-pie-level");
        CmdArgs.push_back(IsPICLevelTwo ? "2" : "1");
      }
    }
  }

  if (!Args.hasFlag(options::OPT_fmerge_all_constants,
                    options::OPT_fno_merge_all_constants))
    CmdArgs.push_back("-fno-merge-all-constants");

  // LLVM Code Generator Options.

  if (Arg *A = Args.getLastArg(options::OPT_mregparm_EQ)) {
    CmdArgs.push_back("-mregparm");
    CmdArgs.push_back(A->getValue());
  }

  if (Args.hasFlag(options::OPT_mrtd, options::OPT_mno_rtd, false))
    CmdArgs.push_back("-mrtd");

  if (shouldUseFramePointer(Args, getToolChain().getTriple()))
    CmdArgs.push_back("-mdisable-fp-elim");
  if (!Args.hasFlag(options::OPT_fzero_initialized_in_bss,
                    options::OPT_fno_zero_initialized_in_bss))
    CmdArgs.push_back("-mno-zero-initialized-in-bss");
  if (!Args.hasFlag(options::OPT_fstrict_aliasing,
                    options::OPT_fno_strict_aliasing,
                    getToolChain().IsStrictAliasingDefault()))
    CmdArgs.push_back("-relaxed-aliasing");
  if (Args.hasFlag(options::OPT_fstrict_enums, options::OPT_fno_strict_enums,
                   false))
    CmdArgs.push_back("-fstrict-enums");
  if (!Args.hasFlag(options::OPT_foptimize_sibling_calls,
                    options::OPT_fno_optimize_sibling_calls))
    CmdArgs.push_back("-mdisable-tail-calls");

  // Handle various floating point optimization flags, mapping them to the
  // appropriate LLVM code generation flags. The pattern for all of these is to
  // default off the codegen optimizations, and if any flag enables them and no
  // flag disables them after the flag enabling them, enable the codegen
  // optimization. This is complicated by several "umbrella" flags.
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_ffinite_math_only,
                               options::OPT_fno_finite_math_only,
                               options::OPT_fhonor_infinities,
                               options::OPT_fno_honor_infinities))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_finite_math_only &&
        A->getOption().getID() != options::OPT_fhonor_infinities)
      CmdArgs.push_back("-menable-no-infs");
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_ffinite_math_only,
                               options::OPT_fno_finite_math_only,
                               options::OPT_fhonor_nans,
                               options::OPT_fno_honor_nans))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_finite_math_only &&
        A->getOption().getID() != options::OPT_fhonor_nans)
      CmdArgs.push_back("-menable-no-nans");

  // -fmath-errno is the default on some platforms, e.g. BSD-derived OSes.
  bool MathErrno = getToolChain().IsMathErrnoDefault();
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_fmath_errno,
                               options::OPT_fno_math_errno))
    MathErrno = A->getOption().getID() == options::OPT_fmath_errno;
  if (MathErrno)
    CmdArgs.push_back("-fmath-errno");

  // There are several flags which require disabling very specific
  // optimizations. Any of these being disabled forces us to turn off the
  // entire set of LLVM optimizations, so collect them through all the flag
  // madness.
  bool AssociativeMath = false;
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_funsafe_math_optimizations,
                               options::OPT_fno_unsafe_math_optimizations,
                               options::OPT_fassociative_math,
                               options::OPT_fno_associative_math))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_unsafe_math_optimizations &&
        A->getOption().getID() != options::OPT_fno_associative_math)
      AssociativeMath = true;
  bool ReciprocalMath = false;
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_funsafe_math_optimizations,
                               options::OPT_fno_unsafe_math_optimizations,
                               options::OPT_freciprocal_math,
                               options::OPT_fno_reciprocal_math))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_unsafe_math_optimizations &&
        A->getOption().getID() != options::OPT_fno_reciprocal_math)
      ReciprocalMath = true;
  bool SignedZeros = true;
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_funsafe_math_optimizations,
                               options::OPT_fno_unsafe_math_optimizations,
                               options::OPT_fsigned_zeros,
                               options::OPT_fno_signed_zeros))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_unsafe_math_optimizations &&
        A->getOption().getID() != options::OPT_fsigned_zeros)
      SignedZeros = false;
  bool TrappingMath = true;
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_funsafe_math_optimizations,
                               options::OPT_fno_unsafe_math_optimizations,
                               options::OPT_ftrapping_math,
                               options::OPT_fno_trapping_math))
    if (A->getOption().getID() != options::OPT_fno_fast_math &&
        A->getOption().getID() != options::OPT_fno_unsafe_math_optimizations &&
        A->getOption().getID() != options::OPT_ftrapping_math)
      TrappingMath = false;
  if (!MathErrno && AssociativeMath && ReciprocalMath && !SignedZeros &&
      !TrappingMath)
    CmdArgs.push_back("-menable-unsafe-fp-math");


  // Validate and pass through -fp-contract option. 
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math,
                               options::OPT_fno_fast_math,
                               options::OPT_ffp_contract)) {
    if (A->getOption().getID() == options::OPT_ffp_contract) {
      StringRef Val = A->getValue();
      if (Val == "fast" || Val == "on" || Val == "off") {
        CmdArgs.push_back(Args.MakeArgString("-ffp-contract=" + Val));
      } else {
        D.Diag(diag::err_drv_unsupported_option_argument)
          << A->getOption().getName() << Val;
      }
    } else if (A->getOption().getID() == options::OPT_ffast_math) {
      // If fast-math is set then set the fp-contract mode to fast.
      CmdArgs.push_back(Args.MakeArgString("-ffp-contract=fast"));
    }
  }

  // We separately look for the '-ffast-math' and '-ffinite-math-only' flags,
  // and if we find them, tell the frontend to provide the appropriate
  // preprocessor macros. This is distinct from enabling any optimizations as
  // these options induce language changes which must survive serialization
  // and deserialization, etc.
  if (Arg *A = Args.getLastArg(options::OPT_ffast_math, options::OPT_fno_fast_math))
    if (A->getOption().matches(options::OPT_ffast_math))
      CmdArgs.push_back("-ffast-math");
  if (Arg *A = Args.getLastArg(options::OPT_ffinite_math_only, options::OPT_fno_fast_math))
    if (A->getOption().matches(options::OPT_ffinite_math_only))
      CmdArgs.push_back("-ffinite-math-only");

  // Decide whether to use verbose asm. Verbose assembly is the default on
  // toolchains which have the integrated assembler on by default.
  bool IsVerboseAsmDefault = getToolChain().IsIntegratedAssemblerDefault();
  if (Args.hasFlag(options::OPT_fverbose_asm, options::OPT_fno_verbose_asm,
                   IsVerboseAsmDefault) ||
      Args.hasArg(options::OPT_dA))
    CmdArgs.push_back("-masm-verbose");

  if (Args.hasArg(options::OPT_fdebug_pass_structure)) {
    CmdArgs.push_back("-mdebug-pass");
    CmdArgs.push_back("Structure");
  }
  if (Args.hasArg(options::OPT_fdebug_pass_arguments)) {
    CmdArgs.push_back("-mdebug-pass");
    CmdArgs.push_back("Arguments");
  }

  // Enable -mconstructor-aliases except on darwin, where we have to
  // work around a linker bug;  see <rdar://problem/7651567>.
  if (!getToolChain().getTriple().isOSDarwin() &&
      !getToolChain().getTriple().isAVM2())
    CmdArgs.push_back("-mconstructor-aliases");

  // Darwin's kernel doesn't support guard variables; just die if we
  // try to use them.
  if (KernelOrKext && getToolChain().getTriple().isOSDarwin())
    CmdArgs.push_back("-fforbid-guard-variables");

  if (Args.hasArg(options::OPT_mms_bitfields)) {
    CmdArgs.push_back("-mms-bitfields");
  }

  // This is a coarse approximation of what llvm-gcc actually does, both
  // -fasynchronous-unwind-tables and -fnon-call-exceptions interact in more
  // complicated ways.
  bool AsynchronousUnwindTables =
    Args.hasFlag(options::OPT_fasynchronous_unwind_tables,
                 options::OPT_fno_asynchronous_unwind_tables,
                 getToolChain().IsUnwindTablesDefault() &&
                 !KernelOrKext);
  if (Args.hasFlag(options::OPT_funwind_tables, options::OPT_fno_unwind_tables,
                   AsynchronousUnwindTables))
    CmdArgs.push_back("-munwind-tables");

  getToolChain().addClangTargetOptions(CmdArgs);

  if (Arg *A = Args.getLastArg(options::OPT_flimited_precision_EQ)) {
    CmdArgs.push_back("-mlimit-float-precision");
    CmdArgs.push_back(A->getValue());
  }

  // FIXME: Handle -mtune=.
  (void) Args.hasArg(options::OPT_mtune_EQ);

  if (Arg *A = Args.getLastArg(options::OPT_mcmodel_EQ)) {
    CmdArgs.push_back("-mcode-model");
    CmdArgs.push_back(A->getValue());
  }

  // Add target specific cpu and features flags.
  switch(getToolChain().getTriple().getArch()) {
  default:
    break;

  case llvm::Triple::arm:
  case llvm::Triple::thumb:
    AddARMTargetArgs(Args, CmdArgs, KernelOrKext);
    break;

  case llvm::Triple::mips:
  case llvm::Triple::mipsel:
  case llvm::Triple::mips64:
  case llvm::Triple::mips64el:
    AddMIPSTargetArgs(Args, CmdArgs);
    break;

  case llvm::Triple::ppc:
  case llvm::Triple::ppc64:
    AddPPCTargetArgs(Args, CmdArgs);
    break;

  case llvm::Triple::sparc:
    AddSparcTargetArgs(Args, CmdArgs);
    break;

  case llvm::Triple::x86:
  case llvm::Triple::x86_64:
    AddX86TargetArgs(Args, CmdArgs);
    break;

  case llvm::Triple::hexagon:
    AddHexagonTargetArgs(Args, CmdArgs);
    break;
  }



  // Pass the linker version in use.
  if (Arg *A = Args.getLastArg(options::OPT_mlinker_version_EQ)) {
    CmdArgs.push_back("-target-linker-version");
    CmdArgs.push_back(A->getValue());
  }

  // -mno-omit-leaf-frame-pointer is the default on Darwin.
  if (Args.hasFlag(options::OPT_momit_leaf_frame_pointer,
                   options::OPT_mno_omit_leaf_frame_pointer,
                   !getToolChain().getTriple().isOSDarwin()))
    CmdArgs.push_back("-momit-leaf-frame-pointer");

  // Explicitly error on some things we know we don't support and can't just
  // ignore.
  types::ID InputType = Inputs[0].getType();
  if (!Args.hasArg(options::OPT_fallow_unsupported)) {
    Arg *Unsupported;
    if (types::isCXX(InputType) &&
        getToolChain().getTriple().isOSDarwin() &&
        getToolChain().getTriple().getArch() == llvm::Triple::x86) {
      if ((Unsupported = Args.getLastArg(options::OPT_fapple_kext)) ||
          (Unsupported = Args.getLastArg(options::OPT_mkernel)))
        D.Diag(diag::err_drv_clang_unsupported_opt_cxx_darwin_i386)
          << Unsupported->getOption().getName();
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_v);
  Args.AddLastArg(CmdArgs, options::OPT_H);
  if (D.CCPrintHeaders && !D.CCGenDiagnostics) {
    CmdArgs.push_back("-header-include-file");
    CmdArgs.push_back(D.CCPrintHeadersFilename ?
                      D.CCPrintHeadersFilename : "-");
  }
  Args.AddLastArg(CmdArgs, options::OPT_P);
  Args.AddLastArg(CmdArgs, options::OPT_print_ivar_layout);

  if (D.CCLogDiagnostics && !D.CCGenDiagnostics) {
    CmdArgs.push_back("-diagnostic-log-file");
    CmdArgs.push_back(D.CCLogDiagnosticsFilename ?
                      D.CCLogDiagnosticsFilename : "-");
  }

  // Use the last option from "-g" group. "-gline-tables-only" is
  // preserved, all other debug options are substituted with "-g".
  Args.ClaimAllArgs(options::OPT_g_Group);
  if (Arg *A = Args.getLastArg(options::OPT_g_Group)) {
    if (A->getOption().matches(options::OPT_gline_tables_only)) {
      CmdArgs.push_back("-gline-tables-only");
    } else if (!A->getOption().matches(options::OPT_g0) &&
               !A->getOption().matches(options::OPT_ggdb0)) {
      CmdArgs.push_back("-g");
    }
  }

  // We ignore flags -gstrict-dwarf and -grecord-gcc-switches for now.
  Args.ClaimAllArgs(options::OPT_g_flags_Group);
  if (Args.hasArg(options::OPT_gcolumn_info))
    CmdArgs.push_back("-dwarf-column-info");

  Args.AddAllArgs(CmdArgs, options::OPT_ffunction_sections);
  Args.AddAllArgs(CmdArgs, options::OPT_fdata_sections);

  Args.AddAllArgs(CmdArgs, options::OPT_finstrument_functions);

  if (Args.hasArg(options::OPT_ftest_coverage) ||
      Args.hasArg(options::OPT_coverage))
    CmdArgs.push_back("-femit-coverage-notes");
  if (Args.hasArg(options::OPT_fprofile_arcs) ||
      Args.hasArg(options::OPT_coverage))
    CmdArgs.push_back("-femit-coverage-data");

  if (C.getArgs().hasArg(options::OPT_c) ||
      C.getArgs().hasArg(options::OPT_S)) {
    if (Output.isFilename()) {
      CmdArgs.push_back("-coverage-file");
      SmallString<128> absFilename(Output.getFilename());
      llvm::sys::fs::make_absolute(absFilename);
      CmdArgs.push_back(Args.MakeArgString(absFilename));
    }
  }

  // Pass options for controlling the default header search paths.
  if (Args.hasArg(options::OPT_nostdinc)) {
    CmdArgs.push_back("-nostdsysteminc");
    CmdArgs.push_back("-nobuiltininc");
  } else {
    if (Args.hasArg(options::OPT_nostdlibinc))
        CmdArgs.push_back("-nostdsysteminc");
    Args.AddLastArg(CmdArgs, options::OPT_nostdincxx);
    Args.AddLastArg(CmdArgs, options::OPT_nobuiltininc);
  }

  // Pass the path to compiler resource files.
  CmdArgs.push_back("-resource-dir");
  CmdArgs.push_back(D.ResourceDir.c_str());

  Args.AddLastArg(CmdArgs, options::OPT_working_directory);

  bool ARCMTEnabled = false;
  if (!Args.hasArg(options::OPT_fno_objc_arc)) {
    if (const Arg *A = Args.getLastArg(options::OPT_ccc_arcmt_check,
                                       options::OPT_ccc_arcmt_modify,
                                       options::OPT_ccc_arcmt_migrate)) {
      ARCMTEnabled = true;
      switch (A->getOption().getID()) {
      default:
        llvm_unreachable("missed a case");
      case options::OPT_ccc_arcmt_check:
        CmdArgs.push_back("-arcmt-check");
        break;
      case options::OPT_ccc_arcmt_modify:
        CmdArgs.push_back("-arcmt-modify");
        break;
      case options::OPT_ccc_arcmt_migrate:
        CmdArgs.push_back("-arcmt-migrate");
        CmdArgs.push_back("-mt-migrate-directory");
        CmdArgs.push_back(A->getValue());

        Args.AddLastArg(CmdArgs, options::OPT_arcmt_migrate_report_output);
        Args.AddLastArg(CmdArgs, options::OPT_arcmt_migrate_emit_arc_errors);
        break;
      }
    }
  }

  if (const Arg *A = Args.getLastArg(options::OPT_ccc_objcmt_migrate)) {
    if (ARCMTEnabled) {
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << A->getAsString(Args) << "-ccc-arcmt-migrate";
    }
    CmdArgs.push_back("-mt-migrate-directory");
    CmdArgs.push_back(A->getValue());

    if (!Args.hasArg(options::OPT_objcmt_migrate_literals,
                     options::OPT_objcmt_migrate_subscripting)) {
      // None specified, means enable them all.
      CmdArgs.push_back("-objcmt-migrate-literals");
      CmdArgs.push_back("-objcmt-migrate-subscripting");
    } else {
      Args.AddLastArg(CmdArgs, options::OPT_objcmt_migrate_literals);
      Args.AddLastArg(CmdArgs, options::OPT_objcmt_migrate_subscripting);
    }
  }

  // Add preprocessing options like -I, -D, etc. if we are using the
  // preprocessor.
  //
  // FIXME: Support -fpreprocessed
  if (types::getPreprocessedType(InputType) != types::TY_INVALID)
    AddPreprocessingOptions(C, D, Args, CmdArgs, Output, Inputs);

  // Don't warn about "clang -c -DPIC -fPIC test.i" because libtool.m4 assumes
  // that "The compiler can only warn and ignore the option if not recognized".
  // When building with ccache, it will pass -D options to clang even on
  // preprocessed inputs and configure concludes that -fPIC is not supported.
  Args.ClaimAllArgs(options::OPT_D);

  // Manually translate -O to -O2 and -O4 to -O3; let clang reject
  // others.
  if (Arg *A = Args.getLastArg(options::OPT_O_Group)) {
    if (A->getOption().matches(options::OPT_O4))
      CmdArgs.push_back("-O3");
    else if (A->getOption().matches(options::OPT_O) &&
             A->getValue()[0] == '\0')
      CmdArgs.push_back("-O2");
    else
      A->render(Args, CmdArgs);
  }

  Args.AddAllArgs(CmdArgs, options::OPT_W_Group);
  if (Args.hasFlag(options::OPT_pedantic, options::OPT_no_pedantic, false))
    CmdArgs.push_back("-pedantic");
  Args.AddLastArg(CmdArgs, options::OPT_pedantic_errors);
  Args.AddLastArg(CmdArgs, options::OPT_w);

  // Handle -{std, ansi, trigraphs} -- take the last of -{std, ansi}
  // (-ansi is equivalent to -std=c89).
  //
  // If a std is supplied, only add -trigraphs if it follows the
  // option.
  if (Arg *Std = Args.getLastArg(options::OPT_std_EQ, options::OPT_ansi)) {
    if (Std->getOption().matches(options::OPT_ansi))
      if (types::isCXX(InputType))
        CmdArgs.push_back("-std=c++98");
      else
        CmdArgs.push_back("-std=c89");
    else
      Std->render(Args, CmdArgs);

    if (Arg *A = Args.getLastArg(options::OPT_std_EQ, options::OPT_ansi,
                                 options::OPT_trigraphs))
      if (A != Std)
        A->render(Args, CmdArgs);
  } else {
    // Honor -std-default.
    //
    // FIXME: Clang doesn't correctly handle -std= when the input language
    // doesn't match. For the time being just ignore this for C++ inputs;
    // eventually we want to do all the standard defaulting here instead of
    // splitting it between the driver and clang -cc1.
    if (!types::isCXX(InputType))
      Args.AddAllArgsTranslated(CmdArgs, options::OPT_std_default_EQ,
                                "-std=", /*Joined=*/true);
    else if (getToolChain().getTriple().getOS() == llvm::Triple::Win32)
      CmdArgs.push_back("-std=c++11");

    Args.AddLastArg(CmdArgs, options::OPT_trigraphs);
  }

  // Map the bizarre '-Wwrite-strings' flag to a more sensible
  // '-fconst-strings'; this better indicates its actual behavior.
  if (Args.hasFlag(options::OPT_Wwrite_strings, options::OPT_Wno_write_strings,
                   false)) {
    // For perfect compatibility with GCC, we do this even in the presence of
    // '-w'. This flag names something other than a warning for GCC.
    CmdArgs.push_back("-fconst-strings");
  }

  // GCC provides a macro definition '__DEPRECATED' when -Wdeprecated is active
  // during C++ compilation, which it is by default. GCC keeps this define even
  // in the presence of '-w', match this behavior bug-for-bug.
  if (types::isCXX(InputType) &&
      Args.hasFlag(options::OPT_Wdeprecated, options::OPT_Wno_deprecated,
                   true)) {
    CmdArgs.push_back("-fdeprecated-macro");
  }

  // Translate GCC's misnamer '-fasm' arguments to '-fgnu-keywords'.
  if (Arg *Asm = Args.getLastArg(options::OPT_fasm, options::OPT_fno_asm)) {
    if (Asm->getOption().matches(options::OPT_fasm))
      CmdArgs.push_back("-fgnu-keywords");
    else
      CmdArgs.push_back("-fno-gnu-keywords");
  }

  if (ShouldDisableCFI(Args, getToolChain()))
    CmdArgs.push_back("-fno-dwarf2-cfi-asm");

  if (ShouldDisableDwarfDirectory(Args, getToolChain()))
    CmdArgs.push_back("-fno-dwarf-directory-asm");

  if (const char *pwd = ::getenv("PWD")) {
    // GCC also verifies that stat(pwd) and stat(".") have the same inode
    // number. Not doing those because stats are slow, but we could.
    if (llvm::sys::path::is_absolute(pwd)) {
      std::string CompDir = pwd;
      CmdArgs.push_back("-fdebug-compilation-dir");
      CmdArgs.push_back(Args.MakeArgString(CompDir));
    }
  }

  if (Arg *A = Args.getLastArg(options::OPT_ftemplate_depth_,
                               options::OPT_ftemplate_depth_EQ)) {
    CmdArgs.push_back("-ftemplate-depth");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(options::OPT_fconstexpr_depth_EQ)) {
    CmdArgs.push_back("-fconstexpr-depth");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(options::OPT_Wlarge_by_value_copy_EQ,
                               options::OPT_Wlarge_by_value_copy_def)) {
    if (A->getNumValues()) {
      StringRef bytes = A->getValue();
      CmdArgs.push_back(Args.MakeArgString("-Wlarge-by-value-copy=" + bytes));
    } else
      CmdArgs.push_back("-Wlarge-by-value-copy=64"); // default value
  }

  if (Arg *A = Args.getLastArg(options::OPT_fbounds_checking,
                               options::OPT_fbounds_checking_EQ)) {
    if (A->getNumValues()) {
      StringRef val = A->getValue();
      CmdArgs.push_back(Args.MakeArgString("-fbounds-checking=" + val));
    } else
      CmdArgs.push_back("-fbounds-checking=1");
  }

  if (Args.hasArg(options::OPT_relocatable_pch))
    CmdArgs.push_back("-relocatable-pch");

  if (Arg *A = Args.getLastArg(options::OPT_fconstant_string_class_EQ)) {
    CmdArgs.push_back("-fconstant-string-class");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(options::OPT_ftabstop_EQ)) {
    CmdArgs.push_back("-ftabstop");
    CmdArgs.push_back(A->getValue());
  }

  CmdArgs.push_back("-ferror-limit");
  if (Arg *A = Args.getLastArg(options::OPT_ferror_limit_EQ))
    CmdArgs.push_back(A->getValue());
  else
    CmdArgs.push_back("19");

  if (Arg *A = Args.getLastArg(options::OPT_fmacro_backtrace_limit_EQ)) {
    CmdArgs.push_back("-fmacro-backtrace-limit");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(options::OPT_ftemplate_backtrace_limit_EQ)) {
    CmdArgs.push_back("-ftemplate-backtrace-limit");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(options::OPT_fconstexpr_backtrace_limit_EQ)) {
    CmdArgs.push_back("-fconstexpr-backtrace-limit");
    CmdArgs.push_back(A->getValue());
  }

  // Pass -fmessage-length=.
  CmdArgs.push_back("-fmessage-length");
  if (Arg *A = Args.getLastArg(options::OPT_fmessage_length_EQ)) {
    CmdArgs.push_back(A->getValue());
  } else {
    // If -fmessage-length=N was not specified, determine whether this is a
    // terminal and, if so, implicitly define -fmessage-length appropriately.
    unsigned N = llvm::sys::Process::StandardErrColumns();
    CmdArgs.push_back(Args.MakeArgString(Twine(N)));
  }

  if (const Arg *A = Args.getLastArg(options::OPT_fvisibility_EQ)) {
    CmdArgs.push_back("-fvisibility");
    CmdArgs.push_back(A->getValue());
  }

  Args.AddLastArg(CmdArgs, options::OPT_fvisibility_inlines_hidden);

  Args.AddLastArg(CmdArgs, options::OPT_ftlsmodel_EQ);

  // -fhosted is default.
  if (Args.hasFlag(options::OPT_ffreestanding, options::OPT_fhosted, false) ||
      KernelOrKext)
    CmdArgs.push_back("-ffreestanding");

  // Forward -f (flag) options which we can pass directly.
  Args.AddLastArg(CmdArgs, options::OPT_femit_all_decls);
  Args.AddLastArg(CmdArgs, options::OPT_fheinous_gnu_extensions);
  Args.AddLastArg(CmdArgs, options::OPT_flimit_debug_info);
  Args.AddLastArg(CmdArgs, options::OPT_fno_limit_debug_info);
  Args.AddLastArg(CmdArgs, options::OPT_fno_operator_names);
  Args.AddLastArg(CmdArgs, options::OPT_faltivec);
  Args.AddLastArg(CmdArgs, options::OPT_fdiagnostics_show_template_tree);
  Args.AddLastArg(CmdArgs, options::OPT_fno_elide_type);

  SanitizerArgs Sanitize(D, Args);
  Sanitize.addArgs(Args, CmdArgs);

  // Report and error for -faltivec on anything other then PowerPC.
  if (const Arg *A = Args.getLastArg(options::OPT_faltivec))
    if (!(getToolChain().getTriple().getArch() == llvm::Triple::ppc ||
          getToolChain().getTriple().getArch() == llvm::Triple::ppc64))
      D.Diag(diag::err_drv_argument_only_allowed_with)
        << A->getAsString(Args) << "ppc/ppc64";

  if (getToolChain().SupportsProfiling())
    Args.AddLastArg(CmdArgs, options::OPT_pg);

  // -flax-vector-conversions is default.
  if (!Args.hasFlag(options::OPT_flax_vector_conversions,
                    options::OPT_fno_lax_vector_conversions))
    CmdArgs.push_back("-fno-lax-vector-conversions");

  if (Args.getLastArg(options::OPT_fapple_kext))
    CmdArgs.push_back("-fapple-kext");

  if (Args.hasFlag(options::OPT_frewrite_includes,
                   options::OPT_fno_rewrite_includes, false))
    CmdArgs.push_back("-frewrite-includes");

  Args.AddLastArg(CmdArgs, options::OPT_fobjc_sender_dependent_dispatch);
  Args.AddLastArg(CmdArgs, options::OPT_fdiagnostics_print_source_range_info);
  Args.AddLastArg(CmdArgs, options::OPT_fdiagnostics_parseable_fixits);
  Args.AddLastArg(CmdArgs, options::OPT_ftime_report);
  Args.AddLastArg(CmdArgs, options::OPT_ftrapv);

  if (Arg *A = Args.getLastArg(options::OPT_ftrapv_handler_EQ)) {
    CmdArgs.push_back("-ftrapv-handler");
    CmdArgs.push_back(A->getValue());
  }

  Args.AddLastArg(CmdArgs, options::OPT_ftrap_function_EQ);

  // -fno-strict-overflow implies -fwrapv if it isn't disabled, but
  // -fstrict-overflow won't turn off an explicitly enabled -fwrapv.
  if (Arg *A = Args.getLastArg(options::OPT_fwrapv,
                               options::OPT_fno_wrapv)) {
    if (A->getOption().matches(options::OPT_fwrapv))
      CmdArgs.push_back("-fwrapv");
  } else if (Arg *A = Args.getLastArg(options::OPT_fstrict_overflow,
                                      options::OPT_fno_strict_overflow)) {
    if (A->getOption().matches(options::OPT_fno_strict_overflow))
      CmdArgs.push_back("-fwrapv");
  }
  Args.AddLastArg(CmdArgs, options::OPT_fwritable_strings);
  Args.AddLastArg(CmdArgs, options::OPT_funroll_loops);

  Args.AddLastArg(CmdArgs, options::OPT_pthread);


  // -stack-protector=0 is default.
  unsigned StackProtectorLevel = 0;
  if (Arg *A = Args.getLastArg(options::OPT_fno_stack_protector,
                               options::OPT_fstack_protector_all,
                               options::OPT_fstack_protector)) {
    if (A->getOption().matches(options::OPT_fstack_protector))
      StackProtectorLevel = 1;
    else if (A->getOption().matches(options::OPT_fstack_protector_all))
      StackProtectorLevel = 2;
  } else {
    StackProtectorLevel =
      getToolChain().GetDefaultStackProtectorLevel(KernelOrKext);
  }
  if (StackProtectorLevel) {
    CmdArgs.push_back("-stack-protector");
    CmdArgs.push_back(Args.MakeArgString(Twine(StackProtectorLevel)));
  }

  // --param ssp-buffer-size=
  for (arg_iterator it = Args.filtered_begin(options::OPT__param),
       ie = Args.filtered_end(); it != ie; ++it) {
    StringRef Str((*it)->getValue());
    if (Str.startswith("ssp-buffer-size=")) {
      if (StackProtectorLevel) {
        CmdArgs.push_back("-stack-protector-buffer-size");
        // FIXME: Verify the argument is a valid integer.
        CmdArgs.push_back(Args.MakeArgString(Str.drop_front(16)));
      }
      (*it)->claim();
    }
  }

  // Translate -mstackrealign
  if (Args.hasFlag(options::OPT_mstackrealign, options::OPT_mno_stackrealign,
                   false)) {
    CmdArgs.push_back("-backend-option");
    CmdArgs.push_back("-force-align-stack");
  }
  if (!Args.hasFlag(options::OPT_mno_stackrealign, options::OPT_mstackrealign,
                   false)) {
    CmdArgs.push_back(Args.MakeArgString("-mstackrealign"));
  }

  if (Args.hasArg(options::OPT_mstack_alignment)) {
    StringRef alignment = Args.getLastArgValue(options::OPT_mstack_alignment);
    CmdArgs.push_back(Args.MakeArgString("-mstack-alignment=" + alignment));
  }
  if (Args.hasArg(options::OPT_mstrict_align)) {
    CmdArgs.push_back("-backend-option");
    CmdArgs.push_back("-arm-strict-align");
  }

  // Forward -f options with positive and negative forms; we translate
  // these by hand.

  if (Args.hasArg(options::OPT_mkernel)) {
    if (!Args.hasArg(options::OPT_fapple_kext) && types::isCXX(InputType))
      CmdArgs.push_back("-fapple-kext");
    if (!Args.hasArg(options::OPT_fbuiltin))
      CmdArgs.push_back("-fno-builtin");
    Args.ClaimAllArgs(options::OPT_fno_builtin);
  }
  // -fbuiltin is default.
  else if (!Args.hasFlag(options::OPT_fbuiltin, options::OPT_fno_builtin))
    CmdArgs.push_back("-fno-builtin");

  if (!Args.hasFlag(options::OPT_fassume_sane_operator_new,
                    options::OPT_fno_assume_sane_operator_new))
    CmdArgs.push_back("-fno-assume-sane-operator-new");

  // -fblocks=0 is default.
  if (Args.hasFlag(options::OPT_fblocks, options::OPT_fno_blocks,
                   getToolChain().IsBlocksDefault()) ||
        (Args.hasArg(options::OPT_fgnu_runtime) &&
         Args.hasArg(options::OPT_fobjc_nonfragile_abi) &&
         !Args.hasArg(options::OPT_fno_blocks))) {
    CmdArgs.push_back("-fblocks");

    if (!Args.hasArg(options::OPT_fgnu_runtime) && 
        !getToolChain().hasBlocksRuntime())
      CmdArgs.push_back("-fblocks-runtime-optional");
  }

  // -fmodules enables modules (off by default). However, for C++/Objective-C++,
  // users must also pass -fcxx-modules. The latter flag will disappear once the
  // modules implementation is solid for C++/Objective-C++ programs as well.
  if (Args.hasFlag(options::OPT_fmodules, options::OPT_fno_modules, false)) {
    bool AllowedInCXX = Args.hasFlag(options::OPT_fcxx_modules, 
                                     options::OPT_fno_cxx_modules, 
                                     false);
    if (AllowedInCXX || !types::isCXX(InputType))
      CmdArgs.push_back("-fmodules");
  }

  // -faccess-control is default.
  if (Args.hasFlag(options::OPT_fno_access_control,
                   options::OPT_faccess_control,
                   false))
    CmdArgs.push_back("-fno-access-control");

  // -felide-constructors is the default.
  if (Args.hasFlag(options::OPT_fno_elide_constructors,
                   options::OPT_felide_constructors,
                   false))
    CmdArgs.push_back("-fno-elide-constructors");

  // -frtti is default.
  if (!Args.hasFlag(options::OPT_frtti, options::OPT_fno_rtti) ||
      KernelOrKext) {
    CmdArgs.push_back("-fno-rtti");

    // -fno-rtti cannot usefully be combined with -fsanitize=vptr.
    if (Sanitize.sanitizesVptr()) {
      std::string NoRttiArg =
        Args.getLastArg(options::OPT_mkernel,
                        options::OPT_fapple_kext,
                        options::OPT_fno_rtti)->getAsString(Args);
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << "-fsanitize=vptr" << NoRttiArg;
    }
  }

  // -fshort-enums=0 is default for all architectures except Hexagon.
  if (Args.hasFlag(options::OPT_fshort_enums,
                   options::OPT_fno_short_enums,
                   getToolChain().getTriple().getArch() ==
                   llvm::Triple::hexagon))
    CmdArgs.push_back("-fshort-enums");

  // -fsigned-char is default.
  if (!Args.hasFlag(options::OPT_fsigned_char, options::OPT_funsigned_char,
                    isSignedCharDefault(getToolChain().getTriple())))
    CmdArgs.push_back("-fno-signed-char");

  // -fthreadsafe-static is default.
  if (!Args.hasFlag(options::OPT_fthreadsafe_statics,
                    options::OPT_fno_threadsafe_statics))
    CmdArgs.push_back("-fno-threadsafe-statics");

  // -fuse-cxa-atexit is default.
  if (!Args.hasFlag(options::OPT_fuse_cxa_atexit,
                    options::OPT_fno_use_cxa_atexit,
                   getToolChain().getTriple().getOS() != llvm::Triple::Cygwin &&
                  getToolChain().getTriple().getOS() != llvm::Triple::MinGW32 &&
              getToolChain().getTriple().getArch() != llvm::Triple::hexagon) ||
      KernelOrKext)
    CmdArgs.push_back("-fno-use-cxa-atexit");

  // -fms-extensions=0 is default.
  if (Args.hasFlag(options::OPT_fms_extensions, options::OPT_fno_ms_extensions,
                   getToolChain().getTriple().getOS() == llvm::Triple::Win32))
    CmdArgs.push_back("-fms-extensions");

  // -fms-inline-asm.
  if (Args.hasArg(options::OPT_fenable_experimental_ms_inline_asm))
    CmdArgs.push_back("-fenable-experimental-ms-inline-asm");

  // -fms-compatibility=0 is default.
  if (Args.hasFlag(options::OPT_fms_compatibility, 
                   options::OPT_fno_ms_compatibility,
                   (getToolChain().getTriple().getOS() == llvm::Triple::Win32 &&
                    Args.hasFlag(options::OPT_fms_extensions, 
                                 options::OPT_fno_ms_extensions,
                                 true))))
    CmdArgs.push_back("-fms-compatibility");

  // -fmsc-version=1300 is default.
  if (Args.hasFlag(options::OPT_fms_extensions, options::OPT_fno_ms_extensions,
                   getToolChain().getTriple().getOS() == llvm::Triple::Win32) ||
      Args.hasArg(options::OPT_fmsc_version)) {
    StringRef msc_ver = Args.getLastArgValue(options::OPT_fmsc_version);
    if (msc_ver.empty())
      CmdArgs.push_back("-fmsc-version=1300");
    else
      CmdArgs.push_back(Args.MakeArgString("-fmsc-version=" + msc_ver));
  }


  // -fborland-extensions=0 is default.
  if (Args.hasFlag(options::OPT_fborland_extensions,
                   options::OPT_fno_borland_extensions, false))
    CmdArgs.push_back("-fborland-extensions");

  // -fno-delayed-template-parsing is default, except for Windows where MSVC STL
  // needs it.
  if (Args.hasFlag(options::OPT_fdelayed_template_parsing,
                   options::OPT_fno_delayed_template_parsing,
                   getToolChain().getTriple().getOS() == llvm::Triple::Win32))
    CmdArgs.push_back("-fdelayed-template-parsing");

  // -fgnu-keywords default varies depending on language; only pass if
  // specified.
  if (Arg *A = Args.getLastArg(options::OPT_fgnu_keywords,
                               options::OPT_fno_gnu_keywords))
    A->render(Args, CmdArgs);

  if (Args.hasFlag(options::OPT_fgnu89_inline,
                   options::OPT_fno_gnu89_inline,
                   false))
    CmdArgs.push_back("-fgnu89-inline");

  if (Args.hasArg(options::OPT_fno_inline))
    CmdArgs.push_back("-fno-inline");

  if (Args.hasArg(options::OPT_fno_inline_functions))
    CmdArgs.push_back("-fno-inline-functions");

  ObjCRuntime objcRuntime = AddObjCRuntimeArgs(Args, CmdArgs, rewriteKind);

  // -fobjc-dispatch-method is only relevant with the nonfragile-abi, and
  // legacy is the default.
  if (objcRuntime.isNonFragile()) {
    if (!Args.hasFlag(options::OPT_fobjc_legacy_dispatch,
                      options::OPT_fno_objc_legacy_dispatch,
                      objcRuntime.isLegacyDispatchDefaultForArch(
                        getToolChain().getTriple().getArch()))) {
      if (getToolChain().UseObjCMixedDispatch())
        CmdArgs.push_back("-fobjc-dispatch-method=mixed");
      else
        CmdArgs.push_back("-fobjc-dispatch-method=non-legacy");
    }
  }

  // -fobjc-default-synthesize-properties=1 is default. This only has an effect
  // if the nonfragile objc abi is used.
  if (getToolChain().IsObjCDefaultSynthPropertiesDefault()) {
    CmdArgs.push_back("-fobjc-default-synthesize-properties");
  }

  // -fencode-extended-block-signature=1 is default.
  if (getToolChain().IsEncodeExtendedBlockSignatureDefault()) {
    CmdArgs.push_back("-fencode-extended-block-signature");
  }
  
  // Allow -fno-objc-arr to trump -fobjc-arr/-fobjc-arc.
  // NOTE: This logic is duplicated in ToolChains.cpp.
  bool ARC = isObjCAutoRefCount(Args);
  if (ARC) {
    getToolChain().CheckObjCARC();

    CmdArgs.push_back("-fobjc-arc");

    // FIXME: It seems like this entire block, and several around it should be
    // wrapped in isObjC, but for now we just use it here as this is where it
    // was being used previously.
    if (types::isCXX(InputType) && types::isObjC(InputType)) {
      if (getToolChain().GetCXXStdlibType(Args) == ToolChain::CST_Libcxx)
        CmdArgs.push_back("-fobjc-arc-cxxlib=libc++");
      else
        CmdArgs.push_back("-fobjc-arc-cxxlib=libstdc++");
    }

    // Allow the user to enable full exceptions code emission.
    // We define off for Objective-CC, on for Objective-C++.
    if (Args.hasFlag(options::OPT_fobjc_arc_exceptions,
                     options::OPT_fno_objc_arc_exceptions,
                     /*default*/ types::isCXX(InputType)))
      CmdArgs.push_back("-fobjc-arc-exceptions");
  }

  // -fobjc-infer-related-result-type is the default, except in the Objective-C
  // rewriter.
  if (rewriteKind != RK_None)
    CmdArgs.push_back("-fno-objc-infer-related-result-type");

  // Handle -fobjc-gc and -fobjc-gc-only. They are exclusive, and -fobjc-gc-only
  // takes precedence.
  const Arg *GCArg = Args.getLastArg(options::OPT_fobjc_gc_only);
  if (!GCArg)
    GCArg = Args.getLastArg(options::OPT_fobjc_gc);
  if (GCArg) {
    if (ARC) {
      D.Diag(diag::err_drv_objc_gc_arr)
        << GCArg->getAsString(Args);
    } else if (getToolChain().SupportsObjCGC()) {
      GCArg->render(Args, CmdArgs);
    } else {
      // FIXME: We should move this to a hard error.
      D.Diag(diag::warn_drv_objc_gc_unsupported)
        << GCArg->getAsString(Args);
    }
  }

  // Add exception args.
  addExceptionArgs(Args, InputType, getToolChain().getTriple(),
                   KernelOrKext, objcRuntime, CmdArgs);

  if (getToolChain().UseSjLjExceptions())
    CmdArgs.push_back("-fsjlj-exceptions");

  // C++ "sane" operator new.
  if (!Args.hasFlag(options::OPT_fassume_sane_operator_new,
                    options::OPT_fno_assume_sane_operator_new))
    CmdArgs.push_back("-fno-assume-sane-operator-new");

  // -fconstant-cfstrings is default, and may be subject to argument translation
  // on Darwin.
  if (!Args.hasFlag(options::OPT_fconstant_cfstrings,
                    options::OPT_fno_constant_cfstrings) ||
      !Args.hasFlag(options::OPT_mconstant_cfstrings,
                    options::OPT_mno_constant_cfstrings))
    CmdArgs.push_back("-fno-constant-cfstrings");

  // -fshort-wchar default varies depending on platform; only
  // pass if specified.
  if (Arg *A = Args.getLastArg(options::OPT_fshort_wchar))
    A->render(Args, CmdArgs);

  // -fno-pascal-strings is default, only pass non-default. If the tool chain
  // happened to translate to -mpascal-strings, we want to back translate here.
  //
  // FIXME: This is gross; that translation should be pulled from the
  // tool chain.
  if (Args.hasFlag(options::OPT_fpascal_strings,
                   options::OPT_fno_pascal_strings,
                   false) ||
      Args.hasFlag(options::OPT_mpascal_strings,
                   options::OPT_mno_pascal_strings,
                   false))
    CmdArgs.push_back("-fpascal-strings");

  // Honor -fpack-struct= and -fpack-struct, if given. Note that
  // -fno-pack-struct doesn't apply to -fpack-struct=.
  if (Arg *A = Args.getLastArg(options::OPT_fpack_struct_EQ)) {
    std::string PackStructStr = "-fpack-struct=";
    PackStructStr += A->getValue();
    CmdArgs.push_back(Args.MakeArgString(PackStructStr));
  } else if (Args.hasFlag(options::OPT_fpack_struct,
                          options::OPT_fno_pack_struct, false)) {
    CmdArgs.push_back("-fpack-struct=1");
  }

  if (Args.hasArg(options::OPT_mkernel) ||
      Args.hasArg(options::OPT_fapple_kext)) {
    if (!Args.hasArg(options::OPT_fcommon))
      CmdArgs.push_back("-fno-common");
    Args.ClaimAllArgs(options::OPT_fno_common);
  }

  // -fcommon is default, only pass non-default.
  else if (!Args.hasFlag(options::OPT_fcommon, options::OPT_fno_common))
    CmdArgs.push_back("-fno-common");

  // -fsigned-bitfields is default, and clang doesn't yet support
  // -funsigned-bitfields.
  if (!Args.hasFlag(options::OPT_fsigned_bitfields,
                    options::OPT_funsigned_bitfields))
    D.Diag(diag::warn_drv_clang_unsupported)
      << Args.getLastArg(options::OPT_funsigned_bitfields)->getAsString(Args);

  // -fsigned-bitfields is default, and clang doesn't support -fno-for-scope.
  if (!Args.hasFlag(options::OPT_ffor_scope,
                    options::OPT_fno_for_scope))
    D.Diag(diag::err_drv_clang_unsupported)
      << Args.getLastArg(options::OPT_fno_for_scope)->getAsString(Args);

  // -fcaret-diagnostics is default.
  if (!Args.hasFlag(options::OPT_fcaret_diagnostics,
                    options::OPT_fno_caret_diagnostics, true))
    CmdArgs.push_back("-fno-caret-diagnostics");

  // -fdiagnostics-fixit-info is default, only pass non-default.
  if (!Args.hasFlag(options::OPT_fdiagnostics_fixit_info,
                    options::OPT_fno_diagnostics_fixit_info))
    CmdArgs.push_back("-fno-diagnostics-fixit-info");

  // Enable -fdiagnostics-show-option by default.
  if (Args.hasFlag(options::OPT_fdiagnostics_show_option,
                   options::OPT_fno_diagnostics_show_option))
    CmdArgs.push_back("-fdiagnostics-show-option");

  if (const Arg *A =
        Args.getLastArg(options::OPT_fdiagnostics_show_category_EQ)) {
    CmdArgs.push_back("-fdiagnostics-show-category");
    CmdArgs.push_back(A->getValue());
  }

  if (const Arg *A =
        Args.getLastArg(options::OPT_fdiagnostics_format_EQ)) {
    CmdArgs.push_back("-fdiagnostics-format");
    CmdArgs.push_back(A->getValue());
  }

  if (Arg *A = Args.getLastArg(
      options::OPT_fdiagnostics_show_note_include_stack,
      options::OPT_fno_diagnostics_show_note_include_stack)) {
    if (A->getOption().matches(
        options::OPT_fdiagnostics_show_note_include_stack))
      CmdArgs.push_back("-fdiagnostics-show-note-include-stack");
    else
      CmdArgs.push_back("-fno-diagnostics-show-note-include-stack");
  }

  // Color diagnostics are the default, unless the terminal doesn't support
  // them.
  if (Args.hasFlag(options::OPT_fcolor_diagnostics,
                   options::OPT_fno_color_diagnostics,
                   llvm::sys::Process::StandardErrHasColors()))
    CmdArgs.push_back("-fcolor-diagnostics");

  if (!Args.hasFlag(options::OPT_fshow_source_location,
                    options::OPT_fno_show_source_location))
    CmdArgs.push_back("-fno-show-source-location");

  if (!Args.hasFlag(options::OPT_fshow_column,
                    options::OPT_fno_show_column,
                    true))
    CmdArgs.push_back("-fno-show-column");

  if (!Args.hasFlag(options::OPT_fspell_checking,
                    options::OPT_fno_spell_checking))
    CmdArgs.push_back("-fno-spell-checking");


  // Silently ignore -fasm-blocks for now.
  (void) Args.hasFlag(options::OPT_fasm_blocks, options::OPT_fno_asm_blocks,
                      false);

  if (Arg *A = Args.getLastArg(options::OPT_fshow_overloads_EQ))
    A->render(Args, CmdArgs);

  // -fdollars-in-identifiers default varies depending on platform and
  // language; only pass if specified.
  if (Arg *A = Args.getLastArg(options::OPT_fdollars_in_identifiers,
                               options::OPT_fno_dollars_in_identifiers)) {
    if (A->getOption().matches(options::OPT_fdollars_in_identifiers))
      CmdArgs.push_back("-fdollars-in-identifiers");
    else
      CmdArgs.push_back("-fno-dollars-in-identifiers");
  }

  // -funit-at-a-time is default, and we don't support -fno-unit-at-a-time for
  // practical purposes.
  if (Arg *A = Args.getLastArg(options::OPT_funit_at_a_time,
                               options::OPT_fno_unit_at_a_time)) {
    if (A->getOption().matches(options::OPT_fno_unit_at_a_time))
      D.Diag(diag::warn_drv_clang_unsupported) << A->getAsString(Args);
  }

  if (Args.hasFlag(options::OPT_fapple_pragma_pack,
                   options::OPT_fno_apple_pragma_pack, false))
    CmdArgs.push_back("-fapple-pragma-pack");

  // Default to -fno-builtin-str{cat,cpy} on Darwin for ARM.
  //
  // FIXME: This is disabled until clang -cc1 supports -fno-builtin-foo. PR4941.
#if 0
  if (getToolChain().getTriple().isOSDarwin() &&
      (getToolChain().getTriple().getArch() == llvm::Triple::arm ||
       getToolChain().getTriple().getArch() == llvm::Triple::thumb)) {
    if (!Args.hasArg(options::OPT_fbuiltin_strcat))
      CmdArgs.push_back("-fno-builtin-strcat");
    if (!Args.hasArg(options::OPT_fbuiltin_strcpy))
      CmdArgs.push_back("-fno-builtin-strcpy");
  }
#endif

  // Only allow -traditional or -traditional-cpp outside in preprocessing modes.
  if (Arg *A = Args.getLastArg(options::OPT_traditional,
                               options::OPT_traditional_cpp)) {
    if (isa<PreprocessJobAction>(JA))
      CmdArgs.push_back("-traditional-cpp");
    else
      D.Diag(diag::err_drv_clang_unsupported) << A->getAsString(Args);
  }

  Args.AddLastArg(CmdArgs, options::OPT_dM);
  Args.AddLastArg(CmdArgs, options::OPT_dD);
  
  // Handle serialized diagnostics.
  if (Arg *A = Args.getLastArg(options::OPT__serialize_diags)) {
    CmdArgs.push_back("-serialize-diagnostic-file");
    CmdArgs.push_back(Args.MakeArgString(A->getValue()));
  }

  if (Args.hasArg(options::OPT_fretain_comments_from_system_headers))
    CmdArgs.push_back("-fretain-comments-from-system-headers");

  // Forward -Xclang arguments to -cc1, and -mllvm arguments to the LLVM option
  // parser.
  Args.AddAllArgValues(CmdArgs, options::OPT_Xclang);
  for (arg_iterator it = Args.filtered_begin(options::OPT_mllvm),
         ie = Args.filtered_end(); it != ie; ++it) {
    (*it)->claim();

    // We translate this by hand to the -cc1 argument, since nightly test uses
    // it and developers have been trained to spell it with -mllvm.
    if (StringRef((*it)->getValue(0)) == "-disable-llvm-optzns")
      CmdArgs.push_back("-disable-llvm-optzns");
    else
      (*it)->render(Args, CmdArgs);
  }

  if (Output.getType() == types::TY_Dependencies) {
    // Handled with other dependency code.
  } else if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back("-x");
    if (Args.hasArg(options::OPT_rewrite_objc))
      CmdArgs.push_back(types::getTypeName(types::TY_PP_ObjCXX));
    else
      CmdArgs.push_back(types::getTypeName(II.getType()));
    if (II.isFilename())
      CmdArgs.push_back(II.getFilename());
    else
      II.getInputArg().renderAsInput(Args, CmdArgs);
  }

  Args.AddAllArgs(CmdArgs, options::OPT_undef);

  const char *Exec = getToolChain().getDriver().getClangProgramPath();

  // Optionally embed the -cc1 level arguments into the debug info, for build
  // analysis.
  if (getToolChain().UseDwarfDebugFlags()) {
    ArgStringList OriginalArgs;
    for (ArgList::const_iterator it = Args.begin(),
           ie = Args.end(); it != ie; ++it)
      (*it)->render(Args, OriginalArgs);

    SmallString<256> Flags;
    Flags += Exec;
    for (unsigned i = 0, e = OriginalArgs.size(); i != e; ++i) {
      Flags += " ";
      Flags += OriginalArgs[i];
    }
    CmdArgs.push_back("-dwarf-debug-flags");
    CmdArgs.push_back(Args.MakeArgString(Flags.str()));
  }

  C.addCommand(new Command(JA, *this, Exec, CmdArgs));

  if (Arg *A = Args.getLastArg(options::OPT_pg))
    if (Args.hasArg(options::OPT_fomit_frame_pointer))
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << "-fomit-frame-pointer" << A->getAsString(Args);

  // Claim some arguments which clang supports automatically.

  // -fpch-preprocess is used with gcc to add a special marker in the output to
  // include the PCH file. Clang's PTH solution is completely transparent, so we
  // do not need to deal with it at all.
  Args.ClaimAllArgs(options::OPT_fpch_preprocess);

  // Claim some arguments which clang doesn't support, but we don't
  // care to warn the user about.
  Args.ClaimAllArgs(options::OPT_clang_ignored_f_Group);
  Args.ClaimAllArgs(options::OPT_clang_ignored_m_Group);

  // Disable warnings for clang -E -use-gold-plugin -emit-llvm foo.c
  Args.ClaimAllArgs(options::OPT_use_gold_plugin);
  Args.ClaimAllArgs(options::OPT_emit_llvm);
}

void ClangAs::AddARMTargetArgs(const ArgList &Args,
                               ArgStringList &CmdArgs) const {
  const Driver &D = getToolChain().getDriver();
  llvm::Triple Triple = getToolChain().getTriple();

  // Set the CPU based on -march= and -mcpu=.
  CmdArgs.push_back("-target-cpu");
  CmdArgs.push_back(Args.MakeArgString(getARMTargetCPU(Args, Triple)));

  // Honor -mfpu=.
  if (const Arg *A = Args.getLastArg(options::OPT_mfpu_EQ))
    addFPUArgs(D, A, Args, CmdArgs);

  // Honor -mfpmath=.
  if (const Arg *A = Args.getLastArg(options::OPT_mfpmath_EQ))
    addFPMathArgs(D, A, Args, CmdArgs, getARMTargetCPU(Args, Triple));
}

/// Add options related to the Objective-C runtime/ABI.
///
/// Returns true if the runtime is non-fragile.
ObjCRuntime Clang::AddObjCRuntimeArgs(const ArgList &args,
                                      ArgStringList &cmdArgs,
                                      RewriteKind rewriteKind) const {
  // Look for the controlling runtime option.
  Arg *runtimeArg = args.getLastArg(options::OPT_fnext_runtime,
                                    options::OPT_fgnu_runtime,
                                    options::OPT_fobjc_runtime_EQ);

  // Just forward -fobjc-runtime= to the frontend.  This supercedes
  // options about fragility.
  if (runtimeArg &&
      runtimeArg->getOption().matches(options::OPT_fobjc_runtime_EQ)) {
    ObjCRuntime runtime;
    StringRef value = runtimeArg->getValue();
    if (runtime.tryParse(value)) {
      getToolChain().getDriver().Diag(diag::err_drv_unknown_objc_runtime)
        << value;
    }

    runtimeArg->render(args, cmdArgs);
    return runtime;
  }

  // Otherwise, we'll need the ABI "version".  Version numbers are
  // slightly confusing for historical reasons:
  //   1 - Traditional "fragile" ABI
  //   2 - Non-fragile ABI, version 1
  //   3 - Non-fragile ABI, version 2
  unsigned objcABIVersion = 1;
  // If -fobjc-abi-version= is present, use that to set the version.
  if (Arg *abiArg = args.getLastArg(options::OPT_fobjc_abi_version_EQ)) {
    StringRef value = abiArg->getValue();
    if (value == "1")
      objcABIVersion = 1;
    else if (value == "2")
      objcABIVersion = 2;
    else if (value == "3")
      objcABIVersion = 3;
    else
      getToolChain().getDriver().Diag(diag::err_drv_clang_unsupported)
        << value;
  } else {
    // Otherwise, determine if we are using the non-fragile ABI.
    bool nonFragileABIIsDefault = 
      (rewriteKind == RK_NonFragile || 
       (rewriteKind == RK_None &&
        getToolChain().IsObjCNonFragileABIDefault()));
    if (args.hasFlag(options::OPT_fobjc_nonfragile_abi,
                     options::OPT_fno_objc_nonfragile_abi,
                     nonFragileABIIsDefault)) {
      // Determine the non-fragile ABI version to use.
#ifdef DISABLE_DEFAULT_NONFRAGILEABI_TWO
      unsigned nonFragileABIVersion = 1;
#else
      unsigned nonFragileABIVersion = 2;
#endif

      if (Arg *abiArg = args.getLastArg(
            options::OPT_fobjc_nonfragile_abi_version_EQ)) {
        StringRef value = abiArg->getValue();
        if (value == "1")
          nonFragileABIVersion = 1;
        else if (value == "2")
          nonFragileABIVersion = 2;
        else
          getToolChain().getDriver().Diag(diag::err_drv_clang_unsupported)
            << value;
      }

      objcABIVersion = 1 + nonFragileABIVersion;
    } else {
      objcABIVersion = 1;
    }
  }

  // We don't actually care about the ABI version other than whether
  // it's non-fragile.
  bool isNonFragile = objcABIVersion != 1;

  // If we have no runtime argument, ask the toolchain for its default runtime.
  // However, the rewriter only really supports the Mac runtime, so assume that.
  ObjCRuntime runtime;
  if (!runtimeArg) {
    switch (rewriteKind) {
    case RK_None:
      runtime = getToolChain().getDefaultObjCRuntime(isNonFragile);
      break;
    case RK_Fragile:
      runtime = ObjCRuntime(ObjCRuntime::FragileMacOSX, VersionTuple());
      break;
    case RK_NonFragile:
      runtime = ObjCRuntime(ObjCRuntime::MacOSX, VersionTuple());
      break;
    }

  // -fnext-runtime
  } else if (runtimeArg->getOption().matches(options::OPT_fnext_runtime)) {
    // On Darwin, make this use the default behavior for the toolchain.
    if (getToolChain().getTriple().isOSDarwin()) {
      runtime = getToolChain().getDefaultObjCRuntime(isNonFragile);

    // Otherwise, build for a generic macosx port.
    } else {
      runtime = ObjCRuntime(ObjCRuntime::MacOSX, VersionTuple());
    }

  // -fgnu-runtime
  } else {
    assert(runtimeArg->getOption().matches(options::OPT_fgnu_runtime));
    // Legacy behaviour is to target the gnustep runtime if we are i
    // non-fragile mode or the GCC runtime in fragile mode.
    if (isNonFragile)
      runtime = ObjCRuntime(ObjCRuntime::GNUstep, VersionTuple(1,6));
    else
      runtime = ObjCRuntime(ObjCRuntime::GCC, VersionTuple());
  }

  cmdArgs.push_back(args.MakeArgString(
                                 "-fobjc-runtime=" + runtime.getAsString()));
  return runtime;
}

void ClangAs::ConstructJob(Compilation &C, const JobAction &JA,
                           const InputInfo &Output,
                           const InputInfoList &Inputs,
                           const ArgList &Args,
                           const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  assert(Inputs.size() == 1 && "Unexpected number of inputs.");
  const InputInfo &Input = Inputs[0];

  // Don't warn about "clang -w -c foo.s"
  Args.ClaimAllArgs(options::OPT_w);
  // and "clang -emit-llvm -c foo.s"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and "clang -use-gold-plugin -c foo.s"
  Args.ClaimAllArgs(options::OPT_use_gold_plugin);

  // Invoke ourselves in -cc1as mode.
  //
  // FIXME: Implement custom jobs for internal actions.
  CmdArgs.push_back("-cc1as");

  // Add the "effective" target triple.
  CmdArgs.push_back("-triple");
  std::string TripleStr = 
    getToolChain().ComputeEffectiveClangTriple(Args, Input.getType());
  CmdArgs.push_back(Args.MakeArgString(TripleStr));

  // Set the output mode, we currently only expect to be used as a real
  // assembler.
  CmdArgs.push_back("-filetype");
  CmdArgs.push_back("obj");

  if (UseRelaxAll(C, Args))
    CmdArgs.push_back("-relax-all");

  // Add target specific cpu and features flags.
  switch(getToolChain().getTriple().getArch()) {
  default:
    break;

  case llvm::Triple::arm:
  case llvm::Triple::thumb:
    AddARMTargetArgs(Args, CmdArgs);
    break;
  }

  // Ignore explicit -force_cpusubtype_ALL option.
  (void) Args.hasArg(options::OPT_force__cpusubtype__ALL);

  // Determine the original source input.
  const Action *SourceAction = &JA;
  while (SourceAction->getKind() != Action::InputClass) {
    assert(!SourceAction->getInputs().empty() && "unexpected root action!");
    SourceAction = SourceAction->getInputs()[0];
  }

  // Forward -g, assuming we are dealing with an actual assembly file.
  if (SourceAction->getType() == types::TY_Asm ||
      SourceAction->getType() == types::TY_PP_Asm) {
    Args.ClaimAllArgs(options::OPT_g_Group);
    if (Arg *A = Args.getLastArg(options::OPT_g_Group))
      if (!A->getOption().matches(options::OPT_g0))
        CmdArgs.push_back("-g");
  }

  // Optionally embed the -cc1as level arguments into the debug info, for build
  // analysis.
  if (getToolChain().UseDwarfDebugFlags()) {
    ArgStringList OriginalArgs;
    for (ArgList::const_iterator it = Args.begin(),
           ie = Args.end(); it != ie; ++it)
      (*it)->render(Args, OriginalArgs);

    SmallString<256> Flags;
    const char *Exec = getToolChain().getDriver().getClangProgramPath();
    Flags += Exec;
    for (unsigned i = 0, e = OriginalArgs.size(); i != e; ++i) {
      Flags += " ";
      Flags += OriginalArgs[i];
    }
    CmdArgs.push_back("-dwarf-debug-flags");
    CmdArgs.push_back(Args.MakeArgString(Flags.str()));
  }

  // FIXME: Add -static support, once we have it.

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);
  Args.AddAllArgs(CmdArgs, options::OPT_mllvm);

  assert(Output.isFilename() && "Unexpected lipo output.");
  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  assert(Input.isFilename() && "Invalid input.");
  CmdArgs.push_back(Input.getFilename());

  const char *Exec = getToolChain().getDriver().getClangProgramPath();
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void gcc::Common::ConstructJob(Compilation &C, const JobAction &JA,
                               const InputInfo &Output,
                               const InputInfoList &Inputs,
                               const ArgList &Args,
                               const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  for (ArgList::const_iterator
         it = Args.begin(), ie = Args.end(); it != ie; ++it) {
    Arg *A = *it;
    if (forwardToGCC(A->getOption())) {
      // Don't forward any -g arguments to assembly steps.
      if (isa<AssembleJobAction>(JA) &&
          A->getOption().matches(options::OPT_g_Group))
        continue;

      // It is unfortunate that we have to claim here, as this means
      // we will basically never report anything interesting for
      // platforms using a generic gcc, even if we are just using gcc
      // to get to the assembler.
      A->claim();
      A->render(Args, CmdArgs);
    }
  }

  RenderExtraToolArgs(JA, CmdArgs);

  // If using a driver driver, force the arch.
  llvm::Triple::ArchType Arch = getToolChain().getArch();
  if (getToolChain().getTriple().isOSDarwin()) {
    CmdArgs.push_back("-arch");

    // FIXME: Remove these special cases.
    if (Arch == llvm::Triple::ppc)
      CmdArgs.push_back("ppc");
    else if (Arch == llvm::Triple::ppc64)
      CmdArgs.push_back("ppc64");
    else
      CmdArgs.push_back(Args.MakeArgString(getToolChain().getArchName()));
  }

  // Try to force gcc to match the tool chain we want, if we recognize
  // the arch.
  //
  // FIXME: The triple class should directly provide the information we want
  // here.
  if (Arch == llvm::Triple::x86 || Arch == llvm::Triple::ppc)
    CmdArgs.push_back("-m32");
  else if (Arch == llvm::Triple::x86_64 || Arch == llvm::Triple::x86_64)
    CmdArgs.push_back("-m64");

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Unexpected output");
    CmdArgs.push_back("-fsyntax-only");
  }

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  // Only pass -x if gcc will understand it; otherwise hope gcc
  // understands the suffix correctly. The main use case this would go
  // wrong in is for linker inputs if they happened to have an odd
  // suffix; really the only way to get this to happen is a command
  // like '-x foobar a.c' which will treat a.c like a linker input.
  //
  // FIXME: For the linker case specifically, can we safely convert
  // inputs into '-Wl,' options?
  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;

    // Don't try to pass LLVM or AST inputs to a generic gcc.
    if (II.getType() == types::TY_LLVM_IR || II.getType() == types::TY_LTO_IR ||
        II.getType() == types::TY_LLVM_BC || II.getType() == types::TY_LTO_BC)
      D.Diag(diag::err_drv_no_linker_llvm_support)
        << getToolChain().getTripleString();
    else if (II.getType() == types::TY_AST)
      D.Diag(diag::err_drv_no_ast_support)
        << getToolChain().getTripleString();

    if (types::canTypeBeUserSpecified(II.getType())) {
      CmdArgs.push_back("-x");
      CmdArgs.push_back(types::getTypeName(II.getType()));
    }

    if (II.isFilename())
      CmdArgs.push_back(II.getFilename());
    else {
      const Arg &A = II.getInputArg();

      // Reverse translate some rewritten options.
      if (A.getOption().matches(options::OPT_Z_reserved_lib_stdcxx)) {
        CmdArgs.push_back("-lstdc++");
        continue;
      }

      // Don't render as input, we need gcc to do the translations.
      A.render(Args, CmdArgs);
    }
  }

  const std::string customGCCName = D.getCCCGenericGCCName();
  const char *GCCName;
  if (!customGCCName.empty())
    GCCName = customGCCName.c_str();
  else if (D.CCCIsCXX) {
    GCCName = "g++";
  } else
    GCCName = "gcc";

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath(GCCName));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void gcc::Preprocess::RenderExtraToolArgs(const JobAction &JA,
                                          ArgStringList &CmdArgs) const {
  CmdArgs.push_back("-E");
}

void gcc::Precompile::RenderExtraToolArgs(const JobAction &JA,
                                          ArgStringList &CmdArgs) const {
  // The type is good enough.
}

void gcc::Compile::RenderExtraToolArgs(const JobAction &JA,
                                       ArgStringList &CmdArgs) const {
  const Driver &D = getToolChain().getDriver();

  // If -flto, etc. are present then make sure not to force assembly output.
  if (JA.getType() == types::TY_LLVM_IR || JA.getType() == types::TY_LTO_IR ||
      JA.getType() == types::TY_LLVM_BC || JA.getType() == types::TY_LTO_BC)
    CmdArgs.push_back("-c");
  else {
    if (JA.getType() != types::TY_PP_Asm)
      D.Diag(diag::err_drv_invalid_gcc_output_type)
        << getTypeName(JA.getType());

    CmdArgs.push_back("-S");
  }
}

void gcc::Assemble::RenderExtraToolArgs(const JobAction &JA,
                                        ArgStringList &CmdArgs) const {
  CmdArgs.push_back("-c");
}

void gcc::Link::RenderExtraToolArgs(const JobAction &JA,
                                    ArgStringList &CmdArgs) const {
  // The types are (hopefully) good enough.
}

// Hexagon tools start.
void hexagon::Assemble::RenderExtraToolArgs(const JobAction &JA,
                                        ArgStringList &CmdArgs) const {

}
void hexagon::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                               const InputInfo &Output,
                               const InputInfoList &Inputs,
                               const ArgList &Args,
                               const char *LinkingOutput) const {

  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  std::string MarchString = "-march=";
  MarchString += getHexagonTargetCPU(Args);
  CmdArgs.push_back(Args.MakeArgString(MarchString));

  RenderExtraToolArgs(JA, CmdArgs);

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Unexpected output");
    CmdArgs.push_back("-fsyntax-only");
  }


  // Only pass -x if gcc will understand it; otherwise hope gcc
  // understands the suffix correctly. The main use case this would go
  // wrong in is for linker inputs if they happened to have an odd
  // suffix; really the only way to get this to happen is a command
  // like '-x foobar a.c' which will treat a.c like a linker input.
  //
  // FIXME: For the linker case specifically, can we safely convert
  // inputs into '-Wl,' options?
  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;

    // Don't try to pass LLVM or AST inputs to a generic gcc.
    if (II.getType() == types::TY_LLVM_IR || II.getType() == types::TY_LTO_IR ||
        II.getType() == types::TY_LLVM_BC || II.getType() == types::TY_LTO_BC)
      D.Diag(clang::diag::err_drv_no_linker_llvm_support)
        << getToolChain().getTripleString();
    else if (II.getType() == types::TY_AST)
      D.Diag(clang::diag::err_drv_no_ast_support)
        << getToolChain().getTripleString();

    if (II.isFilename())
      CmdArgs.push_back(II.getFilename());
    else
      // Don't render as input, we need gcc to do the translations. FIXME: Pranav: What is this ?
      II.getInputArg().render(Args, CmdArgs);
  }

  const char *GCCName = "hexagon-as";
  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath(GCCName));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));

}
void hexagon::Link::RenderExtraToolArgs(const JobAction &JA,
                                    ArgStringList &CmdArgs) const {
  // The types are (hopefully) good enough.
}

void hexagon::Link::ConstructJob(Compilation &C, const JobAction &JA,
                               const InputInfo &Output,
                               const InputInfoList &Inputs,
                               const ArgList &Args,
                               const char *LinkingOutput) const {

  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  for (ArgList::const_iterator
         it = Args.begin(), ie = Args.end(); it != ie; ++it) {
    Arg *A = *it;
    if (forwardToGCC(A->getOption())) {
      // Don't forward any -g arguments to assembly steps.
      if (isa<AssembleJobAction>(JA) &&
          A->getOption().matches(options::OPT_g_Group))
        continue;

      // It is unfortunate that we have to claim here, as this means
      // we will basically never report anything interesting for
      // platforms using a generic gcc, even if we are just using gcc
      // to get to the assembler.
      A->claim();
      A->render(Args, CmdArgs);
    }
  }

  RenderExtraToolArgs(JA, CmdArgs);

  // Add Arch Information
  Arg *A;
  if ((A = getLastHexagonArchArg(Args))) {
    if (A->getOption().matches(options::OPT_m_Joined))
      A->render(Args, CmdArgs);
    else
      CmdArgs.push_back (Args.MakeArgString("-m" + getHexagonTargetCPU(Args)));
  }
  else {
    CmdArgs.push_back (Args.MakeArgString("-m" + getHexagonTargetCPU(Args)));
  }

  CmdArgs.push_back("-mqdsp6-compat");

  const char *GCCName;
  if (C.getDriver().CCCIsCXX)
    GCCName = "hexagon-g++";
  else
    GCCName = "hexagon-gcc";
  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath(GCCName));

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  }

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;

    // Don't try to pass LLVM or AST inputs to a generic gcc.
    if (II.getType() == types::TY_LLVM_IR || II.getType() == types::TY_LTO_IR ||
        II.getType() == types::TY_LLVM_BC || II.getType() == types::TY_LTO_BC)
      D.Diag(clang::diag::err_drv_no_linker_llvm_support)
        << getToolChain().getTripleString();
    else if (II.getType() == types::TY_AST)
      D.Diag(clang::diag::err_drv_no_ast_support)
        << getToolChain().getTripleString();

    if (II.isFilename())
      CmdArgs.push_back(II.getFilename());
    else
      // Don't render as input, we need gcc to do the translations. FIXME: Pranav: What is this ?
      II.getInputArg().render(Args, CmdArgs);
  }
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));

}
// Hexagon tools end.

llvm::Triple::ArchType darwin::getArchTypeForDarwinArchName(StringRef Str) {
  // See arch(3) and llvm-gcc's driver-driver.c. We don't implement support for
  // archs which Darwin doesn't use.

  // The matching this routine does is fairly pointless, since it is neither the
  // complete architecture list, nor a reasonable subset. The problem is that
  // historically the driver driver accepts this and also ties its -march=
  // handling to the architecture name, so we need to be careful before removing
  // support for it.

  // This code must be kept in sync with Clang's Darwin specific argument
  // translation.

  return llvm::StringSwitch<llvm::Triple::ArchType>(Str)
    .Cases("ppc", "ppc601", "ppc603", "ppc604", "ppc604e", llvm::Triple::ppc)
    .Cases("ppc750", "ppc7400", "ppc7450", "ppc970", llvm::Triple::ppc)
    .Case("ppc64", llvm::Triple::ppc64)
    .Cases("i386", "i486", "i486SX", "i586", "i686", llvm::Triple::x86)
    .Cases("pentium", "pentpro", "pentIIm3", "pentIIm5", "pentium4",
           llvm::Triple::x86)
    .Case("x86_64", llvm::Triple::x86_64)
    // This is derived from the driver driver.
    .Cases("arm", "armv4t", "armv5", "armv6", llvm::Triple::arm)
    .Cases("armv7", "armv7f", "armv7k", "armv7s", "xscale", llvm::Triple::arm)
    .Case("r600", llvm::Triple::r600)
    .Case("nvptx", llvm::Triple::nvptx)
    .Case("nvptx64", llvm::Triple::nvptx64)
    .Case("amdil", llvm::Triple::amdil)
    .Case("spir", llvm::Triple::spir)
    .Default(llvm::Triple::UnknownArch);
}

const char *darwin::CC1::getCC1Name(types::ID Type) const {
  switch (Type) {
  default:
    llvm_unreachable("Unexpected type for Darwin CC1 tool.");
  case types::TY_Asm:
  case types::TY_C: case types::TY_CHeader:
  case types::TY_PP_C: case types::TY_PP_CHeader:
    return "cc1";
  case types::TY_ObjC: case types::TY_ObjCHeader:
  case types::TY_PP_ObjC: case types::TY_PP_ObjC_Alias:
  case types::TY_PP_ObjCHeader:
    return "cc1obj";
  case types::TY_CXX: case types::TY_CXXHeader:
  case types::TY_PP_CXX: case types::TY_PP_CXXHeader:
    return "cc1plus";
  case types::TY_ObjCXX: case types::TY_ObjCXXHeader:
  case types::TY_PP_ObjCXX: case types::TY_PP_ObjCXX_Alias:
  case types::TY_PP_ObjCXXHeader:
    return "cc1objplus";
  }
}

void darwin::CC1::anchor() {}

const char *darwin::CC1::getBaseInputName(const ArgList &Args,
                                          const InputInfoList &Inputs) {
  return Args.MakeArgString(
    llvm::sys::path::filename(Inputs[0].getBaseInput()));
}

const char *darwin::CC1::getBaseInputStem(const ArgList &Args,
                                          const InputInfoList &Inputs) {
  const char *Str = getBaseInputName(Args, Inputs);

  if (const char *End = strrchr(Str, '.'))
    return Args.MakeArgString(std::string(Str, End));

  return Str;
}

const char *
darwin::CC1::getDependencyFileName(const ArgList &Args,
                                   const InputInfoList &Inputs) {
  // FIXME: Think about this more.
  std::string Res;

  if (Arg *OutputOpt = Args.getLastArg(options::OPT_o)) {
    std::string Str(OutputOpt->getValue());
    Res = Str.substr(0, Str.rfind('.'));
  } else {
    Res = darwin::CC1::getBaseInputStem(Args, Inputs);
  }
  return Args.MakeArgString(Res + ".d");
}

void darwin::CC1::RemoveCC1UnsupportedArgs(ArgStringList &CmdArgs) const {
  for (ArgStringList::iterator it = CmdArgs.begin(), ie = CmdArgs.end();
       it != ie;) {

    StringRef Option = *it;
    bool RemoveOption = false;

    // Erase both -fmodule-cache-path and its argument.
    if (Option.equals("-fmodule-cache-path") && it+2 != ie) {
      it = CmdArgs.erase(it, it+2);
      ie = CmdArgs.end();
      continue;
    }

    // Remove unsupported -f options.
    if (Option.startswith("-f")) {
      // Remove -f/-fno- to reduce the number of cases.
      if (Option.startswith("-fno-"))
        Option = Option.substr(5);
      else
        Option = Option.substr(2);
      RemoveOption = llvm::StringSwitch<bool>(Option)
        .Case("altivec", true)
        .Case("modules", true)
        .Case("diagnostics-show-note-include-stack", true)
        .Default(false);
    }

    // Handle machine specific options.
    if (Option.startswith("-m")) {
      RemoveOption = llvm::StringSwitch<bool>(Option)
        .Case("-mthumb", true)
        .Case("-mno-thumb", true)
        .Case("-mno-fused-madd", true)
        .Case("-mlong-branch", true)
        .Case("-mlongcall", true)
        .Case("-mcpu=G4", true)
        .Case("-mcpu=G5", true)
        .Default(false);
    }
    
    // Handle warning options.
    if (Option.startswith("-W")) {
      // Remove -W/-Wno- to reduce the number of cases.
      if (Option.startswith("-Wno-"))
        Option = Option.substr(5);
      else
        Option = Option.substr(2);
      
      RemoveOption = llvm::StringSwitch<bool>(Option)
        .Case("address-of-temporary", true)
        .Case("ambiguous-member-template", true)
        .Case("analyzer-incompatible-plugin", true)
        .Case("array-bounds", true)
        .Case("array-bounds-pointer-arithmetic", true)
        .Case("bind-to-temporary-copy", true)
        .Case("bitwise-op-parentheses", true)
        .Case("bool-conversions", true)
        .Case("builtin-macro-redefined", true)
        .Case("c++-hex-floats", true)
        .Case("c++0x-compat", true)
        .Case("c++0x-extensions", true)
        .Case("c++0x-narrowing", true)
        .Case("c++11-compat", true)
        .Case("c++11-extensions", true)
        .Case("c++11-narrowing", true)
        .Case("conditional-uninitialized", true)
        .Case("constant-conversion", true)
        .Case("conversion-null", true)
        .Case("CFString-literal", true)
        .Case("constant-logical-operand", true)
        .Case("custom-atomic-properties", true)
        .Case("default-arg-special-member", true)
        .Case("delegating-ctor-cycles", true)
        .Case("delete-non-virtual-dtor", true)
        .Case("deprecated-implementations", true)
        .Case("deprecated-writable-strings", true)
        .Case("distributed-object-modifiers", true)
        .Case("duplicate-method-arg", true)
        .Case("dynamic-class-memaccess", true)
        .Case("enum-compare", true)
        .Case("enum-conversion", true)
        .Case("exit-time-destructors", true)
        .Case("gnu", true)
        .Case("gnu-designator", true)
        .Case("header-hygiene", true)
        .Case("idiomatic-parentheses", true)
        .Case("ignored-qualifiers", true)
        .Case("implicit-atomic-properties", true)
        .Case("incompatible-pointer-types", true)
        .Case("incomplete-implementation", true)
        .Case("int-conversion", true)
        .Case("initializer-overrides", true)
        .Case("invalid-noreturn", true)
        .Case("invalid-token-paste", true)
        .Case("language-extension-token", true)
        .Case("literal-conversion", true)
        .Case("literal-range", true)
        .Case("local-type-template-args", true)
        .Case("logical-op-parentheses", true)
        .Case("method-signatures", true)
        .Case("microsoft", true)
        .Case("mismatched-tags", true)
        .Case("missing-method-return-type", true)
        .Case("non-pod-varargs", true)
        .Case("nonfragile-abi2", true)
        .Case("null-arithmetic", true)
        .Case("null-dereference", true)
        .Case("out-of-line-declaration", true)
        .Case("overriding-method-mismatch", true)
        .Case("readonly-setter-attrs", true)
        .Case("return-stack-address", true)
        .Case("self-assign", true)
        .Case("semicolon-before-method-body", true)
        .Case("sentinel", true)
        .Case("shift-overflow", true)
        .Case("shift-sign-overflow", true)
        .Case("sign-conversion", true)
        .Case("sizeof-array-argument", true)
        .Case("sizeof-pointer-memaccess", true)
        .Case("string-compare", true)
        .Case("super-class-method-mismatch", true)
        .Case("tautological-compare", true)
        .Case("typedef-redefinition", true)
        .Case("typename-missing", true)
        .Case("undefined-reinterpret-cast", true)
        .Case("unknown-warning-option", true)
        .Case("unnamed-type-template-args", true)
        .Case("unneeded-internal-declaration", true)
        .Case("unneeded-member-function", true)
        .Case("unused-comparison", true)
        .Case("unused-exception-parameter", true)
        .Case("unused-member-function", true)
        .Case("unused-result", true)
        .Case("vector-conversions", true)
        .Case("vla", true)
        .Case("used-but-marked-unused", true)
        .Case("weak-vtables", true)
        .Default(false);
    } // if (Option.startswith("-W"))
    if (RemoveOption) {
      it = CmdArgs.erase(it);
      ie = CmdArgs.end();
    } else {
      ++it;
    }
  }
}

void darwin::CC1::AddCC1Args(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  const Driver &D = getToolChain().getDriver();

  CheckCodeGenerationOptions(D, Args);

  // Derived from cc1 spec.
  if ((!Args.hasArg(options::OPT_mkernel) ||
       (getDarwinToolChain().isTargetIPhoneOS() &&
        !getDarwinToolChain().isIPhoneOSVersionLT(6, 0))) &&
      !Args.hasArg(options::OPT_static) &&
      !Args.hasArg(options::OPT_mdynamic_no_pic))
    CmdArgs.push_back("-fPIC");

  if (getToolChain().getTriple().getArch() == llvm::Triple::arm ||
      getToolChain().getTriple().getArch() == llvm::Triple::thumb) {
    if (!Args.hasArg(options::OPT_fbuiltin_strcat))
      CmdArgs.push_back("-fno-builtin-strcat");
    if (!Args.hasArg(options::OPT_fbuiltin_strcpy))
      CmdArgs.push_back("-fno-builtin-strcpy");
  }

  if (Args.hasArg(options::OPT_g_Flag) &&
      !Args.hasArg(options::OPT_fno_eliminate_unused_debug_symbols))
    CmdArgs.push_back("-feliminate-unused-debug-symbols");
}

void darwin::CC1::AddCC1OptionsArgs(const ArgList &Args, ArgStringList &CmdArgs,
                                    const InputInfoList &Inputs,
                                    const ArgStringList &OutputArgs) const {
  const Driver &D = getToolChain().getDriver();

  // Derived from cc1_options spec.
  if (Args.hasArg(options::OPT_fast) ||
      Args.hasArg(options::OPT_fastf) ||
      Args.hasArg(options::OPT_fastcp))
    CmdArgs.push_back("-O3");

  if (Arg *A = Args.getLastArg(options::OPT_pg))
    if (Args.hasArg(options::OPT_fomit_frame_pointer))
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << A->getAsString(Args) << "-fomit-frame-pointer";

  AddCC1Args(Args, CmdArgs);

  if (!Args.hasArg(options::OPT_Q))
    CmdArgs.push_back("-quiet");

  CmdArgs.push_back("-dumpbase");
  CmdArgs.push_back(darwin::CC1::getBaseInputName(Args, Inputs));

  Args.AddAllArgs(CmdArgs, options::OPT_d_Group);

  Args.AddAllArgs(CmdArgs, options::OPT_m_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_a_Group);

  // FIXME: The goal is to use the user provided -o if that is our
  // final output, otherwise to drive from the original input
  // name. Find a clean way to go about this.
  if ((Args.hasArg(options::OPT_c) || Args.hasArg(options::OPT_S)) &&
      Args.hasArg(options::OPT_o)) {
    Arg *OutputOpt = Args.getLastArg(options::OPT_o);
    CmdArgs.push_back("-auxbase-strip");
    CmdArgs.push_back(OutputOpt->getValue());
  } else {
    CmdArgs.push_back("-auxbase");
    CmdArgs.push_back(darwin::CC1::getBaseInputStem(Args, Inputs));
  }

  Args.AddAllArgs(CmdArgs, options::OPT_g_Group);

  Args.AddAllArgs(CmdArgs, options::OPT_O);
  // FIXME: -Wall is getting some special treatment. Investigate.
  Args.AddAllArgs(CmdArgs, options::OPT_W_Group, options::OPT_pedantic_Group);
  Args.AddLastArg(CmdArgs, options::OPT_w);
  Args.AddAllArgs(CmdArgs, options::OPT_std_EQ, options::OPT_ansi,
                  options::OPT_trigraphs);
  if (!Args.getLastArg(options::OPT_std_EQ, options::OPT_ansi)) {
    // Honor -std-default.
    Args.AddAllArgsTranslated(CmdArgs, options::OPT_std_default_EQ,
                              "-std=", /*Joined=*/true);
  }

  if (Args.hasArg(options::OPT_v))
    CmdArgs.push_back("-version");
  if (Args.hasArg(options::OPT_pg) &&
      getToolChain().SupportsProfiling())
    CmdArgs.push_back("-p");
  Args.AddLastArg(CmdArgs, options::OPT_p);

  // The driver treats -fsyntax-only specially.
  if (getToolChain().getTriple().getArch() == llvm::Triple::arm ||
      getToolChain().getTriple().getArch() == llvm::Triple::thumb) {
    // Removes -fbuiltin-str{cat,cpy}; these aren't recognized by cc1 but are
    // used to inhibit the default -fno-builtin-str{cat,cpy}.
    //
    // FIXME: Should we grow a better way to deal with "removing" args?
    for (arg_iterator it = Args.filtered_begin(options::OPT_f_Group,
                                               options::OPT_fsyntax_only),
           ie = Args.filtered_end(); it != ie; ++it) {
      if (!(*it)->getOption().matches(options::OPT_fbuiltin_strcat) &&
          !(*it)->getOption().matches(options::OPT_fbuiltin_strcpy)) {
        (*it)->claim();
        (*it)->render(Args, CmdArgs);
      }
    }
  } else
    Args.AddAllArgs(CmdArgs, options::OPT_f_Group, options::OPT_fsyntax_only);

  // Claim Clang only -f options, they aren't worth warning about.
  Args.ClaimAllArgs(options::OPT_f_clang_Group);

  Args.AddAllArgs(CmdArgs, options::OPT_undef);
  if (Args.hasArg(options::OPT_Qn))
    CmdArgs.push_back("-fno-ident");

  // FIXME: This isn't correct.
  //Args.AddLastArg(CmdArgs, options::OPT__help)
  //Args.AddLastArg(CmdArgs, options::OPT__targetHelp)

  CmdArgs.append(OutputArgs.begin(), OutputArgs.end());

  // FIXME: Still don't get what is happening here. Investigate.
  Args.AddAllArgs(CmdArgs, options::OPT__param);

  if (Args.hasArg(options::OPT_fmudflap) ||
      Args.hasArg(options::OPT_fmudflapth)) {
    CmdArgs.push_back("-fno-builtin");
    CmdArgs.push_back("-fno-merge-constants");
  }

  if (Args.hasArg(options::OPT_coverage)) {
    CmdArgs.push_back("-fprofile-arcs");
    CmdArgs.push_back("-ftest-coverage");
  }

  if (types::isCXX(Inputs[0].getType()))
    CmdArgs.push_back("-D__private_extern__=extern");
}

void darwin::CC1::AddCPPOptionsArgs(const ArgList &Args, ArgStringList &CmdArgs,
                                    const InputInfoList &Inputs,
                                    const ArgStringList &OutputArgs) const {
  // Derived from cpp_options
  AddCPPUniqueOptionsArgs(Args, CmdArgs, Inputs);

  CmdArgs.append(OutputArgs.begin(), OutputArgs.end());

  AddCC1Args(Args, CmdArgs);

  // NOTE: The code below has some commonality with cpp_options, but
  // in classic gcc style ends up sending things in different
  // orders. This may be a good merge candidate once we drop pedantic
  // compatibility.

  Args.AddAllArgs(CmdArgs, options::OPT_m_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_std_EQ, options::OPT_ansi,
                  options::OPT_trigraphs);
  if (!Args.getLastArg(options::OPT_std_EQ, options::OPT_ansi)) {
    // Honor -std-default.
    Args.AddAllArgsTranslated(CmdArgs, options::OPT_std_default_EQ,
                              "-std=", /*Joined=*/true);
  }
  Args.AddAllArgs(CmdArgs, options::OPT_W_Group, options::OPT_pedantic_Group);
  Args.AddLastArg(CmdArgs, options::OPT_w);

  // The driver treats -fsyntax-only specially.
  Args.AddAllArgs(CmdArgs, options::OPT_f_Group, options::OPT_fsyntax_only);

  // Claim Clang only -f options, they aren't worth warning about.
  Args.ClaimAllArgs(options::OPT_f_clang_Group);

  if (Args.hasArg(options::OPT_g_Group) && !Args.hasArg(options::OPT_g0) &&
      !Args.hasArg(options::OPT_fno_working_directory))
    CmdArgs.push_back("-fworking-directory");

  Args.AddAllArgs(CmdArgs, options::OPT_O);
  Args.AddAllArgs(CmdArgs, options::OPT_undef);
  if (Args.hasArg(options::OPT_save_temps))
    CmdArgs.push_back("-fpch-preprocess");
}

void darwin::CC1::AddCPPUniqueOptionsArgs(const ArgList &Args,
                                          ArgStringList &CmdArgs,
                                          const InputInfoList &Inputs) const {
  const Driver &D = getToolChain().getDriver();

  CheckPreprocessingOptions(D, Args);

  // Derived from cpp_unique_options.
  // -{C,CC} only with -E is checked in CheckPreprocessingOptions().
  Args.AddLastArg(CmdArgs, options::OPT_C);
  Args.AddLastArg(CmdArgs, options::OPT_CC);
  if (!Args.hasArg(options::OPT_Q))
    CmdArgs.push_back("-quiet");
  Args.AddAllArgs(CmdArgs, options::OPT_nostdinc);
  Args.AddAllArgs(CmdArgs, options::OPT_nostdincxx);
  Args.AddLastArg(CmdArgs, options::OPT_v);
  Args.AddAllArgs(CmdArgs, options::OPT_I_Group, options::OPT_F);
  Args.AddLastArg(CmdArgs, options::OPT_P);

  // FIXME: Handle %I properly.
  if (getToolChain().getArch() == llvm::Triple::x86_64) {
    CmdArgs.push_back("-imultilib");
    CmdArgs.push_back("x86_64");
  }

  if (Args.hasArg(options::OPT_MD)) {
    CmdArgs.push_back("-MD");
    CmdArgs.push_back(darwin::CC1::getDependencyFileName(Args, Inputs));
  }

  if (Args.hasArg(options::OPT_MMD)) {
    CmdArgs.push_back("-MMD");
    CmdArgs.push_back(darwin::CC1::getDependencyFileName(Args, Inputs));
  }

  Args.AddLastArg(CmdArgs, options::OPT_M);
  Args.AddLastArg(CmdArgs, options::OPT_MM);
  Args.AddAllArgs(CmdArgs, options::OPT_MF);
  Args.AddLastArg(CmdArgs, options::OPT_MG);
  Args.AddLastArg(CmdArgs, options::OPT_MP);
  Args.AddAllArgs(CmdArgs, options::OPT_MQ);
  Args.AddAllArgs(CmdArgs, options::OPT_MT);
  if (!Args.hasArg(options::OPT_M) && !Args.hasArg(options::OPT_MM) &&
      (Args.hasArg(options::OPT_MD) || Args.hasArg(options::OPT_MMD))) {
    if (Arg *OutputOpt = Args.getLastArg(options::OPT_o)) {
      CmdArgs.push_back("-MQ");
      CmdArgs.push_back(OutputOpt->getValue());
    }
  }

  Args.AddLastArg(CmdArgs, options::OPT_remap);
  if (Args.hasArg(options::OPT_g3))
    CmdArgs.push_back("-dD");
  Args.AddLastArg(CmdArgs, options::OPT_H);

  AddCPPArgs(Args, CmdArgs);

  Args.AddAllArgs(CmdArgs, options::OPT_D, options::OPT_U, options::OPT_A);
  Args.AddAllArgs(CmdArgs, options::OPT_i_Group);

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;

    CmdArgs.push_back(II.getFilename());
  }

  Args.AddAllArgValues(CmdArgs, options::OPT_Wp_COMMA,
                       options::OPT_Xpreprocessor);

  if (Args.hasArg(options::OPT_fmudflap)) {
    CmdArgs.push_back("-D_MUDFLAP");
    CmdArgs.push_back("-include");
    CmdArgs.push_back("mf-runtime.h");
  }

  if (Args.hasArg(options::OPT_fmudflapth)) {
    CmdArgs.push_back("-D_MUDFLAP");
    CmdArgs.push_back("-D_MUDFLAPTH");
    CmdArgs.push_back("-include");
    CmdArgs.push_back("mf-runtime.h");
  }
}

void darwin::CC1::AddCPPArgs(const ArgList &Args,
                             ArgStringList &CmdArgs) const {
  // Derived from cpp spec.

  if (Args.hasArg(options::OPT_static)) {
    // The gcc spec is broken here, it refers to dynamic but
    // that has been translated. Start by being bug compatible.

    // if (!Args.hasArg(arglist.parser.dynamicOption))
    CmdArgs.push_back("-D__STATIC__");
  } else
    CmdArgs.push_back("-D__DYNAMIC__");

  if (Args.hasArg(options::OPT_pthread))
    CmdArgs.push_back("-D_REENTRANT");
}

void darwin::Preprocess::ConstructJob(Compilation &C, const JobAction &JA,
                                      const InputInfo &Output,
                                      const InputInfoList &Inputs,
                                      const ArgList &Args,
                                      const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  assert(Inputs.size() == 1 && "Unexpected number of inputs!");

  CmdArgs.push_back("-E");

  if (Args.hasArg(options::OPT_traditional) ||
      Args.hasArg(options::OPT_traditional_cpp))
    CmdArgs.push_back("-traditional-cpp");

  ArgStringList OutputArgs;
  assert(Output.isFilename() && "Unexpected CC1 output.");
  OutputArgs.push_back("-o");
  OutputArgs.push_back(Output.getFilename());

  if (Args.hasArg(options::OPT_E) || getToolChain().getDriver().CCCIsCPP) {
    AddCPPOptionsArgs(Args, CmdArgs, Inputs, OutputArgs);
  } else {
    AddCPPOptionsArgs(Args, CmdArgs, Inputs, ArgStringList());
    CmdArgs.append(OutputArgs.begin(), OutputArgs.end());
  }

  Args.AddAllArgs(CmdArgs, options::OPT_d_Group);

  RemoveCC1UnsupportedArgs(CmdArgs);

  const char *CC1Name = getCC1Name(Inputs[0].getType());
  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath(CC1Name));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::Compile::ConstructJob(Compilation &C, const JobAction &JA,
                                   const InputInfo &Output,
                                   const InputInfoList &Inputs,
                                   const ArgList &Args,
                                   const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  assert(Inputs.size() == 1 && "Unexpected number of inputs!");

  // Silence warning about unused --serialize-diagnostics
  Args.ClaimAllArgs(options::OPT__serialize_diags);

  types::ID InputType = Inputs[0].getType();
  if (const Arg *A = Args.getLastArg(options::OPT_traditional))
    D.Diag(diag::err_drv_argument_only_allowed_with)
      << A->getAsString(Args) << "-E";

  if (JA.getType() == types::TY_LLVM_IR ||
      JA.getType() == types::TY_LTO_IR)
    CmdArgs.push_back("-emit-llvm");
  else if (JA.getType() == types::TY_LLVM_BC ||
           JA.getType() == types::TY_LTO_BC)
    CmdArgs.push_back("-emit-llvm-bc");
  else if (Output.getType() == types::TY_AST)
    D.Diag(diag::err_drv_no_ast_support)
      << getToolChain().getTripleString();
  else if (JA.getType() != types::TY_PP_Asm &&
           JA.getType() != types::TY_PCH)
    D.Diag(diag::err_drv_invalid_gcc_output_type)
      << getTypeName(JA.getType());

  ArgStringList OutputArgs;
  if (Output.getType() != types::TY_PCH) {
    OutputArgs.push_back("-o");
    if (Output.isNothing())
      OutputArgs.push_back("/dev/null");
    else
      OutputArgs.push_back(Output.getFilename());
  }

  // There is no need for this level of compatibility, but it makes
  // diffing easier.
  bool OutputArgsEarly = (Args.hasArg(options::OPT_fsyntax_only) ||
                          Args.hasArg(options::OPT_S));

  if (types::getPreprocessedType(InputType) != types::TY_INVALID) {
    AddCPPUniqueOptionsArgs(Args, CmdArgs, Inputs);
    if (OutputArgsEarly) {
      AddCC1OptionsArgs(Args, CmdArgs, Inputs, OutputArgs);
    } else {
      AddCC1OptionsArgs(Args, CmdArgs, Inputs, ArgStringList());
      CmdArgs.append(OutputArgs.begin(), OutputArgs.end());
    }
  } else {
    CmdArgs.push_back("-fpreprocessed");

    for (InputInfoList::const_iterator
           it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
      const InputInfo &II = *it;

      // Reject AST inputs.
      if (II.getType() == types::TY_AST) {
        D.Diag(diag::err_drv_no_ast_support)
          << getToolChain().getTripleString();
        return;
      }

      CmdArgs.push_back(II.getFilename());
    }

    if (OutputArgsEarly) {
      AddCC1OptionsArgs(Args, CmdArgs, Inputs, OutputArgs);
    } else {
      AddCC1OptionsArgs(Args, CmdArgs, Inputs, ArgStringList());
      CmdArgs.append(OutputArgs.begin(), OutputArgs.end());
    }
  }

  if (Output.getType() == types::TY_PCH) {
    assert(Output.isFilename() && "Invalid PCH output.");

    CmdArgs.push_back("-o");
    // NOTE: gcc uses a temp .s file for this, but there doesn't seem
    // to be a good reason.
    const char *TmpPath = C.getArgs().MakeArgString(
      D.GetTemporaryPath("cc", "s"));
    C.addTempFile(TmpPath);
    CmdArgs.push_back(TmpPath);

    // If we're emitting a pch file with the last 4 characters of ".pth"
    // and falling back to llvm-gcc we want to use ".gch" instead.
    std::string OutputFile(Output.getFilename());
    size_t loc = OutputFile.rfind(".pth");
    if (loc != std::string::npos)
      OutputFile.replace(loc, 4, ".gch");
    const char *Tmp = C.getArgs().MakeArgString("--output-pch="+OutputFile);
    CmdArgs.push_back(Tmp);
  }

  RemoveCC1UnsupportedArgs(CmdArgs);

  const char *CC1Name = getCC1Name(Inputs[0].getType());
  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath(CC1Name));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  assert(Inputs.size() == 1 && "Unexpected number of inputs.");
  const InputInfo &Input = Inputs[0];

  // Determine the original source input.
  const Action *SourceAction = &JA;
  while (SourceAction->getKind() != Action::InputClass) {
    assert(!SourceAction->getInputs().empty() && "unexpected root action!");
    SourceAction = SourceAction->getInputs()[0];
  }

  // Forward -g, assuming we are dealing with an actual assembly file.
  if (SourceAction->getType() == types::TY_Asm ||
      SourceAction->getType() == types::TY_PP_Asm) {
    if (Args.hasArg(options::OPT_gstabs))
      CmdArgs.push_back("--gstabs");
    else if (Args.hasArg(options::OPT_g_Group))
      CmdArgs.push_back("-g");
  }

  // Derived from asm spec.
  AddDarwinArch(Args, CmdArgs);

  // Use -force_cpusubtype_ALL on x86 by default.
  if (getToolChain().getTriple().getArch() == llvm::Triple::x86 ||
      getToolChain().getTriple().getArch() == llvm::Triple::x86_64 ||
      Args.hasArg(options::OPT_force__cpusubtype__ALL))
    CmdArgs.push_back("-force_cpusubtype_ALL");

  if (getToolChain().getTriple().getArch() != llvm::Triple::x86_64 &&
      (((Args.hasArg(options::OPT_mkernel) ||
         Args.hasArg(options::OPT_fapple_kext)) &&
        (!getDarwinToolChain().isTargetIPhoneOS() ||
         getDarwinToolChain().isIPhoneOSVersionLT(6, 0))) ||
       Args.hasArg(options::OPT_static)))
    CmdArgs.push_back("-static");

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  assert(Output.isFilename() && "Unexpected lipo output.");
  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  assert(Input.isFilename() && "Invalid input.");
  CmdArgs.push_back(Input.getFilename());

  // asm_final spec is empty.

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::DarwinTool::anchor() {}

void darwin::DarwinTool::AddDarwinArch(const ArgList &Args,
                                       ArgStringList &CmdArgs) const {
  StringRef ArchName = getDarwinToolChain().getDarwinArchName(Args);

  // Derived from darwin_arch spec.
  CmdArgs.push_back("-arch");
  CmdArgs.push_back(Args.MakeArgString(ArchName));

  // FIXME: Is this needed anymore?
  if (ArchName == "arm")
    CmdArgs.push_back("-force_cpusubtype_ALL");
}

bool darwin::Link::NeedsTempPath(const InputInfoList &Inputs) const {
  // We only need to generate a temp path for LTO if we aren't compiling object
  // files. When compiling source files, we run 'dsymutil' after linking. We
  // don't run 'dsymutil' when compiling object files.
  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it)
    if (it->getType() != types::TY_Object)
      return true;

  return false;
}

void darwin::Link::AddLinkArgs(Compilation &C,
                               const ArgList &Args,
                               ArgStringList &CmdArgs,
                               const InputInfoList &Inputs) const {
  const Driver &D = getToolChain().getDriver();
  const toolchains::Darwin &DarwinTC = getDarwinToolChain();

  unsigned Version[3] = { 0, 0, 0 };
  if (Arg *A = Args.getLastArg(options::OPT_mlinker_version_EQ)) {
    bool HadExtra;
    if (!Driver::GetReleaseVersion(A->getValue(), Version[0],
                                   Version[1], Version[2], HadExtra) ||
        HadExtra)
      D.Diag(diag::err_drv_invalid_version_number)
        << A->getAsString(Args);
  }

  // Newer linkers support -demangle, pass it if supported and not disabled by
  // the user.
  if (Version[0] >= 100 && !Args.hasArg(options::OPT_Z_Xlinker__no_demangle)) {
    // Don't pass -demangle to ld_classic.
    //
    // FIXME: This is a temporary workaround, ld should be handling this.
    bool UsesLdClassic = (getToolChain().getArch() == llvm::Triple::x86 &&
                          Args.hasArg(options::OPT_static));
    if (getToolChain().getArch() == llvm::Triple::x86) {
      for (arg_iterator it = Args.filtered_begin(options::OPT_Xlinker,
                                                 options::OPT_Wl_COMMA),
             ie = Args.filtered_end(); it != ie; ++it) {
        const Arg *A = *it;
        for (unsigned i = 0, e = A->getNumValues(); i != e; ++i)
          if (StringRef(A->getValue(i)) == "-kext")
            UsesLdClassic = true;
      }
    }
    if (!UsesLdClassic)
      CmdArgs.push_back("-demangle");
  }

  // If we are using LTO, then automatically create a temporary file path for
  // the linker to use, so that it's lifetime will extend past a possible
  // dsymutil step.
  if (Version[0] >= 116 && D.IsUsingLTO(Args) && NeedsTempPath(Inputs)) {
    const char *TmpPath = C.getArgs().MakeArgString(
      D.GetTemporaryPath("cc", types::getTypeTempSuffix(types::TY_Object)));
    C.addTempFile(TmpPath);
    CmdArgs.push_back("-object_path_lto");
    CmdArgs.push_back(TmpPath);
  }

  // Derived from the "link" spec.
  Args.AddAllArgs(CmdArgs, options::OPT_static);
  if (!Args.hasArg(options::OPT_static))
    CmdArgs.push_back("-dynamic");
  if (Args.hasArg(options::OPT_fgnu_runtime)) {
    // FIXME: gcc replaces -lobjc in forward args with -lobjc-gnu
    // here. How do we wish to handle such things?
  }

  if (!Args.hasArg(options::OPT_dynamiclib)) {
    AddDarwinArch(Args, CmdArgs);
    // FIXME: Why do this only on this path?
    Args.AddLastArg(CmdArgs, options::OPT_force__cpusubtype__ALL);

    Args.AddLastArg(CmdArgs, options::OPT_bundle);
    Args.AddAllArgs(CmdArgs, options::OPT_bundle__loader);
    Args.AddAllArgs(CmdArgs, options::OPT_client__name);

    Arg *A;
    if ((A = Args.getLastArg(options::OPT_compatibility__version)) ||
        (A = Args.getLastArg(options::OPT_current__version)) ||
        (A = Args.getLastArg(options::OPT_install__name)))
      D.Diag(diag::err_drv_argument_only_allowed_with)
        << A->getAsString(Args) << "-dynamiclib";

    Args.AddLastArg(CmdArgs, options::OPT_force__flat__namespace);
    Args.AddLastArg(CmdArgs, options::OPT_keep__private__externs);
    Args.AddLastArg(CmdArgs, options::OPT_private__bundle);
  } else {
    CmdArgs.push_back("-dylib");

    Arg *A;
    if ((A = Args.getLastArg(options::OPT_bundle)) ||
        (A = Args.getLastArg(options::OPT_bundle__loader)) ||
        (A = Args.getLastArg(options::OPT_client__name)) ||
        (A = Args.getLastArg(options::OPT_force__flat__namespace)) ||
        (A = Args.getLastArg(options::OPT_keep__private__externs)) ||
        (A = Args.getLastArg(options::OPT_private__bundle)))
      D.Diag(diag::err_drv_argument_not_allowed_with)
        << A->getAsString(Args) << "-dynamiclib";

    Args.AddAllArgsTranslated(CmdArgs, options::OPT_compatibility__version,
                              "-dylib_compatibility_version");
    Args.AddAllArgsTranslated(CmdArgs, options::OPT_current__version,
                              "-dylib_current_version");

    AddDarwinArch(Args, CmdArgs);

    Args.AddAllArgsTranslated(CmdArgs, options::OPT_install__name,
                              "-dylib_install_name");
  }

  Args.AddLastArg(CmdArgs, options::OPT_all__load);
  Args.AddAllArgs(CmdArgs, options::OPT_allowable__client);
  Args.AddLastArg(CmdArgs, options::OPT_bind__at__load);
  if (DarwinTC.isTargetIPhoneOS())
    Args.AddLastArg(CmdArgs, options::OPT_arch__errors__fatal);
  Args.AddLastArg(CmdArgs, options::OPT_dead__strip);
  Args.AddLastArg(CmdArgs, options::OPT_no__dead__strip__inits__and__terms);
  Args.AddAllArgs(CmdArgs, options::OPT_dylib__file);
  Args.AddLastArg(CmdArgs, options::OPT_dynamic);
  Args.AddAllArgs(CmdArgs, options::OPT_exported__symbols__list);
  Args.AddLastArg(CmdArgs, options::OPT_flat__namespace);
  Args.AddAllArgs(CmdArgs, options::OPT_force__load);
  Args.AddAllArgs(CmdArgs, options::OPT_headerpad__max__install__names);
  Args.AddAllArgs(CmdArgs, options::OPT_image__base);
  Args.AddAllArgs(CmdArgs, options::OPT_init);

  // Add the deployment target.
  VersionTuple TargetVersion = DarwinTC.getTargetVersion();

  // If we had an explicit -mios-simulator-version-min argument, honor that,
  // otherwise use the traditional deployment targets. We can't just check the
  // is-sim attribute because existing code follows this path, and the linker
  // may not handle the argument.
  //
  // FIXME: We may be able to remove this, once we can verify no one depends on
  // it.
  if (Args.hasArg(options::OPT_mios_simulator_version_min_EQ))
    CmdArgs.push_back("-ios_simulator_version_min");
  else if (DarwinTC.isTargetIPhoneOS())
    CmdArgs.push_back("-iphoneos_version_min");
  else
    CmdArgs.push_back("-macosx_version_min");
  CmdArgs.push_back(Args.MakeArgString(TargetVersion.getAsString()));

  Args.AddLastArg(CmdArgs, options::OPT_nomultidefs);
  Args.AddLastArg(CmdArgs, options::OPT_multi__module);
  Args.AddLastArg(CmdArgs, options::OPT_single__module);
  Args.AddAllArgs(CmdArgs, options::OPT_multiply__defined);
  Args.AddAllArgs(CmdArgs, options::OPT_multiply__defined__unused);

  if (const Arg *A = Args.getLastArg(options::OPT_fpie, options::OPT_fPIE,
                                     options::OPT_fno_pie,
                                     options::OPT_fno_PIE)) {
    if (A->getOption().matches(options::OPT_fpie) ||
        A->getOption().matches(options::OPT_fPIE))
      CmdArgs.push_back("-pie");
    else
      CmdArgs.push_back("-no_pie");
  }

  Args.AddLastArg(CmdArgs, options::OPT_prebind);
  Args.AddLastArg(CmdArgs, options::OPT_noprebind);
  Args.AddLastArg(CmdArgs, options::OPT_nofixprebinding);
  Args.AddLastArg(CmdArgs, options::OPT_prebind__all__twolevel__modules);
  Args.AddLastArg(CmdArgs, options::OPT_read__only__relocs);
  Args.AddAllArgs(CmdArgs, options::OPT_sectcreate);
  Args.AddAllArgs(CmdArgs, options::OPT_sectorder);
  Args.AddAllArgs(CmdArgs, options::OPT_seg1addr);
  Args.AddAllArgs(CmdArgs, options::OPT_segprot);
  Args.AddAllArgs(CmdArgs, options::OPT_segaddr);
  Args.AddAllArgs(CmdArgs, options::OPT_segs__read__only__addr);
  Args.AddAllArgs(CmdArgs, options::OPT_segs__read__write__addr);
  Args.AddAllArgs(CmdArgs, options::OPT_seg__addr__table);
  Args.AddAllArgs(CmdArgs, options::OPT_seg__addr__table__filename);
  Args.AddAllArgs(CmdArgs, options::OPT_sub__library);
  Args.AddAllArgs(CmdArgs, options::OPT_sub__umbrella);

  // Give --sysroot= preference, over the Apple specific behavior to also use
  // --isysroot as the syslibroot.
  StringRef sysroot = C.getSysRoot();
  if (sysroot != "") {
    CmdArgs.push_back("-syslibroot");
    CmdArgs.push_back(C.getArgs().MakeArgString(sysroot));
  } else if (const Arg *A = Args.getLastArg(options::OPT_isysroot)) {
    CmdArgs.push_back("-syslibroot");
    CmdArgs.push_back(A->getValue());
  }

  Args.AddLastArg(CmdArgs, options::OPT_twolevel__namespace);
  Args.AddLastArg(CmdArgs, options::OPT_twolevel__namespace__hints);
  Args.AddAllArgs(CmdArgs, options::OPT_umbrella);
  Args.AddAllArgs(CmdArgs, options::OPT_undefined);
  Args.AddAllArgs(CmdArgs, options::OPT_unexported__symbols__list);
  Args.AddAllArgs(CmdArgs, options::OPT_weak__reference__mismatches);
  Args.AddLastArg(CmdArgs, options::OPT_X_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_y);
  Args.AddLastArg(CmdArgs, options::OPT_w);
  Args.AddAllArgs(CmdArgs, options::OPT_pagezero__size);
  Args.AddAllArgs(CmdArgs, options::OPT_segs__read__);
  Args.AddLastArg(CmdArgs, options::OPT_seglinkedit);
  Args.AddLastArg(CmdArgs, options::OPT_noseglinkedit);
  Args.AddAllArgs(CmdArgs, options::OPT_sectalign);
  Args.AddAllArgs(CmdArgs, options::OPT_sectobjectsymbols);
  Args.AddAllArgs(CmdArgs, options::OPT_segcreate);
  Args.AddLastArg(CmdArgs, options::OPT_whyload);
  Args.AddLastArg(CmdArgs, options::OPT_whatsloaded);
  Args.AddAllArgs(CmdArgs, options::OPT_dylinker__install__name);
  Args.AddLastArg(CmdArgs, options::OPT_dylinker);
  Args.AddLastArg(CmdArgs, options::OPT_Mach);
}

void darwin::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                const InputInfo &Output,
                                const InputInfoList &Inputs,
                                const ArgList &Args,
                                const char *LinkingOutput) const {
  assert(Output.getType() == types::TY_Image && "Invalid linker output type.");

  // The logic here is derived from gcc's behavior; most of which
  // comes from specs (starting with link_command). Consult gcc for
  // more information.
  ArgStringList CmdArgs;

  /// Hack(tm) to ignore linking errors when we are doing ARC migration.
  if (Args.hasArg(options::OPT_ccc_arcmt_check,
                  options::OPT_ccc_arcmt_migrate)) {
    for (ArgList::const_iterator I = Args.begin(), E = Args.end(); I != E; ++I)
      (*I)->claim();
    const char *Exec =
      Args.MakeArgString(getToolChain().GetProgramPath("touch"));
    CmdArgs.push_back(Output.getFilename());
    C.addCommand(new Command(JA, *this, Exec, CmdArgs));
    return;
  }

  // I'm not sure why this particular decomposition exists in gcc, but
  // we follow suite for ease of comparison.
  AddLinkArgs(C, Args, CmdArgs, Inputs);

  Args.AddAllArgs(CmdArgs, options::OPT_d_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_s);
  Args.AddAllArgs(CmdArgs, options::OPT_t);
  Args.AddAllArgs(CmdArgs, options::OPT_Z_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_u_Group);
  Args.AddLastArg(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_m_Separate);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  // Forward -ObjC when either -ObjC or -ObjC++ is used, to force loading
  // members of static archive libraries which implement Objective-C classes or
  // categories.
  if (Args.hasArg(options::OPT_ObjC) || Args.hasArg(options::OPT_ObjCXX))
    CmdArgs.push_back("-ObjC");

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    // Derived from startfile spec.
    if (Args.hasArg(options::OPT_dynamiclib)) {
      // Derived from darwin_dylib1 spec.
      if (getDarwinToolChain().isTargetIOSSimulator()) {
        // The simulator doesn't have a versioned crt1 file.
        CmdArgs.push_back("-ldylib1.o");
      } else if (getDarwinToolChain().isTargetIPhoneOS()) {
        if (getDarwinToolChain().isIPhoneOSVersionLT(3, 1))
          CmdArgs.push_back("-ldylib1.o");
      } else {
        if (getDarwinToolChain().isMacosxVersionLT(10, 5))
          CmdArgs.push_back("-ldylib1.o");
        else if (getDarwinToolChain().isMacosxVersionLT(10, 6))
          CmdArgs.push_back("-ldylib1.10.5.o");
      }
    } else {
      if (Args.hasArg(options::OPT_bundle)) {
        if (!Args.hasArg(options::OPT_static)) {
          // Derived from darwin_bundle1 spec.
          if (getDarwinToolChain().isTargetIOSSimulator()) {
            // The simulator doesn't have a versioned crt1 file.
            CmdArgs.push_back("-lbundle1.o");
          } else if (getDarwinToolChain().isTargetIPhoneOS()) {
            if (getDarwinToolChain().isIPhoneOSVersionLT(3, 1))
              CmdArgs.push_back("-lbundle1.o");
          } else {
            if (getDarwinToolChain().isMacosxVersionLT(10, 6))
              CmdArgs.push_back("-lbundle1.o");
          }
        }
      } else {
        if (Args.hasArg(options::OPT_pg) &&
            getToolChain().SupportsProfiling()) {
          if (Args.hasArg(options::OPT_static) ||
              Args.hasArg(options::OPT_object) ||
              Args.hasArg(options::OPT_preload)) {
            CmdArgs.push_back("-lgcrt0.o");
          } else {
            CmdArgs.push_back("-lgcrt1.o");

            // darwin_crt2 spec is empty.
          }
          // By default on OS X 10.8 and later, we don't link with a crt1.o
          // file and the linker knows to use _main as the entry point.  But,
          // when compiling with -pg, we need to link with the gcrt1.o file,
          // so pass the -no_new_main option to tell the linker to use the
          // "start" symbol as the entry point.
          if (getDarwinToolChain().isTargetMacOS() &&
              !getDarwinToolChain().isMacosxVersionLT(10, 8))
            CmdArgs.push_back("-no_new_main");
        } else {
          if (Args.hasArg(options::OPT_static) ||
              Args.hasArg(options::OPT_object) ||
              Args.hasArg(options::OPT_preload)) {
            CmdArgs.push_back("-lcrt0.o");
          } else {
            // Derived from darwin_crt1 spec.
            if (getDarwinToolChain().isTargetIOSSimulator()) {
              // The simulator doesn't have a versioned crt1 file.
              CmdArgs.push_back("-lcrt1.o");
            } else if (getDarwinToolChain().isTargetIPhoneOS()) {
              if (getDarwinToolChain().isIPhoneOSVersionLT(3, 1))
                CmdArgs.push_back("-lcrt1.o");
              else if (getDarwinToolChain().isIPhoneOSVersionLT(6, 0))
                CmdArgs.push_back("-lcrt1.3.1.o");
            } else {
              if (getDarwinToolChain().isMacosxVersionLT(10, 5))
                CmdArgs.push_back("-lcrt1.o");
              else if (getDarwinToolChain().isMacosxVersionLT(10, 6))
                CmdArgs.push_back("-lcrt1.10.5.o");
              else if (getDarwinToolChain().isMacosxVersionLT(10, 8))
                CmdArgs.push_back("-lcrt1.10.6.o");

              // darwin_crt2 spec is empty.
            }
          }
        }
      }
    }

    if (!getDarwinToolChain().isTargetIPhoneOS() &&
        Args.hasArg(options::OPT_shared_libgcc) &&
        getDarwinToolChain().isMacosxVersionLT(10, 5)) {
      const char *Str =
        Args.MakeArgString(getToolChain().GetFilePath("crt3.o"));
      CmdArgs.push_back(Str);
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);

  SanitizerArgs Sanitize(getToolChain().getDriver(), Args);
  // If we're building a dynamic lib with -fsanitize=address, or
  // -fsanitize=undefined, unresolved symbols may appear. Mark all
  // of them as dynamic_lookup. Linking executables is handled in
  // lib/Driver/ToolChains.cpp.
  if (Sanitize.needsAsanRt() || Sanitize.needsUbsanRt()) {
    if (Args.hasArg(options::OPT_dynamiclib) ||
        Args.hasArg(options::OPT_bundle)) {
      CmdArgs.push_back("-undefined");
      CmdArgs.push_back("dynamic_lookup");
    }
  }

  if (Args.hasArg(options::OPT_fopenmp))
    // This is more complicated in gcc...
    CmdArgs.push_back("-lgomp");

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);
  
  if (isObjCRuntimeLinked(Args) &&
      !Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    // Avoid linking compatibility stubs on i386 mac.
    if (!getDarwinToolChain().isTargetMacOS() ||
        getDarwinToolChain().getArch() != llvm::Triple::x86) {
      // If we don't have ARC or subscripting runtime support, link in the
      // runtime stubs.  We have to do this *before* adding any of the normal
      // linker inputs so that its initializer gets run first.
      ObjCRuntime runtime =
        getDarwinToolChain().getDefaultObjCRuntime(/*nonfragile*/ true);
      // We use arclite library for both ARC and subscripting support.
      if ((!runtime.hasNativeARC() && isObjCAutoRefCount(Args)) ||
          !runtime.hasSubscripting())
        getDarwinToolChain().AddLinkARCArgs(Args, CmdArgs);
    }
    CmdArgs.push_back("-framework");
    CmdArgs.push_back("Foundation");
    // Link libobj.
    CmdArgs.push_back("-lobjc");
  }

  if (LinkingOutput) {
    CmdArgs.push_back("-arch_multiple");
    CmdArgs.push_back("-final_output");
    CmdArgs.push_back(LinkingOutput);
  }

  if (Args.hasArg(options::OPT_fnested_functions))
    CmdArgs.push_back("-allow_stack_execute");

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (getToolChain().getDriver().CCCIsCXX)
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);

    // link_ssp spec is empty.

    // Let the tool chain choose which runtime library to link.
    getDarwinToolChain().AddLinkRuntimeLibArgs(Args, CmdArgs);
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    // endfile_spec is empty.
  }

  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_F);

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::Lipo::ConstructJob(Compilation &C, const JobAction &JA,
                                const InputInfo &Output,
                                const InputInfoList &Inputs,
                                const ArgList &Args,
                                const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  CmdArgs.push_back("-create");
  assert(Output.isFilename() && "Unexpected lipo output.");

  CmdArgs.push_back("-output");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    assert(II.isFilename() && "Unexpected lipo input.");
    CmdArgs.push_back(II.getFilename());
  }
  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("lipo"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::Dsymutil::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  assert(Inputs.size() == 1 && "Unable to handle multiple inputs.");
  const InputInfo &Input = Inputs[0];
  assert(Input.isFilename() && "Unexpected dsymutil input.");
  CmdArgs.push_back(Input.getFilename());

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("dsymutil"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void darwin::VerifyDebug::ConstructJob(Compilation &C, const JobAction &JA,
				       const InputInfo &Output,
				       const InputInfoList &Inputs,
				       const ArgList &Args,
				       const char *LinkingOutput) const {
  ArgStringList CmdArgs;
  CmdArgs.push_back("--verify");
  CmdArgs.push_back("--debug-info");
  CmdArgs.push_back("--eh-frame");
  CmdArgs.push_back("--quiet");

  assert(Inputs.size() == 1 && "Unable to handle multiple inputs.");
  const InputInfo &Input = Inputs[0];
  assert(Input.isFilename() && "Unexpected verify input");

  // Grabbing the output of the earlier dsymutil run.
  CmdArgs.push_back(Input.getFilename());

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("dwarfdump"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void solaris::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                      const InputInfo &Output,
                                      const InputInfoList &Inputs,
                                      const ArgList &Args,
                                      const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}


void solaris::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                  const InputInfo &Output,
                                  const InputInfoList &Inputs,
                                  const ArgList &Args,
                                  const char *LinkingOutput) const {
  // FIXME: Find a real GCC, don't hard-code versions here
  std::string GCCLibPath = "/usr/gcc/4.5/lib/gcc/";
  const llvm::Triple &T = getToolChain().getTriple();
  std::string LibPath = "/usr/lib/";
  llvm::Triple::ArchType Arch = T.getArch();
  switch (Arch) {
        case llvm::Triple::x86:
          GCCLibPath += ("i386-" + T.getVendorName() + "-" +
              T.getOSName()).str() + "/4.5.2/";
          break;
        case llvm::Triple::x86_64:
          GCCLibPath += ("i386-" + T.getVendorName() + "-" +
              T.getOSName()).str();
          GCCLibPath += "/4.5.2/amd64/";
          LibPath += "amd64/";
          break;
        default:
          assert(0 && "Unsupported architecture");
  }

  ArgStringList CmdArgs;

  // Demangle C++ names in errors
  CmdArgs.push_back("-C");

  if ((!Args.hasArg(options::OPT_nostdlib)) &&
      (!Args.hasArg(options::OPT_shared))) {
    CmdArgs.push_back("-e");
    CmdArgs.push_back("_start");
  }

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
    CmdArgs.push_back("-dn");
  } else {
    CmdArgs.push_back("-Bdynamic");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-shared");
    } else {
      CmdArgs.push_back("--dynamic-linker");
      CmdArgs.push_back(Args.MakeArgString(LibPath + "ld.so.1"));
    }
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back(Args.MakeArgString(LibPath + "crt1.o"));
      CmdArgs.push_back(Args.MakeArgString(LibPath + "crti.o"));
      CmdArgs.push_back(Args.MakeArgString(LibPath + "values-Xa.o"));
      CmdArgs.push_back(Args.MakeArgString(GCCLibPath + "crtbegin.o"));
    } else {
      CmdArgs.push_back(Args.MakeArgString(LibPath + "crti.o"));
      CmdArgs.push_back(Args.MakeArgString(LibPath + "values-Xa.o"));
      CmdArgs.push_back(Args.MakeArgString(GCCLibPath + "crtbegin.o"));
    }
    if (getToolChain().getDriver().CCCIsCXX)
      CmdArgs.push_back(Args.MakeArgString(LibPath + "cxa_finalize.o"));
  }

  CmdArgs.push_back(Args.MakeArgString("-L" + GCCLibPath));

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (getToolChain().getDriver().CCCIsCXX)
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
    CmdArgs.push_back("-lgcc_s");
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-lgcc");
      CmdArgs.push_back("-lc");
      CmdArgs.push_back("-lm");
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    CmdArgs.push_back(Args.MakeArgString(GCCLibPath + "crtend.o"));
  }
  CmdArgs.push_back(Args.MakeArgString(LibPath + "crtn.o"));

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void auroraux::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                      const InputInfo &Output,
                                      const InputInfoList &Inputs,
                                      const ArgList &Args,
                                      const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("gas"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void auroraux::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                  const InputInfo &Output,
                                  const InputInfoList &Inputs,
                                  const ArgList &Args,
                                  const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  if ((!Args.hasArg(options::OPT_nostdlib)) &&
      (!Args.hasArg(options::OPT_shared))) {
    CmdArgs.push_back("-e");
    CmdArgs.push_back("_start");
  }

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
    CmdArgs.push_back("-dn");
  } else {
//    CmdArgs.push_back("--eh-frame-hdr");
    CmdArgs.push_back("-Bdynamic");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-shared");
    } else {
      CmdArgs.push_back("--dynamic-linker");
      CmdArgs.push_back("/lib/ld.so.1"); // 64Bit Path /lib/amd64/ld.so.1
    }
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crt1.o")));
      CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crtbegin.o")));
    } else {
      CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crti.o")));
    }
    CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crtn.o")));
  }

  CmdArgs.push_back(Args.MakeArgString("-L/opt/gcc4/lib/gcc/"
                                       + getToolChain().getTripleString()
                                       + "/4.2.4"));

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    // FIXME: For some reason GCC passes -lgcc before adding
    // the default system libraries. Just mimic this for now.
    CmdArgs.push_back("-lgcc");

    if (Args.hasArg(options::OPT_pthread))
      CmdArgs.push_back("-pthread");
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back("-lc");
    CmdArgs.push_back("-lgcc");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crtend.o")));
  }

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void openbsd::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                     const InputInfo &Output,
                                     const InputInfoList &Inputs,
                                     const ArgList &Args,
                                     const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void openbsd::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  if ((!Args.hasArg(options::OPT_nostdlib)) &&
      (!Args.hasArg(options::OPT_shared))) {
    CmdArgs.push_back("-e");
    CmdArgs.push_back("__start");
  }

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    CmdArgs.push_back("--eh-frame-hdr");
    CmdArgs.push_back("-Bdynamic");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-shared");
    } else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back("/usr/libexec/ld.so");
    }
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      if (Args.hasArg(options::OPT_pg))  
        CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("gcrt0.o")));
      else
        CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crt0.o")));
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbegin.o")));
    } else {
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbeginS.o")));
    }
  }

  std::string Triple = getToolChain().getTripleString();
  if (Triple.substr(0, 6) == "x86_64")
    Triple.replace(0, 6, "amd64");
  CmdArgs.push_back(Args.MakeArgString("-L/usr/lib/gcc-lib/" + Triple +
                                       "/4.2.1"));

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
      if (Args.hasArg(options::OPT_pg)) 
        CmdArgs.push_back("-lm_p");
      else
        CmdArgs.push_back("-lm");
    }

    // FIXME: For some reason GCC passes -lgcc before adding
    // the default system libraries. Just mimic this for now.
    CmdArgs.push_back("-lgcc");

    if (Args.hasArg(options::OPT_pthread)) {
      if (!Args.hasArg(options::OPT_shared) &&
          Args.hasArg(options::OPT_pg))
         CmdArgs.push_back("-lpthread_p");
      else
         CmdArgs.push_back("-lpthread");
    }

    if (!Args.hasArg(options::OPT_shared)) {
      if (Args.hasArg(options::OPT_pg))
         CmdArgs.push_back("-lc_p");
      else
         CmdArgs.push_back("-lc");
    }

    CmdArgs.push_back("-lgcc");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtend.o")));
    else
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtendS.o")));
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void bitrig::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void bitrig::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                const InputInfo &Output,
                                const InputInfoList &Inputs,
                                const ArgList &Args,
                                const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  if ((!Args.hasArg(options::OPT_nostdlib)) &&
      (!Args.hasArg(options::OPT_shared))) {
    CmdArgs.push_back("-e");
    CmdArgs.push_back("__start");
  }

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    CmdArgs.push_back("--eh-frame-hdr");
    CmdArgs.push_back("-Bdynamic");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-shared");
    } else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back("/usr/libexec/ld.so");
    }
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("gcrt0.o")));
      else
        CmdArgs.push_back(Args.MakeArgString(
                                getToolChain().GetFilePath("crt0.o")));
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbegin.o")));
    } else {
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbeginS.o")));
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lm_p");
      else
        CmdArgs.push_back("-lm");
    }

    if (Args.hasArg(options::OPT_pthread)) {
      if (!Args.hasArg(options::OPT_shared) &&
          Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lpthread_p");
      else
        CmdArgs.push_back("-lpthread");
    }

    if (!Args.hasArg(options::OPT_shared)) {
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lc_p");
      else
        CmdArgs.push_back("-lc");
    }

    std::string myarch = "-lclang_rt.";
    const llvm::Triple &T = getToolChain().getTriple();
    llvm::Triple::ArchType Arch = T.getArch();
    switch (Arch) {
          case llvm::Triple::arm:
            myarch += ("arm");
            break;
          case llvm::Triple::x86:
            myarch += ("i386");
            break;
          case llvm::Triple::x86_64:
            myarch += ("amd64");
            break;
          default:
            assert(0 && "Unsupported architecture");
     }
     CmdArgs.push_back(Args.MakeArgString(myarch));
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtend.o")));
    else
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtendS.o")));
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void freebsd::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                     const InputInfo &Output,
                                     const InputInfoList &Inputs,
                                     const ArgList &Args,
                                     const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // When building 32-bit code on FreeBSD/amd64, we have to explicitly
  // instruct as in the base system to assemble 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86)
    CmdArgs.push_back("--32");
  else if (getToolChain().getArch() == llvm::Triple::ppc)
    CmdArgs.push_back("-a32");
  else if (getToolChain().getArch() == llvm::Triple::mips ||
           getToolChain().getArch() == llvm::Triple::mipsel ||
           getToolChain().getArch() == llvm::Triple::mips64 ||
           getToolChain().getArch() == llvm::Triple::mips64el) {
    StringRef CPUName;
    StringRef ABIName;
    getMipsCPUAndABI(Args, getToolChain(), CPUName, ABIName);

    CmdArgs.push_back("-march");
    CmdArgs.push_back(CPUName.data());

    // Convert ABI name to the GNU tools acceptable variant.
    if (ABIName == "o32")
      ABIName = "32";
    else if (ABIName == "n64")
      ABIName = "64";

    CmdArgs.push_back("-mabi");
    CmdArgs.push_back(ABIName.data());

    if (getToolChain().getArch() == llvm::Triple::mips ||
        getToolChain().getArch() == llvm::Triple::mips64)
      CmdArgs.push_back("-EB");
    else
      CmdArgs.push_back("-EL");

    Arg *LastPICArg = Args.getLastArg(options::OPT_fPIC, options::OPT_fno_PIC,
                                      options::OPT_fpic, options::OPT_fno_pic,
                                      options::OPT_fPIE, options::OPT_fno_PIE,
                                      options::OPT_fpie, options::OPT_fno_pie);
    if (LastPICArg &&
        (LastPICArg->getOption().matches(options::OPT_fPIC) ||
         LastPICArg->getOption().matches(options::OPT_fpic) ||
         LastPICArg->getOption().matches(options::OPT_fPIE) ||
         LastPICArg->getOption().matches(options::OPT_fpie))) {
      CmdArgs.push_back("-KPIC");
    }
  }

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void freebsd::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const toolchains::FreeBSD& ToolChain = 
    static_cast<const toolchains::FreeBSD&>(getToolChain());
  const Driver &D = ToolChain.getDriver();
  ArgStringList CmdArgs;

  // Silence warning for "clang -g foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_g_Group);
  // and "clang -emit-llvm foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and for "clang -w foo.o -o foo". Other warning options are already
  // handled somewhere else.
  Args.ClaimAllArgs(options::OPT_w);

  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));

  if (Args.hasArg(options::OPT_pie))
    CmdArgs.push_back("-pie");

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    CmdArgs.push_back("--eh-frame-hdr");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-Bshareable");
    } else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back("/libexec/ld-elf.so.1");
    }
    if (ToolChain.getTriple().getOSMajorVersion() >= 9) {
      llvm::Triple::ArchType Arch = ToolChain.getArch();
      if (Arch == llvm::Triple::arm || Arch == llvm::Triple::sparc ||
          Arch == llvm::Triple::x86 || Arch == llvm::Triple::x86_64) {
        CmdArgs.push_back("--hash-style=both");
      }
    }
    CmdArgs.push_back("--enable-new-dtags");
  }

  // When building 32-bit code on FreeBSD/amd64, we have to explicitly
  // instruct ld in the base system to link 32-bit code.
  if (ToolChain.getArch() == llvm::Triple::x86) {
    CmdArgs.push_back("-m");
    CmdArgs.push_back("elf_i386_fbsd");
  }

  if (ToolChain.getArch() == llvm::Triple::ppc) {
    CmdArgs.push_back("-m");
    CmdArgs.push_back("elf32ppc_fbsd");
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    const char *crt1 = NULL;
    if (!Args.hasArg(options::OPT_shared)) {
      if (Args.hasArg(options::OPT_pg))
        crt1 = "gcrt1.o";
      else if (Args.hasArg(options::OPT_pie))
        crt1 = "Scrt1.o";
      else
        crt1 = "crt1.o";
    }
    if (crt1)
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(crt1)));

    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crti.o")));

    const char *crtbegin = NULL;
    if (Args.hasArg(options::OPT_static))
      crtbegin = "crtbeginT.o";
    else if (Args.hasArg(options::OPT_shared) || Args.hasArg(options::OPT_pie))
      crtbegin = "crtbeginS.o";
    else
      crtbegin = "crtbegin.o";

    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(crtbegin)));
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  const ToolChain::path_list Paths = ToolChain.getFilePaths();
  for (ToolChain::path_list::const_iterator i = Paths.begin(), e = Paths.end();
       i != e; ++i)
    CmdArgs.push_back(Args.MakeArgString(StringRef("-L") + *i));
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_s);
  Args.AddAllArgs(CmdArgs, options::OPT_t);
  Args.AddAllArgs(CmdArgs, options::OPT_Z_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      ToolChain.AddCXXStdlibLibArgs(Args, CmdArgs);
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lm_p");
      else
        CmdArgs.push_back("-lm");
    }
    // FIXME: For some reason GCC passes -lgcc and -lgcc_s before adding
    // the default system libraries. Just mimic this for now.
    if (Args.hasArg(options::OPT_pg))
      CmdArgs.push_back("-lgcc_p");
    else
      CmdArgs.push_back("-lgcc");
    if (Args.hasArg(options::OPT_static)) {
      CmdArgs.push_back("-lgcc_eh");
    } else if (Args.hasArg(options::OPT_pg)) {
      CmdArgs.push_back("-lgcc_eh_p");
    } else {
      CmdArgs.push_back("--as-needed");
      CmdArgs.push_back("-lgcc_s");
      CmdArgs.push_back("--no-as-needed");
    }

    if (Args.hasArg(options::OPT_pthread)) {
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lpthread_p");
      else
        CmdArgs.push_back("-lpthread");
    }

    if (Args.hasArg(options::OPT_pg)) {
      if (Args.hasArg(options::OPT_shared))
        CmdArgs.push_back("-lc");
      else
        CmdArgs.push_back("-lc_p");
      CmdArgs.push_back("-lgcc_p");
    } else {
      CmdArgs.push_back("-lc");
      CmdArgs.push_back("-lgcc");
    }

    if (Args.hasArg(options::OPT_static)) {
      CmdArgs.push_back("-lgcc_eh");
    } else if (Args.hasArg(options::OPT_pg)) {
      CmdArgs.push_back("-lgcc_eh_p");
    } else {
      CmdArgs.push_back("--as-needed");
      CmdArgs.push_back("-lgcc_s");
      CmdArgs.push_back("--no-as-needed");
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (Args.hasArg(options::OPT_shared) || Args.hasArg(options::OPT_pie))
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crtendS.o")));
    else
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crtend.o")));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crtn.o")));
  }

  addProfileRT(ToolChain, Args, CmdArgs, ToolChain.getTriple());

  const char *Exec =
    Args.MakeArgString(ToolChain.GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void avm2::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                     const InputInfo &Output,
                                     const InputInfoList &Inputs,
                                     const ArgList &Args,
                                     const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // When building 32-bit code on FreeBSD/amd64, we have to explicitly
  // instruct as in the base system to assemble 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86)
    CmdArgs.push_back("--32");
  else if (getToolChain().getArch() == llvm::Triple::ppc)
    CmdArgs.push_back("-a32");
  else if (getToolChain().getArch() == llvm::Triple::mips ||
           getToolChain().getArch() == llvm::Triple::mipsel ||
           getToolChain().getArch() == llvm::Triple::mips64 ||
           getToolChain().getArch() == llvm::Triple::mips64el) {
    StringRef CPUName;
    StringRef ABIName;
    getMipsCPUAndABI(Args, getToolChain(), CPUName, ABIName);

    CmdArgs.push_back("-march");
    CmdArgs.push_back(CPUName.data());

    // Convert ABI name to the GNU tools acceptable variant.
    if (ABIName == "o32")
      ABIName = "32";
    else if (ABIName == "n64")
      ABIName = "64";

    CmdArgs.push_back("-mabi");
    CmdArgs.push_back(ABIName.data());

    if (getToolChain().getArch() == llvm::Triple::mips ||
        getToolChain().getArch() == llvm::Triple::mips64)
      CmdArgs.push_back("-EB");
    else
      CmdArgs.push_back("-EL");

    Arg *LastPICArg = Args.getLastArg(options::OPT_fPIC, options::OPT_fno_PIC,
                                      options::OPT_fpic, options::OPT_fno_pic,
                                      options::OPT_fPIE, options::OPT_fno_PIE,
                                      options::OPT_fpie, options::OPT_fno_pie);
    if (LastPICArg &&
        (LastPICArg->getOption().matches(options::OPT_fPIC) ||
         LastPICArg->getOption().matches(options::OPT_fpic) ||
         LastPICArg->getOption().matches(options::OPT_fPIE) ||
         LastPICArg->getOption().matches(options::OPT_fpie))) {
      CmdArgs.push_back("-KPIC");
    }
  }

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("avm2-as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void avm2::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const toolchains::AVM2& ToolChain = 
    static_cast<const toolchains::AVM2&>(getToolChain());
  const Driver &D = ToolChain.getDriver();
  ArgStringList CmdArgs;

  // Silence warning for "clang -g foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_g_Group);
  // and "clang -emit-llvm foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and for "clang -w foo.o -o foo". Other warning options are already
  // handled somewhere else.
  Args.ClaimAllArgs(options::OPT_w);

  CmdArgs.push_back("--plugin");
#ifdef __CYGWIN__
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("multiplug.dll")));
#elif __linux__
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("multiplug.so")));
#else
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("multiplug.dylib")));
#endif

  // If use verbose mode.
  if (Args.hasArg(options::OPT_v)) {
    CmdArgs.push_back("-V");
    CmdArgs.push_back("--verbose");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("verbose");
  }

  // Set system root for relative path searching.
  CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));
  CmdArgs.push_back("--plugin-opt");
  CmdArgs.push_back(Args.MakeArgString("\"sysroot=" + D.SysRoot + "\""));
  

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  const ToolChain::path_list Paths = ToolChain.getFilePaths();
  for (ToolChain::path_list::const_iterator i = Paths.begin(), e = Paths.end();
       i != e; ++i)
    CmdArgs.push_back(Args.MakeArgString(StringRef("-L") + *i));
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_s);
  Args.AddAllArgs(CmdArgs, options::OPT_t);
  Args.AddAllArgs(CmdArgs, options::OPT_Z_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  bool has_emit_swf = false;
  bool has_emit_swc = false;
  
  if (has_emit_swc = Args.hasArg(options::OPT_emit_swc_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString("emit-swc=" +
                      Args.getLastArgValue(options::OPT_emit_swc_EQ)));
  }
  
  if (has_emit_swf = Args.hasArg(options::OPT_emit_swf)) {

    CmdArgs.push_back("--plugin-opt");
    if (Args.hasArg(options::OPT_symbol_class_EQ)) {
      CmdArgs.push_back(Args.MakeArgString("symbolclass=" +
                        Args.getLastArgValue(options::OPT_symbol_class_EQ)));
    } else {
      CmdArgs.push_back("symbol-class=0:com.adobe.flascc::Console");
    }
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("appendabc");
    CmdArgs.push_back("--plugin-opt");
    if (Args.hasArg(options::OPT_symbol_abc_EQ)) {
      CmdArgs.push_back(Args.MakeArgString(
                        Args.getLastArgValue(options::OPT_symbol_abc_EQ)));
    } else {
      CmdArgs.push_back(Args.MakeArgString(
                                ToolChain.GetFilePath("Console.abc")));
    }
    
    if (!Args.hasArg(options::OPT_no_swf_preloader_EQ)) {
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("swfpreloader");
      CmdArgs.push_back("--plugin-opt");
      if (Args.hasArg(options::OPT_swf_preloader_EQ)) {
        CmdArgs.push_back(Args.MakeArgString(
                          Args.getLastArgValue(options::OPT_swf_preloader_EQ)));
      } else {
        CmdArgs.push_back(
          Args.MakeArgString(ToolChain.GetFilePath("DefaultPreloader.swf")));
      }
    }
  }
  
  if (has_emit_swc || has_emit_swf) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("disableprojector");
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("llc-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-target-player");
    
    if (Args.hasArg(options::OPT_disable_telemetry)) {
      Args.ClaimAllArgs(options::OPT_disable_telemetry);
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("disable-telemetry");
    }
    
    if (!Args.hasArg(options::OPT_nostdlib) &&
        !Args.hasArg(options::OPT_nodefaultlibs)) {
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("DefaultVFS.abc")));
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("InMemoryBackingStore.abc")));
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("PlayerKernel.abc")));
      
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("appendabc");
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("PlayerCreateWorker.abc")));
    }
    
  }
  
  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "ISpecialFile.abc")));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "IBackingStore.abc")));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "IVFS.abc")));
    if(!has_emit_swf && !has_emit_swc)
    {
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("appendabc");
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("startHack.abc")));
      
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("appendabc");
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back(
        Args.MakeArgString(ToolChain.GetFilePath("ShellCreateWorker.abc")));
      }
  }
 
  // Set java memory.
  CmdArgs.push_back("--plugin-opt");
  if (Args.hasArg(options::OPT_jvmopt_EQ)) {
    CmdArgs.push_back(Args.MakeArgString("jvmopt=" +
                      Args.getLastArgValue(options::OPT_jvmopt_EQ)));
  } else {
    CmdArgs.push_back("jvmopt=-Xmx1500M");
  }

  // If use legacy asc.
  if (Args.hasArg(options::OPT_muse_legacy_asc)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("llc-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-use-legacy-as3-asm");
  }
  
  // If use pthread.
  if (Args.hasArg(options::OPT_pthread)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("pthread");
  }
  
  // Set flash name space.
  if (Args.hasArg(options::OPT_swf_ns_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString("swf-ns=" +
                      Args.getLastArgValue(options::OPT_swf_ns_EQ)));
  }
  
  // Pass llc opt.
  if (Args.hasArg(options::OPT_fllvm_llc_opt_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("llc-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString(
                      Args.getLastArgValue(options::OPT_fllvm_llc_opt_EQ)));
  }
  
  // Pass opt's opt.
  if (Args.hasArg(options::OPT_fllvm_opt_opt_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString(
                      Args.getLastArgValue(options::OPT_fllvm_opt_opt_EQ)));
  }
  
  // Set LTO preserving funcs.
  if (Args.hasArg(options::OPT_flto_api_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString("-internalize-public-api-file=" +
      Args.getLastArgValue(options::OPT_flto_api_EQ)));
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-internalize");
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-globaldce");
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-dce");
  }
  
  if (Args.hasArg(options::OPT_ascopt_EQ)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString("ascopt=" +
                      Args.getLastArgValue(options::OPT_ascopt_EQ)));
  }
  
  // If add -g.
  if (Args.hasArg(options::OPT_g_Flag)) {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("enabledebugger");

    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("llc-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-gendbgsymtable");
    
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
      "AlcDbgHelper.abc")));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
      "ELF.abc")));
    
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-disable-inlining");

  } else {
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("codegen-opt");
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back("-strip-debug");

    // If use enable-debugger
    if (Args.hasArg(options::OPT_enable_debugger)) {
      CmdArgs.push_back("--plugin-opt");
      CmdArgs.push_back("enabledebugger");
    }
  }

  // Set output file name.
  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }
  
  // Save temps.
  bool save_temps = false;
  std::string tempFilename;
  if (save_temps = Args.hasArg(options::OPT_save_temps)) {
    tempFilename = Output.getFilename();
  } else {
    tempFilename = D.GetTemporaryPath("", NULL);
    if (tempFilename.empty()) tempFilename = Output.getFilename();
  }
  
  const char* names[][2] = {
    {"obj-path=", ".lto.abc"},
    {"also-emit-llvm=", ".lto.bc"},
    {"lto-as3-1=", ".lto.1.as"},
    {"lto-as3-2=", ".lto.2.as"},
    {NULL, NULL}
  };
  
  std::string f;
  for (int i = 0; names[i][0] && names[i][1]; i++) {
    f = names[i][0] + tempFilename + names[i][1];
    CmdArgs.push_back("--plugin-opt");
    CmdArgs.push_back(Args.MakeArgString(f));
    //save_temps ? C.addResultFile(f.c_str()) : C.addTempFile(f.c_str());
  }
  
  // Set flash window size.
  CmdArgs.push_back("--plugin-opt");
  if (Args.hasArg(options::OPT_swf_size_EQ)) {
    CmdArgs.push_back(Args.MakeArgString("swf-size=" +
      Args.getLastArgValue(options::OPT_swf_size_EQ)));
  } else {
    CmdArgs.push_back("swf-size=640x480");
  }
  
  // Set flash version.
  CmdArgs.push_back("--plugin-opt");
  if (Args.hasArg(options::OPT_swf_version_EQ)) {
    CmdArgs.push_back(Args.MakeArgString("swf-version=" +
      Args.getLastArgValue(options::OPT_swf_version_EQ)));
  } else {
    CmdArgs.push_back("swf-version=18");
  }

  CmdArgs.push_back("--plugin-opt");
  CmdArgs.push_back("mtriple=avm2-unknown-freebsd8");


  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "C_Run.abc")));
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "BinaryData.abc")));
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "Exit.abc")));
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "LongJmp.abc")));
  CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        "CModule.abc")));


  std::string postfix = "/stdlibs_abc/";
  if (Args.hasArg(options::OPT_O4) || Args.hasArg(options::OPT_emit_llvm)
      || Args.hasArg(options::OPT_flto)) {
    postfix = "";
  }
  //CmdArgs.push_back(Args.MakeArgString("-L" + D.SysRoot +
  //                                   "/usr/lib/avm2-unknown-freebsd8/4.2.1"));
  CmdArgs.push_back(Args.MakeArgString("-L" + D.SysRoot +
                                     "/usr/lib" + postfix));

  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs);
  
  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      ToolChain.AddCXXStdlibLibArgs(Args, CmdArgs);
      if (Args.hasArg(options::OPT_pg))
        CmdArgs.push_back("-lm_p");
      else
        CmdArgs.push_back("-lm");
    }
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        (postfix + "crt1_c.o").c_str())));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        (postfix + "libcHack.o").c_str())));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        (postfix + "libm.o").c_str())));
    if (Args.hasArg(options::OPT_pthread)) {
      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
          (postfix + "libthr.a").c_str())));
    }
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(
        (postfix + "libc.a").c_str())));
  }

  addProfileRT(ToolChain, Args, CmdArgs, ToolChain.getTriple());

  const char *Exec =
    Args.MakeArgString(ToolChain.GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void netbsd::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                     const InputInfo &Output,
                                     const InputInfoList &Inputs,
                                     const ArgList &Args,
                                     const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // When building 32-bit code on NetBSD/amd64, we have to explicitly
  // instruct as in the base system to assemble 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86)
    CmdArgs.push_back("--32");

  // Set byte order explicitly
  if (getToolChain().getArch() == llvm::Triple::mips)
    CmdArgs.push_back("-EB");
  else if (getToolChain().getArch() == llvm::Triple::mipsel)
    CmdArgs.push_back("-EL");

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec = Args.MakeArgString((getToolChain().GetProgramPath("as")));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void netbsd::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    CmdArgs.push_back("--eh-frame-hdr");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-Bshareable");
    } else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back("/libexec/ld.elf_so");
    }
  }

  // When building 32-bit code on NetBSD/amd64, we have to explicitly
  // instruct ld in the base system to link 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86) {
    CmdArgs.push_back("-m");
    CmdArgs.push_back("elf_i386");
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crt0.o")));
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbegin.o")));
    } else {
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtbeginS.o")));
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_s);
  Args.AddAllArgs(CmdArgs, options::OPT_t);
  Args.AddAllArgs(CmdArgs, options::OPT_Z_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
      CmdArgs.push_back("-lm");
    }
    // FIXME: For some reason GCC passes -lgcc and -lgcc_s before adding
    // the default system libraries. Just mimic this for now.
    if (Args.hasArg(options::OPT_static)) {
      CmdArgs.push_back("-lgcc_eh");
    } else {
      CmdArgs.push_back("--as-needed");
      CmdArgs.push_back("-lgcc_s");
      CmdArgs.push_back("--no-as-needed");
    }
    CmdArgs.push_back("-lgcc");

    if (Args.hasArg(options::OPT_pthread))
      CmdArgs.push_back("-lpthread");
    CmdArgs.push_back("-lc");

    CmdArgs.push_back("-lgcc");
    if (Args.hasArg(options::OPT_static)) {
      CmdArgs.push_back("-lgcc_eh");
    } else {
      CmdArgs.push_back("--as-needed");
      CmdArgs.push_back("-lgcc_s");
      CmdArgs.push_back("--no-as-needed");
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath(
                                                                  "crtend.o")));
    else
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath(
                                                                 "crtendS.o")));
    CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath(
                                                                    "crtn.o")));
  }

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  const char *Exec = Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void linuxtools::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                        const InputInfo &Output,
                                        const InputInfoList &Inputs,
                                        const ArgList &Args,
                                        const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // Add --32/--64 to make sure we get the format we want.
  // This is incomplete
  if (getToolChain().getArch() == llvm::Triple::x86) {
    CmdArgs.push_back("--32");
  } else if (getToolChain().getArch() == llvm::Triple::x86_64) {
    CmdArgs.push_back("--64");
  } else if (getToolChain().getArch() == llvm::Triple::ppc) {
    CmdArgs.push_back("-a32");
    CmdArgs.push_back("-mppc");
    CmdArgs.push_back("-many");
  } else if (getToolChain().getArch() == llvm::Triple::ppc64) {
    CmdArgs.push_back("-a64");
    CmdArgs.push_back("-mppc64");
    CmdArgs.push_back("-many");
  } else if (getToolChain().getArch() == llvm::Triple::arm) {
    StringRef MArch = getToolChain().getArchName();
    if (MArch == "armv7" || MArch == "armv7a" || MArch == "armv7-a")
      CmdArgs.push_back("-mfpu=neon");

    StringRef ARMFloatABI = getARMFloatABI(getToolChain().getDriver(), Args,
                                           getToolChain().getTriple());
    CmdArgs.push_back(Args.MakeArgString("-mfloat-abi=" + ARMFloatABI));

    Args.AddLastArg(CmdArgs, options::OPT_march_EQ);
    Args.AddLastArg(CmdArgs, options::OPT_mcpu_EQ);
    Args.AddLastArg(CmdArgs, options::OPT_mfpu_EQ);
  } else if (getToolChain().getArch() == llvm::Triple::mips ||
             getToolChain().getArch() == llvm::Triple::mipsel ||
             getToolChain().getArch() == llvm::Triple::mips64 ||
             getToolChain().getArch() == llvm::Triple::mips64el) {
    StringRef CPUName;
    StringRef ABIName;
    getMipsCPUAndABI(Args, getToolChain(), CPUName, ABIName);

    CmdArgs.push_back("-march");
    CmdArgs.push_back(CPUName.data());

    // Convert ABI name to the GNU tools acceptable variant.
    if (ABIName == "o32")
      ABIName = "32";
    else if (ABIName == "n64")
      ABIName = "64";

    CmdArgs.push_back("-mabi");
    CmdArgs.push_back(ABIName.data());

    if (getToolChain().getArch() == llvm::Triple::mips ||
        getToolChain().getArch() == llvm::Triple::mips64)
      CmdArgs.push_back("-EB");
    else
      CmdArgs.push_back("-EL");

    Arg *LastPICArg = Args.getLastArg(options::OPT_fPIC, options::OPT_fno_PIC,
                                      options::OPT_fpic, options::OPT_fno_pic,
                                      options::OPT_fPIE, options::OPT_fno_PIE,
                                      options::OPT_fpie, options::OPT_fno_pie);
    if (LastPICArg &&
        (LastPICArg->getOption().matches(options::OPT_fPIC) ||
         LastPICArg->getOption().matches(options::OPT_fpic) ||
         LastPICArg->getOption().matches(options::OPT_fPIE) ||
         LastPICArg->getOption().matches(options::OPT_fpie))) {
      CmdArgs.push_back("-KPIC");
    }
  }

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

static void AddLibgcc(llvm::Triple Triple, const Driver &D,
                      ArgStringList &CmdArgs, const ArgList &Args) {
  bool isAndroid = Triple.getEnvironment() == llvm::Triple::Android;
  bool StaticLibgcc = isAndroid || Args.hasArg(options::OPT_static) ||
    Args.hasArg(options::OPT_static_libgcc);
  if (!D.CCCIsCXX)
    CmdArgs.push_back("-lgcc");

  if (StaticLibgcc) {
    if (D.CCCIsCXX)
      CmdArgs.push_back("-lgcc");
  } else {
    if (!D.CCCIsCXX)
      CmdArgs.push_back("--as-needed");
    CmdArgs.push_back("-lgcc_s");
    if (!D.CCCIsCXX)
      CmdArgs.push_back("--no-as-needed");
  }

  if (StaticLibgcc && !isAndroid)
    CmdArgs.push_back("-lgcc_eh");
  else if (!Args.hasArg(options::OPT_shared) && D.CCCIsCXX)
    CmdArgs.push_back("-lgcc");
}

static bool hasMipsN32ABIArg(const ArgList &Args) {
  Arg *A = Args.getLastArg(options::OPT_mabi_EQ);
  return A && (A->getValue() == StringRef("n32"));
}

void linuxtools::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
  const toolchains::Linux& ToolChain =
    static_cast<const toolchains::Linux&>(getToolChain());
  const Driver &D = ToolChain.getDriver();
  const bool isAndroid =
    ToolChain.getTriple().getEnvironment() == llvm::Triple::Android;

  ArgStringList CmdArgs;

  // Silence warning for "clang -g foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_g_Group);
  // and "clang -emit-llvm foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and for "clang -w foo.o -o foo". Other warning options are already
  // handled somewhere else.
  Args.ClaimAllArgs(options::OPT_w);

  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));

  if (Args.hasArg(options::OPT_pie))
    CmdArgs.push_back("-pie");

  if (Args.hasArg(options::OPT_rdynamic))
    CmdArgs.push_back("-export-dynamic");

  if (Args.hasArg(options::OPT_s))
    CmdArgs.push_back("-s");

  for (std::vector<std::string>::const_iterator i = ToolChain.ExtraOpts.begin(),
         e = ToolChain.ExtraOpts.end();
       i != e; ++i)
    CmdArgs.push_back(i->c_str());

  if (!Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("--eh-frame-hdr");
  }

  CmdArgs.push_back("-m");
  if (ToolChain.getArch() == llvm::Triple::x86)
    CmdArgs.push_back("elf_i386");
  else if (ToolChain.getArch() == llvm::Triple::arm
           ||  ToolChain.getArch() == llvm::Triple::thumb)
    CmdArgs.push_back("armelf_linux_eabi");
  else if (ToolChain.getArch() == llvm::Triple::ppc)
    CmdArgs.push_back("elf32ppclinux");
  else if (ToolChain.getArch() == llvm::Triple::ppc64)
    CmdArgs.push_back("elf64ppc");
  else if (ToolChain.getArch() == llvm::Triple::mips)
    CmdArgs.push_back("elf32btsmip");
  else if (ToolChain.getArch() == llvm::Triple::mipsel)
    CmdArgs.push_back("elf32ltsmip");
  else if (ToolChain.getArch() == llvm::Triple::mips64) {
    if (hasMipsN32ABIArg(Args))
      CmdArgs.push_back("elf32btsmipn32");
    else
      CmdArgs.push_back("elf64btsmip");
  }
  else if (ToolChain.getArch() == llvm::Triple::mips64el) {
    if (hasMipsN32ABIArg(Args))
      CmdArgs.push_back("elf32ltsmipn32");
    else
      CmdArgs.push_back("elf64ltsmip");
  }
  else
    CmdArgs.push_back("elf_x86_64");

  if (Args.hasArg(options::OPT_static)) {
    if (ToolChain.getArch() == llvm::Triple::arm
        || ToolChain.getArch() == llvm::Triple::thumb)
      CmdArgs.push_back("-Bstatic");
    else
      CmdArgs.push_back("-static");
  } else if (Args.hasArg(options::OPT_shared)) {
    CmdArgs.push_back("-shared");
    if (isAndroid) {
      CmdArgs.push_back("-Bsymbolic");
    }
  }

  if (ToolChain.getArch() == llvm::Triple::arm ||
      ToolChain.getArch() == llvm::Triple::thumb ||
      (!Args.hasArg(options::OPT_static) &&
       !Args.hasArg(options::OPT_shared))) {
    CmdArgs.push_back("-dynamic-linker");
    if (isAndroid)
      CmdArgs.push_back("/system/bin/linker");
    else if (ToolChain.getArch() == llvm::Triple::x86)
      CmdArgs.push_back("/lib/ld-linux.so.2");
    else if (ToolChain.getArch() == llvm::Triple::arm ||
             ToolChain.getArch() == llvm::Triple::thumb) {
      if (ToolChain.getTriple().getEnvironment() == llvm::Triple::GNUEABIHF)
        CmdArgs.push_back("/lib/ld-linux-armhf.so.3");
      else
        CmdArgs.push_back("/lib/ld-linux.so.3");
    }
    else if (ToolChain.getArch() == llvm::Triple::mips ||
             ToolChain.getArch() == llvm::Triple::mipsel)
      CmdArgs.push_back("/lib/ld.so.1");
    else if (ToolChain.getArch() == llvm::Triple::mips64 ||
             ToolChain.getArch() == llvm::Triple::mips64el) {
      if (hasMipsN32ABIArg(Args))
        CmdArgs.push_back("/lib32/ld.so.1");
      else
        CmdArgs.push_back("/lib64/ld.so.1");
    }
    else if (ToolChain.getArch() == llvm::Triple::ppc)
      CmdArgs.push_back("/lib/ld.so.1");
    else if (ToolChain.getArch() == llvm::Triple::ppc64)
      CmdArgs.push_back("/lib64/ld64.so.1");
    else
      CmdArgs.push_back("/lib64/ld-linux-x86-64.so.2");
  }

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!isAndroid) {
      const char *crt1 = NULL;
      if (!Args.hasArg(options::OPT_shared)){
        if (Args.hasArg(options::OPT_pie))
          crt1 = "Scrt1.o";
        else
          crt1 = "crt1.o";
      }
      if (crt1)
        CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(crt1)));

      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crti.o")));
    }

    const char *crtbegin;
    if (Args.hasArg(options::OPT_static))
      crtbegin = isAndroid ? "crtbegin_static.o" : "crtbeginT.o";
    else if (Args.hasArg(options::OPT_shared))
      crtbegin = isAndroid ? "crtbegin_so.o" : "crtbeginS.o";
    else if (Args.hasArg(options::OPT_pie))
      crtbegin = isAndroid ? "crtbegin_dynamic.o" : "crtbeginS.o";
    else
      crtbegin = isAndroid ? "crtbegin_dynamic.o" : "crtbegin.o";
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(crtbegin)));

    // Add crtfastmath.o if available and fast math is enabled.
    ToolChain.AddFastMathRuntimeIfAvailable(Args, CmdArgs);
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);

  const ToolChain::path_list Paths = ToolChain.getFilePaths();

  for (ToolChain::path_list::const_iterator i = Paths.begin(), e = Paths.end();
       i != e; ++i)
    CmdArgs.push_back(Args.MakeArgString(StringRef("-L") + *i));

  // Tell the linker to load the plugin. This has to come before AddLinkerInputs
  // as gold requires -plugin to come before any -plugin-opt that -Wl might
  // forward.
  if (D.IsUsingLTO(Args) || Args.hasArg(options::OPT_use_gold_plugin)) {
    CmdArgs.push_back("-plugin");
    std::string Plugin = ToolChain.getDriver().Dir + "/../lib/LLVMgold.so";
    CmdArgs.push_back(Args.MakeArgString(Plugin));
  }

  if (Args.hasArg(options::OPT_Z_Xlinker__no_demangle))
    CmdArgs.push_back("--no-demangle");

  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs);

  SanitizerArgs Sanitize(D, Args);

  // Call this before we add the C++ ABI library.
  if (Sanitize.needsUbsanRt())
    addUbsanRTLinux(getToolChain(), Args, CmdArgs);

  if (D.CCCIsCXX &&
      !Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    bool OnlyLibstdcxxStatic = Args.hasArg(options::OPT_static_libstdcxx) &&
      !Args.hasArg(options::OPT_static);
    if (OnlyLibstdcxxStatic)
      CmdArgs.push_back("-Bstatic");
    ToolChain.AddCXXStdlibLibArgs(Args, CmdArgs);
    if (OnlyLibstdcxxStatic)
      CmdArgs.push_back("-Bdynamic");
    CmdArgs.push_back("-lm");
  }

  // Call this before we add the C run-time.
  if (Sanitize.needsAsanRt())
    addAsanRTLinux(getToolChain(), Args, CmdArgs);
  if (Sanitize.needsTsanRt())
    addTsanRTLinux(getToolChain(), Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib)) {
    if (!Args.hasArg(options::OPT_nodefaultlibs)) {
      if (Args.hasArg(options::OPT_static))
        CmdArgs.push_back("--start-group");

      AddLibgcc(ToolChain.getTriple(), D, CmdArgs, Args);

      if (Args.hasArg(options::OPT_pthread) ||
          Args.hasArg(options::OPT_pthreads))
        CmdArgs.push_back("-lpthread");

      CmdArgs.push_back("-lc");

      if (Args.hasArg(options::OPT_static))
        CmdArgs.push_back("--end-group");
      else
        AddLibgcc(ToolChain.getTriple(), D, CmdArgs, Args);
    }

    if (!Args.hasArg(options::OPT_nostartfiles)) {
      const char *crtend;
      if (Args.hasArg(options::OPT_shared))
        crtend = isAndroid ? "crtend_so.o" : "crtendS.o";
      else if (Args.hasArg(options::OPT_pie))
        crtend = isAndroid ? "crtend_android.o" : "crtendS.o";
      else
        crtend = isAndroid ? "crtend_android.o" : "crtend.o";

      CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath(crtend)));
      if (!isAndroid)
        CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crtn.o")));
    }
  }

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  C.addCommand(new Command(JA, *this, ToolChain.Linker.c_str(), CmdArgs));
}

void minix::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                   const InputInfo &Output,
                                   const InputInfoList &Inputs,
                                   const ArgList &Args,
                                   const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void minix::Link::ConstructJob(Compilation &C, const JobAction &JA,
                               const InputInfo &Output,
                               const InputInfoList &Inputs,
                               const ArgList &Args,
                               const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath("crt1.o")));
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath("crtbegin.o")));
      CmdArgs.push_back(Args.MakeArgString(getToolChain().GetFilePath("crtn.o")));
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    if (D.CCCIsCXX) {
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
      CmdArgs.push_back("-lm");
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (Args.hasArg(options::OPT_pthread))
      CmdArgs.push_back("-lpthread");
    CmdArgs.push_back("-lc");
    CmdArgs.push_back("-lCompilerRT-Generic");
    CmdArgs.push_back("-L/usr/pkg/compiler-rt/lib");
    CmdArgs.push_back(
	 Args.MakeArgString(getToolChain().GetFilePath("crtend.o")));
  }

  const char *Exec = Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

/// DragonFly Tools

// For now, DragonFly Assemble does just about the same as for
// FreeBSD, but this may change soon.
void dragonfly::Assemble::ConstructJob(Compilation &C, const JobAction &JA,
                                       const InputInfo &Output,
                                       const InputInfoList &Inputs,
                                       const ArgList &Args,
                                       const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  // When building 32-bit code on DragonFly/pc64, we have to explicitly
  // instruct as in the base system to assemble 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86)
    CmdArgs.push_back("--32");

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA,
                       options::OPT_Xassembler);

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (InputInfoList::const_iterator
         it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    const InputInfo &II = *it;
    CmdArgs.push_back(II.getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void dragonfly::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                   const InputInfo &Output,
                                   const InputInfoList &Inputs,
                                   const ArgList &Args,
                                   const char *LinkingOutput) const {
  const Driver &D = getToolChain().getDriver();
  ArgStringList CmdArgs;

  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));

  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_shared))
      CmdArgs.push_back("-Bshareable");
    else {
      CmdArgs.push_back("-dynamic-linker");
      CmdArgs.push_back("/usr/libexec/ld-elf.so.2");
    }
  }

  // When building 32-bit code on DragonFly/pc64, we have to explicitly
  // instruct ld in the base system to link 32-bit code.
  if (getToolChain().getArch() == llvm::Triple::x86) {
    CmdArgs.push_back("-m");
    CmdArgs.push_back("elf_i386");
  }

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back(
            Args.MakeArgString(getToolChain().GetFilePath("crt1.o")));
      CmdArgs.push_back(
            Args.MakeArgString(getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(
            Args.MakeArgString(getToolChain().GetFilePath("crtbegin.o")));
    } else {
      CmdArgs.push_back(
            Args.MakeArgString(getToolChain().GetFilePath("crti.o")));
      CmdArgs.push_back(
            Args.MakeArgString(getToolChain().GetFilePath("crtbeginS.o")));
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);

  AddLinkerInputs(getToolChain(), Inputs, Args, CmdArgs);

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    // FIXME: GCC passes on -lgcc, -lgcc_pic and a whole lot of
    //         rpaths
    CmdArgs.push_back("-L/usr/lib/gcc41");

    if (!Args.hasArg(options::OPT_static)) {
      CmdArgs.push_back("-rpath");
      CmdArgs.push_back("/usr/lib/gcc41");

      CmdArgs.push_back("-rpath-link");
      CmdArgs.push_back("/usr/lib/gcc41");

      CmdArgs.push_back("-rpath");
      CmdArgs.push_back("/usr/lib");

      CmdArgs.push_back("-rpath-link");
      CmdArgs.push_back("/usr/lib");
    }

    if (D.CCCIsCXX) {
      getToolChain().AddCXXStdlibLibArgs(Args, CmdArgs);
      CmdArgs.push_back("-lm");
    }

    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-lgcc_pic");
    } else {
      CmdArgs.push_back("-lgcc");
    }


    if (Args.hasArg(options::OPT_pthread))
      CmdArgs.push_back("-lpthread");

    if (!Args.hasArg(options::OPT_nolibc)) {
      CmdArgs.push_back("-lc");
    }

    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-lgcc_pic");
    } else {
      CmdArgs.push_back("-lgcc");
    }
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared))
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtend.o")));
    else
      CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtendS.o")));
    CmdArgs.push_back(Args.MakeArgString(
                              getToolChain().GetFilePath("crtn.o")));
  }

  addProfileRT(getToolChain(), Args, CmdArgs, getToolChain().getTriple());

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("ld"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}

void visualstudio::Link::ConstructJob(Compilation &C, const JobAction &JA,
                                      const InputInfo &Output,
                                      const InputInfoList &Inputs,
                                      const ArgList &Args,
                                      const char *LinkingOutput) const {
  ArgStringList CmdArgs;

  if (Output.isFilename()) {
    CmdArgs.push_back(Args.MakeArgString(std::string("-out:") +
                                         Output.getFilename()));
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib) &&
    !Args.hasArg(options::OPT_nostartfiles)) {
    CmdArgs.push_back("-defaultlib:libcmt");
  }

  CmdArgs.push_back("-nologo");

  Args.AddAllArgValues(CmdArgs, options::OPT_l);

  // Add filenames immediately.
  for (InputInfoList::const_iterator
       it = Inputs.begin(), ie = Inputs.end(); it != ie; ++it) {
    if (it->isFilename())
      CmdArgs.push_back(it->getFilename());
  }

  const char *Exec =
    Args.MakeArgString(getToolChain().GetProgramPath("link.exe"));
  C.addCommand(new Command(JA, *this, Exec, CmdArgs));
}
