Install
=======

# OSX

## Build a Mac only SDK.

> "make" 

## Build both Mac and Windows (Cross-Compiled) SDK and package them as DMG and ZIP.

> "make; make win; make deliverables" 

# Windows

## Build a Windows only SDK.

> "make all_win"

### Notes

#### Edit/duplicate cygwin.bat with the following options

* Shortened Java home

> set JAVA_HOME=/cygdrive/c/Progra~1/Java/jdk

* Igncr in shell options

> set SHELLOPTS=braceexpand:emacs:hashall:histexpand:history:igncr:interactive-comments:monitor

# Linux

## Build a Linux only SDK.

> "make"