#pragma once

#include "stdinc.h"
#include "Settings.h"

namespace utlFTP
{
	class SettingsPage
	{
		enum {HOSTFTP, USERNAME, PASSWORD, DIR, HOSTHTTP, SEND_LINK};
		static const int ROW_HEIGHT = 24;
		static const int EDIT_WIDTH = 260;
		static const int EDIT_HEIGHT = 18;
		static const int EDIT_OFFSET = 60;
		static const int EDITS = 5;

		HWND edits[EDITS];
		HWND statics[EDITS];
		HWND checkSend;
		HWND autoClose;
		wtwUtils::Settings* sett;
	public:
		SettingsPage(HWND hParent, HINSTANCE hInst, int x, int y, int cx, int cy);
		~SettingsPage();

		void show();
		void hide();
		inline void move(int x, int y, int cx, int cy) {}
		void apply();
		inline void cancel() {}
	};
};