#pragma once

#include "stdinc.h"

namespace utlFTP
{
	static void MessageBoxInetErr(const wchar_t* func, WTWFUNCTIONS* wtw)
	{
		wchar_t pBuffer[256] = {0};
		int lasterr = GetLastError();
		DWORD extCode = 0;
		wchar_t extMsg[256] = {0};
		DWORD extSize = 255;
		if(lasterr == ERROR_INTERNET_EXTENDED_ERROR)
			 InternetGetLastResponseInfo(&extCode,extMsg,&extSize);

		HWND hMain;
		wtw->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);

		HMODULE hModule = LoadLibraryEx(L"wininet.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | ((hModule != NULL) ? FORMAT_MESSAGE_FROM_HMODULE : 0),
			hModule, lasterr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pBuffer, 255, NULL))
		{
			if(extMsg && wcslen(extMsg) > 0)
			{
				MessageBox(hMain,extMsg,L"FTP",MB_ICONERROR);
			}
			else
			{
				wstringstream str;
				str << func << L" [" << lasterr << "] " << pBuffer;
				MessageBox(hMain,str.str().c_str(),L"FTP",MB_ICONERROR);
			}
		}
		if(hModule != NULL) 
			FreeLibrary (hModule);
	}

	class iThread
	{
	public:
		virtual HANDLE getThreadHandle() = 0;
		virtual void abort() = 0;
	};
};