//#define _CRT_SECURE_NO_WARNINGS

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/otAssert.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"

// std header
#include <iostream>
#include <ctime>
#include <thread>

ot::ServiceLogNotifier& ot::ServiceLogNotifier::instance(void) {
	static ServiceLogNotifier g_instance;
	return g_instance;
}

ot::ServiceLogNotifier& ot::ServiceLogNotifier::initialize(const std::string& _serviceName, const std::string& _loggingServiceUrl, bool _addCoutReceiver) {
	ot::LogDispatcher::initialize(_serviceName, _addCoutReceiver);

	ot::ServiceLogNotifier& obj = ot::ServiceLogNotifier::instance();
	obj.deleteLater();
	ot::LogDispatcher::instance().addReceiver(&obj);
	if (!_loggingServiceUrl.empty() && _loggingServiceUrl.find("//") != 0) obj.setLoggingServiceURL(_loggingServiceUrl);
	return obj;
}

// #################################################################################

void ot::ServiceLogNotifier::setLoggingServiceURL(const std::string& _url) {
	if (_url.empty() || _url.find("//") == 0) {
		m_loggingServiceURL = "";
	}
	else {
		JsonDocument pingDoc;
		pingDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_Ping, pingDoc.GetAllocator());
		std::string response;
		if (ot::msg::send("", _url, ot::EXECUTE, pingDoc.toJson(), response, 3000, false, false)) {
			if (response == OT_ACTION_CMD_Ping) {
				m_loggingServiceURL = _url;
			}
			else {
				OTAssert(0, "Failed to ping Logging Service: Invalid ping response");
			}
		}
		else {
			OTAssert(0, "Failed to ping Logging Service: Ping not delivered");
		}
	}
}

void ot::ServiceLogNotifier::log(const LogMessage& _message) {	
	if (m_loggingServiceURL.empty()) return;

	std::string response;
	JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_Log, doc.GetAllocator());
	JsonObject logObj;
	_message.addToJsonObject(logObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LOG, logObj, doc.GetAllocator());

	try {
		// Write log message to logger service
		if (!ot::msg::send("", m_loggingServiceURL, ot::EXECUTE, doc.toJson(), response, 3000, false, false)) {
			assert(0);
		}
	}

#ifdef _DEBUG
	catch (const std::exception& _e) {
		assert(0);
		std::cout << "[FATAL] Error while logging: " << _e.what() << std::endl;
	}
	catch (...) {
		assert(0);
		std::cout << "[FATAL] Error while logging: Unknown Error" << std::endl;
	}
#else
	catch (...) {
		// todo: in release mode the error handling for sending log messages does not exists in any way..
	}
#endif // _DEBUG
}

ot::ServiceLogNotifier::ServiceLogNotifier()
{
	char buffer[128];
	size_t bufferLen;
	getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_URL");
	setLoggingServiceURL(std::string(buffer));
}

ot::ServiceLogNotifier::~ServiceLogNotifier() {
	
}