//%attributes = {}
  // win_97
  //
  // This method tests the sys_SetRegLongint command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 3/8/21 WIN-97

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($lRandom;$lRandom2;$lRandom3;$lErr;$i)
$lRandom:=Random:C100

If (False:C215)  // Don't test GR_HKEY_LOCAL_MACHINE by default
	$lErr:=sys_SetRegLongint (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom)
	
	If ($lErr#1)
		If ($lErr=-5) | ($lErr=5)
			TRACE:C157  // You need to run as administrator to write to GR_HKEY_LOCAL_MACHINE
		Else 
			TRACE:C157
		End if 
	Else 
		$lErr:=sys_GetRegLongint (GR_HKEY_LOCAL_MACHINE;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom2)
		If ($lRandom#$lRandom2)
			TRACE:C157  // There was a problem with reading and/or writing
		End if 
	End if 
End if 

$lErr:=sys_SetRegLongint (GR_HKEY_CURRENT_USER;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom)

If ($lErr#1)
	TRACE:C157
Else 
	$lErr:=sys_GetRegLongint (GR_HKEY_CURRENT_USER;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom2)
	If ($lRandom#$lRandom2)
		TRACE:C157  // There was a problem with reading and/or writing
	End if 
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		$lRandom:=Random:C100
		$lErr:=sys_SetRegLongint (GR_HKEY_CURRENT_USER;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom)
		
		If ($lErr#1)
			TRACE:C157
		Else 
			$lErr:=sys_GetRegLongint (GR_HKEY_CURRENT_USER;"Software\\WIN64API Orchard Software";"RandomNumber";$lRandom2)
			If ($lRandom#$lRandom2)
				TRACE:C157  // There was a problem with reading and/or writing
			End if 
		End if 
	End for 
End if 