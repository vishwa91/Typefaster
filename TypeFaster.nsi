; TypeFaster.nsi
;
; This script is based on example1.nsi, but adds uninstall support
; and (optionally) start menu shortcuts.
;
; It will install notepad.exe into a directory that the user selects,
;
!include WinMessages.nsh

Var hwnd ; Window handle of the custom page
Var filehandle
Var multi
Var teacher
Var teacherpass


; The name of the installer
Name "TypeFaster Typing Tutor"

; The file to write
OutFile "..\TypeFaster-v0.4.1-install.exe"
;Icon "test3.ico"
LicenseText "The GPL - note that this program is freely distributable under these conditions"
LicenseData "gpl.txt"

; The default installation directory
InstallDir $PROGRAMFILES\TypeFaster
; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\NSIS_TypeFaster "Install_Dir"

; The text to prompt the user to enter a directory
ComponentText "This will install the TypeFaster Typing Tutor on your computer. Select which optional things you want installed."
; The text to prompt the user to enter a directory
DirText "Choose a directory to install into:"

Page license
Page components
Page directory
Page custom createCustom leaveCustom ": Multi or single user"

Function .onInit

  InitPluginsDir
  File /oname=$PLUGINSDIR\custom.ini custom.ini

FunctionEnd



Function createCustom
  InstallOptions::initDialog /NOUNLOAD "$PLUGINSDIR\custom.ini"
  ; In this mode InstallOptions returns the window handle so we can use it
  Pop $hwnd
  ; Now show the dialog and wait for it to finish
  InstallOptions::show
  ; Finally fetch the InstallOptions status value (we don't care what it is though)
  Pop $0

FunctionEnd

Function leaveCustom

; At this point the user has either pressed Next or one of our custom buttons
  ; We find out which by reading from the INI file
  ReadINIStr $0 "$PLUGINSDIR\custom.ini" "Settings" "State"
  StrCmp $0 0 validate
  StrCmp $0 1 single
  StrCmp $0 2 multi
  StrCmp $0 3 teacher
  Abort ; Return to the page
  
single:
  GetDlgItem $1 $hwnd 1202 ; PathRequest control (1200 + field 4 - 1)
  EnableWindow $1 0
  GetDlgItem $1 $hwnd 1203 ; PathRequest control (1200 + field 4 - 1)
  EnableWindow $1 0
  GetDlgItem $1 $hwnd 1204 ; ... button (the following control)
  EnableWindow $1 0

  WriteINIStr "$PLUGINSDIR\custom.ini" "Field 3" "Flags" "DISABLED|NOTIFY"
  WriteINIStr "$PLUGINSDIR\custom.ini" "Field 4" "Flags" "DISABLED"
  WriteINIStr "$PLUGINSDIR\custom.ini" "Field 5" "Flags" "DISABLED"
  
  Abort
multi:
  GetDlgItem $1 $hwnd 1202 ; PathRequest control (1200 + field 4 - 1)
  EnableWindow $1 1       ;enable tick box
  
  WriteINIStr "$PLUGINSDIR\custom.ini" "Field 3" "Flags" "NOTIFY" ;enable it in file

  ReadINIStr $0 "$PLUGINSDIR\custom.ini" "Field 3" "State"
  GetDlgItem $1 $hwnd 1203 ; PathRequest control (1200 + field 4 - 1)
  EnableWindow $1 $0
  GetDlgItem $1 $hwnd 1204 ; ... button (the following control)
  EnableWindow $1 $0
  
  StrCmp $0 0 0 +4 ;if teachers is 0 then disable passwords in file else 'enable' them
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 4" "Flags" "DISABLED"
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 5" "Flags" "DISABLED"
    Goto +3
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 4" "Flags" ""
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 5" "Flags" ""
  Abort
teacher:
  ReadINIStr $0 "$PLUGINSDIR\custom.ini" "Field 3" "State"
  GetDlgItem $1 $hwnd 1203 ; PathRequest control (1200 + field 4 - 1)
  EnableWindow $1 $0
  GetDlgItem $1 $hwnd 1204 ; ... button (the following control)
  EnableWindow $1 $0

  StrCmp $0 0 0 +4 ;if teachers is 0 then disable passwords in file else 'enable' them
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 4" "Flags" "DISABLED"
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 5" "Flags" "DISABLED"
    Goto +3
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 4" "Flags" ""
    WriteINIStr "$PLUGINSDIR\custom.ini" "Field 5" "Flags" ""
  Abort
validate:
  StrCpy $multi 0
  StrCpy $teacher 0
  StrCpy $teacherpass 0
  
  ReadINIStr $1 "$PLUGINSDIR\custom.ini" "Field 1" "State"   ; now $1 has single user value
  StrCmp $1 1 done
  
  StrCpy $multi 1
  ReadINIStr $1 "$PLUGINSDIR\custom.ini" "Field 3" "State" ; is teacher support checked
  StrCmp $1 0 done
  
  StrCpy $teacher 1
  ReadINIStr $2 "$PLUGINSDIR\custom.ini" "Field 4" "State"
  ReadINIStr $3 "$PLUGINSDIR\custom.ini" "Field 5" "State" ;get the passwords
  StrCmp $2 $3 0 notsame
  StrLen $3 $2
  IntCmp $3 2 tooshort tooshort 0
  StrCpy $teacherpass $2
  goto done
tooshort:
   MessageBox MB_ICONEXCLAMATION|MB_OK "Password must be greater than 2 characters"
   Abort
notsame:
  MessageBox MB_ICONEXCLAMATION|MB_OK "Passwords entered are not the same"
  Abort
done:
FunctionEnd

Page instfiles


; The stuff to install
Section "TypeFaster Typing Tutor (required)"
  ; Set output path to the installation directory.
SetOutPath $INSTDIR
  File /r "C:\Documents and Settings\dleroux\Desktop\0.4.1\*.*"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_TypeFaster "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TypeFaster" "DisplayName" "TypeFaster Typing Tutor"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TypeFaster" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
  
  StrCmp $multi 1 0 notmulti
  File "C:\users.txt" ; the presence of a users.txt file indicates a multi user system
  StrCmp $teacher 1 0 notteacher
  File "C:\teacherhash.txt"
  StrCpy $4 $teacherpass
  exdll::myFunction ; $5 now contains the hash
  FileOpen $filehandle "teacherhash.txt" w ;current folder is set using setoutpath
  FileWrite $filehandle $5
  FileClose $filehandle
  SetFileAttributes "teacherhash.txt" HIDDEN|READONLY
notteacher:
notmulti:
SectionEnd

; optional section
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\TypeFaster"
  CreateShortCut "$SMPROGRAMS\TypeFaster\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\TypeFaster\TypeFaster.lnk" "$INSTDIR\TypeFaster.exe" "" "$INSTDIR\TypeFaster.exe" 0
SectionEnd

; uninstall stuff

UninstallText "This will uninstall TypeFaster. Note that the whole directory will just be removed, so press Cancel if you have your own files there."

; special uninstall section.
Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TypeFaster"
  DeleteRegKey HKLM SOFTWARE\NSIS_TypeFaster
  ; remove files
  Delete "$SMPROGRAMS\TypeFaster\*.*"
  ; remove directories used.
  RMDir "$SMPROGRAMS\TypeFaster"
  
  RMDir /r $INSTDIR

SectionEnd


