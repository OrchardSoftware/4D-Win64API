//%attributes = {}
  // win_96
  //
  // This method tests the sys_KillProcessByName command
  //
  // $1 - True if we are debugging memory issues
  //
  // JEM 3/16/21 WIN-96

C_BOOLEAN:C305($1;$bMemoryTest)

TRACE:C157

C_LONGINT:C283($x;$lRepeatCount)
C_TEXT:C284($tStnIn;$tStdOut;$tStdErr;$tPath)

If ($1)
	$lRepeatCount:=500
Else 
	$lRepeatCount:=1
End if 
For ($i;1;$lRepeatCount)
	
	  // Graceful kill of single occurance
	
	  // Launch 2 calls to notepad
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	  // This should only kill one
	$lErr:=sys_KillProcessByName ("notepad.exe";1;1)
	If ($lErr#0)
		TRACE:C157
	End if 
	
	  // Graceful kill of all occurance
	
	  // Launch 2 two additional calls to notepad
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	  //This should only kill all of them
	$lErr:=sys_KillProcessByName ("notepad.exe";0;1)
	If ($lErr#0)
		TRACE:C157
	End if 
	
	  // Immediate kill of single occurance
	
	  // Launch 2 calls to notepad
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	  // This should only kill one
	$lErr:=sys_KillProcessByName ("notepad.exe";1;0)
	If ($lErr#0)
		TRACE:C157
	End if 
	
	  // Immediate kill of all occurance
	
	  // Launch 2 two additional calls to notepad
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811("notepad.exe";$tStnIn;$tStdOut;$tStdErr)
	
	  //This should only kill all of them
	$lErr:=sys_KillProcessByName ("notepad.exe";0;0)
	If ($lErr#0)
		TRACE:C157
	End if 
	
	  // 32-bit application
	$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Hex Editor\\XVI32.exe"
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811($tPath;$tStnIn;$tStdOut;$tStdErr)
	
	  // Graceful kill
	$lErr:=sys_KillProcessByName ("XVI32.exe";1;1)
	If ($lErr#0)
		TRACE:C157
	End if 
	
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_BLOCKING_EXTERNAL_PROCESS";"False")
	SET ENVIRONMENT VARIABLE:C812("_4D_OPTION_HIDE_CONSOLE";"False")
	LAUNCH EXTERNAL PROCESS:C811($tPath;$tStnIn;$tStdOut;$tStdErr)
	
	  // Immediate kill
	$lErr:=sys_KillProcessByName ("XVI32.exe";1;0)
	If ($lErr#0)
		TRACE:C157
	End if 
End for 