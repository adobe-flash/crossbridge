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
:: set SHELLOPTS=igncr
:: Trigger Cygwin Bash
C:
chdir C:\cygwin\bin
bash --login -i