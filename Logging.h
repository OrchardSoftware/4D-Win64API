/* --------------------------------------------------------------------------------
#
#	Logging.h
#
#	Project : Win64API
#	author : dwaddell
#	3/3/2021
#
# --------------------------------------------------------------------------------*/

#pragma once
#include "Defines.h"

#include <Windows.h>
#include <stdio.h>

#define _SECOND ((ULONGLONG) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

// WJF 6/17/16 Win-18 Logging
// WJF 7/8/16 Win-20 Moved to their own header
void sys_LoggingStart(PA_PluginParameters params);
void sys_LoggingStop(PA_PluginParameters params);
void writeLogFile(WCHAR *strLog);

PA_long32 logMaintenance(); // WJF 7/11/16 Win-20
PA_long32 logOpenFile();
PA_long32 logCloseFile();
