#ifndef __TOOLBAR_H_
#define __TOOLBAR_H_


#include "../../common/contwnd.h"
#include "../../bfc/wnds/buttwnd.h"
#include "../../../vBase/List.H"
#include "ChatLogViewer.H"
#include "IrcClient.H"


class ToolbarBtn;


class Toolbar : public ContWnd {
public:
	Toolbar();
	virtual ~Toolbar();

public:
	void addButton(ToolbarBtn *button);
	void instertButton(ToolbarBtn *button, ToolbarBtn *insertAfter);
	void deleteButton(ToolbarBtn *button);

	virtual int onResize();
	virtual int onInit();

	virtual int setXmlParam(const char *name, const char *strvalue);

private:
	BOOL deleting;
	ButtonWnd *bg;  //make XML param to turn on and off
	vList<ToolbarBtn*> buttons;

	int buttonAlign;
	int buttonVertAlign;
//	ButtonJustify buttonTextAlign;
	TextAlign alignment;
	int buttonWidth;
	int buttonHeight;
	int buttonSpacing;
};


class ToolbarBtn : public ButtonWnd {
public:
	ToolbarBtn(Toolbar *bar, const char *button_text=NULL, int notifyId=0);
	virtual ~ToolbarBtn();

private:
	Toolbar *toolbar;
};


extern char ToolbarXuiObjectStr[];
extern char ToolbarXuiSvcName[];
class ToolbarXuiSvc : public XuiObjectSvc<Toolbar, ToolbarXuiObjectStr, ToolbarXuiSvcName> {};


//-------------------------------------------------------------------
//wIRC specifics


class ButtonMenu : public ToolbarBtn, public vChatViewer {
	public:
		ButtonMenu(Toolbar *bar, const char *button_text=NULL);

		//ClickWnd
		virtual int onInit();
		virtual void onLeftPush(int x, int y);
		virtual int onRightButtonDown(int x, int y) { popMenu(); return 0; }

		//vChatViewer
		virtual void onActivated();
		virtual void onDeactivated();
		virtual void onSetDocked(BOOL dock);
		virtual void onLogDelete();
		virtual void onSetFlashing(BOOL enabled);
		virtual void onNewMessage(const char *message, const char *nick=NULL);
		virtual void onTextChanged() { onNewMessage("", ""); }

		//TimerClientI
		virtual void timerclient_timerCallback(int id);

	private:
		BOOL flash;
		int flashCount;
};


class ManagerTop : public Toolbar, public vChatListViewer {
	public:
		ManagerTop();
		~ManagerTop();

		virtual int onInit();
		virtual void onAddChat(vChatLog *data);
};


extern char ChannelbarXuiObjectStr[];
extern char ChannelbarXuiSvcName[];
class ChannelbarXuiSvc : public XuiObjectSvc<ManagerTop, ChannelbarXuiObjectStr, ChannelbarXuiSvcName> {};

/*
class ButtonNoContext : public ButtonWnd
{
public:
	ButtonNoContext(const char *button_text=NULL) : ButtonWnd(button_text) {}
	virtual int onRightButtonUp(int x, int y) {
		notifyParent(ChildNotify::CLICKWND_RIGHTUP);
		return 0;
	}
};
*/

#endif//__TOOLBAR_H_