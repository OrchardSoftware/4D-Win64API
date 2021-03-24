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

//----------------------------------------------------------------------
//
// FUNCTION:	sys_SendRawPrinterData
//
// PURPOSE:		Sends raw printer data directly to a printer. sys_PrintDirect2Driver
//
//
// AMS 12/5/14 #37816
//

void sys_SendRawPrinterData(PA_PluginParameters params) {
	PA_long32 PALReturnValue;
	PA_Unistring *PAUPrinterName, *PAURawData;

	BOOL bReturn;
	DWORD dwJob = 0 , dwSize = 0, dwBytesWritten = 0;
	DOC_INFO_1 DocInfo;
	HANDLE hPrinter = NULL;

	// Get the function parameters.
	PAUPrinterName = PA_GetStringParameter(params, 1);
	PAURawData = PA_GetStringParameter(params, 2);

	dwSize = (DWORD) (sizeof(WCHAR) * (PAURawData->fLength + 1));

	// Open a handle to the printer.
	bReturn = OpenPrinter((LPWSTR) PAUPrinterName->fString, &hPrinter, NULL);
	if (bReturn)
	{
		// Fill in the structure with info about this "document."
		DocInfo = { L"My Document", NULL, L"RAW" };

		// Inform the spooler the document is beginning.
		dwJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&DocInfo);
		if (dwJob > 0) {
			// Start a page.
			bReturn = StartPagePrinter(hPrinter);
			if (bReturn) {
				// Send the data to the printer.
				bReturn = WritePrinter(hPrinter, (LPVOID) PAURawData->fString, dwSize, &dwBytesWritten);
				EndPagePrinter(hPrinter);
			}
			// Inform the spooler that the document is ending.
			EndDocPrinter(hPrinter);
		}
		// Close the printer handle.
		ClosePrinter(hPrinter);
	}

	// Check to see if correct number of bytes were written.
	if (!bReturn || (dwBytesWritten != dwSize))
	{
		PALReturnValue = GetLastError();
	}
	else
	{
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}