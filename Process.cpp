/* --------------------------------------------------------------------------------
#
#	Process.cpp
#	All of the methods related to process functions
#	Project : Win32API
#	author : jmiller
#	3/15/2021 WIN-96
#
# --------------------------------------------------------------------------------*/

#include "Process.h"

typedef struct 
{
	DWORD dwErrorCode;
	BOOL  bSuccess;
	BOOL  bAppFound;
} FindOrKillReturn;

// ------------------------------------------------
//
//  FUNCTION: RequestCloseWindow(hWND, lparam)
//
//  Callback method to post the WM_CLOSE message for
//  cleanly exiting an app
//
//  DATE: MWD 1/8/07 #5421
//
// ------------------------------------------------

BOOL CALLBACK RequestCloseWindow(HWND hWnd, LPARAM lParam)
{
	DWORD dwID;

	// Get the ID of the window that this was called on
	GetWindowThreadProcessId(hWnd, &dwID);

	// Make sure that it matches our PID
	// If it does, post a WM_CLOSE message
	// Posting this message to a window is the
	// same as hitting its close button
	if (dwID == (DWORD)lParam)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

// ------------------------------------------------
// 
//  FUNCTION: FindOrKillProcessByName (PA_PluginParameters params, BOOL bKillProcess, PA_Unistring* pauProcessName, BOOL bOnlyFristMatch, BOOL bRequestTermination)
//
//  PURPOSE:  Common method to either kill or find a running process with the given name.
//
//  BOOL bKillProcess - TRUE to kill the process - FALSE to only find the process
//  PA_Unistring* pauProcessName - Name of the process to kill or find
//  BOOL bOnlyFristMatch - Used only when killing process - TRUE it will only kill first match found - FALSE will kill all matches found.
//  BOOL bRequestTermination - Used only when killing process - TRUE it will attempt graceful termination before forced termination - FALSE it will force kill the process.
//
//	DATE: JEM 3/17/21 WIN-96/WIN-77
//
//-------------------------------------------------
FindOrKillReturn FindOrKillProcessByName (BOOL bKillProcess, PA_Unistring* pauProcessName, BOOL bOnlyFristMatch, BOOL bRequestTermination)
{			
	HANDLE hProcessSnap;            // Handle to the process snapshot
	PROCESSENTRY32 pe32ProcData;	// Process entry data
	BOOL bDone;						// Used to indicate when we are done with the processes
	DWORD dwLastError;

	FindOrKillReturn FOKReturn;

	// Initialize return structure
	FOKReturn.dwErrorCode = 0;   // Used to hold return error code from GetLastError > 0
	FOKReturn.bAppFound = FALSE; // Used to indicate that the application was found (at least once)
	FOKReturn.bSuccess = TRUE;

	// Take a snapshot of all processes in the system.
	do
	{
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// If we fail
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			// Check the last error
			dwLastError = GetLastError();

			// If the error code is not ERROR_BAD_LENGTH
			if (dwLastError != ERROR_BAD_LENGTH)
			{
				// Update return structure with error code, failure, and exit function.
				FOKReturn.dwErrorCode = dwLastError;
				FOKReturn.bSuccess = FALSE;
				CloseHandle(hProcessSnap);	// Must clean up the snapshot object!
				return (FOKReturn);
			}

			// If error code is ERROR_BAD_LENGTH, we should try again per documentation 
			//
			// TH32CS_SNAPMODULE32
			//	0x00000010
			//
			//	Includes all 32 - bit modules of the process specified in th32ProcessID in the 
			//  snapshot when called from a 64 - bit process.  This flag can be combined with 
			//  TH32CS_SNAPMODULE or TH32CS_SNAPALL.  If the function fails with ERROR_BAD_LENGTH, 
			//  retry the function until it succeeds.

		}

		// If we get to the while and hProcessSnap == INVALID_HANDLE_VALUE 
		// that means we received an ERROR_BAD_LENGTH error 
		// so we need to retry per documentation.
	} while (hProcessSnap == INVALID_HANDLE_VALUE);

	// Set the size of the structure before using it.
	pe32ProcData.dwSize = sizeof(PROCESSENTRY32);

	// Get the data about the first process from the snapshot
	if (!Process32First(hProcessSnap, &pe32ProcData))
	{
		// Update return structure with error code, failure, and exit function.
		FOKReturn.dwErrorCode = GetLastError();
		FOKReturn.bSuccess = FALSE;
		CloseHandle(hProcessSnap);	// Must clean up the snapshot object!
		return (FOKReturn);
	}

	// Default to FALSE
	bDone = FALSE;

	// Now walk the snapshot of processes
	do
	{
		// Check if the name matches
		if (wcscmp(pe32ProcData.szExeFile, (wchar_t*)pauProcessName->fString) == 0)
		{
			// The app name was found
			FOKReturn.bAppFound = TRUE;

			// If we are killing the process
			if (bKillProcess)
			{
				// Named matched now check if we can terminate the process
				HANDLE hProcess;                // Handle to the process itself
				BOOL bProcessTerminated;		// True if the process was terminated

												// First match has been found.  If we are only processing one file then
												// we want to exit the loop, if not we want to continue.
				bDone = bOnlyFristMatch;

				// Get the process
				hProcess = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pe32ProcData.th32ProcessID);

				// If we failed to get the process handle
				if (hProcess == NULL) {
					// Update return structure with error code, failure, and exit function.
					FOKReturn.dwErrorCode = GetLastError();
					FOKReturn.bSuccess = FALSE;
					CloseHandle(hProcessSnap);	// Must clean up the snapshot object!
					return (FOKReturn);
				}

				// Default to the process not being terminated
				bProcessTerminated = FALSE;

				// If we want to attempt a clean termination first
				if (bRequestTermination)
				{
					// RequestCloseWindow posts WM_CLOSE to all windows whose PID matches.
					EnumWindows((WNDENUMPROC)RequestCloseWindow, (LPARAM)pe32ProcData.th32ProcessID);

					// Make sure the process terminated - wait 500 milliseconds
					DWORD dwWaitReturn;
					dwWaitReturn = WaitForSingleObject(hProcess, 500);
					bProcessTerminated = (dwWaitReturn == WAIT_OBJECT_0);
				}

				// If the process was not terminated
				if (!bProcessTerminated)
				{
					// Force kill the process
					bProcessTerminated = TerminateProcess(hProcess, 1);

					if (!bProcessTerminated)
					{
						// Get the last error
						dwLastError = GetLastError();
					}
				}

				// Close our process handle
				CloseHandle(hProcess);

				// If the process was not terminated
				if (!bProcessTerminated)
				{
					// Update return structure with error code, failure, and exit function.
					FOKReturn.dwErrorCode = dwLastError;
					FOKReturn.bSuccess = FALSE;
					CloseHandle(hProcessSnap); // Must clean up the snapshot object!
					return (FOKReturn);
				}
			}
			else // else of if (bKillProcess)
			{
				// Only looking for process and it was found so we are done.
				bDone = TRUE;
			}
		} // end if (wcscmp(pe32ProcData.szExeFile, (wchar_t*) pauProcessName->fString) == 0)

		  // At this point either the filename did not match or the process was successfully terminated
		  //    Failure to terminate will exit method above.
		  // If we are not done, we need to get the next process's data
		if (!bDone)
		{
			// We have not found a match yet or we need to look at all processes
			// Get the next process
			if (!Process32Next(hProcessSnap, &pe32ProcData))
			{
				// Check the last error
				dwLastError = GetLastError();

				// We have reached the end of the list of processes
				if (dwLastError == ERROR_NO_MORE_FILES)
				{
					bDone = TRUE;
				}
				else
				{
					// Some other error occurred while trying to get next process

					// Update return structure with error code, failure, and exit function.
					FOKReturn.dwErrorCode = dwLastError;
					FOKReturn.bSuccess = FALSE;
					CloseHandle(hProcessSnap);	// Must clean up the snapshot object!
					return (FOKReturn);
				}
			}
		}

		// REB 11/30/11 Was "| bDone" which caused us to enter an infinite loop if we should stop after killing the first match.
		// Change to loop while not bDone - this allows capture of any errors that occur is Process32Next fails besides
	} while (!bDone);

	// Close the handle and return success
	CloseHandle(hProcessSnap);

	// For killing the process if we have reached this point,
	// we have either not found the process or we have successfully
	// kill the request process(es).  Either way no error has 
	// occurred and the process was either killed or didn't need
	// to be killed.
	//
	// For seeing if the app is loaded, we have successfully
	// transitioned the processes.  We may or may not have found
	// the application but no error has occurred.
	return(FOKReturn);
}

// ------------------------------------------------
// 
//  FUNCTION: sys_KillProcessByName(PA_PluginParameters params, BOOL bFindOnly) 
//
//  PURPOSE:  Kills a process or processes with the given name.
//
//  RETURN : 0 if successful or process not found.  Non-zero indicates failure.
//						     
//	DATE: MWD 11/30/06 #12572
//        JEM 3/15/21 WIN-96 Update to 64-bit and improved logic some.
//-------------------------------------------------

void sys_KillProcessByName(PA_PluginParameters params)
{
	PA_Unistring* pauProcessName;	// Unistring pointer to hold the processName param ($1)
	BOOL bOnlyFristMatch;			// LongInt32 (as Boolean) to hold the kill mode ($2)
										// 0 = all processes matching name
										// 1 = just first process matching name
	BOOL bRequestTermination;		// LongInt32 (as Boolean) to see if we should try to cleanly close the app before killing it mercilessly ($3)
										// 0 = force kill immediately
										// 1 = attempt graceful shutdown message then force kill
	FindOrKillReturn FOKReturn;

	pauProcessName = PA_GetStringParameter(params, 1);
	bOnlyFristMatch = (PA_GetLongParameter(params, 2) == 1);
	bRequestTermination = (PA_GetLongParameter(params, 3) == 1);

	// Call common method to kill the process
	FOKReturn = FindOrKillProcessByName(TRUE, pauProcessName, bOnlyFristMatch, bRequestTermination);

	// We should be able to just return the error code.  
	// If no error occurred, the value should be 0.
	// If an error occurred, the value should be > 0.
	PA_ReturnLong(params, (PA_long32) FOKReturn.dwErrorCode);
}

// ------------------------------------------------
// 
//  FUNCTION: sys_IsAppLoaded( PA_PluginParameters params )
//
//  PURPOSE: Check if a program is loaded
//
//  RETURN : -1 if process is not found.
//            0 if process is found.
//           >0 if error.
//        
//	DATE: dcc 08/23/02
//        JEM 3/17/21 WIN-77 Major update to modernize
// ------------------------------------------------

void sys_IsAppLoaded(PA_PluginParameters params)
{
	PA_Unistring* pauProcessName;	// Unistring pointer to hold the processName param ($1)
	FindOrKillReturn FOKReturn;

	pauProcessName = PA_GetStringParameter(params, 1);

	// Call common method to find the process
	FOKReturn = FindOrKillProcessByName(FALSE, pauProcessName, TRUE, TRUE);

	// If successful, return -1 if process was not found
	// If successful, return 0 if process was found
	// If not successful, return error code.
	if (FOKReturn.bSuccess)
	{
		if (FOKReturn.bAppFound)
		{
			PA_ReturnLong(params, 0);
		}
		else
		{
			PA_ReturnLong(params, -1);
		}
	}
	else
	{
		PA_ReturnLong(params, (PA_long32)FOKReturn.dwErrorCode);
	}
}

// ------------------------------------------------
// 
//  FUNCTION: sys_ShellExecute( PA_PluginParameters params)
//
//  PURPOSE:	Launch a program or open a folder
//
//  COMMENTS:	Pretty much a direct mapping to ShellExecute.  
//				Returns value >32 if successful.  <32 if error. - eh? This is returning text
//        
//	DATE:		   dcc 08/21/02
//
//	MODIFICATIONS: 10/28/02 replace forward slashes in a provided UNC with backslashes (3.5.2)
//				   11/21/01 made parameters longer for LONG_PTR urls etc (3.5.3)
//                 08/08/04 Removed the limit on the length of the parameters.
//                 JEM 3/18/21 WIN-84 Major update to modernize 
//
// ------------------------------------------------

void sys_ShellExecute(PA_PluginParameters params)
{
	PA_Unistring* pauOperation;	// Unistring pointer to hold the operation param ($1)
	PA_Unistring* pauFile;
	PA_Unistring* pauParameters;
	PA_Unistring* pauDirectory;
	PA_long32 palHowToShow;
	long long llResult;

	pauOperation = NULL;
	pauFile = NULL;
	pauParameters = NULL;
	pauDirectory = NULL;

	// Get the function parameters.
	pauOperation = PA_GetStringParameter(params, 1);
	pauFile = PA_GetStringParameter(params, 2);
	pauParameters = PA_GetStringParameter(params, 3);
	pauDirectory = PA_GetStringParameter(params, 4);
	palHowToShow = PA_GetLongParameter(params, 5);

	// Convert the operation to lowercase
	for (int i = 0; i < pauOperation->fLength; i++)
	{
		if (iswupper(pauOperation->fString[i]))
		{
			pauOperation->fString[i] = towlower(pauOperation->fString[i]);
		}
	}

	// The operation is not open, explore, or print or if the file is blank.
	if (((pauOperation->fLength == 0) ||
		((wcscmp(L"open", (wchar_t*)pauOperation->fString) != 0) &&
		(wcscmp(L"explore", (wchar_t*)pauOperation->fString) != 0) &&
			(wcscmp(L"print", (wchar_t*)pauOperation->fString) != 0))
		))
	{
		// Mark De Wever #12225 replaced the line above
		// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
		PA_ReturnString(params, (PA_Unichar *) L"Invalid Operation");
	}
	else if (pauFile->fLength == 0)
	{
		// JEM 3/18/21 WIN-84 Added for clarification this case used to be part of Invalid Operation
		PA_ReturnString(params, (PA_Unichar *) L"No File");
	}
	else if ((palHowToShow < 0) || (palHowToShow > 10)) // // JEM 3/18/21 WIN-84 This was just > 11.  Valid values per documentation are 0 - 10.
	{
		// Mark De Wever #12225 replaced the line above
		// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
		PA_ReturnString(params, (PA_Unichar *)L"Invalid HowToShow Constant");
	}
	else
	{
		// added 10/28/02 shellExecute wants backslashes
		// JEM 3/18/21 WIN-84 Removed code correcting incorrect directory separators in the pauFile and pauDirectory
		llResult = (long long) ShellExecute(NULL, (LPCWSTR) pauOperation->fString, (LPCWSTR) pauFile->fString, (LPCWSTR) pauParameters->fString, (LPCWSTR) pauDirectory->fString, palHowToShow);

		// Based documentation if the value returned is greater than 32 that means success
		if (llResult > 32)
		{ 
			PA_ReturnString(params, (PA_Unichar *)L"");
		}
		else
		{
			// error occurred
			switch (llResult)
			{
				case ERROR_FILE_NOT_FOUND:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"File Not Found");
					break;

				case ERROR_PATH_NOT_FOUND:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"Path Not Found");
					break;

				case ERROR_BAD_FORMAT:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L".EXE File is Invalid");
					break;

				case SE_ERR_ACCESSDENIED:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"OS Denied Access to Fil");
					break;

				case SE_ERR_ASSOCINCOMPLETE:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"File Name Association is Incomplete or Invalid");
					break;

				case SE_ERR_DDEBUSY:
				case SE_ERR_DDEFAIL:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"DDE Transaction Could Not be Completed");
					break;

				case SE_ERR_DDETIMEOUT:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"DDE Request Timed Out");
					break;

				case SE_ERR_DLLNOTFOUND:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"DLL Libray Not Found");
					break;

				case SE_ERR_NOASSOC:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"No Application Associated with File Extension");
					break;

				case SE_ERR_OOM:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"Insufficient Memory");
					break;

				case SE_ERR_SHARE:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"Sharing Violation Occurred");
					break;

				default:
					// Mark De Wever #12225 replaced the line above
					// JEM 3/18/21 WIN-84 Removed support for dutch language - just returning english
					PA_ReturnString(params, (PA_Unichar *)L"Unknown error occurred");
					break;
			}
		}
	}
}
