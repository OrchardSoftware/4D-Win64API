/* --------------------------------------------------------------------------------
#
#	Process.h
#
#	Project : Win64API
#	author : jmiller
#	3/15/2021 WIN-96
#
# --------------------------------------------------------------------------------*/

#pragma once
#include "Defines.h"

#include <Windows.h>
#include <tlhelp32.h>

void sys_KillProcessByName(PA_PluginParameters params);
void sys_IsAppLoaded(PA_PluginParameters params);
void sys_ShellExecute(PA_PluginParameters params);
