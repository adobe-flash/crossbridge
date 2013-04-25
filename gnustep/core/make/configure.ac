#
#   configure.ac
#
#   Copyright (C) 1997-2007 Free Software Foundation, Inc.
#
#   Author:  Scott Christley <scottc@net-community.com>
#	     Ovidiu Predescu <ovidiu@net-community.com>
#   Rewrite: Adam Fedor <fedor@gnu.org>
#            Nicola Pero <n.pero@mi.flashnet.it>
#
#   This file is part of the GNUstep Makefile Package.
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 3
#   of the License, or (at your option) any later version.
#   
#   You should have received a copy of the GNU General Public
#   License along with this library; see the file COPYING.
#   If not, write to the Free Software Foundation,
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

AC_INIT
AC_PREREQ(2.57)
AC_CONFIG_SRCDIR([application.make])

#
# TODO: This configure file should not contain any checks that depend on the
# Objective-C runtime being used or available.  This is because on
# some platforms the Objective-C runtime itself is compiled using
# gnustep-make!
#
# All the runtime-dependent checks should go in gnustep-base.
#
# We still need compiler checks, because if we are compiling the
# Objective-C runtime itself, we need to know what compiler we have
# and what flags we can use.  But these checks should test
# compilation, not linking or execution, because we don't necessarily
# have an Objective-C runtime available yet.
#

#--------------------------------------------------------------------
# Setup the library combination
#--------------------------------------------------------------------
targetArgument=${target}
AC_PROG_CC
AC_PROG_CPP

# We also look for a C++ compiler.  While not strictly needed, some
# people use gnustep-make to compile C++ code.  It's nice to detect a
# C++ compiler, if we have one, and automatically use it to
# compile/link C++ code. :-)
AC_PROG_CXX

# Similarly for the ObjC++ compiler ... but only new versions of autoconf
# support it, so we pretend by using the C++ compiler
#AC_PROG_OBJCXX
OBJCXX="${CXX}"

AC_CANONICAL_TARGET([])


#--------------------------------------------------------------------
# Check if objc should use garbage collection.
#--------------------------------------------------------------------
AC_ARG_ENABLE(objc-garbage-collection, [
--enable-objc-garbage-collection
   Enable objc garbage collection.  Use this option if you want
   to use garbage collection for objective-c software. This is equivalent
   to using --with-library-combo=gnugc-gnu-gnu to set the runtime library
   to be the gnu runtime with garbage collection.
], 
OBJC_WITH_GC=$enableval,
OBJC_WITH_GC="")

if test "$OBJC_WITH_GC" = "yes"; then
  ac_cv_library_combo="gnugc-gnu-gnu"
elif test "$OBJC_WITH_GC" = "no"; then
  if test "$ac_cv_library_combo"="gnugc-gnu-gnu"; then
    ac_cv_library_combo="gnu-gnu-gnu"
  fi
else
  OBJC_WITH_GC=no
fi
AC_SUBST(OBJC_WITH_GC)

AC_MSG_CHECKING(for library combo)
AC_ARG_WITH(library-combo,[
--with-library-combo
   Define the default "library combo".  The library combo is a string
   of the form aaa-bbb-ccc where 'aaa' is the Objective-C runtime
   library to use (examples are 'gnu' and 'apple'), 
   'bbb' is the Foundation library to use (examples are 'gnu' for
   gnustep-base, and 'apple' for Apple Cocoa FoundationKit),
   and 'ccc' is the ApplicationKit to use (examples are 'gnu'
   for gnustep-gui and 'apple' for Apple Cocoa AppKit).  Use this 
   option if you want to force a different default library combo than 
   the one that would be used by default.  For example, on Darwin GNUstep
   will automatically use the Apple Objective-C frameworks by
   default (library-combo=apple-apple-apple); if you are planning 
   on installing and using gnustep-base on there, you would need 
   to use --with-library-combo=gnu-gnu-gnu instead.  Please notice
   that if --disable-flattened is used, gnustep-make can have fat
   binaries that support multiple library combos.  In that case,
   this flag will only configure the default one, but you can still
   use other ones at run-time.
],
ac_cv_library_combo=$withval,
ac_cv_library_combo=$ac_cv_library_combo
)

if test "$ac_cv_library_combo" = "" -o "$ac_cv_library_combo" = "no"; then
  case "$host_os" in
    darwin*)   ac_cv_library_combo=apple-apple-apple ;;
    nextstep4) ac_cv_library_combo=nx-nx-nx          ;;
    openstep4) ac_cv_library_combo=nx-nx-nx          ;;
    *)         ac_cv_library_combo=gnu-gnu-gnu       ;;
  esac
fi

case "$ac_cv_library_combo" in
  apple) ac_cv_library_combo=apple-apple-apple ;;
  gnu)   ac_cv_library_combo=gnu-gnu-gnu ;;
  nx)    ac_cv_library_combo=nx-nx-nx ;;
esac

AC_SUBST(ac_cv_library_combo)
AC_MSG_RESULT($ac_cv_library_combo)

OBJC_RUNTIME_LIB=`echo $ac_cv_library_combo | awk -F- '{print $1}'`

#--------------------------------------------------------------------
# Check if we are using Apple cc
#--------------------------------------------------------------------
cc_cppprecomp=0
cc_byndle=0
AC_MSG_CHECKING([for apple compiler flags])
cc_cppprecomp=`${CC} -no-cpp-precomp 2>&1 | grep -c "unrecognized"`
cc_bundle=`${CC} -bundle 2>&1 | grep -c "couldn"`
# 0 means we have the flag
if test $cc_cppprecomp = 0; then
  cc_cppprecomp=yes
else
  cc_cppprecomp=no
fi
if test $cc_bundle = 0; then
  cc_bundle=yes
else
  cc_bundle=no
fi
AC_MSG_RESULT($cc_bundle)
AC_SUBST(cc_cppprecomp)
AC_SUBST(cc_bundle)

#--------------------------------------------------------------------
# specific target_os options
#--------------------------------------------------------------------
case "$target_os" in
  freebsd* | openbsd* )	
	        INCLUDES="$INCLUDES -I/usr/local/include"
		LIB_DIR="$LIB_DIR -L/usr/local/lib";;
  netbsd*)	INCLUDES="$INCLUDES -I/usr/pkg/include"
		LIB_DIR="$LIB_DIR -Wl,-R/usr/pkg/lib -L/usr/pkg/lib";;
esac

#--------------------------------------------------------------------
# Determine the host, build, and target systems
#--------------------------------------------------------------------
case $host_os in
  *cygwin*  )
	MINGW32=no
	CYGWIN=yes
	MSWIND=yes;;
  *mingw32* )
	MINGW32=yes
	CYGWIN=no
	MSWIND=yes;;
          * )
	MINGW32=no
	CYGWIN=no
	MSWIND=no;;
esac
AC_SUBST(CYGWIN)

AC_EXEEXT
AC_OBJEXT
if test "$MINGW32" = yes; then
  echo "hosted on mingw32 .."
  export SHELL=sh
  export CC=${CC:-gcc}
  export AR=${AR:-ar}
  export RANLIB=${RANLIB:-ranlib}
  export DLLTOOL=${DLLTOOL:-dlltool}
elif test "$CYGWIN" = yes; then
  echo "hosted on cygwin .."
  export CC=${CC:-gcc}
  export AR=${AR:-ar}
  export RANLIB=${RANLIB:-ranlib}
  export DLLTOOL=${DLLTOOL:-dlltool}
fi

#--------------------------------------------------------------------
# Find the binary and compile tools
#--------------------------------------------------------------------
if test "x$target" != "x$host"; then
  echo "cross compiling from $host to $target .."
  cross_compiling="yes"
  AC_CHECK_PROG(CC,      "${targetArgument}-gcc",     dnl
                         "${targetArgument}-gcc",     gcc)
  AC_CHECK_PROG(RANLIB,  "${targetArgument}-ranlib",  dnl
                         "${targetArgument}-ranlib",  ranlib)
  AC_CHECK_PROG(AR,      "${targetArgument}-ar",      dnl
                         "${targetArgument}-ar",      ar)
  AC_CHECK_PROG(DLLTOOL, "${targetArgument}-dlltool", dnl
                         "${targetArgument}-dlltool", dlltool)
else
  AC_CHECK_PROG(AR,      ar,      ar)
  AC_CHECK_PROG(DLLTOOL, dlltool, dlltool)
  AC_PROG_RANLIB
fi

AC_PROG_INSTALL

AC_MSG_CHECKING([if 'install -p' works])
gs_install_p_test_builddir="`pwd`"
gs_install_p_test_results=`(INSTALL="$INSTALL"; export INSTALL; cd "$srcdir/config-install-p-test/"; ./run-test.sh "$gs_install_p_test_builddir"; echo $?) 2>&5`
if test "$gs_install_p_test_results" = "0"; then
  AC_MSG_RESULT(yes);
else
  AC_MSG_RESULT(no);
fi

AC_MSG_CHECKING([if we should use 'install -p' when installing files])
AC_ARG_ENABLE(install-p, [
--disable-install-p
  Disable using 'install -p' when installing files.  By default,
  assuming that 'install -p' works, when installing files such as header files or libraries, 
  gnustep-make uses 'install -p', which preserves the original timestamp of the header file or
  library.  If you do not want the timestamp to be preserved, use this option
  to have gnustep-make use 'install' instead of 'install -p'.
  Unless you have a specific reason for not liking the default behaviour
  this option is most likely irrelevant for you.
], 
  ac_cv_install_p=$enableval,
  ac_cv_install_p="yes")

if test "$ac_cv_install_p" = "yes"; then
  if test "$gs_install_p_test_results" = "0"; then
    AC_MSG_RESULT(yes);
    INSTALL="${INSTALL} -p"
  else
    AC_MSG_RESULT(no: install -p does not work);
  fi
else
  AC_MSG_RESULT(no);
fi

AC_PROG_LN_S([])

AC_CHECK_PROGS(TAR, gnutar gtar, tar)
AC_ARG_WITH(tar,[
--with-tar
   Set the name of the tar program to use.  Use this option if the
   default choice does not work for you.
],
  TAR="$withval",)

AC_CHECK_PROG(CHOWN, chown, chown, none)
if test "$MINGW32" = no; then
  if test "$CHOWN" = "none"; then
    AC_MSG_ERROR("Could not find chown.");
  fi
fi

#-------------------------------------------------------------------
# GNUstep specific options follow
#-------------------------------------------------------------------

#-------------------------------------------------------------------
# Determine if we should enable strict gnustep-make v2 mode by default
#-------------------------------------------------------------------

AC_MSG_CHECKING([if we should enable strict gnustep-make version 2 mode by default])
AC_ARG_ENABLE(strict-v2-mode, [
--enable-strict-v2-mode
  Enable strict gnustep-make version 2 mode by default.  Use this
  option to have gnustep-make be aggressively backwards-incompatible
  with gnustep-make version 1.  You can use it when you are debugging /
  upgrading your makefiles and software to gnustep-make version 2 and
  want to test test that they do not depend on obsolete or deprecated
  version 1 features and variables.  Important: you must never ship
  gnustep-make to end-users with --enable-strict-v2-mode as it would
  cause extremely confusing and dangerous problems with software that
  has not been updated to gnustep-make v2 yet.  Use it only on test
  builds or development machines to test that makefiles and software
  have been updated.  Make sure to use it extremely carefully with old
  makefiles because it can produce unpredictable and dangerous results
  (for examples, since GNUSTEP_SYSTEM_ROOT is undefined when using
  --enable-strict-v2-mode, installing into GNUSTEP_SYSTEM_ROOT/Headers
  would actually install into /Headers).
], 
  ac_cv_strict_v2_mode=$enableval,
  ac_cv_strict_v2_mode="no")

if test "$ac_cv_strict_v2_mode" = "yes"; then
  AC_MSG_RESULT(yes);
  GNUSTEP_MAKE_STRICT_V2_MODE="yes"
  AC_MSG_WARN(Use strict version 2 mode at your own risk; it must only be used on development/test systems)
else
  AC_MSG_RESULT(no);
  GNUSTEP_MAKE_STRICT_V2_MODE="no"
fi

AC_SUBST(GNUSTEP_MAKE_STRICT_V2_MODE)

#-------------------------------------------------------------------
# Determine filesystem layout to use
#-------------------------------------------------------------------

AC_MSG_CHECKING([for GNUstep filesystem layout to use])
AC_ARG_WITH(layout,[
--with-layout=FILE
   Set the type of filesystem layout that you want your GNUstep
   installation to use.  The layout described by FILE will be
   read from the FilesystemLayouts directory.  Check the 
   FilesystemLayouts subdirectory for a list of layouts and how 
   to choose which one you want (or create your own).
   If you do not specify a layout then 'fhs' is used except
   when the library-combo is apple-apple-apple, in which case it is 'apple'.
   Example: --with-layout=fhs
],
  GNUSTEP_FILESYSTEM_LAYOUT="$withval",)

# The variable GNUSTEP_FILESYSTEM_LAYOUT is empty if no layout was
# specified on the command line.
if test ! x"$GNUSTEP_FILESYSTEM_LAYOUT" = x""; then
  AC_MSG_RESULT($GNUSTEP_FILESYSTEM_LAYOUT)
  GNUSTEP_FILESYSTEM_LAYOUT_FILE="$GNUSTEP_FILESYSTEM_LAYOUT"
else
  # This is the default layout that is used when installing
  # GNUstep with no other indication.  We normally use 'fhs'.
  # However, if the library-combo is apple-apple-apple we use
  # 'apple' irrespective of the target OS.
  if test "$ac_cv_library_combo" = "apple-apple-apple"; then
    AC_MSG_RESULT(default layout: 'apple' since we're on apple-apple-apple)
    GNUSTEP_FILESYSTEM_LAYOUT_FILE=apple
  else
    GNUSTEP_FILESYSTEM_LAYOUT_FILE=fhs
    AC_MSG_RESULT(default layout: '$GNUSTEP_FILESYSTEM_LAYOUT_FILE')
  fi
fi

if test ! -f "$srcdir/FilesystemLayouts/$GNUSTEP_FILESYSTEM_LAYOUT_FILE" >&5 2>&5; then
  echo "Can not find $srcdir/FilesystemLayouts/$GNUSTEP_FILESYSTEM_LAYOUT_FILE"
  echo "Please check your --with-layout=xxx argument and try again."
  echo "Valid arguments for --with-layout= are:"
  echo `ls "$srcdir/FilesystemLayouts/" | grep -v README`
  exit 1
fi

# Need to do some checks related to building dylibs on darwin.
GNUSTEP_ABSOLUTE_INSTALL_PATHS=;

case "$target_os" in
  darwin*)
    AC_MSG_CHECKING([if we should build dynamic libraries with an absolute install_name])
    AC_ARG_ENABLE(absolute-install-paths, [
--enable-absolute-install-paths
  Enable the use of absolute paths for the install_name of dynamic
  libraries on Darwin.  This option is specific to Darwin and is
  ignored on all other platforms.  Any code that links against a
  dynamic library with an absolute install_name will tell dyld to
  find the library at that location.  Enabling this option allows
  one to place libraries in non-standard locations without having
  to set DYLD_LIBRARY_PATH and DYLD_FRAMEWORK_PATH in the shell
  and ~/.MacOSX/environment.plist, but libraries built with an
  absolute install_name are not relocatable.  By default, this
  setting is disabled unless the gnu-gnu-gnu library combo is
  used, in which case it is enabled.
    ],
      ac_cv_absolute_install_paths=$enableval,
      ac_cv_absolute_install_paths="undefined")

    if test "$ac_cv_absolute_install_paths" = "yes"; then
      AC_MSG_RESULT(yes)
      GNUSTEP_ABSOLUTE_INSTALL_PATHS=yes;
    else
      if test "$ac_cv_absolute_install_paths" = "undefined"; then
        # gnu-gnu-gnu on darwin is difficult because of the risk of
        # conflicts between the GNU Objective-C runtime and the Apple
        # one.  absolute-install-paths makes this slightly easier
        # by hardcoding the absolute path of dynamic libraries into the
        # programs that link them so that it's less likely that the
        # wrong dynamic libraries will be pulled in.  So we enable it
        # by default in that case.
        if test "$ac_cv_library_combo" = "gnu-gnu-gnu"; then
           GNUSTEP_ABSOLUTE_INSTALL_PATHS=yes;
           AC_MSG_RESULT(yes)
        else
           AC_MSG_RESULT(no)
        fi
      else
        AC_MSG_RESULT(no)        
      fi
    fi

    ;;
esac

AC_SUBST(GNUSTEP_ABSOLUTE_INSTALL_PATHS)

#-------------------------------------------------------------------
# Read filesystem layout from file
#-------------------------------------------------------------------
AC_MSG_CHECKING([if we manage to import the filesystem layout configuration])

. "$srcdir/FilesystemLayouts/$GNUSTEP_FILESYSTEM_LAYOUT_FILE"

if test ! x"$GNUSTEP_DEFAULT_PREFIX" = x""; then
  AC_MSG_RESULT(ok)
else
  AC_MSG_RESULT(failed)
  echo "Please check your $srcdir/FilesystemLayouts/$GNUSTEP_FILESYSTEM_LAYOUT_FILE"
  echo "layout file, as it can not be loaded (GNUSTEP_DEFAULT_PREFIX missing)"
  exit 1
fi

# Now, all MAKEFILES/SYSTEM/LOCAL/NETWORK paths in the filesystem
# layout file will be relative to prefix.  So we need to immediately
# determine prefix, and use it.

#--------------------------------------------------------------------
# Process --prefix
#--------------------------------------------------------------------

# We use this to make sure we know if the user has passed a
# --prefix=xxx option or not.  We use it if it was passed.
#
# If it wasn't passed, then we want to know because in that case we
# want to use the specified default prefix for that filesystem.  For
# example, the 'gnustep' filesystem will install by default in
# /usr/GNUstep, while the 'fhs' filesystem will install by default in
# /usr/local.  Please note that AC_PREFIX_DEFAULT will actually be
# done at the very beginning of ./configure, not here.  So we wouldn't
# have access to all the filesystem layout information yet, which is
# why we only use the macro to know if a --prefix=xxx was passed or
# not.
AC_PREFIX_DEFAULT(NONE)

AC_MSG_CHECKING([for prefix])
if test "x$prefix" = "xNONE"; then
  # Use the default prefix for this filesystem layout
  GNUSTEP_PREFIX="$GNUSTEP_DEFAULT_PREFIX";
else
  # Use the prefix that the user specified
  GNUSTEP_PREFIX="$prefix";
fi

# Remove any '/' at the end of prefix.  All system/network/local paths
# from layout files must start with a '/'.  We don't want a '/' at the
# end of the prefix to generate a '//' mostly because common.make will
# try to double-check that GNUSTEP_SYSTEM_TOOLS is in your PATH.  Any
# '//' in GNUSTEP_SYSTEM_TOOLS will have been normalized to '/' in
# PATH (on some systems at least) and so comparing them won't find a
# match and the check will fail even if GNUSTEP_SYSTEM_TOOLS is in
# PATH and a spurious warning will be produced.
GNUSTEP_PREFIX=`echo "$GNUSTEP_PREFIX" | sed 's%/*$%%'`

AC_MSG_RESULT($GNUSTEP_PREFIX)

# Now we apply the prefix (we don't need to apply it to
# GNUSTEP_SYSTEM_USERS_DIR and similar, which are something like
# '/home' - we never install anything in there).
GNUSTEP_MAKEFILES="$GNUSTEP_PREFIX$GNUSTEP_MAKEFILES"

GNUSTEP_SYSTEM_APPS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_APPS"
GNUSTEP_SYSTEM_ADMIN_APPS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_ADMIN_APPS"
GNUSTEP_SYSTEM_WEB_APPS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_WEB_APPS"
GNUSTEP_SYSTEM_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_TOOLS"
GNUSTEP_SYSTEM_ADMIN_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_ADMIN_TOOLS"
GNUSTEP_SYSTEM_LIBRARY="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_LIBRARY"
GNUSTEP_SYSTEM_HEADERS="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_HEADERS"
GNUSTEP_SYSTEM_LIBRARIES="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_LIBRARIES"
GNUSTEP_SYSTEM_DOC="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_DOC"
GNUSTEP_SYSTEM_DOC_MAN="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_DOC_MAN"
GNUSTEP_SYSTEM_DOC_INFO="$GNUSTEP_PREFIX$GNUSTEP_SYSTEM_DOC_INFO"

GNUSTEP_NETWORK_APPS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_APPS"
GNUSTEP_NETWORK_ADMIN_APPS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_ADMIN_APPS"
GNUSTEP_NETWORK_WEB_APPS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_WEB_APPS"
GNUSTEP_NETWORK_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_TOOLS"
GNUSTEP_NETWORK_ADMIN_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_ADMIN_TOOLS"
GNUSTEP_NETWORK_LIBRARY="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_LIBRARY"
GNUSTEP_NETWORK_HEADERS="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_HEADERS"
GNUSTEP_NETWORK_LIBRARIES="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_LIBRARIES"
GNUSTEP_NETWORK_DOC="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_DOC"
GNUSTEP_NETWORK_DOC_MAN="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_DOC_MAN"
GNUSTEP_NETWORK_DOC_INFO="$GNUSTEP_PREFIX$GNUSTEP_NETWORK_DOC_INFO"

GNUSTEP_LOCAL_APPS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_APPS"
GNUSTEP_LOCAL_ADMIN_APPS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_ADMIN_APPS"
GNUSTEP_LOCAL_WEB_APPS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_WEB_APPS"
GNUSTEP_LOCAL_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_TOOLS"
GNUSTEP_LOCAL_ADMIN_TOOLS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_ADMIN_TOOLS"
GNUSTEP_LOCAL_LIBRARY="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_LIBRARY"
GNUSTEP_LOCAL_HEADERS="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_HEADERS"
GNUSTEP_LOCAL_LIBRARIES="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_LIBRARIES"
GNUSTEP_LOCAL_DOC="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_DOC"
GNUSTEP_LOCAL_DOC_MAN="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_DOC_MAN"
GNUSTEP_LOCAL_DOC_INFO="$GNUSTEP_PREFIX$GNUSTEP_LOCAL_DOC_INFO"

#---------------------------------------------------------------------
# Location of the GNUstep.conf config file (--with-config-file)
#---------------------------------------------------------------------
AC_MSG_CHECKING([for GNUstep configuration file to use])
AC_ARG_WITH(config-file,[
--with-config-file=PATH
   Set the path of the system GNUstep config file.  Use this option
   if you want to have the GNUstep config file in a non-standard
   place.
   Example: --with-config-file=/usr/GNUstep/GNUstep.conf
   By default, if this option is not specified, the config file is
   /etc/GNUstep/GNUstep.conf if the prefix is /, /usr or /usr/GNUstep
   and $prefix/etc/GNUstep/GNUstep.conf otherwise.  On Apple, it is
   always /Library/GNUstep/GNUstep.conf regardless of prefix.
],
  GNUSTEP_CONFIG_FILE="$withval",)
if test "$GNUSTEP_CONFIG_FILE" = "" -o "$GNUSTEP_CONFIG_FILE" = "no"; then
  case "$target_os" in
    darwin*)              GNUSTEP_CONFIG_FILE=/Library/GNUstep/GNUstep.conf ;;
    *)  case x"$GNUSTEP_PREFIX" in
          x)             GNUSTEP_CONFIG_FILE=/etc/GNUstep/GNUstep.conf;;
          x/usr)         GNUSTEP_CONFIG_FILE=/etc/GNUstep/GNUstep.conf;;
          x/usr/GNUstep) GNUSTEP_CONFIG_FILE=/etc/GNUstep/GNUstep.conf;;
          *)             GNUSTEP_CONFIG_FILE="$GNUSTEP_PREFIX/etc/GNUstep/GNUstep.conf";;
        esac ;;
  esac
fi
if echo "$GNUSTEP_CONFIG_FILE" | grep '[[ 	\\]] > /dev/null'
then
  echo "found a backslash or space (illegal) in '$GNUSTEP_CONFIG_FILE'"
  echo "Please try again using --with-config-file= to specify a valid path."
  if test "$MSWIND" = "yes"
  then
    echo "Please note that on windows you must use unix-style paths within"
    echo "the make package even though gnustep programs built in the mingw32"
    echo "environment use native paths throughout."
  fi
  exit 1
fi
AC_MSG_RESULT($GNUSTEP_CONFIG_FILE)
AC_SUBST(GNUSTEP_CONFIG_FILE)

#---------------------------------------------------------------------
# Now read/import the existing configuration file, if any
#---------------------------------------------------------------------

# Reading/importing the existing configuration file might be good as a
# default because it means that you don't have to type in your
# layout/prefix settings every time you configure
# gnustep-make. (please note we only read the system-wide one, not the
# user one.  Reason being that the settings we read will be used to
# generate the new system-wide one, while the user one will be left
# untouched).

# Please note that this option will override any --with-layout=xxx and
# --prefix=yyy options.

AC_MSG_CHECKING([if we should import an existing configuration file])
AC_ARG_ENABLE(importing-config-file, [
--enable-importing-config-file
   Enable importing the existing system GNUstep configuration file.
   Use this option to use an existing configuration file for
   setting up the filesystem layout and prefix.  If you specify this
   option, all the configuration will be read from the existing
   system GNUstep configuration file and any --with-layout=xxx 
   and --prefix=yyy options will be ignored.
], 
  ac_cv_importing_config_file=$enableval,
  ac_cv_importing_config_file="no")

if test "$ac_cv_importing_config_file" = "yes"; then
 if test ! -f "$GNUSTEP_CONFIG_FILE"; then
   AC_MSG_RESULT([no: file "$GNUSTEP_CONFIG_FILE" does not exist])
   AC_MSG_ERROR([asked to import non-existing configuration file.  To fix this problem, make sure you have a config file to import, or remove the --enable-importing-config-file option.])
 else
   AC_MSG_RESULT([yes: trying to import "$GNUSTEP_CONFIG_FILE"])
   AC_MSG_NOTICE([If this fails, run configure again with --disable-importing-config-file])
   . "$GNUSTEP_CONFIG_FILE"
 fi
else
  AC_MSG_RESULT([no])
fi


#--------------------------------------------------------------------
# Important - from now on, any variable that is set in the filesystem
# layout and/or configuration file (eg, GNUSTEP_SYSTEM_TOOLS) could
# already have a value that we have imported from the files.
# ./configure command line options should override those values, but
# otherwise we should keep them!
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# Process --with-user-config-file
#--------------------------------------------------------------------
AC_MSG_CHECKING(for user config file to use)
AC_ARG_WITH(user-config-file,[
--with-user-config-file
   Set the name of the user config file to use.  This can be 
   relative to the user's home directory if it is a relative path,
   or an absolute directory (the same for all users) if it is an
   absolute path.  Use '' if you want to disable user config files.
   The default is .GNUstep.conf if not specified.
   Example: --with-user-config-file=GNUstep/GNUstep.conf
],
GNUSTEP_USER_CONFIG_FILE="$withval",)
# Keep in mind we already have a default value set by the filesystem
# layout, so it should never be empty.
if echo "$GNUSTEP_USER_CONFIG_FILE" | grep '[[ 	\\]] > /dev/null'
then
  echo "found a backslash or space (illegal) in '$GNUSTEP_USER_CONFIG_FILE'"
  echo "Please try again using --with-user-config-file= to specify a value."
  if test "$MSWIND" = "yes"
  then
    echo "Please note that on windows you must use unix-style paths within"
    echo "the make package even though gnustep programs built in the mingw32"
    echo "environment use native paths throughout."
  fi
  exit 1
fi
AC_MSG_RESULT($GNUSTEP_USER_CONFIG_FILE)
AC_SUBST(GNUSTEP_USER_CONFIG_FILE)

#--------------------------------------------------------------------
# Process --with-user-dir
#--------------------------------------------------------------------
AC_MSG_CHECKING(if the obsolete --with-user-dir option was used)
AC_ARG_WITH(user-dir,[
--with-user-dir
   This is an obsolete option.  It used to determine the location of
   the USER domain directory (inside of each user's home directory)
   under the 'gnustep' layout.  We ignore this option so that you can
   write ./configure commands that work with both old and new
   gnustep-makes.  To get the effect of this option with this version
   of gnustep-make, please create your own layout file and use
   --with-layout=xxx to use it.
],
OBSOLETE_GNUSTEP_USER_DIR="$withval",)
if test ! x"$OBSOLETE_GNUSTEP_USER_DIR" = x"" && test ! x"$OBSOLETE_GNUSTEP_USER_DIR" = x"no"; then 
  AC_MSG_RESULT([$OBSOLETE_GNUSTEP_USER_DIR: ignored])
  AC_MSG_WARN([ignoring --with-user-dir=$OBSOLETE_GNUSTEP_USER_DIR option])
else
  AC_MSG_RESULT(no: good)
fi

#--------------------------------------------------------------------
# Process --with-user-defaults-dir
#--------------------------------------------------------------------
AC_MSG_CHECKING(for user defaults dir to use)
AC_ARG_WITH(user-defaults-dir,[
--with-user-defaults-dir
   Set the GNUstep user defaults directory for all users.  This can be
   relative to the user's home directory if it is a relative path,
   or an absolute directory (the same for all users) if it is an
   absolute path.  Use this option if you want to have the 
   GNUSTEP_USER_DEFAULTS_DIR directory in a non default place for
   all users.  The default is 'GNUstep/Defaults'
   Example: --with-user-defaults-dir='GNUstep/Library/Defaults'
],
GNUSTEP_USER_DEFAULTS_DIR="$withval",)
# Keep in mind we already have a default value set by the filesystem
# layout, so it should never be empty.
if echo "$GNUSTEP_USER_DEFAULTS_DIR" | grep '[[ 	\\]] > /dev/null'
then
  echo "found a backslash or space (illegal) in '$GNUSTEP_USER_DEFAULTS_DIR'"
  echo "Please try again using --with-user-defaults-dir= to specify a value."
  if test "$MSWIND" = "yes"
  then
    echo "Please note that on windows you must use unix-style paths within"
    echo "the make package even though gnustep programs built in the mingw32"
    echo "environment use native paths throughout."
  fi
  exit 1
fi
AC_MSG_RESULT($GNUSTEP_USER_DEFAULTS_DIR)
AC_SUBST(GNUSTEP_USER_DEFAULTS_DIR)

#--------------------------------------------------------------------
# Setting up GNUSTEP_MAKEFILES
#--------------------------------------------------------------------
#
# This is where you install gnustep-make.  We just print it out.
#
AC_MSG_CHECKING(for GNUSTEP_MAKEFILES to use)
AC_SUBST(GNUSTEP_MAKEFILES)
AC_MSG_RESULT($GNUSTEP_MAKEFILES)
# If GNUSTEP_MAKEFILES contains a space, we may have problems later
# because make does not really support having spaces in filenames
if echo "$GNUSTEP_MAKEFILES" | grep " " >/dev/null 2>&1; then
  AC_MSG_WARN([GNUSTEP_MAKEFILES ($GNUSTEP_MAKEFILES) contains spaces: this may not work])
fi

#--------------------------------------------------------------------
# Setting up the install-sh script
#--------------------------------------------------------------------

# HOST_INSTALL is the name of the install program in config.make so set it up
# to point to the install-sh script in the GNUstep tree if no system install is
# found.
AC_SUBST(HOST_INSTALL)
if test "$INSTALL" = "$ac_install_sh"; then
  HOST_INSTALL="$GNUSTEP_MAKEFILES/$INSTALL"
else
  HOST_INSTALL="$INSTALL"
fi

#--------------------------------------------------------------------
# Is the system flattened?
#--------------------------------------------------------------------
AC_MSG_CHECKING(for flattened directory structure)
AC_ARG_ENABLE(flattened, [
--disable-flattened
   Disable flattened directory structure.  Use this option if you want
   to have support for multiple library combos and fat binaries.  A
   library combo specifies the Objective-C frameworks to use to compile
   a program, so having multiple library combos is only useful if you 
   have (or plan to have) multiple OpenStep-like Objective-C frameworks 
   installed on your machine, for example both the Apple Cocoa Objective-C 
   frameworks and the GNUstep frameworks.  Fat binaries allow you to
   have multiple versions for different CPUs and Operating Systems.  
   Please note that using the fat directory structure will not blend 
   easily with native filesystems and so if you want the non-flattened
   directory structure you probably want to install GNUstep using its 
   own default filesystem layout.  To switch between different 
   library-combos, you also need to source GNUstep.sh after
   setting the LIBRARY_COMBO environment variable.  Please refer to
   the documentation for more information on library-combos and fat
   binaries.
], 
  ac_cv_flattened=$enableval,
  ac_cv_flattened="undefined")

if test "$ac_cv_flattened" = "no"; then

  # GNUSTEP_FLATTENED is here for backwards compatibility only and
  # should be removed at some point.  It has the problem that it
  # defaults to 'no' while we want the default to be 'yes'!
  # The new GNUSTEP_IS_FLATTENED variable defaults to 'yes'. :-)

  GNUSTEP_FLATTENED=;
  GNUSTEP_IS_FLATTENED=no;

  # FIXME - maybe we should have a warning here if you try to 
  # use 'fhs' or 'fhs-system' layout with non-flattened!
else
  GNUSTEP_FLATTENED=yes;
  GNUSTEP_IS_FLATTENED=yes;
fi
AC_SUBST(GNUSTEP_FLATTENED)
AC_SUBST(GNUSTEP_IS_FLATTENED)

if test "$GNUSTEP_IS_FLATTENED" = "yes"; then
  AC_MSG_RESULT(yes);
else
  AC_MSG_RESULT(no);
fi

#--------------------------------------------------------------------
# Output the full filesystem layout
#--------------------------------------------------------------------

# Note: We print out the entire filesystem layout so that people can
# see with their eyes the result of the configuration options that
# they used.  This is good to immediately spot mistakes.
AC_MSG_NOTICE([Now printing the filesystem layout configuration.])

AC_MSG_CHECKING([for System Applications directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_APPS)
AC_SUBST(GNUSTEP_SYSTEM_APPS)
AC_MSG_CHECKING([for System Admin Applications directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_ADMIN_APPS)
AC_SUBST(GNUSTEP_SYSTEM_ADMIN_APPS)
AC_MSG_CHECKING([for System Web Applications directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_WEB_APPS)
AC_SUBST(GNUSTEP_SYSTEM_WEB_APPS)
AC_MSG_CHECKING([for System Tools directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_TOOLS)
AC_SUBST(GNUSTEP_SYSTEM_TOOLS)
AC_MSG_CHECKING([for System Admin Tools directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_ADMIN_TOOLS)
AC_SUBST(GNUSTEP_SYSTEM_ADMIN_TOOLS)
AC_MSG_CHECKING([for System Library directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_LIBRARY)
AC_SUBST(GNUSTEP_SYSTEM_LIBRARY)
AC_MSG_CHECKING([for System Headers directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_HEADERS)
AC_SUBST(GNUSTEP_SYSTEM_HEADERS)
AC_MSG_CHECKING([for System Libraries directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_LIBRARIES)
AC_SUBST(GNUSTEP_SYSTEM_LIBRARIES)
AC_MSG_CHECKING([for System Documentation directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_DOC)
AC_SUBST(GNUSTEP_SYSTEM_DOC)
AC_MSG_CHECKING([for System Info Documentation directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_DOC_INFO)
AC_SUBST(GNUSTEP_SYSTEM_DOC_INFO)
AC_MSG_CHECKING([for System Man Documentation directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_DOC_MAN)
AC_SUBST(GNUSTEP_SYSTEM_DOC_MAN)

AC_MSG_CHECKING([for Network Applications directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_APPS)
AC_SUBST(GNUSTEP_NETWORK_APPS)
AC_MSG_CHECKING([for Network Admin Applications directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_ADMIN_APPS)
AC_SUBST(GNUSTEP_NETWORK_ADMIN_APPS)
AC_MSG_CHECKING([for Network Web Applications directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_WEB_APPS)
AC_SUBST(GNUSTEP_NETWORK_WEB_APPS)
AC_MSG_CHECKING([for Network Tools directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_TOOLS)
AC_SUBST(GNUSTEP_NETWORK_TOOLS)
AC_MSG_CHECKING([for Network Admin Tools directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_ADMIN_TOOLS)
AC_SUBST(GNUSTEP_NETWORK_ADMIN_TOOLS)
AC_MSG_CHECKING([for Network Library directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_LIBRARY)
AC_SUBST(GNUSTEP_NETWORK_LIBRARY)
AC_MSG_CHECKING([for Network Headers directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_HEADERS)
AC_SUBST(GNUSTEP_NETWORK_HEADERS)
AC_MSG_CHECKING([for Network Libraries directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_LIBRARIES)
AC_SUBST(GNUSTEP_NETWORK_LIBRARIES)
AC_MSG_CHECKING([for Network Documentation directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_DOC)
AC_SUBST(GNUSTEP_NETWORK_DOC)
AC_MSG_CHECKING([for Network Info Documentation directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_DOC_INFO)
AC_SUBST(GNUSTEP_NETWORK_DOC_INFO)
AC_MSG_CHECKING([for Network Man Documentation directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_DOC_MAN)
AC_SUBST(GNUSTEP_NETWORK_DOC_MAN)

AC_MSG_CHECKING([for Local Applications directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_APPS)
AC_SUBST(GNUSTEP_LOCAL_APPS)
AC_MSG_CHECKING([for Local Admin Applications directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_ADMIN_APPS)
AC_SUBST(GNUSTEP_LOCAL_ADMIN_APPS)
AC_MSG_CHECKING([for Local Web Applications directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_WEB_APPS)
AC_SUBST(GNUSTEP_LOCAL_WEB_APPS)
AC_MSG_CHECKING([for Local Tools directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_TOOLS)
AC_SUBST(GNUSTEP_LOCAL_TOOLS)
AC_MSG_CHECKING([for Local Admin Tools directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_ADMIN_TOOLS)
AC_SUBST(GNUSTEP_LOCAL_ADMIN_TOOLS)
AC_MSG_CHECKING([for Local Library directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_LIBRARY)
AC_SUBST(GNUSTEP_LOCAL_LIBRARY)
AC_MSG_CHECKING([for Local Headers directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_HEADERS)
AC_SUBST(GNUSTEP_LOCAL_HEADERS)
AC_MSG_CHECKING([for Local Libraries directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_LIBRARIES)
AC_SUBST(GNUSTEP_LOCAL_LIBRARIES)
AC_MSG_CHECKING([for Local Documentation directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_DOC)
AC_SUBST(GNUSTEP_LOCAL_DOC)
AC_MSG_CHECKING([for Local Info Documentation directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_DOC_INFO)
AC_SUBST(GNUSTEP_LOCAL_DOC_INFO)
AC_MSG_CHECKING([for Local Man Documentation directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_DOC_MAN)
AC_SUBST(GNUSTEP_LOCAL_DOC_MAN)

AC_MSG_CHECKING([for User Applications directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_APPS)
AC_SUBST(GNUSTEP_USER_DIR_APPS)
AC_MSG_CHECKING([for User Admin Applications directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_ADMIN_APPS)
AC_SUBST(GNUSTEP_USER_DIR_ADMIN_APPS)
AC_MSG_CHECKING([for User Web Applications directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_WEB_APPS)
AC_SUBST(GNUSTEP_USER_DIR_WEB_APPS)
AC_MSG_CHECKING([for User Tools directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_TOOLS)
AC_SUBST(GNUSTEP_USER_DIR_TOOLS)
AC_MSG_CHECKING([for User Admin Tools directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_ADMIN_TOOLS)
AC_SUBST(GNUSTEP_USER_DIR_ADMIN_TOOLS)
AC_MSG_CHECKING([for User Library directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_LIBRARY)
AC_SUBST(GNUSTEP_USER_DIR_LIBRARY)
AC_MSG_CHECKING([for User Headers directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_HEADERS)
AC_SUBST(GNUSTEP_USER_DIR_HEADERS)
AC_MSG_CHECKING([for User Libraries directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_LIBRARIES)
AC_SUBST(GNUSTEP_USER_DIR_LIBRARIES)
AC_MSG_CHECKING([for User Documentation directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_DOC)
AC_SUBST(GNUSTEP_USER_DIR_DOC)
AC_MSG_CHECKING([for User Info Documentation directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_DOC_INFO)
AC_SUBST(GNUSTEP_USER_DIR_DOC_INFO)
AC_MSG_CHECKING([for User Man Documentation directory])
AC_MSG_RESULT($GNUSTEP_USER_DIR_DOC_MAN)
AC_SUBST(GNUSTEP_USER_DIR_DOC_MAN)

AC_MSG_CHECKING([for System User directory])
AC_MSG_RESULT($GNUSTEP_SYSTEM_USERS_DIR)
AC_SUBST(GNUSTEP_SYSTEM_USERS_DIR)
AC_MSG_CHECKING([for Network User directory])
AC_MSG_RESULT($GNUSTEP_NETWORK_USERS_DIR)
AC_SUBST(GNUSTEP_NETWORK_USERS_DIR)
AC_MSG_CHECKING([for Local User directory])
AC_MSG_RESULT($GNUSTEP_LOCAL_USERS_DIR)
AC_SUBST(GNUSTEP_LOCAL_USERS_DIR)

#--------------------------------------------------------------------
# These variables no longer exist!  We try to set some compatibility
# values for them that should work with the old 'gnustep' layout.
# So things using the old 'gnustep' layout should keep working.
# These variables won't have any meaning with the new layouts.
# They are deprecated and they *will* be removed.
#--------------------------------------------------------------------
GNUSTEP_SYSTEM_ROOT="$GNUSTEP_PREFIX/System"
GNUSTEP_NETWORK_ROOT="$GNUSTEP_PREFIX/Network"
GNUSTEP_LOCAL_ROOT="$GNUSTEP_PREFIX/Local"
GNUSTEP_USER_DIR="GNUstep"
AC_SUBST(GNUSTEP_SYSTEM_ROOT)
AC_SUBST(GNUSTEP_NETWORK_ROOT)
AC_SUBST(GNUSTEP_LOCAL_ROOT)
AC_SUBST(GNUSTEP_USER_DIR)

#--------------------------------------------------------------------
# Is the system multi-platform?
#--------------------------------------------------------------------
#
# Multi-platform means that GNUstep.sh will determine the host
# platform (by running config.guess) each time that it is sourced.
# This is good if you are sharing your GNUstep.sh across your network
# (for example, mounting the makefiles via NFS), but it requires you
# to be able to run config.guess on your machine(s), which usually
# requires a development environment (compiler, libc etc).
#
# The default instead is not using multi-platform, which means the
# local host os, cpu and version is hardcoded in GNUstep.sh.  This
# works nicely for a single machine using this gnustep-make
# installation, and it works even if you don't have development
# packages (gcc, binutils, libc-dev etc) installed.  We had to make
# this the default after end-users (with no development packages
# installed) complained that binary packages wouldn't work (and the
# reason turned out to be that GNUstep.sh was running config.guess
# which was returning the wrong platform because the development
# tools needed/used to determine the platform were not available).
#
# Unless you know what you are doing, stick with the default, which is
# also much faster when sourcing GNUstep.sh.
#
AC_ARG_ENABLE(multi-platform, [
--enable-multi-platform
   Enable run-time multi-platform support.  If this option is enabled,
   then every time GNUstep.sh is run it will determine/guess the type
   of local host machine instead of using the hardcoded value.  Use
   this together with --disable-flattened if you have a single GNUstep
   installation with fat binaries that is being shared over the network
   by a variety of machines with different hardware and o/s.
], 
  ac_cv_multi_platform=$enableval,
  ac_cv_multi_platform="undefined")

if test "$ac_cv_multi_platform" = "yes"; then
  GNUSTEP_MULTI_PLATFORM=yes;
else
  GNUSTEP_MULTI_PLATFORM=;
fi
AC_SUBST(GNUSTEP_MULTI_PLATFORM)

#--------------------------------------------------------------------
# Build backend bundles (on by default)
#--------------------------------------------------------------------
AC_ARG_ENABLE(backend-bundle, [
--disable-backend-bundle
   Compile GUI backend as a library.  Use this option if the default
   compilation of the GUI backend as a bundle (loadable module) is
   not supported / does not work on your machine.
], 
  ac_cv_backend=$enableval,  
  ac_cv_backend="yes")

if test "$ac_cv_backend" = "yes"; then
  BACKEND_BUNDLE=yes;
else
  BACKEND_BUNDLE=;
fi
AC_SUBST(BACKEND_BUNDLE)

#--------------------------------------------------------------------
# Miscellaneous flags and setup
#--------------------------------------------------------------------

# TODO: This check for a custom ObjC library needs to be moved to
# gnustep-base since it concerns the runtime library (see explanations
# at the beginning of this file).

if test "$GNUSTEP_IS_FLATTENED" != yes; then
  clean_target_os=`$srcdir/clean_os.sh $target_os`
  clean_target_cpu=`$srcdir/clean_cpu.sh $target_cpu`
  obj_dir="$clean_target_cpu/$clean_target_os"
fi

# Check to see if the libobjc library is in our GNUSTEP_SYSTEM_LIBRARIES
# If so, there are probably other libraries that we want there also, so
# leave the proper includes in CPPFLAGS and LDFLAGS
AC_MSG_CHECKING(for custom shared objc library)
AC_CACHE_VAL(gs_cv_objc_libdir,
[dnl
gs_cv_objc_libdir=NONE
gs_cv_objc_incdir=NONE
# Try GNUSTEP_SYSTEM_LIBRARIES first
if test "$GNUSTEP_IS_FLATTENED" = yes; then
  GNUSTEP_LDIR="$GNUSTEP_SYSTEM_LIBRARIES"
  GNUSTEP_HDIR="$GNUSTEP_SYSTEM_HEADERS"
else
  GNUSTEP_LDIR="$GNUSTEP_SYSTEM_LIBRARIES/$obj_dir"
  GNUSTEP_HDIR="$GNUSTEP_SYSTEM_HEADERS/$LIBRARY_COMBO"
fi

if test -f "$GNUSTEP_HDIR/objc/objc.h"; then
  if test -f "$GNUSTEP_LDIR/libobjc.a" -o -f "$GNUSTEP_LDIR/libobjc.so" -o -f "$GNUSTEP_LDIR/libobjc.dll.a" -o -f "$GNUSTEP_LDIR/libobjc-gnu.dylib"; then
    gs_cv_objc_libdir="$GNUSTEP_LDIR"
    gs_cv_objc_incdir="$GNUSTEP_HDIR"
    gs_cv_objc_tools="$GNUSTEP_SYSTEM_TOOLS"
  fi
fi

# Try GNUSTEP_NETWORK_LIBRARIES second (override GNUSTEP_SYSTEM if
# found)
if test "$GNUSTEP_IS_FLATTENED" = yes; then
  GNUSTEP_LDIR="$GNUSTEP_NETWORK_LIBRARIES"
  GNUSTEP_HDIR="$GNUSTEP_NETWORK_HEADERS"
else
  GNUSTEP_LDIR="$GNUSTEP_NETWORK_LIBRARIES/$obj_dir"
  GNUSTEP_HDIR="$GNUSTEP_NETWORK_HEADERS/$LIBRARY_COMBO"
fi

if test -f "$GNUSTEP_HDIR/objc/objc.h"; then
  if test -f "$GNUSTEP_LDIR/libobjc.a" -o -f "$GNUSTEP_LDIR/libobjc.so" -o -f "$GNUSTEP_LDIR/libobjc.dll.a" -o -f "$GNUSTEP_LDIR/libobjc-gnu.dylib"; then
    gs_cv_objc_libdir="$GNUSTEP_LDIR"
    gs_cv_objc_incdir="$GNUSTEP_HDIR"
    gs_cv_objc_tools="$GNUSTEP_NETWORK_TOOLS"
  fi
fi

# Try GNUSTEP_LOCAL_LIBRARIES third (override GNUSTEP_SYSTEM and
# GNUSTEP_NETWORK if found)
if test "$GNUSTEP_IS_FLATTENED" = yes; then
  GNUSTEP_LDIR="$GNUSTEP_LOCAL_LIBRARIES"
  GNUSTEP_HDIR="$GNUSTEP_LOCAL_HEADERS"
else
  GNUSTEP_LDIR="$GNUSTEP_LOCAL_LIBRARIES/$obj_dir"
  GNUSTEP_HDIR="$GNUSTEP_LOCAL_HEADERS/$LIBRARY_COMBO"
fi

if test -f "$GNUSTEP_HDIR/objc/objc.h"; then
  if test -f "$GNUSTEP_LDIR/libobjc.a" -o -f "$GNUSTEP_LDIR/libobjc.so" -o -f "$GNUSTEP_LDIR/libobjc.dll.a" -o -f "$GNUSTEP_LDIR/libobjc-gnu.dylib"; then
    gs_cv_objc_libdir="$GNUSTEP_LDIR"
    gs_cv_objc_incdir="$GNUSTEP_HDIR"
    gs_cv_objc_tools="$GNUSTEP_LOCAL_TOOLS"
  fi
fi
#gcc_shared_libobjc=`gcc -print-file-name=libobjc.so`
#if test -f "$gcc_shared_libobjc"; then
#  gs_cv_objc_libdir=`dirname $gcc_shared_libobjc`
#fi
])
AC_MSG_RESULT($gs_cv_objc_libdir)

if test "$gs_cv_objc_libdir" != "NONE"; then
  # The following are needed to compile the test programs
  OBJC_CPPFLAGS="$CPPFLAGS $INCLUDES -I$gs_cv_objc_incdir"
  OBJC_LDFLAGS="$LDFLAGS $LIB_DIR -L$gs_cv_objc_libdir"

  # And the following to execute them
  LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$gs_cv_objc_libdir"
  export LD_LIBRARY_PATH
  # Need to also add the Tools library on mingw
  case $host_os in
    *mingw32* )
      PATH=$PATH:$gs_cv_objc_tools;;
    * )
      ;;
  esac
fi

# Special case for Apple systems: When compiling plain C source files that
# include Objective-C runtime headers we must make sure that the correct
# header files are used with a gnu-*-* combo. The -fgnu-runtime compiler
# option takes care of this when compiling Objective-C source files, but
# has no effect when compiling plain C (or C++) source files.
cc_gnuruntime=
case $target_os-$ac_cv_library_combo in
  darwin*-gnu-*-* )
    if test "$gs_cv_objc_libdir" = "NONE"; then
      AC_MSG_CHECKING(GNU Objective-C runtime header directory)
      install_dir="`$CC -print-search-dirs | sed -n 's/install: //p'`"
      if test -n "${install_dir}" && \
        test -d "${install_dir}"include-gnu-runtime; then
        cc_gnuruntime="${install_dir}"include-gnu-runtime
        AC_MSG_RESULT($cc_gnuruntime)
      else
        AC_MSG_RESULT(NONE)
      fi
    fi
    ;;
esac
AC_SUBST(cc_gnuruntime)

# TODO: This flag needs to be moved to gnustep-base since it concerns
# the runtime library (see explanations at the beginning of this
# file).
AC_MSG_CHECKING(for the flag to link libobjc)
AC_ARG_WITH(objc-lib-flag,[
--with-objc-lib-flag
   Specify a different flag to link libobjc (the Objective-C runtime
   library).  The default is -lobjc.  In some situations you may have
   multiple versions of libobjc installed and if your linker supports
   it you may want to specify exactly which one must be used; for
   example on GNU/Linux you should be able to use
     --with-objc-lib-flag=-l:libobjc.so.1
   to request libobjc.so.1 (as opposed to, say, libobjc.so.2) to be
   linked.
],
OBJC_LIB_FLAG="$withval",
OBJC_LIB_FLAG=""
)
# We pass the flag, unchanged, to gnustep-make - if it's empty,
# gnustep-make will be free to use -lobjc or -lobjc_gc depending on
# the case.
AC_SUBST(OBJC_LIB_FLAG)

# But we need to compute, and print out, what flag we're using now.
OBJC_FINAL_LIB_FLAG="$OBJC_LIB_FLAG"

saved_CFLAGS="$CFLAGS"
saved_LIBS="$LIBS"
CFLAGS="$CFLAGS -x objective-c -I$srcdir $OBJC_CPPFLAGS $OBJC_LDFLAGS"
if test "$OBJC_RUNTIME_LIB" = "gnu"; then
  CFLAGS="$CFLAGS -fgnu-runtime -DGNU_RUNTIME"
  if test "$OBJC_FINAL_LIB_FLAG" = ""; then
    OBJC_FINAL_LIB_FLAG=-lobjc
  fi
  OBJCRT="$OBJC_FINAL_LIB_FLAG"
fi
if test "$OBJC_RUNTIME_LIB" = "gnugc"; then
  CFLAGS="$CFLAGS -fgnu-runtime -DGNU_RUNTIME"
  if test "$OBJC_FINAL_LIB_FLAG" = ""; then
    OBJC_FINAL_LIB_FLAG=-lobjc_gc
  fi
  OBJCRT="$OBJC_FINAL_LIB_FLAG -ldl -lgc"
fi
if test "$OBJC_RUNTIME_LIB" = "nx"; then
  CFLAGS="$CFLAGS -DNeXT_RUNTIME"
  if test "$OBJC_FINAL_LIB_FLAG" = ""; then
    OBJC_FINAL_LIB_FLAG=-lobjc
  fi
  OBJCRT="$OBJC_FINAL_LIB_FLAG"
fi
if test "$OBJC_RUNTIME_LIB" = "apple"; then
  CFLAGS="$CFLAGS -DAPPLE_RUNTIME"
  if test "$OBJC_FINAL_LIB_FLAG" = ""; then
    OBJC_FINAL_LIB_FLAG=-lobjc
  fi
  OBJCRT="$OBJC_FINAL_LIB_FLAG"
fi

AC_MSG_RESULT($OBJC_FINAL_LIB_FLAG)

#--------------------------------------------------------------------
# Check if libobjc was compiled with thread support.
#--------------------------------------------------------------------
# TODO: This check needs to be moved to gnustep-base since it concerns
# the runtime library (see explanations at the beginning of this
# file).
OBJC_THREAD=
AC_ARG_WITH(thread-lib,[
--with-thread-lib
   Specify alternate thread library.  Use this flag if configure can
   not properly determine the thread library used by your libobjc.
],
OBJC_THREAD=$withval,
OBJC_THREAD=
)
if test "$OBJC_THREAD" = no; then
OBJC_THREAD=
fi


AC_MSG_CHECKING(whether objc has thread support)
if test "$OBJC_THREAD" != ""; then
  LIBS="$OBJCRT $LIBS $OBJC_THREAD"
  AC_TRY_RUN([#include "config_thread.m"], 
	objc_threaded="$OBJC_THREAD",
	objc_threaded="", objc_threaded="")
elif test "$host_os" = linux-gnu; then
  LIBS="$OBJCRT -lpthread"
  AC_TRY_RUN([#include "config_thread.m"], objc_threaded="-lpthread",
	objc_threaded="", objc_threaded="-lpthread")
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT"
    AC_TRY_RUN([#include "config_thread.m"],
          objc_threaded="works",
	  objc_threaded="", objc_threaded="works")
  fi
elif test "`echo $host_os|sed 's/[[0-9]].*//'|sed s/elf//`" = freebsd; then
  LIBS="-pthread $OBJCRT"
  AC_TRY_RUN([#include "config_thread.m"], objc_threaded="-pthread",
       objc_threaded="", objc_threaded="-pthread")
  if test x"$objc_threaded" = x""; then
    LIBS="-lpthread $OBJCRT"
    AC_TRY_RUN([#include "config_thread.m"], objc_threaded="-lpthread",
	objc_threaded="", objc_threaded="-lpthread")
  fi
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT -lpcthread"
    AC_TRY_RUN([#include "config_thread.m"], objc_threaded="-lpcthread",
	objc_threaded="", objc_threaded="-lpcthread")
  fi
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT"
    AC_TRY_RUN([#include "config_thread.m"],
          objc_threaded="works",
	  objc_threaded="", objc_threaded="works")
  fi
elif test "$MINGW32" = yes; then
  # Mingw doesn't need anything extra for threads
  LIBS="$OBJCRT $LIBS"
  AC_TRY_RUN([#include "config_thread.m"], 
	objc_threaded="works",
	objc_threaded="", objc_threaded="works")
elif test "$OBJC_RUNTIME_LIB" = "apple"; then
  # Apple doesn't need anything extra for threads
  LIBS="$OBJCRT $LIBS"
  AC_TRY_RUN([#include "config_thread.m"], 
	objc_threaded="works",
	objc_threaded="", objc_threaded="works")
else
  LIBS="$OBJCRT $LIBS"
  AC_TRY_RUN([#include "config_thread.m"], 
	objc_threaded="works",
	objc_threaded="", objc_threaded="")
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT $saved_LIBS -lpthread "
    AC_TRY_RUN([#include "config_thread.m"], 
    	objc_threaded="-lpthread", 
	objc_threaded="", objc_threaded="")
  fi
  if test x"$objc_threaded" = x""; then
    # Solaris, OpenBSD/sparc
    LIBS="$OBJCRT $saved_LIBS -lpthread -lposix4"
    AC_TRY_RUN([#include "config_thread.m"], 
    	objc_threaded="-lpthread -lposix4", 
	objc_threaded="", objc_threaded="")
  fi
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT $saved_LIBS -lthread "
    AC_TRY_RUN([#include "config_thread.m"], 
    	objc_threaded="-lthread", 
	objc_threaded="", objc_threaded="")
  fi
  if test x"$objc_threaded" = x""; then
    LIBS="$OBJCRT"
    AC_TRY_RUN([#include "config_thread.m"], 
          objc_threaded="works",
	  objc_threaded="", objc_threaded="works")
  fi
fi
if test x"$objc_threaded" = x""; then
  AC_MSG_RESULT(no)
else
  if test x"$objc_threaded" = x"works"; then
    objc_threaded=""
  fi
  AC_MSG_RESULT(yes: $objc_threaded)
fi
ac_cv_objc_threaded="$objc_threaded"
AC_SUBST(objc_threaded)
AC_SUBST(ac_cv_objc_threaded)

AC_MSG_CHECKING(whether Objective-C++ is supported)

# Only new autoconf supports ObjC++ so we use C++ instead for now
#AC_LANG_PUSH(Objective C++)
#OBJCXXFLAGS_saved="$OBJCXXFLAGS"
#OBJCXXFLAGS="$OBJCXXFLAGS -x objective-c++"
#OBJCXXPPFLAGS_saved="$OBJCXXPPFLAGS"
#OBJCXXPPFLAGS="$OBJCXXPPFLAGS -x objective-c++"
AC_LANG_PUSH(C++)
CXXFLAGS_saved="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -x objective-c++"
CXXPPFLAGS_saved="$CXXPPFLAGS"
CXXPPFLAGS="$CXXPPFLAGS -x objective-c++"

AC_COMPILE_IFELSE([[

@interface	ObjCClass
{
  int x;
}
@end

class CppClass
{
  int x;
};

int
main()
{
  return 0;
}
]], objcc=yes, objcc=no)
AC_MSG_RESULT($objcc)
if test x"$objcc" = x"no"; then
  OBJCXX=
fi
AC_SUBST(OBJCXX)
# Only new autoconf supports ObjC++ so we use C++ instead for now
#OBJCXXFLAGS="$OBJCXXFLAGS_saved"
#OBJCXXPPFLAGS="$OBJCXXPPFLAGS_saved"
#AC_LANG_POP(Objective C++)
CXXFLAGS="$CXXFLAGS_saved"
CXXPPFLAGS="$CXXPPFLAGS_saved"
AC_LANG_POP(C++)

#--------------------------------------------------------------------
# Check if GCC supports -fobjc-nonfragile-abi, and if so, turn it on!
#--------------------------------------------------------------------

# TODO: Enable it by default if the compiler supports it :-)
AC_ARG_ENABLE(objc-nonfragile-abi, [
--enable-objc-nonfragile-abi
   Use the non-fragile ABI for Objective-C.  Use this option if you want
   to use non-fragile instance variables provided by clang and libobjc2.
], 
USE_NONFRAGILE_ABI=$enableval,
USE_NONFRAGILE_ABI=unset)

AC_MSG_CHECKING(whether we should use the nonfragile ABI)
if test x"$USE_NONFRAGILE_ABI" = x"yes"; then
  # What we want to do: set USE_NONFRAGILE_ABI to yes if we can compile
  # something with -fobjc-nonfragile-abi.
  CFLAGS_no_nonfragile="$CFLAGS"
  CFLAGS="$CFLAGS -fobjc-nonfragile-abi"
  AC_COMPILE_IFELSE([[
/* Note that we never execute this code so it does not really matter
   what it is.  We are testing that the compiler accepts the
   '-fobjc-nonfragile-abi' flag.  */
int
main()
{
#ifndef __has_feature
#define __has_feature(x) 0
#endif
return __has_feature(objc_nonfragile_abi) ? 0 : 1;
}
]], USE_NONFRAGILE_ABI=yes, USE_NONFRAGILE_ABI=no)
  AC_MSG_RESULT($USE_NONFRAGILE_ABI)
  CFLAGS="$CFLAGS_no_nonfragile"

  if test x$USE_NONFRAGILE_ABI = xno; then
    AC_MSG_NOTICE([The nonfragile ABI was requested, but the compiler])
    AC_MSG_NOTICE([doesn't support it.])
    AC_MSG_ERROR([compiler doesn't support nonfragile ABI])
  fi
else
  if test x$USE_NONFRAGILE_ABI != xno; then
  default_nonfragile=`echo -e '#ifndef __has_feature\n#define __has_feature(x) 0\n#endif\n__has_feature(objc_nonfragile_abi)'|$CC $CFLAGS -E -x objective-c - -o - |tail -1`
  
    if test x$default_nonfragile = x1; then
      USE_NONFRAGILE_ABI=yes;
    else
      USE_NONFRAGILE_ABI=no;
    fi
  fi
  if test x$USE_NONFRAGILE_ABI = xyes; then
    AC_MSG_RESULT([yes (compiler default)])
  else
    AC_MSG_RESULT(not requested by user)
  fi
fi

AC_SUBST(USE_NONFRAGILE_ABI)


# Keep LIBS and CFLAGS as they are for a while - we need them in 
# the following Objective-C tests!

#--------------------------------------------------------------------
# Check if we should use -r or -Wl,-r when doing partial linking
#--------------------------------------------------------------------

# Ideally, we'd use -r or -Wl,-r everywhere and that would work with
# both GCC and clang.  But -r doesn't work with clang on some
# platforms, while using -Wl,-r is a problem with GCC on some Sparc
# systems, where GCC automatically passes --relax to the linker unless
# you specify -r.  If you specify -Wl,-r, it still passes --relax,
# producing the error
#
#   ld: --relax and -r may not be used together
#
# To work around this, we use -Wl,-r with clang, and -r with GCC.
AC_MSG_CHECKING(for the flag to use to do partial linking)
gs_cv_using_clang=`${CC} --version 2>&5 | grep -c "clang"`
if test x"${gs_cv_using_clang}" = x"0" ; then
  OBJ_MERGE_CMD_FLAG=-r
  AC_MSG_RESULT([-r])
else
  OBJ_MERGE_CMD_FLAG=-Wl,-r
  AC_MSG_RESULT([-Wl,-r])
fi

AC_SUBST(OBJ_MERGE_CMD_FLAG)

#--------------------------------------------------------------------
# Check for the GCC version - this is used in the following tests
#--------------------------------------------------------------------

AC_MSG_CHECKING(for the GCC version)
if test ! ${GCC} = "yes" ; then
  AC_MSG_RESULT(no: it's not gcc)
else
  # Running gcc -dumpversion we get something like 2.95.4 or
  #  egcs-2.91.66 or 3.0.2 or 3.1 20011211
  # We want to discard anything but the major number.
  # Explanation of the regexp -
  # \([^0-9]*\) matches non numeric chars at the beginning
  # \([0-9][0-9]*\) matches 1 or more numeric chars (this is the 2^nd
  #  subpattern)
  # \([^0-9]*\) matches one or more non numeric chars
  # \([0-9][0-9]*\) matches 1 or more numeric chars (this is the 4^nd
  #  subpattern)
  # \([^0-9].*\) matches a non numeric char followed by anything
  # /\2/ replace the whole lot with the 2^nd subpattern
  # /\4/ replace the whole lot with the 4^nd subpattern
  # All square brackets are doubled because this file is processed by m4 first.
  # Finally, any error messages are redirected to &5, so that they are logged
  # into config.log but don't clutter the normal user output.
  gs_cv_gcc_major_version=`(${CC} -dumpversion | sed "s/\([[^0-9]]*\)\([[0-9]][[0-9]]*\)\([[^0-9]]*\)\([[0-9]][[0-9]]*\)\([[^0-9]]*.*\)/\2/") 2>&5`;
  gs_cv_gcc_minor_version=`(${CC} -dumpversion | sed "s/\([[^0-9]]*\)\([[0-9]][[0-9]]*\)\([[^0-9]]*\)\([[0-9]][[0-9]]*\)\([[^0-9]]*.*\)/\4/") 2>&5`;

  gs_cv_gcc_parsed_version=${gs_cv_gcc_major_version}.${gs_cv_gcc_minor_version}

  AC_MSG_RESULT(version: ${gs_cv_gcc_parsed_version})
fi

# Do not restore LIBS and CFLAGS yet as we need to test if the
# compiler supports native exceptions.

#--------------------------------------------------------------------
# Check if GCC supports -fobjc-exceptions, and if so, turn it on!
#--------------------------------------------------------------------

AC_ARG_ENABLE(native-objc-exceptions, [
--disable-native-objc-exceptions
   Disable native Objective-C exception support (@try / @catch /
   @finally / @synchronized).  If unspecified, Objective-C exception
   support is enabled if the compiler can compile Objective-C code
   containing @try / @catch / @finally / @synchronized.  Please note
   that native exceptions also require support in the Objective-C
   runtime; GNUstep-base will check for that support and may still
   disable native exceptions if such support is not available.
   Use this option if you do not want to use the native Objective-C
   exception support provided by newer compilers.
], 
USE_OBJC_EXCEPTIONS=$enableval,
USE_OBJC_EXCEPTIONS=maybe)

# Please note that -fobjc-exceptions should automatically enable
# -fexceptions.
AC_MSG_CHECKING(whether the compiler supports native ObjC exceptions)
if test x"$USE_OBJC_EXCEPTIONS" = x"maybe"; then
  # What we want to do: set USE_OBJC_EXCEPTIONS to yes if we can compile
  # something with @try/@catch/@finally in it.
  CFLAGS_no_exceptions="$CFLAGS"
  CFLAGS="$CFLAGS -fexceptions -fobjc-exceptions"
  AC_COMPILE_IFELSE([[
#include <objc/objc.h>
@interface Test { id isa; } @end

int test (Test *o)
{
  @try
    {
      @throw o;
    }
   @catch (id foo)
    {
      if (o != foo)
	return 1;
    }
  return 0;
}
  ]], USE_OBJC_EXCEPTIONS=yes, USE_OBJC_EXCEPTIONS=no)
    if test x$USE_OBJC_EXCEPTIONS = xno; then
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
    fi
    CFLAGS="$CFLAGS_no_exceptions"
else
  AC_MSG_RESULT($USE_OBJC_EXCEPTIONS)
fi

AC_SUBST(USE_OBJC_EXCEPTIONS)

#--------------------------------------------------------------------
# Check if compiler supports -MMD -MP to generate %.d files ...
#--------------------------------------------------------------------

AC_MSG_CHECKING(if the compiler supports autodependencies)

# What we want to do: set AUTO_DEPENDENCIES to yes if gcc => 3.x.  As
# an exception, we disabled them on cygwin because the compiler
# generates autodependencies files containing filenames such as
# G:/GNUstep/mingw/include/stdlib.h which can't really work with GNU
# make because of the ':'.

AUTO_DEPENDENCIES=""
if test "$CYGWIN" = yes; then
  AC_MSG_RESULT(no: autodependencies do not work on cygwin)
elif test ! ${GCC} = "yes" ; then
  AC_MSG_RESULT(no: it's not gcc)
else
  if test "${gs_cv_gcc_major_version}" -ge "3" >&5 2>&5; then
    AUTO_DEPENDENCIES=yes
    AC_MSG_RESULT(yes: gcc version is ${gs_cv_gcc_parsed_version} >= 3.0)
  else
    AC_MSG_RESULT(no: gcc version is ${gs_cv_gcc_parsed_version} < 3.0)
  fi
fi

AC_SUBST(AUTO_DEPENDENCIES)

#--------------------------------------------------------------------
# Check if compiler supports precompiled headers
#--------------------------------------------------------------------

AC_MSG_CHECKING(if the compiler supports precompiled headers)

# We used to check the compiler version here; this is not that
# reliable because precompiled headers were added in 3.4, then they
# were broken in 4.1, then fixed again.

# So we prefer to actually try them out! ;-)

# (Unfortunately, even that is not enough since we have reports of
# 4.1.1 GCC prereleases that pass this simple test, but where any
# serious usage of precompiled headers (ie, including
# Foundation/Foundation.h and AppKit/AppKit.h) doesn't work.  So in
# addition to doing this test, we also forcefully disable precompiled
# headers in 4.1)

# What we want to do: set GCC_WITH_PRECOMPILED_HEADERS to yes if gcc
# can compile and use a precompiled header.

GCC_WITH_PRECOMPILED_HEADERS="no"

# First, a preliminary test.  If this is not gcc, precompiled headers
# are not supported.
if test ! "${GCC}" = "yes" ; then
  AC_MSG_RESULT(no: it's not gcc)
else
  if test "${gs_cv_gcc_parsed_version}" = "4.1" >&5 2>&5; then
    AC_MSG_RESULT(no: gcc 4.1 is often shipped with broken precompiled headers)
  else
    gs_precomp_test_builddir="`pwd`"
    gs_precomp_test_results=`(CC="$CC"; export CC; CFLAGS="$CFLAGS"; export CFLAGS; CPPLAGS="$CPPFLAGS"; export CPPFLAGS; LDFLAGS="$LDFLAGS"; export LDFLAGS; LIBS="$LIBS"; export LIBS; cd "$srcdir/config-precomp-test/"; ./run-test.sh "$gs_precomp_test_builddir"; echo $?) 2>&5`
    if test "$gs_precomp_test_results" = "0"; then
      GCC_WITH_PRECOMPILED_HEADERS="yes"
      AC_MSG_RESULT(yes)
    else
      # Please check the config-precomp-test.log log file for more info
      AC_MSG_RESULT(no: old or buggy gcc)
    fi
  fi
fi

# Important - if you think there is a problem with precompiled
# headers, try adding ADDITIONAL_OBJC_FLAGS = -Winvalid-pch to your
# GNUmakefile to check that they are used.
AC_SUBST(GCC_WITH_PRECOMPILED_HEADERS)

# Restore LIBS and CFLAGS - we used to compile C code after this
# point.  Useful to keep this in case we need to compile C code again.
LIBS="$saved_LIBS"
CFLAGS="$saved_CFLAGS"

#--------------------------------------------------------------------
# Check if compiler requires -shared flag on Solaris ...
#--------------------------------------------------------------------

AC_MSG_CHECKING(if the compiler requires -shared flag to build for Solaris)

# set SOLARIS_SHARED to yes if gcc => 4.x

SOLARIS_SHARED=""
if test ! ${GCC} = "yes" ; then
  AC_MSG_RESULT(no: it's not gcc)
else
  if test "${gs_cv_gcc_major_version}" -ge "4" >&5 2>&5; then
    if test "$host_os" = "solaris2.7" -o $"host_os" = "solaris2.6"; then
      AC_MSG_RESULT(no: solaris 2.6 or 2.7)
    else
      SOLARIS_SHARED=yes
      AC_MSG_RESULT(yes: gcc version is ${gs_cv_gcc_parsed_version} >= 4.0)
    fi
  else
    AC_MSG_RESULT(no: gcc version is ${gs_cv_gcc_parsed_version} < 4.0)
  fi
fi

AC_SUBST(SOLARIS_SHARED)

AC_SUBST(INCLUDES)
AC_SUBST(LIB_DIR)
AC_SUBST(OBJCFLAGS)

#--------------------------------------------------------------------
# Check if we need to enable debug=yes builds by default
#--------------------------------------------------------------------

AC_MSG_CHECKING(if we should enable 'make debug=yes' by default)
AC_ARG_ENABLE(debug-by-default, [
--enable-debug-by-default
   Enable building with 'make debug=yes' by default.  When you use
   gnustep-make to build software, you have a choice of using
   debug=yes or debug=no.  The debug=no will use the default compiler 
   flags determined when gnustep-make was configured (usually -g -O2), 
   while debug=yes will remove the optimization flags and add a 
   number of debugging compiler flags.  If you do not specify the
   --enable-debug-by-default option, gnustep-make will default to
   building with debug=no when nothing is specified.  If you 
   specify the --enable-debug-by-default option, gnustep-make will
   default to building with debug=yes instead, which can be handy if you
   always want to compile software with debug=yes and want to avoid
   having to type debug=yes each time you compile (an alternative
   is to define the variable debug=yes in your shell).  If you are
   unsure, you should stick with the default and ignore this option.
],
  ac_cv_debug_by_default=$enableval,
  ac_cv_debug_by_default="undefined")

if test "$ac_cv_debug_by_default" = "yes"; then
  GNUSTEP_DEFAULT_DEBUG=yes;
  AC_MSG_RESULT(yes);
else
  GNUSTEP_DEFAULT_DEBUG=no;
  AC_MSG_RESULT(no);
fi
AC_SUBST(GNUSTEP_DEFAULT_DEBUG)

#--------------------------------------------------------------------
# Check for GNU make
#--------------------------------------------------------------------

# Search for GNU make.  We try 'gmake' then 'gnumake' then 'make'.  If
# we can't find it, we set it to 'make', even if that will fail later
# on.
AC_CHECK_PROGS(GNUMAKE, [gmake gnumake make], make)

# Used by gnustep-config to launch gnustep-make to compute
# compilation/link flags
AC_SUBST(GNUMAKE)

# If GNU make supports $(info ...), gnustep-make can use it to print
# out a help message at the beginning of each compilation.  This
# feature was introduced in GNU make 3.81, so we check GNU make's
# version number (alternatively, we could try executing a makefile
# that contains an info command).
AC_MSG_CHECKING(for the GNU Make version)

# To get the make version, take the output of 'make --version', read
# only the first line (that we expect to be something like "GNU Make
# 3.81"), and ignore everything up to the first numeric character.
gs_cv_make_version=`($GNUMAKE --version | head -1 | sed -e 's/^[[^0-9]]*//') 2>&5`

# Now check for the major/minor version numbers.
gs_cv_make_major_version=`(echo ${gs_cv_make_version} | sed -e 's/\([[0-9]][[0-9]]*\)[[^0-9]]\([[0-9]][[0-9]]*\).*/\1/') 2>&5`
gs_cv_make_minor_version=`(echo ${gs_cv_make_version} | sed -e 's/\([[0-9]][[0-9]]*\)[[^0-9]]\([[0-9]][[0-9]]*\).*/\2/') 2>&5`
AC_MSG_RESULT(version: ${gs_cv_make_major_version}.${gs_cv_make_minor_version})

#--------------------------------------------------------------------
# Check for GNU Make >= 3.79
#--------------------------------------------------------------------
# We want to emit a warning if they are using GNU make < 3.79 as it's
# no longer supported.  We let them install everything at their own
# risk though.
AC_MSG_CHECKING(for GNU Make >= 3.79)
SUPPORTED_MAKE=no
if test "${gs_cv_make_major_version}" = "3" >&5 2>&5; then
  if test "${gs_cv_make_minor_version}" -ge "79" >&5 2>&5; then
    SUPPORTED_MAKE=yes
  fi
fi

if test "${gs_cv_make_major_version}" -ge "4" >&5 2>&5; then
  SUPPORTED_MAKE=yes
fi

if test "${SUPPORTED_MAKE}" = "yes" >&5 2>&5; then
  AC_MSG_RESULT(yes)
else
  # We do not abort mostly because the checks for GNU make might have
  # gone wrong and returned the wrong version, and because GNU make <
  # 3.79.1 probably works anyway (with the exception of parallel
  # building).
  AC_MSG_RESULT(no)
  AC_MSG_WARN(GNU Make >= 3.79.1 is recommended!  Older versions are no longer supported.  Continue at your own risk.)
fi

#--------------------------------------------------------------------
# Check for $(info ...) function in GNU make
#--------------------------------------------------------------------
AC_MSG_CHECKING(if GNU Make has the info function)

# Things may go wrong (eg, make couldn't be found in one of the
# previous steps), so by default we assume 'no' here.  If things go
# wrong, you'll lost some non-essential features.
MAKE_WITH_INFO_FUNCTION=no
if test "${gs_cv_make_major_version}" = "3" >&5 2>&5; then
  if test "${gs_cv_make_minor_version}" -ge "81" >&5 2>&5; then
    MAKE_WITH_INFO_FUNCTION=yes
  fi
fi

if test "${gs_cv_make_major_version}" -ge "4" >&5 2>&5; then
  MAKE_WITH_INFO_FUNCTION=yes
fi

AC_MSG_RESULT(${MAKE_WITH_INFO_FUNCTION})
AC_SUBST(MAKE_WITH_INFO_FUNCTION)

#--------------------------------------------------------------------
# Shall we strip makefiles upon installation ?
#--------------------------------------------------------------------

# Stripping makefiles removes comments and newlines from them.  The
# resulting stripped makefiles execute around 5% faster on average.
# Too little for it to be worth for the common user who's more
# interested in the comments :-) so it's disabled by default.
AC_MSG_CHECKING(if we should strip makefiles after installation)
AC_ARG_ENABLE(strip-makefiles, [
--enable-strip-makefiles
   Enable stripping system makefiles after installation.  This will 
   speed up gnustep-make by about 5% (average), but will remove all 
   comments from the installed makefiles.  Those comments are quite
   useful if you are trying to find some information on how gnustep-make
   works (eg, if you're trying to locate a bug), and the performance
   gain is quite small, so you probably don't want to strip makefiles.
],
  ac_cv_strip_makefiles=$enableval,
  ac_cv_strip_makefiles="undefined")

if test "$ac_cv_strip_makefiles" = "yes"; then
  GNUSTEP_STRIP_MAKEFILES=strip;
  AC_MSG_RESULT(yes);
else
  GNUSTEP_STRIP_MAKEFILES=;
  AC_MSG_RESULT(no);
fi
AC_SUBST(GNUSTEP_STRIP_MAKEFILES)

#-------------------------------------------------
# Determine if we should enable parallel building
#-------------------------------------------------

AC_MSG_CHECKING([if we should enable support for parallel building])
AC_ARG_ENABLE(parallel-building, [
--disable-parallel-building
  Disable support for parallel building.  Normally this is enabled
  and you can request parallel building of a project by using the '-j N' 
  flag for make (where N is a number, eg, '-j 4').  If you know that 
  you are never going to be using parallel building, you can disable 
  parallel building here - which will ignore all '-j N' flags, and
  also avoid executing a 'compile' submake invocation - resulting
  in slightly faster non-parallel builds, but making it impossible
  to do parallel builds.
],
  ac_cv_parallel_building=$enableval,
  ac_cv_parallel_building="yes")

if test "$ac_cv_parallel_building" = "yes"; then
  AC_MSG_RESULT(yes);
  GNUSTEP_MAKE_PARALLEL_BUILDING="yes"
else
  AC_MSG_RESULT(no);
  GNUSTEP_MAKE_PARALLEL_BUILDING="no"
fi

AC_SUBST(GNUSTEP_MAKE_PARALLEL_BUILDING)


#--------------------------------------------------------------------
# Record the version
#--------------------------------------------------------------------
AC_MSG_CHECKING(for the version of gnustep-make we are compiling)
. "$srcdir/Version"
AC_MSG_RESULT($GNUSTEP_MAKE_VERSION)
AC_SUBST(GNUSTEP_MAKE_VERSION)
AC_SUBST(GNUSTEP_MAKE_MAJOR_VERSION)
AC_SUBST(GNUSTEP_MAKE_MINOR_VERSION)
AC_SUBST(GNUSTEP_MAKE_SUBMINOR_VERSION)

#-------------------------------------------------------------------
# Record the 'clean' target_os, target_cpu and target_vendor
#-------------------------------------------------------------------
# This is just for efficiency, so that core/make/GNUmakefile does not
# have to compute clean_target_os from target_os (and similar) by
# running shell scripts each time you 'make' something inside
# gnustep-make.  We basically compute them once now, and cache them
# forever.  It is also used by GNUstep.sh when multi-platform is
# disabled.
clean_target_os=`$srcdir/clean_os.sh $target_os`
clean_target_cpu=`$srcdir/clean_cpu.sh $target_cpu`
clean_target_vendor=`$srcdir/clean_cpu.sh $target_vendor`
AC_SUBST(clean_target_os)
AC_SUBST(clean_target_cpu)
AC_SUBST(clean_target_vendor)
AC_SUBST(target)

#--------------------------------------------------------------------
# For documentation
#--------------------------------------------------------------------
AC_CHECK_PROGS(LATEX2HTML, latex2html)

#--------------------------------------------------------------------
# For test framework
#--------------------------------------------------------------------
AC_PATH_PROG(SHELLPROG,bash,/bin/sh)
AC_PATH_PROG(TESTPROG, test)

#--------------------------------------------------------------------
# Produce the output files
#--------------------------------------------------------------------
AC_CONFIG_FILES([config-noarch.make config.make openapp opentool 
executable.template GNUmakefile GNUstep.conf GNUstep-strict-v2.conf
GNUstep.sh GNUstep.csh fixpath.sh
gnustep-make.spec gnustep-config TestFramework/gnustep-tests
filesystem.make filesystem.sh filesystem.csh])
AC_CONFIG_COMMANDS([default],
	[[chmod a+x openapp opentool fixpath.sh executable.template]],
	[[]])
AC_OUTPUT
