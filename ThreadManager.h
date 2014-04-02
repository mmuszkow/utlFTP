#pragma once

#include "iThread.h"

namespace utlFTP
{
	class ThreadManager
	{
		CRITICAL_SECTION	m_active;
		stack<iThread*>		active;
		HANDLE				hThread;
		static WTWFUNCTIONS* wtw;
		bool				abortAll;

		static DWORD WINAPI ThreadManager::CleanerThreadFunc(LPVOID arg);


	public:
		ThreadManager(WTWFUNCTIONS* fn) : hThread(NULL), abortAll(false)
		{
			InitializeCriticalSection(&m_active);
			ThreadManager::wtw = fn;
		}
		~ThreadManager();

		void start(iThread* thread);
	};
};
