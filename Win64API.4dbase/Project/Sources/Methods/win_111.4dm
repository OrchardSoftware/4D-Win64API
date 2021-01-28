//%attributes = {}
  // win_111
  //
  // This method tests the gui_TakeScreenshotEx command
  //
  // $1 - True if we are debugging memory issues
  // $2 - Window reference
  //
  // ACW 1/12/21 WIN-111

C_BOOLEAN:C305($1;$bMemoryTest)
C_LONGINT:C283($2;$lWinRef)

$bMemoryTest:=$1
$lWinRef:=$2

C_BOOLEAN:C305($bCreated)
C_LONGINT:C283($lErr)
C_TEXT:C284($tPath)
C_OBJECT:C1216($oFolder)

$oFolder:=Folder:C1567(fk resources folder:K87:11).folder("Screenshots")
If (Not:C34($oFolder.exists))
	$oFolder:=Folder:C1567($oFolder.path)
	$bCreated:=$oFolder.create()
End if 

$tPath:=$oFolder.platformPath+String:C10(Milliseconds:C459)+" screenshot.bmp"
$lErr:=gui_TakeScreenshotEx (-1;$tPath)

If ($bMemoryTest)
	For ($i;1;1000)
		$tPath:=$oFolder.platformPath+String:C10(Milliseconds:C459)+" screenshot.bmp"
		$lErr:=gui_TakeScreenshotEx (-1;$tPath)
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 

If (Application type:C494#4D Server:K5:6)
	CALL FORM:C1391($lWinRef;"win_111_formMethod")
End if 