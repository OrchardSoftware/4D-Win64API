/* --------------------------------------------------------------------------------
#
#	Printing.h
#
#	Project : Win64API
#	author : dwaddell
#	3/15/2021
#
# --------------------------------------------------------------------------------*/

#include "Defines.h"
#include "windows.h"

#pragma once

void sys_GetDefPrinter(PA_PluginParameters params);
void sys_SetDefPrinter(PA_PluginParameters params);
void sys_SendRawPrinterData(PA_PluginParameters params);
