//%attributes = {}
  // win_113_formMethod
  //
  // ACW 1/7/21 WIN-113

C_LONGINT:C283($lErr)
C_TEXT:C284($tPath)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Win64API.ico"

$lErr:=gui_SetIconEx (Current form window:C827;$tPath)

