//%attributes = {}
  // win_71
  //
  // This methos will test the sys_GetRegEnum command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 10/16/20 WIN-71

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lErr;$i)

ARRAY TEXT:C222($atKeys;0)
$lErr:=sys_GetRegEnum (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft";$atKeys)

If (Size of array:C274($atKeys)=0) | ($lErr#1)
	TRACE:C157  // Something is unexpected, there should be a lot of items found at "SOFTWARE\Microsoft"
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		ARRAY TEXT:C222($atKeys;0)
		$lErr:=sys_GetRegEnum (GR_HKEY_LOCAL_MACHINE;"SOFTWARE\\Microsoft";$atKeys)
		DELAY PROCESS:C323(Current process:C322;10)
	End for 
End if 

