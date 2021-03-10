//%attributes = {}
  // win_87
  //
  // This method tests the sys_PlayWav command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 2/23/21 WIN-87

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lErr;$i)
C_TEXT:C284($tPath)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"DiffClick.wav"

$lErr:=sys_PlayWav ($tPath)

$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"organfinale.wav"

$lErr:=sys_PlayWav ($tPath)

If ($bMemoryTest)
	For ($i;1;1000)
		$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"DiffClick.wav"
		
		$lErr:=sys_PlayWav ($tPath)
		
		$tPath:=Get 4D folder:C485(Current resources folder:K5:16)+"organfinale.wav"
		
		$lErr:=sys_PlayWav ($tPath)
		
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 
