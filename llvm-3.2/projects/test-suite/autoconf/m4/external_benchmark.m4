dnl This macro allows an external benchmark to be added
dnl into the configure.ac script more easily. The
dnl arguments are:
dnl   $1 short name of benchmark (e.g. spec95)
dnl   $2 full path to benchmark source
dnl   $3 a file or directory name the benchmark must contain or
dnl      blank to skip that check
AC_DEFUN([EXTERNAL_BENCHMARK],[
m4_define([allcapsname],translit($1,a-z,A-Z))
m4_define([thepath],[$2])
m4_define([checkdir],[$3])
AC_MSG_CHECKING([for $1 benchmark sources])
if test -d "thepath()" ; then
  if test -n "checkdir()" ; then
    if test -e "thepath()/checkdir()" ; then
      AC_SUBST(allcapsname()[_ROOT],thepath())
      AC_SUBST([USE_]allcapsname(),[USE_]allcapsname()=1)
      checkresult="yes, found in thepath()"  
    else
      checkresult="no"
    fi
  else
    AC_SUBST(allcapsname()[_ROOT],thepath())
    AC_SUBST([USE_]allcapsname(),[USE_]allcapsname()=1)
    checkresult="yes, found in thepath()"  
  fi
else
  checkresult="no"
fi
if test "$checkresult" = "no" ; then
  AC_SUBST(allcapsname()[_ROOT],[])
  AC_SUBST([USE_]allcapsname(),[])
  checkresult="no, not found in thepath()"
fi
AC_MSG_RESULT($checkresult)
m4_undefine([allcapsname])
m4_undefine([thepath])
m4_undefine([checkdir])
])

