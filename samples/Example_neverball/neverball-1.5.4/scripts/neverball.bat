@echo off

setlocal
    path %~dp0\..;%PATH%

    if (%NEVERBALL_BINARY%) == () (
        set NEVERBALL_BINARY=neverball.exe
    )
    if (%NEVERBALL_DATA%) == () (
        set NEVERBALL_DATA=%~dp0\..\data
    )
    if (%NEVERBALL_LOCALE%) == () (
        set NEVERBALL_LOCALE=%~dp0\..\locale
    )

    del /f "%~dp0\..\stdout.txt" 2>NUL
    del /f "%~dp0\..\stderr.txt" 2>NUL

    "%NEVERBALL_BINARY%" %*

    type "%~dp0\..\stdout.txt"      2>NUL
    type "%~dp0\..\stderr.txt" 1>&2 2>NUL
endlocal

