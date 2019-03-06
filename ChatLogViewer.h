#ifndef __CHATLOGVIEWER_H_
#define __CHATLOGVIEWER_H_

#include "TextLog.H"


class vChatLog;



class vChatViewer : public TextLogDependant {
protected:
	vChatViewer(const char *chanName);
	vChatViewer(vChatLog *newLog = NULL);
	~vChatViewer();

public:
//	virtual void assignLog(const char *name);
	virtual void assignLog(vChatLog *newLog);
	virtual void assignLog(const char *logName=NULL);
	virtual void assignLogNoDeregister(vChatLog *newLog);
	virtual void assignLogNoDeregister(const char *logName=NULL);
	virtual void onAssignLog(vChatLog *newLog) {}
	virtual void onLogDelete() {}
	virtual void onLogHide() {}
	virtual void onLogShow() {}
	BOOL getAutoActive() { return autoactive; }
	void autoActivate() { assignLog("AutoActive"); };

	int getLogType();

	const char *getName();
	const char *getNameSafe(const char *safeName);

	void activate();
	BOOL getActivated();
	virtual void onActivated() {}
	virtual void onDeactivated() {}

	vChatLog *getChatLog() { return log; }
	const char *getChatName();


	virtual void onSetDocked(BOOL dock) {}
	void setDocked(BOOL d);
	BOOL getDocked();

	void part();

//	void popMenu();

	void say(const char *message);
	void action(const char *message);
	void send(const char *message);
	void notice(const char *message);
	void echo(const char *message);

	BOOL getTimeStamps();
	void setTimeStamps(BOOL b);
	void toggleTimeStamps();
	virtual void onSetStamps(BOOL enabled) {}

	virtual void onSetFormatting(BOOL enabled) {}
	BOOL getFormatting();
	void setFormatting(BOOL f);
	
	virtual void onSetFlashing(BOOL enabled) {}
	BOOL getFlashing();
	void setFlashing(BOOL f);
	
	void clearLog();
	virtual void onLogCleared() {}
	virtual void onNewMessage(const char *message, const char *nick=NULL) {}

protected:
	vChatLog *log;
	BOOL autoactive;
};


#endif//__CHATLOGVIEWER_H_