//%attributes = {}
  // win_80
  //
  // This method tests the sys_GetRegLongint command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-80

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lErr)
C_LONGINT:C283($lCurrentVersion)

$lErr:=sys_GetRegLongint (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";"CurrentMajorVersionNumber";$lCurrentVersion)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($lCurrentVersion#10)
		TRACE:C157  // We should be on Windows 10; if we are on newly supported version of Windows update the condition
	End if 
End if 

  // Read the value from the 32-bit registy (it should be the same due to registry reflection: https://docs.microsoft.com/en-us/troubleshoot/windows-server/performance/registry-changes-in-x64-based-windows)
$lErr:=sys_GetRegLongint (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";"CurrentMajorVersionNumber";$lCurrentVersion;1)
If ($lErr=0)
	TRACE:C157  // There is an error
Else 
	If ($lCurrentVersion#10)
		TRACE:C157  // We should be on Windows 10; if we are on newly supported version of Windows update the condition
	End if 
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		ARRAY TEXT:C222($atKeys;0)
		$lErr:=sys_GetRegLongint (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";"CurrentMajorVersionNumber";$lCurrentVersion)
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 
