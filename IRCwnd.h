#ifndef __IRCWND_H_
#define __IRCWND_H_

#include "../../common/contwnd.h"
//#include "../../common/nsGUID.h"
#include "../../bfc/skinclr.h"
//#include "../../bfc/wnds/scrollbar.h"

//#include "../../../../vBase/Base.H"
#include "../../../NetSlave lib/Connect.H"

#include "TextLogWnd.H"
#include "ircclient.h"
#include "ChatLogViewer.H"
#include "ChatLog.H"


//#define TEXTWND_PARENT ContWnd
#define TEXTWND_PARENT TextLogWnd
class IrcConnect;
//class TreeWnd;


class TextClicker : public Named {
public:
	TextClicker(int newX=0, int newY=0, int newW=0, int newH=0, const char *newText=NULL);
//	virtual ~TextClicker();

	BOOL isValidRegion(int testX, int textY);
	virtual void activate();

private:
	int x, y, w, h;
};


class ClickList : public vList<TextClicker*> {
public:
	ClickList() {}
	virtual void onDelete(TextClicker *data) { delete data; }
};


class TextWnd : public TEXTWND_PARENT//, public vChatViewer
{
public:
	TextWnd();
	~TextWnd();

	virtual int onInit();
//	virtual int onResize();
	virtual int onPaint(Canvas *canvas);

//	virtual void onNewChannel() { invalidate(); }
	virtual void onLogCleared() { invalidate(); }
	virtual void onNewMessage(const char *message, const char *nick=NULL) { invalidate(); }
	virtual void onSetStamps(BOOL enabled) { invalidate(); }
	virtual void onSetFormatting(BOOL enabled) { invalidate(); }
	virtual void onAssignLog(vChatLog *newLog) { invalidate(); assignTextLog(newLog); }

//	virtual int onLeftButtonDown(int x, int y);
//	virtual int onRightButtonDown(int x, int y) { /*popMenu();*/ return 0; }
//	virtual int onRightButtonUp(int x, int y) { return 0; }

//private:
//	SkinColor text_color;
//	ScrollBar *vScroll;
//	int totalLines;
//	ClickList clickers;
};




#endif __IRCWND_H_
