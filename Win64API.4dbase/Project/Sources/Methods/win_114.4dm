//%attributes = {}
  // win_114
  //
  // This method tests the gui_DisableCloseBoxEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 1/14/21 WIN-114

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

  // Don't know if you'd want to do this
$lErr:=gui_DisableCloseBoxEx (-1)

If (Application type:C494#4D Server:K5:6)
	If ($lWinRef>0)
		CALL FORM:C1391($lWinRef;"win_114_formMethod")
	End if 
End if 