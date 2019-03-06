#include "precomp.h"
#include "../../bfc/std.h"

#include "ChannelLog.H"
#include "ChannelLogViewer.H"
#include "IrcClient.H"


vChannelLog::vChannelLog() {
	topic = NULL;
	joined = FALSE;
	setTopic("");
}


vChannelLog::~vChannelLog() {
	if (irc) irc->deregisterChannel(this);

	vChannelViewer *v = channelViewers.getFirst();
	while (v) {
		v->assignLogNoDeregister(NULL);
		v->onExitChannel();
		channelViewers.delItem(v);
		viewers.delItem(static_cast<vChatViewer*>(v));
		v = channelViewers.getFirst();
	}

	if (topic) free(topic);
	topic = NULL;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		free((void*)user->nickname);
		free((void*)user);
		user = nicks.getNext(user);
	}
}


void vChannelLog::setTopic(const char *text)
{
	const char *oldTopic = topic;
	if (text == NULL) text = "";
//	if (topic) free(topic);
//	topic = NULL;
	
	int len = strlen(text);
	topic = (char*)malloc(len+1);
	memcpy(topic, text, len+1);

	vChannelViewer *v = channelViewers.getFirst();
	while (v) {
		v->onTopicChange(topic, oldTopic);
		v = channelViewers.getNext(v);
	}

	free((void*)oldTopic);
}


const char *vChannelLog::getFirstNick() {
	LPvChannelUser user = nicks.getFirst();
	if (user) return user->nickname;
	return NULL;
}


const char *vChannelLog::getNextNick(const char *n) {
	if (n == NULL) return NULL;
	if (*n == NULL) return NULL;

	LPvChannelUser user = nicks.getCached();
	if (user) if (strcmpi(user->nickname, n) == 0) {
		user = nicks.getNext(user);
		if (user) return user->nickname;
		return NULL;
	}

	user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, n) == 0) {
			user = nicks.getNext(user);
			if (user) return user->nickname;
			return NULL;
		}
		user = nicks.getNext(user);
	}

	return NULL;
}


BYTE vChannelLog::getUserStatus(const char *n)
{
	if (n == NULL) return NULL;
	if (*n == NULL) return NULL;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, n) == 0) return user->userMode;
		user = nicks.getNext(user);
	}

	return NULL;
}


void vChannelLog::addUser(const char *nick, BYTE status)
{
	if (*nick == ':') nick++;

	LPvChannelUser user = (LPvChannelUser) malloc(sizeof(vChannelUser));
	user->userMode = (status & 0x7F);
	int len = lstrlen(nick);
	user->nickname = (char*) malloc(len+1);
	memcpy((char*)user->nickname, nick, len+1);
	nicks.addToEnd(user);

	vChannelViewer *v = channelViewers.getFirst();
	while (v)
	{
		v->onNewUser(nick, status);
		v = channelViewers.getNext(v);
	}
}


void vChannelLog::delUser(const char *nick)
{
	if (nick == NULL) return;
	if (*nick == NULL) return;
	if (*nick == ':') nick++;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, nick) == 0) {
			nicks.delItem(user);
			free((void*)user->nickname);
			free((void*)user);
			break;
		}
		user = nicks.getNext(user);
	}

	
	vChannelViewer *v = channelViewers.getFirst();
	while (v)
	{
		v->onUserPart(nick);
		v = channelViewers.getNext(v);
	}
}


BOOL vChannelLog::hasUser(const char *nick) {
	if (nick == NULL) return FALSE;
	if (*nick == NULL) return FALSE;
	if (*nick == ':') nick++;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, nick) == 0) return TRUE;
		user = nicks.getNext(user);
	}
	return FALSE;
}


void vChannelLog::onUserStatusChange(const char *name, BYTE newStatus) {
	if (name == NULL) return;
	if (*name == NULL) return;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, name) == 0) {
			BYTE oldStatus = user->userMode;
			if (newStatus & 0x80) user->userMode |= (newStatus & 0x7F);
			else user->userMode ^= (newStatus & 0x7F);

			vChannelViewer *v = channelViewers.getFirst();
			while (v) {
				v->onUserStatusChange(name, user->userMode, oldStatus);
				v = channelViewers.getNext(v);
			}

			return;
		}
		user = nicks.getNext(user);
	}
}


void vChannelLog::part() {
	if (irc == NULL) return;
	char str[256];
	sprintf(str, "PART %s", getNameSafe(""));
	irc->Send(str);

	LPvIrcWindow wnd = (LPvIrcWindow) malloc(sizeof(vIrcWindow));
	ZeroMemory(wnd, sizeof(vIrcWindow));
	wnd->size = sizeof(vIrcWindow);
	wnd->msg = 2;
	wnd->log = this;
	irc->sendWndMsg(wnd);
}


void vChannelLog::onNickChange(const char *newNick, const char *oldNick)
{
	if (*oldNick == ':') oldNick++;

	LPvChannelUser user = nicks.getFirst();
	while (user) {
		if (strcmpi(user->nickname, oldNick) == 0) break;
		user = nicks.getNext(user);
	}

	if (user == NULL) return;

	vChannelViewer *v = channelViewers.getFirst();
	while (v) {
		v->onNickChange(newNick, oldNick);
		v = channelViewers.getNext(v);
	}

	free((void*)user->nickname);
	int len = lstrlen(newNick);
	user->nickname = (char*) malloc(len+1);
	memcpy((void*)user->nickname, newNick, len+1);
}


void vChannelLog::onMyNickChange(const char *newNick, const char *oldNick)
{
	vChannelViewer *v = channelViewers.getFirst();
	while (v) {
		v->onMyNickChange(newNick, oldNick);
		v = channelViewers.getNext(v);
	}
}