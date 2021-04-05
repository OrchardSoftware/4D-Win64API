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
//char pathName[512]; // MWD 10/21/05 #9246 holds path to Win32API.4DX

// MWD 10/21/05 #9246
// Use the DllMain function to get the path to the calling DLL and store it in a global for further use.
#if WIN32
#ifdef _cplusplus
extern "C" __declspec(dllexport) {
#endif
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
	{
		BOOL result = TRUE;
		//char fullpath[512] = "";
		GetModuleFileName(hinstDLL, wc4DXPath, MAXBUF); // Get the path to the 4DX
		//strcpy_s((char *)pathName, sizeof(pathName), (char *)fullpath);  // ZRW 3/22/17 WIN-39 strcpy -> strcpy_s
		return result;
	}
#ifdef _cplusplus
}
#endif
#endif

// Note, this does not need to use the semaphore since it is already waiting on a file
void TWAIN_GetSources(PA_PluginParameters params)
{
	PA_Unistring PAUAppPath;
	PA_Variable PAVSources;
	PA_long32 PALReturnValue, PALDoNotAddSuffix, PALFlags;
	
	size_t bufferSize;
	WCHAR *WCPluginPath;
	WCHAR filePath[MAX_PATH] = L"", lpParameters[16] = L"-S", source[256] = L"";
	SHELLEXECUTEINFO utilities;
	DWORD dwExitCode = 0, index = 1;
	BOOL bSuccess = FALSE;
	FILE *fp = NULL;
	//LONG				returnValue, debug; // WJF 6/30/21 Win-21 LONG_PTR -> LONG
	//DWORD				index = 1;
	//PA_Variable			atSources;
	//// TW_IDENTITY			NewSourceId; // WJF 9/14/15 #43727 Removed
	//char				lpParameters[16] = "-S"; // WJF 9/21/15 #43940 3 -> 16
	//char				filePath[MAX_PATH] = "";
	//BOOL				get64 = FALSE;
	//FILE				*fp = NULL;
	//char				source[256] = "";
	//char				pluginPath[MAX_PATH] = "";
	//char				*pos = NULL;
	//SHELLEXECUTEINFO	utilities;
	//DWORD				dwExitCode = 0;
	//BOOL				bSuccess = FALSE;
	//BOOL				bDoNotAddSuffix = FALSE; // WJF 10/27/16 Win-41
	//LONG				flags = 0x0000; // SDL 10/3/17 WIN-51 "optional" parameter. Include WIA by default and create room for future flags // SDL 1/31/18 H-9141 UNLONG -> LONG

	PAVSources = PA_GetVariableParameter(params, 1);
	PA_ResizeArray(&PAVSources, 0);

	PALDoNotAddSuffix = PA_GetLongParameter(params, 2); // WJF 10/27/16 Win-41

	PALFlags = PA_GetLongParameter(params, 3); // SDL 10/3/17 WIN-51 

	PALReturnValue = 1;



	
	//// WJF 9/11/15 #43727 Begin changes

	bufferSize = (25 + wcslen(wc4DXPath));
	WCPluginPath = (WCHAR*)malloc(sizeof(WCHAR) * bufferSize);

	wcscpy_s(WCPluginPath, bufferSize, wc4DXPath);

	WCHAR* pos;
	pos = wcsrchr(WCPluginPath, L'\\');
	// pos = L'\0'; // THIS ISN'T DOING ANYTHING
	// pos = NULL;

	WCPluginPath[wcslen(WCPluginPath) - wcslen(pos)] = L'\0';

	wcscat_s(WCPluginPath, bufferSize, L"\\Orchard_Utilities.exe");

	GetTempPath(MAX_PATH, filePath);
	wcscat_s(filePath, MAX_PATH, L"twainSources.txt");

	utilities.cbSize = sizeof(SHELLEXECUTEINFO);
	utilities.fMask = SEE_MASK_NOCLOSEPROCESS;
	utilities.hInstApp = NULL;
	utilities.hwnd = (HWND)PA_GetMainWindowHWND(); //windowHandles.fourDhWnd;
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
					pos = wcsrchr(source, L'\n');
					source[wcslen(source) - wcslen(pos)] = L'\0';
					//pos = strrchr(source, '\n');
					//strcpy_s(pos, MAX_PATH, "\0");  // ZRW 3/22/17 WIN-39 256 -> MAX_PATH

					if (!PALDoNotAddSuffix) // WJF 10/27/16 Win-41 Do not add suffix if this is TRUE
					{
						wcscat_s(source, 256, L"-TWAIN"); // WJF 9/21/15 #43940  // ZRW 4/5/17 WIN-39 256 -> sizeof(source)
					}

					PA_ResizeArray(&PAVSources, index);

					PA_Unistring paSource = PA_CreateUnistring((PA_Unichar*)source);
					PA_SetStringInArray(PAVSources, index, &paSource);

					//PA_SetTextInArray(atSources, index, source, strlen(source));

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
		//strcat_s(filePath, sizeof(filePath), "wiaSources.txt");  // ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(filePath)

		wcscpy_s(lpParameters, 16, L"-ws");
		// strcpy_s(lpParameters, sizeof(lpParameters), "-ws");  // ZRW 3/22/17 WIN-39 16 -> sizeof(lpParameters)

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
						pos = wcsrchr(source, L'\n');
						source[wcslen(source) - wcslen(pos)] = L'\0';
						//wcscpy_s(pos, MAX_PATH, L'\0');


						//pos = strrchr(source, '\n');
						//strcpy_s(pos, MAX_PATH, "\0");  // ZRW 3/22/17 WIN-39 256 -> MAX_PATH

						if (!PALDoNotAddSuffix) // WJF 10/27/16 Win-41 Do not add suffix if this is TRUE
						{
							//wcscat_s(source, MAX_PATH, L"-WIA");
							wcscat_s(source, 256, L"-WIA");
							
							//strcat_s(source, sizeof(source), "-WIA");  // ZRW 4/5/17 WIN-39 256 -> sizeof(source)
						}


						PA_ResizeArray(&PAVSources, index);

						PA_Unistring paSource = PA_CreateUnistring((PA_Unichar*)source);
						PA_SetStringInArray(PAVSources, index, &paSource);

						//PA_ResizeArray(&atSources, index);
						//PA_SetTextInArray(atSources, index, source, strlen(source));

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

	PA_SetVariableParameter(params, 1, PAVSources, 0);
	PA_ReturnLong(params, PALReturnValue);
}
