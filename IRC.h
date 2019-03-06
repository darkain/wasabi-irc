#ifndef __IRC_H_
#define __IRC_H_

#include "../../studio/wac.h"
#include "../../bfc/timerclient.h"
#include "../../bfc/attribs/attrhandler.h"
#include "../../../vBase/List.H"

#define WACNAME WACGeneric
#define WACPARENT WAComponentClient


typedef struct vtagIrcMessage {
	UINT size;
	UINT msg;
} vIrcMessage, *LPvIrcMessage;


class WACNAME : public WACPARENT, public vList<LPvIrcMessage>, public TimerClientI
{
public:
  WACNAME();
  virtual ~WACNAME();

  virtual GUID getGUID();

  virtual void onCreate();
  virtual void onDestroy();
	
	virtual void timerclient_timerCallback(int id);
	virtual Dependent *timerclient_getDependencyPtr() { return this; }

	virtual void onDelete(LPvIrcMessage data);
private:
	TimerClientI *t;
};


extern WACPARENT *the;

#endif __IRC_H_
