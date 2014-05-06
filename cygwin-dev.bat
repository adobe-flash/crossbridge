@echo off

::set JAVA_HOME=/cygdrive/c/Program Files/Java/jdk
set JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk
set PATH=%JAVA_HOME%/bin;
set JAVA_HOME=
set JAVA_OPTS=
set PYTHONHOME=
set PYTHONPATH=
set MINGW_HOME=
set MSYS_HOME=
set FLASCC_ROOT=%CD%
set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor

C:
chdir C:\cygwin\bin

bash --login -i