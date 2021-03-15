﻿//Win64API.cpp
/* --------------------------------------------------------------------------------
 #
 #	Win64API.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Win32API
 #	author : dwaddell
 #	9/30/2019
 #
 # --------------------------------------------------------------------------------*/


#include "Win64API.h"
#include "Registry.h"
#include "WindowManagement.h"
#include "Miscellaneous.h"
#include "Logging.h"
#include "Printing.h"

const wchar_t* const win32Commands[] = {
	L"sys_GetRegEnum"
	,L"sys_GetRegText" // ACW 10/20/20 WIN-78 
	,L"sys_GetRegLongint" // ACW 10/16/20 WIN-80
	,L"gui_SetWindowTitleEx" // ACW 10/28/20 WIN-108
	,L"gui_SetIconEX" // ACW 1/8/21 WIN-113
	,L"gui_GetWindowStateEx" // ACW 1/11/21 WIN-109
	,L"gui_ShowWindowEx" // ACW 1/14/21 WIN-112
	,L"gui_DisableCloseBox" // ACW 1/14/21 WIN-114
	,L"sys_PlayWav" // ACW 2/23/21 WIN-87
	,L"sys_LoggingStart" // ACW 3/4/21 WIN-105
	,L"sys_LoggingStop" // ACW 3/4/21 WIN-76
	,L"sys_SetRegLongint" // ACW 3/8/21 WIN-97
	,L"sys_SetRegText" // ACW 3/9/21 WIN-99
	,L"gui_SetWindowLongEx" // ACW 3/11/21 WIN-115
	,L"gui_SelectColor" // ACW 3/11/21 WIN-118	
	,L"sys_GetDefPrinter" // ACW 3/15/21 WIN-104
	,L"sys_SetDefPrinter" // ACW 3/15/21 WIN-120
};

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{

	WCHAR *wCommandID;

	if ((selector > 0) && (selector < NUM_COMMANDS)) { // WJF 7/11/16 Win-20 200 -> NUM_COMMANDS, <= -> <
		size_t bufferSize = (5 + wcslen(win32Commands[selector - 1]));
		wCommandID = (WCHAR*)malloc(sizeof(WCHAR) * bufferSize);		
		
		// WJF 7/11/16 Win-20 szCommandConst -> win32Commands[selector-1]  
		// ZRW 3/23/17 WIN-39 128 -> sizeof(szCommandID)
		// ACW 3/3/21 WIN-105 strcpy_s -> wcscpy_s
		wcscpy_s(wCommandID, bufferSize, win32Commands[selector - 1]);
																					
		// ZRW 4/5/17 WIN-39 128 -> sizeof(szCommandID)
		// ACW 3/3/21 WIN-105 strcat_s -> wcscat_s
		wcscat_s(wCommandID, bufferSize, L"\r\n");

		writeLogFile(wCommandID);
	}

	switch (selector)
	{
		case kInitPlugin:
			InitPlugin();
			break;

		case kDeinitPlugin:
			DeinitPlugin();
			break;

		case 1: // sys_GetRegEnum
			sys_GetRegEnum(params);
			break;

		case 2: // sys_GetRegText
		case 3: // sys_GetRegLongint
			// ACW 10/16/20 WIN-80
			sys_GetRegKey(params);
			break;

		case 4: // gui_setWindowTitleEx
			// ACW 10/28/20 WIN-108
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_SetWindowTitleEx, params);
			break;

		case 5: // gui_setIconEX
			// ACW 1/8/21 WIN-113
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_SetIconEx, params);
			break;

		case 6: // gui_GetWindowStateEx
			// ACW 1/11/21 WIN-109
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_GetWindowStateEx, params);
			break;

		case 7: // gui_ShowWindowEx
			// ACW 1/14/21 WIN-112
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_ShowWindowEx, params);
			break;

		case 8: // gui_DisableCloseBoxEx
			// ACW 1/14/21 WIN-114
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_DisableCloseBoxEx, params);
			break;

		case 9: // sys_PlayWav
			// ACW 2/23/21 WIN-87
			sys_PlayWav(params);
			break;

		case 10: // sys_LoggingStart
			// ACW 3/4/21 WIN-105
			sys_LoggingStart(params);
			break;

		case 11: // sys_LoggingStop
			// ACW 3/4/21 WIN-76
			sys_LoggingStop(params);
			break;
      
		case 12: // sys_SetRegLongint
			// ACW 3/8/21 WIN-97
			sys_SetRegKey(params, selector); 
			break;

		case 13: // sys_SetRegLongint
			// ACW 3/9/21 WIN-99
			sys_SetRegKey(params, selector);
			break;

		case 14: // gui_SetWindowLongEx
				 // ACW 3/11/21 WIN-115
			PA_RunInMainProcess((PA_RunInMainProcessProcPtr)gui_SetWindowLongEx, params);
			break;

		case 15: // gui_SelectColor
			// ACW 3/11/21 WIN-118
			gui_SelectColor(params);
			break;
			
		case 16: // sys_GetDefPrinter
			// ACW 3/15/21 WIN-104
			sys_GetDefPrinter(params);
			break;

		case 17: // sys_SetDefPrinter
			// ACW 3/15/21 WIN-120
			sys_SetDefPrinter(params);
			break;

	}	
}

void InitPlugin()
{

	// WJF 9/1/15 #43731
	// ACW 10/28/20 WIN-107 Changed handleArray_init to initWindowManagement
	initWindowManagement();

}

void DeinitPlugin()
{
	// Write deinitialisation code here...
}




