//%attributes = {}
  // win_109
  //
  // This method tests the gui_GetWindowStateEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 1/8/21 WIN-113

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

$lState:=gui_GetWindowStateEx (-1)
If ($lWinRef>0)
	Win64API_dialog_output ("gui_GetWindowStateEx (MDI): "+String:C10($lState))
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		$lState:=gui_GetWindowStateEx (-1)
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 

  // The demo dialog shouldn't be open on the server
If (Application type:C494#4D Server:K5:6)
	  // CALL FORM($lWinRef;"win_113_formMethod")
	$lState:=gui_GetWindowStateEx ($lWinRef)
	Win64API_dialog_output ("gui_GetWindowStateEx ("+String:C10($lWinRef)+"): "+String:C10($lState))
End if 