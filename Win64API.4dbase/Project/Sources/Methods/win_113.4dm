//%attributes = {}
  // win_113
  //
  // This method tests the gui_SetIconEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 1/8/21 WIN-113

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

C_LONGINT:C283($lErr;$i)
C_TEXT:C284($tPath)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"Win64API.ico"

$lErr:=gui_SetIconEx (-1;$tPath)

If ($bMemoryTest)
	For ($i;1;1000)
		$lErr:=gui_SetIconEx (-1;$tPath)
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 

  // The demo dialog shouldn't be open on the server
If (Application type:C494#4D Server:K5:6)
	CALL FORM:C1391($lWinRef;"win_113_formMethod")
End if 
