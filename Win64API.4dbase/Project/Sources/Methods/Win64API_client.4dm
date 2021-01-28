//%attributes = {}
  // Win64API_client

C_LONGINT:C283($lWinRef;$lProcess)

ARRAY TEXT:C222(atWIN64Output;0)

$lWinRef:=Open form window:C675("/RESOURCES/Demo.json")
$lProcess:=New process:C317("win64API_main";0;"Win64API Demo";$lWinRef)
DIALOG:C40("/RESOURCES/Demo.json")
CLOSE WINDOW:C154($lWinRef)

