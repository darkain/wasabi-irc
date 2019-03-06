#ifndef __ABOUT_H_
#define __ABOUT_H_


#include "../../common/contwnd.h"

#define ABOUTWND_PARENT ContWnd


class AboutWnd : public ABOUTWND_PARENT {
public:
	AboutWnd();

	static const char *getWindowTypeName();
	static GUID getWindowTypeGuid();
//  static void setIconBitmaps(ButtonWnd *button);
};


#endif//__ABOUT_H_