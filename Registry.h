/* --------------------------------------------------------------------------------
#
#	Registry.h
#	
#	Project : Win64API
#	author : jmiller
#	9/29/2020
#
# --------------------------------------------------------------------------------*/

#include "Defines.h"

#if VERSIONWIN
#include <Windows.h>
#endif

#define MAX_KEY_LENGTH				255

// sys_GetRegKey
#define GR_HKEY_CLASSES_ROOT		1
#define GR_HKEY_CURRENT_USER		2
#define GR_HKEY_DYN_DATA			3
#define GR_HKEY_LOCAL_MACHINE		4
#define GR_HKEY_USERS				5
#define GR_HKEY_CURRENT_CONFIG		6
#define GR_HKEY_PERFORMANCE_DATA	7

// Internal methods
HKEY util_getRegRoot(PA_long32 lKey);

// Published methods
void sys_GetRegEnum(PA_PluginParameters params); 
void sys_GetRegKey(PA_PluginParameters params); // ACW 10/16/20 WIN-80
void sys_SetRegKey(PA_PluginParameters params, PA_long32 selector); // ACW 3/8/21 WIN-97