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
	DWORD dwJob = 0 , dwConvBuffSize = 0, dwBytesWritten = 0;
	char *mbConversion;
	size_t numConvertedBytes;
	errno_t error;
	DOC_INFO_1 DocInfo;
	HANDLE hPrinter = NULL;

	// Get the function parameters.
	PAUPrinterName = PA_GetStringParameter(params, 1);
	PAURawData = PA_GetStringParameter(params, 2);

	// JEM 5/19/21 H-14330 If we have data to send   
	if (PAURawData->fLength > 0) 
	{
		// JEM 5/19/21 H-14330 MB_CUR_MAX is the largest size a single wchar can be in mb.
		dwConvBuffSize = MB_CUR_MAX * (PAURawData->fLength + 1);

		// JEM 5/19/21 H-14330 Allocate a buffer for the converted data
		mbConversion = (char *)malloc(dwConvBuffSize);

		// JEM 5/19/21 H-14330 Convert the wchar to char
		error = wcstombs_s(&numConvertedBytes, mbConversion, (size_t)dwConvBuffSize, (WCHAR*)PAURawData->fString, _TRUNCATE);

		// JEM 5/19/21 H-14330 No error in when converting
		if (error == 0)  
		{
			// JEM 5/19/21 H-14330 If we have data that was converted
			if (numConvertedBytes > 0)  
			{
				// JEM 5/19/21 H-14330 Reuse dwConvBuffSize to now hold the number bytes used in the coverted buffer. 
				dwConvBuffSize = (DWORD)numConvertedBytes;
				dwConvBuffSize--; // Subtract one so we don't send the ending NULL char

				// Open a handle to the printer.
				bReturn = OpenPrinter((LPWSTR)PAUPrinterName->fString, &hPrinter, NULL);
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
							// JEM 5/19/21 H-14330 Now using the mbConversion buffer rather than the raw data from 4D call
							bReturn = WritePrinter(hPrinter, (LPVOID)mbConversion, dwConvBuffSize, &dwBytesWritten);
							EndPagePrinter(hPrinter);
						}
						// Inform the spooler that the document is ending.
						EndDocPrinter(hPrinter);
					}
					// Close the printer handle.
					ClosePrinter(hPrinter);
				}

				// Check to see if correct number of bytes were written.
				if (!bReturn || (dwBytesWritten != dwConvBuffSize))
				{
					PALReturnValue = GetLastError();
				}
				else
				{
					PALReturnValue = 0;
				}
			}
			else
			{
				// JEM 5/19/21 H-14330 No data after conversion 
				PALReturnValue = -1;
			}
		}
		else
		{
			// JEM 5/19/21 H-14330 Return the conversion error code
			PALReturnValue = error;
		}

		// JEM 5/19/21 H-14330 Free the conversion buffer
		free(mbConversion);
	}
	else
	{
		// JEM 5/19/21 H-14330 No data was passed to send, so just indicate we were successful.
		PALReturnValue = 0; 
	}

	PA_ReturnLong(params, PALReturnValue);
}

// This is just stub for now and will possible be removed from the plugin altogether
void sys_GetPrintJob(PA_PluginParameters params)
{
	PA_long32 PALReturnValue;

	PALReturnValue = 0;

	PA_ReturnLong(params, PALReturnValue);
}
