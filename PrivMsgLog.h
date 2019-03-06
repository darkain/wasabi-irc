#ifndef __PRIVMSGLOG_H_
#define __PRIVMSGLOG_H_


#include "ChatLog.H"


class vPrivMsgLog : public vChatLog {
public:
	vPrivMsgLog() {}
	virtual ~vPrivMsgLog() {}

	virtual int getLogType() { return 2; }
};


#endif//__PRIVMSGLOG_H_