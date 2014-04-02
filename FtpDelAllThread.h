#pragma once

#include "iThread.h"

namespace utlFTP
{
	class FtpDelAllThread : public iThread
	{
		HANDLE			hThread;
		bool			shouldAbort;
		WTWFUNCTIONS*	wtw;

		static DWORD WINAPI FtpDelAllThread::FtpDelAllFunc(LPVOID arg);
	public:
		FtpDelAllThread(WTWFUNCTIONS *fn);

		inline HANDLE getThreadHandle() { return hThread; }
		inline void abort() { shouldAbort = true; }
	};
};