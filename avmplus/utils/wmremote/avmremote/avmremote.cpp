/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
// avmremote.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <rapi.h>
#include <Msgqueue.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


STDAPI StartAVMShell(
   DWORD cbInput,
   BYTE  *pInput,
   DWORD *pcbOutput,
   BYTE  **ppOutput,
   IRAPIStream *pIRAPIStream )
{
    WCHAR wShell[1024];
	char shell[1024];
	WCHAR wCmdline[1024];
	DWORD dwLen;
	char cmdline[1024];
	strcpy(shell,(char *)pInput);
	if (cbInput>0) {
		shell[cbInput]=0;
		char *match;
		match=strchr(shell,'\t');
		if (match!=NULL) {
			strcpy(cmdline,match+1);
			shell[cbInput-strlen(match)]=0;
			cmdline[cbInput-strlen(shell)-1]=0;
			dwLen=MultiByteToWideChar(CP_ACP,0, (LPCSTR)shell, -1, wShell, 256);
		} else {
			wcscpy(wShell,L"\\Program Files\\shell\\avmshell.exe");
			strcpy(cmdline,shell);
		}
   		dwLen=MultiByteToWideChar(CP_ACP,0, (LPCSTR)cmdline, -1, wCmdline, 1024);
	} else {
		wcscpy(wShell,L"\\Program Files\\shell\\avmshell.exe");
		wcscpy(wCmdline, L"-log \\Temp\\remoteshell.abc \\Temp\\avmfile.abc");
	}
	PROCESS_INFORMATION processInfo;
	// program live in \\Program Files\\shell\\avmshell.exe
	BOOL bResult = CreateProcess(wShell,
								wCmdline,
								NULL, NULL, FALSE,
								CREATE_NEW_CONSOLE,
								NULL, NULL, NULL,
								&processInfo);
	
	if (!bResult) {
		DebugBreak();
		return E_FAIL;
	}

	if (pcbOutput)
	{
		*ppOutput = (BYTE*)LocalAlloc(LPTR, sizeof(HANDLE));

		*(HANDLE*)*ppOutput = processInfo.hProcess;

		*pcbOutput = sizeof(HANDLE);
	}

	return S_OK;
}

typedef struct _wait_info
{
	HANDLE proc_id;
	DWORD dwTimeout;
} wait_info;

STDAPI WaitForAVMShell(
   DWORD cbInput,
   BYTE  *pInput,
   DWORD *pcbOutput,
   BYTE  **ppOutput,
   IRAPIStream *pIRAPIStream )
{
	if (cbInput!=sizeof(wait_info))
		return E_FAIL;

	wait_info* info = (wait_info*)pInput;

	DWORD dwWait = WaitForSingleObject(info->proc_id, info->dwTimeout);
	if (dwWait==WAIT_FAILED)
		return E_FAIL;
    
    DWORD exitCode;
    if (dwWait==WAIT_TIMEOUT)
        exitCode=STILL_ACTIVE;
    else
    {
        GetExitCodeProcess(info->proc_id,&exitCode);
	}
	*ppOutput = (BYTE*)LocalAlloc(LPTR, sizeof(DWORD));
	*((DWORD*)*ppOutput) = exitCode;
	*pcbOutput = sizeof(DWORD);

	return S_OK;
}

STDAPI KillAVMShell(
   DWORD cbInput,
   BYTE  *pInput,
   DWORD *pcbOutput,
   BYTE  **ppOutput,
   IRAPIStream *pIRAPIStream )
{
	HANDLE hProcess = (HANDLE)*pInput;
	TerminateProcess(hProcess, 0);

	return S_OK;
}

STDAPI RunMemProfiler(
	DWORD cbInput,
	BYTE  *pInput,
	DWORD *pcbOutput,
	BYTE  **ppOutput,
	IRAPIStream *pIRAPIStream )
{
	HANDLE m_queue;
	MSGQUEUEOPTIONS msgopts;
	
	msgopts.dwFlags = MSGQUEUE_NOPRECOMMIT;
	msgopts.dwMaxMessages = 1;
	msgopts.cbMaxMessage = 256;
	msgopts.bReadAccess = false;
	msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);

	WCHAR* wName = L"MMgc::MemoryProfiler::DumpFatties";

	m_queue = CreateMsgQueue(wName, &msgopts);
	
	char buff = 'P';
	WriteMsgQueue(m_queue, &buff, 1, 0, 0);

	CloseMsgQueue(m_queue);
	return S_OK;
}
