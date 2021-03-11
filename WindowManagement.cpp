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
		lStyle = GetWindowLong(hWindow, GWL_STYLE);
		
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
