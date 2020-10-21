//%attributes = {}
  // win_78
  //
  // This method tests the sys_GetRegText command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/20/20 WIN-78

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lErr)
C_TEXT:C284($tProfileListDefault;$tCurrentVersion)

  // *** Registry type REG_EXPAND_SZ
$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList";"Default";$tProfileListDefault)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($tProfileListDefault="")
		TRACE:C157  // We should have something here
	End if 
End if 

  // Read the value from the 32-bit registy (it should be the same due to registry reflection: https://docs.microsoft.com/en-us/troubleshoot/windows-server/performance/registry-changes-in-x64-based-windows)
$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList";"Default";$tProfileListDefault;1)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($tProfileListDefault="")
		TRACE:C157  // We should have something here
	End if 
End if 

  // *** Registry type REG_SZ
$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";"CurrentVersion";$tCurrentVersion)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($tCurrentVersion="")
		TRACE:C157  // We should have something here
	End if 
End if 

  // 32-bit
$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";"CurrentVersion";$tCurrentVersion;1)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($tCurrentVersion="")
		TRACE:C157  // We should have something here
	End if 
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		$tProfileListDefault:=""
		
		$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList";"Default";$tProfileListDefault)
		If ($lErr=0)
			TRACE:C157  // There is an error
		Else 
			If ($tProfileListDefault="")
				TRACE:C157  // We should have something here
			End if 
		End if 
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 
