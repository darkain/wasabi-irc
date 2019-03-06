#if 0
#include "precomp.h"
#include "../../bfc/std.h"

#include "ScriptObject.h"
#include "IrcClient.H"


extern IrcConnect *irc;

static ServerScriptObjectController _serverController;
ScriptObjectController *serverController = &_serverController;

static ChatScriptObjectController _chatController;
ScriptObjectController *chatController = &_chatController;


static ChannelScriptObjectController _channelController;
ScriptObjectController *channelController = &_channelController;


// -----------------------------------------------------------------------------------------------------
// Service
ScriptObjectController *IrcScriptObjectSvc::getController(int n) {
	switch (n)
	{
	case 0: return serverController;
	case 1: return chatController;
	case 2: return channelController;
	// add more controllers here
	}
  return NULL;
}


ServerObject::ServerObject() {
	if (irc) irc->registerObject(this);
}

ServerObject::~ServerObject() {
	if (irc) irc->deregisterObject(this);
}


void ServerObject::onJoin(const char *name) {
  v_onJoinChannel(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(name));
}


void ServerObject::onPart(const char *name) {
  v_onPartChannel(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(name));
}


scriptVar ServerObject::v_SendRaw(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar data) {
  SCRIPT_FUNCTION_INIT
	if (irc) irc->Send((char*)GET_SCRIPT_STRING(data));
	return MAKE_SCRIPT_VOID();
}


scriptVar ServerObject::v_onJoinChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, serverController, name); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, name);
}


scriptVar ServerObject::v_onPartChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, serverController, name); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, name);
}


scriptVar ServerObject::v_getNickname(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING(irc->getNick());
}


scriptVar ServerObject::v_getServer(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING(irc->getServer());
}


scriptVar ServerObject::v_getServerPort(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_INT(irc->getServerPort());
}


function_descriptor_struct ServerScriptObjectController::exportedFunction[] = {
	{"SendRaw", 1, ServerObject::v_SendRaw},
	{"onJoinChannel", 1, ServerObject::v_onJoinChannel},
	{"onPartChannel", 1, ServerObject::v_onPartChannel},
	{"getNickname", 0, ServerObject::v_getNickname},
	{"getServer", 0, ServerObject::v_getServer},
	{"getServerPort", 0, ServerObject::v_getServerPort},
};


ServerScriptObjectController::getNumFunctions() {
  return sizeof(exportedFunction) / sizeof(function_descriptor_struct); 
}


//---------------------------------------------------------------------------------------------
ChatObject::ChatObject() {
}


ChatObject::~ChatObject() {
}


void ChatObject::onExitChannel() {
  v_onExitChannel(SCRIPT_CALL, this);
}

void ChatObject::onSetStamps(BOOL enabled) {
  v_onSetTimeStamps(SCRIPT_CALL, this, MAKE_SCRIPT_BOOLEAN(enabled));
}

void ChatObject::onSetFormatting(BOOL enabled) {
  v_onSetTimeStamps(SCRIPT_CALL, this, MAKE_SCRIPT_BOOLEAN(enabled));
}

void ChatObject::onLogCleared() {
  v_onLogCleared(SCRIPT_CALL, this);
}

void ChatObject::onNewMessage(const char *message, const char *nick) {
  v_onNewMessage(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(message), MAKE_SCRIPT_STRING(nick));
}

void ChatObject::onSetDocked(BOOL dock) {
  v_onSetDocked(SCRIPT_CALL, this, MAKE_SCRIPT_BOOLEAN(dock));
}


scriptVar ChatObject::v_say(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->say(GET_SCRIPT_STRING(message));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_action(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->action(GET_SCRIPT_STRING(message));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_send(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->send(GET_SCRIPT_STRING(message));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_notice(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->notice(GET_SCRIPT_STRING(message));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_echo(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->echo(GET_SCRIPT_STRING(message));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_assignLog(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->assignLog(irc->getLog(GET_SCRIPT_STRING(name)));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_getChannelName(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING((static_cast<ChatObject*>(o))->getName());
}


scriptVar ChatObject::v_onExitChannel(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS0(o, channelController); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT0(o);
}


scriptVar ChatObject::v_setDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar docked) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->setDocked(GET_SCRIPT_BOOLEAN(docked));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_getDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChatObject*>(o))->getDocked());
}


scriptVar ChatObject::v_onSetDocked(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar docked) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, channelController, docked); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, docked);
}


scriptVar ChatObject::v_getTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChatObject*>(o))->getTimeStamps());
}


scriptVar ChatObject::v_setTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->setTimeStamps(GET_SCRIPT_BOOLEAN(enabled));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_toggleTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->toggleTimeStamps();
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_onSetTimeStamps(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, channelController, enabled); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, enabled);
}


scriptVar ChatObject::v_getFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChatObject*>(o))->getFormatting());
}


scriptVar ChatObject::v_setFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->setFormatting(GET_SCRIPT_BOOLEAN(enabled));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_onSetFormatting(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar enabled) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, channelController, enabled); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, enabled);
}


scriptVar ChatObject::v_clearLog(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChatObject*>(o))->clearLog();
	return MAKE_SCRIPT_VOID();
}


scriptVar ChatObject::v_onLogCleared(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS0(o, channelController); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT0(o);
}


scriptVar ChatObject::v_onNewMessage(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar message, scriptVar nick) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS2(o, channelController, message, nick); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT2(o, message, nick);
}


function_descriptor_struct ChatScriptObjectController::exportedFunction[] = {
	{"say", 1, ChatObject::v_say},
	{"action", 1, ChatObject::v_action},
	{"send", 1, ChatObject::v_send},
	{"notice", 1, ChatObject::v_notice},
	{"echo", 1, ChatObject::v_echo},
	{"assignLog", 1, ChatObject::v_assignLog},
	{"getChannelName", 0, ChatObject::v_getChannelName},
	{"onExitChannel", 0, ChatObject::v_onExitChannel},
	{"setDocked", 1, ChatObject::v_setDocked},
	{"getDocked", 0, ChatObject::v_getDocked},
	{"onSetDocked", 1, ChatObject::v_onSetDocked},
	{"getTimeStamps", 0, ChatObject::v_getTimeStamps},
	{"setTimeStamps", 1, ChatObject::v_setTimeStamps},
	{"toggleTimeStamps", 0, ChatObject::v_toggleTimeStamps},
	{"onSetTimeStamps", 1, ChatObject::v_onSetTimeStamps},
	{"getFormatting", 0, ChatObject::v_getFormatting},
	{"setFormatting", 1, ChatObject::v_setFormatting},
	{"onSetFormatting", 1, ChatObject::v_onSetFormatting},
	{"clearLog", 0, ChatObject::v_clearLog},
	{"onLogCleared", 0, ChatObject::v_onLogCleared},
	{"onNewMessage", 2, ChatObject::v_onNewMessage},
};


ChatScriptObjectController::getNumFunctions() {
  return sizeof(exportedFunction) / sizeof(function_descriptor_struct); 
}

//---------------------------------------------------------------------------------------------
ChannelObject::ChannelObject() {
}


ChannelObject::~ChannelObject() {
}


void ChannelObject::onTopicChange(const char *newTopic, const char *oldTopic) {
  v_onTopicChange(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(newTopic), MAKE_SCRIPT_STRING(oldTopic));
}

void ChannelObject::onNewUser(const char *name, BYTE status) {
  v_onNewUser(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(name), MAKE_SCRIPT_INT(status));
}

void ChannelObject::onUserPart(const char *name) {
  v_onUserPart(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(name));
}

void ChannelObject::onNickChange(const char *newNick, const char *oldNick) {
  v_onNickChange(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(newNick), MAKE_SCRIPT_STRING(oldNick));
}

void ChannelObject::onUserStatusChange(const char *name, BYTE newStatus, BYTE oldStatus) {
  v_onUserStatusChange(SCRIPT_CALL, this, MAKE_SCRIPT_STRING(name), MAKE_SCRIPT_INT(newStatus), MAKE_SCRIPT_INT(oldStatus));
}


scriptVar ChannelObject::v_setTopic(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newTopic) {
  SCRIPT_FUNCTION_INIT
	(static_cast<ChannelObject*>(o))->setTopic(GET_SCRIPT_STRING(newTopic));
	return MAKE_SCRIPT_VOID();
}


scriptVar ChannelObject::v_getTopic(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING((static_cast<ChannelObject*>(o))->getTopic());
}


scriptVar ChannelObject::v_onTopicChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newTopic, scriptVar oldTopic) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS2(o, channelController, newTopic, oldTopic); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT2(o, newTopic, oldTopic);
}


scriptVar ChannelObject::v_onNewUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name, scriptVar status) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS2(o, channelController, name, status); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT2(o, name, status);
}


scriptVar ChannelObject::v_onUserPart(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS1(o, channelController, name); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT1(o, name);
}


scriptVar ChannelObject::v_onNickChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar newNick, scriptVar oldNick) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS2(o, channelController, newNick, oldNick); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT2(o, newNick, oldNick);
}


scriptVar ChannelObject::v_onUserStatusChange(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name, scriptVar newStatus, scriptVar oldStatus) {
  SCRIPT_FUNCTION_INIT  
  PROCESS_HOOKS3(o, channelController, name, oldStatus, newStatus); 
  SCRIPT_FUNCTION_CHECKABORTEVENT;
  SCRIPT_EXEC_EVENT3(o, name, oldStatus, newStatus);
}


scriptVar ChannelObject::v_getUserCount(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_INT((static_cast<ChannelObject*>(o))->getUserCount());
}


scriptVar ChannelObject::v_enumUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar id) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING((static_cast<ChannelObject*>(o))->enumUser(GET_SCRIPT_INT(id)));
}


scriptVar ChannelObject::v_getFirstUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING((static_cast<ChannelObject*>(o))->getFirstUser());
}


scriptVar ChannelObject::v_getNextUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_STRING((static_cast<ChannelObject*>(o))->getNextUser(GET_SCRIPT_STRING(name)));
}


scriptVar ChannelObject::v_hasUser(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChannelObject*>(o))->hasUser(GET_SCRIPT_STRING(name)));
}


scriptVar ChannelObject::v_getUserStatus(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_INT((static_cast<ChannelObject*>(o))->getUserStatus(GET_SCRIPT_STRING(name)));
}


scriptVar ChannelObject::v_isUserOwner(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChannelObject*>(o))->getUserStatus(GET_SCRIPT_STRING(name)) && 0x02);
}


scriptVar ChannelObject::v_isUserOp(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChannelObject*>(o))->getUserStatus(GET_SCRIPT_STRING(name)) && 0x08);
}


scriptVar ChannelObject::v_isUserHalfop(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChannelObject*>(o))->getUserStatus(GET_SCRIPT_STRING(name)) && 0x01);
}


scriptVar ChannelObject::v_isUserVoice(SCRIPT_FUNCTION_PARAMS, ScriptObject *o, scriptVar name) {
  SCRIPT_FUNCTION_INIT
	return MAKE_SCRIPT_BOOLEAN((static_cast<ChannelObject*>(o))->getUserStatus(GET_SCRIPT_STRING(name)) && 0x04);
}


function_descriptor_struct ChannelScriptObjectController::exportedFunction[] = {
	{"setTopic", 1, ChannelObject::v_setTopic},
	{"getTopic", 0, ChannelObject::v_getTopic},
	{"onTopicChanged", 2, ChannelObject::v_onTopicChange},
	{"onUserJoin", 2, ChannelObject::v_onNewUser},
	{"onUserPart", 1, ChannelObject::v_onUserPart},
	{"onUserNickChange", 2, ChannelObject::v_onNickChange},
	{"onUserStatusChange", 3, ChannelObject::v_onUserStatusChange},
	{"getUserCount", 0, ChannelObject::v_getUserCount},
	{"enumUser", 1, ChannelObject::v_enumUser},
	{"getFirstUser", 0, ChannelObject::v_getFirstUser},
	{"getNextUser", 1, ChannelObject::v_getNextUser},
	{"hasUser", 1, ChannelObject::v_hasUser},
	{"getUserStatus", 1, ChannelObject::v_getUserStatus},
	{"isUserOwner", 1, ChannelObject::v_isUserOwner},
	{"isUserOp", 1, ChannelObject::v_isUserOp},
	{"isUserHalfop", 1, ChannelObject::v_isUserHalfop},
	{"isUserVoice", 1, ChannelObject::v_isUserVoice},
};


ChannelScriptObjectController::getNumFunctions() {
  return sizeof(exportedFunction) / sizeof(function_descriptor_struct); 
}
#endif
