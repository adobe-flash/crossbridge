@echo off
:: Java Home short format is important to build the SDK
set JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk
:: Only include Java in inherited path
set PATH=%JAVA_HOME%/bin;
:: Disable some variables
set JAVA_HOME=
set PYTHONHOME=
set PYTHONPATH=
set MINGW_HOME=
set MSYS_HOME=
:: Set CrossBridge Home
set FLASCC_ROOT=%CD%
:: Line endings compatibility fix (Can make errors in scripts)
set SHELLOPTS=igncr
:: Trigger Cygwin Bash
C:
chdir C:\cygwin\bin
bash --login -i