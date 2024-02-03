/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCommunication/actionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_STUDIOSUITE

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_STUDIOSUITE

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	// This method is called once the service can start its operation
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED)
	{
		uploadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_FILES_UPLOADED)
	{
		filesUploaded(_doc);
		return "";
	}

	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Project");
	
	_ui->addMenuGroup("Project", "Import");

	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	_ui->addMenuButton("Project", "Import", "CST File", "CST File", modelWrite, "Import", "Default");

	modelSelectionChangedNotification();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "Project:Import:CST File")			  importProject();
	//else if (action == "ElmerFEM:Solver:Run Solver")		  runSolver();
	//else if (action == "Model:Sources:Add Terminal")	      addTerminal();
	//else if (action == "ElmerFEM:Sources:Define Electrostatic Potential")  definePotential();
	else assert(0); // Unhandled button action
	return std::string();
}

std::string Application::handleModelSelectionChanged(ot::JsonDocument& _document) {
	selectedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	modelSelectionChangedNotification();
	return std::string();
}

void Application::modelSelectionChangedNotification(void)
{
	//if (isUiConnected()) {
	//	std::list<std::string> enabled;
	//	std::list<std::string> disabled;

	//	if (selectedEntities.size() > 0)
	//	{
	//		enabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}
	//	else
	//	{
	//		disabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}

	//	m_uiComponent->setControlsEnabledState(enabled, disabled);
	//}
}

void Application::EnsureVisualizationModelIDKnown(void)
{
	if (visualizationModelID > 0) return;
	if (m_modelComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// The visualization model isnot known yet -> get it from the model
	visualizationModelID = m_modelComponent->getCurrentVisualizationModelID();
}


void Application::importProject(void)
{
	// TODO: Check whether the project has already been initialized
	 
	
	// Send the import message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_IMPORT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::uploadNeeded(ot::JsonDocument& _doc)
{
	size_t count = ot::json::getInt64(_doc, OT_ACTION_PARAM_COUNT);

	ot::UIDList entityID, versionID;

	for (size_t i = 0; i < count; i++)
	{
		entityID.push_back(m_modelComponent->createEntityUID());
		versionID.push_back(m_modelComponent->createEntityUID());
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	
	uiComponent()->sendMessage(true, doc);
}

void Application::filesUploaded(ot::JsonDocument& _doc)
{
	std::string changeMessage = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);

	std::list<std::string> modifiedNameList = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
	std::list<ot::UID> fileEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityID);
	std::list<ot::UID> fileVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersion);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataID);
	std::list<ot::UID> dataVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataVersion);

	std::list<std::string> deletedNameList = ot::json::getStringList(_doc, OT_ACTION_CMD_MODEL_DeleteEntity);

	// Now we need to send the model change to the model service 

	modelComponent()->clearNewEntityList();

	for (auto item : modifiedNameList)
	{
		ot::UID fileEntityID = fileEntityIDList.front(); fileEntityIDList.pop_front();
		ot::UID fileVersion  = fileVersionList.front();  fileVersionList.pop_front();
		ot::UID dataEntityID = dataEntityIDList.front(); dataEntityIDList.pop_front();
		ot::UID dataVersion  = dataVersionList.front();  dataVersionList.pop_front();

		modelComponent()->addNewTopologyEntity(fileEntityID, fileVersion, false);
		modelComponent()->addNewDataEntity(dataEntityID, dataVersion, fileEntityID);
	}

	modelComponent()->deleteEntitiesFromModel(deletedNameList, false);

	m_modelComponent->storeNewEntities(changeMessage);

	// Determine the new version
	std::string newVersion = m_modelComponent->getCurrentModelVersion();

	// Finally we send the new version to the frontend 
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_COPY, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}