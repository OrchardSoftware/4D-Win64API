//%attributes = {}
  // win_119
  //
  // This method will test the TWAIN functionality.
  //
  // ACW 4/1/21 WIN-119

C_LONGINT:C283($lErr)

ARRAY TEXT:C222($atSources;0)

$lErr:=TWAIN_GetSources ($atSources;0;0)

If (Size of array:C274($atSources)>=1)
	$lErr:=TWAIN_SetSource ($atSources{1})
End if 

If (Size of array:C274($atSources)>=2)
	$lErr:=TWAIN_SetSource ($atSources{2})
End if 

