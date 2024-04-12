//! @file GlobalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OTObjectBase.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/IDManager.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <mutex>

class LocalSessionService;

class GlobalSessionService : public ot::OTObjectBase, public ot::ServiceBase {
public:
	static GlobalSessionService * instance(void);
	static bool hasInstance(void);
	static void deleteInstance(void);

	// ###################################################################################################

	// Service handling

	bool addSessionService(LocalSessionService& _service);

	void setUrl(const std::string& _url) { m_url = _url; };
	const std::string& url(void) const { return m_url; };

	void setDatabaseUrl(const std::string& _url) { m_databaseUrl = _url; };
	const std::string databaseUrl(void) const { return m_databaseUrl; };

	void setAuthorizationUrl(const std::string& _url) { m_authorizationUrl = _url; };
	const std::string& authorizationUrl(void) const { return m_authorizationUrl; };

	void setGlobalDirectoryUrl(const std::string& _url) { m_globalDirectoryUrl = _url; };
	const std::string& globalDirectoryUrl(void) const { return m_globalDirectoryUrl; };

private:

	// ###################################################################################################

	// Handler

	OT_HANDLER(handleGetDBUrl, GlobalSessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetDBandAuthURL, GlobalSessionService, OT_ACTION_CMD_GetDBandAuthServerUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleCreateSession, GlobalSessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleCheckProjectOpen, GlobalSessionService, OT_ACTION_CMD_IsProjectOpen, ot::ALL_MESSAGE_TYPES);

	OT_HANDLER(handleRegisterSessionService, GlobalSessionService, OT_ACTION_CMD_RegisterNewSessionService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleShutdownSession, GlobalSessionService, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleForceHealthcheck, GlobalSessionService, OT_ACTION_CMD_ForceHealthCheck, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleNewGlobalDirectoryService, GlobalSessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES);

	// ###################################################################################################

	// Private functions

	void healthCheck(void);

	//! @brief Will remove the service from the service map aswell as all sessions in this service
	void removeSessionService(LocalSessionService* _service);

	LocalSessionService* leastLoadedSessionService(void);

	GlobalSessionService();
	virtual ~GlobalSessionService();

	// ###################################################################################################

	void setHealthCheckRunning(bool _isRunning) { m_healthCheckRunning = _isRunning; };

	std::string m_url;
	std::string m_databaseUrl;
	std::string m_authorizationUrl;
	std::string m_globalDirectoryUrl;
	bool m_healthCheckRunning;

	std::mutex									m_mapMutex;
	std::map<std::string, LocalSessionService*>		m_sessionToServiceMap;
	std::map<ot::serviceID_t, LocalSessionService*>	m_sessionServiceIdMap;
	ot::IDManager<ot::serviceID_t>				m_sessionServiceIdManager;
	bool										m_forceHealthCheck;

	GlobalSessionService(GlobalSessionService&) = delete;
	GlobalSessionService& operator = (GlobalSessionService&) = delete;
};