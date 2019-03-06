#include "precomp.h"
#include "../../bfc/std.h"

#include "ChatLogViewer.H"
#include "ChatLog.H"
#include "IrcClient.H"


extern IrcConnect *irc;


vChatViewer::vChatViewer(const char *logName=NULL) {
//	vChatViewer(irc->getLog(chanName));
//	setVirtual(FALSE);
	autoactive = FALSE;
	log = NULL;
	assignLog(logName);
}


vChatViewer::vChatViewer(vChatLog *newLog) {
	autoactive = FALSE;
	log = NULL;
	assignLog(newLog);
	//if (log) log->registerViewer(this);
}


vChatViewer::~vChatViewer() {
	if (log) log->deregisterViewer(this);
}

/*
void vChatViewer::assignChannel(const char *name) {
	assignChannel(irc->getLog(name));
}*/


void vChatViewer::assignLog(vChatLog *newLog) {
	if (log) log->deregisterViewer(this);
	log = newLog;
	if (log) {
		log->registerViewer(this);
		if (log->getActivated() == FALSE) autoactive = FALSE;
	}
	assignTextLog(newLog);
	onAssignLog(log);
}


void vChatViewer::assignLog(const char *logName) {
	if (strcmpi(logName, "AutoActive") == 0) {
		autoactive = TRUE;
		assignLog(irc->getActiveLog());
	}
	else assignLog(irc->getLog(logName));
}


void vChatViewer::assignLogNoDeregister(vChatLog *newLog) {
	log = newLog;
	if (log) log->registerViewer(this);
}


void vChatViewer::assignLogNoDeregister(const char *logName) {
	if (logName == NULL) {
		log = NULL;
		return;
	}

	log = irc->getLog(logName);
	if (log) log->registerViewer(this);
}


const char *vChatViewer::getNameSafe(const char *safeName) {
	if (log) return log->getNameSafe(safeName);
	if (safeName == NULL) return "(NULL)";
	return safeName;
}

const char *vChatViewer::getName() { if (log) return log->getName(); return NULL; }

int vChatViewer::getLogType() { if (log) return log->getLogType(); return -1; }

void vChatViewer::setDocked(BOOL d) { if (log) log->setDocked(d); }
BOOL vChatViewer::getDocked() { if (log) return log->getDocked(); return FALSE; }

void vChatViewer::say(const char *message) { if (log) log->say(message); }
void vChatViewer::action(const char *message) { if (log) log->action(message); }
void vChatViewer::send(const char *message) { if (log) log->send(message); }
void vChatViewer::notice(const char *message) { if (log) log->notice(message); }
void vChatViewer::echo(const char *message) { if (log) log->addIrcText(message); }

void vChatViewer::clearLog() { if (log) log->clearLog(); }

BOOL vChatViewer::getFormatting() { if (log) return log->getFormatting(); return FALSE; }
void vChatViewer::setFormatting(BOOL f) { if (log) log->setFormatting(f); }

BOOL vChatViewer::getFlashing() { if (log) return log->getFlashing(); return FALSE; }
void vChatViewer::setFlashing(BOOL f) { if (log) log->setFlashing(f); }

BOOL vChatViewer::getTimeStamps() { if (log) return log->getStamps(); return FALSE; }
void vChatViewer::setTimeStamps(BOOL b) { if (log) log->setStamps(!!b); }
void vChatViewer::toggleTimeStamps() { setTimeStamps(!getTimeStamps()); }

void vChatViewer::activate() { if (log) log->activate(); }
BOOL vChatViewer::getActivated() { if (log) return log->getActivated(); return FALSE; }

void vChatViewer::part() { if (log) log->part(); }

//void vChatViewer::popMenu() { if (log) log->popMenu(); }
