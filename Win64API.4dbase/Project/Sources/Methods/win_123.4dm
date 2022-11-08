//%attributes = {}
  // win_123
  //
  // This method will test the TWAIN functionality with 32-bit drivers.
  //
  // ACW 11/8/22 WIN-123

C_LONGINT:C283($lErr;$i)
C_OBJECT:C1216($oFile)

ARRAY TEXT:C222($atSources;0)
$lErr:=TWAIN_GetSources ($atSources;0;0;1)

If (Size of array:C274($atSources)>=1)
	$lErr:=TWAIN_SetSource ($atSources{1})
End if 

If (Size of array:C274($atSources)>=2)
	$lErr:=TWAIN_SetSource ($atSources{2})
	
	ARRAY BLOB:C1222($axTWAINBLOB;0)
	$lErr:=TWAIN_AcquireImage (1;$axTWAINBLOB;1;0;1)
	
	For ($i;1;Size of array:C274($axTWAINBLOB))
		$oFile:=Folder:C1567(fk documents folder:K87:21).file("acquiredImage_"+Generate UUID:C1066+"_"+String:C10($i)+".bmp")
		If ($oFile.create())
			$oFile.setContent($axTWAINBLOB{$i})
		End if 
	End for 
End if 


