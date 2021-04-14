

#include "Logging.h"

PA_Unistring* PAULogPath;
PA_long32 PALNumDays = 0;

BOOL bLogIsOpen = FALSE;
HANDLE hLogFile = NULL;
WCHAR *logFilePath, *dateOpened;

//  FUNCTION:   sys_LoggingStart(PA_PluginParameters params)
//
//  PURPOSE:	Start logging Win64API commands
//
//  COMMENTS:
//
//	DATE:		7/11/16 Win-20
void sys_LoggingStart(PA_PluginParameters params) {
	PA_long32 PALReturnValue = 0; // WJF 7/11/16 Win-20
		
	PAULogPath = PA_GetStringParameter(params, 1);
	PALNumDays = PA_GetLongParameter(params, 2);

	PALReturnValue = logOpenFile();

	PA_ReturnLong(params, PALReturnValue); // WJF 7/11/16 Win-20
}

//  FUNCTION:   sys_LoggingStop(PA_PluginParameters params)
//
//  PURPOSE:	Stop logging Win64API commands
//
//  COMMENTS:
//
//	DATE:		WJF 7/11/16 Win-20
void sys_LoggingStop(PA_PluginParameters params) {
	PA_ReturnLong(params, logCloseFile()); // WJF 7/11/16 Win-20 Return long and moved to common method
}

void writeLogFile(WCHAR * strLog)
{
	DWORD logLength, dwPos = 0, dwBytesWritten = 0;
	SYSTEMTIME lt; // WJF 7/11/16 Win-20 localTime -> lt
	WCHAR *dateComp, *szOutput;

	if (bLogIsOpen) { // WJF 7/8/16 Win-20 Don't bother doing this if the log isn't open
		GetLocalTime(&lt);

		dateComp = (WCHAR*)malloc(sizeof(WCHAR) * (17));
		swprintf_s(dateComp, 16, L"%04u%02u%02u", lt.wYear, lt.wMonth, lt.wDay);

		// WJF 7/11/16 Win-20 If the current date doesn't match the date of the open log file, close it and make a new one
		if (wcscmp(dateComp, dateOpened) != 0) {
			logCloseFile();
			logOpenFile();
		}

		if (bLogIsOpen) { // WJF 7/11/16 Win-20 This might change if the day rolled over
						  // MM/DD/YYYY H:M:S:MS - 
						  // WJF 7/11/16 Win-20 Rewrote to be simpler

			size_t bufferSize = 129 + wcslen(strLog);

			szOutput = (WCHAR*)malloc(sizeof(WCHAR) * (bufferSize));
			swprintf_s(szOutput, bufferSize, L"%02u/%02u/%04u %02u:%02u:%02u:%03u - ", lt.wMonth, lt.wDay, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
			
			wcscat_s(szOutput, bufferSize, strLog);  // ZRW 4/5/17 WIN-39 strcat -> strcat_s

			logLength = (DWORD) wcslen(szOutput) * sizeof(WCHAR); // WJF 6/30/16 Win-21 Cast to DWORD
			
			dwPos = SetFilePointer(hLogFile, 0, NULL, FILE_END);
			LockFile(hLogFile, dwPos, 0, logLength, 0);
			WriteFile(hLogFile, szOutput, logLength, &dwBytesWritten, NULL);
			UnlockFile(hLogFile, dwPos, 0, logLength, 0);

			free(szOutput);
		}

		free(dateComp);
	}
}


//  FUNCTION:   logMaintenance(PA_PluginParameters params)
//
//  PURPOSE:	Deletes log files older than the specified number of days
//
//  COMMENTS:
//
//	DATE:		WJF 6/17/16 Win-18
//  WJF 7/8/16 Win-20 Now returns the number of deleted files
PA_long32 logMaintenance() {
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	FILETIME ftCurrent;
	FILETIME ftFile;
	ULONGLONG qwCurrent = 0;
	ULONGLONG qwFile = 0;
	ULONGLONG qwResult = 0;
	WCHAR *deletePath;
	PA_long32 lNumDeleted = 0;

	size_t bufferSize = PAULogPath->fLength + 4;

	// Adding in additional space for  "*.*"
	WCHAR *searchPath = (WCHAR*) malloc(sizeof(WCHAR) * (bufferSize));

	// WJF 7/11/16 Win-20 parameter -> global  
	// ZRW 3/23/17 WIN-39 MAX_PATH -> sizeof(searchPath)
	// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
	wcscpy_s(searchPath, bufferSize, (WCHAR*) PAULogPath->fString);
	
	// ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(searchPath)
	// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
	wcscat_s(searchPath, bufferSize, L"*.*");

	GetSystemTimeAsFileTime(&ftCurrent);

	qwCurrent = (((ULONGLONG)ftCurrent.dwHighDateTime) << 32) + ftCurrent.dwLowDateTime;

	hFind = FindFirstFile(searchPath, &ffd);

	if (hFind == INVALID_HANDLE_VALUE) {
		return lNumDeleted;
	}

	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			ftFile = ffd.ftCreationTime;
			qwFile = (((ULONGLONG)ftFile.dwHighDateTime) << 32) + ftFile.dwLowDateTime;
			qwResult = qwCurrent - qwFile;

			// Delete files older than the specified number of days
			if (qwResult > (PALNumDays * _DAY)) { // WJF 7/8/16 Win-20 30 -> lNumDays, >= -> >
				bufferSize = PAULogPath->fLength + 1 + wcslen(ffd.cFileName);
				deletePath = (WCHAR*) malloc(sizeof(WCHAR) * (bufferSize));
												
				// ZRW 3/23/17 WIN-39 MAX_PATH -> sizeof(deletePath)
				// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
				wcscpy_s(deletePath, bufferSize, (WCHAR*)PAULogPath->fString);

				// ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(deletePath)
				// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
				wcscat_s(deletePath, bufferSize, ffd.cFileName);
				if (DeleteFile(deletePath)) {
					lNumDeleted++; // WJF 7/11/16 Win-20
				}

				free(deletePath);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	free(searchPath);

	return lNumDeleted;
}

// WJF 7/11/16 Win-20 Internal function so that we can switch files when the day rolls over
PA_long32 logOpenFile() {
	SYSTEMTIME lt;

	PA_long32 PALReturnValue = 0;

	if (PAULogPath != NULL) {
		// Adding in additional space for "Win64API_", the date, and ".log"
		size_t bufferSize = PAULogPath->fLength + 30;
	
		logFilePath = (WCHAR*) malloc(sizeof(WCHAR) * (bufferSize));

		// ZRW 3/23/17 WIN-39 strcpy -> strcpy_s 
		// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
		wcscpy_s (logFilePath, bufferSize, (WCHAR*) PAULogPath->fString);
	
		CreateDirectory(logFilePath, NULL);

		if (PALNumDays > 0) {
			logMaintenance();
		}

		wcscat_s(logFilePath, bufferSize, L"Win64API_"); // ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(logFilePath)

		GetLocalTime(&lt);

		dateOpened = (WCHAR*)malloc(sizeof(WCHAR) * 16);
		swprintf_s(dateOpened, 16, L"%04u%02u%02u", lt.wYear, lt.wMonth, lt.wDay);

		wcscat_s(logFilePath, bufferSize, dateOpened);  // ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(logFilePath)

		wcscat_s(logFilePath, bufferSize, L".log");  // ZRW 4/5/17 WIN-39 MAX_PATH -> sizeof(logFilePath)

		hLogFile = CreateFile(logFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hLogFile != INVALID_HANDLE_VALUE) {
			bLogIsOpen = TRUE;
			PALReturnValue = 1; // WJF 7/11/16 Win-20
		}		
	}

	return PALReturnValue;
}


// WJF 7/11/16 Win-20 Internal function so that we can switch files when the day rolls over
PA_long32 logCloseFile() {
	PA_long32 lReturnValue = -1;

	if (hLogFile != INVALID_HANDLE_VALUE) {
		if (CloseHandle(hLogFile)) {
			lReturnValue = 0;
			bLogIsOpen = FALSE;
		}
	}
	else {
		lReturnValue = -2;
	}

	return lReturnValue;
}

