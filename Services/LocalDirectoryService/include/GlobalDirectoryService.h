#pragma once

#include "OTCore/ServiceBase.h"

#include <string>

class GlobalDirectoryService : ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		WaitingForConnection
	};

	GlobalDirectoryService(void);
	virtual ~GlobalDirectoryService();
	
	// #################################################################################################################################

	// Connection

	bool isConnected(void) const;
	ConnectionStatus connectionStatus(void) const { return m_connectionStatus; }
	void connect(const std::string& _url);

	// #################################################################################################################################
private:
	void registerAtGlobalDirectoryService(void);
	void healthCheck(void);
	void addSystemValues(ot::JsonDocument& _jsonDocument);

	bool				m_isShuttingDown;
	ConnectionStatus	m_connectionStatus;

	GlobalDirectoryService(GlobalDirectoryService&) = delete;
	GlobalDirectoryService& operator = (GlobalDirectoryService&) = delete;
};