#ifndef __CHATLOG_H_
#define __CHATLOG_H_

#include "../../../vBase/List.H"
#include "../../bfc/named.h"

#include "ircclient.h"
#include "TextLog.H"


class vChatLog;
class vChatViewer;
class IrcConnect;



typedef struct vtagTEXTLINE
{
	vtagTEXTLINE() { vZeroMemory(this, sizeof(vtagTEXTLINE)); }
	char *text;
	int textLen;
	int textWidth;
	int textRows;
	BYTE hour;
	BYTE min;
	BYTE sec;
	UINT type;
} vTextLine, *LPvTextLine;


typedef struct vtagUSER
{
	const char *nickname;
	BYTE userMode;
} vChannelUser, *LPvChannelUser;



class vChatLog : public TextLog, /*public vList<LPvTextLine>,*/ public Named
{
public:
	vChatLog();
	virtual ~vChatLog();

	virtual int getLogType() { return -1; }

	void registerViewer(vChatViewer *v) { viewers.addToEnd(v); }
	void deregisterViewer(vChatViewer *v) { viewers.delItem(v); }

	virtual void say(const char *message);
	virtual void action(const char *message);
	virtual void send(const char *message);
	virtual void notice(const char *message);
	virtual void ctcp(const char *user, const char *message, const char *type=NULL);

	virtual void part();

	virtual void popMenu();

	BOOL getActivated() { return active; }
	void activate();
	void deactivate();

	void addIrcText(const char *text);

//	void addText(const char *text);
	void clearLog();
	void appendLog(LPvTextLine text);

	void setStamps(BOOL s);
	BOOL getStamps() { return stamps; }

	void setFormatting(BOOL f);
	BOOL getFormatting() { return formatting; }

	void setFlashing(BOOL f);
	BOOL getFlashing() { return flashing; }

	IrcConnect *getServer() { return irc; }
	void setServer(IrcConnect *s);

	virtual void setDocked(BOOL d);
	virtual BOOL getDocked() { return docked; }

//	BOOL isStatus() { return (strcmp(name, "Status") == 0); }
  virtual void onSetName();
/*
protected:
	virtual void onAddItem(LPvTextLine data);
	virtual void onDelete(LPvTextLine data);
*/


protected:
	vList<vChatViewer*> viewers;
	bool deleting;
	bool registered;

	BOOL stamps;
	BOOL formatting;
	BOOL flashing;

	BOOL active;
	BOOL docked;

	IrcConnect *irc;
};


#endif//__CHATLOG_H_