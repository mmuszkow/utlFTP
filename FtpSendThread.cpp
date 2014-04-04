#include "stdinc.h"
#include "FtpSendThread.h"
#include "PluginController.h"

namespace utlFTP
{
	UINT FtpSendThread::WM_SETDIALOGTXTW = RegisterWindowMessage(L"utlFTPsetDialogTxtW");

	DWORD WINAPI FtpSendThread::FtpSendThreadFunc(LPVOID arg)
	{
		if (!arg)
			return 1;

		FtpSendThread *info = static_cast<FtpSendThread*>(arg);
		if (!info->wtw || !info->hInst)
			return 2;

		WTWFUNCTIONS* wtw = info->wtw;
		wtwUtils::Settings* sett = PluginController::getInstance().getSettings();
		if (!sett) {
			__LOG_F(wtw, WTW_LOG_LEVEL_ERROR, MDL, L"Could not load settings");
			return 3;
		}

		HINTERNET hInternet = InternetOpenW(L"utlFTP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) {
			MessageBoxInetErr(L"InternetOpen", info->wtw);
			return 4;
		}

		wstring host = sett->getWStr(config::FTPHOST).c_str();
		INTERNET_PORT port = INTERNET_DEFAULT_FTP_PORT;
		size_t pos;
		if((pos = host.find(L":")) != wstring::npos)
		{
			port = _wtoi(host.substr(pos+1).c_str());
			host = host.substr(0,pos);
		}

		HINTERNET hSession = InternetConnect(hInternet, 
			host.c_str(), 
			port, 
			sett->getWStr(config::USERNAME).c_str(),
			sett->getWStr(config::PASSWORD).c_str(),
			INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (!hSession) {
			MessageBoxInetErr(L"InternetConnect", info->wtw);
			InternetCloseHandle(hInternet);
			return 5;
		}

		wstring dir = sett->getWStr(config::DIR);
		if(!dir.empty() && dir != L"" && FtpSetCurrentDirectory(hSession,dir.c_str())==FALSE)
		{
			MessageBoxInetErr(L"FtpSetCurrentDirectory",wtw);
		} // FtpSetCurrentDirectory()==FALSE
		else
		{
			unsigned int i = 0;
			while(i < info->files.size() && !info->shouldAbort)
			{
				wstring path(info->files[i]);
				wstring filename;
				WIN32_FIND_DATA findData;
				HINTERNET hFind = FtpFindFirstFile(hSession, extractFilename(path).c_str(), &findData, 0, 0);
				if(hFind != NULL) // if exists add timestamp to begining of it's name
				{
					wchar_t time_str[32];
					wsprintf(time_str,L"%d",time(NULL));
					filename = time_str;
					filename += L"_";
					filename += extractFilename(path);
					InternetCloseHandle(hFind);
				}
				else
					filename = extractFilename(path);
							

				HINTERNET hFile = FtpOpenFile(hSession,filename.c_str(),GENERIC_WRITE,FTP_TRANSFER_TYPE_UNKNOWN,0);
				if(hFile != NULL)
				{
					FILE* f = _wfopen(path.c_str(), L"rb");
					DWORD toWrite = 0;
					DWORD bytesWritten = 0;
					DWORD sent = 0;
					DWORD sentSinceLast = 0;
					if(f)
					{
						char buff[4096];
						wstringstream val;

						//// checking file size
						fseek(f, 0, SEEK_END);
						DWORD file_size = ftell(f);
						fseek(f, 0, SEEK_SET);
								
						//// initial labels
						setDialogTextW(-1, filename.c_str());
						setDialogTextW(IDC_NAME, path.c_str());
						val << sent << L"B/" << file_size << L"B";
						setDialogTextW(IDC_PROGRESS, val.str().c_str());
						setDialogTextW(IDC_SPEED, L"0 KB/s");
						setDialogTextW(IDC_LEFT, L"00:00:00");
						PostMessage(GetDlgItem(info->hDialog,IDC_BAR), PBM_SETRANGE32, 0, file_size);
						ShowWindow(info->hDialog, SW_SHOWNORMAL);

						time_t start = time(NULL);
						time_t last = start;
						BOOL writeSucc;
						while((toWrite = fread(buff, 1, 4096, f)) && ((writeSucc = InternetWriteFile(hFile,buff,toWrite,&bytesWritten))==TRUE) && !info->shouldAbort) 
						{
							sent += bytesWritten;
							sentSinceLast += bytesWritten;
							if(info->hDialog)
							{
								if(GetProp(info->hDialog, L"ABORT")!=NULL)
								{
									info->shouldAbort = true;
									break;
								}
								val.str(L"");
								val << niceBytes(static_cast<float>(sent)) << L" / " << niceBytes(static_cast<float>(file_size));
								setDialogTextW(IDC_PROGRESS, val.str().c_str());
										
								if (difftime(time(NULL), last) != 0)
								{
									float speedB = static_cast<float>(sentSinceLast / difftime(time(NULL), last)); // B/s
									val.str(L""); // formatting speed/s
									val << niceBytes(speedB) << L"/s";
									setDialogTextW(IDC_SPEED, val.str().c_str());
									int sec_left = static_cast<int>((file_size-sent) / speedB); // s
									val.str(L""); // formatting HH:MM:SS
									val.fill(L'0');
									val.width(2);
									val << right << sec_left / 3600 << L":";
									val.width(2);
									val << right << (sec_left % 3600) / 60 << L":";
									val.width(2);
									val << right << (sec_left % 3600) % 60;
									setDialogTextW(IDC_LEFT, val.str().c_str());
									//PostMessage(GetDlgItem(info->hDialog,IDC_CHART), SpeedCtrl::WM_NEWVAL, static_cast<WPARAM>(speedB), 0);
									last = time(NULL);
									sentSinceLast = 0;
								}
								PostMessage(GetDlgItem(info->hDialog,IDC_BAR), PBM_SETPOS, sent, 0);
							}
						}
						fclose(f);
						InternetCloseHandle(hFile);

						if(writeSucc==FALSE) // something has interrupted the transmision
						{
							MessageBoxInetErr(L"InternetWriteFile",wtw);
							info->shouldAbort = true;
						}

						if (info->shouldAbort)
						{
							//if(info->hDialog)
							//	SendMessage(info->hDialog, WM_COMMAND, IDCANCEL, 0);					
							if(FtpDeleteFile(hSession,filename.c_str())==FALSE)
								__LOG_F(wtw, WTW_LOG_LEVEL_NORMAL, MDL, L"Could not delete file %s", filename.c_str());
						} // shouldAbort
						else
						{
							wstring http = sett->getWStr(config::HTTPHOST);
							if (!info->noLink)
							{
								if (http.size() > 0 && http[http.size() - 1] != '\\' && http[http.size() - 1] != '/')
									http += L"/";
								http += urlencode(filename);

								wtwMessageDef msg;
								msg.contactData = info->contact;
								msg.msgMessage = http.c_str();
								msg.msgFlags = WTW_MESSAGE_FLAG_OUTGOING | WTW_MESSAGE_FLAG_CHAT_MSG;
								msg.msgTime = time(NULL);
								/*TIME_ZONE_INFORMATION tz;
								switch(GetTimeZoneInformation(&tz))
								{
								case TIME_ZONE_ID_UNKNOWN: msg.msgTime += tz.Bias*60; break;
								case TIME_ZONE_ID_STANDARD:msg.msgTime += tz.StandardBias*60; break;
								case TIME_ZONE_ID_DAYLIGHT:msg.msgTime += tz.DaylightBias*60;
								}*/
								if (!sett->getInt(config::SENDLINK, 1))
								{
									wchar_t fn[512] = {0};
									wsprintf(fn, L"%s/%d/%s", info->contact.netClass, info->contact.netId, WTW_PF_MESSAGE_SEND);
									wtw->fnCall(fn, reinterpret_cast<WTW_PARAM>(&msg), NULL);
									wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&msg), NULL);
								}
								else
								{
									wtw->fnCall(WTW_FUNCT_OPEN_CHAT_WINDOW, reinterpret_cast<WTW_PARAM>(&info->contact), NULL);
											
									wtwChatWndText text;
									text.contactInfo = info->contact;
											
									text.buffer.pBuffer = NULL; // get the buffer size
									text.buffer.bufferSize = -1;
									WTW_PTR res = wtw->fnCall(WTW_CHATWND_INPUT_TEXT_GET, reinterpret_cast<WTW_PARAM>(&text), NULL);
									if(res == S_OK)
									{
										wstring	textToPut;
										if(text.buffer.bufferSize != 0)
										{
											text.buffer.bufferSize++;
											text.buffer.pBuffer = new wchar_t[text.buffer.bufferSize]; // get text
											res = wtw->fnCall(WTW_CHATWND_INPUT_TEXT_GET, reinterpret_cast<WTW_PARAM>(&text), NULL);
											if(res == S_OK)
												textToPut += text.buffer.pBuffer;
											else
												__LOG_F(wtw, WTW_LOG_LEVEL_ERROR, MDL, L"Could not get input window text, result 0x%X", res);
											delete [] text.buffer.pBuffer;
										}

										textToPut += msg.msgMessage; // add link(s)
										if(i != info->files.size()-1)
											textToPut += L"\r\n";

										text.buffer.pBuffer = const_cast<wchar_t*>(textToPut.c_str()); // set new text
										text.buffer.bufferSize = textToPut.size();
										wtw->fnCall(WTW_CHATWND_INPUT_TEXT_SET, reinterpret_cast<WTW_PARAM>(&text), NULL);
									}
									else
									{
										__LOG_F(wtw, WTW_LOG_LEVEL_NORMAL, MDL, L"Could not get input window text length, result 0x%X", res);
									}
								}
							}
						}
					}
					else
					{
						MessageBoxInetErr(L"_wfopen",info->wtw);
						if(FtpDeleteFile(hSession,filename.c_str())==FALSE)
							__LOG_F(info->wtw, WTW_LOG_LEVEL_NORMAL, MDL, L"Could not delete file %s", filename.c_str());
						info->shouldAbort = true;
					}
				}  // hFile != NULL
				else
				{
					MessageBoxInetErr(L"FtpOpenFile",info->wtw);
					info->shouldAbort = true;
				}
			i++;
			}
		}
		InternetCloseHandle(hSession);
		InternetCloseHandle(hInternet);

		if (info->hDialog && !info->shouldAbort && sett->getInt(config::AUTOCLOSE, 0))
			PostMessage(info->hDialog, WM_COMMAND, IDCANCEL, 0);

		return 0;
	}

	FtpSendThread::FtpSendThread(HINSTANCE hInst, WTWFUNCTIONS *fn, wtwContactDef &cnt, wtwFtpFile* ftpFile)	: 
		hInst(hInst), wtw(fn), hDialog(NULL), contact(cnt), hThread(NULL), shouldAbort(false), noLink(false)
	{
		DWORD id;
		//// Getting the file name
		HWND hMain;
		
		wtw->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);
		if (!hMain) {
			__LOG_F(fn, WTW_LOG_LEVEL_ERROR, MDL, L"Failed to get main window handle");
			return;
		}

		if (!ftpFile) // if using open dialog
		{
			wchar_t path[MAX_PATH+1] = {0};
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof(OPENFILENAME));
			wchar_t dir[MAX_PATH+1];
			GetCurrentDirectoryW(MAX_PATH, dir);
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hMain;
			ofn.hInstance = hInst;
			ofn.lpstrFilter = L"Wszystkie pliki\0*.*\0";
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFile = path;
			ofn.lpstrInitialDir = dir;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				wstring dir(path);
				dir = dir.substr(0,ofn.nFileOffset);
				if(dir.size()==0)
					return;
				if(dir[dir.size()-1]!=L'\\' && dir[dir.size()-1]!=L'/')
					dir += L"\\";
				int pos = ofn.nFileOffset;
				while(pos < MAX_PATH + 1)
				{
					wstring filename(path + pos);
					if(filename.size()==0)
						break;
					else
					{
						wstring filepath(dir);
						filepath += filename;
						files.push_back(filepath);
					}
					pos+=filename.size()+1;
				}
				hDialog = CreateDialog(hInst,MAKEINTRESOURCE(IDD_FTP_DIALOG),hMain,ProgressDlgProc);
				hThread = CreateThread(NULL, 0, FtpSendThreadFunc, this, 0, &id);
			}
			else
			{
				if(CommDlgExtendedError()==FNERR_BUFFERTOOSMALL)
					MessageBox(hMain,L"Zbyt d³uga œcie¿ka lub zbyt wiele wybranych plików",L"FTP",MB_OK);
			}
		} // if using API
		else
		{
			if(ftpFile->flags & flag::WTW_FTP_MULTIFILE)
			{
				wstring dir(ftpFile->filePath);
				if(dir.size()==0)
					return;
				if(dir[dir.size()-1]!=L'\\' && dir[dir.size()-1]!=L'/')
					dir += L"\\";
				int pos = dir.size();
				while(pos < MAX_PATH + 1)
				{
					wstring filename(ftpFile->filePath + pos);
					if(filename.size()==0)
						break;
					else
					{
						wstring filepath(dir);
						filepath += filename;
						files.push_back(filepath);
					}
					pos+=filename.size()+1;
				}
			}
			else
				files.push_back(ftpFile->filePath);

			if(!(ftpFile->flags & flag::WTW_FTP_NODIALOG))
				hDialog = CreateDialog(hInst,MAKEINTRESOURCE(IDD_FTP_DIALOG),hMain,ProgressDlgProc);
			if(ftpFile->flags & flag::WTW_FTP_NOLINK)
				noLink = true;
			hThread = CreateThread(NULL, 0, FtpSendThreadFunc, this, 0, &id);
		}
	}
	
	INT_PTR CALLBACK FtpSendThread::ProgressDlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg == WM_COMMAND && LOWORD(wParam) == IDABORT)
		{
			SetProp(hwnd, L"ABORT", reinterpret_cast<HANDLE>(1));
			return TRUE;
		} else
		if (Msg == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
		{
			DestroyWindow(hwnd);
			return TRUE;
		} else
		if (Msg == WM_SETDIALOGTXTW)
		{
			if(wParam==-1)
				SetWindowText(hwnd,reinterpret_cast<LPCWSTR>(lParam));
			else
				SetWindowText(GetDlgItem(hwnd,static_cast<int>(wParam)),reinterpret_cast<LPCWSTR>(lParam));
			GlobalFree(reinterpret_cast<HGLOBAL>(lParam));
			return TRUE;
		}
		return FALSE;
	}


}