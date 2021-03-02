/* --------------------------------------------------------------------------------
#
#	Miscellaneous.cpp
#	All of the methods related to random functions that don't fit in one of the other files
#	Project : Win64API
#	author : dwaddell
#	10/26/2020
#
# --------------------------------------------------------------------------------*/


#include "Miscellaneous.h"

// ------------------------------------------------
//
//  FUNCTION: sys_PlayWav( PA_PluginParameters params )
//
void sys_PlayWav(PA_PluginParameters params)
{
	BOOL bFuncReturn;
	PA_Unistring* PAUFileName;

	PAUFileName = PA_GetStringParameter(params, 1);

	// Stop the sound that's is playing
	bFuncReturn = PlaySound (0, 0, SND_ASYNC);

	if (PlaySound((LPCTSTR)PAUFileName->fString, NULL, SND_ASYNC)) {
		PA_ReturnLong(params, 1);
	}
	else {
		PA_ReturnLong(params, 0);
	}
}
