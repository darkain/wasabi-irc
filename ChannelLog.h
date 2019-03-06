#ifndef __CHANNELLOG_H_
#define __CHANNELLOG_H_


#include "ChatLog.H"

class vChannelViewer;


class vChannelLog : public vChatLog {
public:
	vChannelLog();
	virtual ~vChannelLog();


public:
	virtual int getLogType() { return 1; }

	void registerChannelViewer(vChannelViewer *v) { channelViewers.addToEnd(v); }
	void deregisterChannelViewer(vChannelViewer *v) { channelViewers.delItem(v); }

	void setTopic(const char *text);
	const char *getTopic() { return topic; }

	virtual void part();

	void addUser(const char *nick, BYTE status=0);
	void delUser(const char *nick);
	BOOL hasUser(const char *nick);
	int  getNumUsers() { return nicks.getTotalItems(); }
	void onNickChange(const char *newNick, const char *oldNick);
	void onMyNickChange(const char *newNick, const char *oldNick);
	void onUserStatusChange(const char *name, BYTE newStatus);

	const char *getFirstNick();
	const char *getNextNick(const char *n);
	BYTE getUserStatus(const char *n);
	LPvChannelUser getFirstUser() { return nicks.getFirst(); }
	LPvChannelUser getNextUser(LPvChannelUser u) { return nicks.getNext(u); }

	BOOL isJoined() { return joined; }
	void setJoined(BOOL j) { joined = !!j; }


private:
	vList<vChannelViewer*> channelViewers;
	vList<LPvChannelUser> nicks;
	char *topic;
	BOOL joined;
};


#endif//__CHANNELLOG_H_
