#include "precomp.h"
#include "../../bfc/std.h"
#include "../../bfc/attribs/attrhandler.h"
#include "../../common/popup.h"
#include <time.h>
#include <shellapi.h>
#include "IrcClient.H"
#include "ChatLog.H"
#include "ChatLogViewer.H"
#include "ChannelLog.H"
#include "DccChatLog.H"

#include "../../../vBase/Base.h"


//extern vChannelList *channels;
extern _string *ircserver;
extern _int    *ircport;
extern IrcConnect *irc;


void vChatLog::part() {}


vChatLog::vChatLog() {
	deleting = false;
	registered = false;
	stamps = TRUE;
	formatting = TRUE;
	flashing = TRUE;
	irc = NULL;
	docked = TRUE;
	active = FALSE;
//	addData(0, CRLF);
}


vChatLog::~vChatLog() {
	if (deleting) return;
	deleting = true;

	if (irc) { irc->deregisterLog(this); }

	{
		char str[256];
		sprintf(str, "%s_Formatting", getNameSafe("Default"));
		api->setIntPrivate(str, formatting);

		sprintf(str, "%s_Stamps", getNameSafe("Default"));
		api->setIntPrivate(str, stamps);

		sprintf(str, "%s_Flashing", getNameSafe("Default"));
		api->setIntPrivate(str, flashing);
	}


	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->assignLogNoDeregister();
		v->onLogDelete();
		viewers.delItem(v);
		v = viewers.getFirst();
	}
}


void vChatLog::onSetName() {
	char str[256];
	sprintf(str, "%s_Formatting", getNameSafe("Default"));
	setFormatting(api->getIntPrivate(str, TRUE));

	sprintf(str, "%s_Stamps", getNameSafe("Default"));
	setStamps(api->getIntPrivate(str, TRUE));

	sprintf(str, "%s_Flashing", getNameSafe("Default"));
	setFlashing(api->getIntPrivate(str, (getLogType() != -1)));
}


void vChatLog::popMenu() {
	PopupMenu *menu = new PopupMenu();

	menu->addCommand( "Time Stamps", 1, getStamps());
	menu->addCommand( "Text Formatting", 2, getFormatting());
	menu->addCommand( "Flashing", 3, getFlashing());

	PopupMenu *sub1 = new PopupMenu();
	sub1->addCommand("Clear log", 101);
	sub1->addCommand("Save log", 102, 0, TRUE);

	menu->addSubMenu(sub1, "Log");


	if (getLogType() != -1) {
		menu->addSeparator();
		menu->addCommand(" Close ", 20);
	}

	switch (menu->popAtMouse()) {
	case 1: setStamps(!getStamps()); break;
	case 2: setFormatting(!getFormatting()); break;
	case 3: setFlashing(!getFlashing()); break;
	case 101:	clearLog(); break;

	case 20:
		{
			part();
			LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
			ZeroMemory(wnd, sizeof(vIrcWindow));
			wnd->size = sizeof(vIrcWindow);
			wnd->msg = 2;
			wnd->log = this;
			irc->sendWndMsg(wnd);
		} break;
	}

	delete menu;
}


void vChatLog::setStamps(BOOL s) {
	stamps = !!s;

	setShowTimestamp(s);
 
	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onSetStamps(stamps);
		v = viewers.getNext(v);
	}
}


void vChatLog::setFormatting(BOOL f) {
	formatting = !!f;

	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onSetFormatting(formatting);
		v = viewers.getNext(v);
	}
}


void vChatLog::setFlashing(BOOL f) {
	flashing = !!f;

	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onSetFlashing(flashing);
		v = viewers.getNext(v);
	}
}


void vChatLog::setServer(IrcConnect *s) {
	if (irc) irc->deregisterLog(this);
	irc = s;
	if (irc) irc->registerLog(this);
}


void vChatLog::setDocked(BOOL d) {
	if (getLogType() == -1) return;

	d = !!d;
	if (d == docked) return;
	docked = d;

	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onSetDocked(d);
		v = viewers.getNext(v);
	}

	if (irc) {
		if (docked) irc->unfloatWindow(this);
		else irc->floatWindow(this);
	}
	
	if (docked) activate();
}


void vChatLog::activate() {
	if (irc == NULL) return;
	if (active) return;
	
	active = TRUE;

	vChatLog *log = irc->getFirstLog();
	while (log) {
		if (log != this) log->deactivate();
		log = irc->getNextLog(log);
	}

	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onActivated();
		v = viewers.getNext(v);
	}
}


void vChatLog::deactivate() {
	if (!active) return;
	active = FALSE;

	vList<vChatViewer*> tmplist;

	vChatViewer *v = viewers.getFirst();
	while (v) {
		if (v->getAutoActive()) tmplist.addToEnd(v);
		v = viewers.getNext(v);
	}

	v = tmplist.getFirst();
	while (v) {
		v->autoActivate();
		v = tmplist.getNext(v);
	}

	v = viewers.getFirst();
	while (v) {
		vChatViewer *next = viewers.getNext(v);
		v->onDeactivated();
		v = next;
	}
}

/*
void vChatLog::addText(const char *text)
{
	if (text==NULL) return;
	int len = lstrlen(text);
	if (len<1) return;
	
	LPvTextLine a = new vTextLine;
	a->textLen = len;
	a->text = (char*)malloc(len+1);
	lstrcpy(a->text, text);

	time_t ltime;
	struct tm *today;
	time( &ltime );
	today = localtime( &ltime );

	a->hour = today->tm_hour;
	a->min = today->tm_min;
	a->sec = today->tm_sec;

	addToEnd(a);
	appendLog(a);

	vChatViewer *v = viewers.getFirst();
	while (v)
	{
		v->onNewMessage(a->text);
		v = viewers.getNext(v);
	}
}
*/

void vChatLog::appendLog(LPvTextLine text) {
	char path[256];
	sprintf(path, "%sLogs\\%s.log", getPath(), getNameSafe("Deafult"));
	FILE *file = FOPEN(path, "a+");
	if (file == NULL) {
		char path2[256];
		sprintf(path2, "%sLogs", getPath());
		Std::createDirectory(path2);
		file = FOPEN(path, "a+");
	}
	if (file == NULL) return;
	fprintf(file, "[%02d:%02d:%02d] %s%c%c", text->hour, text->min, text->sec, text->text, 0x0D, 0x0A);
	FCLOSE(file);
}

/*
void vChatLog::onAddItem(LPvTextLine data)
{
	if (getTotalItems() > 100) delFirst();
}


void vChatLog::onDelete(LPvTextLine data)
{
	free(data->text);
	delete data;
}
*/

void vChatLog::clearLog() {
//	delAll();

	vChatViewer *v = viewers.getFirst();
	while (v) {
		v->onLogCleared();
		v = viewers.getNext(v);
	}
}


void vChatLog::say(const char *message) {
	if (irc == NULL) return;
	char str[300];
	sprintf(str, "PRIVMSG %s :%s", getNameSafe(""), message);
	irc->Send(str);
	sprintf(str, "<%s> %s", irc->getNick(), message);
	addIrcText(str);
}


void vChatLog::action(const char *message) {
	if (irc == NULL) return;
	char str[300];
	sprintf(str, "PRIVMSG %s :%cACTION %s%c", getNameSafe(""), 0x01, message, 0x01);
	irc->Send(str);
	sprintf(str, "* %s %s", irc->getNick(), message);
	addIrcText(str);
}


void vChatLog::notice(const char *message) {
	if (irc == NULL) return;
	char str[300];
	sprintf(str, "NOTICE %s :%s", getNameSafe(""), message);
	irc->Send(str);
	sprintf(str, "-%s- %s", irc->getNick(), message);
	addIrcText(str);
}


void vChatLog::ctcp(const char *user, const char *message, const char *type) {
	if (irc == NULL) return;
	
	if (type) {
		char str[300];
		sprintf(str, "PRIVMSG %s :%c%s %s%c", user, 0x01, type, message, 0x01);
		irc->Send(str);
		sprintf(str, "Sent CTCP %s to %s", type, user);
		addIrcText(str);
	}
	
	else {
		char str[300];
		sprintf(str, "PRIVMSG %s :%s", user, message);
		irc->Send(str);
		//todo:  make window stuffz, or just a notification that would make it elsewarez
	}
}


void vChatLog::send(const char *message) {
	if (irc == NULL) return;
	if (message == NULL) return;
	int len = lstrlen(message);
	if (len < 1) return;

	if (message[0] == '/') {
		char cmd[64];
		sscanf(message+1, "%s", cmd);
		
		if ( (lstrcmpi(cmd, "ME") == 0) | (lstrcmpi(cmd, "ACTION") == 0) | (lstrcmpi(cmd, "DESCRIBE") == 0) ) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;

			if (*message == '#') {
				char chanName[64];
				sscanf(message, "%s", chanName);
				while ( (*message != ' ') & (*message != NULL) ) message++;
				if (*message == NULL) return;
				vChannelLog *log = irc->getChannel(chanName);
				if (log) log->action(message);		
			}
			else action(message);
		}

		else if (lstrcmpi(cmd, "AME") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->action(message);
				log = irc->getNextChannel(log);
			}
		}

		else if (lstrcmpi(cmd, "AMSG") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->say(message);
				log = irc->getNextChannel(log);
			}
		}

		else if (lstrcmpi(cmd, "ANOTICE") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->notice(message);
				log = irc->getNextChannel(log);
			}
		}

		else if (lstrcmpi(cmd, "AECHO") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->addIrcText(message);
				log = irc->getNextChannel(log);
			}
		}

		else if (lstrcmpi(cmd, "AECHOS") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->addIrcText(message);
				log = irc->getNextChannel(log);
			}
			irc->statusMessage(message);
		}

		else if (lstrcmpi(cmd, "NOTICE") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;

			if (*message == '#') {
				char chanName[64];
				sscanf(message, "%s", chanName);
				while ( (*message != ' ') & (*message != NULL) ) message++;
				if (*message == NULL) return;
				vChannelLog *log = irc->getChannel(chanName);
				if (log) log->notice(message);		
			}
			else notice(message);
		}

		else if (lstrcmpi(cmd, "DISCONNECT") == 0) {
			if (irc) irc->closeSocket();
		}

		else if (lstrcmpi(cmd, "EXIT") == 0) {
			irc->closeSocket();
			api->main_shutdown();
		}

		else if (lstrcmpi(cmd, "CLEAR") == 0) {
			clearLog();
		}

		else if (lstrcmpi(cmd, "CLEARSTATUS") == 0) {
			vChatLog *log = irc->getStatus();
			if (log) log->clearLog();
		}

		else if (lstrcmpi(cmd, "CLEARALL") == 0) {
			clearLog();
			vChatLog *log = irc->getFirstLog();
			while (log) {
				log->clearLog();
				log = irc->getNextLog(log);
			}
		}

		else if (lstrcmpi(cmd, "PARTALL") == 0) {
			vChannelLog *log = irc->getFirstChannel();
			while (log) {
				log->part();
				log = irc->getNextChannel(log);
			}
		}

		else if (lstrcmpi(cmd, "CTCP") == 0) {
			char *user = (char*)message;
			while ( (*user != ' ') & (*user != NULL) ) user++;
			if (*user == NULL) return;
			*user = NULL;
			user++;

			char *type = user;
			while ( (*type != ' ') & (*type != NULL) ) type++;
			if (*type = NULL) return;
			*type = NULL;
			type++;

			char *msg = type;
			while ( (*msg != ' ') & (*msg != NULL) ) msg++;
			if (*msg != NULL) {	*msg = NULL; *msg++; }
			ctcp(user, msg, type);			
		}

		else if (lstrcmpi(cmd, "RAW") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			irc->Send(message);
		}

		else if (lstrcmpi(cmd, "PART") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) {
				*message++;
				vChannelLog *log = irc->getChannel(message);
				if (log) log->part();
			} else part();
		}

		else if ( (lstrcmpi(cmd, "CONNECT") == 0) | (lstrcmpi(cmd, "SERVER") == 0) ){
			int port = 0;
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) {
				message++;
				char *cPort = (char*)message;
				while ( (*cPort != ':') & (*cPort != NULL) ) cPort++;
				if (*cPort) {
					*cPort = NULL;
					cPort++;
					sscanf(cPort, "%d", &port);
				}
			}
			else {
				message = NULL;
			}
			irc->closeSocket();
			irc->connectToServer(message, port);
		}

		else if (lstrcmpi(cmd, "SAY") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;

			if (*message == '#') {
				char chanName[64];
				sscanf(message, "%s", chanName);
				while ( (*message != ' ') & (*message != NULL) ) message++;
				if (*message == NULL) return;
				vChannelLog *log = irc->getChannel(chanName);
				if (log) log->say(message);		
			}
			else say(message);
		}

		else if (lstrcmpi(cmd, "ECHO") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;

			if (*message == '#') {
				char chanName[64];
				sscanf(message, "%s", chanName);
				while ( (*message != ' ') & (*message != NULL) ) message++;
				if (*message == NULL) return;
				vChannelLog *log = irc->getChannel(chanName);
				if (log) log->addIrcText(message);		
			}
			else addIrcText(message);
		}

		else if (lstrcmpi(cmd, "ECHOS") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			irc->statusMessage(message);
		}

		//*****************************************************************
		//temp hard coded... as you said, gonna be done by script eventualy
		//*****************************************************************
		else if ( (lstrcmpi(cmd, "J") == 0) | (lstrcmpi(cmd, "JOIN") == 0) ) {
			while ( (*message != ' ') & (*message != NULL) & (*message != '#') )message++;
			if (*message) message++; else return;
			if (*message == '#') message++;
			if (!*message) return;

			char *shortcut = (char*) malloc(strlen(message) + 10);
			sprintf(shortcut,"JOIN #%s",message);
			irc->Send(shortcut);
			free(shortcut);
		}
		//*****************************************************************
		//*****************************************************************
		//*****************************************************************

		else if ( (lstrcmpi(cmd, "SW") == 0) | (lstrcmpi(cmd, "SWITCH") == 0) ) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			if (*message == '#') message++;
			if (!*message) return;

			if (lstrcmpi(message, "Status") == 0) {
				vChatLog *log = irc->getStatus();
				if (log) log->activate();
			} else {
				char *shortcut = (char*) malloc(strlen(message) + 10);
				sprintf(shortcut,"#%s",message);
				vChannelLog *log = irc->getChannel(shortcut);
				if (log) log->activate();
				free(shortcut);
			}
		}

		else if ( (lstrcmpi(cmd, "RUN") == 0) | (lstrcmpi(cmd, "SHELL") == 0) ) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
				
			if (*message == '"') {
				char *msgEnd = (char*)++message;
				while ( (*msgEnd != '"') & (*msgEnd != NULL) ) msgEnd++;
				if (*msgEnd != NULL) {
					*msgEnd = NULL;
					msgEnd++;
				}
				ShellExecute(NULL, NULL, message, msgEnd, NULL, SW_SHOW);
			}

			else {
				char *msgEnd = (char*)message;
				while ( (*msgEnd != ' ') & (*msgEnd != NULL) ) msgEnd++;
				if (*msgEnd != NULL) {
					*msgEnd = NULL;
					msgEnd++;
				}
				ShellExecute(NULL, NULL, message, msgEnd, NULL, SW_SHOW);
			}
		}

		else if (lstrcmpi(cmd, "DCC") == 0) {
			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message) message++; else return;
			char *type = (char*)message;

			while ( (*message != ' ') & (*message != NULL) ) message++;
			if (*message == NULL) return;
			char *name = (char*) message;
			*name = NULL;
			*name++;
			message++;

			while ( (*message != ' ') & (*message != NULL) ) message++;
			char *end = (char*) message;
			*end = NULL;

	// /dcc chat {nickname} Opens a dcc window and sends a dcc chat request to nickname. 
			if (strcmpi(type, "CHAT") == 0) {
				vDccChat *dcc = new vDccChat();
				dcc->setName(name);
				dcc->listenSocket(5010);
				dcc->setServer(irc);

				char str[256];
				sprintf(str, "PRIVMSG %s :%cDCC CHAT chat 2210344340 %d%c", name, 0x01, dcc->getPort(), 0x01);
				irc->Send(str);

				sprintf(str, "DCC CHAT request sent to %s", name);
				dcc->addIrcText(str);
			}
		}

		else irc->Send((char*)message+1);
	}
	
	else say(message);
}


void vChatLog::addIrcText(const char *text) {
	time_t thetime;
	time(&thetime);

	addData(0, CRLF);
	addData(thetime, TIMESTAMP);

	char buffer[1024];
	BOOL bold = FALSE;
	BOOL italic = FALSE;
	BOOL underline = FALSE;

	const char *textBegin = text;
	while (1) {
		if (*text == NULL) {
			addText(textBegin, TRUE);
			break;
		}
		else if (*text == 0x02) {
			int len = text-textBegin;
			memcpy(buffer, textBegin, len);
			buffer[len] = NULL;
			addText(buffer, TRUE);
			bold = !bold;
			addData(bold, SETBOLD);
			textBegin = text+1;
		}
		else if (*text == 0x1F) {
			int len = text-textBegin;
			memcpy(buffer, textBegin, len);
			buffer[len] = NULL;
			addText(buffer, TRUE);
			underline = !underline;
			addData(underline, SETUNDERLINE);
			textBegin = text+1;
		}

		else if (*text == 0x03) {
			int len = text-textBegin;
			memcpy(buffer, textBegin, len);
			buffer[len] = NULL;
			addText(buffer, TRUE);

			int c1 = -1;
			int c2 = -1;
			const char *num = text+1;

			if ( (*num >= '0') & (*num <= '9') ) {
				c1 = *num-'0';
				num++;
				if ( (*num >= '0') & (*num <= '9') ) {
					c1 *= 10;
					c1 += *num-'0';
					num++;
				}

				if (*num == ',')
				{
					num++;
					if ( (*num >= '0') & (*num <= '9') ) {
						c2 = *num-'0';
						num++;
						if ( (*num >= '0') & (*num <= '9') ) {
							c2 *= 10;
							c2 += *num-'0';
							num++;
						}
					}
				}
			}

			if (c1 >= 0) {
				c1 = (c1 & 0x8000000F);
				switch (c1) {
				case 0:	addData(RGB(255, 255, 255), SETCOLOR); break;
				case 1:	addData(RGB(0, 0, 0), SETCOLOR); break;
				case 2:	addData(RGB(0, 0, 127), SETCOLOR); break;
				case 3:	addData(RGB(0, 127, 0), SETCOLOR); break;
				case 4:	addData(RGB(255, 0, 0), SETCOLOR); break;
				case 5:	addData(RGB(127, 0, 0), SETCOLOR); break;
				case 6:	addData(RGB(127, 0, 127), SETCOLOR); break;
				case 7:	addData(RGB(255, 127, 0), SETCOLOR); break;
				case 8:	addData(RGB(255, 255, 0), SETCOLOR); break;
				case 9:	addData(RGB(0, 255, 0), SETCOLOR); break;
				case 10:	addData(RGB(0, 127, 127), SETCOLOR); break;
				case 11:	addData(RGB(0, 255, 255), SETCOLOR); break;
				case 12:	addData(RGB(0, 0, 255), SETCOLOR); break;
				case 13:	addData(RGB(255, 0, 255), SETCOLOR); break;
				case 14:	addData(RGB(127, 127, 127), SETCOLOR); break;
				case 15:	addData(RGB(196, 196, 196), SETCOLOR); break;
				default:
					//default colours... needsta be variable (pulled from skin)
					addData(RGB(255, 255, 255), SETCOLOR);
					addData(-1, SETBGCOLOR);
				}
				c2 = (c2 & 0x8000000F);
				if (c2 >= 0)  switch (c2) {
					case 0:	addData(RGB(255, 255, 255), SETBGCOLOR); break;
					case 1:	addData(RGB(0, 0, 0), SETBGCOLOR); break;
					case 2:	addData(RGB(0, 0, 127), SETBGCOLOR); break;
					case 3:	addData(RGB(0, 127, 0), SETBGCOLOR); break;
					case 4:	addData(RGB(255, 0, 0), SETBGCOLOR); break;
					case 5:	addData(RGB(127, 0, 0), SETBGCOLOR); break;
					case 6:	addData(RGB(127, 0, 127), SETBGCOLOR); break;
					case 7:	addData(RGB(255, 127, 0), SETBGCOLOR); break;
					case 8:	addData(RGB(255, 255, 0), SETBGCOLOR); break;
					case 9:	addData(RGB(0, 255, 0), SETBGCOLOR); break;
					case 10:	addData(RGB(0, 127, 127), SETBGCOLOR); break;
					case 11:	addData(RGB(0, 255, 255), SETBGCOLOR); break;
					case 12:	addData(RGB(0, 0, 255), SETBGCOLOR); break;
					case 13:	addData(RGB(255, 0, 255), SETBGCOLOR); break;
					case 14:	addData(RGB(127, 127, 127), SETBGCOLOR); break;
					case 15:	addData(RGB(196, 196, 196), SETBGCOLOR); break;
				}
			} else {
				//default colours... needsta be variable (pulled from skin)
				addData(RGB(255, 255, 255), SETCOLOR);
				addData(-1, SETBGCOLOR);
			}
			text = num;
			textBegin = text;
		}

		text++;
	}
}

