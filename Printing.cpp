/* --------------------------------------------------------------------------------
#
#	Printing.cpp
#	All of the methods related to printing
#	Project : Win64API
#	author : dwaddell
#	3/15/21
#
# --------------------------------------------------------------------------------*/

#include "Printing.h"

// ------------------------------------------------
//
//  FUNCTION: sys_GetDefPrinter( PA_PluginParameters params )
//
//  PURPOSE:  Determines which printer is marked as default
//		REB 3/6/09 #17333 Updated to work with Windows Vista and beyond.
//		REB 8/11/09		Removed logic used in versions of Windows older than 2000.
//
void sys_GetDefPrinter(PA_PluginParameters params)
{
	PA_long32 PALReturnValue;
	PA_Unistring* PAUPrinterName;

	DWORD dwDataSize;
	WCHAR *pcPrinterBuffer;
	
	PAUPrinterName = PA_GetStringParameter(params, 1);
	
	pcPrinterBuffer = NULL;
		
	dwDataSize = MAXBUF;
	pcPrinterBuffer = (WCHAR*)malloc(sizeof(WCHAR) * (dwDataSize + 1));
	pcPrinterBuffer[dwDataSize] = 0;

	PALReturnValue = GetDefaultPrinter(pcPrinterBuffer, &dwDataSize);
	PA_SetUnistring(PAUPrinterName, (PA_Unichar*) pcPrinterBuffer);

	free(pcPrinterBuffer); // WJF 6/4/15 #42921

	PA_ReturnLong(params, PALReturnValue);
}

// ------------------------------------------------
//
//  FUNCTION: sys_SetDefPrinter( PA_PluginParameters params )
//
//  PURPOSE:  Sets the default printer
//
void sys_SetDefPrinter(PA_PluginParameters params)
{
	PA_long32 PALReturnValue;
	PA_Unistring* PAUPrinterName;
	
	PALReturnValue = 0;
	PAUPrinterName = PA_GetStringParameter(params, 1);

	if (PAUPrinterName->fLength > 0) {
		PALReturnValue = SetDefaultPrinter((LPCWSTR)PAUPrinterName->fString);
	}

	PA_ReturnLong(params, PALReturnValue);
}

