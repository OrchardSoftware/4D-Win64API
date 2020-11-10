//%attributes = {}
  // win_107
  //
  // This method tests the gui_GetWindowEx command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-107

C_LONGINT:C283($hWnd)

  // Get main window
$hWnd:=gui_GetWindowEx ("")

