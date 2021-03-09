/* --------------------------------------------------------------------------------
#
#	Registry.cpp
#	All of the methods related to registry functions
#	Project : Win32API
#	author : jmiller
#	9/29/2020
#
# --------------------------------------------------------------------------------*/

#include "Registry.h"

void sys_GetRegEnum(PA_PluginParameters params)
{
	DWORD dwSubKeys, i, j;
	DWORD dwMaxSubKeyLength = MAX_KEY_LENGTH;
	HKEY hRootKey, hOpenKey;
	REGSAM regSamFlag;
	WCHAR wcSubKeyName[MAX_KEY_LENGTH];

	PA_long32 lRootKey, lEnum32, lReturnValue;
	PA_Unistring paSubKeyName;
	PA_Unistring* paSubKeyNames;
	PA_Variable paReturnKeyNamesArray;

	// Get the parameters
	lRootKey = PA_GetLongParameter(params, 1);
	paSubKeyNames = PA_GetStringParameter(params, 2);
	paReturnKeyNamesArray = PA_GetVariableParameter(params, 3);
	lEnum32 = PA_GetLongParameter(params, 4); // WJF 4/22/16 Win-15
	
	regSamFlag = KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE ;

	if (lEnum32) {
		regSamFlag = regSamFlag | KEY_WOW64_32KEY;
	}
	else {
		regSamFlag = regSamFlag | KEY_WOW64_64KEY;
	}

	lReturnValue = 0;
	dwSubKeys = 0;
	hRootKey = hOpenKey = 0;

	hRootKey = util_getRegRoot(lRootKey);

	if (RegOpenKeyEx(hRootKey, (LPCWSTR)paSubKeyNames->fString, 0, regSamFlag, &hOpenKey) == ERROR_SUCCESS)
	{
		if (RegQueryInfoKey(hOpenKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			if (dwSubKeys)
			{
				PA_ResizeArray(&paReturnKeyNamesArray, dwSubKeys);

				for (i = 0, j = 0; i < dwSubKeys; i++)
				{
					dwMaxSubKeyLength = MAX_KEY_LENGTH;

					if (RegEnumKeyEx(hOpenKey, i, wcSubKeyName, &dwMaxSubKeyLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
					{
						paSubKeyName = PA_CreateUnistring((PA_Unichar*)wcSubKeyName);
						PA_SetStringInArray(paReturnKeyNamesArray, ++j, &paSubKeyName);
					}
				}

				lReturnValue = 1;
			}
		}
	}

	// Close the handle
	RegCloseKey(hOpenKey);

	// Set the return values
	PA_SetVariableParameter(params, 3, paReturnKeyNamesArray, 0);
	PA_ReturnLong(params, lReturnValue);
}

// ------------------------------------------------
//
//  FUNCTION: sys_GetRegKey( PA_PluginParameters params )
//
//  PURPOSE:  Get a key from the registry.
//
//	DATE:	  MJG 12/4/03 (3.5.6)
//
// ACW 10/16/20 WIN-80
//
void sys_GetRegKey(PA_PluginParameters params)
{
	DWORD dwDataType, dwDataSize;
	HKEY hRootKey, hOpenKey;
	REGSAM regSamFlag;
	WCHAR *pcReturnDataBuffer, *pcReturnDataBuffer2;


	PA_long32 lRootKey, lReturnValue, lEnum32, lReturnLong;
	PA_Unistring* paSubKeyNames;
	PA_Unistring* paRegName;
	PA_Unistring* ppavReturnData;

	lRootKey = PA_GetLongParameter(params, 1);
	paSubKeyNames = PA_GetStringParameter(params, 2);
	paRegName = PA_GetStringParameter(params, 3);
	
	lEnum32 = PA_GetLongParameter(params, 5); // WJF 4/22/16 Win-15

	regSamFlag = KEY_READ;

	if (lEnum32) {
		regSamFlag = regSamFlag | KEY_WOW64_32KEY;
	}
	else {
		regSamFlag = regSamFlag | KEY_WOW64_64KEY;
	}
	
	pcReturnDataBuffer = NULL;
	dwDataSize = 0;
	lReturnValue = 0;
	hRootKey = hOpenKey = 0;

	hRootKey = util_getRegRoot(lRootKey);

	if (RegOpenKeyEx(hRootKey, (LPCWSTR)paSubKeyNames->fString, 0, regSamFlag, &hOpenKey) == ERROR_SUCCESS)
	{
		// Get the value type and size.
		if (RegQueryValueEx(hOpenKey, (LPCWSTR)paRegName->fString, NULL, &dwDataType, NULL, &dwDataSize) == ERROR_SUCCESS)
		{
			switch (dwDataType)
			{
				case REG_DWORD:
				case REG_DWORD_BIG_ENDIAN:
					// sys_GetRegLongint

					if (RegQueryValueEx(hOpenKey, (LPCWSTR)paRegName->fString, NULL, NULL, (LPBYTE)&lReturnLong, &dwDataSize) == ERROR_SUCCESS)
					{
						PA_SetLongParameter(params, 4, lReturnLong);
					
						lReturnValue = 1;
					}

					break;

				case REG_SZ:
				case REG_EXPAND_SZ:
					// sys_GetRegText

					pcReturnDataBuffer = (WCHAR*) malloc(sizeof(WCHAR) * (dwDataSize + 1));
					pcReturnDataBuffer[dwDataSize] = 0;

					// WJF 6/24/16 Win-21 Casting to LPBYTE
					if (RegQueryValueEx(hOpenKey, (LPCWSTR)paRegName->fString, NULL, NULL, (LPBYTE)pcReturnDataBuffer, &dwDataSize) == ERROR_SUCCESS)
					{
						
						if (dwDataType == REG_EXPAND_SZ) {
							dwDataSize = ExpandEnvironmentStrings((LPCWSTR)pcReturnDataBuffer, NULL, 0);
							
							if (dwDataSize > 0) {
								pcReturnDataBuffer2 = (WCHAR*)malloc(sizeof(WCHAR) * (dwDataSize));
								
								ExpandEnvironmentStrings((LPCWSTR)pcReturnDataBuffer, (LPWSTR)pcReturnDataBuffer2, dwDataSize);
								free(pcReturnDataBuffer);
								pcReturnDataBuffer = pcReturnDataBuffer2;
							}
						}
						
						ppavReturnData = PA_GetStringParameter(params, 4);
						
						PA_SetUnistring(ppavReturnData, (PA_Unichar*)pcReturnDataBuffer);

						lReturnValue = 1;
					}

					free(pcReturnDataBuffer);

					break;

			}
		}
	}

	// Close the handle
	RegCloseKey(hOpenKey);

	// Set the return value
	PA_ReturnLong(params, lReturnValue);
}

HKEY util_getRegRoot(PA_long32 lKey) {
	HKEY hReturnKey = 0;

	switch (lKey) {
	case (GR_HKEY_CLASSES_ROOT):
		hReturnKey = HKEY_CLASSES_ROOT;
		break;

	case (GR_HKEY_CURRENT_USER):
		hReturnKey = HKEY_CURRENT_USER;
		break;

	case (GR_HKEY_DYN_DATA):
		hReturnKey = HKEY_DYN_DATA;
		break;

	case (GR_HKEY_LOCAL_MACHINE):
		hReturnKey = HKEY_LOCAL_MACHINE;
		break;

	case (GR_HKEY_USERS):
		hReturnKey = HKEY_USERS;
		break;

	case (GR_HKEY_CURRENT_CONFIG):
		hReturnKey = HKEY_CURRENT_CONFIG;
		break;

	case (GR_HKEY_PERFORMANCE_DATA):
		hReturnKey = HKEY_PERFORMANCE_DATA;
		break;

	default:
		hReturnKey = HKEY_LOCAL_MACHINE;
	}

	return hReturnKey;
}
