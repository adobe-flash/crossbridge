dnl Initialize autoconf
AC_INIT([[LLVM-TEST]],[[3.2svn]],[llvmbugs@cs.uiuc.edu])

dnl Place all of the extra autoconf files into the config subdirectory
AC_CONFIG_AUX_DIR([autoconf])

dnl Verify that the source directory is valid
AC_CONFIG_SRCDIR([SingleSource/Benchmarks/Makefile])

dnl Do special configuration of Makefiles
AC_CONFIG_COMMANDS([setup],,[llvm_src="${srcdir}"])
AC_CONFIG_FILES([Makefile.config])
AC_CONFIG_MAKEFILE(Makefile)
AC_CONFIG_MAKEFILE(Makefile.common)
AC_CONFIG_MAKEFILE(Makefile.nagfortran)
AC_CONFIG_MAKEFILE(Makefile.f2c)
AC_CONFIG_MAKEFILE(Makefile.programs)
AC_CONFIG_MAKEFILE(Makefile.tests)
AC_CONFIG_MAKEFILE(TEST.aa.Makefile)
AC_CONFIG_MAKEFILE(TEST.aa.report)
AC_CONFIG_MAKEFILE(TEST.example.Makefile)
AC_CONFIG_MAKEFILE(TEST.nightly.Makefile)
AC_CONFIG_MAKEFILE(TEST.buildrepo.Makefile)
AC_CONFIG_MAKEFILE(TEST.jit.Makefile)
AC_CONFIG_MAKEFILE(TEST.nightly.report)
AC_CONFIG_MAKEFILE(TEST.jit.report)
AC_CONFIG_MAKEFILE(TEST.typesafe.Makefile)
AC_CONFIG_MAKEFILE(TEST.vtl.Makefile)
AC_CONFIG_MAKEFILE(External/Makefile)
AC_CONFIG_MAKEFILE(External/Makefile.external)
AC_CONFIG_MAKEFILE(External/Povray/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile.spec)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile.spec.config)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile.spec2006)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile.spec2000)
AC_CONFIG_MAKEFILE(External/SPEC/Makefile.spec95)
AC_CONFIG_MAKEFILE(External/SPEC/CFP2006/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/CINT2006/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/CFP2000/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/CINT2000/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/CFP95/Makefile)
AC_CONFIG_MAKEFILE(External/SPEC/CINT95/Makefile)
AC_CONFIG_MAKEFILE(External/skidmarks10/Makefile)
AC_CONFIG_MAKEFILE(MultiSource/Makefile)
AC_CONFIG_MAKEFILE(MultiSource/Makefile.multisrc)
AC_CONFIG_MAKEFILE(SingleSource/Makefile)
AC_CONFIG_MAKEFILE(SingleSource/Makefile.singlesrc)
AC_CONFIG_MAKEFILE(tools/Makefile)

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 2: Architecture, target, and host checks
dnl===
dnl===-----------------------------------------------------------------------===

dnl Check the target for which we're compiling and the host that will do the
dnl compilations. This will tell us which LLVM compiler will be used for
dnl compiling SSA into object code. This needs to be done early because
dnl following tests depend on it.
AC_CANONICAL_TARGET

AC_CACHE_CHECK([type of operating system we're going to target],
               [llvm_cv_target_os_type],
[case $target in
  *-*-aix*)
    llvm_cv_target_os_type="AIX" ;;
  *-*-irix*)
    llvm_cv_target_os_type="IRIX" ;;
  *-*-cygwin*)
    llvm_cv_target_os_type="Cygwin" ;;
  *-*-darwin*)
    llvm_cv_target_os_type="Darwin" ;;
  *-*-minix*)
    llvm_cv_target_os_type="Minix" ;;
  *-*-freebsd*)
    llvm_cv_target_os_type="FreeBSD" ;;
  *-*-openbsd*)
    llvm_cv_target_os_type="OpenBSD" ;;
  *-*-netbsd*)
    llvm_cv_target_os_type="NetBSD" ;;
  *-*-dragonfly*)
    llvm_cv_target_os_type="DragonFly" ;;
  *-*-hpux*)
    llvm_cv_target_os_type="HP-UX" ;;
  *-*-interix*)
    llvm_cv_target_os_type="Interix" ;;
  *-*-linux*)
    llvm_cv_target_os_type="Linux" ;;
  *-*-solaris*)
    llvm_cv_target_os_type="SunOS" ;;
  *-*-auroraux*)
    llvm_cv_target_os_type="AuroraUX" ;;
  *-*-win32*)
    llvm_cv_target_os_type="Win32" ;;
  *-*-mingw*)
    llvm_cv_target_os_type="MingW" ;;
  *-*-haiku*)
    llvm_cv_target_os_type="Haiku" ;;
  *-unknown-eabi*)
    llvm_cv_target_os_type="Freestanding" ;;
  *)
    llvm_cv_target_os_type="Unknown" ;;
esac])

dnl Set the "TARGET_OS" Makefile variable based on the target type.
AC_SUBST(TARGET_OS,$llvm_cv_target_os_type)

dnl Determine what our target architecture is and configure accordingly.
dnl This will allow Makefiles to make a distinction between the hardware and
dnl the OS.
AC_CACHE_CHECK([target architecture],[llvm_cv_target_arch],
[case $target in
  i?86-*)                 llvm_cv_target_arch="x86" ;;
  amd64-* | x86_64-*)     llvm_cv_target_arch="x86_64" ;;
  sparc*-*)               llvm_cv_target_arch="Sparc" ;;
  powerpc*-*)             llvm_cv_target_arch="PowerPC" ;;
  alpha*-*)               llvm_cv_target_arch="Alpha" ;;
  arm*-*)                 llvm_cv_target_arch="ARM" ;;
  mips-* | mips64-*)      llvm_cv_target_arch="Mips" ;;
  mipsel-* | mips64el-*)  llvm_cv_target_arch="Mips" ;;
  xcore-*)                llvm_cv_target_arch="XCore" ;;
  msp430-*)               llvm_cv_target_arch="MSP430" ;;
  hexagon-*)              llvm_cv_target_arch="Hexagon" ;;
  s390x-*)                llvm_cv_target_arch="SystemZ" ;;
  bfin-*)                 llvm_cv_target_arch="Blackfin" ;;
  mblaze-*)               llvm_cv_target_arch="MBlaze" ;;
  ptx-*)                  llvm_cv_target_arch="PTX" ;;
  *)                      llvm_cv_target_arch="Unknown" ;;
esac])
if test "$llvm_cv_target_arch" = "Unknown" ; then
  AC_MSG_WARN([Configuring LLVM for an unknown target archicture])
fi

# Determine the LLVM native architecture for the target
case "$llvm_cv_target_arch" in
    x86)     LLVM_NATIVE_ARCH="X86" ;;
    x86_64)  LLVM_NATIVE_ARCH="X86" ;;
    *)       LLVM_NATIVE_ARCH="$llvm_cv_target_arch" ;;
esac

dnl Define a substitution, ARCH, for the target architecture
AC_SUBST(ARCH,$llvm_cv_target_arch)

dnl Check for the endianness of the target
AC_C_BIGENDIAN(AC_SUBST([ENDIAN],[big]),AC_SUBST([ENDIAN],[little]))

dnl **************************************************************************
dnl * Get command line options
dnl **************************************************************************

dnl Location of LLVM source code
AC_ARG_WITH(llvmsrc,
  AS_HELP_STRING([--with-llvmsrc=DIR],Location of LLVM Source Code),
  AC_SUBST(LLVM_SRC,[$withval]),
  AC_SUBST(LLVM_SRC,[`cd ${srcdir}/../..; pwd`]))

dnl Location of LLVM object code
AC_ARG_WITH(llvmobj,
  AS_HELP_STRING([--with-llvmobj=DIR],Location of LLVM Object Code),
  AC_SUBST(LLVM_OBJ,[$withval]),
  AC_SUBST(LLVM_OBJ,[`cd ../..; pwd`]))

dnl Location of the parent directory of the external tests
AC_ARG_WITH(externals,
  AS_HELP_STRING([--with-externals=DIR],Location of External Test code),
  AC_SUBST(LLVM_EXTERNALS,[$withval]),
  AC_SUBST(LLVM_EXTERNALS,[$LLVM_SRC/projects/test-suite-externals]))


dnl Allow a specific Clang compiler to be used with this LLVM config.
AC_ARG_WITH(clang,
  AS_HELP_STRING([--with-clang],
    [Specify location of clang compiler (default is --with-built-clang)]),
    [],[with_clang=default])

dnl Enable use of the built Clang.
AC_ARG_WITH(built-clang,
  AS_HELP_STRING([--with-built-clang],
    [Use the compiled Clang as the LLVM compiler (default=check)]),
    [],[with_built_clang=check])

dnl Select the Clang compiler option.
dnl
dnl If --with-clang is given, always honor that; otherwise honor
dnl --with-built-clang, or check if we have the clang sources.
AC_MSG_CHECKING([clang compiler])
WITH_CLANGPATH=""
WITH_BUILT_CLANG=0
if test "$with_clang" != "default"; then
   WITH_CLANGPATH="$with_clang"
   if ! test -x "$WITH_CLANGPATH"; then
     AC_MSG_ERROR([invalid --with-clang, path does not specify an executable])
   fi
elif test "$with_built_clang" = "yes"; then
   WITH_BUILT_CLANG=1
elif test "$with_built_clang" = "no"; then
   WITH_BUILT_CLANG=0
else
   if test "$with_built_clang" != "check"; then
      AC_MSG_ERROR([invalid value for --with-built-clang.])
   fi
   if test -f $LLVM_SRC/tools/clang/README.txt; then
     WITH_BUILT_CLANG=1
   fi
fi

if ! test -z "$WITH_CLANGPATH"; then
   AC_MSG_RESULT([$WITH_CLANGPATH])
   WITH_CLANGXXPATH=`"$WITH_CLANGPATH" --print-prog-name=clang++`
elif test "$WITH_BUILT_CLANG" = "1"; then
   AC_MSG_RESULT([built])
else
   AC_MSG_RESULT([none])
fi
AC_SUBST(CLANGPATH,$WITH_CLANGPATH)
AC_SUBST(CLANGXXPATH,$WITH_CLANGXXPATH)
AC_SUBST(ENABLE_BUILT_CLANG,$WITH_BUILT_CLANG)

dnl Select the LLVM capable compiler to use, we default to using clang if
dnl found.
AC_ARG_WITH(llvmcc,
  AS_HELP_STRING([--with-llvmcc=<name>],
    [Choose the LLVM capable compiler to use (llvm-gcc, clang, or none; default=check)]),
    [],[with_llvmcc=check])
AC_MSG_CHECKING([LLVM capable compiler])
if test "$with_llvmcc" != "check"; then
   if (test "$with_llvmcc" != "llvm-gcc" &&
       test "$with_llvmcc" != "clang" &&
       test "$with_llvmcc" != "none"); then
      AC_MSG_ERROR([invalid value for --with-llvmcc, expected 'clang', 'llvm-gcc', or 'none'.])
   fi
   WITH_LLVMCC="$with_llvmcc"
elif test -n "$WITH_CLANGPATH" || test "$WITH_BUILT_CLANG" -ne "0"; then
   WITH_LLVMCC=clang
else
   WITH_LLVMCC=none
fi
AC_MSG_RESULT([$WITH_LLVMCC])
AC_SUBST(LLVMCC_OPTION,$WITH_LLVMCC)

dnl Allow a specific llvm-gcc/llvm-g++ pair to be used with this LLVM config.
AC_ARG_WITH(llvmgccdir,
  AS_HELP_STRING([--with-llvmgccdir],
    [Specify location of llvm-gcc install dir (default searches PATH)]),,
    withval=default)
case "$withval" in
  default) WITH_LLVMGCCDIR=default ;;
  /* | [[A-Za-z]]:[[\\/]]*)      WITH_LLVMGCCDIR=$withval ;;
  *) AC_MSG_ERROR([Invalid path for --with-llvmgccdir. Provide full path]) ;;
esac

if test "$WITH_LLVMGCCDIR" = "default" ; then
  LLVMGCC="llvm-gcc${EXEEXT}"
  LLVMGXX="llvm-g++${EXEEXT}"
  AC_PATH_PROG(LLVMGCC, $LLVMGCC, [])
  AC_PATH_PROG(LLVMGXX, $LLVMGXX, [])
else
  if test -z "$LLVMGCC"; then
    LLVMGCC="$WITH_LLVMGCCDIR/bin/llvm-gcc${EXEEXT}"
  fi
  if test -z "$LLVMGXX"; then
    LLVMGXX="$WITH_LLVMGCCDIR/bin/llvm-g++${EXEEXT}"
  fi

  AC_SUBST(LLVMGCC,$LLVMGCC)
  AC_SUBST(LLVMGXX,$LLVMGXX)
fi

dnl Check whether llvm-gcc is based on dragonegg
AC_CACHE_CHECK([whether llvm-gcc is dragonegg],[llvm_cv_llvmgcc_dragonegg],
[llvm_cv_llvmgcc_dragonegg="no"
if test "$LLVMCC_OPTION" == "llvm-gcc" ; then
  cp /dev/null conftest.c
  $LLVMGCC -fplugin-arg-dragonegg-emit-ir -S -o - conftest.c > /dev/null 2>&1
  if test $? -eq 0 ; then
    llvm_cv_llvmgcc_dragonegg="yes"
  fi
  rm conftest.c
fi])


dnl Set the flags needed to emit LLVM IR and to disable optimizations
dnl in clang/llvm-gcc.
if test "$llvm_cv_llvmgcc_dragonegg" = "yes" ; then
  LLVMCC_EMITIR_FLAG="-fplugin-arg-dragonegg-emit-ir"
  LLVMCC_DISABLEOPT_FLAGS="-fplugin-arg-dragonegg-llvm-ir-optimize=0"
else
  LLVMCC_EMITIR_FLAG="-emit-llvm"
  LLVMCC_DISABLEOPT_FLAGS="-mllvm -disable-llvm-optzns"
fi

AC_SUBST(LLVMCC_EMITIR_FLAG)

dnl See if the llvm-gcc executable can compile to LLVM assembly
AC_CACHE_CHECK([whether llvm-gcc is sane],[llvm_cv_llvmgcc_sanity],
[llvm_cv_llvmgcc_sanity="no"
if test "$LLVMCC_OPTION" != none ; then
  cp /dev/null conftest.c
  $LLVMGCC "$LLVMCC_EMITIR_FLAG" -S -o - conftest.c | \
      grep 'target datalayout =' > /dev/null 2>&1
  if test $? -eq 0 ; then
    llvm_cv_llvmgcc_sanity="yes"
  fi
  rm conftest.c
fi])

dnl Since we have a sane llvm-gcc, identify it and its sub-tools
dnl Furthermore, add some information about the tools
if test "$LLVMCC_OPTION" == "llvm-gcc" ; then
  AC_MSG_CHECKING([llvm-gcc component support])
  llvmcc1path=`$LLVMGCC --print-prog-name=cc1`
  AC_SUBST(LLVMCC1,$llvmcc1path)
  llvmcc1pluspath=`$LLVMGCC --print-prog-name=cc1plus`
  AC_SUBST(LLVMCC1PLUS,$llvmcc1pluspath)
  llvmgccdir=`echo "$llvmcc1path" | sed 's,/libexec/.*,,'`
  AC_SUBST(LLVMGCCDIR,$llvmgccdir)
  llvmgcclangs=[`$LLVMGCC -v --help 2>&1 | grep '^Configured with:' | sed \
's/^.*--enable-languages=\([^ ]*\).*/\1/'`]
  AC_SUBST(LLVMGCC_LANGS,$llvmgcclangs)
  AC_SUBST(LLVMGCC_DRAGONEGG,$llvm_cv_llvmgcc_dragonegg)
  AC_SUBST(LLVMCC_DISABLEOPT_FLAGS)
  AC_MSG_RESULT([ok])
fi

dnl Configure the default locations of the external benchmarks
EXTERNAL_BENCHMARK(spec95,${LLVM_EXTERNALS}/spec95/benchspec)
EXTERNAL_BENCHMARK(spec2000,${LLVM_EXTERNALS}/speccpu2000/benchspec,[CINT2000])
EXTERNAL_BENCHMARK(spec2006,${LLVM_EXTERNALS}/speccpu2006/benchspec,[CPU2006])
EXTERNAL_BENCHMARK(povray,${LLVM_EXTERNALS}/povray31)
EXTERNAL_BENCHMARK(namd,${LLVM_EXTERNALS}/spec_namd)
EXTERNAL_BENCHMARK(sweep3d,${LLVM_EXTERNALS}/sweep3d)
EXTERNAL_BENCHMARK(fpgrowth,${LLVM_EXTERNALS}/fpgrowth)
EXTERNAL_BENCHMARK(alp,${LLVM_EXTERNALS}/alp)
EXTERNAL_BENCHMARK(nurbs,${LLVM_EXTERNALS}/nurbs)
EXTERNAL_BENCHMARK(hmmer,${LLVM_EXTERNALS}/hmmer)
EXTERNAL_BENCHMARK(skidmarks10,${LLVM_EXTERNALS}/skidmarks10)

dnl LLC Diff Option
AC_ARG_ENABLE(llc_diffs,
  AS_HELP_STRING(--enable-llc_diffs,
                 [Enable LLC Diffs when testing (default is YES)]),,
              enableval=yes)
if test ${enableval} = "no"
then
      AC_SUBST(DISABLE_LLC_DIFFS,[DISABLE_LLC_DIFFS:=1])
else
      AC_SUBST(DISABLE_LLC_DIFFS,[[]])
fi

dnl **************************************************************************
dnl * Check for things needed by the test suite programs
dnl **************************************************************************

dnl Check for compilation tools
AC_PROG_CXX
AC_PROG_CC(gcc)
AC_PROG_CPP

dnl Verify that GCC is version 3.0 or higher

dnl Check for GNU Make.  We use its extensions too, so don't build without it
AC_CHECK_GNU_MAKE
if test -z "$llvm_cv_gnu_make_command" ; then
  AC_MSG_ERROR([GNU Make required but not found])
fi

dnl Checks for other tools
AC_PROG_LIBTOOL

dnl Check for GNU f2c FORTRAN -> C translator
FIND_STD_PROGRAM(f2c,f2c.h,libf2c.a)
dnl Check for the NAG f95 FORTRAN -> C translator
FIND_STD_PROGRAM(f95,,libf97.dylib,[nag fortran])
CHECK_PROGRAM_SANITY([f95],[-V],[NAGWare Fortran 95],1)
if test -z "$sanity" ; then
  AC_SUBST(USE_F95,[[]])
else
  if test "$sanity" -eq "0" ; then
    AC_SUBST(USE_F95,[[]])
  else
    AC_SUBST(USE_F95,[USE_F95=1])
  fi
fi

dnl Checks for header files.
dnl We don't check for ancient stuff or things that are guaranteed to be there
dnl by the C++ standard. We always use the <cfoo> versions of <foo.h> C headers.
AC_HEADER_SYS_WAIT

dnl Determine if the linker supports the -R option.
AC_LINK_USE_R

dnl Check for needed functions
AC_CHECK_FUNC([re_comp],[AC_SUBST(HAVE_RE_COMP,[HAVE_RE_COMP:=1])],
                        [AC_SUBST(HAVE_RE_COMP,[[]])])

dnl Get the extension used for shared libraries
AC_LTDL_SHLIBEXT
AC_SUBST(SHLIBEXT,$libltdl_cv_shlibext)

dnl Add --program-prefix value to Makefile.rules. Already an ARG variable.
test "x$program_prefix" = "xNONE" && program_prefix=""
AC_SUBST([program_prefix])

dnl Create the output files
AC_OUTPUT
