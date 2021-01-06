//%attributes = {}
  // win_107
  //
  // This method tests the gui_GetWindowEx command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-107

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($hWnd)

  // Get main window
$hWnd:=gui_GetWindowEx ("")

If ($bMemoryTest)
	For ($i;1;1000)
		$hWnd:=gui_GetWindowEx ("")
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 
