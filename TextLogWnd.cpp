#include "precomp.h"
#include "../../bfc/std.h"
#include "../../bfc/canvas.h"
#include "../../bfc/notifmsg.h"
#include "TextLogWnd.H"
#include "TextLog.H"
#include <shellapi.h>
#include <time.h>


TextLogWnd::TextLogWnd(TextLog *log) {
	TextLogWnd(FALSE);
	textlog = log;
}


TextLogWnd::TextLogWnd(BOOL createLog) {
	vScroll = NULL;
//	totalHeight = 0;
	textlog = NULL;
	linecount = 0;
	marginl = marginr = 0;
	margint = marginb = 0;
	marginw = -1;

	delLog = createLog;
	setVirtual(FALSE);

	if (createLog) {
		textlog = new TextLog();
	}
}


TextLogWnd::~TextLogWnd() {
	if (delLog) {
		delete textlog;
		textlog = NULL;
	}
}


int TextLogWnd::onInit() {
	int retval = TEXTLOGWND_PARENT::onInit();

	vScroll = new ScrollBar();

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
	vScroll->setPosition(SCROLLBAR_FULL);
	vScroll->setUpDownValue(SCROLLBAR_FULL);

 	addChild(vScroll, 0, 0, 0, 0);
	vScroll->init(this);

	return retval;
}


int TextLogWnd::onResize() {
	int retval = TEXTLOGWND_PARENT::onResize();
	if (vScroll == NULL) return retval;

  RECT crect = clientRect();
	int width = crect.right - crect.left;
	int height = crect.bottom - crect.top;
	int swidth = vScroll->getWidth();
	vScroll->resize(width-swidth, 0, swidth, height);

	return retval;
}


int TextLogWnd::onLeftButtonDown(int x, int y) {
	TextClickRegion *rgn = clickables.getFirst();
	while (rgn) {
		if (rgn->testRegion(x, y)) {
			rgn->activateRegion();
			return 0;
		}
		rgn = clickables.getNext(rgn);
	}
	return TEXTLOGWND_PARENT::onLeftButtonDown(x, y);
}


int TextLogWnd::onRightButtonDown(int x, int y) {
	popMenu();
	return 0;
}


int TextLogWnd::onRightButtonUp(int x, int y) {
	return 0;
}


int TextLogWnd::setXmlParam(const char *name, const char *strvalue) {
	if (strcmpi(name, "MARGINL") == 0) {
		int margin = -1;
		if (sscanf(strvalue, "%d", margin) < 1) margin = -1;
		setMarginLeft(margin);
	}

	else if (strcmpi(name, "MARGINR") == 0) {
		int margin = -1;
		if (sscanf(strvalue, "%d", margin) < 1) margin = -1;
		setMarginRight(margin);
	}

	else if (strcmpi(name, "MARGINT") == 0) {
		int margin = -1;
		if (sscanf(strvalue, "%d", margin) < 1) margin = -1;
		setMarginTop(margin);
	}

	else if (strcmpi(name, "MARGINB") == 0) {
		int margin = -1;
		if (sscanf(strvalue, "%d", margin) < 1) margin = -1;
		setMarginBottom(margin);
	}

	else if (strcmpi(name, "MARGINW") == 0) {
		int margin = -1;
		if (sscanf(strvalue, "%d", margin) < 1) margin = -1;
		setMarginWrap(margin);
	}

	return TEXTLOGWND_PARENT::setXmlParam(name, strvalue);
}


void TextLogWnd::setMargins(int MarginL, int MarginR, int MarginT, int MarginB, int MarginW) {
	BOOL needUpdate = FALSE;

	if ( (MarginL != -1) & (MarginL != marginl) ) {
		needUpdate = TRUE;
		marginl = MarginL;
	}

	if ( (MarginR != -1) & (MarginR != marginr) ) {
		needUpdate = TRUE;
		marginr = MarginR;
	}

	if ( (MarginT != -1) & (MarginT != margint) ) {
		needUpdate = TRUE;
		margint = MarginT;
	}

	if ( (MarginB != -1) & (MarginB != marginb) ) {
		needUpdate = TRUE;
		marginb = MarginB;
	}

	if (MarginW != marginw) {
		needUpdate = TRUE;
		marginw = MarginW;
	}

	if ( (isInited()) & (needUpdate) ) invalidate();
}


void TextLogWnd::setMarginLeft(int margin) {
	if (margin == -1) return;
	if (marginl == margin) return;
	marginl = margin;
	if (isInited()) invalidate();
}


void TextLogWnd::setMarginRight(int margin) {
	if (margin == -1) return;
	if (marginr == margin) return;
	marginr = margin;
	if (isInited()) invalidate();
}


void TextLogWnd::setMarginTop(int margin) {
	if (margin == -1) return;
	if (margint == margin) return;
	margint = margin;
	if (isInited()) invalidate();
}


void TextLogWnd::setMarginBottom(int margin) {
	if (margin == -1) return;
	if (marginb == margin) return;
	marginb = margin;
	if (isInited()) invalidate();
}


void TextLogWnd::setMarginWrap(int margin) {
	if (marginw == margin) return;
	marginw = margin;
	if (isInited()) invalidate();
}


int TextLogWnd::childNotify(RootWnd *child, int msg, int param1, int param2) {
	if (child == NULL) return TEXTLOGWND_PARENT::childNotify(child, msg, param1, param2);

	if (child->getNotifyId() == 11) {
		if (msg == ChildNotify::SCROLLBAR_SETPOSITION) {
			int sl = linescroll;
			if (sl != getScrollLine()) {
				invalidate();
			}
		}
	}
	return TEXTLOGWND_PARENT::childNotify(child, msg, param1, param2);
}


const char *TextLogWnd::renderTimestamp(time_t time) {
	struct tm *tme = localtime(&time);
	sprintf(tsbuffer, "[%02d:%02d:%02d] ", tme->tm_hour, tme->tm_min, tme->tm_sec);
	return tsbuffer;
}


int TextLogWnd::getScrollLine() {
	int sl = 0;
	if (vScroll) {
		int lineCount = getLineCount(FALSE);
		if (lineCount < 1) lineCount = 1;
		lineCount--;
		int pos = (-vScroll->getPosition()+65536);
		sl = (pos*lineCount/65536);
	}
	linescroll = sl;
	return sl;
}


int TextLogWnd::getLineCount(BOOL recalc) {
	if (!recalc) return linecount;
	if (textlog == NULL) return 0;

  RECT crect = clientRect();
	int width = crect.right - crect.left;
	if (vScroll) width -= vScroll->getWidth();
	int x=0;
	int count = 0;

	LPLogWord textword = textlog->getFirst();
	while (textword) {
		switch (textword->type) {
		case CRLF:
		case HR:
			count++;
			x = 0;
			break;

		case TIMESTAMP:
			if (!getAssignedTextLog()->getShowTimestamp()) break;  //gotta make this call a bit easier
//			if (!getStamps()) break;
			//dont break if enabled, cause we wanna calculate teh width
			//for now tho, we asume they are enabled until option is available

		case TEXT:
		case URL:
			if ( (textword->width + x) > width ) {
				x = 0;
				count++;
			}
			//no break, we wanna add the width.  :)

		case SPACE:
			x += textword->width;
			break;
		}
		textword = textlog->getNext(textword);
	}
	
	if (linecount != count) {
		linecount = count;
//		if (count < 1) count = 1;
//		vScroll->setUpDownValue(SCROLLBAR_FULL/count);
	}
	return count;
}


int TextLogWnd::onPaint(Canvas *canvas) {
	clickables.delAll();

  PaintBltCanvas pbc;
  if (canvas == NULL) {
    pbc.beginPaint(this);
    canvas = &pbc;
  }

  TEXTLOGWND_PARENT::onPaint(canvas);

  RECT crect = clientRect();
	crect.left += marginl;
	crect.right -= marginr;
	crect.top += margint;
	crect.bottom -= marginb;

	int wrapx = 0;
	if (marginw == -1) wrapx = marginl;
	else wrapx = marginw;

	if (textlog == NULL) {
		canvas->textOutCentered(&crect, "Invalid LOG Specified");
		return 1;
	}

  canvas->setTextColor(RGB(255,255,255));
	canvas->setTextFont("Lucida Console");
  canvas->setTextSize(14);

	int width = crect.right - crect.left;
	int height = crect.bottom - crect.top;
	if (vScroll) width -= vScroll->getWidth();

	if ( (width < 1) | (height < 1) ) return 1;

	int textHeight = canvas->getTextHeight();

	LPLogWord textword = textlog->getFirst();
	while (textword) {
		switch (textword->type) {
			case TEXT:
			case URL:
				if (textword->width == UNKNOWN_SIZE) textword->width = canvas->getTextWidth(textword->data.text);
				if (textword->height == UNKNOWN_SIZE) textword->height = canvas->getTextHeight(textword->data.text);
				break;

			case SPACE:
				if (textword->width == UNKNOWN_SIZE) textword->width = (canvas->getTextWidth(" ") * textword->len) >> 1;
				if (textword->height == UNKNOWN_SIZE) textword->height = (canvas->getTextHeight(" ") * textword->len);
				break;

			case TIMESTAMP: {
					const char *ts = renderTimestamp(textword->data.time);
					if (textword->width == UNKNOWN_SIZE) textword->width = canvas->getTextWidth(ts);
					if (textword->height == UNKNOWN_SIZE) textword->height = canvas->getTextHeight(ts);
				} break;

			case CRLF:
			case HR:
				canvas->setTextBold(FALSE);
				canvas->setTextItalic(FALSE);
				canvas->setTextUnderline(FALSE);
				canvas->setTextColor(RGB(255,255,255));
				break;

			case SETBOLD: canvas->setTextBold(textword->data.boolean); break;
			case DEFAULTBOLD: canvas->setTextBold(FALSE); break;
			case PUSHBOLD: canvas->pushTextBold(textword->data.boolean); break;
			case POPBOLD: canvas->popTextBold(); break;

			case SETITALIC: canvas->setTextItalic(textword->data.boolean); break;
			case DEFAULTITALIC: canvas->setTextItalic(FALSE); break;
			case PUSHITALIC: canvas->pushTextItalic(textword->data.boolean); break;
			case POPITALIC: canvas->popTextItalic(); break;

			case SETUNDERLINE: canvas->setTextUnderline(textword->data.boolean); break;
			case DEFAULTUNDERLINE: canvas->setTextUnderline(FALSE); break;
			case PUSHUNDERLINE: canvas->pushTextUnderline(textword->data.boolean); break;
			case POPUNDERLINE: canvas->popTextUnderline(); break;
		}
		textword = textlog->getNext(textword);
	}


	int lines = getLineCount(TRUE);


	canvas->setTextBold(FALSE);
	canvas->setTextItalic(FALSE);
	canvas->setTextUnderline(FALSE);
	canvas->setTextColor(RGB(255,255,255));


	int x = marginl;
	int y = height - textHeight - marginb;
//	totalHeight = 0;

	WordCoordList linelist;
	WordCoordList wordlist;
	WordCoordList outlist(true);

	int lineNum = getScrollLine();
	int currentLine = 0;

	textword = textlog->getLast();
	while (textword) {
		if (y+textHeight < margint) break;

		if ( (textword->type == CRLF) | (textword->type == HR) ) {
			int outlines = 1;

			if (textword->type == HR) {
				int drawy = y + (textHeight/2);
				canvas->lineDraw(10, drawy, width-10, drawy);
				y -= textHeight;
			}

			WordCoord *logword = linelist.getFirst();
			while (logword) {
				switch (logword->word->type) {
				case TIMESTAMP:
					if (!getAssignedTextLog()->getShowTimestamp()) break;  //gotta make this call a bit easier

				case TEXT:
				case URL:
				case SPACE:
					if ( (logword->word->width + x) > width ) {
						WordCoord *wrd = outlist.getFirst();
						while (wrd) {
							wrd->line++;
							if (wrd->line > lineNum) {
								wrd->y -= textHeight;
							}
							wrd = outlist.getNext(wrd);
						}
						x = wrapx;
						outlines++;
					}
					logword->x = x;
					logword->y = y;
					logword->line = currentLine;
					x += logword->word->width;
					break;

				default:
					logword->line = currentLine;
					logword->x = x;
					logword->y = y;
				}
				outlist.addToEnd(logword);
				logword = linelist.getNext(logword);
			}

			canvas->setTextColor(RGB(255,255,255));
			canvas->setTextOpaque(FALSE);
			canvas->setTextBold(FALSE);
			canvas->setTextItalic(FALSE);
			canvas->setTextUnderline(FALSE);

			int totalLinesPainted = 0;
			int lastLinePainted = -100;

			logword = outlist.getFirst();
			while (logword) {
				if (logword->line >= lineNum) {
					if (lastLinePainted != logword->line) {
						totalLinesPainted++;
						lastLinePainted = logword->line;
					}

					switch (logword->word->type) {
						case URL:
							clickables.addToEnd(new UrlClickRegion(logword->x, logword->y,
								logword->word->width, logword->word->height, logword->word));
							//do NOT break, because we wanna paint the text too.  :)

						case TEXT:
							canvas->textOut(logword->x, logword->y, logword->word->data.text);
							break;

						case TIMESTAMP: {
								const char *ts = renderTimestamp(logword->word->data.time);
								canvas->textOut(logword->x, logword->y, ts);
							} break;

						case SETBGCOLOR:
							if (logword->word->data.colour == -1) {
								canvas->setTextOpaque(FALSE);
							} else {
								canvas->setTextOpaque(TRUE);
								canvas->setTextBkColor(logword->word->data.colour);
							}
							break;

						case SETCOLOR: canvas->setTextColor(logword->word->data.colour); break;
						case DEFAULTCOLOR: canvas->setTextColor(RGB(255,255,255)); break;  //needsta make read skin
						case PUSHCOLOR: canvas->pushTextColor(logword->word->data.colour); break;
						case POPCOLOR: canvas->popTextColor(); break;

						case SETBOLD: canvas->setTextBold(logword->word->data.boolean); break;
						case DEFAULTBOLD: canvas->setTextBold(FALSE); break;
						case PUSHBOLD: canvas->pushTextBold(logword->word->data.boolean); break;
						case POPBOLD: canvas->popTextBold(); break;

						case SETITALIC: canvas->setTextItalic(logword->word->data.boolean); break;
						case DEFAULTITALIC: canvas->setTextItalic(FALSE); break;
						case PUSHITALIC: canvas->pushTextItalic(logword->word->data.boolean); break;
						case POPITALIC: canvas->popTextItalic(); break;

						case SETUNDERLINE: canvas->setTextUnderline(logword->word->data.boolean); break;
						case DEFAULTUNDERLINE: canvas->setTextUnderline(FALSE); break;
						case PUSHUNDERLINE: canvas->pushTextUnderline(logword->word->data.boolean); break;
						case POPUNDERLINE: canvas->popTextUnderline(); break;
					}
				}
				logword = outlist.getNext(logword);
			}


			linelist.delAll();
			wordlist.delAll();
			outlist.delAll();
			y -= (textHeight * totalLinesPainted);
			x = marginl;
			currentLine += outlines;
		}

		else {
			linelist.addWord(textword, 0, y, 1);
		}

		textword = textlog->getPrev(textword);
	}

	return 1;
}



//---------------------------------------------------------




void UrlClickRegion::activateRegion() {
	ShellExecute(NULL, NULL, logword->data.text, "", NULL, SW_SHOW);
}
