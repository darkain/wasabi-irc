#if 0
#ifndef __WNDMANAGER_H_
#define __WNDMANAGER_H_

//#include "../../common/framewnd.h"
#include "../../common/contwnd.h"
#include "../../bfc/wnds/buttwnd.h"
//#include "../../bfc/appcmds.h"
//#include "../../bfc/notifmsg.h"
//#include "../../bfc/timerclient.h"
//#include "../../../../vBase/List.H"
//#include "IrcClient.H"
//#include "ChatLog.H"
//#include "ChatLogViewer.H"
//#include "Toolbar.H"


#define WNDMANAGER_PARENT ContWnd
class WndManager : public WNDMANAGER_PARENT {
public:
	WndManager();
	~WndManager();

  virtual int onInit();
	virtual int onResize();

	static const char *getWindowTypeName();
	static GUID getWindowTypeGuid();
  static void setIconBitmaps(ButtonWnd *button);    
/*
	virtual void appcmds_onCommand(int id, const RECT *buttonRect);
	void AddCommand(const char *name, int _id, int _side=0)
	{ CmdRec *c = new CmdRec(name, _id, _side, 1); appcmds_addCmd(c); }
*/
private:
//	ManagerTop *top;
};


#endif __WNDMANAGER_H_
#endif