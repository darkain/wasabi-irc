#ifndef __TEXTLOGWND_H_
#define __TEXTLOGWND_H_


#include "../../common/contwnd.h"
#include "../../bfc/skinclr.h"
#include "../../bfc/wnds/scrollbar.h"
#include "../../bfc/wnds/scbkgwnd.h"
#include "../../bfc/wnds/blankwnd.h"

#include "TextLog.H"


#define TEXTLOGWND_PARENT ContWnd


//basic class for testing clickable regions within the text window
class TextClickRegion {
	protected:
		TextClickRegion(int nx=-1, int ny=-1, int nw=-1, int nh=-1) { setRegion(x, y, w, h); }
	public:
		virtual void activateRegion() {}
		void setRegion(int nx=0, int ny=0, int nw=0, int nh=0) {
			x = nx;  y = ny; w = nw;  h = nh;
		}

		BOOL testRegion(int nx, int ny) {
			if ( (nx >= x) & (nx < x+w) & (ny >= y) & (ny < y+h) ) return TRUE;
			return FALSE;
		}
	protected:
		int x, y, w, h;
};



//the text window itself.  this one is done "Text Log" style,
//such as a message log, chat lot, event log, things like that.
//soon, i will make another version more geared twards things like
//liscense text displays, redmes, and other varions things like that.
class TextLogWnd : public TEXTLOGWND_PARENT, public TextLogDependant {
	public:
		TextLogWnd(TextLog *log);
		TextLogWnd(BOOL createLog=FALSE);
		virtual ~TextLogWnd();

		int getLineCount(BOOL recalc=TRUE);
		int getScrollLine();

		//set the text display margins on the screen
		//note:  gotta add xml params for these
		void setMargins(int MarginL=-1, int MarginR=-1, int MarginT=-1, int MarginB=-1, int MarginW=-1);
		void setMarginLeft(int Margin);
		void setMarginRight(int Margin);
		void setMarginTop(int Margin);
		void setMarginBottom(int Margin);
		
		//set this to -1 to use same val as left margin
		//if this is -1, and left margin changes,
		//this is automatically updated
		void setMarginWrap(int Margin=-1); 


		const char *renderTimestamp(time_t time);
		virtual void onTextChanged() { invalidate(); }

		virtual int onInit();
		virtual int onResize();
		virtual int onPaint(Canvas *canvas);
		virtual int childNotify(RootWnd *child, int msg, int param1, int param2);

		virtual int onLeftButtonDown(int x, int y);
		virtual int onRightButtonDown(int x, int y);
		virtual int onRightButtonUp(int x, int y);

		virtual int setXmlParam(const char *name, const char *strvalue);

//		virtual void popMenu() {}

//		virtual int getContentsHeight() { return totalHeight; }	// not safe to call getclientrect!

	private:
		ScrollBar *vScroll;
		BOOL delLog;
		int linecount;
		int linescroll;
		char tsbuffer[16];

		int marginl, marginr;	//left and right margins
		int margint, marginb;	//top and bottom margins
		int marginw;					//margin for wrapped text

		class ClickRegions : public vList<TextClickRegion*> {
			virtual void onDelete(TextClickRegion *data) { delete data; }
		};
		ClickRegions clickables;
};


class UrlClickRegion : public TextClickRegion {
	public:
		UrlClickRegion(int nx, int ny, int nw, int nh, LPLogWord word) {
			setRegion(nx, ny, nw, nh);
			logword = word;
		}

		virtual void activateRegion();

	private:
		LPLogWord logword;
};


#endif//__TEXTLOGWND_H_