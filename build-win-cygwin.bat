::
:: =BEGIN MIT LICENSE
:: 
:: The MIT License (MIT)
::
:: Copyright (c) 2014 The CrossBridge Team
:: 
:: Permission is hereby granted, free of charge, to any person obtaining a copy
:: of this software and associated documentation files (the "Software"), to deal
:: in the Software without restriction, including without limitation the rights
:: to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
:: copies of the Software, and to permit persons to whom the Software is
:: furnished to do so, subject to the following conditions:
:: 
:: The above copyright notice and this permission notice shall be included in
:: all copies or substantial portions of the Software.
:: 
:: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
:: IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
:: FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
:: AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
:: LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
:: OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
:: THE SOFTWARE.
:: 
:: =END MIT LICENSE
::

:: Purpose: Windows (Cygwin) build helper
:: Author: Andras Csizmadia
:: Date: 04.05.14.
:: Note: The script uses fixed location for JAVA_HOME and CYGWIN, edit if necessary.

:: Echo off and begin localisation of Environment Variables
@ECHO OFF & SETLOCAL

:: Prepare the Command Processor
VERIFY errors 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Warning: Unable to enable extensions.
SETLOCAL ENABLEDELAYEDEXPANSION

:: Save base directory
PUSHD %CD%

:: Set title
TITLE %~n0

:: STARTUP
SET FLASCC_ROOT=%CD:\=/%
set JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk
set PATH=%JAVA_HOME%/bin;C:\cygwin\bin;C:\cygwin\usr\X11R6\bin
set JAVA_HOME=
set JAVA_OPTS=
set PYTHONHOME=
set PYTHONPATH=
set MINGW_HOME=
set MSYS_HOME=
set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor
C:
chdir C:\cygwin\bin
bash --login %FLASCC_ROOT%/build-win-cygwin.sh
GOTO :EXIT

:: SHUTDOWN
:EXIT
POPD
ENDLOCAL
PAUSE
EXIT /B 0 
GOTO :EOF