#if 0
#ifndef __SCRIPTOBJECT_H_
#define __SCRIPTOBJECT_H_


#include "../services/svc_scriptobj.h"
#include "../../common/script/scriptobj.h"
#include "../../common/script/objcontroller.h"
#include "../../studio/services/svc_scriptobj.h"

#include "../../common/nsGUID.h"
#include "ChannelLogViewer.H"


class ServerObject;

extern ScriptObjectController *serverController;
extern ScriptObjectController *chatController;
extern ScriptObjectController *channelController;


// -----------------------------------------------------------------------------------------------------
// ScriptObject Provider Service. This class tells the system that we have some ScriptObjectControllers
// to register.
class IrcScriptObjectSvc : public svc_scriptObjectI 
{
public:
  IrcScriptObjectSvc() {};
  virtual ~IrcScriptObjectSvc() {};

  static const char *getServiceName() { return "wIRC Object Service"; }
  virtual ScriptObjectController *getController(int n);
};   


// -----------------------------------------------------------------------------------------------------
// Script classe GUID
// {CCDDCB88-1C97-4e4f-A93D-C5EB05D74EFD} // this is the guid to use in your compiler definitions, change it for each new class!
static const GUID IRC_SCRIPTOBJECT_GUID = 
{ 0xccddcb88, 0x1c97, 0x4e4f, { 0xa9, 0x3d, 0xc5, 0xeb, 0x5, 0xd7, 0x4e, 0xfd } };


// -----------------------------------------------------------------------------------------------------
// ScriptObject Interface. This is your script object, you could attach this class to a preexisting object
// you have, as a seconday inheritance, then fill in a few functions and events. Or you could start from the
// scriptobject and turn it into a wrapper to your own object. 
class ServerObject : public ScriptObjectI
{
public:
	ServerObject();
	virtual ~ServerObject();

	virtual const char *vcpu_getClassName() { return "IrcServer"; }
	virtual ScriptObjectController *vcpu_getController() { return serverController; }

//	virtual void PRIVMSG(const char *channel, const char *message);
//	virtual void onSend(const char *msg);
	virtual void onJoin(const char *name);
	virtual void onPart(const char *name);

//	static scriptVar v_PRIVMSG(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar channel, scriptVar message);
	static scriptVar v_SendRaw(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar data);
//	static scriptVar v_onSend(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar msg);
	static scriptVar v_onJoinChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_onPartChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);

	static scriptVar v_getNickname(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_getServer(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_getServerPort(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
};


// -----------------------------------------------------------------------------------------------------
// This class describes to the system how to handle, create and delete our script class
class ServerScriptObjectController : public ScriptObjectControllerI
{
public:
  ServerScriptObjectController() {} // (don't ask my why, the compiler wanted it.)
  virtual const char *getClassName() { return "IrcServer"; } // has to match the controlled class' vcpu_getClassName() !
  virtual const char *getAncestorClassName() { return "Object"; }  // "Object" to inherit from root, otherwise name of parent class
  virtual ScriptObjectController *getAncestorController() { return NULL; } // NULL if inheriting from root, otherwise, your parent controller class
  virtual int getNumFunctions(); // number of functions and events in table
  virtual const function_descriptor_struct *getExportedFunctions() { return exportedFunction; } // pointer to function table
  virtual GUID getClassGuid() { return IRC_SCRIPTOBJECT_GUID; } // script class guid
  virtual int getInstantiable() { return 1; } // 1 if using '&' in compiler definitions
  virtual int getReferenceable() { return 1; } // usually 1, or 0 if using _predecl in compiler definitions
  virtual ScriptObject *instantiate() { return new ServerObject; } // create your object here
  virtual void destroy(ScriptObject *o) { delete static_cast<ServerObject*>(o); } // delete it here
  virtual void *encapsulate(ScriptObject *o) { return NULL; } // no encapsulation for exampleobject yet
  virtual void deencapsulate(void *o ) {} // no encapsulation for exampleobject yet

private:
  static function_descriptor_struct exportedFunction[];
};


//-----------------------------------------------------------------------------------------------
// Script classe GUID
// {02D41315-D460-409c-A378-5ADEE92274F7}
static const GUID CHAT_SCRIPTOBJECT_GUID = 
{ 0x2d41315, 0xd460, 0x409c, { 0xa3, 0x78, 0x5a, 0xde, 0xe9, 0x22, 0x74, 0xf7 } };


// -----------------------------------------------------------------------------------------------------
// ScriptObject Interface. This is your script object, you could attach this class to a preexisting object
// you have, as a seconday inheritance, then fill in a few functions and events. Or you could start from the
// scriptobject and turn it into a wrapper to your own object. 


class ChatObject : public ScriptObjectI, public vChatViewer
{
public:
	ChatObject();
	virtual ~ChatObject();

	virtual const char *vcpu_getClassName() { return "ChatLog"; }
	virtual ScriptObjectController *vcpu_getController() { return chatController; }

	virtual void onExitChannel();
	virtual void onSetDocked(BOOL dock);
	virtual void onSetStamps(BOOL enabled);
	virtual void onLogCleared();
	virtual void onNewMessage(const char *message, const char *nick=NULL);
	virtual void onSetFormatting(BOOL enabled);

	static scriptVar v_assignLog(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_getChannelName(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onExitChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);

	static scriptVar v_send(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message);
	static scriptVar v_say(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message);
	static scriptVar v_action(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message);
	static scriptVar v_notice(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message);
	static scriptVar v_echo(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message);

	static scriptVar v_setDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar docked);
	static scriptVar v_getDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onSetDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar docked);

	static scriptVar v_getTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_setTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled);
	static scriptVar v_toggleTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onSetTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled);

	static scriptVar v_getFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_setFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled);
	static scriptVar v_onSetFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled);

	static scriptVar v_clearLog(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onLogCleared(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onNewMessage(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message, scriptVar nick);
};


// -----------------------------------------------------------------------------------------------------
// This class describes to the system how to handle, create and delete our script class
class ChatScriptObjectController : public ScriptObjectControllerI
{
public:
  ChatScriptObjectController() {} // (don't ask my why, the compiler wanted it.)
  virtual const char *getClassName() { return "ChatLog"; } // has to match the controlled class' vcpu_getClassName() !
  virtual const char *getAncestorClassName() { return "Object"; }  // "Object" to inherit from root, otherwise name of parent class
  virtual ScriptObjectController *getAncestorController() { return NULL; } // NULL if inheriting from root, otherwise, your parent controller class
  virtual int getNumFunctions(); // number of functions and events in table
  virtual const function_descriptor_struct *getExportedFunctions() { return exportedFunction; } // pointer to function table
  virtual GUID getClassGuid() { return CHAT_SCRIPTOBJECT_GUID; } // script class guid
  virtual int getInstantiable() { return 1; } // 1 if using '&' in compiler definitions
  virtual int getReferenceable() { return 1; } // usually 1, or 0 if using _predecl in compiler definitions
  virtual ScriptObject *instantiate() { return new ChatObject; } // create your object here
  virtual void destroy(ScriptObject *o) { delete static_cast<ChatObject*>(o); } // delete it here
  virtual void *encapsulate(ScriptObject *o) { return NULL; } // no encapsulation for exampleobject yet
  virtual void deencapsulate(void *o ) {} // no encapsulation for exampleobject yet

private:
  static function_descriptor_struct exportedFunction[];
};






//-----------------------------------------------------------------------------------------------
// Script classe GUID
// {8481851B-6AEE-4d08-9DDA-02DA15E8EC67}
static const GUID CHANNEL_SCRIPTOBJECT_GUID = 
{ 0x8481851b, 0x6aee, 0x4d08, { 0x9d, 0xda, 0x2, 0xda, 0x15, 0xe8, 0xec, 0x67 } };


// -----------------------------------------------------------------------------------------------------
// ScriptObject Interface. This is your script object, you could attach this class to a preexisting object
// you have, as a seconday inheritance, then fill in a few functions and events. Or you could start from the
// scriptobject and turn it into a wrapper to your own object. 
class ChannelObject : public vChannelViewer, public ChatObject// : public ScriptObjectI, public vChannelViewer
{
public:
	ChannelObject();
	virtual ~ChannelObject();

	virtual const char *vcpu_getClassName() { return "IrcChannel"; }
	virtual ScriptObjectController *vcpu_getController() { return channelController; }

	virtual void onTopicChange(const char *newTopic, const char *oldTopic);
	virtual void onNewUser(const char *name, BYTE status=0);
	virtual void onUserPart(const char *name);
	virtual void onNickChange(const char *newNick, const char *oldNick);
	virtual void onUserStatusChange(const char *name, BYTE newStatus, BYTE oldStatus);

	static scriptVar v_setTopic(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newTopic);
	static scriptVar v_getTopic(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_onTopicChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newTopic, scriptVar oldTopic);

	static scriptVar v_onNewUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name, scriptVar status);
	static scriptVar v_onUserPart(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_onUserStatusChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name, scriptVar newStatus, scriptVar oldStatus);
	static scriptVar v_getUserCount(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_enumUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar id);
	static scriptVar v_getFirstUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o);
	static scriptVar v_getNextUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_hasUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_onNickChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newNick, scriptVar oldNick);

	static scriptVar v_getUserStatus(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_isUserOwner(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_isUserOp(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_isUserHalfop(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
	static scriptVar v_isUserVoice(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name);
};


// -----------------------------------------------------------------------------------------------------
// This class describes to the system how to handle, create and delete our script class
class ChannelScriptObjectController : public ScriptObjectControllerI
{
public:
  ChannelScriptObjectController() {} // (don't ask my why, the compiler wanted it.)
  virtual const char *getClassName() { return "IrcChannel"; } // has to match the controlled class' vcpu_getClassName() !
  virtual const char *getAncestorClassName() { return "ChatLog"; }  // "Object" to inherit from root, otherwise name of parent class
  virtual ScriptObjectController *getAncestorController() { return NULL; } // NULL if inheriting from root, otherwise, your parent controller class
  virtual int getNumFunctions(); // number of functions and events in table
  virtual const function_descriptor_struct *getExportedFunctions() { return exportedFunction; } // pointer to function table
  virtual GUID getClassGuid() { return CHANNEL_SCRIPTOBJECT_GUID; } // script class guid
  virtual int getInstantiable() { return 1; } // 1 if using '&' in compiler definitions
  virtual int getReferenceable() { return 1; } // usually 1, or 0 if using _predecl in compiler definitions
  virtual ScriptObject *instantiate() { return new ChannelObject; } // create your object here
  virtual void destroy(ScriptObject *o) { delete static_cast<ChannelObject*>(o); } // delete it here
  virtual void *encapsulate(ScriptObject *o) { return NULL; } // no encapsulation for exampleobject yet
  virtual void deencapsulate(void *o ) {} // no encapsulation for exampleobject yet

private:
  static function_descriptor_struct exportedFunction[];
};

#endif//__SCRIPTOBJECT_H_
#endif