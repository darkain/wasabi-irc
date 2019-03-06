#include "precomp.h"
#include "../../bfc/std.h"
#include "Toolbar.H"
#include "ChatLog.H"

extern IrcConnect *irc;


char ToolbarXuiObjectStr[] = "Wasabi:Toolbar";
char ToolbarXuiSvcName[]   = "Wasabi:Toolbar XuiObject Service";


// {B978EB90-7C8F-497d-860E-DE5D486200BD}
static const GUID guid_toolbar = 
{ 0xb978eb90, 0x7c8f, 0x497d, { 0x86, 0xe, 0xde, 0x5d, 0x48, 0x62, 0x0, 0xbd } };



Toolbar::Toolbar() {
	buttonAlign = 0;
	buttonVertAlign = 0;
//	buttonTextAlign = BUTTONJUSTIFY_CENTER;
	alignment = TEXTALIGN_CENTER;
	buttonWidth = 20;
	buttonHeight = 20;
	buttonSpacing = 0;
	bg = NULL;
	deleting = FALSE;
}


Toolbar::~Toolbar() {
	deleting = TRUE;
	ToolbarBtn *btn = buttons.getFirst();
	while (btn) {
		deleteChild(btn);
		delete btn;
		btn = buttons.getNext(btn);
	}
}


void Toolbar::addButton(ToolbarBtn *button) {
	ASSERT(button != NULL);

	if (isInited()) {
		addChild(button, 0, 0, 0, 0);
		if (!button->isInited()) button->init(this);
	}
//	button->setTextJustification(buttonTextAlign);
	button->setTextAlign(alignment);
	buttons.addToEnd(static_cast<ToolbarBtn*>(button));
	onResize();
}


void Toolbar::instertButton(ToolbarBtn *button, ToolbarBtn *insertAfter) {
	ASSERT(button != NULL);

	if (isInited()) {
		addChild(button, 0, 0, 0, 0);
		if (!button->isInited())+ button->init(this);
	}
//	button->setTextJustification(buttonTextAlign);
	button->setTextAlign(alignment);
	buttons.addAfter(static_cast<ToolbarBtn*>(button), static_cast<ToolbarBtn*>(insertAfter));
	onResize();
}


void Toolbar::deleteButton(ToolbarBtn *button) {
	ASSERT(button != NULL);

	if (deleting) return;
	buttons.delItem(static_cast<ToolbarBtn*>(button));
	deleteChild(button);
	onResize();
}


int Toolbar::onResize() {
	if ( (!isInited()) | (deleting) ) return 0;
	int retval = ContWnd::onResize();

	RECT rect;
	getClientRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if (bg) bg->resize(&rect);

	int i=0;
	ToolbarBtn *btn = buttons.getFirst();
	while (btn) {
		int spacing = 0;
		if (i > 0) spacing = buttonSpacing;
		btn->resize((buttonWidth*i++)+spacing, 0, buttonWidth, buttonHeight);
		btn = buttons.getNext(btn);
	}

	return retval;
}


int Toolbar::onInit() {
	int retval = ContWnd::onInit();

	bg = new ButtonWnd();
	addChild(bg, 0, 0, 0, 0);
	bg->enableButton(FALSE);
	bg->init(this);

	ToolbarBtn *btn = buttons.getFirst();
	while (btn) {
		addChild(btn, 0, 0, 0, 0);
		btn->init(this);
		btn = buttons.getNext(btn);
	}

	return retval;
}


int Toolbar::setXmlParam(const char *name, const char *strvalue) {
	if (strcmpi(name, "ButtonAlign") == 0) {
		if (strcmpi(strvalue, "left") == 0) buttonAlign = 0;
		else if (strcmpi(strvalue, "right") == 0) buttonAlign = 1;
		else if (strcmpi(strvalue, "center") == 0) buttonAlign = 2;
		onResize();
	}

	else if (strcmpi(name, "ButtonVertAlign") == 0) {
		if (strcmpi(strvalue, "top")) buttonVertAlign = 0;
		else if (strcmpi(strvalue, "middle")) buttonVertAlign = 1;
		else if (strcmpi(strvalue, "bottom")) buttonVertAlign = 2;
		onResize();
	}

	else if (strcmpi(name, "ButtonTextAlign") == 0) {
		if (strcmpi(strvalue, "center") == 0) alignment = TEXTALIGN_CENTER;
		else if (strcmpi(strvalue, "left") == 0) alignment = TEXTALIGN_LEFT;
		else if (strcmpi(strvalue, "right") == 0) alignment = TEXTALIGN_RIGHT;
		ToolbarBtn *btn = buttons.getFirst();
		while (btn) {
//			btn->setTextJustification(buttonTextAlign);
			btn->setTextAlign(alignment);
			btn = buttons.getNext(btn);
		}
	}

	else  if (strcmpi(name, "ButtonWidth") == 0) {
		int width = -1;
		sscanf(strvalue, "%d", &width);
		if (width > 0) {
			buttonWidth = width;
			onResize();
		}
	}
	
	else  if (strcmpi(name, "ButtonHeight") == 0) {
		int height = -1;
		sscanf(strvalue, "%d", &height);
		if (height > 0) {
			buttonHeight = height;
			onResize();
		}
	}
	
	else  if (strcmpi(name, "ButtonSpacing") == 0) {
		int spacing = -1;
		sscanf(strvalue, "%d", &spacing);
		if (spacing > -1) {
			buttonSpacing = spacing;
			onResize();
		}
	}
	
	return ContWnd::setXmlParam(name, strvalue);
}


//---------------------------------------------------------

ToolbarBtn::ToolbarBtn(Toolbar *bar, const char *button_text, int notifyId) : ButtonWnd(button_text) {
	ASSERT(bar != NULL);

	setNotifyId(notifyId);
	toolbar = bar;

	toolbar->addButton(this);
}


ToolbarBtn::~ToolbarBtn() {
	toolbar->deleteButton(this);
}


//---------------------------------------------------------


char ChannelbarXuiObjectStr[] = "wIRC:Channelbar";
char ChannelbarXuiSvcName[]   = "wIRC:Channelbar XuiObject Service";


ManagerTop::ManagerTop() {
}


ManagerTop::~ManagerTop() {
	irc->deregisterLogViewer(this);
}


int ManagerTop::onInit() {
	vChatLog *log = irc->getFirstLog();
	while (log) {
		onAddChat(log);
		log = irc->getNextLog(log);
	}
	irc->registerLogViewer(this);
	return Toolbar::onInit();
}


void ManagerTop::onAddChat(vChatLog *data) {
	char str[256] = " ";
	lstrcat(str, data->getName());

	ButtonMenu *btn = new ButtonMenu(this, str);
//	btn->setTextJustification(BUTTONJUSTIFY_LEFT);
	btn->assignLog(data);
}



//-------------------------------------------------------------------------------------



ButtonMenu::ButtonMenu(Toolbar *bar, const char *button_text) : ToolbarBtn(bar, button_text) {
	flash = FALSE;
	flashCount = 0;
}


void ButtonMenu::onLogDelete() {
	delete this;
}


void ButtonMenu::onLeftPush(int x, int y) {
	vChatViewer::activate();
	ButtonWnd::onLeftPush(x, y);
}


int ButtonMenu::onInit() {
	if (log) { setPushed(getActivated()); }
	return ButtonWnd::onInit();
}


void ButtonMenu::onSetDocked(BOOL dock) {
	enableButton(dock);
}


void ButtonMenu::onSetFlashing(BOOL enabled) {
	if (enabled == FALSE) {
		flash = FALSE;
		flashCount = 0;
		timerclient_killTimer(1);
		setColors();
		invalidate();
	}
}


void ButtonMenu::onActivated() {
	setPushed(TRUE);
	onSetFlashing(FALSE);
}


void ButtonMenu::onDeactivated() {
	setPushed(FALSE);
}


void ButtonMenu::onNewMessage(const char *message, const char *nick) {
	if ( (getFlashing()) & (!getActivated()) & (getDocked()) & (flashCount==0) ) {
		flash = FALSE;
		flashCount = 0;
		timerclient_setTimer(1, 500);
		timerclient_timerCallback(1);
	}
}


void ButtonMenu::timerclient_timerCallback(int id) {
	flash = !flash;
	if (flash) {
		setColors("studio.button.hiliteText", "studio.button.hiliteText", "studio.button.hiliteText");
		flashCount++;
		if (flashCount == 5) timerclient_killTimer(1);
	} else setColors();
	invalidate();
	ToolbarBtn::timerclient_timerCallback(id);
}
