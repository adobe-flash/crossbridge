/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// wmrunner.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "wmrunner.h"

#define MAX_LOADSTRING 100
#define IDT_TIMER1 100
#define TIMER_INTERVAL 100

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hWndMenuBar;		// menu bar handle
INT                 i_timer;
TCHAR               szMessage[550];
TCHAR               szError[550];
TCHAR               szExitCode[550];
TCHAR               szExitCodeFile[550];
TCHAR               nextvm[500];
TCHAR               shell[100];
TCHAR               last[200];
INT                 i_buildcount;
LONG				l_buildsum;
INT                 i_maxtime;
INT                 i_currenttime;
DWORD               i_exitcode;

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL			checkForCommand(HWND);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WMRUNNER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMRUNNER));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    g_hInst = hInstance; // Store instance handle in our global variable

    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_WMRUNNER, szWindowClass, MAX_LOADSTRING);

    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    BOOL bRes;
    LONG avg;
	static SHACTIVATEINFO s_sai;
	RECT r;
	HANDLE handle;
	TCHAR szFile[200];
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_HELP_ABOUT:
                    DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
                    break;
                case IDM_OK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
            SHMENUBARINFO mbi;
            i_timer=0;
            memset(&mbi, 0, sizeof(SHMENUBARINFO));
            mbi.cbSize     = sizeof(SHMENUBARINFO);
            mbi.hwndParent = hWnd;
            mbi.nToolBarId = IDR_MENU;
            mbi.hInstRes   = g_hInst;

            if (!SHCreateMenuBar(&mbi)) 
            {
                g_hWndMenuBar = NULL;
            }
            else
            {
                g_hWndMenuBar = mbi.hwndMB;
            }

            // Initialize the shell activate info structure
            memset(&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
            _tcscpy(szMessage,L"waiting...");
            _tcscpy(szError,L"no errors");
			_tcscpy(shell,L"\\Storage Card\\shell\\avmshell_arm.exe");
			//_tcscpy(shell,L"\\Program Files\\shell\\avmshell.exe");
			_tcscpy(nextvm,L"\\Storage Card\\nextvm.txt");
			last[0]='\0';
			i_buildcount=0;
			l_buildsum=0;
			i_maxtime=0;
			i_exitcode=0;
			_tcscpy(szFile,L"\\Storage Card\\running.txt");
			handle=CreateFile(szFile,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
      		if (handle==INVALID_HANDLE_VALUE) {
  			    _tcscpy(szMessage,L"ERROR: unable to write to Storage Card");
			    SetRect(&r,10,10,220,230);
			    InvalidateRect(hWnd,&r,TRUE);
			} else {
				TCHAR szMsg[100];
				DWORD dwBytesWritten;
				_tcscpy(szMsg,L"running");
				WriteFile(handle,szMsg,6,&dwBytesWritten,NULL);
			}
            CloseHandle(handle);
			SetTimer(hWnd, IDT_TIMER1, TIMER_INTERVAL, (TIMERPROC)NULL);
			break;
		case WM_TIMER:
            bRes=checkForCommand(hWnd);
			if (bRes==TRUE) {
			   SetRect(&r,10,10,220,230);
			   InvalidateRect(hWnd,&r,TRUE);
			}
			SetTimer(hWnd, IDT_TIMER1, TIMER_INTERVAL, (TIMERPROC)NULL);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			SetTextColor(hdc,RGB(0,0,0));
			SetRect(&r,10,10,220,90);
			DrawText(hdc,szMessage,-1,&r,DT_WORDBREAK);
			TCHAR szCurrent[200];
			wsprintf(szCurrent,TEXT("current test: %d ms"),i_currenttime);
			SetRect(&r,10,90,220,110);
			DrawText(hdc,szCurrent,-1,&r,DT_WORDBREAK);
			TCHAR szCount[200];
			wsprintf(szCount,TEXT("tests run: %d"),i_buildcount);
			SetRect(&r,10,110,220,130);
			DrawText(hdc,szCount,-1,&r,DT_WORDBREAK);
			if (i_buildcount==0) {
				avg=0;
			} else {
                avg=l_buildsum/i_buildcount;
			}
			TCHAR szAvg[200];
			wsprintf(szAvg,TEXT("average time: %d ms"),avg);
			SetRect(&r,10,130,220,150);
			DrawText(hdc,szAvg,-1,&r,DT_WORDBREAK);
			TCHAR szMax[200];
			wsprintf(szMax,TEXT("max time: %d ms"),i_maxtime);
			SetRect(&r,10,150,220,170);
			DrawText(hdc,szMax,-1,&r,DT_WORDBREAK);
			wsprintf(szExitCode,TEXT("exit code: %d"),i_exitcode);
			SetRect(&r,10,170,220,190);
			DrawText(hdc,szExitCode,-1,&r,DT_WORDBREAK);

/*
			SetRect(&r,10,170,220,230);
			DrawText(hdc,szError,-1,&r,DT_WORDBREAK);
*/
			EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
			_tcscpy(szFile,L"\\Storage Card\\running.txt");
            DeleteFile(szFile);
 			KillTimer(hWnd,IDT_TIMER1);
            CommandBar_Destroy(g_hWndMenuBar);
            PostQuitMessage(0);
            break;

        case WM_ACTIVATE:
            // Notify shell of our activate message
            SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            {
                // Create a Done button and size it.  
                SHINITDLGINFO shidi;
                shidi.dwMask = SHIDIM_FLAGS;
                shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
                shidi.hDlg = hDlg;
                SHInitDialog(&shidi);
            }
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;

        case WM_CLOSE:
            TCHAR szFile[200];
			_tcscpy(szFile,L"\\Storage Card\\running.txt");
            DeleteFile(szFile);
            EndDialog(hDlg, message);
            return TRUE;

    }
    return (INT_PTR)FALSE;
}
BOOL checkForCommand(HWND hWnd) {
	HANDLE handle=CreateFile(nextvm,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (handle==INVALID_HANDLE_VALUE) {
		if (_tcslen(last)==0) {
			_tcscpy(szMessage,L"waiting...");
		}
		CloseHandle(handle);
		return FALSE;
	} else {
		DWORD count;
		char data[500]={0};
		char msg[500]={0};
		BOOL read=ReadFile(handle,data,500,&count,NULL);
		if (count==0) {
			_tcscpy(szMessage,L"error reading data file");
			_tcscpy(szError,L"error reading data file");
			CloseHandle(handle);
			return TRUE;
		} else {
			while (data[count-1]==10 || data[count-1]==13) {
				count--;
			}
			if (data[count-1]!=' ' && data[count-1]!='\"') {
				_tcscpy(szMessage,L"error data file bad format");
				_tcscpy(szError,L"error data file bad format");
				CloseHandle(handle);
				return TRUE;
			}
			data[count-1]='\0';
            
			TCHAR params[500];
			DWORD space=count-1;
			for (;space>0 && data[space]!='\\';space--);
			space++;
			int i=0;
			for (;space<count-1;space++,i++) {
				msg[i]=data[space];
			}
			msg[i-1]='\0';
			BOOL convert;
            convert=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,msg,-1,last,500);
			convert=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,data,-1,params,500);
            char *pch;
			char filename[500];
			pch=strtok(data," ");
			while (pch!=NULL) {
				if (pch[0]!='\"') {
					strcat(filename," ");
					strcat(filename,pch);
				} else {
					strcpy(filename,pch+1);
				}
				if (filename[strlen(filename)-1]=='\"') {
					filename[strlen(filename)-1]=0;
				}
				pch=strtok(NULL," ");
			}
			strcat(filename,".exitcode");
			MultiByteToWideChar(CP_ACP,0,filename,-1,szExitCodeFile,500);
			if (convert==0) {
				_tcscpy(szMessage,L"error converting string");
				_tcscpy(szError,L"error converting string");
			} else {
				for (uint i=0;i<_tcslen(last);i++) {
					if (last[i]=='_') {
						last[i]=' ';
					}
				}
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi,sizeof(pi));
				i_exitcode=-1;
                CreateProcess(shell,params,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,NULL,&pi);
				DWORD start_time=GetTickCount();
                DWORD last_time=0;
				RECT r;
				while (true) {
					DWORD res=WaitForSingleObject(pi.hProcess,0);
                    DWORD end_time=GetTickCount();
					if (res==WAIT_TIMEOUT) {
  						wsprintf(szMessage,TEXT("running %s"),last);
						if (last_time/1000!=(end_time-start_time)/1000) {
                            last_time=i_currenttime=end_time-start_time;
							SetRect(&r,10,10,220,190);
  							InvalidateRect(hWnd,&r,TRUE);
							UpdateWindow(hWnd); 
						}
					} else if (res==WAIT_OBJECT_0) {
						GetExitCodeProcess(pi.hProcess,&i_exitcode);
						char szExit[100];
						sprintf(szExit,"%d\n",i_exitcode);
						DWORD dwBytes;
						HANDLE ceExitFile = CreateFile(szExitCodeFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
                        WriteFile(ceExitFile, (void*)szExit, strlen(szExit), &dwBytes, NULL);
                        CloseHandle(ceExitFile);
						i_buildcount++;
						l_buildsum+=(end_time-start_time);
						if ((LONG)(end_time-start_time)>i_maxtime) {
							i_maxtime=end_time-start_time;
						}
						wsprintf(szMessage,TEXT("finished %s"),last);
						

						i_currenttime=end_time-start_time;
						SetRect(&r,10,10,220,230);
  						InvalidateRect(hWnd,&r,TRUE);
						UpdateWindow(hWnd); 
                        break;
					} else {
						wsprintf(szError,TEXT("ERROR: %s wait returned failure %d"),last,res);
						wsprintf(szMessage,TEXT("ERROR: %s wait returned failure %d"),last,res);
						SetRect(&r,10,10,220,230);
  						InvalidateRect(hWnd,&r,TRUE);
						UpdateWindow(hWnd); 
                        break;
					}
				}
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}
        CloseHandle(handle);
        DeleteFile(nextvm);
	}
	return TRUE;
}
