#ifndef __CHANNELLOGVIEWER_H_
#define __CHANNELLOGVIEWER_H_


#include "ChatLogViewer.H"


class vChannelLog;


class vChannelViewer : public vChatViewer {
	protected:
		vChannelViewer(vChannelLog *newLog = NULL);
		~vChannelViewer();


	public:
		virtual void assignLog(vChatLog *newLog);
		virtual void assignLogNoDeregister(vChatLog *newLog);

		void setTopic(const char *newTopic);
		const char *getTopic();
		virtual void onTopicChange(const char *newTopic, const char *oldTopic) {}

		virtual void onExitChannel() {}

		virtual void onNewUser(const char *name, BYTE status=0) {}
		virtual void onUserPart(const char *name) {}
		virtual void onUserStatusChange(const char *name, BYTE newStatus, BYTE oldStatus) {}
		int getUserCount();
		const char *enumUser(int num);
		const char *getFirstUser();
		const char *getNextUser(const char *user);
		BOOL hasUser(const char *name);
		BYTE getUserStatus(const char *name);
		virtual void onNickChange(const char *newNick, const char *oldNick) {}
		virtual void onMyNickChange(const char *newNick, const char *oldNick) {}

		vChannelLog *getLog() { return chanLog; }
	protected:
		vChannelLog *chanLog;
};


#endif//__CHANNELLOGVIEWER_H_