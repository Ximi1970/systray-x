;
;	Derived from https://nsis.sourceforge.io/Sample_installation_script_for_an_application
;
 
; -------------------------------
; Start
 
!define Name "SysTray-X"
!define PRODUCT_ID "systray-x@Ximi1970"
!define VERSIONMAJOR 0
!define VERSIONMINOR 9
!define VERSIONBUILD 3
!define VERSION "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
CRCCheck On
 
; We should test if we must use an absolute path 
!include "${NSISDIR}\Contrib\Modern UI\System.nsh"
 
;---------------------------------
;General
 
OutFile  "${Name}-setup32.exe"
ShowInstDetails "nevershow"
ShowUninstDetails "nevershow"
;SetCompressor "bzip2"
 
!define MUI_ICON "..\app\SysTray-X\SysTray-X-app\files\icons\SysTray-X.ico"
!define MUI_UNICON "..\app\SysTray-X\SysTray-X-app\files\icons\SysTray-X.ico"

;--------------------------------
;Folder selection page
 
InstallDir "$PROGRAMFILES\${Name}"
 
;-------------------------------- 
;Modern UI System

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES 

;--------------------------------
;Language
 
!insertmacro MUI_LANGUAGE "English" 

;--------------------------------
;Data
 
LicenseData "..\LICENSE"
 
;--------------------------------
;Macros
 
!include "TextFunc.nsh"
!insertmacro TrimNewLines
!define MyStrStr "!insertmacro MyStrStr"
!macro MyStrStr ResultVar String SubString
  Push `${String}`
  Push `${SubString}`
  !ifdef __UNINSTALL__
    Call un.MyStrStr
  !else
    Call MyStrStr
  !endif
  Pop `${ResultVar}`
!macroend

!define MyStrRep "!insertmacro MyStrRep"
!macro MyStrRep output string old new
  Push `${string}`
  Push `${old}`
  Push `${new}`
  !ifdef __UNINSTALL__
    Call un.MyStrRep
  !else
    Call MyStrRep
  !endif
  Pop ${output}
!macroend

!define ReplaceInFile "!insertmacro ReplaceInFile"
!macro ReplaceInFile SOURCE_FILE SEARCH_TEXT REPLACEMENT
  Push `${SOURCE_FILE}`
  Push `${SEARCH_TEXT}`
  Push `${REPLACEMENT}`
  Call RIF
!macroend
	
;-------------------------------- 
;Installer Sections     

Section "install"
 
  ;Add files
  SetOutPath "$INSTDIR"

  File "..\app\SysTray-X\SysTray-X-app\files\icons\SysTray-X.ico"
  File /r "..\app\dist\win32\*"
  File "..\systray-x@Ximi1970.xpi"

  StrCpy $0 "$INSTDIR\SysTray-X.exe"
  ${MyStrRep} $0 $0 "\" "\\" 
  ${ReplaceInFile} "$INSTDIR\SysTray_X.json" "SYSTRAY_X_PATH" "$0"

  StrCpy $0 "$INSTDIR\SysTray_X.json"
  ${MyStrRep} $0 $0 "\" "\\" 
  WriteRegStr HKLM "SOFTWARE\Mozilla\NativeMessagingHosts\SysTray_X" "" "$0"

  AccessControl::GrantOnFile "$INSTDIR\SysTray_X.json" "(S-1-5-32-545)" "GenericRead"
  Pop $0

  ;
  ;	Find all profiles and install the addon
  ;
  ClearErrors
  FileOpen $0 "$APPDATA\Thunderbird\profiles.ini" r
  IfErrors end
  loop:
    FileRead $0 $1
    IfErrors close
    ${TrimNewLines} "$1" $1
    ${MyStrStr} $2 $1 "Path="
    StrCmp $2 "" loop 0

    ${MyStrRep} $2 $2 "/" "\" 
    SetOutPath "$APPDATA\Thunderbird\$2\extensions"
    File "..\systray-x@Ximi1970.xpi"

    goto loop
  close:
    FileClose $0
  end:
 
  ;
  ; Menu item
  ;
  CreateDirectory "$SMPROGRAMS\${Name}"
  CreateShortCut "$SMPROGRAMS\${Name}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "SysTray-X Uninstall" "$INSTDIR\SysTray-X.ico"

  ;
  ; Write uninstall information to the registry
  ;
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}" "DisplayName" "${Name}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}" "DisplayVersion" "${VERSION}"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}" "VersionMajor" ${VERSIONMAJOR}
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}" "VersionMinor" ${VERSIONMINOR}
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}" "UninstallString" "$INSTDIR\Uninstall.exe"
 
  WriteUninstaller "$INSTDIR\Uninstall.exe"
 
SectionEnd
 
 
;--------------------------------    
;Uninstaller Section  
Section "Uninstall"
  ;
  ; Kill Thunderbird
  ;
  StrCpy $1 "thunderbird.exe"
 
  nsProcess::_FindProcess "$1"
  Pop $R0
  ${If} $R0 = 0
    nsProcess::_KillProcess "$1"
    Pop $R0
 
    Sleep 500
  ${EndIf}

  ;
  ;	Find all profiles and delete the addon
  ;
  ClearErrors
  FileOpen $0 "$APPDATA\Thunderbird\profiles.ini" r
  IfErrors end
  loop:
    FileRead $0 $1
    IfErrors close
    ${TrimNewLines} "$1" $1
    ${MyStrStr} $2 $1 "Path="
    StrCmp $2 "" loop 0

    ${MyStrRep} $2 $2 "/" "\" 

    Delete "$APPDATA\Thunderbird\$2\extensions\systray-x@Ximi1970.xpi"

    goto loop
  close:
    FileClose $0
  end:
 
  ;Delete Files 
  RMDir /r "$INSTDIR\*.*"    
 
  ;Remove the installation directory
  RMDir "$INSTDIR"
 
  ;Delete Uninstaller And Unistall Registry Entries
  DeleteRegKey HKLM "SOFTWARE\Mozilla\NativeMessagingHosts\SysTray_X"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_ID}"  
 
SectionEnd
 
;--------------------------------
;Macros
 
;
;	Modified StrStr function
;
!macro Func_MyStrStr un
  Function ${un}MyStrStr

    /*  After this point:
    ------------------------------------------
    $R0 = SubString (input)
    $R1 = String (input)
    $R2 = SubStringLen (temp)
    $R3 = StrLen (temp)
    $R4 = StartCharPos (temp)
    $R5 = TempStr (temp)*/

    ;Get input from user
    Exch $R0
    Exch
    Exch $R1
    Push $R2
    Push $R3
    Push $R4
    Push $R5

    ;Get "String" and "SubString" length
    StrLen $R2 $R0
    StrLen $R3 $R1

    ;Start "StartCharPos" counter
    StrCpy $R4 0

    ;Loop until "SubString" is found or "String" reaches its end
  loop:
    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R5 $R1 $R2 $R4

    ;Compare "TempStr" with "SubString"
    StrCmp $R5 $R0 done
    ;If not "SubString", this could be "String"'s end
    IntCmp $R4 $R3 done 0 done
    ;If not, continue the loop
    IntOp $R4 $R4 + 1
    Goto loop
  done:

    /*After this point:
    ------------------------------------------
    $R0 = ResultVar (output)*/

    ;Remove part before "SubString" on "String" (if there has one)
    StrCpy $R0 $R1 `` $R4
    ;Remove also "SubString" on "String"
    StrCpy $R0 $R0 `` $R2

    ;Return output to user
    Pop $R5
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
  FunctionEnd
!macroend
!insertmacro Func_MyStrStr ""
!insertmacro Func_MyStrStr "un."

!macro Func_MyStrRep un
  Function ${un}MyStrRep
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
!insertmacro Func_MyStrRep ""
!insertmacro Func_MyStrRep "un."

!macro Func_RIF
  Function RIF
    
    ClearErrors  ; want to be a newborn
    
    Exch $0      ; REPLACEMENT
    Exch
    Exch $1      ; SEARCH_TEXT
    Exch 2
    Exch $2      ; SOURCE_FILE
    
    Push $R0     ; SOURCE_FILE file handle
    Push $R1     ; temporary file handle
    Push $R2     ; unique temporary file name
    Push $R3     ; a line to sar/save
    Push $R4     ; shift puffer
    
    IfFileExists $2 +1 RIF_error      ; knock-knock
    FileOpen $R0 $2 "r"               ; open the door

    GetTempFileName $R2               ; who's new?
    FileOpen $R1 $R2 "w"              ; the escape, please!
 
  RIF_loop:                         ; round'n'round we go
    FileRead $R0 $R3                ; read one line
    IfErrors RIF_leaveloop          ; enough is enough
    RIF_sar:                        ; sar - search and replace
      Push "$R3"                    ; (hair)stack
      Push "$1"                     ; needle
      Push "$0"                     ; blood
      !ifdef __UNINSTALL__
        Call un.MyStrRep
      !else
        Call MyStrRep
      !endif
      StrCpy $R4 "$R3"              ; remember previous state
      Pop $R3                       ; gimme s.th. back in return!
      StrCmp "$R3" "$R4" +1 RIF_sar ; loop, might change again!
    FileWrite $R1 "$R3"             ; save the newbie
    Goto RIF_loop                     ; gimme more
  
  RIF_leaveloop:                    ; over'n'out, Sir!
    FileClose $R1                   ; S'rry, Ma'am - clos'n now
    FileClose $R0                   ; me 2

    Delete "$2.old"                 ; go away, Sire
    Rename "$2" "$2.old"            ; step aside, Ma'am
    Rename "$R2" "$2"               ; hi, baby!

    ClearErrors                     ; now i AM a newborn
    Goto RIF_out                    ; out'n'away
  
  RIF_error:                        ; ups - s.th. went wrong...
    SetErrors                       ; ...so cry, boy!
  
  RIF_out:                          ; your wardrobe?
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
    Pop $R0
    Pop $2
    Pop $0
    Pop $1
 
  FunctionEnd
!macroend
!insertmacro Func_RIF

;eof
