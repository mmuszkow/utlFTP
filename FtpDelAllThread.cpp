#include "stdinc.h"
#include "FtpDelAllThread.h"
#include "PluginController.h"

namespace utlFTP
{
	DWORD WINAPI FtpDelAllThread::FtpDelAllFunc(LPVOID arg)
	{
		if (!arg)
			return 1;

		FtpDelAllThread *info = static_cast<FtpDelAllThread*>(arg);
		if (!info->wtw)
			return 2;

		wtwUtils::Settings *sett = PluginController::getInstance().getSettings();
		if (!sett) {
			__LOG_F(info->wtw, WTW_LOG_LEVEL_ERROR, MDL, L"Could not get Internet handle");
			return 3;
		}

		HINTERNET hInternet = InternetOpenW(L"utlFTP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) {
			MessageBoxInetErr(L"InternetOpen", info->wtw);
			return 3;
		}

		HINTERNET hSession = InternetConnect(hInternet, 
				sett->getWStr(config::FTPHOST).c_str(), 
				INTERNET_DEFAULT_FTP_PORT, 
				sett->getWStr(config::USERNAME).c_str(),
				sett->getWStr(config::PASSWORD).c_str(),
				INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (!hSession) {
			MessageBoxInetErr(L"InternetConnect", info->wtw);
			InternetCloseHandle(hInternet);
			return 4;
		}

		wstring dir = sett->getWStr(config::DIR);
		if(!dir.empty() && dir != L"" && FtpSetCurrentDirectory(hSession,dir.c_str())==FALSE)
		{
			MessageBoxInetErr(L"FtpSetCurrentDirectory",info->wtw);
		} // FtpSetCurrentDirectory()==FALSE
		else
		{
			WIN32_FIND_DATA findData;
			HINTERNET hFind = FtpFindFirstFile(hSession, L"*", &findData, 0, 0);
			if(hFind != NULL)
			{
				if(findData.cFileName[0] != L'.' && !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					FtpDeleteFile(hSession, findData.cFileName);

				while(InternetFindNextFile(hFind, &findData) == TRUE && !info->shouldAbort)
				{
					if(findData.cFileName[0] != L'.' && !(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
						FtpDeleteFile(hSession, findData.cFileName);
				}
				MessageBox(0,txt::DEL_ALL_ENDED,txt::PLUGIN_NAME,MB_OK);
				InternetCloseHandle(hFind);
			} // hFind != NULL
		} // // FtpSetCurrentDirectory()==TRUE
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);

		return 0;
	}

	FtpDelAllThread::FtpDelAllThread(WTWFUNCTIONS *fn) : hThread(NULL),  shouldAbort(false), wtw(fn)
	{
		DWORD id;
		HWND hMain;
		fn->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);

		if (MessageBox(hMain, txt::DEL_ALL_PROMPT, txt::PLUGIN_NAME, MB_YESNO) == IDYES)
			hThread = CreateThread(NULL, 0, FtpDelAllFunc, this, 0, &id);
	}

}