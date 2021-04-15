//%attributes = {}
  // win_84
  //
  // This method tests the sys_ShellExecute command
  //
  // $1 - True if we are debugging memory issues
  //
  // JEM 3/17/21 WIN-84


C_TEXT:C284($tPath;$tResult)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Win64API.bmp"

$tResult:=sys_ShellExecute ("BADOperation";$tPath;"";"";1)
If ($tResult#"Invalid Operation")
	TRACE:C157
End if 


$tResult:=sys_ShellExecute ("open";"";"";"";1)
If ($tResult#"Invalid Operation")
	TRACE:C157
End if 


$tResult:=sys_ShellExecute ("open";$tPath;"";"";11)
If ($tResult#"Invalid HowToShow Constant")
	TRACE:C157
End if 

$tResult:=sys_ShellExecute ("open";$tPath;"";"";1)
If ($tResult#"")
	TRACE:C157
End if 
