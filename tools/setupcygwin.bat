@IF EXIST setup.exe (
ECHO running setup.exe
) ELSE (
ECHO Couldn't find setup.exe in the current directory. Please download it from http://www.cygwin.com
pause
EXIT 0
)

setup.exe --no-shortcuts --disable-buggy-antivirus --quiet-mode --packages make,libuuid1,python,cmake,gcc4-g++,rsync