#include "stdafx.h"

#include "MicroserviceAPI.h"
#include "MicroserviceNotifier.h"
#include "ProjectTypeManager.h"
#include "Model.h"
#include "Types.h"
#include "curl/curl.h"

#include "EntityGeometry.h"

#include "DataBase.h"
#include <array>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <thread>

#include "OTCore/Logger.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ServiceLogNotifier.h"

#include "base64.h"
#include "zlib.h"

std::string globalUIserviceURL;
std::string globalServiceURL;
std::string globalSessionServiceURL;
std::string globalDirectoryServiceURL;
ot::serviceID_t globalServiceID;
std::string globalSessionID;
std::string globalSiteID;
int globalSessionCount = 0;

bool globalServiceURLsRetrieved = false;
std::map<std::string, std::string> globalServiceURLs;

bool globalUIMessageQueuingEnabled = false;
ot::JsonDocument *globalQueuedDoc = nullptr;

MicroserviceNotifier *globalNotifier = nullptr;

Model *globalModel = nullptr;

#define SERVICE_INFO_NAME "Model"

void sessionServiceHealthChecker(std::string _sessionServiceURL) {
	// Create ping request
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string ping = pingDoc.toJson();

	bool alive = true;
	while (alive) {
		// Wait for 20s
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20s);

		// Try to send message and check the response
		std::string ret;
		try {
			if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE, ping, ret, 3000)) { alive = false; }
			else OT_ACTION_IF_RESPONSE_ERROR(ret) { alive = false; }
			else OT_ACTION_IF_RESPONSE_WARNING(ret) { alive = false; }
			else if (ret != OT_ACTION_CMD_Ping) { alive = false; }
		}
		catch (...) { alive = false; }
	}

	OT_LOG_E("Session service has died unexpectedly. Shutting down...");
	exit(0);
}


std::string getServiceName(void)
{
	return SERVICE_INFO_NAME;
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

void MicroserviceAPI::enableQueuingHttpRequests(bool flag)
{
	using namespace std::chrono_literals;
	static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	static int count = 0;
	if (flag)
	{
		count++;
	}
	else
	{
		count--;
		assert(count >= 0);
	}

	if (!flag && (count == 0) && globalUIMessageQueuingEnabled)
	{
		flushQueuedHttpRequests();
	}

	globalUIMessageQueuingEnabled = (count > 0);

	lock = false;
}

void MicroserviceAPI::flushQueuedHttpRequests(void)
{
	using namespace std::chrono_literals;
	static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	if (globalQueuedDoc != nullptr)
	{
#ifdef _DEBUG
		//std::cout << "<< Flush: " << ot::json::toJSON(*globalQueuedDoc) << std::endl;
#endif // _DEBUG

		std::string response;
		assert(globalQueuedDoc->IsObject());
		sendHttpRequest(MicroserviceAPI::QUEUE, MicroserviceAPI::getUIURL(), *globalQueuedDoc, response);

		delete globalQueuedDoc;
		globalQueuedDoc = nullptr;
	}

	lock = false;
}

void MicroserviceAPI::queuedHttpRequestToUI(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds)
{
	using namespace std::chrono_literals;
	static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	if (globalUIMessageQueuingEnabled)
	{
		if (globalQueuedDoc == nullptr)
		{
			globalQueuedDoc = new ot::JsonDocument(MicroserviceAPI::BuildJsonDocFromAction(OT_ACTION_CMD_Compound));

			rapidjson::Value docs(rapidjson::kArrayType);
			rapidjson::Value prefID(rapidjson::kArrayType);
			rapidjson::Value prefVersion(rapidjson::kArrayType);

			ot::JsonDocument::AllocatorType& allocator = globalQueuedDoc->GetAllocator();

			std::string projectName = DataBase::GetDataBase()->getProjectName();

			globalQueuedDoc->AddMember(OT_ACTION_PARAM_PROJECT_NAME, rapidjson::Value(projectName.c_str(), allocator), allocator);
			globalQueuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_Documents, docs, allocator);
			globalQueuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_ID, prefID, allocator);
			globalQueuedDoc->AddMember(OT_ACTION_PARAM_PREFETCH_Version, prefVersion, allocator);
		}

		ot::JsonDocument::AllocatorType& allocator = globalQueuedDoc->GetAllocator();

		rapidjson::Value v(doc, allocator);
		(*globalQueuedDoc)[OT_ACTION_PARAM_PREFETCH_Documents].PushBack(v, allocator);

		for (auto id : prefetchIds)
		{
			(*globalQueuedDoc)[OT_ACTION_PARAM_PREFETCH_ID].PushBack(id.first, allocator);
			(*globalQueuedDoc)[OT_ACTION_PARAM_PREFETCH_Version].PushBack(id.second, allocator);
		}
	}
	else
	{
		std::string response;
		sendHttpRequest(MicroserviceAPI::QUEUE, MicroserviceAPI::getUIURL(), doc, response);
	}

	lock = false;
}

bool MicroserviceAPI::sendHttpRequest(RequestType operation, const std::string &url, ot::JsonDocument &doc, std::string &response)
{
	return sendHttpRequest(operation, url, doc.toJson(), response);
}

bool MicroserviceAPI::sendHttpRequest(RequestType operation, const std::string &url, const std::string & message, std::string &response)
{
	bool success = false;

	switch (operation)
	{
	case EXECUTE:
		success = ot::msg::send(globalServiceURL, url, ot::EXECUTE, message, response);
		break;
	case QUEUE:
		success = ot::msg::send(globalServiceURL, url, ot::QUEUE, message, response);
		break;
	default:
		assert(0);
	}

	return success;
}

const char *MicroserviceAPI::queueAction(const char *json, const char *senderIP) 
{ 
	OT_LOG("Forwarding message to execute endpoint", ot::QUEUED_INBOUND_MESSAGE_LOG);
	return performAction(json, senderIP);
}

const char *MicroserviceAPI::performAction(const char *json, const char *senderIP)
{
	OT_LOG("From \"" + std::string(senderIP) + "\" Received: \"" + std::string(json) + "\"", ot::INBOUND_MESSAGE_LOG);

	using namespace std::chrono_literals;
	thread_local static bool lock = false;
	while (lock) std::this_thread::sleep_for(1ms);
	lock = true;

	// Parse json -> action + arguments
	ot::JsonDocument doc;
	doc.Parse(json);
	assert(doc.IsObject());

	std::string result = dispatchAction(doc, senderIP);

	// Create a copy of result... -> const char * (new to allocate the memory)
	char *retVal = new char[result.size() + 1];
	strcpy_s(retVal, result.size() + 1, result.c_str());

	lock = false;

	OT_LOG(".. Execute completed. Returning: \"" + result + "\"", ot::INBOUND_MESSAGE_LOG);

	return retVal;
}

const char *MicroserviceAPI::getServiceURL(void)
{
	char *retVal = new char[globalServiceURL.size() + 1];
	strcpy_s(retVal, globalServiceURL.size() + 1, globalServiceURL.c_str());

	return retVal;
}

void MicroserviceAPI::deallocateData(const char *str)
{
	if (str) {
		delete[] str;
	}
}

std::string initInternal() {
	OT_LOG_D("Initialization (URL = \"" + globalServiceURL +
		"\"; LSS = \"" + globalSessionServiceURL +
		"\"; SessionID = \"" + globalSessionID + "\")");

	// Get the database information
	ot::JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDatabaseUrl, request.GetAllocator()), request.GetAllocator());

	std::string response;
	if (!MicroserviceAPI::sendHttpRequest(MicroserviceAPI::EXECUTE, globalSessionServiceURL, request.toJson(), response)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to send message"; }
	if (response.rfind("ERROR:") != std::string::npos) { 
		return response;
	}
	else if (response.rfind("WARNING:") != std::string::npos) { 
		return response;
	}

	DataBase::GetDataBase()->setDataBaseServerURL(response);
	DataBase::GetDataBase()->setSiteIDString("1");
	//		DataBase::GetDataBase()->InitializeConnection(response, _siteID);

	// Register service
	response.clear();
	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewService, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(globalSessionID, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(SERVICE_INFO_NAME, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(OT_INFO_SERVICE_TYPE_MODEL, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_PORT, ot::JsonString(ot::IpConverter::portFromIp(globalServiceURL), commandDoc.GetAllocator()), commandDoc.GetAllocator());

#ifdef _DEBUG
	auto handle = GetCurrentProcess();
	if (handle != nullptr) {
		unsigned long handleID = GetProcessId(handle);
		assert(handleID != 0); // Failed to get process handle ID
		commandDoc.AddMember(OT_ACTION_PARAM_PROCESS_ID, ot::JsonString(std::to_string(handleID), commandDoc.GetAllocator()), commandDoc.GetAllocator());
	}
	else {
		assert(0); // Failed to get current process handle
	}
#endif // _DEBUG

	if (!ot::msg::send(globalServiceURL, globalSessionServiceURL, ot::EXECUTE, commandDoc.toJson(), response)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to send message"; }
	OT_ACTION_IF_RESPONSE_ERROR(response) { return response; }
	else OT_ACTION_IF_RESPONSE_WARNING(response) { return response; }

	ot::JsonDocument reply;
	reply.fromJson(response);
	globalServiceID = ot::json::getUInt(reply, OT_ACTION_PARAM_SERVICE_ID);

	OT_LOG_I("Initialization completed successfully");

	// Start the session service health check
	std::thread t{ sessionServiceHealthChecker, globalSessionServiceURL };
	t.detach();

	return OT_ACTION_RETURN_VALUE_OK;
}

int MicroserviceAPI::init(const char * _localDirectoryServiceURL, const char * _serviceIP, const char * _sessionServiceURL, const char * _sessionID)
{
	try {
		// Initialize logging
#ifdef _DEBUG
		std::cout << "Model Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_MODEL, "", true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_MODEL, "", false);
#endif

		globalNotifier = new MicroserviceNotifier;
		globalSiteID = "1";
		globalServiceURL = _serviceIP;
		globalDirectoryServiceURL = _localDirectoryServiceURL;

#ifdef _DEBUG
		{
			// Get file path
			char * buffer = nullptr;
			size_t bufferSize = 0;
			if (_dupenv_s(&buffer, &bufferSize, "OPENTWIN_DEV_ROOT") != 0 || buffer == nullptr) {
				std::cout << "[ERROR] Please specify the environment variable \"OPENTWIN_DEV_ROOT\"" << std::endl;
				return -20;
			}
			std::string path = buffer;
			delete[] buffer;
			buffer = nullptr;
			path.append("\\Deployment\\" OT_INFO_SERVICE_TYPE_MODEL ".cfg");

			// Read file
			std::ifstream stream(path);
			char inBuffer[512];
			stream.getline(inBuffer, 512);
			std::string info(inBuffer);

			// Parse doc
			ot::JsonDocument params;
			params.Parse(info.c_str());
			assert(params.IsObject());

			// Apply data
			globalServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
			globalSessionServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
			globalDirectoryServiceURL = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
			globalSessionID = ot::json::getString(params, OT_ACTION_PARAM_SESSION_ID);
			globalSiteID = ot::json::getString(params, OT_ACTION_PARAM_SITE_ID);

			initInternal();
		}
#endif // _DEBUG

		OT_LOG_D("Data initialized (SiteID = \"" + globalSiteID + "\"; URL = \"" + globalServiceURL + "\"; GDS = \"" + globalDirectoryServiceURL + ")");

		return 0;
	}
	catch (const std::exception & e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "ERROR: Unknown error" << std::endl;
		return 2;
	}
}

std::string MicroserviceAPI::getStringFromDocument(ot::JsonDocument &doc, const char *attribute)
{
	std::string value = doc[attribute].GetString();

	// Skip the " at the beginning and at the end of the string
	size_t index = value.find('"');
	if (index != -1) value.erase(index, 1);

	index = value.rfind('"');
	if (index != -1) value.erase(index);

	return value;
}

std::string MicroserviceAPI::dispatchAction(ot::JsonDocument &doc, const std::string &senderIP)
{
	try {
		std::string action = getStringFromDocument(doc, OT_ACTION_MEMBER);
#ifdef _DEBUG
		{
			//std::string debugMsg{ ">>> Dispatch action: " };
			//debugMsg.append(action);
			//std::cout << debugMsg << std::endl;
			//debugMsg.append("\n");
			//OutputDebugStringA(debugMsg.c_str());
		}
#endif // DEBUG
		std::string result;
		/*
		if (action == "setUIConnectionInformation")
		{
			try
			{
				std::string uiServicePort = doc["uiServicePort"].GetString();

				std::string uiServiceIP = senderIP;

				if (uiServiceIP.substr(0, 5) == "Some(") uiServiceIP = uiServiceIP.substr(5);
				if (uiServiceIP.substr(0, 3) == "V4(") uiServiceIP = uiServiceIP.substr(3);

				// We have an IP V4 address
				size_t index = uiServiceIP.rfind(':');
				if (index == -1) throw new std::exception;

				uiServiceIP = uiServiceIP.substr(0, index);

				globalUIserviceURL = uiServiceIP + ":" + uiServicePort;

				std::cout << "Connection with UI service established at: " << globalUIserviceURL << std::endl;
			}
			catch (std::exception)
			{
				// Unknown address format
				std::cout << "ERROR: Unknown sender address: " << senderIP << std::endl;
				assert(0);
			}
		}
		else
		*/
		if (action == OT_ACTION_CMD_Ping) {
			return OT_ACTION_CMD_Ping;
		}
		else if (action == OT_ACTION_CMD_Init) {
			globalSessionServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SESSION_SERVICE_URL);
			globalSessionID = ot::json::getString(doc, OT_ACTION_PARAM_SESSION_ID);

			return initInternal();
		}
		else if (action == OT_ACTION_CMD_Run) {

			std::string credentialsUsername = ot::json::getString(doc, OT_PARAM_AUTH_USERNAME);
			std::string credentialsPassword = ot::json::getString(doc, OT_PARAM_AUTH_PASSWORD);

			std::string projectType = ot::json::getString(doc, OT_ACTION_PARAM_SESSION_TYPE);

			DataBase::GetDataBase()->setUserCredentials(credentialsUsername, credentialsPassword);
			DataBase::GetDataBase()->InitializeConnection(DataBase::GetDataBase()->getDataBaseServerURL(), DataBase::GetDataBase()->getSiteIDString());

			// Change the service to visible
			{
				ot::JsonDocument visibilityCommand;
				visibilityCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceShow, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
				visibilityCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, visibilityCommand.GetAllocator());
				visibilityCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(globalSessionID, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());

				std::string response;
				if (!sendHttpRequest(QUEUE, globalSessionServiceURL, visibilityCommand, response)) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request" << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request";
				}
				if (response.rfind(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
				}
				else if (response.rfind(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
				}
			}

			// Get information about running services in the session
			if (!checkForUiConnection()) { std::cout << "No UI is currently registered in the session" << std::endl; }

			// Filter project information
			size_t index = globalSessionID.find(':');
			std::string projectName(globalSessionID.substr(0, index));
			std::string collectionName(globalSessionID.substr(index + 1));

			if (index < 0) { return OT_ACTION_RETURN_INDICATOR_Error "Session ID invalid syntax"; }

			assert(globalModel == nullptr);
			globalModel = new Model(projectName, projectType, collectionName);

			// Create a model in the UI
			if (globalUIserviceURL.length() > 0) {
				ot::JsonDocument commandDoc;
				commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MODEL_Create, commandDoc.GetAllocator()), commandDoc.GetAllocator());
				commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, commandDoc.GetAllocator());

				std::string response;
				if (!sendHttpRequest(QUEUE, globalUIserviceURL, commandDoc, response)) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request" << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request";
				}
				if (response.rfind(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
				}
				else if (response.rfind(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
					std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
					return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
				}
			}

			return "";
		}
		else if (action == OT_ACTION_CMD_MODEL_Delete)
		{
			if (globalModel != nullptr)
			{
				delete globalModel;
				globalModel = nullptr;
			}
		}/*
		else if (action == "projectOpen") {
			assert(0); // Not in use anymore
			
		}*/
		else if (action == OT_ACTION_CMD_ServicePreShutdown) {

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->projectSave("", false);

			// Here we disconnect all viewers, but leave them alone
			globalModel->detachAllViewer();

			// Disable the UI clear since the UI will clear all controls anyway
			globalModel->setClearUiOnDelete(false);

			delete globalModel;
			globalModel = nullptr;

			//deregisterAtUI();
		}
		else if (action == OT_ACTION_CMD_PROJ_Save) {
			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->projectSave("", false);
		}
		else if (action == OT_ACTION_CMD_MODEL_SelectionChanged)
		{
			std::list<ot::UID> selectedEntityID = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
			std::list<ot::UID> selectedVisibleEntityID = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs);
			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->modelSelectionChangedNotification(selectedEntityID, selectedVisibleEntityID);
		}
		else if (action == OT_ACTION_CMD_MODEL_ItemRenamed)
		{
			ot::UID entityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_ID);
			std::string newName = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_ITM_Name);
			globalModel->modelItemRenamed(entityID, newName);
		}
		else if (action == OT_ACTION_CMD_SetVisualizationModel)
		{
			ot::UID viewerModelID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_ID);
			ot::UID viewerViewID = ot::json::getUInt64(doc, OT_ACTION_PARAM_VIEW_ID);
			std::cout << "Visualization model set to \"" << viewerModelID << "\". Opening project" << std::endl;
			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->projectOpen();
			globalModel->setVisualizationModel(viewerModelID);
		}
		else if (action == OT_ACTION_CMD_GetVisualizationModel)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");
			ot::UID visualizationModelID = globalModel->getVisualizationModel();
			result = getReturnJSONFromUID(visualizationModelID);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetIsModified)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");
			bool modified = globalModel->getModified();
			result = getReturnJSONFromBool(modified);
		}
		else if (action == OT_ACTION_CMD_MODEL_CommonPropertiesJSON)
		{
			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			if (globalModel == nullptr) throw std::exception("No model created yet");
			std::string props = globalModel->getCommonPropertiesAsJson(entityIDList, false);
			result = getReturnJSONFromString(props);
		}
		else if (action == OT_ACTION_CMD_MODEL_SetPropertiesFromJSON)
		{
			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			std::string props = doc[OT_ACTION_PARAM_MODEL_PropertyList].GetString();
			bool update = doc[OT_ACTION_PARAM_MODEL_Update].GetBool();
			bool itemsVisible = doc[OT_ACTION_PARAM_MODEL_ItemsVisible].GetBool();
			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->setPropertiesFromJson(entityIDList, props, update, itemsVisible);
		}
		else if (action == OT_ACTION_CMD_MODEL_GenerateEntityIDs)
		{
			unsigned long long count = ot::json::getUInt64(doc, OT_ACTION_PARAM_COUNT);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			return getReturnJSONFromUIDList(globalModel->getNewEntityIDs(count));
		}
		else if (action == OT_ACTION_CMD_MODEL_ImportTableFile)
		{
			std::string itemName = doc[OT_ACTION_PARAM_NAME].GetString();

			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->importTableFile(itemName);
		}
		else if (action == OT_ACTION_CMD_MODEL_QueueMessages)
		{
			bool flag = doc[OT_ACTION_PARAM_QUEUE_FLAG].GetBool();

			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->enableQueuingHttpRequests(flag);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetListOfFolderItems)
		{
			std::string folder = ot::json::getString(doc, OT_ACTION_PARAM_Folder);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);
			if (globalModel == nullptr) throw std::exception("No model created yet");
			
			return getReturnJSONFromStringList(globalModel->getListOfFolderItems(folder,recursive));
		}
		else if (action == OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType)
		{
			std::string folder    = ot::json::getString(doc, OT_ACTION_PARAM_Folder);
			std::string className = ot::json::getString(doc, OT_ACTION_PARAM_Type);
			bool recursive        = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			return getReturnJSONFromUIDList(globalModel->getIDsOfFolderItemsOfType(folder, className, recursive));
		}

		else if (action == OT_ACTION_CMD_MODEL_UpdateVisualizationEntity)
		{
			ot::UID visEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
			ot::UID visEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion);
			ot::UID binaryDataItemID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_DataID);
			ot::UID binaryDataItemVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_DataVersion);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->updateVisualizationEntity(visEntityID, visEntityVersion, binaryDataItemID, binaryDataItemVersion);
		}
		else if (action == OT_ACTION_CMD_MODEL_UpdateGeometryEntity)
		{
			ot::UID geomEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
			ot::UID brepEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID_Brep);
			ot::UID facetsEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID_Facets);
			ot::UID brepEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion_Brep);
			ot::UID facetsEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion_Facets);
			bool overrideGeometry = doc[OT_ACTION_PARAM_MODEL_OverrideGeometry].GetBool();

			std::string properties;
			bool updateProperties = false;

			if (doc.HasMember(OT_ACTION_PARAM_MODEL_NewProperties)) {
				properties = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_NewProperties);
				updateProperties = true;
			}

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->updateGeometryEntity(geomEntityID, brepEntityID, brepEntityVersion, facetsEntityID, facetsEntityVersion, overrideGeometry, properties, updateProperties);
		}
		else if (action == OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted)
		{
			std::string description = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Description);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->modelChangeOperationCompleted(description);
		}
		else if (action == OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity)
		{
			ot::UID visEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->requestUpdateVisualizationEntity(visEntityID);
		}
		else if (action == OT_ACTION_CMD_MODEL_CheckParentUpdates)
		{
			std::list<ot::UID> modifiedEntities = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			if (globalModel == nullptr) throw std::exception("No model created yet");

			return globalModel->checkParentUpdates(modifiedEntities);
		}
		else if (action == OT_ACTION_CMD_MODEL_AddEntities)
		{
			std::list<ot::UID> topologyEntityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
			std::list<ot::UID> topologyEntityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
			std::list<bool> topologyEntityForceVisible = ot::json::getBoolList(doc, OT_ACTION_PARAM_MODEL_TopologyEntityForceShowList);
			std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityIDList);
			std::list<ot::UID> dataEntityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
			std::list<ot::UID> dataEntityParentList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityParentList);
			std::string changeComment = getStringFromDocument(doc, OT_ACTION_PARAM_MODEL_ITM_Description);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList, changeComment, true);
		}
		else if (action == OT_ACTION_CMD_MODEL_UpdateTopologyEntity)
		{
			std::list<ot::UID> topologyEntityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
			std::list<ot::UID> topologyEntityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
			std::string comment = getStringFromDocument(doc, OT_ACTION_PARAM_MODEL_ITM_Description);
			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList,comment);
		}
		else if (action == OT_ACTION_CMD_MODEL_AddGeometryOperation)
		{
			ot::UID geometryEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
			ot::UID geometryEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion);
			std::string geomEntityName = getStringFromDocument(doc, OT_ACTION_PARAM_MODEL_EntityName);
			std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityIDList);
			std::list<ot::UID> dataEntityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
			std::list<ot::UID> dataEntityParentList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_DataEntityParentList);
			std::list<std::string> childrenList = ot::json::getStringList(doc, OT_ACTION_PARAM_MODEL_EntityChildrenList);
			std::string changeComment = getStringFromDocument(doc, OT_ACTION_PARAM_MODEL_ITM_Description);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->addGeometryOperation(geometryEntityID, geometryEntityVersion, geomEntityName, dataEntityIDList, dataEntityVersionList, dataEntityParentList, childrenList, changeComment);
		}
		else if (action == OT_ACTION_CMD_MODEL_DeleteEntity)
		{
			std::list<std::string> entityNameList = ot::json::getStringList(doc, OT_ACTION_PARAM_MODEL_EntityNameList);
			bool saveModel = ot::json::getBool(doc, OT_ACTION_PARAM_MODEL_SaveModel);

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->deleteEntitiesFromModel(entityNameList, saveModel);
		}
		else if (action == OT_ACTION_CMD_MODEL_MeshingCompleted)
		{
			ot::UID meshEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);

			globalModel->setMeshingActive(meshEntityID, false);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityInformationFromID)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);

			return getEntityInformation(entityIDList);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityInformationFromName)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::list<std::string> entityNameList = ot::json::getStringList(doc, OT_ACTION_PARAM_MODEL_EntityNameList);

			std::list<ot::UID> entityIDList;
			for (auto name : entityNameList)
			{
				EntityBase *entity = globalModel->findEntityFromName(name);

				if (entity != nullptr)
				{
					entityIDList.push_back(entity->getEntityID());
				}
			}

			return getEntityInformation(entityIDList);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetSelectedEntityInformation)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::string typeFilter = getStringFromDocument(doc, OT_ACTION_PARAM_SETTINGS_Type);

			std::list<EntityBase *> entityList = globalModel->getListOfSelectedEntities(typeFilter);

			std::list<ot::UID> entityIDList;
			for (auto entity : entityList)
			{
				entityIDList.push_back(entity->getEntityID());
			}

			return getEntityInformation(entityIDList);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::string entityName = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_ITM_Name);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);

			std::list<ot::UID> entityIDList;
			EntityContainer *entity = dynamic_cast<EntityContainer*>(globalModel->findEntityFromName(entityName));

			if (entity != nullptr)
			{
				if (recursive)
				{
					std::list<std::pair<ot::UID, ot::UID>> childrenEntities;
					globalModel->getListOfAllChildEntities(entity, childrenEntities);

					for (auto item : childrenEntities)
					{
						entityIDList.push_back(item.first);
					}
				}
				else
				{
					for (auto child : entity->getChildrenList())
					{
						entityIDList.push_back(child->getEntityID());
					}
				}
			}

			return getEntityInformation(entityIDList);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			ot::UID entityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_ITM_ID);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);

			std::list<ot::UID> entityIDList;

			EntityContainer *entity = dynamic_cast<EntityContainer*>(globalModel->getEntity(entityID));

			if (entity != nullptr)
			{
				if (recursive)
				{
					std::list<std::pair<ot::UID, ot::UID>> childrenEntities;
					globalModel->getListOfAllChildEntities(entity, childrenEntities);

					for (auto item : childrenEntities)
					{
						entityIDList.push_back(item.first);
					}
				}
				else
				{
					for (auto child : entity->getChildrenList())
					{
						entityIDList.push_back(child->getEntityID());
					}
				}
			}

			return getEntityInformation(entityIDList);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			ot::JsonDocument newDoc;
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(globalModel->getAllGeometryEntitiesForMeshing(), newDoc.GetAllocator()), newDoc.GetAllocator());

			return newDoc.toJson();
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityProperties)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			ot::UID entityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);
			std::string propertyGroupFilter = ot::json::getString(doc, OT_ACTION_PARAM_Filter);

			std::map<ot::UID, std::string> entityProperties;

			globalModel->getEntityProperties(entityID, recursive, propertyGroupFilter, entityProperties);

			// Now we convert the results into JSON data
			std::list<ot::UID> entityIDList;
			std::list<std::string> propertyList;

			for (auto item : entityProperties)
			{
				entityIDList.push_back(item.first);
				propertyList.push_back(item.second);
			}

			ot::JsonDocument newDoc;
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, ot::JsonArray(propertyList, newDoc.GetAllocator()), newDoc.GetAllocator());

			return newDoc.toJson();
		}
		else if (action == OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::string entityName = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_EntityName);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);
			std::string propertyGroupFilter = ot::json::getString(doc, OT_ACTION_PARAM_Filter);

			EntityBase *entity = globalModel->findEntityFromName(entityName);

			std::map<ot::UID, std::string> entityProperties;

			if (entity != nullptr)
			{
				globalModel->getEntityProperties(entity->getEntityID(), recursive, propertyGroupFilter, entityProperties);
			}

			// Now we convert the results into JSON data
			std::list<ot::UID> entityIDList;
			std::list<std::string> propertyList;

			for (auto item : entityProperties)
			{
				entityIDList.push_back(item.first);
				propertyList.push_back(item.second);
			}

			ot::JsonDocument newDoc;
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_PropertyList, ot::JsonArray(propertyList, newDoc.GetAllocator()), newDoc.GetAllocator());

			return newDoc.toJson();
		}
		else if (action == OT_ACTION_CMD_MODEL_UpdatePropertyGrid)
		{
			globalModel->updatePropertyGrid();
		}
		else if (action == OT_ACTION_CMD_MODEL_GetCurrentVersion)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			return getReturnJSONFromString(globalModel->getCurrentModelVersion());
		}
		else if (action == OT_ACTION_CMD_MODEL_ActivateVersion)
		{
			std::string version = doc[OT_ACTION_PARAM_MODEL_Version].GetString();

			if (globalModel == nullptr) throw std::exception("No model created yet");

			globalModel->activateVersion(version);
		}
		else if (action == OT_ACTION_CMD_MODEL_GetCurrentVisModelID)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			return getReturnJSONFromUID(globalModel->getVisualizationModel());
		}
		else if (action == OT_ACTION_CMD_MODEL_AddPropertiesToEntities)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			std::string propertiesJson = doc[OT_ACTION_PARAM_MODEL_JSON].GetString();

			globalModel->addPropertiesToEntities(entityIDList, propertiesJson);
		}
		else if (action == OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");

			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			std::string propertiesJson = doc[OT_ACTION_PARAM_MODEL_JSON].GetString();

			globalModel->updatePropertiesOfEntities(entityIDList, propertiesJson);
		}
		else if (action == OT_ACTION_CMD_MODEL_DeleteProperty)
		{
			if (globalModel == nullptr) throw std::exception("No model created yet");
			
			std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
			std::string propertyName = doc[OT_ACTION_PARAM_MODEL_EntityName].GetString();

			globalModel->deleteProperty(entityIDList, propertyName);
		}
		else if (action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = doc[OT_ACTION_PARAM_MODEL_ActionName].GetString();
			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->executeAction(action, doc);
		}
		else if (action == OT_ACTION_CMD_MODEL_ExecuteFunction)
		{
			std::string function = doc[OT_ACTION_PARAM_MODEL_FunctionName].GetString();
			std::string mode = doc[OT_ACTION_PARAM_FILE_Mode].GetString();

			if (mode == OT_ACTION_VALUE_FILE_Mode_Name)
			{
				std::string fileName = doc[OT_ACTION_PARAM_FILE_Name].GetString();
				if (globalModel == nullptr) throw std::exception("No model created yet");
				globalModel->executeFunction(function, fileName, false);
			}
			else if (mode == OT_ACTION_VALUE_FILE_Mode_Content)
			{
				std::string content = doc[OT_ACTION_PARAM_FILE_Content].GetString();
				ot::UID uncompressedDataLength = doc[OT_ACTION_PARAM_FILE_Content_UncompressedDataLength].GetUint64();

				// Create a tmp file from uncompressing the data
				std::string tmpFileName = CreateTmpFileFromCompressedData(content, uncompressedDataLength);

				// Process the file content
				if (globalModel == nullptr) throw std::exception("No model created yet");
				globalModel->executeFunction(function, tmpFileName, true);
			}
			else
			{
				assert(0); // Unknown mode
			}
		}
		else if (action == OT_ACTION_CMD_MODEL_EntitiesSelected)
		{
			std::string selectionAction = doc[OT_ACTION_PARAM_MODEL_SelectionAction].GetString();
			std::string selectionInfo = doc[OT_ACTION_PARAM_MODEL_SelectionInfo].GetString();
			std::list<std::string> optionNames = ot::json::getStringList(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
			std::list<std::string> optionValues = ot::json::getStringList(doc, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);

			// Build a map from the option name and values lists
			std::map<std::string, std::string> options;
			auto optValue = optionValues.begin();
			for (auto optName : optionNames)
			{
				options[optName] = *optValue;
				optValue++;
			}

			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->entitiesSelected(selectionAction, selectionInfo, options);
		}
		else if (action == OT_ACTION_CMD_ServiceShutdown)
		{
			shutdown(true);
		}
		else if (action == OT_ACTION_CMD_ServiceConnected) {
			std::string senderIP = doc[OT_ACTION_PARAM_SERVICE_URL].GetString();
			std::string senderName = doc[OT_ACTION_PARAM_SERVICE_NAME].GetString();
			std::string senderType = doc[OT_ACTION_PARAM_SERVICE_TYPE].GetString();
			ot::serviceID_t senderID = doc[OT_ACTION_PARAM_SERVICE_ID].GetUint();
			if (senderType == OT_INFO_SERVICE_TYPE_UI) {
				if (globalUIserviceURL.empty())
				{
					std::cout << "UI found with {\"name\":\"" << senderName << "\", \"ip\":\"" << senderIP << "\"}" << std::endl;
					globalUIserviceURL = senderIP;
					registerAtUI();

					// Create a model in the UI
					if (globalUIserviceURL.length() > 0) {
						ot::JsonDocument commandDoc;
						commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MODEL_Create, commandDoc.GetAllocator()), commandDoc.GetAllocator());
						commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, commandDoc.GetAllocator());

						std::string response;
						if (!sendHttpRequest(QUEUE, globalUIserviceURL, commandDoc, response)) {
							std::cout << OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request" << std::endl;
							return OT_ACTION_RETURN_INDICATOR_Error "Failed to send http request";
						}
						if (response.rfind(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
							std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
							return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
						}
						else if (response.rfind(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
							std::cout << OT_ACTION_RETURN_INDICATOR_Error "From uiService: " << response << std::endl;
							return OT_ACTION_RETURN_INDICATOR_Error "From uiService: " + response;
						}
					}

					if (globalModel == nullptr) throw std::exception("No model created yet");
					globalModel->uiIsAvailable();
				}
			}
			else if (senderType == OT_INFO_SERVICE_TYPE_LOGGER) {
				// Change to logger service functionallity
			}
		}
		else if (action == OT_ACTION_CMD_UI_PromptResponse) {
			std::string response = doc[OT_ACTION_PARAM_RESPONSE].GetString();
			std::string answer = doc[OT_ACTION_PARAM_ANSWER].GetString();
			std::string parameter1 = doc[OT_ACTION_PARAM_PARAMETER1].GetString();

			if (globalModel == nullptr) throw std::exception("No model created yet");
			globalModel->promptResponse(response, answer, parameter1);
		}
		else if (action == OT_ACTION_CMD_ShutdownRequestedByService) {
			shutdown(false);
		}
		else if (action == OT_ACTION_CMD_Message) {
			std::string message = doc[OT_ACTION_PARAM_MESSAGE].GetString();
			std::string senderIP = doc[OT_ACTION_PARAM_SERVICE_URL].GetString();
			std::string senderName = doc[OT_ACTION_PARAM_SERVICE_NAME].GetString();
			std::string senderType = doc[OT_ACTION_PARAM_SERVICE_TYPE].GetString();
			ot::serviceID_t senderID = doc[OT_ACTION_PARAM_SERVICE_ID].GetUint();
			std::string msg("Message from \"");
			msg.append(senderName).append("\" (").append(senderIP).append("): ").append(message);
			std::cout << msg << std::endl;
		}
		else if (action == OT_ACTION_CMD_KeySequenceActivated) {
			std::string keySequence = doc[OT_ACTION_PARAM_UI_KeySequence].GetString();
			globalModel->keySequenceActivated(keySequence);
		}
		else if (action == OT_ACTION_CMD_MODEL_GET_ENTITY_IDENTIFIER)
		{
			const int numberOfIdentifier = ot::json::getInt(doc, OT_ACTION_PARAM_MODEL_ENTITY_IDENTIFIER_AMOUNT);
			ot::UIDList requestedUIDs, requestedVersions;
			for (int i = 0; i < numberOfIdentifier; i++)
			{
				requestedUIDs.push_back(globalModel->createEntityUID());
				requestedVersions.push_back(globalModel->createEntityUID());
			}
			const std::string& requestingService = ot::json::getString(doc, OT_ACTION_PARAM_SENDER_URL);
			const std::string& subsequentFunction = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_FunctionName);
			
			ot::JsonDocument replyDoc;
			replyDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteFunction, replyDoc.GetAllocator()), replyDoc.GetAllocator());
			replyDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(subsequentFunction, replyDoc.GetAllocator()), replyDoc.GetAllocator());
			replyDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(requestedUIDs, replyDoc.GetAllocator()), replyDoc.GetAllocator());
			replyDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(requestedVersions, replyDoc.GetAllocator()), replyDoc.GetAllocator());
			
			std::string response;
			//Currently only the ui service uses this function but the ui url is currently not available when this method is being invoked.
			sendHttpRequest(QUEUE, globalUIserviceURL, replyDoc, response);
		}
		else if (action == OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION)
		{
			std::string	collectionName = ot::json::getString(doc, OT_ACTION_PARAM_COLLECTION_NAME);
			std::string folder = ot::json::getString(doc, OT_ACTION_PARAM_Folder);
			std::string className = ot::json::getString(doc, OT_ACTION_PARAM_Type);
			bool recursive = ot::json::getBool(doc, OT_ACTION_PARAM_Recursive);
			
			std::string actualOpenedProject= DataBase::GetDataBase()->getProjectName();
			DataBase::GetDataBase()->setProjectName(collectionName);
			DataBase::GetDataBase()->RemovePrefetchedDocument(0);
			ModelState secondary(globalModel->getSessionCount(), globalModel->getServiceIDAsInt());
			secondary.openProject();

			std::list<ot::UID> prefetchIds;
			secondary.getListOfTopologyEntites(prefetchIds);
			std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;
			for (auto id : prefetchIds)
			{
				ot::UID entityID = id;
				ot::UID entityVersion = secondary.getCurrentEntityVersion(entityID);
				prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
			}
			DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
			ot::UIDList entityIDList, entityVersionList;
			for (auto& identifier : prefetchIdandVersion)
			{
				auto doc = bsoncxx::builder::basic::document{};
				if (!DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(identifier.first, identifier.second, doc))
				{
					return nullptr;
				}
				auto doc_view = doc.view()["Found"].get_document().view();
				std::string entityType = doc_view["SchemaType"].get_utf8().value.data();
				if (entityType == className)
				{
					entityIDList.push_back(identifier.first);
					entityVersionList.push_back(identifier.second);
				}
			}

			DataBase::GetDataBase()->setProjectName(actualOpenedProject);
			DataBase::GetDataBase()->RemovePrefetchedDocument(0);

			ot::JsonDocument newDoc;
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
			newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersionList, newDoc.GetAllocator()), newDoc.GetAllocator());
			
			return newDoc.toJson();
		}
		else if (action == OT_ACTION_PARAM_MODEL_ViewsForProjectType)
		{
			ProjectTypeManager typeManager(globalModel->getProjectType());

			return typeManager.getViews();
		}

		else if (action == OT_ACTION_CMD_ServiceEmergencyShutdown) {
#ifdef _DEBUG
		OutputDebugStringA("[OPEN TWIN] [DEBUG] Emergency shutdown received. exit(-100)");
#endif // _DEBUG
		exit(-100);
		}
		else
		{
			return OT_ACTION_RETURN_INDICATOR_Error "Unknown message";
		}

		return result;
	}
	catch (const std::exception & e) {
		std::string errorMsg{ "!!!   ERROR: " };
		errorMsg.append(e.what());
		std::cout << errorMsg << std::endl;
#ifdef _DEBUG
		errorMsg.append("\n");
		OutputDebugStringA(errorMsg.c_str());
#endif // DEBUG
		return std::string(OT_ACTION_RETURN_INDICATOR_Error) + e.what();
	}
	catch (...) {
#ifdef _DEBUG
		OutputDebugStringA("!!!   ERROR: Unknown error\n");
#endif // DEBUG
		std::cout << "!!!   ERROR: Unknown error" << std::endl;
		return OT_ACTION_RETURN_INDICATOR_Error "Unknown error";
	}
}

std::string MicroserviceAPI::getEntityInformation(std::list<ot::UID> &entityIDList)
{
	std::list<ot::UID> entityVersions;
	std::list<std::string> entityNames;
	std::list<std::string> entityTypes;

	globalModel->getEntityVersions(entityIDList, entityVersions);
	globalModel->getEntityNames(entityIDList, entityNames);
	globalModel->getEntityTypes(entityIDList, entityTypes);

	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityIDList, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(entityVersions, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityNameList, ot::JsonArray(entityNames, newDoc.GetAllocator()), newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityTypeList, ot::JsonArray(entityTypes, newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string MicroserviceAPI::CreateTmpFileFromCompressedData(const std::string &data, ot::UID uncompressedDataLength)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(data.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, data.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef *)decodedString, &destLen, (Bytef *)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	// Store the data in a temporary file
	std::string tmpFileName = DataBase::GetDataBase()->getTmpFileName();

	std::ofstream file(tmpFileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	return tmpFileName;
}

std::string MicroserviceAPI::getReturnJSONFromUID(ot::UID uid)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	ot::JsonDocument::AllocatorType& allocator = newDoc.GetAllocator();

	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_UID, uid, allocator);

	return newDoc.toJson();
}

std::string MicroserviceAPI::getReturnJSONFromString(std::string props)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_Props, rapidjson::Value(props.c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string MicroserviceAPI::getReturnJSONFromBool(bool flag)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	ot::JsonDocument::AllocatorType& allocator = newDoc.GetAllocator();

	newDoc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, flag, allocator);

	return newDoc.toJson();
}

std::string MicroserviceAPI::getReturnJSONFromUIDList(std::list<ot::UID> list)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	AddUIDListToJsonDoc(newDoc, OT_ACTION_PARAM_BASETYPE_List, list);

	return newDoc.toJson();
}

std::string MicroserviceAPI::getReturnJSONFromStringList(std::list<std::string> list)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	AddStringListToJsonDoc(newDoc, OT_ACTION_PARAM_BASETYPE_List, list);

	return newDoc.toJson();
}

ot::JsonDocument MicroserviceAPI::BuildJsonDocFromAction(const std::string &action)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	newDoc.AddMember(OT_ACTION_MEMBER, rapidjson::Value(action.c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc;
}

ot::JsonDocument MicroserviceAPI::BuildJsonDocFromString(std::string json)
{
	ot::JsonDocument doc;
	doc.Parse(json.c_str());
	assert(doc.IsObject());

	return doc;
}

void MicroserviceAPI::addTreeIconsToJsonDoc(ot::JsonDocument &doc, const TreeIcon &treeIcons)
{
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconSize, rapidjson::Value(treeIcons.size), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemVisible, rapidjson::Value(treeIcons.visibleIcon.c_str(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemHidden, rapidjson::Value(treeIcons.hiddenIcon.c_str(), doc.GetAllocator()), doc.GetAllocator());
}

void MicroserviceAPI::AddUIDListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName,const std::list<ot::UID> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(id, doc.GetAllocator());
	}

	doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), listUID, doc.GetAllocator());
}

void MicroserviceAPI::AddStringListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::list<std::string> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(rapidjson::Value(id.c_str(), doc.GetAllocator()), doc.GetAllocator());
	}

	doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), listUID, doc.GetAllocator());
}

void MicroserviceAPI::AddUIDVectorToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::vector<ot::UID> &list)
{
	rapidjson::Value listUID(rapidjson::kArrayType);
	for (auto id : list)
	{
		listUID.PushBack(id, doc.GetAllocator());
	}

	doc.AddMember(rapidjson::Value(itemName.c_str(), doc.GetAllocator()), listUID, doc.GetAllocator());
}

void MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector)
{
	ot::JsonArray vectorDouble;
	vectorDouble.Reserve(3 * (int) vector.size(), doc.GetAllocator());
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][0]), doc.GetAllocator());
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][1]), doc.GetAllocator());
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][2]), doc.GetAllocator());
	}

	doc.AddMember(ot::JsonString(name, doc.GetAllocator()), vectorDouble, doc.GetAllocator());
}

void MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(ot::JsonDocument &doc, const std::string &name, const double *doubleArray, int size)
{
	ot::JsonArray doubleArr;
	doubleArr.Reserve(size, doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(doubleArray[i], doc.GetAllocator());
	}

	doc.AddMember(ot::JsonString(name, doc.GetAllocator()), doubleArr, doc.GetAllocator());
}

std::vector<ot::UID> MicroserviceAPI::getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::vector<ot::UID> result;

	rapidjson::Value vector = doc[itemName.c_str()].GetArray();

	for (unsigned int i = 0; i < vector.Size(); i++)
	{
		result.push_back(vector[i].GetUint64());
	}

	return result;
}

void MicroserviceAPI::AddNodeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<Geometry::Node> &nodes)
{
	rapidjson::Value nodeList(rapidjson::kArrayType);
	nodeList.Reserve(8 * (int) nodes.size(), doc.GetAllocator());
	for (auto node : nodes) {
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(1)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(2)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(1)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(2)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getUVpar(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getUVpar(1)), doc.GetAllocator());
	}
	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), nodeList, doc.GetAllocator());
}

bool MicroserviceAPI::checkForUiConnection() {
	ot::JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetSessionServices, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(globalSessionID, request.GetAllocator()), request.GetAllocator());
	
	std::string response;
	if (!sendHttpRequest(EXECUTE, globalSessionServiceURL, request, response)) { return false; }
	OT_ACTION_IF_RESPONSE_ERROR(response) { return false; }
	else OT_ACTION_IF_RESPONSE_WARNING(response) { return false; }

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!responseDoc.HasMember(OT_ACTION_PARAM_SESSION_SERVICES)) {
		throw std::exception("The member \"" OT_ACTION_PARAM_SESSION_SERVICES "\" is missing");
	}
	ot::ConstJsonObjectList services = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_SESSION_SERVICES);
	for (auto service : services)
	{
		if (service.HasMember(OT_ACTION_PARAM_SERVICE_TYPE)) {
			std::string type = service[OT_ACTION_PARAM_SERVICE_TYPE].GetString();
			if (type == OT_INFO_SERVICE_TYPE_UI) {
				if (service.HasMember(OT_ACTION_PARAM_SERVICE_NAME) &&
					service.HasMember(OT_ACTION_PARAM_SERVICE_URL)) {

					std::string name = service[OT_ACTION_PARAM_SERVICE_NAME].GetString();
					std::string ip = service[OT_ACTION_PARAM_SERVICE_URL].GetString();
					std::cout << "UI found with {\"name\":\"" << name << "\", \"ip\":\"" << ip << "\"}" << std::endl;
					globalUIserviceURL = ip;
					registerAtUI();
					return true;
				}
			}
		}
	}
	return false;
}

bool MicroserviceAPI::registerAtUI() {

	ot::JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RegisterForModelEvents, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_PORT, ot::JsonString(ot::IpConverter::portFromIp(globalServiceURL), request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_RegisterForModelEvents, true, request.GetAllocator());
	
	std::string response;
	if (!sendHttpRequest(QUEUE, globalUIserviceURL, request.toJson(), response)) {
		std::cout << "ERROR: Failed to register at UI: Failed to send HTTP request" << std::endl;
		return false;
	}

	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to register at UI: " << response << std::endl;
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to register at UI: " << response << std::endl;
		return false;
	}
	return true;
}

bool MicroserviceAPI::deregisterAtUI() {

	ot::JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_DeregisterForModelEvents, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, request.GetAllocator());

	std::string response;
	if (!sendHttpRequest(QUEUE, globalUIserviceURL, request.toJson(), response)) {
		std::cout << "ERROR: Failed to register at UI: Failed to send HTTP request" << std::endl;
		return false;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to deregister at UI: " << response << std::endl;
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to deregister at UI: " << response << std::endl;
		return false;
	}
	return true;
}

void MicroserviceAPI::AddEdgeToJsonDoc(ot::JsonDocument &doc, const std::string &name,const std::list<Geometry::Edge> &edges)
{
	int elements = 0;

	for (auto edge : edges)
	{
		elements += 2 + 3 * edge.getNpoints();
	}
	rapidjson::Value edgeList(rapidjson::kArrayType);
	edgeList.Reserve(elements+2, doc.GetAllocator());
	edgeList.PushBack(rapidjson::Value().SetInt64(edges.size()), doc.GetAllocator());  // Store the number of edges
	for (auto edge : edges)
	{
		edgeList.PushBack(rapidjson::Value().SetInt64(edge.getFaceId()), doc.GetAllocator());   // For each edge store the corresponding faceId first
		edgeList.PushBack(rapidjson::Value().SetInt64(edge.getNpoints()), doc.GetAllocator());   // For each edge store the number of points 
		for (int i = 0; i < edge.getNpoints(); i++) {
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getX()), doc.GetAllocator());   // Then store the points one by one
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getY()), doc.GetAllocator());
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getZ()), doc.GetAllocator());
		}
	}

	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), edgeList, doc.GetAllocator());
}

void MicroserviceAPI::AddTriangleToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles)
{
	rapidjson::Value triangleArr(rapidjson::kArrayType);
	triangleArr.Reserve(4 * (int) triangles.size(), doc.GetAllocator());
	for (auto triangle : triangles) {
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(0)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(1)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(2)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getFaceId()), doc.GetAllocator());
		
	}
	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), triangleArr, doc.GetAllocator());
}

void MicroserviceAPI::shutdown(bool _receivedAsCommand) {
	if (!_receivedAsCommand) {
		ot::JsonDocument commandDoc;
		commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceClosing, commandDoc.GetAllocator()), commandDoc.GetAllocator());
		commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, globalServiceID, commandDoc.GetAllocator());
		commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(globalSessionID, commandDoc.GetAllocator()), commandDoc.GetAllocator());

		std::string response;
		if (!sendHttpRequest(QUEUE, globalSessionServiceURL, commandDoc.toJson(), response)) {
			assert(0); // Failed to send http request
		}
	}
	exit(0);
}

std::string MicroserviceAPI::getUIURL(void) 
{ 
	return globalUIserviceURL; 
}

std::string MicroserviceAPI::sendMessageToService(RequestType operation, const std::string &owner, ot::JsonDocument &doc)
{
	getSessionServices();
	std::string response;

	if (globalServiceURLs.count(owner) > 0)
	{
		if (!sendHttpRequest(operation, globalServiceURLs[owner], doc.toJson(), response)) {
			assert(0); // Failed to send http request
		}
	}

	return response;
}

void MicroserviceAPI::getSessionServices(void)
{
	if (globalServiceURLsRetrieved) return;

	// Create the request doc
	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetSessionServices, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(globalSessionID, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	
	// Send the command
	std::string response;
	if (!ot::msg::send(globalServiceURL, globalSessionServiceURL, ot::EXECUTE, commandDoc.toJson(), response)) {
		std::cout << "ERROR: Failed to get session information: Failed to send HTTP request" << std::endl;
		return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::cout << "ERROR: Failed to get session information: " << response << std::endl;
		return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::cout << "ERROR: Failed to get session information: " << response << std::endl;
		return;
	}

	// Parse and check the document (it will be checked if the received document is an object)
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	// Check if the received document contains the required information
	if(!responseDoc.HasMember(OT_ACTION_PARAM_SESSION_SERVICES)) {
		std::cout << "ERROR: The member \"" OT_ACTION_PARAM_SESSION_SERVICES "\" is missing";
		return;
	}

	// Get information and iterate trough entries
	ot::ConstJsonObjectList services = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_SESSION_SERVICES);
	for (auto service : services)
	{
		std::string name = service[OT_ACTION_PARAM_SERVICE_NAME].GetString();
		std::string ip = service[OT_ACTION_PARAM_SERVICE_URL].GetString();
		std::string type;

		if (service.HasMember(OT_ACTION_PARAM_SERVICE_TYPE))
		{
			type = service[OT_ACTION_PARAM_SERVICE_TYPE].GetString();
		}

		if (type != OT_INFO_SERVICE_TYPE_UI)
		{
			if (name != getServiceName())
			{
				globalServiceURLs[name] = ip;
			}
		}
	}

	globalServiceURLsRetrieved = true;
}

