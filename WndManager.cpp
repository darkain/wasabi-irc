#if 0
#include "precomp.h"
#include "../../bfc/std.h"
#include "IRC.h"
#include "WndManager.H"
//#include "ircwnd.h"
//#include "chatwnd.h"
#include "resource.h"
//#include "IrcClient.H"


extern WACPARENT *the;
/*extern IrcConnect *irc;
extern _string *nickname;
extern _string *ircserver;
extern _int    *ircport;
*/

// {B6B9BF9D-84A7-4efa-A8B6-019E85B5C24C}
static const GUID myGuid =
{ 0xb6b9bf9d, 0x84a7, 0x4efa, { 0xa8, 0xb6, 0x1, 0x9e, 0x85, 0xb5, 0xc2, 0x4c } };


WndManager::WndManager() {
	setName(the->getName());
}


WndManager::~WndManager() {
}


int WndManager::onInit() {
/*	GuiObject *guiObj = getGuiObject();
	if (guiObj) {
	//	GuiObject *guiTxt = guiObj->guiobject_findObject("wasabi.statusbar.text");
	//	guiTxt->guiobject_setXmlParam("align", "center");
//		guiObj->guiobject_setStatusText("Status Window", 0);

		AddCommand(" Connect ", 20);
		AddCommand(" Disconnect ", 21);
		AddCommand(" Detach ", 2, 1);
		guiObj->guiobject_addAppCmds(this);
	}
	
	setDividerType(DIVIDER_HORIZONTAL);
	setResizeable(FALSE);
//	setDividerPosNoCfg(SDP_FROMTOP, 20);

//

	ChatWnd::setNextLog(irc->getStatus());
	ChatWnd *chat = new ChatWnd();
	if (isInited()) chat->init(this);
//	setChildren(top, chat);
	setChildren(chat);
*/
	return WNDMANAGER_PARENT::onInit();
}


int WndManager::onResize() {
	int retval = WNDMANAGER_PARENT::onResize();

	RootWnd *wnd = WndManager::enumRootWndChildren(0);
	if (wnd) {
		RECT r;
		WndManager::getClientRect(&r);
		wnd->resize(r.left, r.top, r.right-r.left, r.bottom-r.top);
	}

	return retval;
}


/*
void WndManager::appcmds_onCommand(int id, const RECT *buttonRect)
{
	switch (id) {
	case 2:
		{
			vChatLog *log = irc->getActiveLog();
			if (log) if (log->getLogType() != -1) log->setDocked(FALSE);
		}
		break;

	case 20:
		if (irc) irc->connectToServer();
		break;

	case 21:
		{
			char str[256];
			sprintf(str, "Disconnecting from server: %s", ircserver->getValue());
			irc->statusMessage(str);
			irc->closeSocket();
		}
		break;
	}
	AppCmdsI::appcmds_onCommand(id, buttonRect);
}
*/

//  Methods required by Window Creation Services
const char *WndManager::getWindowTypeName() { return "IRC Window Manager"; }
GUID WndManager::getWindowTypeGuid() { return myGuid; }
void WndManager::setIconBitmaps(ButtonWnd *button)
{ button->setBitmaps(the->gethInstance(), IDB_TAB_NORMAL, NULL, IDB_TAB_HILITED, IDB_TAB_SELECTED); }
#endif