
AC_INIT(pkg-config.1)

AM_INIT_AUTOMAKE(pkg-config, 0.26)

AM_CONFIG_HEADER(config.h)

AM_PROG_LIBTOOL
dnl when using libtool 2.x create libtool early, because it's used in configure
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

AC_PROG_CC

AC_DEFUN([PKG_CONFIG_FIND_PC_PATH],
[
AC_MSG_CHECKING([for default search path for .pc files])
AC_ARG_WITH(pc_path,
 [  --with-pc-path          Override the default search path for .pc files ],
 [ pc_path="$withval" ],
 [
# This is slightly wrong, but hopefully causes less confusion than
# people being unable to find their .pc files in the standard location.
if test "${prefix}" = "NONE"; then
	pc_path='${libdir}/pkgconfig:${datadir}/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig'
else
	pc_path='${libdir}/pkgconfig:${datadir}/pkgconfig'
fi
])
 AC_MSG_RESULT([$pc_path])
 AC_SUBST([pc_path])

])

PKG_CONFIG_FIND_PC_PATH

AC_MSG_CHECKING([for --with-system-include-path])
AC_ARG_WITH(system_include_path,
 [  --with-system-include-path    Avoid -I flags that add the given directories ],
 [ system_include_path="$withval" ],
 [ system_include_path="/usr/include" ])
 AC_MSG_RESULT([$system_include_path])
 AC_SUBST([system_include_path])

AC_MSG_CHECKING([for --with-system-library-path])
AC_ARG_WITH(system_library_path,
 [  --with-system-library-path    Avoid -L flags that add the given directories ],
 [ system_library_path="$withval" ],
 [
case "$libdir" in
*lib64)
  system_library_path="/usr/lib64:/usr/lib"
  ;;
*)
  system_library_path="/usr/lib"
  ;;
esac
])
 AC_MSG_RESULT([$system_library_path])
 AC_SUBST([system_library_path])

#
# Code taken from gtk+-2.0's configure.in.
#
# This causes pkg-config to not list private dependencies (a very
# common example is libm) on platforms which support inter-library
# dependencies.

AC_ARG_ENABLE(indirect-deps,
        [AC_HELP_STRING([--enable-indirect-deps],
                              [list both direct and indirect dependencies.])
AC_HELP_STRING([--disable-indirect-deps],
                              [only list direct dependencies.])
    [default=auto]],use_indirect_deps=$enableval,
    [use_indirect_deps=auto])
echo use_indirect_deps=$use_indirect_deps >&AS_MESSAGE_LOG_FD
AC_MSG_CHECKING([whether to list both direct and indirect dependencies])
case $use_indirect_deps in
  auto)
    deplibs_check_method=`(./libtool --config; echo eval echo '\"$deplibs_check_method\"') | sh`
    if test "X$deplibs_check_method" = Xnone; then
    echo "foo: $deplibs_check_method" >&AS_MESSAGE_LOG_FD
      use_indirect_deps=yes
    else
      use_indirect_deps=no
    fi
  ;;
  yes|no)
  ;;
  *) AC_MSG_ERROR([Value given to --enable-indirect-deps must be one of yes, no
or auto])
  ;;
esac
AC_MSG_RESULT($use_indirect_deps)

#
# Choose default CFLAGS and warnings depending on compiler.
#

WARN_CFLAGS=""
if test "${GCC}" = "yes" && test "${ac_env_CFLAGS_set}" != "set"; then
   WARN_CFLAGS="-g -Wall -O2"
fi
AC_SUBST(WARN_CFLAGS)

AC_DEFINE_UNQUOTED(ENABLE_INDIRECT_DEPS, `test $use_indirect_deps = no; echo $?`, [Link library to all dependent libraries, not only directly needed ones])
AC_SUBST(use_indirect_deps)

AC_MSG_CHECKING([for Win32])
case "$build" in
  *-*-mingw*)
    native_win32=yes
  ;;
  *)
    native_win32=no
  ;;
esac
AC_MSG_RESULT([$native_win32])
AM_CONDITIONAL(NATIVE_WIN32, [test "x$native_win32" = xyes])

if test "x$GLIB_CFLAGS" = "x" && test "x$GLIB_LIBS" = "x"; then
   AC_CHECK_PROGS([PKG_CONFIG], [pkg-config], [])
   if test -n $PKG_CONFIG && $PKG_CONFIG --exists glib-2.0; then
      GLIB_CFLAGS=`$PKG_CONFIG --cflags glib-2.0`
      GLIB_LIBS=`$PKG_CONFIG --libs glib-2.0`
   else
      AC_MSG_ERROR([pkg-config and glib-2.0 not found, please set GLIB_CFLAGS and GLIB_LIBS to the correct values])]
   fi
fi
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

AC_ARG_WITH([installed-popt],
  [AS_HELP_STRING([--with-installed-popt], [use installed popt library])],
  [with_installed_popt="$withval"],
  [with_installed_popt=no])
if test "x$with_installed_popt" = xyes; then
  _save_libs="$LIBS"
  AC_CHECK_LIB([popt], [poptGetContext], [:],
    [AC_MSG_ERROR([could not link to installed popt library])])
  LIBS="$_save_libs"
  POPT_LIBS=-lpopt
else
  POPT_LIBS=""
fi
AC_SUBST([POPT_LIBS])
AM_CONDITIONAL([USE_INSTALLED_POPT], [test "x$with_installed_popt" = xyes])

AC_FUNC_ALLOCA

AC_CHECK_FUNCS(setresuid setreuid,break)
AC_CHECK_HEADERS([dirent.h unistd.h sys/wait.h malloc.h])

AC_OUTPUT([Makefile
	   popt/Makefile
	   check/Makefile
	   check/config.sh])
