/* --------------------------------------------------------------------------------
#
#	Miscellaneous.cpp
#	All of the methods related to random functions that don't fit in one of the other files
#	Project : Win64API
#	author : dwaddell
#	2/23/2021
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

// ------------------------------------------------
// 
//  FUNCTION: sys_GetUTCOffset ( PA_PluginParameters params )
//
//  REB 1/21/09 #19035 Updated to use GetTimeZoneInformation
//  JEM 3/18/20 WIN-121 Significant update
// ------------------------------------------------

void sys_GetUTCOffset(PA_PluginParameters params)
{
	DWORD dwReturn;
	DYNAMIC_TIME_ZONE_INFORMATION dtziTimeZoneData;
	TIME_ZONE_INFORMATION tziTimeZoneData;
	long lTimeZoneOffset;

	// This should cover everyone 
	dwReturn = GetDynamicTimeZoneInformation(&dtziTimeZoneData);

	// No error occurred trying to check time zones
	if ((dwReturn == TIME_ZONE_ID_DAYLIGHT) | (dwReturn == TIME_ZONE_ID_STANDARD))
	{
		// If the system is using disabled dynamic time zones - yay 
		if (!dtziTimeZoneData.DynamicDaylightTimeDisabled)
		{
			if (dwReturn == TIME_ZONE_ID_DAYLIGHT)
			{
				// In daylight savings
				lTimeZoneOffset = dtziTimeZoneData.Bias + dtziTimeZoneData.DaylightBias;
			}
			else
			{
				// In standard
				lTimeZoneOffset = dtziTimeZoneData.Bias + dtziTimeZoneData.StandardBias;
			}
		}
		else
		{
			// Dynamic time zones disabled so use fixed dates for transitions
			dwReturn = GetTimeZoneInformation(&tziTimeZoneData);

			// Again check for errors
			if (dwReturn == TIME_ZONE_ID_DAYLIGHT)
			{
				// In daylight savings
				lTimeZoneOffset = tziTimeZoneData.Bias + tziTimeZoneData.DaylightBias;
			}
			else if (dwReturn == TIME_ZONE_ID_STANDARD)
			{
				// In standard
				lTimeZoneOffset = tziTimeZoneData.Bias + tziTimeZoneData.StandardBias;
			}
			
			{
				// Error
				PA_SetLongParameter(params, 1, 0L);
				PA_ReturnLong(params, 0L);
			}
		}
	}
	
	// Final check to see what we need to return
	if ((dwReturn == TIME_ZONE_ID_DAYLIGHT) | (dwReturn == TIME_ZONE_ID_STANDARD))
	{
		PA_SetLongParameter(params, 1, lTimeZoneOffset);
		PA_ReturnLong(params, 1L);
	}
	else
	{
		// Error happened along the way
		PA_SetLongParameter(params, 1, 0L);
		PA_ReturnLong(params, 0L);
	}
}

//
//  FUNCTION: gui_SelectColor( PA_PluginParameters params)
//
//  PURPOSE:  Displays the color common dialog
//
//  COMMENTS:
//
//
//	DATE:	  dcc 11/25/02 (3.5.3)
//
//	MODIFICATIONS:
//
void gui_SelectColor(PA_PluginParameters params)
{
	PA_long32 PALReturnValue, PALRedParam, PALGreenParam, PALBlueParam, PALHasCustomColors;
	PA_Variable	PAVCustomColorArray;

	PA_long32 i;
	CHOOSECOLOR	cColor;
	COLORREF CRCustomColors[16];
	COLORREF CRSelected;
		
	PALReturnValue = 0;
	PALRedParam = PA_GetLongParameter(params, 1);
	PALGreenParam = PA_GetLongParameter(params, 2);
	PALBlueParam = PA_GetLongParameter(params, 3);
	PALHasCustomColors = PA_GetLongParameter(params, 4);

	if (PALRedParam > 255) PALRedParam = 0;
	if (PALGreenParam > 255) PALGreenParam = 0;
	if (PALBlueParam > 255) PALBlueParam = 0;

	if (PALHasCustomColors == 1) {
		PAVCustomColorArray = PA_GetVariableParameter(params, 5);
		if (PA_GetVariableKind(PAVCustomColorArray) == eVK_ArrayLongint) {
			for (i = 0; i < PA_GetArrayNbElements(PAVCustomColorArray); i++)
			{
				CRCustomColors[i] = (COLORREF) PA_GetLongintInArray(PAVCustomColorArray, i + 1);
			}
		}
	}

	ZeroMemory(&cColor, sizeof(CHOOSECOLOR));
	cColor.lStructSize = sizeof(CHOOSECOLOR);
	cColor.hwndOwner = (HWND) PA_GetMainWindowHWND();
	cColor.lpCustColors = CRCustomColors;
	
	if ((PALRedParam > 0) || (PALGreenParam > 0) || (PALBlueParam > 0)) {
		cColor.rgbResult = RGB(PALRedParam, PALGreenParam, PALBlueParam);
		cColor.Flags = CC_FULLOPEN | CC_RGBINIT;
	}
	else {
		cColor.Flags = CC_FULLOPEN;
	}

	if (ChooseColor(&cColor) == TRUE) {
		CRSelected = cColor.rgbResult;

		PALRedParam = GetRValue(CRSelected);
		PALGreenParam = GetGValue(CRSelected);
		PALBlueParam = GetBValue(CRSelected);

		PA_SetLongParameter(params, 1, PALRedParam);
		PA_SetLongParameter(params, 2, PALGreenParam);
		PA_SetLongParameter(params, 3, PALBlueParam);

		if (PALHasCustomColors == 1) {
			PA_ResizeArray(&PAVCustomColorArray, 16);
			for (i = 0; i < 16; i++)
			{
				PA_SetLongintInArray(PAVCustomColorArray, i + 1, CRCustomColors[i]);
			}
			PA_SetVariableParameter(params, 5, PAVCustomColorArray, 0);
		}

		PALReturnValue = 1;
	}

	PA_ReturnLong(params, PALReturnValue);
}
