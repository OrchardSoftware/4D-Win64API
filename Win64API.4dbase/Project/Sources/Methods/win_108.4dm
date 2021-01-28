//%attributes = {}
  // win_108
  //
  // This method tests the gui_SetWindowTitleEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 10/16/20 WIN-108

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

C_LONGINT:C283($i;$lErr)
C_TEXT:C284($tTest)

$lErr:=gui_SetWindowTitleEx (-1;"Win64API Demo")

  // Test a long window's title
For ($i;1;50)
	$tTest:="Hello World "
	$lErr:=gui_SetWindowTitleEx (-1;"Win64API Demo "+String:C10($i)+($tTest*$i))
End for 

$lErr:=gui_SetWindowTitleEx (-1;"Win64API Demo")

If ($bMemoryTest)
	For ($i;1;1000)
		$lErr:=gui_SetWindowTitleEx (-1;"Win64API Demo "+String:C10($i))
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 

  // The demo dialog shouldn't be open on the server
If (Application type:C494#4D Server:K5:6)
	CALL FORM:C1391($lWinRef;"win_108_formMethod")
End if 