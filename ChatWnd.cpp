#include "precomp.h"
#include "../../bfc/std.h"

#include "ChatWnd.H"
#include "ircwnd.h"
#include "Resource.H"
#include "IRC.H"
#include "ircclient.h"
#include "WndManager.H"
#include "ChannelLog.H"

#include "../../bfc/wnds/treewnd.h"


// {B39B264B-9EED-4f6e-89B8-D75058BFCD9B}
static const GUID guidChatWnd = 
{ 0xb39b264b, 0x9eed, 0x4f6e, { 0x89, 0xb8, 0xd7, 0x50, 0x58, 0xbf, 0xcd, 0x9b } };

extern WACPARENT *the;
extern IrcConnect *irc;
extern _string *nickname;
extern _string *ircserver;
extern _int    *ircport;

static vChatLog *nextLog = NULL;


char ChatLogXuiObjectStr[] = "wIRC:ChatLog";
char ChatLogXuiSvcName[]   = "wIRC:ChatLog XuiObject Service";


void ChatWnd::setNextLog(vChatLog *log) { nextLog = log; }
const char *ChatWnd::getWindowTypeName() { return "Chat Window"; }
void ChatWnd::setIconBitmaps(ButtonWnd *button)
{ button->setBitmaps(the->gethInstance(), IDB_TAB_NORMAL, NULL, IDB_TAB_HILITED, IDB_TAB_SELECTED); }


GUID ChatWnd::getGUID() { return guidChatWnd; }


GUID ChatWnd::getWindowTypeGuid() {
	if (nextLog) return guidChatWnd;
	else return INVALID_GUID;
//	return guidChatWnd;
}


ChatWnd::ChatWnd() {
	textwnd = NULL;
	treewnd = NULL;
	undocked = FALSE;

	if (nextLog) {
		assignLog(nextLog);
		nextLog = NULL;
		undocked = TRUE;
	}

	CHATWND_PARENT::setName(vChannelViewer::getNameSafe("Chat Window"));
}


ChatWnd::~ChatWnd() {
	if (undocked) if (!getDocked()) part();
}


int ChatWnd::setXmlParam(const char *name, const char *strvalue) {
	if (strcmpi(name, "AssignLog") == 0) {
		vChatViewer::assignLog(strvalue);
		if (textwnd) textwnd->assignTextLog(vChatViewer::getChatLog());
	}

	return CHATWND_PARENT::setXmlParam(name, strvalue);
}



int ChatWnd::onInit() {
	GuiObject *guiObj = getGuiObject();

	if (guiObj) {
		if (getDocked()) {
			AddCommand(" Connect ", 20);
			AddCommand(" Disconnect ", 21);
			AddCommand(" Detach ", 2, 1);
			guiObj->guiobject_addAppCmds(this);
		} else {
			GuiObject *guiTxt = guiObj->guiobject_findObject("wasabi.statusbar.text");
			if (guiTxt) guiTxt->guiobject_setXmlParam("ticker", "true");
	//		AddCommand(" Topic ", 3, 1);
			AddCommand(" Attach ", 1, 1);
			guiObj->guiobject_addAppCmds(this);
		}
	}

	onTopicChange(getTopic(), "");
		
	CHATWND_PARENT::setName(vChatViewer::getNameSafe("Chat Window"));

	
	setDividerType(DIVIDER_VERTICAL);

	textwnd = new TextWnd();
	textwnd->setNotifyId(2);
	textwnd->setNotifyWindow(this);
	textwnd->assignTextLog(getChatLog());


	if (log) if (log->getLogType() == 1) {
		treewnd = new TreeWnd();
		refreshUserList();
		setResizeable(TRUE);
		setDividerPos(SDP_FROMRIGHT, 100);
	} else {
		setDividerPos(SDP_FROMRIGHT, 0);
		setResizeable(FALSE);
	}

	setChildren(textwnd, treewnd);

	int retval = CHATWND_PARENT::onInit();
	return retval;
}


void ChatWnd::onExitChannel() {
	if (undocked) {
		undocked = FALSE;
		api->skinwnd_destroy(this, NULL);
	}
}


void ChatWnd::onSetDocked(BOOL dock) {
	if ( (dock == TRUE) & (undocked == TRUE) ) {
		api->skinwnd_destroy(this, NULL);
	}
}


void ChatWnd::onAssignLog(vChatLog *newLog) {
	if (textwnd) textwnd->assignTextLog(newLog);

	if (undocked) return;
	if (getChatLog() == NULL) return;
	if (!isInited()) return;

	CHATWND_PARENT::setName(vChatViewer::getNameSafe("Chat Window"));
//	chatfrm->setName(CHATWND_PARENT::getName());

	if (getLogType() == 1) {
		if (!treewnd) {
			treewnd = new TreeWnd();
			if (isInited()) treewnd->init(this);
			setChildren(textwnd, treewnd);
			setResizeable(TRUE);
		}
		setDividerPos(SDP_FROMRIGHT, 100);
		refreshUserList();
	}
	else {
		setChildren(textwnd);
		delete treewnd;
		treewnd = NULL;
		setResizeable(FALSE);
		setDividerPosNoCfg(SDP_FROMRIGHT, 0);
	}
	if (textwnd) textwnd->invalidate();
	
	if (isInited()) onTopicChange(getTopic(), "");
}


void ChatWnd::onNickChange(const char *newNick, const char *oldNick) {
	onUserPart(oldNick);
	onNewUser(newNick, getUserStatus(oldNick));
}


void ChatWnd::onUserStatusChange(const char *name, BYTE newStatus, BYTE oldStatus) {
	onUserPart(name);
	onNewUser(name, newStatus);
}


void ChatWnd::onNewUser(const char *name, BYTE status) {
	if (treewnd == NULL) return;
	if (status) {
		char str[256];
		char userStatus = 0;

		if (status & 0x02) userStatus = '.';
		else if (status & 0x08) userStatus = '@';
		else if (status & 0x01) userStatus = '%';
		else if (status & 0x04) userStatus = '+';

		sprintf(str, "%c%s", userStatus, name);
		TreeItem *itm = new TreeItem(str);
		treewnd->addTreeItem(itm, NULL);
	}
	else {
		TreeItem *itm = new TreeItem(name);
		treewnd->addTreeItem(itm, NULL);
	}
}


void ChatWnd::onUserPart(const char *name) {
	if (treewnd == NULL) return;
	int c = treewnd->getNumRootItems();

	for (int i=0; i<c; i++)
	{
		TreeItem *itm = treewnd->enumRootItem(i);
		const char *nick = itm->getLabel();
		if (*nick == '.') nick++;
		else if (*nick == '@') nick++;
		else if (*nick == '+') nick++;
		else if (*nick == '%') nick++;
		if (strcmpi(nick, name) == 0) {
			treewnd->removeTreeItem(itm);
			break;
		}
	}
}


void ChatWnd::refreshUserList() {
	if (chanLog == NULL) return;
	if (treewnd == NULL) return;

	treewnd->deleteAllItems();
	LPvChannelUser user = chanLog->getFirstUser();
	while (user) {
		onNewUser(user->nickname, user->userMode);
		user = chanLog->getNextUser(user);
	}
}


void ChatWnd::onTopicChange(const char *newTopic, const char *oldTopic) {
	if (newTopic == NULL) newTopic = "";
	GuiObject *guiObj = getGuiObject();
	if (guiObj) guiObj->guiobject_setStatusText(newTopic, 0);
}


void ChatWnd::appcmds_onCommand(int id, const RECT *buttonRect, int which_button) {
	switch (id)	{
	case 1: setDocked(TRUE); break;
	case 2:	setDocked(FALSE);	break;
	case 20: if (irc) irc->connectToServer(); break;
	case 21: if (irc) irc->closeSocket(); break;
	}
}
