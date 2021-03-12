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
#include <unordered_map> // ACW 1/8/21 WIN-113

#define Win64_MDI_WinRef (-1)

#define IS_ICONIC 1
#define IS_ZOOMED 2

// Internal methods
void initWindowManagement();

// Published methods
void gui_SetWindowTitleEx(PA_PluginParameters params); // ACW 10/27/20 WIN-108
void gui_SetIconEx(PA_PluginParameters params); // ACW 1/8/21 WIN-113
void gui_GetWindowStateEx(PA_PluginParameters params); // ACW 1/11/21 WIN-109
void gui_ShowWindowEx(PA_PluginParameters params); // ACW 1/14/21 WIN-112
void gui_DisableCloseBoxEx(PA_PluginParameters params); // ACW 1/14/21 WIN-114
void gui_SetWindowLongEx(PA_PluginParameters params); // ACW 3/11/21 WIN-115
