#include "precomp.h"
#include "../../bfc/std.h"
#include "ChannelLogViewer.H"
#include "ChannelLog.H"


vChannelViewer::vChannelViewer(vChannelLog *newLog) : vChatViewer(newLog) {
	chanLog = (static_cast<vChannelLog*>(newLog));
	if (chanLog) chanLog->registerChannelViewer(this);
}


vChannelViewer::~vChannelViewer() {
	if (chanLog) chanLog->deregisterChannelViewer(this);
}


void vChannelViewer::assignLog(vChatLog *newLog) {
	if (chanLog) chanLog->deregisterChannelViewer(this);
	chanLog = NULL;
	if (newLog) if (newLog->getLogType() == 1) {
		chanLog = (static_cast<vChannelLog*>(newLog));
		chanLog->registerChannelViewer(this);
	}
	vChatViewer::assignLog(newLog);
}


void vChannelViewer::assignLogNoDeregister(vChatLog *newLog) {
	chanLog = NULL;
	if (newLog) if (newLog->getLogType() == 1) chanLog = (static_cast<vChannelLog*>(newLog));
	if (chanLog) chanLog->registerChannelViewer(this);
	vChatViewer::assignLogNoDeregister(newLog);
}

void vChannelViewer::setTopic(const char *newTopic) { if (chanLog) chanLog->setTopic(newTopic); }
const char *vChannelViewer::getTopic() { if (chanLog) return chanLog->getTopic(); return NULL; }

int vChannelViewer::getUserCount() { if (chanLog) return chanLog->getNumUsers(); return 0; }
const char *vChannelViewer::getFirstUser() { if (chanLog) return chanLog->getFirstNick(); return NULL; }
const char *vChannelViewer::getNextUser(const char *user) { if (chanLog) return chanLog->getNextNick(user); return NULL; }
BOOL vChannelViewer::hasUser(const char *name) { if (chanLog) return chanLog->hasUser(name); return FALSE; }
BYTE vChannelViewer::getUserStatus(const char *name) { if (chanLog) return chanLog->getUserStatus(name); return 0; }

const char *vChannelViewer::enumUser(int num) {
	if (chanLog == NULL) return NULL;
	if (num > chanLog->getNumUsers()) return NULL;
	const char *name = chanLog->getFirstNick();
	for (int i=0; i<num; i++) {
		name = chanLog->getNextNick(name);
		if (name == NULL) return NULL;
	}
	return name;
}
