//%attributes = {}
  // win_76
  //
  // This method will call the sys_LoggingStop command
  //
  // ACW 3/5/21 WIN-76

C_LONGINT:C283($lErr)

$lErr:=sys_LoggingStop 

If ($lErr#0)
	TRACE:C157
End if 

  // Call a command just to verify that logging has stopped
win_71 (False:C215)
