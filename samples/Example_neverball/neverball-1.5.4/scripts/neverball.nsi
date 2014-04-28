#------------------------------------------------------------------------------

!include "LogicLib.nsh"

#------------------------------------------------------------------------------

!ifndef VERSION
!define VERSION "unknown"
!endif

!ifndef OUTFILE
!define OUTFILE "../neverball-${VERSION}-setup.exe"
!endif

!define UNINSTALL_REG_ROOT \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\Neverball"

#------------------------------------------------------------------------------

OutFile ${OUTFILE}
Name "Neverball ${VERSION}"
LicenseData "COPYING.txt"
InstallDir "$PROGRAMFILES\Neverball"

XPStyle on
SetCompressor /solid lzma
RequestExecutionLevel user

#------------------------------------------------------------------------------

Page license
Page components
Page directory
Page instfiles

Function .onInit
    Call IsUserAdmin
    Pop $R0

    ${If} $R0 == "true"
        SetShellVarContext all
    ${EndIf}
FunctionEnd

Section "Neverball/Neverputt"
    SectionIn RO

    SetOutPath "$INSTDIR"

    File *.txt doc\*.txt
    File /r /x .svn /x *.map /x *.obj data
    File /r locale

    File neverball.exe neverputt.exe *.dll

    File /oname=data\icon\neverball.ico dist\ico\neverball.ico
    File /oname=data\icon\neverputt.ico dist\ico\neverputt.ico

    CreateDirectory "$INSTDIR\bin"

    SetOutPath "$INSTDIR\bin"

    File scripts\neverball.bat
    File scripts\neverputt.bat
    File contrib\curve.exe

    # http://nsis.sourceforge.net/\
    # Add_uninstall_information_to_Add/Remove_Programs

    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "DisplayName" "Neverball ${VERSION}"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "DisplayVersion" "${VERSION}"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "DisplayIcon" "$INSTDIR\data\icon\neverball.ico"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "URLInfoAbout" "http://www.neverball.org/"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "URLUpdateInfo" "http://www.neverball.org/"
    WriteRegStr SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "HelpLink" "http://www.nevercorner.net/"
    WriteRegDWORD SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "NoModify" 1
    WriteRegDWORD SHELL_CONTEXT ${UNINSTALL_REG_ROOT} \
        "NoRepair" 1

    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Mapping tools (compiler, maps, ...)"
    SetOutPath "$INSTDIR"

    File mapc.exe
    File /oname=bin\mapc.bat scripts\mapc.bat

    SetOutPath "$INSTDIR\data"

    File /r /x ".svn" data\*.map
    File /r /x ".svn" data\*.obj
SectionEnd

SectionGroup "Shortcuts"
    Section "In Start menu"
        # Reset to get a proper working directory
        SetOutPath "$INSTDIR"
    
        CreateDirectory "$SMPROGRAMS\Neverball"
    
        CreateShortcut \
            "$SMPROGRAMS\Neverball\Neverball.lnk" \
            "$INSTDIR\neverball.exe" ""       \
            "$INSTDIR\data\icon\neverball.ico"
    
        CreateShortcut \
            "$SMPROGRAMS\Neverball\Neverputt.lnk" \
            "$INSTDIR\neverputt.exe" ""       \
            "$INSTDIR\data\icon\neverputt.ico"
    SectionEnd

    Section "On desktop"
        # Reset to get a proper working directory
        SetOutPath "$INSTDIR"
    
        CreateShortcut \
            "$DESKTOP\Neverball.lnk"          \
            "$INSTDIR\neverball.exe" ""       \
            "$INSTDIR\data\icon\neverball.ico"
    
        CreateShortcut \
            "$DESKTOP\Neverputt.lnk"          \
            "$INSTDIR\neverputt.exe" ""       \
            "$INSTDIR\data\icon\neverputt.ico"
    SectionEnd
SectionGroupEnd

#------------------------------------------------------------------------------

UninstPage uninstConfirm
UninstPage instfiles

Function un.onInit
    Call un.IsUserAdmin
    Pop $R0

    ${If} $R0 == "true"
        SetShellVarContext all
    ${EndIf}
FunctionEnd

Section "Uninstall"
    Delete "$SMPROGRAMS\Neverball\Neverball.lnk"
    Delete "$SMPROGRAMS\Neverball\Neverputt.lnk"
    RMDir  "$SMPROGRAMS\Neverball"
    Delete "$DESKTOP\Neverball.lnk"
    Delete "$DESKTOP\Neverputt.lnk"

    DeleteRegKey SHELL_CONTEXT ${UNINSTALL_REG_ROOT}

    # FIXME:  unsafe if the directory contains other-than-installed stuff
    RMDir /r $INSTDIR
SectionEnd

#------------------------------------------------------------------------------

# URL:    http://nsis.sourceforge.net/IsUserAdmin
# Author: Lilla (lilla@earthlink.net) 2003-06-13

!macro IsUserAdmin un
Function ${un}IsUserAdmin
    Push $R0
    Push $R1
    Push $R2
 
    ClearErrors
    UserInfo::GetName
    IfErrors Win9x
    Pop $R1
    UserInfo::GetAccountType
    Pop $R2
 
    StrCmp $R2 "Admin" 0 Continue
    StrCpy $R0 "true"
    Goto Done
 
    Continue:
        StrCmp $R2 "" Win9x
        StrCpy $R0 "false"
        Goto Done
 
    Win9x:
        StrCpy $R0 "true"
 
    Done:

    Pop $R2
    Pop $R1
    Exch $R0
FunctionEnd
!macroend

!insertmacro IsUserAdmin ""
!insertmacro IsUserAdmin "un."

#------------------------------------------------------------------------------

# vim:sts=4:sw=4:et:
