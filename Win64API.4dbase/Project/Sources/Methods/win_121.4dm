//%attributes = {}
  // win_121
  //
  // This method tests the sys_ShellExecute command
  //
  // JEM 3/18/21 WIN-121

C_LONGINT:C283($lNumAttempts;$i;$lErr;$lOffset)

$lNumAttempts:=500

For ($i;1;$lNumAttempts)
	$lErr:=sys_GetUTCOffset ($lOffset)
	If (($lErr#1) | (($lOffset#240) & ($lOffset#300)))
		TRACE:C157
	End if 
	
End for 
