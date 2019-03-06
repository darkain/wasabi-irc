#ifndef __CHATWND_H_
#define __CHATWND_H_

#include "../../bfc/wnds/buttwnd.h"
#include "../../bfc/appcmds.h"
#include "../../common/framewnd.h"
#include "../../bfc/attribs/attrhandler.h"
#include "ChannelLogViewer.H"


#define CHATWND_PARENT FrameWnd

class TextWnd;
class TreeWnd;
class vChatLog;


class ChatWnd : public CHATWND_PARENT, public AppCmdsI, public vChannelViewer
{
public:
	ChatWnd();
	~ChatWnd();

	static GUID getGUID();

	virtual int onInit();

	static const char *getWindowTypeName();
	static GUID getWindowTypeGuid();
  static void setIconBitmaps(ButtonWnd *button);

	virtual int setXmlParam(const char *name, const char *strvalue);
//	virtual int childNotify(RootWnd *child, int msg, int param1, int param2);

	static void setNextLog(vChatLog *log=NULL);

	virtual void onSetDocked(BOOL dock);
	void setUndocked(BOOL b) { undocked = b; }

	virtual void onExitChannel();
	virtual void onAssignLog(vChatLog *newLog);
//	virtual void onDeactivated();
	virtual void onTopicChange(const char *newTopic, const char *oldTopic);

	virtual void onUserStatusChange(const char *name, BYTE newStatus, BYTE oldStatus);
	virtual void onNickChange(const char *newNick, const char *oldNick);
	virtual void onNewUser(const char *name, BYTE status=0);
	virtual void onUserPart(const char *name);
	void refreshUserList();

//	virtual void appcmds_onCommand(int id, const RECT *buttonRect);
  virtual void appcmds_onCommand(int id, const RECT *buttonRect, int which_button);
	void AddCommand(const char *name, int _id, int _side=0)
	{ CmdRec *c = new CmdRec(name, _id, _side, 1); appcmds_addCmd(c); }

private:
	TextWnd *textwnd;
//	EditWnd *editwnd;
	TreeWnd *treewnd;
//	FrameWnd *chatfrm;
	
	BOOL undocked;
};

extern char ChatLogXuiObjectStr[];
extern char ChatLogXuiSvcName[];
class ChatLogXuiSvc : public XuiObjectSvc<ChatWnd, ChatLogXuiObjectStr, ChatLogXuiSvcName> {};


#endif
