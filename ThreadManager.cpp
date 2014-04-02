#include "stdinc.h"
#include "ThreadManager.h"


namespace utlFTP
{
	WTWFUNCTIONS* ThreadManager::wtw = NULL;

	DWORD WINAPI ThreadManager::CleanerThreadFunc(LPVOID arg)
	{
		ThreadManager *tm = static_cast<ThreadManager*>(arg);

		EnterCriticalSection(&tm->m_active);
		bool anythingToDo = !tm->active.empty();
		LeaveCriticalSection(&tm->m_active);

		while(anythingToDo)
		{
			EnterCriticalSection(&tm->m_active);
			iThread* thread = tm->active.top();
			LeaveCriticalSection(&tm->m_active);

			HANDLE hActThread = thread->getThreadHandle();
			if(!hActThread)
			{
				EnterCriticalSection(&tm->m_active);
				delete thread;
				tm->active.pop();
				LeaveCriticalSection(&tm->m_active);

				__LOG_F(wtw,WTW_LOG_LEVEL_NORMAL,L"FTP",L"Thread handle is NULL");
				continue;
			}
			if(tm->abortAll)
				thread->abort();
			if (WaitForSingleObject(hActThread, 1000) == WAIT_OBJECT_0)
			{			
				EnterCriticalSection(&tm->m_active);
				CloseHandle(hActThread);
				delete thread;
				tm->active.pop();
				LeaveCriticalSection(&tm->m_active);
			}

			EnterCriticalSection(&tm->m_active);
			anythingToDo = !tm->active.empty();
			LeaveCriticalSection(&tm->m_active);
		}

		EnterCriticalSection(&tm->m_active);
		if(tm->hThread)
		{
			CloseHandle(tm->hThread);
			tm->hThread = NULL;
		}
		LeaveCriticalSection(&tm->m_active);

		return 0;
	}

	ThreadManager::~ThreadManager()
	{
		abortAll = true;
		if (hThread)
			WaitForSingleObject(hThread,15000); // waiting 15 sec for all threads to finish
		DeleteCriticalSection(&m_active);
	}

	void ThreadManager::start(iThread* thread)
	{
		EnterCriticalSection(&m_active);
		if(!hThread)
		{
			DWORD id;
			hThread = CreateThread(NULL, 0, CleanerThreadFunc, this, 0, &id);
		}

		if(thread->getThreadHandle())
			active.push(thread);
		else
			delete thread;
		LeaveCriticalSection(&m_active);
	}
}
