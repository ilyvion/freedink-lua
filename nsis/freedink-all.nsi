; FreeDink all-in-one installer for woe

; Copyright (C) 2009  Sylvain Beucler

; This file is part of GNU FreeDink

; GNU FreeDink is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License as
; published by the Free Software Foundation; either version 3 of the
; License, or (at your option) any later version.

; GNU FreeDink is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; General Public License for more details.

; You should have received a copy of the GNU General Public License
; along with this program.  If not, see
; <http://www.gnu.org/licenses/>.


; This file is inspired by /usr/share/doc/nsis/Examples/Modern UI/*
; See also http://nsis.sourceforge.net/Docs/Modern%20UI%202/Readme.html

; Use better LZMA compression (default zlib), as a solid (tar-like)
; archive for even better compression
SetCompressor /SOLID lzma

; Allow both user and system-wide installs
; Cf. /usr/share/doc/nsis/Docs/MultiUser/Readme.html
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "Software\FreeDink"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME ""
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "Software\FreeDink"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME ""
!define MULTIUSER_INSTALLMODE_INSTDIR "FreeDink"
!include "MultiUser.nsh"

; Modern UI, with macro-based scripting
!include "MUI2.nsh"

; Association .dmod files with DFArc
!include "registerExtension.nsh"

; Precise uninstall
!define INSTDIR_REG_ROOT "HKLM"
!define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\FreeDink"
!include "AdvUninstLog.nsh"
!insertmacro INTERACTIVE_UNINSTALL

Name "GNU FreeDink"
OutFile "FreeDink-installer.exe"


; Default install dir
; Cf. MULTIUSER above
;InstallDir "$PROGRAMFILES\FreeDink"
; Use existing install dir if available
;InstallDirRegKey HKCU "Software\FreeDink" ""


; Recommended level is 'user', since we're not writing in system
; folders
RequestExecutionLevel user

; User-selected subfolder in the woe Start menu
Var StartMenuFolder


;----------
; Graphics
;----------
!define MUI_ICON ../src/media/freedink.ico
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP header.bmp
!define MUI_WELCOMEFINISHPAGE_BITMAP welcome.bmp


;---------------------
; The installer pages
;---------------------
; The welcome page requires Syscall support, which means v>=2.42 for
; Unix
!insertmacro MUI_PAGE_WELCOME

; The GNU GPL is Not A Contract - don't coerce users into accepting it
!define MUI_LICENSEPAGE_BUTTON $(^NextBtn)
!define MUI_LICENSEPAGE_TEXT_BOTTOM $(^ClickNext)
!insertmacro MUI_PAGE_LICENSE "dfarc/dfarc-COPYING.txt"

!insertmacro MULTIUSER_PAGE_INSTALLMODE

!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_COMPONENTS

!insertmacro MUI_PAGE_INSTFILES

; Not interesting, and hide the installation page even if the users
; wanted to review installed files
;!insertmacro MUI_PAGE_FINISH

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "SHELL_CONTEXT"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\FreeDink" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder


;-----------------------
; The uninstaller pages
;-----------------------  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

  
;---------------
; The languages
;---------------
; Add all languages that FreeDink is (partly) translated in:

!insertmacro MUI_LANGUAGE "English" ; first is fallback
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Macedonian"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Norwegian"

; Tell the installer to store the language files first (so the
; installer doesn't uncompress everything just to get them)
!insertmacro MUI_RESERVEFILE_LANGDLL


;----------------
; The components
;----------------
; freedink-engine
Section "freedink-engine" SecFreeDinkEngine
  SetOutPath "$INSTDIR"
  !insertmacro UNINSTALL.LOG_OPEN_INSTALL
  File /r freedink/*.*
  !insertmacro UNINSTALL.LOG_CLOSE_INSTALL

  ; Start menu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\FreeDink.lnk" "$INSTDIR\freedink.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ; Desktop icon
  CreateShortCut "$DESKTOP\FreeDink.lnk" "$INSTDIR\freedink.exe" ""
  CreateShortCut "$DESKTOP\FreeDink.lnk" "$INSTDIR\freedinkedit.exe" ""
SectionEnd

; freedink-dfarc
Section "freedink-dfarc" SecDFArc
  SetOutPath "$INSTDIR"
  !insertmacro UNINSTALL.LOG_OPEN_INSTALL
  File /r dfarc/*.*
  !insertmacro UNINSTALL.LOG_CLOSE_INSTALL

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\DFArc.lnk" "$INSTDIR\dfarc3.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ; Desktop icon
  CreateShortCut "$DESKTOP\DFArc3.lnk" "$INSTDIR\dfarc3.exe" ""

  ; .dmod association
  ${registerExtension} "$INSTDIR\dfarc3.exe" ".dmod" "Dink MODule"
SectionEnd

; freedink-data
Section "freedink-data" SecFreeDinkData
  SetOutPath "$INSTDIR"
  !insertmacro UNINSTALL.LOG_OPEN_INSTALL
  File /r dink/*.*
  ; Single-file for quick testing:
  ;File /r dink/dink/l10n/nl/LC_MESSAGES/dink.mo
  !insertmacro UNINSTALL.LOG_CLOSE_INSTALL
SectionEnd

; Common:
; Put is last, apparently putting the installer first is not good for
; solid archives
Section
  ;Store installation folder
  WriteRegStr SHELL_CONTEXT "Software\FreeDink" "" $INSTDIR
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Start menu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

; Descriptions:
LangString DESC_SecFreeDinkEngine ${LANG_ENGLISH} "Game engine"
LangString DESC_SecDFArc          ${LANG_ENGLISH} "Front-end to run and manage D-Mods"
LangString DESC_SecFreeDinkData   ${LANG_ENGLISH} "The Dink Smallwood game files"

LangString DESC_SecFreeDinkEngine ${LANG_FRENCH} "Moteur de jeu"
LangString DESC_SecDFArc          ${LANG_FRENCH} "Interface pour lancer et gérer les D-Mods"
LangString DESC_SecFreeDinkData   ${LANG_FRENCH} "Données de jeu Dink Smallwood"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFreeDinkEngine} $(DESC_SecFreeDinkEngine)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDFArc}          $(DESC_SecDFArc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFreeDinkData}   $(DESC_SecFreeDinkData)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


;-------------
; Uninstaller
;-------------
Section "Uninstall"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  DeleteRegKey /ifempty SHELL_CONTEXT "Software\FreeDink"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\DFArc.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\FreeDink.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  Delete "$DESKTOP\DFArc3.lnk"
  Delete "$DESKTOP\FreeDink.lnk"
  Delete "$DESKTOP\FreeDinkedit.lnk"

  ${unregisterExtension} ".dmod" "Dink MODule"

  !insertmacro UNINSTALL.LOG_UNINSTALL "$INSTDIR"
  !insertmacro UNINSTALL.LOG_END_UNINSTALL
SectionEnd


; Per-user|system-wide install support
Function .onInit
  !insertmacro MULTIUSER_INIT
  !insertmacro UNINSTALL.LOG_PREPARE_INSTALL
FunctionEnd

Function .onInstSuccess
  !insertmacro UNINSTALL.LOG_UPDATE_INSTALL
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
  !insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL
FunctionEnd
