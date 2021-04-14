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


// ------------------------------------------------
//
//  FUNCTION: gui_SetWindowLongEX( PA_PluginParameters params )
//
//  PURPOSE:  Multipurpose function to set window styles etc
//

void gui_SetWindowLongEx(PA_PluginParameters params)
{
	PA_long32 PAL4DWinRef, PALReturnValue, PALStyle, PALMode;
	LONG_PTR lStyle;

	// WJF 8/31/15 #43731 Changed to index from handle to make 64-bit safe
	// ACW 1/14/21 WIN-114 Changed to be the actual 4D WinRef, -1 for the main application, 0 for current window
	PAL4DWinRef = PA_GetLongParameter(params, 1);
	PALStyle = PA_GetLongParameter(params, 2);
	PALMode = PA_GetLongParameter(params, 3);
	
	HWND hWindow = MDI::getWindowHWND(PAL4DWinRef);

	if (IsWindow(hWindow)) {
		lStyle = GetWindowLongPtr(hWindow, GWL_STYLE);
		
		if (PALMode == 1) {
			lStyle |= (PALStyle);
		}
		else {
			lStyle &= ~(PALStyle);
		}
		
		SetWindowLongPtr(hWindow, GWL_STYLE, lStyle);
		
		SetWindowPos(hWindow, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		PALReturnValue = 1;
	}
	else {
		PALReturnValue = 0;
	}

	PA_ReturnLong(params, PALReturnValue);
}

HWND hBackground;
BOOL CALLBACK FindBackgroundHWND (HWND hwnd, LPARAM lparam) {
	WCHAR szClassName[256] = L"";
	int size = 256;

	GetClassName(hwnd, szClassName, size);
	
	_wcslwr_s(szClassName, size); // ZRW 4/12/17 WIN-39 

	if (wcscmp(szClassName, L"mdiclient") == 0) {
		hBackground = hwnd;
		return FALSE;
	}
	else {
		return TRUE;
	}
}

// ------------------------------------------------
//
//  FUNCTION: gui_LoadBackground( PA_PluginParameters params )
//
//  PURPOSE:	Loads a bitmap as a background for 4D window.
//
//  COMMENTS:	May be scaled or tiled.  Bitmap must be of suffient size
//				for scaling so it does not appear blocky.  Tiled Bitmaps
//				should have edges that transition nicely from one to another.
//
//	DATE:		dcc 11/09/01
//
void gui_LoadBackground(PA_PluginParameters params, BOOL DeInit)
{
	PA_Unistring* PAUBackgroundPath;
	PA_long32 PALTileOrScale, PALReturnValue;
	
	static LONG_PTR	lLastTileOrScale = 0;
	static HWND hWindow = 0;
	static WCHAR *wLastBackgroundPath;

	size_t bufferSize;
	BOOL bFuncReturn;
	HBITMAP hBitmap;
	WPARAM wParam = 0;
	LPARAM lParam = 0;
	RECT rect;
	WCHAR *wCompare;

	if (DeInit) {
		if (hWindow != 0) {
			bFuncReturn = SendNotifyMessage(hWindow, (WM_USER + 0x0031), wParam, lParam);
		}
		return;
	}

	PALReturnValue = 0;
	PAUBackgroundPath = PA_GetStringParameter(params, 1);
	PALTileOrScale = PA_GetLongParameter(params, 2);

	switch (PALTileOrScale)
	{
		case 0:  // Assume tiled if no param
			PALTileOrScale = BM_TILE;
			break;

		case BM_SCALE:
			// Set so window NOT repainted until AFTER resized -- not during resizing
			// bFuncReturn = SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0); // 1/8/04 no longer change the system preference.
			break;

		case BM_SCALETOMAXCLIENT:
			// This causes bitmap to be scaled to maximum client size
			break;
	}

	// If a background has been displayed
	if (hWindow != 0) {
		if (PAUBackgroundPath->fLength == 0) {
			wCompare = L"";
		}
		else {
			wCompare = (WCHAR*) PAUBackgroundPath->fString;
		}
		
		// If its the same bitmap and format as this call don't do it again
		if ((wcscmp(wCompare, wLastBackgroundPath) == 0) && (PALTileOrScale == lLastTileOrScale)) { // This is the same bitmap as before
			PA_ReturnLong(params, 1);
			return;
		}

		// If we're here it's a different bitmap or format release current background
		bFuncReturn = SendNotifyMessage(hWindow, (WM_USER + 0x0031), wParam, lParam);
		bFuncReturn = GetClientRect(hWindow, &rect);
		bFuncReturn = InvalidateRect(hWindow, &rect, TRUE);

		// If there's no bitmap provided, return & clear
		if (PAUBackgroundPath->fLength == 0) {
			PA_ReturnLong(params, 1);
			hWindow = NULL;
			
			bufferSize = 1;
			wLastBackgroundPath = (WCHAR*)malloc(sizeof(WCHAR) * (bufferSize));
			wcscpy_s(wLastBackgroundPath, bufferSize, L"");
			lLastTileOrScale = 0;
			return;
		}
	}
	
	// Find where the backgfound image should go
	hBackground = NULL;
	EnumChildWindows(MDI::getWindowHWND(Win64_MDI_WinRef), (WNDENUMPROC) FindBackgroundHWND, NULL);
		
	// Make sure we found the background window handle
	if (hBackground == NULL) {
		PA_ReturnLong(params, 2);
		return;
	}
	else {
		hWindow = hBackground;
	}	
	
	hBitmap = (HBITMAP) LoadImage (0, (LPCWSTR)PAUBackgroundPath->fString, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap == NULL) {
		PALReturnValue = 0;
	}
	else {

		lParam = (LPARAM) hBitmap;
		
		//returnValue = lParam;
		PALReturnValue = 1;

		wParam = PALTileOrScale;

		// REB 3/18/11 #25290
		// WJF 6/24/16 Win-21 Switch casting around
		g_wpOrigMDIProc = (WNDPROC) SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)BkgrndProc); 
																							   
		if (g_wpOrigMDIProc != 0) {
			bFuncReturn = SendNotifyMessage(hWindow, (WM_USER + 0x0030), wParam, lParam);
			bFuncReturn = GetClientRect(hWindow, &rect);
			bFuncReturn = InvalidateRect(hWindow, &rect, TRUE);
			
			bufferSize = PAUBackgroundPath->fLength + 1;
			wLastBackgroundPath = (WCHAR*)malloc(sizeof(WCHAR) * (bufferSize));
			wcscpy_s(wLastBackgroundPath, bufferSize, (WCHAR*)PAUBackgroundPath->fString);

			lLastTileOrScale = PALTileOrScale;			
		}
		else {
			PALReturnValue = 0;
		}
	}

	PA_ReturnLong(params, PALReturnValue);
}



//  FUNCTION: scaleImage(HDC hdc, HDC hdcMem, HBITMAP hOrigBitmap, INT_PTR width, INT_PTR height)
//
//  PURPOSE:  Scale a bitmap to the desired dimensions.
//
//  COMMENTS:
//
//	DATE: MJG 12/19/03
//
// WJF 6/30/16 Win-21 INT_PTR -> INT
HGDIOBJ scaleImage(HDC hdc, HDC hdcMem, HBITMAP hOrigBitmap, INT width, INT height)
{
	BITMAP origBitmap, scaledBitmap;
	HBITMAP hScaledBitmap;
	HDC	hdcOrigBitmap;
	HGDIOBJ returnObject;

	GetObject(hOrigBitmap, sizeof(BITMAP), &origBitmap);

	hdcOrigBitmap = CreateCompatibleDC(hdc);

	scaledBitmap = origBitmap;
	scaledBitmap.bmWidth = width;
	scaledBitmap.bmHeight = height;
	scaledBitmap.bmWidthBytes = ((scaledBitmap.bmWidth + 15) / 16) * 2;

	hScaledBitmap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(hdcOrigBitmap, hOrigBitmap);
	returnObject = SelectObject(hdcMem, hScaledBitmap);

	StretchBlt(hdcMem, 0, 0, width, height, hdcOrigBitmap, 0, 0, origBitmap.bmWidth, origBitmap.bmHeight, SRCCOPY);
	DeleteDC(hdcOrigBitmap);

	return returnObject;
}

//  FUNCTION: tileImage(HDC hdc, HDC hdcMem, HBITMAP hOrigBitmap, INT_PTR width, INT_PTR height)
//
//  PURPOSE:  Tile a bitmap to the desired dimensions.
//
//  COMMENTS:
//
//	DATE: MJG 12/19/03
//
// WJF 6/30/16 Win-21 INT_PTR -> INT
HGDIOBJ tileImage(HDC hdc, HDC hdcMem, HBITMAP hOrigBitmap, INT width, INT height)
{
	BITMAP origBitmap;
	HBITMAP htiledBitmap;
	HDC	hdcOrigBitmap;
	HGDIOBJ returnObject;
	INT x, y; // WJF 6/30/16 Win-21 INT_PTR -> INT

	GetObject(hOrigBitmap, sizeof(BITMAP), &origBitmap);

	hdcOrigBitmap = CreateCompatibleDC(hdc);

	htiledBitmap = CreateCompatibleBitmap(hdc, width, height);

	SelectObject(hdcOrigBitmap, hOrigBitmap);
	returnObject = SelectObject(hdcMem, htiledBitmap);

	for (y = 0; y < height; y += origBitmap.bmHeight)
		for (x = 0; x < width; x += origBitmap.bmWidth)
			BitBlt(hdcMem, x, y, origBitmap.bmWidth, origBitmap.bmHeight, hdcOrigBitmap, 0, 0, SRCCOPY);

	DeleteDC(hdcOrigBitmap);
	return returnObject;
}

//  FUNCTION: BkgrndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//
//  PURPOSE:	subclassed window procedure
//
//  COMMENTS:
//
//	DATE:			dcc 11/09/01
//
//  MODIFICATIONS:  MJG 12/19/03 Improved efficiency when drawing background.
//
LRESULT APIENTRY BkgrndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static INT cxClient, cyClient; // WJF 6/30/16 Win-21 INT_PTR -> INT
	static HBITMAP hOrigBitmap;
	static BITMAP origBitmap;
	static HDC hdcMem, hdc;
	static RECT	clientRect; //dcc 12/14
	static INT  fullClientWidth, fullClientHeight; // WJF 6/30/16 Win-21 LONG_PTR -> INT
	static HGDIOBJ oldObject;
	PAINTSTRUCT	ps;
	RECT rect;
	INT x, y; // WJF 6/30/16 Win-21 INT_PTR -> INT

	switch (uMsg)
	{
	case (WM_USER + 0x0030): // initial setup -- passing bitmap hndl and tileOrScale
							 // tileOrScale = wParam; // WJF 6/30/16 Win-21 For what purpose
		hOrigBitmap = (HBITMAP) lParam;
		GetObject(hOrigBitmap, sizeof(BITMAP), &origBitmap);
		hdc = GetDC(hwnd);
		hdcMem = CreateCompatibleDC(hdc);

		switch (wParam) { // WJF 6/30/16 Win-21 tileOrScale -> wParam
		case BM_SCALE:
			GetClientRect(hwnd, &clientRect);
			fullClientWidth = clientRect.right;
			fullClientHeight = clientRect.bottom;
			oldObject = scaleImage(hdc, hdcMem, hOrigBitmap, fullClientWidth, fullClientHeight);
			break;
		case BM_SCALETOMAXCLIENT:
			fullClientWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			fullClientHeight = GetSystemMetrics(SM_CYFULLSCREEN);
			oldObject = scaleImage(hdc, hdcMem, hOrigBitmap, fullClientWidth, fullClientHeight);
			break;
		case BM_TILE:
			fullClientWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			fullClientHeight = GetSystemMetrics(SM_CYFULLSCREEN);
			oldObject = tileImage(hdc, hdcMem, hOrigBitmap, fullClientWidth, fullClientHeight);
			break;
		}
		break;

	case WM_SIZE:

		if (wParam == BM_SCALE) { // WJF 6/30/16 Win-21 tileOrScale -> wParam
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			if (((fullClientWidth != cxClient) || (fullClientHeight != cyClient))
				&& (cxClient != 0 && cyClient != 0)) {
				fullClientWidth = cxClient;
				fullClientHeight = cyClient;
				DeleteObject(SelectObject(hdcMem, oldObject));
				oldObject = scaleImage(hdc, hdcMem, hOrigBitmap, cxClient, cyClient);
				InvalidateRect(hwnd, &clientRect, FALSE);
				return 1;
			}
		}

	case WM_PAINT:

		GetUpdateRect(hwnd, &rect, TRUE);
		cxClient = rect.right;
		cyClient = rect.bottom;

		BeginPaint(hwnd, &ps);

		if (cyClient < fullClientHeight && cxClient < fullClientWidth)
			BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
		else
			for (y = 0; y < cyClient; y += fullClientHeight)
				for (x = 0; x < cxClient; x += fullClientWidth)
				{
					BitBlt(hdc, x, y, fullClientWidth, fullClientHeight, hdcMem, 0, 0, SRCCOPY);
				}

		EndPaint(hwnd, &ps);

		return 1; // return immediately - don't let orig window proc repaint

	case (WM_USER + 0x0031):

		ReleaseDC(hwnd, hdc);
		DeleteObject(SelectObject(hdcMem, oldObject));
		DeleteDC(hdcMem);
		DeleteObject(hOrigBitmap);
		// tileOrScale = 0; // WJF 6/30/16 Win-21 Removed
		fullClientWidth = 0;
		fullClientHeight = 0;
		cxClient = 0;
		cyClient = 0;
		// REB 3/18/11 #25290
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)g_wpOrigMDIProc);
		//SetWindowLong(hwnd, GWL_WNDPROC, (LONG) g_wpOrigMDIProc);

		break;
	}

	return CallWindowProc(g_wpOrigMDIProc, hwnd, uMsg, wParam, lParam);
}
