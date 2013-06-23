# Process this file with autoconf to produce a configure script.
AC_INIT([lambda], [0.1.3], [usdesign@earthlink.net])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE
##AC_CONFIG_SRCDIR([lambda.cc])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getcwd strchr strerror])

AC_CONFIG_FILES([Makefile docs/Makefile])
AC_OUTPUT
