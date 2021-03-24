//%attributes = {}
  // win_120
  //
  // This method tests the sys_SetDefPrinter command
  //
  // ACW 3/15/21 WIN-120

C_LONGINT:C283($lErr)
C_TEXT:C284($tDefaultPrinterName;$tPrinterName;$tPrinterName2)

ARRAY TEXT:C222($atPrinterNames;0)
PRINTERS LIST:C789($atPrinterNames)

$tPrinterName:=""

$lErr:=sys_GetDefPrinter ($tDefaultPrinterName)

If (Size of array:C274($atPrinterNames)>0)
	If ($atPrinterNames{1}#$tDefaultPrinterName)
		$tPrinterName:=$atPrinterNames{1}
	Else 
		If (Size of array:C274($atPrinterNames)>1)
			$tPrinterName:=$atPrinterNames{2}
		End if 
	End if 
End if 

If ($tPrinterName#"")
	$lErr:=sys_SetDefPrinter ($tPrinterName)
	If ($lErr=0)
		TRACE:C157  // Something went wrong
	End if 
	
	$lErr:=sys_GetDefPrinter ($tPrinterName2)
	If ($tPrinterName#$tPrinterName2)
		TRACE:C157
	End if 
	
	  // Reset the default printer back to the original
	$lErr:=sys_SetDefPrinter ($tDefaultPrinterName)
End if 
