//%attributes = {}
  // win_111_formMethod
  //
  // ACW 1/12/21 WIN-111

C_BOOLEAN:C305($bCreated)
C_LONGINT:C283($lErr)
C_OBJECT:C1216($oFolder)
C_TEXT:C284($tPath)

$oFolder:=Folder:C1567(fk resources folder:K87:11).folder("Screenshots")
If (Not:C34($oFolder.exists))
	$oFolder:=Folder:C1567($oFolder.path)
	$bCreated:=$oFolder.create()
End if 

$tPath:=$oFolder.platformPath+String:C10(Milliseconds:C459)+" dialog screenshot.bmp"
$lErr:=gui_TakeScreenshotEx (Current form window:C827;$tPath)
