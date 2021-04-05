/* --------------------------------------------------------------------------------
#
#	TWAIN.h
#
#	Project : Win64API
#	author : dwaddell
#	4/1/2021
#
# --------------------------------------------------------------------------------*/

#include "Defines.h"
#include "windows.h"
#include "stdio.h"

#pragma once

#define TW_FLAG_EXCLUDE_WIA 0x0001 // SDL 10/4/17 WIN-51

void TWAIN_GetSources(PA_PluginParameters params); //REB 6/23/09 #14151
//void TWAIN_SetSource(PA_PluginParameters params); // REB 6/23/09 #14151
//void TWAIN_AcquireImage(PA_PluginParameters params); // REB 6/23/09 #14151
