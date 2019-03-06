#include "precomp.h"
#include "../../bfc/std.h"
#include "DccChatLog.H"


void vDccChat::say(const char *message) {
	Send(message);

	char str[300];
	sprintf(str, "<%s> %s", irc->getNick(), message);
	addIrcText(str);
}


void vDccChat::process(char *txt) {
	char str[300];
	sprintf(str, "<%s> %s", vChatLog::getNameSafe("DCC_Chat"), txt);
	addIrcText(str);
}


void vDccChat::onRequestConnection(SOCKET sock) {
	vConnect::onRequestConnection(sock);
	closesocket(listener);
	listener = NULL;
	addIrcText("DCC CHAT request accepted");
}