/* --------------------------------------------------------------------------------
#
#	WindowManagement.h
#
#	Project : Win64API
#	author : dwaddell
#	10/26/2020
#
# --------------------------------------------------------------------------------*/

#pragma once

#include "Defines.h"
#include "windows.h"

typedef struct 
{
	HWND		fourDhWnd; // 4D main window
	HWND		prtSettingshWnd; // handle your print settings dialog
	HWND		prthWnd; // handle to print dialog
	HWND		MDIhWnd;
	HWND		hwndTT; // used for tool tips
	HWND		displayedTTOwnerhwnd; // used for tool tips
	HWND		openSaveTBhwnd; // used in Open/Save dialog
	HWND		MDIs_4DhWnd; // window to get class for all 4D windows (ProToolsSubMDIWndClass)
} WINDOWHANDLES;

// Internal methods
HWND getCurrentFrontmostWindow();
DWORD handleArray_init();
INT handleArray_add(LONG_PTR handle); // WJF 6/24/16 Win-21 DWORD to INT
HWND handleArray_retrieve(DWORD handleIndex);
HWND getWindowHandle(PA_Unistring* paWindowTitle, HWND hWnd);
void initWindowManagement();


// Published methods
void gui_GetWindowEx(PA_PluginParameters params); // ACW 10/26/20 WIN-107
void gui_setWindowTitleEx(PA_PluginParameters params); // ACW 10/27/20 WIN-108
