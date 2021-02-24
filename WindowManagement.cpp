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

WNDPROC	g_wpOrigMDIProc;

namespace MDI
{
	HWND windowRef = NULL;

	HWND getWindowHWND(PA_long32 windowId)
	{
		if (windowId == (PA_long32) Win64_MDI_WinRef)
		{
			return windowRef;
		}
		else
		{
			return (HWND) PA_GetHWND((PA_WindowRef) windowId);
		}
	}
}

// ACW 1/8/21 WIN-113
std::unordered_map <PA_Unistring*, HICON> umIcons;

void initWindowManagement() {
	MDI::windowRef = (HWND) PA_GetMainWindowHWND();
}


// ------------------------------------------------
//
//  FUNCTION: gui_setWindowTitleEx( PA_PluginParameters params )
//
void gui_SetWindowTitleEx(PA_PluginParameters params) {
	PA_long32 paWinRef, returnValue;
	PA_Unistring* paWindowTitle;
	
	// WJF 9/1/15 #43731 We are now getting an index to an internal handle array
	// ACW 1/6/21 WIN-108 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	paWinRef = PA_GetLongParameter(params, 1); 
	paWindowTitle = PA_GetStringParameter(params, 2);

	HWND hWindow = MDI::getWindowHWND(paWinRef);

	if (IsWindow(hWindow)) {
	    SetWindowText(hWindow, (LPCWSTR) paWindowTitle->fString);
		returnValue = 1;
	}
	else {
		returnValue = 0;
	}

	PA_ReturnLong(params, returnValue);
}

// ------------------------------------------------
//
//  FUNCTION: gui_SetIconEx( PA_PluginParameters params )
//
void gui_SetIconEx(PA_PluginParameters params)
{
	PA_long32 PAL4DWinRef, PALReturnValue;
	PA_Unistring* PAUIconPath;
	HICON hIcon;

	// WJF 9/1/15 #43731 We are now getting indexes to an internal array
	// ACW 1/7/21 WIN-113 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	PAUIconPath = PA_GetStringParameter(params, 2);

	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);

	// Check to see if we already loaded this image; if we have it in memory already let's use it.
	std::_List_iterator<std::_List_val<std::_List_simple_types<std::pair<PA_Unistring *const, HICON>>>> search = umIcons.find(PAUIconPath);
	if (search != umIcons.end()) {
		hIcon = search->second;
	}
	else {
		hIcon = (HICON)LoadImage(0, (LPCWSTR)PAUIconPath->fString, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		umIcons.insert({ PAUIconPath , hIcon });
	}
	
	if (IsWindow(hWindow)) {
		// Tell the window to use this icon
		SendMessage(hWindow, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
		PALReturnValue = 1;
	}
	else {
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}

// ------------------------------------------------
//
//  FUNCTION: gui_GetWindowState( PA_PluginParameters params)
//
//  PURPOSE:	Determine if window is minimized or maximized
//
//  COMMENTS:	Returns 0 if window is normal, 1 if minimized
//						and 2 if maximized.
//
//	DATE:			dcc 07/20/02 dcc
//
//	MODIFICATIONS:
//
void gui_GetWindowStateEx (PA_PluginParameters params)
{
	PA_long32 PAL4DWinRef, PALReturnValue;
	
	PALReturnValue = 0; // Not minimized or maximized

	// WJF 9/1/15 #43731 We are now getting an index to an internal array
	// ACW 1/11/21 WIN-109 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	
	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);

	if (IsWindow(hWindow)) {
		if (IsIconic(hWindow)) {
			PALReturnValue = IS_ICONIC; // Minimized
		}
		else if (IsZoomed(hWindow)) {
			PALReturnValue = IS_ZOOMED; // Maximized
		}
	}

	PA_ReturnLong(params, PALReturnValue);
}

//  FUNCTION:	gui_TakeScreenshotEx (PA_PluginParameters params)
//
//  PURPOSE:	Takes a screenshot of the desktop
//
//  COMMENTS:
//
//	DATE:		WJF 7/7/15 #43138
void gui_TakeScreenshotEx(PA_PluginParameters params) {
	PA_long32 PAL4DWinRef, PALReturnValue;
	PA_Unistring* PAUScreenshotPath;

	LPVOID lpbitmap;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	DWORD dwBmpSize, dwBytesWritten, dwSizeofDIB;
	HANDLE hFile, hDIB;
	RECT rcClient;

	// WJF 9/1/15 #43731 We are now getting an index to an internal array;
	// ACW 1/11/21 WIN-109 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	PAUScreenshotPath = PA_GetStringParameter(params, 2);
	
	PALReturnValue = 0;

	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);
	
	if (IsWindow(hWindow)) {
		// Get a screen DC and a DC for the window for which the handle was provided
		HDC hdcWindow = GetDC(hWindow);

		// Create a compatible DC which is used in a BitBlt from the window DC
		HDC hdcMemDC = CreateCompatibleDC(hdcWindow);

		if (hdcMemDC) {
			// Get the client area for size calculation
			GetClientRect(hWindow, &rcClient);

			// Adjust for caption bar and borders
			rcClient.top -= GetSystemMetrics(SM_CYCAPTION);
			rcClient.bottom += 5;
			rcClient.left -= 5;
			rcClient.right += 5;

			// Create a compatible bitmap from the Window DC
			HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

			if (hbmScreen) {
				// Select the compatible bitmap into the compatible memory DC.
				SelectObject(hdcMemDC, hbmScreen);

				// Bit block transfer into our compatible memory DC.
				if (BitBlt(hdcMemDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcWindow, rcClient.left, rcClient.top, SRCCOPY)) {
					// Get the BITMAP from the HBITMAP
					GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

					bi.biSize = sizeof(BITMAPINFOHEADER);
					bi.biWidth = bmpScreen.bmWidth;
					bi.biHeight = bmpScreen.bmHeight;
					bi.biPlanes = 1;
					bi.biBitCount = 32;
					bi.biCompression = BI_RGB;
					bi.biSizeImage = 0;
					bi.biXPelsPerMeter = 0;
					bi.biYPelsPerMeter = 0;
					bi.biClrUsed = 0;
					bi.biClrImportant = 0;

					dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

					// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
					// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
					// have greater overhead than HeapAlloc.
					hDIB = GlobalAlloc(GHND, dwBmpSize);
					lpbitmap = GlobalLock(hDIB);

					// Gets the "bits" from the bitmap and copies them into a buffer
					// which is pointed to by lpbitmap.
					GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

					// A file is created, this is where we will save the screen capture.
					hFile = CreateFile((LPCWSTR) PAUScreenshotPath->fString, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					// ACW 1/12/21 WIN-111 Check to see if the handle was valid; the user may have passed in an invalid path
					if (hFile == INVALID_HANDLE_VALUE) {
						PALReturnValue = 5;
					}
					else {
						// Add the size of the headers to the size of the bitmap to get the total file size
						dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

						// Offset to where the actual bitmap bits start.
						bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

						// Size of the file
						bmfHeader.bfSize = dwSizeofDIB;

						// bfType must always be BM for Bitmaps
						bmfHeader.bfType = 0x4D42; //BM

						dwBytesWritten = 0;
						WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
						WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
						WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
					}

					// Unlock and Free the DIB from the heap
					GlobalUnlock(hDIB);
					GlobalFree(hDIB);
					
					// Close the handle for the file that was created
					CloseHandle(hFile);
				}
				else {
					PALReturnValue = 3;
				}

				DeleteObject(hbmScreen);
			}
			else {
				PALReturnValue = 2;
			}

			DeleteObject(hdcMemDC);
		}
		else {
			PALReturnValue = 1;
		}

		ReleaseDC(hWindow, hdcWindow);
	}
	else {
		PALReturnValue = 4;
	}

	PA_ReturnLong(params, PALReturnValue);
}


// ------------------------------------------------
//
//  FUNCTION: gui_ShowWindowEx ( PA_PluginParameters params )
//

void gui_ShowWindowEx (PA_PluginParameters params)
{
	PA_long32 PAL4DWinRef, PALShowState, PALReturnValue;

	// WJF 9/1/15 #43731 We are now getting an index to an internal array
	// ACW 1/11/21 WIN-109 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	PALShowState = PA_GetLongParameter(params, 2);

	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);

	if (IsWindow(hWindow)) {
		ShowWindowAsync(hWindow, PALShowState);
		PALReturnValue = 1;
	}
	else {
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}


// ------------------------------------------------
//
//  FUNCTION: gui_DisableCloseBoxEx( PA_PluginParameters params )
//
//
//	MODIFICATIONS: 09/09/02 Added functionality to restore the close box.
//								 Pass in the window handle as a negative to restore.

void gui_DisableCloseBoxEx(PA_PluginParameters params)
{
	PA_long32 PAL4DWinRef,  PALReturnValue;

	// WJF 9/1/15 #43731 Changed to Index
	// ACW 1/14/21 WIN-114 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	
	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);

	if (IsWindow(hWindow)) {
		HMENU hSysMenu = GetSystemMenu(hWindow, 0);
		
		EnableMenuItem(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	
		PALReturnValue = 1;
	}
	else {
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}



