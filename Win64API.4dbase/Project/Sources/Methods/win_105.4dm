//%attributes = {}
  // win_105
  //
  // This method will call the sys_LoggingStart command
  //
  // ACW 3/4/21 WIN-105

C_LONGINT:C283($lErr)
C_TEXT:C284($tPath)

$tPath:=Get 4D folder:C485(Logs folder:K5:19)+"Win32API"+Folder separator:K24:12

$lErr:=sys_LoggingStart ($tPath;1)
