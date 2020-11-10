//%attributes = {}
  // win_108
  //
  // This method tests the gui_GetWindowEx command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-108

C_LONGINT:C283($hWnd)

  // Get main window
$hWnd:=gui_GetWindowEx ("")

gui_SetWindowTitleEx ($hWnd;"Win64API Demo")
