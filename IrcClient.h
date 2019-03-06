#ifndef __IRCCLIENT_H_
#define __IRCCLIENT_H_

//#include "../../../vBase/Base.H"
#include "../../../NetSlave lib/Connect.H"
//#include "IRCWnd.H"

class vChannelList;
class TextWnd;
class ServerObject;
class ChannelObject;
class vChatLog;
class vChannelLog;
struct vtagIrcWindow;


class vChatListViewer
{
protected:
	vChatListViewer() {}
	~vChatListViewer() {}
public:
	virtual int getLogType() { return -1; }
	virtual void onAddChat(vChatLog *data) {}
	virtual void onDeleteChat(vChatLog *data) {}
};


class vChannelListViewer : public vChatListViewer
{
protected:
	vChannelListViewer() {}
	~vChannelListViewer() {}
public:
	virtual int getLogType() { return 1; }
	virtual void onAddChannel(vChannelLog *data) {}
	virtual void onDeleteChannel(vChannelLog *data) {}
};



class vChannelList : public vList<vChannelLog*> {
public:
	vChannelList() {}
	~vChannelList() { deregDel(); }

	vChannelLog *getLog(const char *name);
//	vChatLog *getStatus() { return getLog("Status"); }

	void registerChannelViewer(vChannelListViewer *v) { channelViewers.addToEnd(v); }
	void deregisterChannelViewer(vChannelListViewer *v) { channelViewers.delItem(v); }
	void deregDel();

protected:
	virtual void onAddItem(vChannelLog *data);
	virtual void onDelete(vChannelLog *data);

private:
	vList<vChannelListViewer*> channelViewers;
};



class vChatList : public vList<vChatLog*> {
public:
	vChatList() {}
	~vChatList() { deregDel(); }

	vChatLog *getLog(const char *name);

	void registerChatViewer(vChatListViewer *v) { chatViewers.addToEnd(v); }
	void deregisterChatViewer(vChatListViewer *v) { chatViewers.delItem(v); }
	void deregDel();

protected:
	virtual void onAddItem(vChatLog *data);
	virtual void onDelete(vChatLog *data);

private:
	vList<vChatListViewer*> chatViewers;
};


class IrcConnect : public vConnect
{
public:
	IrcConnect();
	~IrcConnect();

	void connectToServer(const char *server = NULL, int port = 0);

	const char *getNick() { return nick; }
//	const char *getServer() { return server; }
	void setNick(const char *n);
//	void setServer(const char *s) { lstrcpy(server, s); }
	const char *getServer();
	int getServerPort();

	void statusMessage(const char *message);

	vChatLog *getStatus() { return status; }
	void activateStatus();

	vChatLog *getActiveLog();

	vChatLog *getLog(const char *name) { return logs.getLog(name); }
	vChatLog *getFirstLog() { return logs.getFirst(); }
	vChatLog *getNextLog(vChatLog *log) { return logs.getNext(log); }

	vChannelLog *getChannel(const char *name) { return channels.getLog(name); }
	vChannelLog *getFirstChannel() { return channels.getFirst(); }
	vChannelLog *getNextChannel(vChannelLog *log) { return channels.getNext(log); }

	void floatWindow(vChatLog *log);
	void unfloatWindow(vChatLog *log);

	void registerLog(vChatLog *log);
	void deregisterLog(vChatLog *log);
	void deregisterChannel(vChannelLog *log);
	void deregisterDeleteLogs();
	void deleteLog(vChatLog *log);

	void registerLogViewer(vChatListViewer *viewer);
	void deregisterLogViewer(vChatListViewer *viewer);

	void onDcc(char *message, char *sender);

	void registerObject(ServerObject *obj) { scripts.addToEnd(obj); }
	void deregisterObject(ServerObject *obj) { scripts.delItem(obj); }
	ServerObject *getFirstObject() { return scripts.getFirst(); }
	ServerObject *getNextObject(ServerObject *o) { return scripts.getNext(o); }

	void registerChannelObject(ChannelObject *obj) { channelScripts.addToEnd(obj); }
	void deregisterChannelObject(ChannelObject *obj) { channelScripts.delItem(obj); }
	ChannelObject *getFirstChannelObject() { return channelScripts.getFirst(); }
	ChannelObject *getNextChannelObject(ChannelObject *o) { return channelScripts.getNext(o); }

	LRESULT CALLBACK WndPrc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void sendWndMsg(vtagIrcWindow *msg);


protected:
	virtual void process(char *txt);

	virtual void onConnected();
	virtual void onDisconnected();

protected:
	vChannelList channels;
	vChatList logs;

	vChatLog *status;

private:
	vList<ServerObject*> scripts;
	vList<ChannelObject*> channelScripts;
	char nick[64];
};


typedef struct vtagIrcWindow {
	UINT size;
	UINT msg;
	vChatLog *log;
} vIrcWindow, *LPvIrcWindow;


#endif __IRCCLIENT_H_