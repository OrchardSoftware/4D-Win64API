//%attributes = {}
  // win_115
  //
  // This method tests the gui_SetWindowLongEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 3/11/21 WIN-115

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

C_LONGINT:C283($lErr)

$lErr:=gui_SetWindowLongEx (-1;WS_MINIMIZEBOX;WIN_DISABLE)

$lErr:=gui_SetWindowLongEx (-1;WS_MAXIMIZEBOX;WIN_DISABLE)

If (False:C215)
	$lErr:=gui_SetWindowLongEx (-1;WS_MINIMIZEBOX;WIN_ENABLE)
	
	$lErr:=gui_SetWindowLongEx (-1;WS_MAXIMIZEBOX;WIN_ENABLE)
End if 

If (Application type:C494#4D Server:K5:6)
	If ($lWinRef>0)
		CALL FORM:C1391($lWinRef;"win_115_formMethod")
	End if 
End if 
