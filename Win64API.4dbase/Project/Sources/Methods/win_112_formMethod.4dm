//%attributes = {}
  // win_112_formMethod
  //
  // ACW 1/14/21 WIN-112

C_LONGINT:C283($lErr)

$lErr:=gui_ShowWindowEx (Current form window:C827;SW_MINIMIZE)

DELAY PROCESS:C323(Current process:C322;25)

$lErr:=gui_ShowWindowEx (Current form window:C827;SW_MAXIMIZE)

DELAY PROCESS:C323(Current process:C322;25)

$lErr:=gui_ShowWindowEx (Current form window:C827;SW_RESTORE)
