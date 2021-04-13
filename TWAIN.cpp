/* --------------------------------------------------------------------------------
#
#	TWAIN.cpp
#	All of the methods related to TWAIN
#	Project : Win64API
#	author : dwaddell
#	4/1/21
#
# --------------------------------------------------------------------------------*/

#include "TWAIN.h"

WCHAR wc4DXPath[MAXBUF];
WCHAR* wcTwainSource;

// MWD 10/21/05 #9246
// Use the DllMain function to get the path to the calling DLL and store it in a global for further use.
#if WIN32
#ifdef _cplusplus
extern "C" __declspec(dllexport) {
#endif
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
	{
		BOOL result = TRUE;
		GetModuleFileName(hinstDLL, wc4DXPath, MAXBUF); // Get the path to the 4DX
		return result;
	}
#ifdef _cplusplus
}
#endif
#endif

// Note, this does not need to use the semaphore since it is already waiting on a file
void TWAIN_GetSources(PA_PluginParameters params)
{
	PA_Variable PAVSources;
	PA_long32 PALReturnValue, PALDoNotAddSuffix, PALFlags;
	
	size_t bufferSize;
	WCHAR *WCPluginPath, *WCToCharInBuffer;
	WCHAR filePath[MAX_PATH] = L"", lpParameters[16] = L"-S", source[256] = L"";
	SHELLEXECUTEINFO utilities;
	DWORD dwExitCode = 0, index = 1;
	BOOL bSuccess = FALSE;
	FILE *fp = NULL;
	
	PAVSources = PA_GetVariableParameter(params, 1);
	PA_ResizeArray(&PAVSources, 0);

	PALDoNotAddSuffix = PA_GetLongParameter(params, 2); // WJF 10/27/16 Win-41

	PALFlags = PA_GetLongParameter(params, 3); // SDL 10/3/17 WIN-51 

	PALReturnValue = 1;

	bufferSize = (25 + wcslen(wc4DXPath));
	WCPluginPath = (WCHAR*)malloc(sizeof(WCHAR) * bufferSize);

	wcscpy_s(WCPluginPath, bufferSize, wc4DXPath);
		
	WCToCharInBuffer = wcsrchr(WCPluginPath, L'\\');
	if (WCToCharInBuffer != NULL) {
		WCToCharInBuffer[0] = NULL;
	}
		
	wcscat_s(WCPluginPath, bufferSize, L"\\Orchard_Utilities.exe");

	GetTempPath(MAX_PATH, filePath);
	wcscat_s(filePath, MAX_PATH, L"twainSources.txt");

	utilities.cbSize = sizeof(SHELLEXECUTEINFO);
	utilities.fMask = SEE_MASK_NOCLOSEPROCESS;
	utilities.hInstApp = NULL;
	utilities.hwnd = (HWND)PA_GetMainWindowHWND(); 
	utilities.lpFile = WCPluginPath;
	utilities.lpParameters = lpParameters;
	utilities.lpDirectory = NULL;
	utilities.nShow = SW_HIDE;
	utilities.lpVerb = NULL;

	if (ShellExecuteEx(&utilities)) {
		PA_YieldAbsolute();
		PA_YieldAbsolute();
		PA_YieldAbsolute();

		do {
			bSuccess = GetExitCodeProcess(utilities.hProcess, &dwExitCode);
			PA_YieldAbsolute();
		} while ((dwExitCode == STILL_ACTIVE) && (bSuccess));

		_wfopen_s(&fp, filePath, L"r");  // ZRW 4/13/17 WIN-39 Using the more secure method


		if (fp) {
			while (fgetws(source, 256, fp) != NULL) {

				if (wcscmp(source, L"-1\n") == 0) { // Failed to load Twain library // WJF 3/7/16 Win-7 Added \n
					PALReturnValue = -1;
				}
				else if (wcscmp(source, L"-2\n") == 0) { // Failed to open Data Source Manager // WJF 3/7/16 Win-7 Added \n
					PALReturnValue = -2;
				}
				else if (wcscmp(source, L"") == 0) { // Empty line
													// do nothing
				}
				else { // Valid Product Name
					WCToCharInBuffer = wcsrchr(source, L'\n');
					if (WCToCharInBuffer != NULL) {
						WCToCharInBuffer[0] = NULL;
					}
					
					if (!PALDoNotAddSuffix) // WJF 10/27/16 Win-41 Do not add suffix if this is TRUE
					{
						wcscat_s(source, 256, L"-TWAIN"); // WJF 9/21/15 #43940  // ZRW 4/5/17 WIN-39 256 -> sizeof(source)
					}

					PA_ResizeArray(&PAVSources, index);

					PA_Unistring paSource = PA_CreateUnistring((PA_Unichar*)source);
					PA_SetStringInArray(PAVSources, index, &paSource);

					++index;
				}
			}

			fclose(fp);

			fp = NULL;

			DeleteFile(filePath);
		}
	}
	else {
		PALReturnValue = -1;
	}

	if (!(PALFlags & TW_FLAG_EXCLUDE_WIA)) { // SDL 10/3/17 WIN-51 Option to include WIA drivers (default)
		GetTempPath(MAX_PATH, filePath);

		wcscat_s(filePath, MAX_PATH, L"wiaSources.txt");
		
		wcscpy_s(lpParameters, 16, L"-ws");

		utilities.hProcess = NULL;
		utilities.lpParameters = lpParameters;

		if (ShellExecuteEx(&utilities)) {
			PA_YieldAbsolute();
			PA_YieldAbsolute();
			PA_YieldAbsolute();

			do {
				bSuccess = GetExitCodeProcess(utilities.hProcess, &dwExitCode);
				PA_YieldAbsolute();
			} while ((dwExitCode == STILL_ACTIVE) && (bSuccess));

			
			_wfopen_s(&fp, filePath, L"r");  // ZRW 4/13/17 WIN-39 Using the more secure method

			if (fp) {
				while (fgetws(source, 256, fp) != NULL) {
					if (wcscmp(source, L"") != 0) {
						WCToCharInBuffer = wcsrchr(source, L'\n');
						if (WCToCharInBuffer != NULL) {
							WCToCharInBuffer[0] = NULL;
						}

						if (!PALDoNotAddSuffix) // WJF 10/27/16 Win-41 Do not add suffix if this is TRUE
						{
							wcscat_s(source, 256, L"-WIA");
						}

						PA_ResizeArray(&PAVSources, index);

						PA_Unistring paSource = PA_CreateUnistring((PA_Unichar*)source);
						PA_SetStringInArray(PAVSources, index, &paSource);

						++index;
					}
				}

				fclose(fp);

				fp = NULL;
			}

			DeleteFile(filePath);
		}
		else {
			PALReturnValue = -2;
		}
	}

	free(WCPluginPath);

	PA_SetVariableParameter(params, 1, PAVSources, 0);
	PA_ReturnLong(params, PALReturnValue);
}




void TWAIN_SetSource(PA_PluginParameters params)
{
	PA_long32 PALReturnValue;
	PA_Unistring *PAUSourceName;

	PAUSourceName = PA_GetStringParameter(params, 1);
	
	PALReturnValue = 1;

	// WJF 9/11/15 #43727 Begin changes
	if (wcTwainSource) { // If we've already set a source, clear it
		free(wcTwainSource);

		wcTwainSource = NULL;
	}
		
	// WJF 7/13/16 Win-21 Removed typecasting on malloc to follow C best practices
	size_t bufferSize = (1 + PAUSourceName->fLength);
	wcTwainSource = (WCHAR*) malloc(sizeof(WCHAR) * bufferSize);
		
	// WJF 9/29/15 Added a check to see if it was actually allocated and a new error code
	if (wcTwainSource) {
		wcscpy_s(wcTwainSource, bufferSize, (WCHAR*) PAUSourceName->fString);
	}
	else {
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}


void TWAIN_AcquireImage(PA_PluginParameters params)
{
	PA_long32 PALReturnValue = 0, PALShowDialog, PALGetMultiple, PALwiaMode;
	PA_Variable PAVBlobArray, PAVTWAINImageBlob;
	
	FILE *TWAINImageFilePointer;
	size_t bufferSize;
	DWORD indexInArray = 0;
	WCHAR fileName[MAX_PATH] = L"", WCFileNameToBlob[MAX_PATH] = L"", WC4DCommand[256] =  L"";
	WCHAR *WCToCharInBuffer;
	TWAIN_CAPTURE TWAINCapture;
	HANDLE CaptureThread;
	UINT threadID = 0;

	PALShowDialog = PA_GetLongParameter(params, 1);

	//PAVBlobArray = PA_GetVariableParameter(params, 2);
	PAVBlobArray = PA_CreateVariable(eVK_ArrayBlob);
	PA_ResizeArray(&PAVBlobArray, indexInArray);

	PALGetMultiple = PA_GetLongParameter(params, 3); // WJF 9/21/15 #43940

	PALwiaMode = PA_GetLongParameter(params, 4); // WJF 9/21/15 #43940

	GetTempPath(MAX_PATH, fileName);
	wcscat_s(fileName, MAX_PATH, L"TWNIMG.bmp");

	// Allow the image dialog to display if so desired.
	// WJF 9/10/15 #43727 Changed to use a new variable instead of the EZTWAIN function
	if (PALShowDialog) {
		TWAINCapture.showUI = TRUE;
	}
	else {
		TWAINCapture.showUI = FALSE;
	}

	// REB 2/26/13 #35165 Load our variables into the structure we can pass to the new thread
	TWAINCapture.returnValue = 0;
	// TWAINCapture.DIBHandle = DIBHandle; // WJF 9/10/15 #43727 Removed
	TWAINCapture.done = FALSE;

	TWAINCapture.get64 = 1; // WJF 9/10/15 #43727 // ACW 4/5/21 WIN-119 Hard-coded to 1 now

	TWAINCapture.getMultiple = PALGetMultiple; // WJF 9/21/15 #43940

	TWAINCapture.wiaMode = PALwiaMode; // WJF 9/21/15 #43940

	TWAINCapture.numPictures = 1; // WJF 9/21/15 #43940 Default

	TWAINCapture.filePath = fileName;

	bufferSize = (1 + wcslen(fileName));
	TWAINCapture.filePath = (WCHAR*)malloc(sizeof(WCHAR) * bufferSize);
	wcscpy_s(TWAINCapture.filePath, bufferSize, fileName);
		
	// REB 2/26/13 #35165 Start a new thread to handle the image acquisition so that we can yield time
	// back to 4D to prevent an application timeout.
	CaptureThread = (HANDLE)_beginthreadex(NULL, 0, TWAIN_GetImage, &TWAINCapture, 0, &threadID);

	// REB 2/26/13 #35165 4D says to call this at least three times when starting an external process.
	// That's not exactly what we're doing but I'll err on the side of caution.
	PA_YieldAbsolute();
	PA_YieldAbsolute();
	PA_YieldAbsolute();

	// Yield time back to 4D until the capture is finished.
	while (TWAINCapture.done == FALSE) {
		PA_YieldAbsolute();
		Sleep(5);
	}

	PALReturnValue = TWAINCapture.returnValue;

	wcscpy_s(WCFileNameToBlob, MAX_PATH, fileName);
	WCToCharInBuffer = wcsrchr(WCFileNameToBlob, L'.');
	if (WCToCharInBuffer != NULL) {
		WCToCharInBuffer[0] = NULL;
	}
	wcscat_s(WCFileNameToBlob, MAX_PATH, L"1.bmp");

	if ((GetFileAttributes(WCFileNameToBlob) != INVALID_FILE_ATTRIBUTES) && (GetLastError() != ERROR_FILE_NOT_FOUND)) {

		// TWAIN_WriteNativetoFilename returns 0 on success
		if (PALReturnValue == 0) {
			PALReturnValue = 1;

			// WJF 9/21/15 #43940
			for (int i = 0; i < TWAINCapture.numPictures; i++) {
				// Increase our index
				indexInArray++;

				// If we are on a 2nd pass then we need to get a new file name
				if (indexInArray > 1) {
					wcscpy_s(WCFileNameToBlob, MAX_PATH, fileName);
					WCToCharInBuffer = wcsrchr(WCFileNameToBlob, L'.');
					if (WCToCharInBuffer != NULL) {
						WCToCharInBuffer[0] = NULL;
					}
					wchar_t bufferExtension[10];
					int j = swprintf_s(bufferExtension, 10, L"%d.bmp", indexInArray);
					wcscat_s(WCFileNameToBlob, MAX_PATH, bufferExtension);
				}
							
				_wfopen_s(&TWAINImageFilePointer, WCFileNameToBlob, L"rb");
				fseek(TWAINImageFilePointer, 0, SEEK_END);
				long filelen = ftell(TWAINImageFilePointer);
				rewind(TWAINImageFilePointer);

				char *buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
				fread(buffer, filelen, 1, TWAINImageFilePointer); // Read in the entire file
				fclose(TWAINImageFilePointer); // Close the file
				
				// Create a blob variable and put it in the array we received as a parameter
				PAVTWAINImageBlob = PA_CreateVariable(eVK_Blob);
				PA_SetBlobVariable(&PAVTWAINImageBlob, buffer, filelen);
				PA_ResizeArray(&PAVBlobArray, indexInArray);
				PA_SetBlobInArray(PAVBlobArray, indexInArray, PAVTWAINImageBlob.uValue.fBlob);

				// Free memory				
				free(buffer);

				// Delete the temp file
				DeleteFile(WCFileNameToBlob);
			}

		}
	}
	
	if (TWAINCapture.filePath) {
		free(TWAINCapture.filePath);
		TWAINCapture.filePath = NULL;
	}

	PA_SetVariableParameter(params, 2, PAVBlobArray, 0);
	PA_ReturnLong(params, PALReturnValue);

}




// REB 2/26/13 #35165 Intermediary method we can call as a new thread.
unsigned __stdcall TWAIN_GetImage(void *arg)
{
	TWAIN_CAPTURE*	TWAINCapture;
	WCHAR			iterator[16] = L"";
	char			*pos = NULL;
	WCHAR *WCToCharInBuffer = NULL;
	BOOL			bContinue = TRUE;
	WCHAR			filePath[MAX_PATH] = L"";
	long			i = 0;

	TWAINCapture = (TWAIN_CAPTURE*) arg;
	// WJF 9/10/15 #43727 Removed
	//TWAIN_UnloadSourceManager();  // REB 2/26/13 #35165 We have to reset our source before trying to acquire an image.
	//TWAINCapture->DIBHandle = TWAIN_AcquireNative(NULL, TWAIN_ANYTYPE, &returnValue);

	TWAINCapture->returnValue = OrchTwain_Get(TWAINCapture->filePath, TWAINCapture->showUI, TWAINCapture->wiaMode, TWAINCapture->getMultiple); // WJF 9/10/15 #43727 // WJF 9/21/15 #43940

	if (TWAINCapture->getMultiple) {
		while (bContinue) {
			SetLastError(ERROR_SUCCESS);

			i++;

			wcscpy_s(filePath, MAX_PATH, TWAINCapture->filePath);

			WCToCharInBuffer = wcsrchr(filePath, L'.');

			_itow_s(i, iterator, 10);

			if (WCToCharInBuffer != NULL) {
				WCToCharInBuffer[0] = NULL;
			}

			wcscat_s(filePath, MAX_PATH, iterator);

			wcscat_s(filePath, MAX_PATH, L".bmp");

			if ((GetFileAttributes(filePath) == INVALID_FILE_ATTRIBUTES) || (GetLastError() == ERROR_FILE_NOT_FOUND)) {
				bContinue = FALSE;
				i--;
			}
		}

		TWAINCapture->numPictures = i;
	}

	TWAINCapture->done = TRUE;

	return 0;
}

//  FUNCTION:	OrchTwain_Get(LPCSTR filePath, BOOL Get64)
//
//  PURPOSE:	Launches the external OrchTwain DLL and waits for the operation to finish
//
//  COMMENTS:
//
//	DATE:		WJF 9/10/15 #43727
long __stdcall OrchTwain_Get(const wchar_t * filePath, BOOL ShowUI, BOOL IsWIA, BOOL GetMultiple) {
	long returnValue = 1;
	WCHAR *WCPluginPath, *WCToCharInBuffer;
	size_t bufferSize;
	WCHAR wcTWAINGetSource[MAX_PATH] = L"";

	WCHAR lpParameters[MAX_PATH_PLUS] = L"";
	char *pos = NULL;
	SHELLEXECUTEINFO utilities;
	DWORD dwExitCode = 0;
	BOOL bSuccess = FALSE;
	
	bufferSize = (25 + wcslen(wc4DXPath));
	WCPluginPath = (WCHAR*)malloc(sizeof(WCHAR) * bufferSize);

	wcscpy_s(WCPluginPath, bufferSize, wc4DXPath);

	WCToCharInBuffer = wcsrchr(WCPluginPath, L'\\');
	if (WCToCharInBuffer != NULL) {
		WCToCharInBuffer[0] = NULL;
	}

	wcscat_s(WCPluginPath, bufferSize, L"\\Orchard_Utilities.exe");

	wcscpy_s(wcTWAINGetSource, MAX_PATH, wcTwainSource);

	// WJF 9/21/15 #43940
	if (wcTWAINGetSource) {
		WCToCharInBuffer = NULL;
		WCToCharInBuffer = wcsrchr(wcTWAINGetSource, L'-');

		if (WCToCharInBuffer != NULL) {
			if (wcscmp(WCToCharInBuffer, L"-WIA") == 0) {
				IsWIA = TRUE;
			}

			WCToCharInBuffer[0] = NULL;
		}
	}

	if (IsWIA) { // WJF 9/21/15 #43940
		wcscpy_s(lpParameters, MAX_PATH_PLUS, L"-wa ");		
	}
	else {
		wcscpy_s(lpParameters, MAX_PATH_PLUS, L"-A ");
	}
	
	wcscat_s(lpParameters, MAX_PATH_PLUS, filePath);
	
	if (GetMultiple) {
		wcscat_s(lpParameters, MAX_PATH_PLUS, L" 1");
	}
	else {
		wcscat_s(lpParameters, MAX_PATH_PLUS, L" 0");
	}

	if (ShowUI) {
		wcscat_s(lpParameters, MAX_PATH_PLUS, L" 1 ");
	}
	else {
		wcscat_s(lpParameters, MAX_PATH_PLUS, L" 0 ");
	}

	if (wcTWAINGetSource) {
		if (wcscmp(wcTWAINGetSource, L"") != 0) {
			wcscat_s(lpParameters, MAX_PATH_PLUS, L"\"");
			wcscat_s(lpParameters, MAX_PATH_PLUS, wcTWAINGetSource);
			wcscat_s(lpParameters, MAX_PATH_PLUS, L"\"");
		}
	}

	// WJF 3/29/16 Win-11
	utilities.cbSize = sizeof(SHELLEXECUTEINFO);
	utilities.fMask = SEE_MASK_NOCLOSEPROCESS;
	utilities.hwnd = (HWND)PA_GetMainWindowHWND();
	utilities.lpVerb = NULL;
	utilities.lpFile = WCPluginPath;
	utilities.lpParameters = lpParameters;
	utilities.lpDirectory = NULL;
	utilities.nShow = SW_SHOW;
	utilities.hInstApp = NULL;
	ShellExecuteEx(&utilities);

	// WJF 3/29/16 Win-11 Wait until the process closes
	do {
		Sleep(100);
		bSuccess = GetExitCodeProcess(utilities.hProcess, &dwExitCode);
	} while ((dwExitCode == STILL_ACTIVE) && (bSuccess));

	free(WCPluginPath);

	returnValue = 0;

	return returnValue;
}
