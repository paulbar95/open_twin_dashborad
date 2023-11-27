#include "GlobalSessionService.h"
#include "SessionService.h"
#include "Session.h"

#include "OTCore/CoreTypes.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#include <thread>
#include <chrono>

GlobalSessionService::GlobalSessionService(const std::string& _url, SessionService * _owner)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService, _url, ot::invalidServiceID),
	m_owner(_owner), m_connectionStatus(Connected), m_healthCheckRunning(false), m_workerThread(nullptr)
{

}

GlobalSessionService::~GlobalSessionService() {

}

void GlobalSessionService::startHealthCheck(void) {
	if (m_workerThread) {
		OTAssert(0, "Health check already running");
		return;
	}

	m_healthCheckRunning = true;

	OT_LOG_D("Starting health check");
	m_workerThread = new std::thread(&GlobalSessionService::healthCheck, this);
}

void GlobalSessionService::stopHealthCheck(bool _joinThread) {
	if (m_workerThread) {
		m_healthCheckRunning = false;
		if (_joinThread) {
			m_workerThread->join();
		}
	}
	else {
		OTAssert(0, "No health check currently running");
	}
}

// #################################################################################################################################################

// Private functions

void GlobalSessionService::healthCheck(void) {
	int ct;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PING, doc.GetAllocator()), doc.GetAllocator());
	std::string pingMessage = doc.toJson();

	while (m_healthCheckRunning) {
		ct = 0;
		while (ct++ < 60) { // Wait for 1 min
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		std::string response;

		// Send ping
		if (!ot::msg::send(m_owner->url(), m_serviceURL, ot::EXECUTE, pingMessage, response)) {
			OT_LOG_W("Global session service can not be reached");
			m_connectionStatus = ConnectionFailed;
		}
		// Check response
		else if (response != OT_ACTION_PING) {
			OT_LOG_W("Received invalid ping response from global session service");
			m_connectionStatus = ConnectionFailed;
		}
		// Check if last ping was not successful
		else if (m_connectionStatus == ConnectionFailed) {
			// Register again at the session service
			ot::JsonDocument registerDoc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewSessionService, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_owner->url(), doc.GetAllocator()), doc.GetAllocator());

			ot::JsonArray sessionList;
			for (auto s : m_owner->sessions()) {
				sessionList.PushBack(ot::JsonString(s->id(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
			}
			doc.AddMember(OT_ACTION_PARAM_SESSION_LIST, sessionList, doc.GetAllocator());

			response.clear();
			// Send registration
			if (!ot::msg::send(m_owner->url(), m_serviceURL, ot::EXECUTE, registerDoc.toJson(), response)) {
				OT_LOG_E("Failed to send register message to global session service");
			}
			else OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
			else {
				// Get new id and database url
				ot::JsonDocument registrationResponseDoc;
				registrationResponseDoc.fromJson(response);
				ot::serviceID_t ssID = ot::json::getUInt(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_ID);
				std::string databaseURL = ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_DATABASE_URL);
				m_owner->setId(ssID);
				m_owner->setDataBaseURL(databaseURL);
				
				m_connectionStatus = Connected;
			}
		}
	}
}