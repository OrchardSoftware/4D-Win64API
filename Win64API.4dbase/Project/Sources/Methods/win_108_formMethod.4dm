//%attributes = {}
  // win_108_formMethod
  //
  // ACW 1/7/21 WIN-108

C_LONGINT:C283($i;$lErr)
C_TEXT:C284($tTest)

  // While we'd recommend using SET WINDOW TITLE you could use gui_SetWindowTitleEx
$lErr:=gui_SetWindowTitleEx (Current form window:C827;"Win64API Demo - Internal Dialog")

  // Test a long window's title
For ($i;1;50)
	$tTest:="Hello World "
	$lErr:=gui_SetWindowTitleEx (Current form window:C827;"Win64API Demo "+String:C10($i)+($tTest*$i))
End for 

  // While we'd recommend using SET WINDOW TITLE you could use gui_SetWindowTitleEx
$lErr:=gui_SetWindowTitleEx (Current form window:C827;"Win64API Demo - Internal Dialog")

If (Not:C34(Get window title:C450(Current form window:C827)="Win64API Demo - Internal Dialog"))
	TRACE:C157  // We should have a match in the If statement
End if 

