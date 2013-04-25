@ECHO OFF
REM This Source Code Form is subject to the terms of the Mozilla Public
REM License, v. 2.0. If a copy of the MPL was not distributed with this
REM file, You can obtain one at http://mozilla.org/MPL/2.0/.

REM %~p0 expands %0 to a Path only
cd %~p0
.\avmshell %*
ECHO EXITCODE=%ERRORLEVEL%
