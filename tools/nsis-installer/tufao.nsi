!define PRODUCT_NAME "Tufão"
!define PRODUCT_VERSION "0.3.1"
!define PRODUCT_WEB_SITE "http://tufao.googlecode.com/"

!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

!include "MUI.nsh"
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
!include "FileFunc.nsh"

; begin of http://nsis.sourceforge.net/StrRep
!define StrRep "!insertmacro StrRep"
!macro StrRep output string old new
    Push "${string}"
    Push "${old}"
    Push "${new}"
    !ifdef __UNINSTALL__
        Call un.StrRep
    !else
        Call StrRep
    !endif
    Pop ${output}
!macroend

!macro Func_StrRep un
    Function ${un}StrRep
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9

        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
            Goto loop
        done:

        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRep ""
!insertmacro Func_StrRep "un."
; end of http://nsis.sourceforge.net/StrRep

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Installer pages
!insertmacro MUI_PAGE_WELCOME
Page custom qtPage qtPageLeave
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

Var Dialog
Var Label
Var DirRequest
Var BrowseButton

Var QtDir

Function qtPage
  !insertmacro MUI_HEADER_TEXT "Qt's dir" "Specify where is the qtdir"

  nsDialogs::Create 1018
  Pop $Dialog
  
  ${If} $Dialog == error
    Abort
  ${EndIf}
  
  ${NSD_CreateLabel} 0 0 100% 12u "Please type QTDIR (eg. C:\Qt\4.8.1)"
  Pop $Label

  ${NSD_CreateDirRequest} 0 13u 240u 12u "C:\Qt\4.8.1"
  Pop $DirRequest
  
  ${NSD_CreateBrowseButton} 241u 13u 59u 12u "Browse"
  Pop $BrowseButton
  ${NSD_OnClick} $BrowseButton qtPageBrowseButtonClick
  
  nsDialogs::Show
FunctionEnd

Function qtPageLeave
  ${NSD_GetText} $DirRequest $QtDir
FunctionEnd

Function qtPageBrowseButtonClick
  ${NSD_GetText} $DirRequest $0
  nsDialogs::SelectFolderDialog "Select the QTDIR" $0
  Pop $1
  ${If} $1 != error
    ${NSD_SetText} $DirRequest $1
  ${EndIf}
FunctionEnd

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Installer.exe"
InstallDir "$PROGRAMFILES\Tufao"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Tufão" SEC_MAIN
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File /r "sections\main\*"
    
  CreateDirectory "$SMPROGRAMS\Tufão"

  Call createPrfFile
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "PrfFile" "$QtDir\mkspecs\features\tufao.prf"

SectionEnd

Section "Tufão routes editor" SEC_ROUTES
  SetOutPath "$INSTDIR"
  File /r "sections\routes-editor"
  
  CreateShortCut "$SMPROGRAMS\Tufão\Routes editor.lnk" "$INSTDIR\routes-editor\routes-editor.exe"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Tufão\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Tufão\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\AppMainExe.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_MAIN} "Tufão headers, libraries and documentation"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_ROUTES} "Application to change the routes file"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function createPrfFile
  FileOpen $1 "$QtDir\mkspecs\features\tufao.prf" w
  Pop $0
  ${If} $0 == error
    Abort
  ${EndIf}

  FileWrite $1 "QT += network$\n"
  FileWrite $1 "$\n"
  FileWrite $1 "DEFINES += TUFAO_VERSION_MAJOR=0$\n"
  FileWrite $1 "$\n"
  FileWrite $1 "release {$\n"
  FileWrite $1 "    LIBS += -ltufao$\n"
  FileWrite $1 "} else {$\n"
  FileWrite $1 "    LIBS += -ltufao_d$\n"
  FileWrite $1 "}$\n"
  FileWrite $1 "$\n"
  
  ${StrRep} $2 $INSTDIR '\' '/'
  FileWrite $1 "LIBS += -L$\"$2/lib$\"$\n"
  FileWrite $1 "INCLUDEPATH += $\"$2/include$\"$\n"
  
  FileClose $1
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successful removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to uninstall $(^Name)?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "PrfFile"
  Delete $0

  RMDir /r "$SMPROGRAMS\Tufão"
  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd