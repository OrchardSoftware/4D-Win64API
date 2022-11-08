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
#include <process.h>    /* _beginthread, _endthread */
#include <iostream>
#include <fstream>

#pragma once

#define TW_FLAG_EXCLUDE_WIA 0x0001 // SDL 10/4/17 WIN-51
#define MAX_PATH_PLUS 388 // WJF 9/11/15 #43727

// REB 2/26/13 #35165 Structure to communicate with the helper thread
typedef struct	_TWAIN_CAPTURE
{
	long		returnValue; // WJF 9/14/15 #43727 Changed to long from LONG_PTR
							 // HANDLE		DIBHandle; // WJF 9/10/15 #43727 No longer needed
	BOOL		done;
	WCHAR		*filePath; // WJF 9/10/15 #43727 File path to save the image to
	BOOL		showUI; // WJF 9/10/15 #43727 True to show TWAIN UI, false to hide
	BOOL		get64; // WJF 9/10/15 #43727 True to load 64-bit TWAIN drivers, false to load 32-bit
	BOOL		wiaMode; // WJF 9/21/15 #43940 True to get/acquire WIA
	BOOL		getMultiple; // WJF 9/21/15 #43940 True to acquire multiple images at once
	long		numPictures; // WJF 9/21/15 #43940 Number of pictures returned
} TWAIN_CAPTURE;

void TWAIN_GetSources(PA_PluginParameters params); //REB 6/23/09 #14151
void TWAIN_SetSource(PA_PluginParameters params); // REB 6/23/09 #14151
void TWAIN_AcquireImage(PA_PluginParameters params); // REB 6/23/09 #14151

// REB 2/26/13 #35165
unsigned __stdcall TWAIN_GetImage(void *);

// WJF 9/10/15 #43727
// WJF 9/21/15 #43940 Added IsWIA and GetMultiple
// ACW 11/8/22 WIN-123 Added Get64
long __stdcall OrchTwain_Get(const wchar_t * filePath, BOOL Get64, BOOL ShowUI, BOOL IsWIA, BOOL GetMultiple);

