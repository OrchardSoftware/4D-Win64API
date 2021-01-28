//%attributes = {}


Case of 
	: (Form event code:C388=On Load:K2:1)
		
		SET TIMER:C645(300)
		
	: (Form event code:C388=On Timer:K2:25)
		
		win_109 (False:C215;Current form window:C827)
		
End case 

