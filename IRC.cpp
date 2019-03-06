// Always start with std.h
#include "precomp.h"
#include "../../bfc/std.h"
#include <time.h>

#include "IRC.h"
#include "resource.h"
#include "IRCwnd.h"
#include "IRCClient.H"
#include "ChatLog.H"
#include "ChatWnd.H"
#include "Toolbar.H"
#include "Sendbar.H"
#include "About.H"
#include "WndManager.H"

#include "../../common/xlatstr.h"
#include "../../studio/services/creators.h"
#include "../../bfc/wndcreator.h"
#include "../../studio/services/svc_action.h"

#include "ScriptObject.H"


static WACNAME wac;
WACPARENT *the = &wac;

IrcConnect *irc = NULL;
LRESULT CALLBACK WndPrc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


_string *nickname;
_string *ircserver;
_int    *ircport;
_bool   *rawirc;
_bool   *ircpings;
_bool   *showprefs;


//extern WndManager *mainManager;
//extern vChannelList *channels;


// {8DDA9D48-B915-4320-A888-831A1D837516} - about window

// {B6B9BF9D-84A7-4efa-A8B6-019E85B5C24C}5
static const GUID guid =
{ 0xb6b9bf9d, 0x84a7, 0x4efa, { 0xa8, 0xb6, 0x1, 0x9e, 0x85, 0xb5, 0xc2, 0x4c } };



WACNAME::WACNAME() : WACPARENT("Wasabi IRC Window Manager") {
	_tzset();
	registerService(new XuiObjectCreator<ToolbarXuiSvc>);
	registerService(new XuiObjectCreator<ChannelbarXuiSvc>);
	registerService(new XuiObjectCreator<ChatLogXuiSvc>);
	registerService(new XuiObjectCreator<SendbarXuiSvc>);

//	registerService(new WndCreateCreatorSingle< CreateWndByGuid<WndManager> >);
	registerService(new WndCreateCreatorSingle< CreateWndByGuid<ChatWnd> >);


//	registerService(new WndCreateCreatorSingle< CreateBucketItem<WndManager> >);
//	registerAutoPopup(ChatWnd::getGUID(), getName());

	//this about window WILL conflict with the WA3 about window
	//	registerService(new WndCreateCreatorSingle< CreateWndByGuid<AboutWnd> >);
	//	registerAutoPopup(AboutWnd::getWindowTypeGuid(), AboutWnd::getWindowTypeName());

//  registerService(new ScriptObjectCreator<IrcScriptObjectSvc>);
}


WACNAME::~WACNAME() {}

GUID WACNAME::getGUID() { return guid; }


void WACNAME::onCreate() {
/*	vBase::setInstance(gethInstance());
	vBase::setWindow(CreateDialog(vBase::getInstance(), MAKEINTRESOURCE(IDD_DLG), NULL, (DLGPROC)WndPrc));
	ShowWindow(vBase::getWindow(), SW_HIDE);
*/

	WORD version = MAKEWORD(1,1);
	WSADATA wsaData;
	WSAStartup(version, &wsaData);

	if (api->main_getBuildNumber() > 489) registerSkinFile("xml/irc/irc.xml"); 
	else registerSkinFile( "wacs/xml/irc/irc.xml" ); 
	api->preferences_registerGroup("wIRC.Preferences", "Wasabi-IRC", getGUID());

	nickname = new _string("Nickname", "Guest");
	ircserver = new _string("Server", "irc.zirc.org");
	ircport = new _int("Port", 6667);
	rawirc = new _bool("Raw IRC");
	ircpings = new _bool("Show Pings");
	showprefs = new _bool("Show Preferences", 1);

	registerAttribute(nickname);
	registerAttribute(ircserver);
	registerAttribute(ircport);
	registerAttribute(rawirc);
	registerAttribute(ircpings);
	registerAttribute(showprefs);

	irc = new IrcConnect;
//	irc->setNick(nickname->getValue());
//	irc->setServer(ircserver->getValue());
//	irc->ConnectSocket((char*)irc->getServer(), 6667);
	
	timerclient_setTimer(1, 100);
	timerclient_setTimer(2, 100);
}


void WACNAME::onDestroy()
{
	timerclient_killTimer(1);
	delAll();
	irc->deregisterDeleteLogs();
	delete irc;
	irc = NULL;
	WSACleanup();
}


LRESULT CALLBACK WndPrc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG) return TRUE;
	if (irc) return irc->WndPrc(hwnd, msg, wParam, lParam);
	else return FALSE;
}



//int vBase::msgBox(const char *txt, const char *title) { return 0; }


void WACNAME::onDelete(LPvIrcMessage data)
{
	free(data);
}


void IrcConnect::sendWndMsg(vtagIrcWindow *msg) {
	wac.addToEnd((LPvIrcMessage)msg);
}


void WACNAME::timerclient_timerCallback(int id) {
	switch (id) {
	case 2:
		timerclient_killTimer(2);
		if (!!showprefs->getValueAsInt()) {
			const char *action = FALSE ? "Preferences:OPENMODAL" : "Preferences:OPEN";
			svc_action *pref = ActionEnum(action).getFirst();
			if (!pref) return;
			char strguid[nsGUID::GUID_STRLEN+1];
			nsGUID::toChar(getGUID(), strguid);
			pref->onAction(action, strguid);
			SvcEnum::release(pref);
		}
		break;

	case 1:
		{
			LPvIrcMessage msg = getFirst();
			if (msg) {
				LPvIrcMessage newMsg = (LPvIrcMessage) malloc(msg->size);
				memcpy(newMsg, msg, msg->size);
				delFirst();

				switch (newMsg->msg) {
				case 1: {
						LPvIrcWindow msg2 = (LPvIrcWindow)newMsg;
						msg2->log->activate();
					}
					break;

				case 2:
					{
						LPvIrcWindow msg2 = (LPvIrcWindow)newMsg;
						if (irc) {
							/*
							ServerObject *o = irc->getFirstObject();
							while (o) {
								o->onPart(msg2->log->getName());
								o = irc->getNextObject(o);
							}
							*/
						}
						if (msg2->log->getActivated()) {
							vChatLog *log = irc->getStatus();
							if (log) log->activate();
						}
						irc->deleteLog(msg2->log);
					}
					break;

				case 3:
					{
						if (irc) {
							LPvIrcWindow msg2 = (LPvIrcWindow)newMsg;
							/*
							ServerObject *o = irc->getFirstObject();
							while (o)
							{
								o->onJoin(msg2->log->getName());
								o = irc->getNextObject(o);
							}*/
						}
					}
					break;
				}
				free(newMsg);
			}
		}
		break;
	}
}






/*
  stringAttr("Example String Attribute","Default Value"),
*/