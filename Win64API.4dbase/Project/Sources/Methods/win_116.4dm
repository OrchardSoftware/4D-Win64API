//%attributes = {}
  // win_116
  //
  //
  // ACW 3/26/21 WIN-116

C_LONGINT:C283($lErr)
C_TEXT:C284($tPath)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Win64API.bmp"

$lErr:=gui_LoadBackground ($tPath)
