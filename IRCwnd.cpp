#include "precomp.h"
#include "../../bfc/std.h"

#include "IRC.h"
#include "IRCwnd.h"
#include "IRCClient.H"
#include "resource.h"
#include "ChatLog.H"

#include "../../bfc/canvas.h"
#include "../../common/xlatstr.h"
#include "../../common/nsguid.h"
#include "../../bfc/wnds/treewnd.h"

#include <shellapi.h>

extern IrcConnect *irc;


TextWnd::TextWnd() {
	setName("TextWnd");
//	log = NULL;
	setHandleRightClick(TRUE);
//	text_color = "wasabi.text";
//	totalLines = 0;
	setMargins(3, 1, -1, 1, 15);
}


TextWnd::~TextWnd() {
}



int TextWnd::onInit() {
/*	vScroll = new ScrollBar();
	vScroll->init(this);
 
	vScroll->setBitmaps("wasabi.scrollbar.vertical.left", 
                      "wasabi.scrollbar.vertical.left.pressed", 
                      "wasabi.scrollbar.vertical.left.hover",
                      "wasabi.scrollbar.vertical.right", 
                      "wasabi.scrollbar.vertical.right.pressed", 
                      "wasabi.scrollbar.vertical.right.hover",
                      "wasabi.scrollbar.vertical.button", 
                      "wasabi.scrollbar.vertical.button.pressed", 
                      "wasabi.scrollbar.vertical.button.hover");

  vScroll->setBackgroundBitmaps("wasabi.scrollbar.vertical.background.top", 
                                "wasabi.scrollbar.vertical.background.middle", 
                                "wasabi.scrollbar.vertical.background.bottom");

  vScroll->setVertical(TRUE);
	vScroll->setNotifyId(11);
	vScroll->setNotifyWindow(this);
	vScroll->setPosition(65535);
	//vScroll->setUpDownValue(MAKELONG(100,200));*/

	int retval = TEXTWND_PARENT::onInit();
	return retval;
}

/*
int TextWnd::onResize() {
	int retval = TEXTWND_PARENT::onResize();
	if (vScroll == NULL) return retval;
  RECT crect = clientRect();

	int width = crect.right - crect.left;
	int height = crect.bottom - crect.top;
	int swidth = vScroll->getWidth();

	vScroll->resize(width-swidth, 0, swidth, height);

	return retval;
}
*/
/*
int TextWnd::onLeftButtonDown(int x, int y) {
	TextClicker *cl = clickers.getFirst();
	while (cl) {
		if (cl->isValidRegion(x, y)) { cl->activate(); return 0; }
		cl = clickers.getNext(cl);
	}
	return TEXTWND_PARENT::onLeftButtonDown(x, y);
}
*/

/*
typedef struct vtagPAINT {
	vtagPAINT() {
		ZeroMemory(this, sizeof(vtagPAINT));
		background = -1;
//		color = text_color;
	}
	int x;
	int y;
	int width;
	int height;
	char *text;
	int len;
	bool bold;
	bool italic;
	bool underline;
	bool strike;
	UINT color;
	UINT background;
} vPaint, *LPvPaint;


class outList : public vList<LPvPaint> { public: outList() {} };

*/
int TextWnd::onPaint(Canvas *canvas)
{
  TEXTWND_PARENT::onPaint(canvas);
return 1;
/*
  PaintBltCanvas pbc;
  if (canvas == NULL) {
    pbc.beginPaint(this);
    canvas = &pbc;
  }

	clickers.delAll();

  RECT crect = clientRect();
	if (log == NULL) {
		canvas->textOutCentered(&crect, "Invalid LOG Specified");
		return 1;
	}

	int width = crect.right - crect.left;
	int height = crect.bottom - crect.top;
	if (vScroll) width -= vScroll->getWidth();
	int textHeight = canvas->getTextHeight();

  canvas->setTextColor(text_color);
//	canvas->setTextFont("Lucida Console");
  canvas->setTextSize(14);


/*
	outList lines;
	outList list;
	outList word;
*/
//	char buffer[1024];
/*
	int line = 1;
	int totalLineCount = 0;

	{
		if (totalLines < 1) totalLines = 1;
		int c = totalLines - 1;
		int a = (-vScroll->getPosition()+65536);
		if (a == 0) a++;
		int b =  (a*c/65536);
		line = -b;
	}

	int y = height - textHeight;
*/
	//this section here is was the second attept at a parser, but was way too damn slow
/*
	LPvTextLine data = log->getLast();
	while (data)
	{
//		if ((y + textHeight) < 0) break;

		const char *text = data->text;
		char *buf = buffer;
		int lineCount = 1;
		int wordWidth = 0;

		bool bold = false;
		bool italic = false;
		bool underline = false;
		canvas->setTextBold(bold);
		canvas->setTextItalic(italic);
		canvas->setTextUnderline(underline);
		UINT color = text_color;
		UINT bcolor = -1;

		int lineWidth = 5;
		if (getTimeStamps()) {
			LPvPaint paint = new vPaint;
			sprintf(buffer, "[%02d:%02d:%02d] ", data->hour, data->min, data->sec);
			int len = strlen(buffer);
			paint->text = (char*)malloc(len+1);
			memcpy(paint->text, buffer, len+1);
			paint->len = len;
			canvas->getTextExtent(paint->text, &paint->width, &paint->height);
			paint->x = lineWidth;
			paint->y = y;
			paint->color = color;
			list.addToEnd(paint);
			lineWidth += paint->width;
		}

		while (1) {
			if ( (*text == ' ') | (*text == NULL) ) {
				*buf = NULL;
				int len = strlen(buffer);
				LPvPaint paint = new vPaint;
				paint->text = (char*)malloc(len+1);
				memcpy(paint->text, buffer, len+1);
				paint->len = len;
				canvas->getTextExtent(paint->text, &paint->width, &paint->height);
				paint->bold = bold;
				paint->italic = italic;
				paint->underline = underline;
				paint->color = color;
				paint->background = bcolor;

				LPvPaint pnt = NULL;
				if ((lineWidth + paint->width) > width) {
					totalLineCount++;
					pnt = list.getFirst();
					while (pnt) {
						lines.addToEnd(pnt);
						pnt = list.getNext(pnt);
					}
					list.delAll();
					lineWidth = 15;
					lineCount++;

					pnt = lines.getFirst();
					while (pnt) {
						pnt->y -= textHeight;
						pnt = lines.getNext(pnt);
					}
					pnt = word.getFirst();
					while (pnt) {
						lines.addToEnd(pnt);
						pnt->x = lineWidth;
						lineWidth += pnt->width;
						pnt = word.getNext(pnt);
					}
				}
				else {
					pnt = word.getFirst();
					while (pnt) {
						lines.addToEnd(pnt);
						pnt = word.getNext(pnt);
					}
				}
				word.delAll();

				paint->x = lineWidth;
				paint->y = y;
				list.addToEnd(paint);
				lineWidth += paint->width;

				if (*text == NULL) {
					totalLineCount++;
					LPvPaint pnt = list.getFirst();
					while (pnt) {
						lines.addToEnd(pnt);
						pnt = list.getNext(pnt);
					}
					pnt = word.getFirst();
					while (pnt) {
						lines.addToEnd(pnt);
						pnt = word.getNext(pnt);
					}
					word.delAll();
					list.delAll();
				}

				wordWidth = 0;
				buf = buffer;
				*buf = ' ';
			}
			
			else if (*text == 0x02) {
				if (getFormatting()) {
					*buf = NULL;
					int len = strlen(buffer);
					if (len > 0) {
						LPvPaint paint = new vPaint;
						paint->text = (char*)malloc(len+1);
						memcpy(paint->text, buffer, len+1);
						paint->len = len;
						canvas->getTextExtent(paint->text, &paint->width, &paint->height);
						paint->bold = bold;
						paint->italic = italic;
						paint->underline = underline;
						paint->color = color;
						paint->background = bcolor;
						paint->x = lineWidth;
						paint->y = y;
						word.addToEnd(paint);
						lineWidth += paint->width;
						wordWidth += paint->width;
					}
					bold = !bold;
					canvas->setTextBold(bold);
					buf = buffer-1;
				}
				else buf--;
			}

			else if (*text == 0x16) {
				if (getFormatting()) {
					*buf = NULL;
					int len = strlen(buffer);
					if (len > 0) {
						LPvPaint paint = new vPaint;
						paint->text = (char*)malloc(len+1);
						memcpy(paint->text, buffer, len+1);
						paint->len = len;
						canvas->getTextExtent(paint->text, &paint->width, &paint->height);
						paint->bold = bold;
						paint->italic = italic;
						paint->underline = underline;
						paint->color = color;
						paint->background = bcolor;
						paint->x = lineWidth;
						paint->y = y;
						word.addToEnd(paint);
						lineWidth += paint->width;
						wordWidth += paint->width;
					}
					italic = !italic;
					canvas->setTextBold(bold);
					buf = buffer-1;
				}
				else buf--;
			}

			else if (*text == 0x1F) {
				if (getFormatting()) {
					*buf = NULL;
					int len = strlen(buffer);
					if (len > 0) {
						LPvPaint paint = new vPaint;
						paint->text = (char*)malloc(len+1);
						memcpy(paint->text, buffer, len+1);
						paint->len = len;
						canvas->getTextExtent(paint->text, &paint->width, &paint->height);
						paint->bold = bold;
						paint->italic = italic;
						paint->underline = underline;
						paint->color = color;
						paint->background = bcolor;
						paint->x = lineWidth;
						paint->y = y;
						word.addToEnd(paint);
						lineWidth += paint->width;
						wordWidth += paint->width;
					}
					underline = !underline;
					canvas->setTextUnderline(underline);
					buf = buffer-1;
				}
				else buf--;
			}

			else if (*text == 0x03) {
				int c1 = -1;
				int c2 = -1;
				const char *num = text+1;

				if ( (*num >= '0') & (*num <= '9') ) {
					c1 = *num-'0';
					num++;
					if ( (*num >= '0') & (*num <= '9') ) {
						c1 *= 10;
						c1 += *num-'0';
						num++;
					}

					if (*num == ',')
					{
						num++;
						if ( (*num >= '0') & (*num <= '9') ) {
							c2 = *num-'0';
							num++;
							if ( (*num >= '0') & (*num <= '9') ) {
								c2 *= 10;
								c2 += *num-'0';
								num++;
							}
						}
					}
				}

				if (getFormatting()) {
					*buf = NULL;
					int len = strlen(buffer);
					if (len > 0) {
						LPvPaint paint = new vPaint;
						paint->text = (char*)malloc(len+1);
						memcpy(paint->text, buffer, len+1);
						paint->len = len;
						canvas->getTextExtent(paint->text, &paint->width, &paint->height);
						paint->bold = bold;
						paint->italic = italic;
						paint->underline = underline;
						paint->color = color;
						paint->background = bcolor;
						paint->x = lineWidth;
						paint->y = y;
						word.addToEnd(paint);
						lineWidth += paint->width;
						wordWidth += paint->width;
					}
					if (c1 >= 0) {
						c1 = (c1 & 0x8000000F);
						switch (c1) {
						case 0:	color = RGB(255, 255, 255); break;
						case 1:	color = RGB(0, 0, 0); break;
						case 2:	color = RGB(0, 0, 127); break;
						case 3:	color = RGB(0, 127, 0); break;
						case 4:	color = RGB(255, 0, 0); break;
						case 5:	color = RGB(128, 0, 0); break;
						case 6:	color = RGB(128, 0, 128); break;
						case 7:	color = RGB(128, 128, 0); break;
						case 8:	color = RGB(255, 255, 0); break;
						case 9:	color = RGB(0, 255, 0); break;
						case 10:	color = RGB(0, 128, 128); break;
						case 11:	color = RGB(0, 255, 255); break;
						case 12:	color = RGB(0, 0, 255); break;
						case 13:	color = RGB(255, 0, 255); break;
						case 14:	color = RGB(128, 128, 128); break;
						case 15:	color = RGB(196, 196, 196); break;
						default:	color = RGB(255, 255, 255); bcolor = -1; break;
						}
						c2 = (c2 & 0x8000000F);
						if (c2 >= 0)  switch (c2) {
							case 0:	bcolor = RGB(255, 255, 255); break;
							case 1:	bcolor = RGB(0, 0, 0); break;
							case 2:	bcolor = RGB(0, 0, 127); break;
							case 3:	bcolor = RGB(0, 127, 0); break;
							case 4:	bcolor = RGB(255, 0, 0); break;
							case 5:	bcolor = RGB(128, 0, 0); break;
							case 6:	bcolor = RGB(128, 0, 128); break;
							case 7:	bcolor = RGB(128, 128, 0); break;
							case 8:	bcolor = RGB(255, 255, 0); break;
							case 9:	bcolor = RGB(0, 255, 0); break;
							case 10:	bcolor = RGB(0, 128, 128); break;
							case 11:	bcolor = RGB(0, 255, 255); break;
							case 12:	bcolor = RGB(0, 0, 255); break;
							case 13:	bcolor = RGB(255, 0, 255); break;
							case 14:	bcolor = RGB(128, 128, 128); break;
							case 15:	bcolor = RGB(196, 196, 196); break;
						}
					} else {
						color = text_color;
						bcolor = -1;
					}
					buf = buffer-1;
				}
				else buf--;
				text = num - 1;
			}

			else {
				*buf = *text;
			}
			if (*text == NULL) break;
			buf++;
			text++;
		}

		bold = false;
		italic = false;
		underline = false;
		color = text_color;
		bcolor = -1;

		canvas->setTextBold(bold);
		canvas->setTextItalic(italic);
		canvas->setTextUnderline(underline);
		canvas->setTextColor(color);
		canvas->setTextBkColor(bcolor);
		canvas->setTextOpaque(FALSE);

		int oldY;

		if (++line > 0)
		{
			LPvPaint paint = lines.getFirst();
			while (paint) {
				if (bold != paint->bold) {
					bold = paint->bold;
					canvas->setTextBold(bold);
				}

				if (italic != paint->italic) {
					italic = paint->italic;
					canvas->setTextItalic(italic);
				}

				if (underline != paint->underline) {
					underline = paint->underline;
					canvas->setTextUnderline(underline);
				}

				if (paint->color != color) {
					color = paint->color;
					canvas->setTextColor(color);
				}

				if (paint->background != bcolor) {
					bcolor = paint->background;
					if (bcolor == -1) {
						canvas->setTextOpaque(FALSE);
					} else {
						canvas->setTextBkColor(bcolor);
						canvas->setTextOpaque(TRUE);
					}
				}
				
				if (oldY == paint->y)
					canvas->textOut(paint->x, paint->y, paint->width, paint->height, paint->text);
				else {
					int w =  width - paint->x;
					paint->width = MIN(w, paint->width);
					canvas->textOutEllipsed(paint->x, paint->y, paint->width, paint->height, paint->text);
				}

				oldY = paint->y;

				if (paint->len > 6) {
					char http[6];
					const char *text = paint->text;
					if (*text == ' ') text++;
					memcpy(http, text, 5);
					http[5] = NULL;
					if (http[3] == ':') http[4] = NULL;
					
					if ( (strcmpi(http, "HTTP:") == 0) | (strcmpi(http, "FTP:") == 0)) {
						TextClicker *cl = new TextClicker(paint->x, paint->y, paint->width, paint->height, text);
						clickers.addToEnd(cl);
					}
				}

				free(paint->text);
				delete paint;
				paint = lines.getNext(paint);
			}

			y -= (lineCount * textHeight);
		}

		lines.delAll();
		list.delAll();
		word.delAll();

		data = log->getPrev(data);
	}

	totalLines = totalLineCount;
*/


/*
//this is the old parser im working on replacing.
	LPvTextLine data = log->getLast();
	while (data)
	{
		int offsetX = 0;

		char stamps[32];
		if (getTimeStamps())
		{
			sprintf(stamps, "[%02d:%02d:%02d] ", data->hour, data->min, data->sec);
			offsetX = canvas->getTextWidth(stamps);
		}

		int loc = 0;
		int oldLoc = 0;

		while (data->text[loc])
		{
			if (data->text[loc] == ' ')
			{
				memcpy(buffer, data->text+oldLoc, loc-oldLoc);
				buffer[loc-oldLoc] = NULL;
				int txtLen = canvas->getTextWidth(buffer);
				if (txtLen > (width - offsetX))
				{
					for (int newLoc = loc-1; ((newLoc > (oldLoc+1)) & (data->text[newLoc] != ' ')); newLoc--) {}
					buffer[newLoc-oldLoc] = NULL;
					char *tmp = (char*) malloc(strlen(buffer)+1);
					strcpy(tmp, buffer);
					output.addToBegining(tmp);
					oldLoc += (strlen(tmp));
				}
			}

			loc++;
			
			if (loc == data->textLen)
			{
				memcpy(buffer, data->text+oldLoc, loc-oldLoc);
				buffer[loc-oldLoc] = NULL;
				int txtLen = canvas->getTextWidth(buffer);
				if (txtLen > (width - offsetX))
				{
					for (int newLoc = loc-1; ((newLoc > (oldLoc+1)) & (data->text[newLoc] != ' ')); newLoc--) {}
					buffer[newLoc-oldLoc] = NULL;
					char *tmp = (char*) malloc(strlen(buffer)+1);
					strcpy(tmp, buffer);
					output.addToBegining(tmp);
					oldLoc += (strlen(tmp));
				}

				char *tmp = (char*) malloc(data->textLen - oldLoc + 1);
				strcpy(tmp, data->text + oldLoc);
				output.addToBegining(tmp);
			}
		}


		char *outText = output.getFirst();
		while (outText)
		{
		//canvas->textOut(offsetX, crect.bottom-((i++)*textHeight)-1, width-offsetX, textHeight, data->text);
			canvas->textOut(offsetX, crect.bottom-((line++)*textHeight)-1, width-offsetX, textHeight, outText);

			free(outText);
			outText = output.getNext(outText);
		}

		if (getTimeStamps())
		{
			canvas->textOut(0, crect.bottom-((line-1)*textHeight)-1, offsetX, textHeight, stamps);
		}

		output.delAll();
		data = log->getPrev(data);
	}
*/
  return 1;
}





TextClicker::TextClicker(int newX, int newY, int newW, int newH, const char *newText) {
	x = newX;
	y = newY;
	w = newW;
	h = newH;
	setName(newText);
}


BOOL TextClicker::isValidRegion(int testX, int testY) {
	if (testX < x) return FALSE;
	if (testY < y) return FALSE;
	if (testX > x+w) return FALSE;
	if (testY > y+h) return FALSE;
	return TRUE;
}


void TextClicker::activate() {
	const char *text = getName();
	if (text) ShellExecute(NULL, NULL, text, "", NULL, SW_SHOW);
}

//					ShellExecute(NULL, NULL, message, msgEnd, NULL, SW_SHOW);
