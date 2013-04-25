@echo off
pushd "%~dp0"
set FLASCC_ROOT=%~dp0
set path=%path%;%~dp0\cygwin\bin;%~dp0\cygwin\usr\X11R6\bin

if not exist cygwin\home\flasccuser\setup.txt (
	echo cygwin setup > cygwin\home\flasccuser\setup.txt
	echo First run setup, please wait...
	cygwin\bin\mkpasswd.exe -l -c > cygwin\etc\passwd
	cygwin\bin\mkgroup.exe -l -c > cygwin\etc\group
	cygwin\bin\chmod.exe -R ug+rw cygwin
)

if [%1] == [] (
cygwin\bin\bash --login -i
) else (
:: to support automation
cygwin\bin\bash --login %*
)
popd