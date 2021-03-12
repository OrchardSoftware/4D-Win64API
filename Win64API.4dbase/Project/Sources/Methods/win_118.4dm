//%attributes = {}
  // win_118
  //
  // This method will test the gui_SelectColor command
  //
  // ACW 3/12/21 WIN-118

C_LONGINT:C283($lErr;$lRed;$lGreen;$lBlue)

  // Yellow color
$lRed:=241
$lGreen:=196
$lBlue:=15

ARRAY LONGINT:C221($alCustomColors;0)
APPEND TO ARRAY:C911($alCustomColors;3951847)  // Red color
APPEND TO ARRAY:C911($alCustomColors;9295448)  // Green color
APPEND TO ARRAY:C911($alCustomColors;14391348)  // Blue color

$lErr:=gui_SelectColor ($lRed;$lGreen;$lBlue;1;$alCustomColors)

If ($lErr=0)
	  // User cancelled
Else 
	  // User picked a color
End if 
