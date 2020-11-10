/* --------------------------------------------------------------------------------
#
#	WindowManagement.cpp
#	All of the methods related to window management functions
#	Project : Win64API
#	author : dwaddell
#	10/26/2020
#
# --------------------------------------------------------------------------------*/


#include "WindowManagement.h"

WINDOWHANDLES windowHandles;

// WJF 9/1/15 #43731 Handle Array Declarations
LONG_PTR handleArray[HANDLEARRAY_CAPACITY];

HANDLE hArrayMutex;


void initWindowManagement() {
	HWND NexthWnd;
	wchar_t wcClassName[255];

	// REB 11/2/12 #34333
	// ACW 10/28/20 WIN-107 Changed to use PA_GetMainWindowHWND which has been the standard in the SDK since version 16
	windowHandles.fourDhWnd = (HWND) PA_GetMainWindowHWND();
	//HWND myWindow = (HWND) PA_GetHWND (0);
	//PA_Unistring paAppPath;
	//paAppPath =  PA_GetApplicationFullPath();
	//wchar_t *charPos = wcsrchr((wchar_t*)paAppPath.fString, L'\\');
	//windowHandles.fourDhWnd = FindWindowEx(NULL, NULL, (LPCWSTR) paAppPath.fString, NULL);

	NexthWnd = GetWindow(windowHandles.fourDhWnd, GW_CHILD);
	do {
		if (IsWindow(NexthWnd)) {
			GetClassName(NexthWnd, wcClassName, 255);

			// ZRW 4/12/17 WIN-39 Modified to use _strlwr_s instead
			// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
			_wcslwr_s(wcClassName, 255);
			// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
			if (wcscmp(wcClassName, L"mdiclient") == 0) {
				// windowHandles.MDIs_4DhWnd =  NexthWnd; // AMS 8/12/14 #39693 This was not the correct handle to use for the MDI Client. It was causing toolbars to work incorrectly.
				windowHandles.MDIhWnd = NexthWnd; // AMS 8/12/14 #39693 This is the correct handle for the MDI Client.
				break;
			}
			NexthWnd = GetNextWindow(NexthWnd, GW_HWNDNEXT);
		}
	} while (IsWindow(NexthWnd));

	// REB 8/30/11 #28504 We already have this handle now.
	// REB 3/26/10 #22878 Get the correct child handle so toolbars work correctly. 
	// AMS 8/12/14 #39693 Uncommented this line. This line is still needed in order for tollbars to work correctly.
	windowHandles.MDIs_4DhWnd = GetWindow(windowHandles.MDIhWnd, GW_CHILD);

	// WJF 9/1/15 #43731
	handleArray_init();
}

//  FUNCTION:	gui_GetWindowEx (PA_PluginParameters params, HWND hWnd)
//
//  PURPOSE:	Finds a handle, adds it to the internal handle array, and returns the index
//
//  COMMENTS:
//
//	DATE:		WJF 9/15/15 #43731
void gui_GetWindowEx(PA_PluginParameters params)
{
	PA_Unistring* paWindowTitle;

	paWindowTitle = PA_GetStringParameter(params, 1);

	PA_long32 lReturnValue;
	HWND lReturnWindowHandle = NULL;

	lReturnValue = -1;

	if (paWindowTitle->fLength <= 0)  {
		// ACW 11/10/20 WIN-107 Changed windowHandles.MDIs_4DhWnd to windowHandles.fourDhWnd which is the main handle for the application; I am unsure why we were checking the 
		// value for MDIs_4DhWnd but returning fourDhWnd
		if (windowHandles.fourDhWnd != NULL) {
			lReturnWindowHandle = windowHandles.fourDhWnd;			
		}
		else {
			lReturnWindowHandle = getCurrentFrontmostWindow();
		}
	}
	else
	{
		if (wcscmp((wchar_t*)paWindowTitle->fString, L"*") == 0) { // return the frontmost window
			lReturnWindowHandle = getCurrentFrontmostWindow();
		}
		else {

			// ZRW 4/12/17 WIN-39 using the more secure _s method; using MAXBUF since windowTitle is a pointer
			// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
			_wcslwr_s((wchar_t*)paWindowTitle->fString, paWindowTitle->fLength + 1);

			// ZRW 4/12/17 WIN-39 _strlwr(windowTitle) -> windowTitle
			// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
			if ((wcscmp((wchar_t*)paWindowTitle->fString, L"mdi") == 0) && (windowHandles.MDIhWnd != NULL)) {
				lReturnWindowHandle = windowHandles.MDIhWnd;
			}
			else {
				lReturnWindowHandle = getWindowHandle(paWindowTitle, getCurrentFrontmostWindow());
			}

			if (!lReturnWindowHandle) {
				lReturnValue = -3;
			}
		}
	}

	if (lReturnWindowHandle) {
		lReturnValue = handleArray_add((LONG_PTR)lReturnWindowHandle);
	}		

	PA_ReturnLong(params, lReturnValue);
}


// ------------------------------------------------
//
//  FUNCTION: gui_setWindowTitleEx( PA_PluginParameters params )
//
void gui_setWindowTitleEx(PA_PluginParameters params) {
	PA_long32 paIndex, returnValue;
	PA_Unistring* paWindowTitle;
	HWND hWindow;

	paIndex = PA_GetLongParameter(params, 1); // WJF 9/1/15 #43731 We are now getting an index to an internal handle array
	paWindowTitle = PA_GetStringParameter(params, 2);
		
	 // WJF 9/16/15 #43731
	hWindow = handleArray_retrieve((DWORD)paIndex);

	if (IsWindow(hWindow)) {
		SetWindowText(hWindow, (LPCWSTR) paWindowTitle->fString);
		returnValue = 1;
	}
	else {
		returnValue = 0;
	}

	PA_ReturnLong(params, returnValue);
}


HWND getCurrentFrontmostWindow()
{
	HWND NexthWnd;
	wchar_t wcClassName[255];
	
	HWND hWnd = (HWND)PA_GetHWND(NULL); // The current frontmost window // WJF 6/24/16 Win-21 Casting to HWND

	if (!(IsWindow(hWnd))) {
		if (!(IsWindow(windowHandles.MDIs_4DhWnd))) {
			// ACW 10/27/20 WIN-107 Removed unused stuff
			// Unistring = PA_GetApplicationFullPath();
			// pathName = UnistringToCString(&Unistring);
			// charPos = strrchr(pathName, '\\');
			// *charPos = 0;

			// ITH 7/25/19 WIN-46 FindWindowEx(NULL, NULL, pathName, NULL)->PA_GetMainWindowHWND(). Updated SDK to version 17.0.1
			// ACW 10/28/20 WIN-107 Commented out; we did this on startup in initWindowManagement, it's the first thing set in windowHandles
			// windowHandles.fourDhWnd = (HWND) PA_GetMainWindowHWND(); 

			// free(pathName);

			NexthWnd = GetWindow(windowHandles.fourDhWnd, GW_CHILD);
			do {
				if (IsWindow(NexthWnd)) {
					GetClassName(NexthWnd, wcClassName, 255);

					// ZRW 4/12/17 WIN-39 Modified to use _strlwr_s instead
					// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
					_wcslwr_s(wcClassName, 255);

					// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
					if (wcscmp(wcClassName, L"mdiclient") == 0) {
						windowHandles.MDIs_4DhWnd = NexthWnd;
						break;
					}
					NexthWnd = GetNextWindow(NexthWnd, GW_HWNDNEXT);
				}
			} while (IsWindow(NexthWnd));
		}
		hWnd = windowHandles.MDIs_4DhWnd;
	}

	return hWnd;
}


HWND getWindowHandle(PA_Unistring* paWindowTitle, HWND hWnd)
{
	HWND MDIhWnd, MainhWnd, ChildhWnd, NexthWnd, returnValue = 0;
	wchar_t wcClassName[255];
	wchar_t wcWindowName[255];

	// ZRW 4/12/17 WIN-39 Added here since we only need to do this once; Using MAXBUF since windowTitle is a pointer
	// ACW 10/27/20 WIN-107 Changed from _strlwr_s to _wcslwr_s; Was: _strlwr_s(windowTitle, MAXBUF);  	
	_wcslwr_s((wchar_t*)paWindowTitle->fString, paWindowTitle->fLength + 1);

	if (IsWindow(hWnd)) {
		NexthWnd = hWnd;		
		GetClassName(NexthWnd, wcClassName, 255);

		do {

			// ZRW 4/12/17 WIN-39 
			// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
			_wcslwr_s(wcClassName, 255);
			
			// ZRW 4/12/17 WIN-39 _strlwr(szClassName) -> szClassName
			// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
			if (wcscmp(wcClassName, L"mdiclient") == 0) {  
				windowHandles.MDIhWnd = NexthWnd;
				MDIhWnd = NexthWnd;

				// ZRW 4/12/17 WIN-39 _strlwr(windowTitle) -> windowTitle
				// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
				if (wcscmp((wchar_t*)paWindowTitle->fString, L"mdi") == 0) {
					return NexthWnd;
				}
				else {
					break;
				}
			}
			MDIhWnd = NexthWnd;
			NexthWnd = GetParent(MDIhWnd);
			GetClassName(NexthWnd, wcClassName, 255);
		} while (NexthWnd != NULL);
	}
	else {
		returnValue = 0;
		return returnValue;
	}

	if (IsWindow(MDIhWnd)) {
		// Now if the caller wants the main window the
		// windowTitle will be blank
		// ACW 10/27/20 WIN-107 Changed from "if (windowTitle_len == 0) {"
		if (paWindowTitle->fLength == 0) {
			// Get the parent of the MDI Window
			// REB 4/14/11 #25290 If we already have the main window handle make sure
			// we use it.
			if (IsWindow(GetParent(MDIhWnd))) {
				MainhWnd = GetParent(MDIhWnd);
			}
			else {
				MainhWnd = MDIhWnd;
			}

			if (IsWindow(MainhWnd)) {
				returnValue = MainhWnd;
				return returnValue;
			}
			else {
				returnValue = 0;
				return returnValue;
			}
		}
		else {
			// Search all the child windows for the window
			// with a Title matching windowTitle
			NexthWnd = GetWindow(MDIhWnd, GW_CHILD);
			do {
				ChildhWnd = NexthWnd;
				if (IsChild(MDIhWnd, ChildhWnd)) {
					GetWindowText(ChildhWnd, wcWindowName, 255);
					GetClassName(ChildhWnd, wcClassName, 255);

					// SDL 8/25/17 WIN-39
					// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
					_wcslwr_s(wcWindowName, 255);

					// ZRW 4/12/17 WIN-39
					// ACW 10/27/20 WIN-107 Changed _strlwr_s to _wcslwr_s
					_wcslwr_s(wcClassName, 255);  

					// ZRW 4/12/17 WIN-39 _strlwr(szClassName) -> szClassName, _strlwr(windowTitle) -> windowTitle
					// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
					if ((wcscmp(wcClassName, L"mdiclient") == 0) && (wcscmp((wchar_t*)paWindowTitle->fString, L"mdi") == 0)) {
						return ChildhWnd;
					}
					NexthWnd = GetNextWindow(ChildhWnd, GW_HWNDNEXT);
				}
				else {
					returnValue = 0;
					return returnValue;
				}

				// ZRW 4/12/17 WIN-39 _strlwr(WindowName) -> WindowName, _strlwr(windowTitle) -> windowTitle
				// ACW 10/27/20 WIN-107 Changed strcmp to wcscmp
			} while (wcscmp((wchar_t*)paWindowTitle->fString, wcWindowName) != 0);
															 // Match found
			returnValue = ChildhWnd;
		}
	}
	else {
		returnValue = 0;
	}
	return returnValue;
}

//  FUNCTION:	handleArray_init ()
//
//  PURPOSE:	Initializes the internal handle array and its mutex object
//
//  COMMENTS:
//
//	DATE:		WJF 9/1/15 #43731
DWORD handleArray_init() {
	for (int i = 0; i < HANDLEARRAY_CAPACITY; i++)
		handleArray[i] = 0;

	hArrayMutex = CreateMutex(NULL, FALSE, NULL);

	if (hArrayMutex == NULL) {
		return GetLastError();
	}
	else {
		return ERROR_SUCCESS;
	}
}

//  FUNCTION:	handleArray_add (LONG_PTR hWND)
//
//  PURPOSE:	Adds a handle to the internal handle array
//
//  COMMENTS:
//
//	DATE:		WJF 9/1/15 #43731
//  WJF 6/24/16 Win-21 DWORD -> INT
INT handleArray_add(LONG_PTR hWND) {
	int i = 0;
	BOOL hasEmptySlot = FALSE;
	DWORD dwResult = 0;

	// Wait for the mutex
	dwResult = WaitForSingleObject(hArrayMutex, 2000);

	if (dwResult == WAIT_OBJECT_0) {
		__try {
			// Find first empty slot
			while (i < HANDLEARRAY_CAPACITY) {
				if (handleArray[i] == 0) {
					hasEmptySlot = TRUE;
					break;
				}
				else {
					i++;
				}
			}

			if (hasEmptySlot) {
				handleArray[i] = hWND;
			}
		}
		__finally {
			ReleaseMutex(hArrayMutex);
		}

		if (hasEmptySlot) {
			return i;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

//  FUNCTION:	handleArray_retrieve (DWORD handleIndex)
//
//  PURPOSE:	Common method to return a handle from the handleArray
//
//  COMMENTS:
//
//	DATE:		WJF 9/16/15 #43731
HWND handleArray_retrieve(DWORD handleIndex) {
	LONG_PTR handle = 0;

	if ((handleIndex >= 0) && (handleIndex < HANDLEARRAY_CAPACITY)) {
		handle = handleArray[handleIndex];
	}

	return (HWND)handle;
}
