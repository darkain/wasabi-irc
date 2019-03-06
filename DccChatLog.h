#ifndef __DCCCHATLOG_H_
#define __DCCCHATLOG_H_


#include "ChatLog.H"


class vDccChat : public vChatLog, public vConnect {
public:
	vDccChat() {}
	virtual ~vDccChat() {}

	virtual int getLogType() { return 3; }

	virtual void say(const char *message);

	virtual void onRequestConnection(SOCKET sock);
	virtual void process(char *txt);
};


#endif//__DCCCHATLOG_H_