#include "precomp.h"
#include "../../bfc/std.h"

#include "IRCClient.H"
#include "IRCWnd.H"
#include "ChatWnd.H"
#include "WndManager.H"
#include "ScriptObject.H"
#include "ChannelLog.H"
#include "PrivMsgLog.H"
#include "DccChatLog.H"
#include "../../bfc/msgbox.h"
#include "../../bfc/attribs/attrhandler.h"

#include "../../common/script/c_script/c_container.h"
#include "../../common/script/c_script/c_guiobject.h"
#include "../../common/script/c_script/c_layout.h"

#include "../../../vBase/Base.h"


extern _string *nickname;
extern _string *ircserver;
extern _int    *ircport;
extern _bool   *rawirc;
extern _bool   *ircpings;
extern IrcConnect *irc;


IrcConnect::IrcConnect() {
	ZeroMemory(nick, sizeof(nick));
//	ZeroMemory(server, sizeof(server));

	irc = this;
//	channels = new vChannelList();
	
	status = new vChatLog();
	status->setName("Status");
	status->setServer(this);
	status->activate();
//	log->setTopic("Status Window");
}


IrcConnect::~IrcConnect() {
	status = NULL;
	closeSocket();
//	delete channels;
//	channels = NULL;
	irc = NULL;
}


void IrcConnect::connectToServer(const char *server, int port) {
	if (getSocket()) return;
	
	if (server == NULL)
		server = ircserver->getValue();
	else
		ircserver->setValue(server);

	if (port == 0)
		port = ircport->getValueAsInt();
	else
		ircport->setValueAsInt(port);

	char str[256];
	sprintf(str, "Connecting to Server: %s:%d", server, port);
	statusMessage(str);

	irc->setNick(nickname->getValue());
	irc->connectSocket(server, port);
}


void IrcConnect::setNick(const char *n) {
	srand(GetTickCount());
	lstrcpy(nick, n);
	if (lstrcmpi(nick, "Guest")==0) sprintf(nick, "Guest_%04X", Rand(GetTickCount()));
}

const char *IrcConnect::getServer() {
	return ircserver->getValue();
}


int IrcConnect::getServerPort() {
	return ircport->getValueAsInt();
}


void IrcConnect::activateStatus() {
	if (status) status->activate();
}


void IrcConnect::onConnected() {
	char str[256];
	sprintf(str, "NICK %s", nick);
	Send(str);
	sprintf(str, "USER %s 0 * :WasabiIRC", nick);
	Send(str);
	Send("IRCX");

//	Send("JOIN #wasabi");

	vConnect::onConnected();
}


void IrcConnect::onDisconnected() {
	Send("QUIT :Connection reset by ph34r");
	statusMessage("Disconnected from server");
//	if (status) status->activate();

	vChannelLog *log = channels.getFirst();
	while (log) {
		vChannelLog *nextLog = channels.getNext(log);
		log->part();
		log = nextLog;
	}

	vConnect::onDisconnected();
}


vChatLog *IrcConnect::getActiveLog() {
	vChatLog *log = logs.getFirst();
	while (log) {
		if (log->getActivated()) return log;
		log = logs.getNext(log);
	}
	return NULL;
}


void IrcConnect::floatWindow(vChatLog *log) {
	if (log == NULL) return;
	log->setDocked(FALSE);

	//if (getLogType == 1)
	{
		ChatWnd::setNextLog(log);
		RootWnd *wnd = api->skinwnd_createByGroupId("irc.detached.content.group");
		if (wnd) {
			C_Container(C_Layout(C_GuiObject(wnd->getGuiObject()->guiobject_getScriptObject()).getParentLayout()).getContainer()).setName(log->getNameSafe("Chat Window"));
/*			RECT r;
			wnd->getWindowRect(&r);
			wnd->resize(r.left, r.top, -1, -1, 0);
			wnd->resize(r.left, r.top, r.right-r.left, r.bottom-r.top, 1);*/
		}
	}

	if (log->getActivated()) if (status) status->activate();
}


void IrcConnect::unfloatWindow(vChatLog *log) {
	if (log == NULL) return;
	log->setDocked(TRUE);
}


void IrcConnect::registerLog(vChatLog *log) {
	if (log == NULL) return;
	if (log->getLogType() == 1)	channels.addToEnd(static_cast<vChannelLog*>(log));
	logs.addToEnd(log);
}

void IrcConnect::deregisterLog(vChatLog *log) {
	if (log == NULL) return;
	if (log->getLogType() == 1)	channels.delItem(static_cast<vChannelLog*>(log));
	logs.delItem(log);
}


void IrcConnect::deregisterChannel(vChannelLog *log) {
	if (log == NULL) return;
	channels.delItem(log);
	logs.delItem(log);
}


void IrcConnect::deregisterDeleteLogs() {
	channels.deregDel();
	logs.deregDel();
}


void IrcConnect::deleteLog(vChatLog *log) {
	if (log == NULL) return;
	if (logs.itemExists(log)) delete log;
	
	else {
		if (channels.itemExists(static_cast<vChannelLog*>(log))) delete log;
	}
}


void IrcConnect::registerLogViewer(vChatListViewer *viewer) {
	if (viewer == NULL) return;
	if (viewer->getLogType() == 1)	channels.registerChannelViewer(static_cast<vChannelListViewer*>(viewer));
	logs.registerChatViewer(viewer);
}

void IrcConnect::deregisterLogViewer(vChatListViewer *viewer) {
	if (viewer == NULL) return;
	if (viewer->getLogType() == 1)	channels.deregisterChannelViewer(static_cast<vChannelListViewer*>(viewer));
	logs.deregisterChatViewer(viewer);
}



void IrcConnect::statusMessage(const char *message) {
	if (status) status->addIrcText(message);
}


//[18:04:48] :Darkain!~pkfk@131.191.45.XXX PRIVMSG wIRC-Darkain :.DCC CHAT chat 2210344340 2998.
void IrcConnect::onDcc(char *message, char *sender) {
	char type[256] = "";
	char param[256] = "";
	UINT address = 0;
	int port = 0;

	sscanf(message, "%s %s %u %d", type, param, &address, &port);

	if (strcmpi(type, "CHAT") == 0) {
		char str[300];
		sprintf(str, "Do you wish to accept DCC CHAT request from: %s", sender);
//		int ret = api->messageBox(str, "Wasabi-IRC  ---  DCC Chat Request", MSGBOX_YES | MSGBOX_NO, NULL, NULL);

//		if (ret == MSGBOX_YES)
		{
			vDccChat *dcc = new vDccChat();
			dcc->setName(sender);
			dcc->setServer(this);

			char addrStr[16];
			sprintf(addrStr, "%d.%d.%d.%d", HIBYTE(HIWORD(address)),
				LOBYTE(HIWORD(address)), HIBYTE(LOWORD(address)), LOBYTE(LOWORD(address)));
			dcc->connectSocket(addrStr, port);

			sprintf(str, "DCC CHAT request recieved from %s", sender);
			dcc->addIrcText(str);
		}
	}
}


void IrcConnect::process(char *txt) {
	if (txt == NULL) return;
	if (*txt == NULL) return;

	if (!!rawirc->getValueAsInt()) statusMessage(txt);

	//old parser
	char cmd[256];
	char cmd2[256];
	char cmd3[256];
	char cmd4[512];
	ZeroMemory(cmd, sizeof(cmd));
	ZeroMemory(cmd2, sizeof(cmd2));
	ZeroMemory(cmd3, sizeof(cmd3));
	ZeroMemory(cmd4, sizeof(cmd4));

	sscanf(txt, "%s %s %s", cmd, cmd2, cmd3);
	int len = lstrlen(cmd) + lstrlen(cmd2) + lstrlen(cmd3) + 3;

	if (strcmpi(cmd, "ERROR") == 0) {
		closeSocket();
		return;
	}
/*
	int cmdID = 0;
	if (sscanf(cmd2, "%d", &cmdID) == 0) cmdID = 0;
*/
/*
	while ( (*message != ':') & (*message != NULL) ) message++;
	if (*message != NULL) message++;
*/


	//new parser
	vList<char*> wordlist(true);	//used to handle the main text parsing
	vList<char*> wordlist2(true);	//used to handle any extra temp strings needed

	char *message = txt;
	if (*message == ':') message++;
	char *msgBegin = message;

	char *beforemessage = NULL;
	char *channelname = NULL;
	char *numberMessage = NULL;
	int returnMessage = -1;
	
	char *userstring = NULL;
	char *username = NULL;
	char *usernick = NULL;
	char *userhost = NULL;
	BOOL usertext = FALSE;


	while (*message) {
		if (*message == ':') {
			message++;
			beforemessage = wordlist.getLast();
			break;
		}

		if (*message == '!') {
			if (userstring == NULL) usertext = TRUE;
		}

		if (*message == ' ') {
			int len = message - msgBegin;
			char *text = (char*)malloc(len+1);
			memcpy(text, msgBegin, len);
			text[len] = NULL;
			wordlist.addToEnd(text);

			if (*text == '#') {
				if (channelname == NULL) channelname = text;
			}
			else if (usertext) {
				userstring = text;
				usertext = FALSE;
			}
			else if ( (*text >= '0') & (*text <= '9') ) {
				if (numberMessage == NULL) {
					numberMessage = text;
					if (sscanf(numberMessage, "%d", &returnMessage) == 0) {
						returnMessage = -1;
					}
				}
			}

			msgBegin = message+1;
		}
		message++;
	}

	if (userstring) {
		char *tmpend = userstring;
		char *tmpbgn = tmpend;

		while (*tmpend) {
			if (*tmpend == '!') {
				int len = tmpend-tmpbgn;
				usernick = (char*)malloc(len+1);
				memcpy(usernick, tmpbgn, len);
				usernick[len] = NULL;
				wordlist2.addToEnd(usernick);
				tmpbgn = tmpend+1;
			}
			else if (*tmpend == '@') {
				int len = tmpend-tmpbgn;
				username = (char*)malloc(len+1);
				memcpy(username, tmpbgn, len);
				username[len] = NULL;
				wordlist2.addToEnd(username);
				userhost = tmpend+1;
				break;
			}
			tmpend++;
		}
	}


	switch (returnMessage)
	{
	case 422:	//no MOTD
	case 376:	//end MOTD
	case 372: //begin MOTD
	case 375:	//MOTD
		{
			sprintf(cmd4, "MOTD: %s", message);
			statusMessage(cmd4);
		}
		break;

	case 332: //channel topic set - upon joining
		{
			vChannelLog *log = channels.getLog(channelname);
			if (log) {
				char str[300];
				sprintf(str, "Topic: %s", message);
				log->addIrcText(str);
				log->setTopic(message);
			}
		}
		break;

	case 353:	//user list within a channel - upon joining
		{
			vChannelLog *log = channels.getLog(channelname);
			if (log) {
				if (log->isJoined()) {
					if (*message == NULL) return;
					char str[300];
					sprintf(str, "Users in %s: %s", log->getNameSafe(""), message);
					statusMessage(str);
					return;
				}

				char *p=txt+1;
				while ( (*p != 0) & (*p != ':') ) p++;
				if (*p == NULL) return;
				p++;

				while (*p != NULL) {
					char status = 0;
					if (*p == '.') { p++; status = 0x02; }
					else if (*p == '@') { p++; status = 0x08; }
					else if (*p == '+') { p++; status = 0x04; }
					else if (*p == '%') { p++; status = 0x01; }
					sscanf(p, "%s", cmd);
					log->addUser(cmd, status);
					p += (lstrlen(cmd));
					if (*p == ' ') p++;
				}
			}
			else {
				char *channel = txt + 1;
				while ( (*channel != '#') & (*channel != NULL) ) channel++;
				if (*channel == NULL) return;
				sscanf(channel, "%s %s", cmd, cmd2);

				char str[256];
				sprintf(str, "NAMES: %s: %s", cmd, message);
				statusMessage(str);
			}
		}
		break;

	case 366: // end of /NAMES
		{
//			sscanf(txt, "%s %s %s %s", cmd, cmd2, cmd3, cmd4);
			vChannelLog *log = channels.getLog(channelname);
			if (log) {
				if (!log->isJoined()) {
					log->setJoined(TRUE);
					LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
					ZeroMemory(wnd, sizeof(vIrcWindow));
					wnd->size = sizeof(vIrcWindow);
					wnd->msg = 3;
					wnd->log = log;
					sendWndMsg(wnd);
				}
			}

			else {
				if (*message == NULL) return;
				char str[300];
				sprintf(str, "NAMES: %s", message);
				statusMessage(str);
			}
		}
		break;

	case 391:
		{
			char str[256];
			sprintf(str, "The local time on %s is: %s", beforemessage, message);
			statusMessage(str);
		}
		break;

	case 404:
		{
			char str[256];
			sprintf(str, "Error: %s - %s", channelname, message);
			statusMessage(str);

			vChatLog *log = channels.getLog(channelname);
			if (log == NULL) return;
			sprintf(str, "Error: %s", message);
			log->addIrcText(str);
		}
		break;

	case 305:
	case 306:
		{
			if (*message == NULL) return;
			char str[256];
			sprintf(str, "AWAY: %s", message);
			statusMessage(str);
/*			vChatLog *log = channels.getFirst();
			log->addIrcText(str);*/
		}
		break;


	case 401:
	case 402:
	case 403:
	case 405:
	case 406:
	case 407:
	case 408:
	case 413:
	case 414:
	case 415:
	case 421:
	case 423:
	case 432:
	case 433:
	case 436:
	case 437:
	case 442:
	case 444:
	case 461:
	case 467:
	case 471:
	case 472:
	case 473:
	case 474:
	case 475:
	case 476:
	case 477:
	case 482:
	case 484:
	case 485:
		{
//			sscanf(txt, "%s %s %s %s", cmd, cmd2, cmd3, cmd4);
//			if (*message == NULL) return;
			char str[256];
			sprintf(str, "Error: %s - %s", beforemessage, message);
			statusMessage(str);
		}
		break;

	case 409:
	case 411:
	case 412:
	case 424:
	case 431:
	case 445:
	case 446:
	case 451:
	case 462:
	case 463:
	case 464:
	case 465:
	case 481:
	case 483:
	case 491:
	case 501:
	case 502:
		{
//			if (*message == NULL) return;
			char str[256];
			sprintf(str, "Error: %s", message);
			statusMessage(str);
		}
		break;


	case 800:	//IRCX mode... not all servers support this
		statusMessage("IRCX Mode is supported by this server, and is now enabled on client side");
		break;


	case -1:	//everything that isnt a numbered message
		if (lstrcmpi(cmd2, "PRIVMSG") == 0)
		{
			if (strcmpi(cmd3, getNick()) == 0) {
				if (*message == NULL) return;
				if (*message == 0x01) {
					*message++;
					char *msgBegin = message;
					while ( (*msgBegin != ' ') & (*msgBegin != NULL) & (*msgBegin != 0x01)  ) msgBegin++;
					if (*msgBegin == NULL) return;
					*msgBegin = NULL;
					msgBegin++;

					char *msgEnd = msgBegin;
					while ( (*msgEnd != 0x01) & (*msgEnd != NULL) ) msgEnd++;
					if (*msgEnd != NULL) *msgEnd = NULL;

					char *sender = cmd;
					if (*sender == ':') sender++;
					char *senderEnd = sender;
					while ( (*senderEnd != '!') & (*senderEnd != NULL) ) senderEnd++;
					if (*senderEnd == NULL) return;
					*senderEnd = NULL;

					if (strcmpi(message, "PING") == 0) {
						char str[256];
						sprintf(str, "NOTICE %s :%cPING %s%c", sender, 0x01, msgBegin, 0x01);
						irc->Send(str);
					}
					else if (strcmpi(message, "VERSION") == 0) {
						char str[256];
						sprintf(str, "NOTICE %s :%cVERSION %s - %s (Wasabi build: %d)%c", sender, 0x01, "Wasabi-IRC",
							api->main_getVersionString(), api->main_getBuildNumber(), 0x01);
						irc->Send(str);
					}
					else if (strcmpi(message, "TIME") == 0) {
						char str[256];
						sprintf(str, "NOTICE %s :%cTIME %s%c", sender, 0x01, "The time is now", 0x01);
						irc->Send(str);
					}

					else if (strcmpi(message, "DCC") == 0) {
//						while ( (*message != ' ') & (*message != NULL) ) message++;
//						if (*message == NULL) return;
//						message++;
						onDcc(msgBegin, sender);
					}
				}
				
				else {
					char *sender = txt;
					if (*sender == ':') sender++;
					char *senderEnd = sender;
					while ( (*senderEnd != '!') & (*senderEnd != NULL) ) senderEnd++;
					if (*senderEnd == NULL) return;
					*senderEnd = NULL;

					vChatLog *log = logs.getLog(sender);
					if (log == NULL) {
						log = new vPrivMsgLog();
						log->setName(sender);
						log->setServer(this);
//						log->activate();
					}
					char str[256];
					sprintf(str, "<%s> %s", sender, message);
					log->addIrcText(str);
				}
				return;
			}

			vChatLog *log = channels.getLog(cmd3);
			if (log)
			{
				int strlen = lstrlen(cmd);
				for(int i=0; i<strlen; i++)
					if (cmd[i] == '!') cmd[i] = 0;

				sscanf(txt+len+1, "%s", cmd4);
				if (cmd4[0] == 0x01)
				{
					if (lstrcmpi(cmd4+1, "ACTION") == 0)
					{
						strlen = lstrlen(txt);
						for (i=0; i<strlen; i++)
							if (txt[i] == 0x01) txt[i] = 0x00;

						sprintf(cmd4, "* %s%s", cmd+1, txt+len+8);
						log->addIrcText(cmd4);
					}
				}
				else
				{
					sprintf(cmd4, "<%s> %s", cmd+1, txt+len+1);
					log->addIrcText(cmd4);
				}
			}
		}

		
		else if (lstrcmpi(cmd2, "TOPIC") == 0) {
			vChannelLog *log = channels.getLog(channelname);
			if (log == NULL) return;
			log->setTopic(message);

			char str[300];
			sprintf(str, "*** %s set topic to: %s", usernick, message);
			log->addIrcText(str);
		}

		else if (lstrcmpi(cmd2, "NOTICE") == 0)
		{
			if (strcmpi(cmd3, getNick()) == 0) {
				if (*message == NULL) return;
				if (*message == 0x01) {
					*message++;
					char *msgBegin = message;
					while ( (*msgBegin != ' ') & (*msgBegin != NULL) & (*msgBegin != 0x01)  ) msgBegin++;
					if (*msgBegin == NULL) return;
					*msgBegin = NULL;
					msgBegin++;

					char *msgEnd = msgBegin;
					while ( (*msgEnd != 0x01) & (*msgEnd != NULL) ) msgEnd++;
					if (*msgEnd != NULL) *msgEnd = NULL;

					char *sender = cmd;
					if (*sender == ':') sender++;
					char *senderEnd = sender;
					while ( (*senderEnd != '!') & (*senderEnd != NULL) ) senderEnd++;
					if (*senderEnd == NULL) return;
					*senderEnd = NULL;

					char str[256];
					sprintf(str, "CTCP %s Reply: %s - %s", message, sender, msgBegin);
					statusMessage(str);
				}
				return;
			}

			const char *p = cmd3;
			if (*p == ':') p++;
			vChatLog *log = channels.getLog(p);
			if (log)
			{
				char *nick = txt;
				if (*nick == ':') nick++;
				char *nickEnd = nick;
				while ( (*nickEnd != '!') & (*nickEnd != NULL) ) nickEnd++;
				if (*nickEnd == NULL) return;
				*nickEnd = NULL;

				if (*message == NULL) return;
				char str[300];
				sprintf(str, "-%s- %s", nick, message);
				log->addIrcText(str);
			}
		}

		else if (lstrcmpi(cmd2, "JOIN") == 0) {
			if (lstrcmpi(usernick, nick) == 0) {
				LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
				ZeroMemory(wnd, sizeof(vIrcWindow));
				wnd->size = sizeof(vIrcWindow);
				wnd->msg = 1;
				wnd->log = new vChannelLog();
				if (channelname) wnd->log->setName(channelname);
				else wnd->log->setName(message);
				wnd->log->setServer(this);

				char str[256];
				sprintf(str, "Joined Chat: %s", cmd3+1);
				wnd->log->addIrcText(str);

				sendWndMsg(wnd);
			}
			
			else {
				vChannelLog *log = channels.getLog(channelname);
				if (log) {
					log->addUser(cmd+1);
					sprintf(cmd4, "*** %s has joined channel: %s", usernick, log->getName());
					log->addIrcText(cmd4);
				}
			}
		}

		else if (lstrcmpi(cmd2, "PART") == 0) {
			vChannelLog *log = channels.getLog(channelname);
			if (log == NULL) return;
			
			if (lstrcmpi(usernick, nick) == 0) {
				LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
				ZeroMemory(wnd, sizeof(vIrcWindow));
				wnd->size = sizeof(vIrcWindow);
				wnd->msg = 2;
				wnd->log = log;
				sendWndMsg(wnd);

				char str[256];
				sprintf(str, "You have parted from: %s", channelname);
				statusMessage(str);
			}
			else {
				log->delUser(cmd+1);
				sprintf(cmd4, "*** %s PART Channel %s: %s", usernick, channelname, message);
				log->addIrcText(cmd4);
			}
		}

		else if (lstrcmpi(cmd2, "QUIT") == 0) {
			vChannelLog *log = channels.getFirst();
			while (log) {
				if (log->hasUser(cmd+1)) {
					log->delUser(cmd+1);
					if (*message) sprintf(cmd4, "*** %s QUIT (%s)", usernick, message);
					else sprintf(cmd4, "*** %s QUIT", usernick);
					log->addIrcText(cmd4);
				}
				log = channels.getNext(log);
			}
		}

		else if (lstrcmpi(cmd2, "KICK") == 0) {
			vChannelLog *log = channels.getLog(channelname);
			if (log == NULL) return;

			if (lstrcmpi(usernick, nick) != 0) {
				LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
				ZeroMemory(wnd, sizeof(vIrcWindow));
				wnd->size = sizeof(vIrcWindow);
				wnd->msg = 2;
				wnd->log = log;
				sendWndMsg(wnd);

				char str[256];
				sprintf(str, "You have been kicked from %s: %s", channelname, message);
				statusMessage(str);
			}
			else {
				log->delUser(cmd4);
				char str[256];
				sprintf(str, "*** %s has been kicked from channel %s: %s", usernick, channelname, message);
				log->addIrcText(str);
			}
		}

		else if (lstrcmpi(cmd2, "NICK") == 0) {
			char str[256];

			if (lstrcmpi(usernick, nick) == 0) {
				nickname->setValue(message);
				lstrcpy(nick, message);

				sprintf(str, "You are now knows as: %s", message);
				statusMessage(str);
			}
			
			vChannelLog *log = channels.getFirst();
			while (log) {
				if (log->hasUser(usernick)) {
					log->onNickChange(message, usernick);
					sprintf(str, "*** %s is now known as %s", usernick, message);
					log->addIrcText(str);
				}
				log = channels.getNext(log);
			}
		}
		
		else if (lstrcmpi(cmd2, "MODE") == 0) {
			message = (char*)txt+1;
			while ( (*message != '#') & (*message != NULL) ) message++;
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message == NULL) return;
			*message++;
			
			vChannelLog *log = channels.getLog(cmd3);
			if (log == NULL) return;

			{
				char str[256];
				sprintf(str, "%s set MODE: %s", usernick, message);
				log->addIrcText(str);
			}

			vList<LPvChannelUser> list;
			BOOL plus = FALSE;
			while ( (*message != ' ') & (*message != NULL) ) {
				switch (*message) {
				case '+': plus = TRUE; break;
				case '-': plus = FALSE; break;
				
				case 'a':
				case 'o': {
						LPvChannelUser user = (LPvChannelUser) malloc(sizeof(vChannelUser));
						if (plus) user->userMode = 0x88;
						else user->userMode = 0x08;
						user->nickname = NULL;
						list.addToEnd(user);
					} break;
				
				case 'v': {
						LPvChannelUser user = (LPvChannelUser) malloc(sizeof(vChannelUser));
						if (plus) user->userMode = 0x84;
						else user->userMode = 0x04;
						user->nickname = NULL;
						list.addToEnd(user);
					} break;
				
				case 'q': {
						LPvChannelUser user = (LPvChannelUser) malloc(sizeof(vChannelUser));
						if (plus) user->userMode = 0x82;
						else user->userMode = 0x02;
						user->nickname = NULL;
						list.addToEnd(user);
					} break;
				
				case 'h': {
						LPvChannelUser user = (LPvChannelUser) malloc(sizeof(vChannelUser));
						if (plus) user->userMode = 0x81;
						else user->userMode = 0x01;
						user->nickname = NULL;
						list.addToEnd(user);
					} break;
				}
				message++;
			}
			if (*message == NULL) return;
			*message++;

			LPvChannelUser user = list.getFirst();
			if (user) user->nickname = message;
			while ( (*message != NULL) ) {
				if (*(message) == ' ') {
					user = list.getNext(user);
					if (user) user->nickname = message+1;
					*message = NULL;
				}
				message++;
			}

			user = list.getFirst();
			while (user) {
				log->onUserStatusChange(user->nickname, user->userMode);
				free(user);
				user = list.getNext(user);
			}
		}

		else if (lstrcmpi(cmd, "PING") == 0) {
			if (*message == NULL) {
				irc->Send("PONG");
				return;
			}
			char str[300];
			sprintf(str, "PONG %s", message);
			irc->Send(str);

			if (ircpings->getValueAsInt()) {
				if (*message == ':') message++;
				sprintf(str, "PING? PONG! - %s", message);
				statusMessage(str);
			}
		}

		else {
			if (!!!rawirc->getValueAsInt()) statusMessage(txt);
			vConnect::process(txt);
		}
		break;

	default: {
			if (!!!rawirc->getValueAsInt()) {
				char *str = txt+len;
				if (str[0] == ':') str++;
				sprintf(cmd4, "Server: %s", str);
				statusMessage(cmd4);
			}
			vConnect::process(txt);
		}
	}
}



LRESULT CALLBACK IrcConnect::WndPrc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_USER+44:
	case WM_USER+45:
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			onRead();
			break;
		}
		break;
	}

	return FALSE;
}


//---------------------------------------------------------------------------------------

/*
vChannelList::vChannelList()
{
}


vChannelList::~vChannelList()
{
	deregDel();
}
*/

void vChannelList::deregDel() {
	vChannelLog *log = getFirst();
	while (log) {
		log->setServer(NULL);
		delete log;
		delItem(log);
		log = getFirst();
	}
}


vChannelLog *vChannelList::getLog(const char *name) {
	if (name == NULL) return NULL;
	if (*name == ' ') name++;
	if (strlen(name) < 1) return NULL;

	vChannelLog *log = getFirst();
	while (log) {
		if (lstrcmpi(log->getName(), name) == 0) return log;
		log = getNext(log);
	}

	return NULL;
}


void vChannelList::onAddItem(vChannelLog *data) {
	vChannelListViewer *v = channelViewers.getFirst();
	while (v) {
		v->onAddChannel(data);
		v = channelViewers.getNext(v);
	}
}


void vChannelList::onDelete(vChannelLog *data) {
	vChannelListViewer *v = channelViewers.getFirst();
	while (v) {
		v->onDeleteChannel(data);
		v = channelViewers.getNext(v);
	}
}


//------------------------------------------------------------------------------------


void vChatList::onAddItem(vChatLog *data) {
	vChatListViewer *v = chatViewers.getFirst();
	while (v) {
		v->onAddChat(data);
		v = chatViewers.getNext(v);
	}
}


void vChatList::onDelete(vChatLog *data) {
	vChatListViewer *v = chatViewers.getFirst();
	while (v) {
		v->onDeleteChat(data);
		v = chatViewers.getNext(v);
	}
}


void vChatList::deregDel() {
	vChatLog *log = getFirst();
	while (log) {
		log->setServer(NULL);
		delete log;
		delItem(log);
		log = getFirst();
	}
}


vChatLog *vChatList::getLog(const char *name) {
	if (name == NULL) return NULL;
	if (*name == ' ') name++;
	if (strlen(name) < 1) return NULL;

	vChatLog *log = getFirst();
	while (log) {
		if (lstrcmpi(log->getName(), name) == 0) return log;
		log = getNext(log);
	}

	return NULL;
}
