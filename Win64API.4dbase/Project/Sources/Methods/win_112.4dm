//%attributes = {}
  // win_112
  //
  // This method tests the gui_ShowWindowEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 1/14/21 WIN-112

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

$lErr:=gui_ShowWindowEx (-1;SW_MINIMIZE)

DELAY PROCESS:C323(Current process:C322;25)

$lErr:=gui_ShowWindowEx (-1;SW_MAXIMIZE)

If (Application type:C494#4D Server:K5:6)
	If ($lWinRef>0)
		CALL FORM:C1391($lWinRef;"win_112_formMethod")
	End if 
End if 