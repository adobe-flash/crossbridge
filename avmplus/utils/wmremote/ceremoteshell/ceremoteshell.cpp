/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// ceremoteshell.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <rapi.h>
#include <io.h>

typedef struct _wait_info
{
	HANDLE proc_id;
	DWORD dwTimeout;
} wait_info;

typedef unsigned int uint32_t;

class SignalData
{
public:
	SignalData(uint32_t *addr, HANDLE handle) 
		: signal(addr), eventHandle(handle) {}
	uint32_t *signal;
	HANDLE eventHandle;
};

DWORD WINAPI WaitForMemorySignal(LPVOID lpParam)
{
	SignalData *sig_data = (SignalData*)lpParam;
	while(true) {
		WaitForSingleObject(sig_data->eventHandle, INFINITE);
		*(sig_data->signal) = true;
	}
	delete sig_data;
	return 0;
}

void WriteOnNamedSignal(const char *name, uint32_t *addr)
{
	HANDLE m_namedSharedObject;

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;

	m_namedSharedObject = CreateEventA(&sa, FALSE, FALSE, name);
	if(m_namedSharedObject == NULL){
		LPVOID lpMsgBuf;
		FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf, 0, NULL );
		fputs((const char*)lpMsgBuf, stderr);
		return;
	}
	SignalData *sig_data = new SignalData(addr, m_namedSharedObject);
	CreateThread(NULL, 0, WaitForMemorySignal, sig_data, 0, NULL);
}

FILE *HandleToStream(void *handle)
{
	return _fdopen(_open_osfhandle((intptr_t)handle, 0), "wb");
}

void* OpenAndConnectToNamedPipe(const char *pipeName)
{
	char name[256];
	_snprintf(name, sizeof(name), "\\\\.\\pipe\\%s", pipeName);

	HANDLE pipe = CreateNamedPipeA((LPCSTR)name, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 4096, 4096, 100, NULL);
	ConnectNamedPipe(pipe, NULL);
	return (void*)pipe;
}


void CloseNamedPipe(void *handle)
{
	FlushFileBuffers(handle);
	CloseHandle((HANDLE)handle);
}

void DoMemProfiler()
{
	DWORD dwAttr;
	DWORD dwBufsize;
	DWORD dwFilesize;
	DWORD dwRead;

	char localBuf[4096];

	//printf("got mmgc spy signal\n");
	DWORD t;
	BYTE* b;
	CeRapiInvoke(L"\\Windows\\avmremote.dll", L"RunMemProfiler",
		0, NULL, &t, &b, NULL, 0);		
	dwAttr = CeGetFileAttributes(L"\\Temp\\gcstats.txt");
	if( dwAttr==-1)
		printf("no gcstats file\n");
	else
	{
		//Sleep(10000);
		//printf("getting gcstats file...\n");

		void *pipe = OpenAndConnectToNamedPipe("MMgc_Spy");
		FILE* spyStream = HandleToStream(pipe);

		HANDLE statsFile = CeCreateFile(L"\\Temp\\gcstats.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		dwBufsize = 4096;
		dwFilesize = CeGetFileSize(statsFile, NULL);
		if (dwFilesize<4096)
			dwBufsize = dwFilesize;

		//printf("reading gcstats files, size is %d\n", dwFilesize);

		while (CeReadFile(statsFile, localBuf, dwBufsize, &dwRead, NULL))
		{

			fwrite((void*)localBuf, 1, dwRead, spyStream);

			dwFilesize -= dwRead;
			if (dwFilesize<4096)
				dwBufsize = dwFilesize;
			if (dwBufsize<=0)
				break;
		}
		fflush(spyStream);
		CloseNamedPipe(pipe);
	}
}
DWORD copyFileToDevice(char *source,char *dest) {
    DWORD dwErr,dwLen;
    WCHAR wDest[1024];
	// if source==null create an empty file
	if (source==NULL) {
		DWORD dwLen=MultiByteToWideChar(CP_ACP,MB_COMPOSITE,dest,-1,wDest, 256);
        HANDLE hEmpty=CeCreateFile(wDest, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
        CeCloseHandle(hEmpty);
		return 0;
	}
	HANDLE hSource = CreateFile(source,GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSource==INVALID_HANDLE_VALUE)
	{
		printf("error: file '%s' does not exist\n",source);
		dwErr = GetLastError();

		CeRapiUninit();
		return -1;
	}
    dwLen=MultiByteToWideChar(CP_ACP,MB_COMPOSITE,dest,-1,wDest, 256);
	HANDLE hDest = CeCreateFile(wDest, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDest==INVALID_HANDLE_VALUE)
	{
		printf("error: unable to create file %s\n",dest);
		dwErr = GetLastError();
        CeCloseHandle(hDest);
		CeRapiUninit();
		return -1;
	}

	char localBuf[4096];
	DWORD dwBufsize=4096, dwFilesize = ::GetFileSize(hSource, NULL);
	if (dwFilesize<4096)
		dwBufsize = dwFilesize;


	DWORD dwCeBytes=0, dwRead;
	while (ReadFile(hSource, localBuf, dwBufsize, &dwRead, NULL))
	{
		BOOL bWrite = CeWriteFile(hDest, (void*)localBuf, dwRead, &dwCeBytes, NULL);
		if (!bWrite)
		{
			dwErr = CeGetLastError();
		}
		dwFilesize -= dwRead;
		if (dwFilesize<4096)
			dwBufsize = dwFilesize;
		if (dwBufsize<=0)
			break;

	}
	dwErr = GetLastError();
	CeCloseHandle(hDest);
	CloseHandle(hSource);
	return dwErr;
}
#define PROCESS_TIMEOUT 300000

static uint32_t mmgc_spy_signal = 0;

int _tmain(int argc, _TCHAR* argv[])
{

	if (argc<2) {
		printf("usage:\n --shell=avmshell_arm.exe [params] test.abc\n runs the abc file in the shell and prints the output to stdout\n");
        return -1;
	}
	//printf("initialize rapi...\n");
	// Initialize RAPI
	HRESULT hr = CeRapiInit();
    char shell[128];
    char str1[128];
	char cmdline[1024];
	char filelist[4096];
	HANDLE hFile;
	filelist[0]=0;
	cmdline[0]=0;
	strcpy(shell,"\\Program Files\\shell\\avmshell.exe");
	BOOL inArgs=0;
    BOOL hasLog=0;
	for (int i=1;i<argc;i++) {
		//printf("%d : %s\n",i,argv[i]);
		if (!strcmp("-log",argv[i])) {
			hasLog=1;
		}
		if (strlen(argv[i])>7) {
			strncpy(str1,argv[i],8);
			str1[8]='\0';
			if (!strcmp(str1,"--shell=")) {
				strcpy(shell,argv[i]+8);
				continue;
			}
		}
		// check if file exists locally to copy to device
		if (argv[i][0]!='-') {
            hFile = CreateFile(argv[i],GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile!=INVALID_HANDLE_VALUE) {
				if (!hasLog) {
					strcat(cmdline,"-log ");
					hasLog=1;
				}
				if (inArgs==0) {
					strcat(filelist,"\\Temp\\remoteshell.abc");
					copyFileToDevice(NULL,"\\Temp\\remoteshell.abc");
				    strcat(cmdline,"\\Temp\\remoteshell.abc ");
                    inArgs=1;
				}	 
  				char dest[128];
				strcpy(dest,"\\Temp\\");
				if (strrchr(argv[i],'/')!=NULL) {
					strcat(dest,strrchr(argv[i],'/')+1);
				} else if (strrchr(argv[i],'\\')!=NULL) {
					strcat(dest,strrchr(argv[i],'\\')+1);
				} else {
					strcat(dest,argv[i]);
				}
				if (strlen(filelist)>0) {
					strcat(filelist,"\t");
				}
				strcat(filelist,dest);
				copyFileToDevice(argv[i],dest);
				strcat(cmdline,dest);
				if (i<argc-1)
					strcat(cmdline," ");
				continue;
			}
		}
		strcat(cmdline,argv[i]);
		if (i<argc-1) {
			strcat(cmdline," ");
		}
	}
	printf("shell: '%s' cmdline: '%s'\n",shell,cmdline);

	WCHAR wShell[1024];
    DWORD dwLen=MultiByteToWideChar(CP_ACP,0,shell,-1,wShell, 128);
	DWORD dwErr = 0;
	// Start avmshell with -redir
	DWORD dwAttr = CeGetFileAttributes(wShell);
    // check for avmshell
	hFile = ::CeCreateFile(wShell, 
								GENERIC_READ, FILE_SHARE_READ,
								NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{
		printf("error: shell '%s' does not exist on device\n",shell);
		dwErr = GetLastError();

		CeRapiUninit();
		return -1;
	}

	WriteOnNamedSignal("MMgc::MemoryProfiler::DumpFatties", &mmgc_spy_signal);

	// remove the log file from previous run if it exists.
	// It is left in place in the event that you need to debug
	hr = CeDeleteFile(L"\\Temp\\remoteshell.log");
//    printf("copying avmfile.abc...\n");

	DWORD cbOut;
	BYTE *pOut;
//	printf("calling start avmshell...\n");
    char pIn[1024];
	pIn[0]=0;
	strcat(pIn,shell);
	strcat(pIn,"\t");
	strcat(pIn,cmdline);
	hr = CeRapiInvoke( L"\\Windows\\avmremote.dll", L"StartAVMShell",
						strlen(pIn), ( BYTE * )pIn,  
						&cbOut, ( BYTE ** ) &pOut, NULL, 0);

	HANDLE hProcess = *(HANDLE*)pOut;
	LocalFree((HLOCAL)*pOut);
	if (pIn) LocalFree((HLOCAL)pIn);

	// Check every second (up to PROCESS_TIMEOUT)
	wait_info info;
	DWORD dwTimeout = 0;
	DWORD dwInterval = 1000;
    DWORD dwCode = 0;
	info.dwTimeout = dwInterval;
	info.proc_id = hProcess;
	//printf("calling wait for avmshell...\n");
	while (CeRapiInvoke(L"\\Windows\\avmremote.dll", L"WaitForAVMShell",
						sizeof(wait_info), (BYTE*)&info,  
						&cbOut, ( BYTE ** ) &pOut, NULL, 0)==S_OK)
	{
		// pOut should be the return code
		dwCode = *(DWORD*)pOut;
		LocalFree((HLOCAL)*pOut);

		if (dwCode==STILL_ACTIVE)
		{
			dwTimeout += dwInterval;
			if( mmgc_spy_signal )
			{
				mmgc_spy_signal = 0;
				DoMemProfiler();
			}
			if (dwTimeout>=PROCESS_TIMEOUT)
			{
				// Kill the process if we don't end in a decent amount of time (e.g. 60 seconds)
				CeRapiInvoke(L"\\Windows\\avmremote.dll", L"KillAVMShell",
								sizeof(HANDLE), (BYTE*)&hProcess,  
								&cbOut, ( BYTE ** ) &pOut, NULL, 0);
				break;
			}
			continue;
		}

		break;
	}
		

	// above may have failed!

	DWORD dwTickStart = ::GetTickCount();
	dwAttr = CeGetFileAttributes(L"\\Temp\\remoteshell.log");
	if (dwAttr==-1)
	{
		printf("error: no output\n");
		// no output file, test must have crashed
		//hr = CeDeleteFile(L"\\Temp\\avmfile.abc");
	
		// Uninitialize RAPI
		CeRapiUninit();

		return -1;
	}

    HANDLE ceFile;
	while (1)
	{
		if (::GetTickCount()>(dwTickStart+600000))
			break; // assume we failed after 10(?) minutes

		Sleep(100);
		dwAttr = CeGetFileAttributes(L"\\Temp\\remoteshell.log");
		if (dwAttr==-1)
			continue;
		ceFile = CeCreateFile(L"\\Temp\\remoteshell.log", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (ceFile==INVALID_HANDLE_VALUE)
			continue;
		if (CeGetFileSize(ceFile, NULL)==0)
			continue;

		break;
	}

	// cleanup
	char *file;
    WCHAR wFile[1024];
	file=strtok(filelist,"\t");
	while (file!=NULL) {
		dwLen=MultiByteToWideChar(CP_ACP,0,file,-1,wFile, 1024);
		CeDeleteFile(wFile);
		file=strtok(NULL,"\t");
	}
    DWORD dwBufsize;	
    DWORD dwFilesize;
	dwBufsize = 4096;
	dwFilesize = CeGetFileSize(ceFile, NULL);
	if (dwFilesize<4096)
		dwBufsize = dwFilesize;

    char localBuf[4096];
    DWORD dwRead;
	while (CeReadFile(ceFile, localBuf, dwBufsize, &dwRead, NULL))
	{
	
		fwrite((void*)localBuf, 1, dwRead, stdout);

		//hr = CeWriteFile(ceFile, (void*)localBuf, dwRead, &dwCeBytes, NULL);
		
		dwFilesize -= dwRead;
		if (dwFilesize<4096)
			dwBufsize = dwFilesize;
		if (dwBufsize<=0)
			break;
	}
	fflush(stdout);
	dwErr = GetLastError();
	CeCloseHandle(ceFile);

	// Uninitialize RAPI
	CeRapiUninit();

	return dwCode;
}

