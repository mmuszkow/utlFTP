#pragma once

#include "stdinc.h"
#include "Settings.h"
#include "SettingsPage.h"
#include "ThreadManager.h"
//#include "SpeedControl.h"

namespace utlFTP {

/** Singleton */
class PluginController
{
	// basic
	WTWFUNCTIONS*	wtw;
	HINSTANCE		hInst;
	
	wtwUtils::Settings*	settings;
	SettingsPage*	settPage;
	ThreadManager*	threadManager;
	HANDLE			menuRebuildHook;
	HANDLE			sendFileFunc;
	
	PluginController() : wtw(NULL), hInst(NULL), menuRebuildHook(NULL), 
		settings(NULL), settPage(NULL), sendFileFunc(NULL), threadManager(NULL) {}
	PluginController(const PluginController&);
	static WTW_PTR onSendFileClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onDelAllClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR onSettingsShow(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
	static WTW_PTR sendFile(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr);
public:
	static PluginController& getInstance()
	{
		static PluginController instance;
		return instance;
	}

	int onLoad(WTWFUNCTIONS *fn);
	int onUnload();
	inline const WTWPLUGINFO* getPlugInfo()
	{
		static WTWPLUGINFO _plugInfo = {
			sizeof(WTWPLUGINFO),						// struct size
			L"utlFTP",									// plugin name
			L"Wysyłanie plików na FTP",					// plugin description
			L"© 2010-2014 Maciej Muszkowski",			// copyright
			L"Maciej Muszkowski",						// author
			L"maciek.muszkowski@gmail.com",				// authors contact
			L"http://alset.pl/Maciek",					// authors webpage
			L"",										// url to xml with autoupdate data
			PLUGIN_API_VERSION,							// api version
			MAKE_QWORD(1, 1, 0, 0),						// plugin version
			WTW_CLASS_UTILITY,							// plugin class
			NULL,										// function called after "O wtyczce..." pressed
			L"{80436411-48b4-4a8b-9cd3-e7740297f2fe}",	// guid
			NULL,										// dependencies (list of guids)
			0,											// options
			0, 0, 0										// reserved
		};
		return &_plugInfo;
	}
	inline void setDllHINSTANCE(const HINSTANCE h)
	{
		hInst = h;
	}

	inline HINSTANCE getDllHINSTANCE() const
	{
		return hInst;
	}

	inline WTWFUNCTIONS* getWTWFUNCTIONS() const
	{
		return wtw;
	}

	inline wtwUtils::Settings* getSettings() {
		return settings;
	}
};

}; // namespace utlFTP
