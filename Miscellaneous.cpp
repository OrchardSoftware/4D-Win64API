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