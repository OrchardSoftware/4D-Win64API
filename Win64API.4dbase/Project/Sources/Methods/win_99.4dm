//%attributes = {}
  // win_99
  //
  // This method tests the sys_SetRegText command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 3/8/21 WIN-97

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lErr;$i)
C_TEXT:C284($tRandom;$tRandom2)

$tRandom:=String:C10(Random:C100)
$lErr:=sys_SetRegText (GR_HKEY_CURRENT_USER;"Software\\WIN64API Orchard Software";"Win64String";$tRandom)

If (False:C215)  // Don't test GR_HKEY_LOCAL_MACHINE by default
	$lErr:=sys_SetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom)
	
	If ($lErr#1)
		If ($lErr=-5) | ($lErr=5)
			TRACE:C157  // You need to run as administrator to write to GR_HKEY_LOCAL_MACHINE
		Else 
			TRACE:C157
		End if 
	Else 
		$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom2)
		If ($tRandom2#$tRandom)
			TRACE:C157  // There was a problem with reading and/or writing
		End if 
	End if 
End if 

$lErr:=sys_SetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom)

If ($lErr#1)
	TRACE:C157
Else 
	$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom2)
	If ($tRandom2#$tRandom)
		TRACE:C157  // There was a problem with reading and/or writing
	End if 
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		$tRandom:=String:C10(Random:C100)
		$lErr:=sys_SetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom)
		
		If ($lErr#1)
			TRACE:C157
		Else 
			$lErr:=sys_GetRegText (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"Win64String";$tRandom2)
			If ($tRandom2#$tRandom)
				TRACE:C157  // There was a problem with reading and/or writing
			End if 
		End if 
	End for 
End if 

