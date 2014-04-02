#include "stdinc.h"
#include "PluginController.h"
#include "FtpSendThread.h"
#include "FtpDelAllThread.h"
#include "ThreadManager.h"
#include "wtwInputWindows.h"

namespace utlFTP
{
	int PluginController::onLoad(WTWFUNCTIONS *fn)
	{
		wtw = fn;

		settings = new wtwUtils::Settings(fn, hInst);
		threadManager = new ThreadManager(wtw);

		wtwOptionPageDef pg;
		pg.id = getPlugInfo()->pluginGUID;
		pg.parentId = WTW_OPTIONS_GROUP_PLUGINS;
		pg.caption = txt::PLUGIN_NAME;
		pg.callback = onSettingsShow; 
		pg.cbData = this;
		wtw->fnCall(WTW_OPTION_PAGE_ADD, reinterpret_cast<WTW_PARAM>(hInst), reinterpret_cast<WTW_PARAM>(&pg));

		wtwMenuItemDef menuDef;
		menuDef.menuID = WTW_MENU_ID_CONTACT_SEND;
		menuDef.callback = PluginController::onSendFileClick;
		menuDef.itemId = id::SEND_FILE;
		menuDef.menuCaption = txt::SEND_FILE;
		menuDef.cbData = this;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&menuDef), NULL);

		menuDef.menuID = WTW_MENU_ID_MAIN_OPT;
		menuDef.callback = PluginController::onDelAllClick;
		menuDef.itemId = id::DEL_ALL;
		menuDef.menuCaption = txt::DEL_ALL;
		menuDef.cbData = this;
		wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&menuDef), NULL);

		menuRebuildHook = wtw->evHook(WTW_EVENT_MENU_REBUILD, onMenuRebuild, NULL);

		sendFileFunc = wtw->fnCreate(WTW_FTP_SEND_FILE, sendFile, &PluginController::getInstance());

		return 0;
	}

	int PluginController::onUnload()
	{
		if(sendFileFunc)
			wtw->fnDestroy(sendFileFunc);

		wtw->fnCall(WTW_OPTION_PAGE_REMOVE, reinterpret_cast<WTW_PARAM>(hInst), reinterpret_cast<WTW_PARAM>(getPlugInfo()->pluginGUID));

		wtwMenuItemDef menuDef;	
		menuDef.menuID = WTW_MENU_ID_CONTACT_SEND;
		menuDef.itemId = id::SEND_FILE;
		wtw->fnCall(WTW_MENU_ITEM_DELETE, reinterpret_cast<WTW_PARAM>(&menuDef), NULL);

		menuDef.menuID = WTW_MENU_ID_MAIN_OPT;
		menuDef.itemId = id::DEL_ALL;
		wtw->fnCall(WTW_MENU_ITEM_DELETE, reinterpret_cast<WTW_PARAM>(&menuDef), NULL);

		if(menuRebuildHook)
			wtw->evUnhook(menuRebuildHook);

		if(settPage)
			delete settPage;

		// after settings page!
		if (settings)
			delete settings;

		if(threadManager)
			delete threadManager;

		return 0;
	}

	WTW_PTR PluginController::onSendFileClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		PluginController *pc = static_cast<PluginController*>(ptr);
		wtwMenuPopupInfo* menuPopupInfo = reinterpret_cast<wtwMenuPopupInfo*>(lParam);
		pc->threadManager->start(new FtpSendThread(pc->hInst,pc->wtw,menuPopupInfo->pContacts[0]));
		return 0;
	}

	WTW_PTR PluginController::onMenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwMenuCallbackEvent * event = reinterpret_cast<wtwMenuCallbackEvent*>(wParam);
		event->slInt.add(event->itemsToShow, id::SEND_FILE);
		return 0;
	}

	WTW_PTR PluginController::onSettingsShow(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		PluginController* plugInst = static_cast<PluginController*>(ptr);
		wtwOptionPageShowInfo* info = reinterpret_cast<wtwOptionPageShowInfo*>(wParam);

		wcscpy(info->windowCaption, txt::PLUGIN_NAME);
		wcscpy(info->windowDescrip, txt::SETT_DESC);

		if (!plugInst->settPage)
			plugInst->settPage = new SettingsPage(info->handle,plugInst->hInst,info->x,info->y,info->cx,info->cy);

		switch (info->action)
		{
		case WTW_OPTIONS_PAGE_ACTION_SHOW:
			plugInst->settPage->show();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_MOVE:
			plugInst->settPage->move(info->x, info->y, info->cx, info->cy);
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_HIDE:
			plugInst->settPage->hide();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_CANCEL:
			plugInst->settPage->cancel();
			delete plugInst->settPage;
			plugInst->settPage = NULL;
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_APPLY:
			plugInst->settPage->apply();
			return 0;
		case WTW_OPTIONS_PAGE_ACTION_OK:
			plugInst->settPage->apply();
			delete plugInst->settPage;
			plugInst->settPage = NULL;
			return 0;
		}

		return 0;
	}

	WTW_PTR PluginController::sendFile(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		wtwFtpFile* info = reinterpret_cast<wtwFtpFile*>(wParam);
		if(!info)
			return -1;
		if(info->structSize != sizeof(wtwFtpFile))
			return -2;
		PluginController *pc = static_cast<PluginController*>(ptr);
		pc->threadManager->start(new FtpSendThread(pc->hInst,pc->wtw,info->contact,info));
		return 0;
	}

	WTW_PTR PluginController::onDelAllClick(WTW_PARAM wParam, WTW_PARAM lParam, void *ptr)
	{
		PluginController *pc = static_cast<PluginController*>(ptr);
		pc->threadManager->start(new FtpDelAllThread(pc->getWTWFUNCTIONS()));
		return 0;
	}
};
