#ifndef _MSC_VER
#error This code can be only compiled using Visual Studio
#endif

#pragma once

#define _CRT_SECURE_NO_DEPRECATE 1
#define _ATL_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <ctime>
#include <vector>
#include <Objbase.h>
#include "resource.h"
using namespace std;

#include "plInterface.h"

#include "string_utils.h"
#include "utlFTP.h"

static const wchar_t MDL[] = L"FTP";

namespace utlFTP {
	namespace config
	{
		static const wchar_t USERNAME[] = L"cfg/utlFTP/username";
		static const wchar_t PASSWORD[] = L"cfg/utlFTP/password";
		static const wchar_t FTPHOST[] = L"cfg/utlFTP/ftpHost";
		static const wchar_t HTTPHOST[] = L"cfg/utlFTP/httpHost";
		static const wchar_t DIR[] = L"cfg/utlFTP/dir";
		static const wchar_t SENDLINK[] = L"cfg/utlFTP/sendLink";
		static const wchar_t AUTOCLOSE[] = L"cfg/utlFTP/autoClose";
	};

	namespace id
	{
		static const wchar_t SEND_FILE[] = L"utlFTP/sendFile";
		static const wchar_t DEL_ALL[] = L"utlFTP/delAll";
	};

	namespace txt
	{
		static const wchar_t PLUGIN_NAME[] = L"FTP";

		static const wchar_t SEND_FILE[] = L"Wyślij plik na FTP...";
		static const wchar_t DEL_ALL[] = L"Usuń wszystkie pliki na serwerze FTP";
		static const wchar_t SETT_DESC[] = L"Wysyłanie plików na serwer FTP, wstawianie linków do nich";
		static const wchar_t DEL_ALL_ENDED[] = L"Usuwanie wszystkich plików zakończone";
		static const wchar_t DEL_ALL_PROMPT[] = L"Czy na pewno usunąć WSZYSTKIE pliki znajdujące się w katalogu roboczym na serwerze FTP?";

		static const wchar_t USERNAME[] = L"Użytkownik:";
		static const wchar_t PASSWORD[] = L"Hasło:";
		static const wchar_t FTPHOST[] = L"Server ftp:";
		static const wchar_t HTTPHOST[] = L"cfg/utlFTP/httpHost";
		static const wchar_t DIR[] = L"Katalog na serwerze:";
		static const wchar_t LINK_APPEND[] = L"Link, który zostanie wysłany z doklejoną nazwą pliku :";
		static const wchar_t SENDLINK[] = L"Nie wysyłaj linków, tylko dodawaj je do okna wpisywania";
		static const wchar_t AUTOCLOSE[] = L"Automatycznie zamykaj okno postępu po wysłaniu pliku";
	};
};
