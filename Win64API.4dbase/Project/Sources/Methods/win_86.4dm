//%attributes = {}
  // win_86
  //
  // This method tests the sys_SendRawPrinterData command
  //
  // ACW 3/16/21 WIN-86

C_LONGINT:C283($lErr)
C_TEXT:C284($tPrinterName)

  // We don't need to run this by default in the demo database
If (False:C215)
	$lErr:=sys_GetDefPrinter ($tPrinterName)
	
	If ($tPrinterName#"")
		$lErr:=sys_SendRawPrinterData ("Microsoft XPS Document Writer";"Hello World!")
		
		$lErr:=sys_SendRawPrinterData ($tPrinterName;"Hello World!")
	End if 
End if 
