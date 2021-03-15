//%attributes = {}
  // win_104
  //
  // This method tests the sys_GetDefPrinter command
  //
  // $1 - True if we are debugging memory issues
  //
  // ACW 3/15/21 WIN-104

C_BOOLEAN:C305($1;$bMemoryTest)

$bMemoryTest:=$1

C_LONGINT:C283($i;$lErr)
C_TEXT:C284($tPrinterName)

ARRAY TEXT:C222($atPrinterNames;0)
PRINTERS LIST:C789($atPrinterNames)

$lErr:=sys_GetDefPrinter ($tPrinterName)

If ($tPrinterName="")
	TRACE:C157  // Is there not a default printer?
Else 
	If (Find in array:C230($atPrinterNames;$tPrinterName)<=0)
		TRACE:C157  // Why didn't we find the default printer in the list of printers?
	End if 
End if 

If ($bMemoryTest)
	For ($i;1;1000)
		$lErr:=sys_GetDefPrinter ($tPrinterName)
	End for 
End if 
