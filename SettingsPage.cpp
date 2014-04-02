#include "stdinc.h"
#include "SettingsPage.h"
#include "PluginController.h"

using namespace utlFTP;

SettingsPage::SettingsPage(HWND hParent, HINSTANCE hInst, int x, int y, int cx, int cy)
{
	statics[HOSTFTP] = CreateWindow(L"STATIC", txt::FTPHOST, WS_CHILD,
		x, y, cx, ROW_HEIGHT, hParent, 0, hInst, 0);
	statics[USERNAME] = CreateWindow(L"STATIC", txt::USERNAME, WS_CHILD,
		x, y + 2 * ROW_HEIGHT, cx, ROW_HEIGHT, hParent, 0, hInst, 0);
	statics[PASSWORD] = CreateWindow(L"STATIC", txt::PASSWORD, WS_CHILD,
		x, y + 4 * ROW_HEIGHT, cx, ROW_HEIGHT, hParent, 0, hInst, 0);
	statics[DIR] = CreateWindow(L"STATIC", txt::DIR, WS_CHILD,
		x, y + 6 * ROW_HEIGHT, cx, ROW_HEIGHT, hParent, 0, hInst, 0);
	statics[HOSTHTTP] = CreateWindow(L"STATIC", txt::LINK_APPEND, WS_CHILD,
		x, y + 8 * ROW_HEIGHT, cx, ROW_HEIGHT, hParent, 0, hInst, 0);

	PluginController& pc = PluginController::getInstance();
	sett = pc.getSettings();
	if (!sett) {
		__LOG_F(pc.getWTWFUNCTIONS(), WTW_LOG_LEVEL_ERROR, MDL, L"Could not get settings");
		return;
	}

	edits[HOSTFTP] = CreateWindow(L"EDIT", sett->getWStr(config::FTPHOST).c_str(), WS_CHILD | WS_BORDER,
		x + EDIT_OFFSET, y + ROW_HEIGHT, EDIT_WIDTH, EDIT_HEIGHT, hParent, (HMENU)HOSTFTP, hInst, 0);
	edits[USERNAME] = CreateWindow(L"EDIT", sett->getWStr(config::USERNAME).c_str(), WS_CHILD | WS_BORDER,
		x + EDIT_OFFSET, y + 3 * ROW_HEIGHT, EDIT_WIDTH, EDIT_HEIGHT, hParent, (HMENU)USERNAME, hInst, 0);
	edits[PASSWORD] = CreateWindow(L"EDIT", sett->getWStr(config::PASSWORD).c_str(), WS_CHILD | WS_BORDER | ES_PASSWORD,
		x + EDIT_OFFSET, y + 5 * ROW_HEIGHT, EDIT_WIDTH, EDIT_HEIGHT, hParent, (HMENU)PASSWORD, hInst, 0);
	edits[DIR] = CreateWindow(L"EDIT", sett->getWStr(config::DIR).c_str(), WS_CHILD | WS_BORDER,
		x + EDIT_OFFSET, y + 7 * ROW_HEIGHT, EDIT_WIDTH, EDIT_HEIGHT, hParent, (HMENU)DIR, hInst, 0);
	edits[HOSTHTTP] = CreateWindow(L"EDIT", sett->getWStr(config::HTTPHOST).c_str(), WS_CHILD | WS_BORDER,
		x + EDIT_OFFSET, y + 9 * ROW_HEIGHT, EDIT_WIDTH, EDIT_HEIGHT, hParent, (HMENU)HOSTHTTP, hInst, 0);

	checkSend = CreateWindow(L"BUTTON", txt::SENDLINK, WS_CHILD | BS_AUTOCHECKBOX,
		x, y + 11 * ROW_HEIGHT, cx, EDIT_HEIGHT, hParent, (HMENU)SEND_LINK, hInst, 0);
	autoClose = CreateWindow(L"BUTTON", txt::AUTOCLOSE, WS_CHILD | BS_AUTOCHECKBOX,
		x, y + 12 * ROW_HEIGHT, cx, EDIT_HEIGHT, hParent, (HMENU)SEND_LINK, hInst, 0);

	HFONT hDefaultFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	for (int i = 0; i<EDITS; i++)
	{
		SendMessage(statics[i], WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
		SendMessage(edits[i], WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
	}
	SendMessage(checkSend, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));
	SendMessage(autoClose, WM_SETFONT, reinterpret_cast<WPARAM>(hDefaultFont), MAKELPARAM(TRUE, 0));

	if (sett->getInt(config::SENDLINK, 1))
		SendMessage(checkSend, BM_SETCHECK, 1, 0);
	if (sett->getInt(config::AUTOCLOSE, 0))
		SendMessage(autoClose, BM_SETCHECK, 1, 0);
}

SettingsPage::~SettingsPage()
{
	for (int i = 0; i<EDITS; i++)
	{
		DestroyWindow(statics[i]);
		DestroyWindow(edits[i]);
	}
	DestroyWindow(checkSend);
	DestroyWindow(autoClose);
}

void SettingsPage::show()
{
	for (int i = 0; i<EDITS; i++)
	{
		ShowWindow(statics[i], SW_SHOW);
		ShowWindow(edits[i], SW_SHOW);
	}
	ShowWindow(checkSend, SW_SHOW);
	ShowWindow(autoClose, SW_SHOW);
}
void SettingsPage::hide()
{
	for (int i = 0; i<EDITS; i++)
	{
		ShowWindow(statics[i], SW_HIDE);
		ShowWindow(edits[i], SW_HIDE);
	}
	ShowWindow(checkSend, SW_HIDE);
	ShowWindow(autoClose, SW_HIDE);
}

void SettingsPage::apply()
{
	wchar_t buff[256];

	GetWindowText(edits[HOSTFTP], buff, 256);
	sett->setStr(config::FTPHOST, buff);
	GetWindowText(edits[USERNAME], buff, 256);
	sett->setStr(config::USERNAME, buff);
	GetWindowText(edits[PASSWORD], buff, 256);
	sett->setStr(config::PASSWORD, buff);
	GetWindowText(edits[DIR], buff, 256);
	sett->setStr(config::DIR, buff);
	GetWindowText(edits[HOSTHTTP], buff, 256);
	sett->setStr(config::HTTPHOST, buff);

	if (SendMessage(checkSend, BM_GETCHECK, 0, 0))
		sett->setInt(config::SENDLINK, 1);
	else
		sett->setInt(config::SENDLINK, 0);

	if (SendMessage(autoClose, BM_GETCHECK, 0, 0))
		sett->setInt(config::AUTOCLOSE, 1);
	else
		sett->setInt(config::AUTOCLOSE, 0);
}
