//%attributes = {}
  // win_115_formMethod
  //
  // ACW 3/11/21 WIN-115

C_LONGINT:C283($lErr)

$lErr:=gui_SetWindowLongEx (Current form window:C827;WS_MINIMIZEBOX;WIN_DISABLE)

$lErr:=gui_SetWindowLongEx (Current form window:C827;WS_MAXIMIZEBOX;WIN_DISABLE)

If (False:C215)
	$lErr:=gui_SetWindowLongEx (Current form window:C827;WS_MINIMIZEBOX;WIN_ENABLE)
	
	$lErr:=gui_SetWindowLongEx (Current form window:C827;WS_MAXIMIZEBOX;WIN_ENABLE)
End if 
