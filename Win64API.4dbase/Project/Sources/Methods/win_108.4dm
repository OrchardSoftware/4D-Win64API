//%attributes = {}
  // win_108
  //
  // This method tests the gui_GetWindowEx command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-108

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($i;$hWnd;$lErr)

  // Get main window
$hWnd:=gui_GetWindowEx ("")

$lErr:=gui_SetWindowTitleEx ($hWnd;"Win64API Demo")

If ($bMemoryTest)
	For ($i;1;1000)
		$lErr:=gui_SetWindowTitleEx ($hWnd;"Win64API Demo "+String:C10($i))
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 