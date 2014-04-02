#pragma once

#include "stdinc.h"
#include "iThread.h"
//#include "SpeedControl.h"

namespace utlFTP
{
	class FtpSendThread : public iThread
	{
		vector<wstring> files;
		HINSTANCE		hInst;
		WTWFUNCTIONS*	wtw;
		wtwContactDef	contact;
		HWND			hDialog;
		HANDLE			hThread;
		bool			shouldAbort;
		bool			noLink;

		/*
		 * we must use PostMessage not to block main thread
		 * but PostMessage doesn't work for WM_SETTEXT
		 * so we create our own function, doing the same
		 * but from the DialogProc
		 */
		static UINT WM_SETDIALOGTXTW; 

		#define setDialogTextW(id, str) \
		{ \
			wchar_t* copy = reinterpret_cast<LPWSTR>(GlobalAlloc(GPTR,(wcslen(str)+1)*2)); \
			wcscpy(copy,str); \
			PostMessage(info->hDialog, WM_SETDIALOGTXTW, id, reinterpret_cast<LPARAM>(copy)); \
		}


		static INT_PTR CALLBACK ProgressDlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		static DWORD WINAPI FtpSendThread::FtpSendThreadFunc(LPVOID arg);

		FtpSendThread() {}
	public:
		FtpSendThread(HINSTANCE hInst, WTWFUNCTIONS *fn, wtwContactDef &cnt, wtwFtpFile* ftpFile = NULL);

		inline HANDLE getThreadHandle() { return hThread; }
		inline void abort() 
		{ 
			shouldAbort = true; 
		}
	};
};