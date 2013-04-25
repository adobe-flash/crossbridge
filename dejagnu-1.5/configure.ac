dnl Process this file with autoconf to produce a configure script.

dnl Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
dnl 2002, 2003, 2005, 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
dnl
dnl This file is part of DejaGnu.
dnl
dnl DejaGnu is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl DejaGnu is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with DejaGnu; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

AC_PREREQ(2.50)
AC_INIT([GNU DejaGnu], 1.5, [bug-dejagnu@gnu.org])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_EXEEXT

dnl Search for the Docbook utilities.
AC_PATH_PROG([DOCBOOK2HTML], [docbook2html], [false])
AC_PATH_PROG([DOCBOOK2RTF], [docbook2rtf], [false])
AC_PATH_PROG([DOCBOOK2PDF], [docbook2pdf], [false])
AC_PATH_PROG([DOCBOOK2PS], [docbook2ps], [false])
AC_PATH_PROG([DOCBOOK2TEXI], [docbook2x-texi], [false])

dnl Search for expect.
AC_PATH_PROG([EXPECT], [expect])
if test -z $ac_cv_path_EXPECT ; then
   AC_MSG_ERROR([unable to locate expect])
fi

dnl Check the Tcl version is >= 8.3.
AC_MSG_CHECKING([Tcl version 8.3 or greater])
AC_CACHE_VAL(ac_cv_dg_tcl_modern,[
  ac_cv_dg_tcl_modern=`$EXPECT << EOF
if @<:@ expr \\${tcl_version} >= 8.3  @:>@ then { puts yes } else { puts no }
EOF`
])
if test x${ac_cv_dg_tcl_modern} = xyes ; then
  AC_MSG_RESULT(${ac_cv_dg_tcl_modern})
else
  AC_MSG_ERROR([Tcl 8.3 or greater is required])
fi

AC_CONFIG_SUBDIRS([example/calc example/hello example/mathhelper])
AC_OUTPUT([Makefile])
