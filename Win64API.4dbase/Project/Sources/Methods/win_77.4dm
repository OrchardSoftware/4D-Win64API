//%attributes = {}
  // win_77
  //
  // This method tests the sys_KillProcessByName command
  //
  // $1 - True if we are debugging memory issues
  //
  // JEM 3/17/21 WIN-77

C_BOOLEAN:C305($1;$bMemoryTest)

TRACE:C157

C_LONGINT:C283($x;$lRepeatCount;$lErr;$i)
C_TEXT:C284($tStnIn;$tStdOut;$tStdErr;$tPath)

If ($1)
	$lRepeatCount:=500
Else 
	$lRepeatCount:=1
End if 

  // Launch notepad
SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)

  // 32-bit application
$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Hex Editor\\XVI32.exe"

SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
LAUNCH EXTERNAL PROCESS:C811($tPath;$tStnIn;$tStdOut;$tStdErr)

For ($i;1;$lRepeatCount)
	
	  // This should return 0
	$lErr:=sys_IsAppLoaded ("notepad.exe")
	If ($lErr#0)  // Expecting zero
		TRACE:C157
	End if 
	
	  // Graceful kill
	$lErr:=sys_IsAppLoaded ("XVI32.exe")
	If ($lErr#0)  // Expecting zero
		TRACE:C157
	End if 
	
End for 

  // Immediate kill
$lErr:=sys_KillProcessByName ("notepad.exe";1;0)
If ($lErr#0)
	TRACE:C157
End if 

  // Immediate kill
$lErr:=sys_KillProcessByName ("XVI32.exe";1;0)
If ($lErr#0)
	TRACE:C157
End if 

For ($i;1;$lRepeatCount)
	
	  // This should return -1
	$lErr:=sys_IsAppLoaded ("notepad.exe")
	If ($lErr#-1)  // Expecting -1
		TRACE:C157
	End if 
	
	  // Graceful kill
	$lErr:=sys_IsAppLoaded ("XVI32.exe")
	If ($lErr#-1)  // Expecting -1
		TRACE:C157
	End if 
	
End for 