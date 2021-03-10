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

// ------------------------------------------------
//
//  FUNCTION: sys_SetRegKey( PA_PluginParameters params, PA_long32 selector )
//
//  PURPOSE:  Set a registry key value.
//
//	DATE:	  REB 11/17/10 #25402
//
// ACW 3/8/21 WIN-97
//
void sys_SetRegKey(PA_PluginParameters params, PA_long32 selector)
{
	PA_long32 PALRootKey, PALReturnValue, PALValue;
	PA_Unistring *PAUSubKeyNames, *PAURegName, *PAUValue;

	DWORD dwDataType;
	DWORD keyState = 0, dataSize = 0;
	LONG retErr;
	HKEY hRootKey, hOpenKey;

	hRootKey = hOpenKey = 0;

	// Get the function parameters.
	PALRootKey = PA_GetLongParameter(params, 1);
	PAUSubKeyNames = PA_GetStringParameter(params, 2);
	PAURegName = PA_GetStringParameter(params, 3);

	// Convert the 4d registry constant into a Windows registry key.
	hRootKey = util_getRegRoot(PALRootKey);

	// Open the registry key.
	retErr = RegOpenKeyEx(hRootKey, (LPCWSTR)PAUSubKeyNames->fString, 0, KEY_ALL_ACCESS, &hOpenKey);

	PALReturnValue = -99;

	// If the key does not exist create it now.
	if (retErr == ERROR_FILE_NOT_FOUND) {
		PALReturnValue = -98;
		retErr = RegCreateKeyEx(hRootKey, (LPCWSTR)PAUSubKeyNames->fString, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hOpenKey, &keyState);
	}

	if (retErr == ERROR_SUCCESS) {
		PALReturnValue = -97;

		// Get the value type from the registry.
		retErr = RegQueryValueEx(hOpenKey, (LPCWSTR)PAURegName->fString, NULL, &dwDataType, NULL, &dataSize);

		// If the value was not found we'll need to determine the type based on the value passed in.
		if (retErr == ERROR_FILE_NOT_FOUND) {
			switch (selector) {
				case 10: // sys_SetRegLongint
					dwDataType = REG_DWORD;
					retErr = ERROR_SUCCESS;
					break;
				case 11: // sys_SetRegText
					dwDataType = REG_SZ;
					retErr = ERROR_SUCCESS;
					break;
			}
		}

		if (retErr == ERROR_SUCCESS) {
			switch (dwDataType) {
				case REG_DWORD:
				case REG_DWORD_BIG_ENDIAN:
					PALValue = PA_GetLongParameter(params, 4);
				
					retErr = RegSetValueEx(hOpenKey, (LPCWSTR)PAURegName->fString, NULL, dwDataType, (PBYTE)&PALValue, sizeof(PALValue));

					if (retErr == ERROR_SUCCESS) {
						PALReturnValue = 1;
					}
					else {
						PALReturnValue = retErr * -1;
					}

					break;

				case REG_EXPAND_SZ:
				case REG_SZ:
					PAUValue = PA_GetStringParameter(params, 4);
					
					retErr = RegSetValueEx(hOpenKey, (LPCWSTR)PAURegName->fString, NULL, dwDataType, (LPCBYTE)PAUValue->fString, (DWORD) ((PAUValue->fLength +1) * sizeof (WCHAR))); 

					if (retErr == ERROR_SUCCESS) {
						PALReturnValue = 1;
					}
					else {
						PALReturnValue = retErr * -1;
					}

					break;
			}
		}


	}

	RegCloseKey(hOpenKey);
	PA_ReturnLong(params, PALReturnValue);
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
