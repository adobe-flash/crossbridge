@echo off
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

:: Purpose: CrossBridge Cygwin Launch Helper Script 
:: Author: Andras Csizmadia

:: Echo off and begin localisation of Environment Variables
@ECHO OFF & SETLOCAL

:: Prepare the Command Processor
VERIFY errors 2>nul
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 ECHO Warning: Unable to enable extensions.
SETLOCAL ENABLEDELAYEDEXPANSION

SET LAUNCH_DIR=%~dp0

:: Check for Cygwin
IF NOT EXIST %LAUNCH_DIR%\cygwin IF NOT DEFINED BUILD_NUMBER GOTO NOCYGWIN

:: Bootstrap Cygwin
:RUNCYGWIN

:: Disable 3rd party C/C++ compiler variables
set MINGW_HOME=
set MSYS_HOME=
set MSVC_HOME=

:: Convert some paths to Unix style
set JAVA_HOME=%JAVA_HOME:\=/%
set AIR_HOME=%AIR_HOME:\=/%
set FLEX_HOME=%FLEX_HOME:\=/%
set FLASH_PLAYER_EXE=%FLASH_PLAYER_EXE:\=/%
set FLASCC_ROOT=%LAUNCH_DIR:\=/%

:: Setup basic path
set PATH=%FLASCC_ROOT%/sdk/usr/bin;%JAVA_HOME%/bin

:: Trigger Cygwin Bash
C:

:: !!!Important!!! For custom Cygwin installations edit the location!
chdir %LAUNCH_DIR%\cygwin\bin

:: Cygwin with automation support 
if [%1] == [] (
bash --login -i
) else (
bash --login %*
)

GOTO EXIT

:: Install Cygwin and additional packages using CLI
:NOCYGWIN
md C:\cygwin\tmp && setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %LAUNCH_DIR%\cygwin --site http://cygwin.mirror.constant.com && setup-x86 --arch x86 --quiet-mode --no-admin --no-startmenu --no-desktop --no-shortcuts --root %CD%\cygwin --site http://cygwin.mirror.constant.com --packages libuuid1,libuuid-devel && run
GOTO EXIT

:: Exit hook
:EXIT