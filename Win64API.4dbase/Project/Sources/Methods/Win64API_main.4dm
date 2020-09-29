//%attributes = {}
  // Win64API_main
  //
  // This method will act as a unit test to the Win64API commands and a sample database to show what
  // it can do.
  //
  // ACW 10/3/19 WIN-71

C_BOOLEAN:C305($bMemoryTest)

  // Turn this to true to loop over a command many times to test memory usage.
$bMemoryTest:=False:C215

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
