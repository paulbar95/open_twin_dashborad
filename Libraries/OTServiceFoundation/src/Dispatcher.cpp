// OpenTwin header
#include "OTCore/otAssert.h"
#include "OTCore/Logger.h"

#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/ActionTypes.h"

#include "OTServiceFoundation/Dispatcher.h"
#include "OTServiceFoundation/OTObject.h"

ot::Dispatcher * g_instance{ nullptr };
ot::Dispatcher * g_UIinstance{ nullptr };

ot::Dispatcher * ot::Dispatcher::instance(void) {
	if (g_instance == nullptr) g_instance = new Dispatcher;
	return g_instance;
}

ot::Dispatcher * ot::Dispatcher::uiPluginInstance(void) {
	if (g_UIinstance == nullptr) g_UIinstance = new Dispatcher;
	return g_UIinstance;
}

void ot::Dispatcher::deleteAllInstances(void) {
	deleteInstance();
	deleteUiPluginInstance();
}

void ot::Dispatcher::deleteInstance(void) {
	if (g_instance) delete g_instance;
	g_instance = nullptr;
}

void ot::Dispatcher::deleteUiPluginInstance(void) {
	if (g_UIinstance) delete g_UIinstance;
	g_UIinstance = nullptr;
}

ot::Dispatcher::Dispatcher() {

}

ot::Dispatcher::~Dispatcher() {

}

void ot::Dispatcher::add(AbstractDispatchItem * _item) {
	m_mutex.lock();
	
	auto it = m_data.find(_item->actionName());
	if (it == m_data.end()) m_data.insert_or_assign(_item->actionName(), new std::list<AbstractDispatchItem *>{ _item });
	else OTAssert(0, "A handler for the specified action was already created");

	m_mutex.unlock();
}

void ot::Dispatcher::remove(AbstractDispatchItem * _item) {
	m_mutex.lock();

	auto it = m_data.find(_item->actionName());
	if (it == m_data.end()) {
		m_mutex.unlock();
		return;
	}
	auto it2 = std::find(it->second->begin(), it->second->end(), _item);
	while (it2 != it->second->end()) {
		it->second->erase(it2);
		it2 = std::find(it->second->begin(), it->second->end(), _item);
	}

	m_mutex.unlock();
}

const char * ot::Dispatcher::dispatchWrapper(const char * _json, const char * _senderUrl, ot::MessageType _messageType) {
	std::string result = dispatch(_json, _messageType);

	// Return value must be copied to a memory location that will not be destroyed by the session service
	size_t len = result.length() + 1;
	char * returnValue = new char[len];
	strcpy_s(returnValue, len, result.c_str());
	return returnValue;
}

std::string ot::Dispatcher::dispatch(const std::string& _json, ot::MessageType _messageType) {
	// Parse the json string and check its main syntax
	JsonDocument doc;
	doc.fromJson(_json);

	// Get the action and dispatch it
	std::string action = json::getString(doc, OT_ACTION_MEMBER);

	if (action.empty()) {
		return OT_ACTION_RETURN_INDICATOR_Error "Action syntax error";
	}

	bool handlerFound = false;
	std::string result = dispatch(action, doc, handlerFound, _messageType);
	
	// Check if a handler was found or an error occured (non empty result);
	if (!handlerFound && result.empty()) {
		OT_LOG_W("Unknown action received: " + action);
		result = OT_ACTION_RETURN_UnknownAction;
	}

	return result;
}

std::string ot::Dispatcher::dispatch(const std::string& _action, JsonDocument& _document, ot::MessageType _messageType) {
	bool tmp = false;
	return dispatch(_action, _document, tmp, _messageType);
}

std::string ot::Dispatcher::dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, ot::MessageType _messageType) {
	// Check for default actions
	if (_action == OT_ACTION_CMD_Ping) { 
		_handlerFound = true;
		return OT_ACTION_CMD_Ping;
	}; // todo: switch to return OK (like for every empty/default result)

	OT_LOG("Dispatching start: \"" + _action + "\"", ot::messageTypeToLogFlag(_messageType));
	
	m_mutex.lock();

	std::string result;


	try {
		auto it = m_data.find(_action);
		_handlerFound = false;
		if (it != m_data.end()) {
			for (auto itm : *it->second) {
				if (itm->mayDispatch(_messageType)) {
					result = itm->forwardDispatch(_document);
					_handlerFound = true;
				}
			}
		}
		else if (_action == OT_ACTION_CMD_SetLoggingOptions) {
			// todo: add global logging options logic
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		result = std::string(OT_ACTION_RETURN_INDICATOR_Error) + _e.what();
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error occured");
		result = OT_ACTION_RETURN_INDICATOR_Error "Unknown error";
	}

	if (_handlerFound) {
		OT_LOG("..... Completed: Dispatching: \"" + _action + "\". Returning: \"" + result + "\"", ot::messageTypeToLogFlag(_messageType));
	}
	else {
		OT_LOG("..... Completed: Dispatching: \"" + _action + "\". FAILED: No handler found. Returning: \"" + result + "\"", ot::messageTypeToLogFlag(_messageType));
	}
	
	m_mutex.unlock();
	OT_LOG("Dispatching end: \"" + _action + "\"", ot::messageTypeToLogFlag(_messageType));

	return result;
}
