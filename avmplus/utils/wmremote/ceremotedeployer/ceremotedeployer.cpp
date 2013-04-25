/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
// ceremoteshell.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <rapi.h>

typedef struct _wait_info
{
	HANDLE proc_id;
	DWORD dwTimeout;
} wait_info;

#define PROCESS_TIMEOUT 60000

int _tmain(int argc, _TCHAR* argv[])
{
	
	// Initialize RAPI
	HRESULT hr = CeRapiInit();

	WCHAR destFile[MAX_PATH];
    WCHAR sourceFile[MAX_PATH];

	if (argc>2) {
		wcscpy(destFile,argv[2]);
	    wcscpy(sourceFile,argv[1]);
	} else if (argc>1) {
   	    wcscpy(destFile,L"\\Program Files\\shell\\avmshell.exe");
		wcscpy(sourceFile,argv[1]);
	} else {
		printf("Usage: ceremotedeployer.exe [source] [dest]\n");
		CeRapiUninit();
		return -1;
	}
	HANDLE hFile = ::CreateFile(sourceFile, 
								GENERIC_READ, FILE_SHARE_READ,
								NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		printf("Error copying file %s to device",sourceFile);
		CeRapiUninit();
		return -1;
	}

	hr = CeDeleteFile(destFile);

	// Create the file on the device in the temp directory
	WCHAR ceFilename[MAX_PATH];
	DWORD dwmblen = ::MultiByteToWideChar(CP_ACP, MB_COMPOSITE, (const char *)sourceFile, strlen((const char *)sourceFile), ceFilename, MAX_PATH); 
	ceFilename[dwmblen] = 0;


	HANDLE ceFile = CeCreateFile(destFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);

	char localBuf[4096];
	DWORD dwBufsize=4096, dwFilesize = ::GetFileSize(hFile, NULL);
	if (dwFilesize<4096)
		dwBufsize = dwFilesize;


	DWORD dwCeBytes=0, dwRead;
	while (ReadFile(hFile, localBuf, dwBufsize, &dwRead, NULL))
	{
		hr = CeWriteFile(ceFile, (void*)localBuf, dwRead, &dwCeBytes, NULL);
		dwFilesize -= dwRead;
		if (dwFilesize<4096)
			dwBufsize = dwFilesize;
		if (dwBufsize<=0)
			break;
	}

	// Uninitialize RAPI
	CeRapiUninit();
	return 0;
}

