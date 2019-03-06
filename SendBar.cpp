#include "precomp.h"
#include "../../bfc/std.h"
#include "../../common/xlatstr.h"
#include "Sendbar.H"



char SendbarXuiObjectStr[] = "wIRC:Sendbar";
char SendbarXuiSvcName[]   = "wIRC:Sendbar XuiObject Service";


Sendbar::Sendbar() {
	sendEdt = NULL;
	sendBtn = NULL;
	btnWidth = 50;
	buttonPos = SDP_FROMRIGHT;
	allowUpdates = FALSE;
}


Sendbar::~Sendbar() {
}


int Sendbar::onInit() {
	setDividerType(DIVIDER_VERTICAL);
	setResizeable(FALSE);
	setMinWidth(10);

	if (btnWidth > 0) {
		sendBtn = new ButtonWnd(_("Send"));
		sendBtn->setNotifyId(1);
		sendBtn->setNotifyWindow(this);
	}
	
	sendEdt = new EditText();
	sendEdt->setNotifyId(1);
	sendEdt->setNotifyWindow(this);

	allowUpdates = TRUE;
	updatePosition();

	return SENDBAR_PARENT::onInit();
}


void Sendbar::updatePosition() {
	if (!allowUpdates) return;
	setDividerPosNoCfg(buttonPos, btnWidth);
	if (buttonPos == SDP_FROMRIGHT) setChildren(sendEdt, sendBtn);
	else setChildren(sendBtn, sendEdt);
}


void Sendbar::sendText() {
	if (sendEdt) {
		char str[256];
		ZeroMemory(str, sizeof(str));
		sendEdt->getBuffer(str, sizeof(str)-1);
		send(str);
		sendEdt->setBuffer("", 250);
	}
}


int Sendbar::setXmlParam(const char *name, const char *strvalue) {
	if (strcmpi(name, "AssignLog") == 0) {
		assignLog(strvalue);
	}

	else if (strcmpi(name, "Send") == 0) {
		sendText();
	}

	else if (strcmpi(name, "ButtonWidth") == 0) {
		int width = -1;
		sscanf(strvalue, "%d", &width);
		if (width == -1) return SENDBAR_PARENT::setXmlParam(name, strvalue);
		btnWidth = width;
		if (!isInited()) return SENDBAR_PARENT::setXmlParam(name, strvalue);
				
		if (width == 0) {
			setChildren(sendEdt);
			delete sendBtn;
			sendBtn = NULL;
		}
		else if (width > 0) {
			if (sendBtn == NULL) {
				sendBtn = new ButtonWnd(_("Send"));
				setChildren(sendEdt, sendBtn);
			}
			updatePosition();
		}
	}

	else if (strcmpi(name, "ButtonPosition") == 0) {
		if (strcmpi(strvalue, "Left") == 0) buttonPos = SDP_FROMLEFT;
		else if (strcmpi(strvalue, "Right") == 0) buttonPos = SDP_FROMRIGHT;
		updatePosition();
	}

	return SENDBAR_PARENT::setXmlParam(name, strvalue);
}


int Sendbar::childNotify(RootWnd *child, int msg, int param1, int param2) {
	if (child == NULL) return SENDBAR_PARENT::childNotify(child, msg, param1, param2);;

	if (child->getNotifyId() == 1) {
		switch (msg) {
			case ChildNotify::BUTTON_LEFTPUSH:
				sendText();
				break;
		}
	}
	return SENDBAR_PARENT::childNotify(child, msg, param1, param2);
}



//-----------------------------------------------------------------------------------------



LRESULT EditText::editWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_KEYDOWN:
			if (Std::keyDown(VK_CONTROL)) {
				switch (wParam) {
				case 'K': insertCharacter(0x03); return 0;	//color
				case 'B': insertCharacter(0x02); return 0;  //bold
				case 'U': insertCharacter(0x1F); return 0;  //underlined
				case 'I': insertCharacter(0x16); return 0;  //italics
				case 'P': insertCharacter(0x11); return 0;  //fixed pitch
				case 'O': insertCharacter(0x12); return 0;  //symbol
				//new line - .r.n  0x10 = .
				//0x09 - tab
				}
			}
      break;
	}
	return EditWnd::editWndProc(hWnd, uMsg, wParam, lParam);
}


void EditText::insertCharacter(char c) {
	char str[2] = { c, 0x00 };
	SendMessage(getEditWnd(), EM_REPLACESEL, TRUE, (LPARAM)str);
}
