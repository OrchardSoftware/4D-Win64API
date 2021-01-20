//%attributes = {}
  // Win64API_dialog_output
  //
  // Add to the output listbox
  //
  // $1 - Message to add to the output listbox
  //
  // ACW 1/12/21 WIN-109

C_TEXT:C284($1;$tMessage)

$tMessage:=$1

INSERT IN ARRAY:C227(atWIN64Output;1)
atWIN64Output{1}:=Timestamp:C1445+"  "+$tMessage
