#ifndef __SENDBAR_H_
#define __SENDBAR_H_


#include "../../common/framewnd.h"
#include "../../common/editwnd.h"
#include "../../bfc/wnds/buttwnd.h"
#include "../../bfc/notifmsg.h"
#include "ChatLogViewer.H"


#define SENDBAR_PARENT FrameWnd
class Sendbar : public SENDBAR_PARENT, public vChatViewer {
public:
	Sendbar();
	~Sendbar();

	void sendText();

	virtual int onInit();

	virtual void onAssignLog(vChatLog *newLog) {}

	virtual int setXmlParam(const char *name, const char *strvalue);
	virtual int childNotify(RootWnd *child, int msg, int param1, int param2);

	void updatePosition();

private:
	EditWnd *sendEdt;
	ButtonWnd *sendBtn;
	int btnWidth;
	int buttonPos;
	BOOL allowUpdates;
};


extern char SendbarXuiObjectStr[];
extern char SendbarXuiSvcName[];
class SendbarXuiSvc : public XuiObjectSvc<Sendbar, SendbarXuiObjectStr, SendbarXuiSvcName> {};


//--------------------------------------------------------------------------


class EditText : public EditWnd {
public:	
	EditText() : EditWnd("", 250) { ctrl = FALSE; }

	virtual int onEnter() {
		notifyParent(ChildNotify::BUTTON_LEFTPUSH, 0, 0);
		return EditWnd::onEnter();
	}

  virtual LRESULT editWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void insertCharacter(char c);

  virtual void setBuffer(char *buffer, int len)
	{ SendMessage(getEditWnd(), WM_SETTEXT, 0, (LPARAM)buffer);	}
  
	virtual void getBuffer(char *outbuf, int len)
	{ SendMessage(getEditWnd(), WM_GETTEXT, len, (LPARAM)outbuf);	}

private:
	BOOL ctrl;
};


#endif//__SENDBAR_H_
