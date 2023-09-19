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
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"
#include "TemplateDefaultManager.h"


#include "BlockItemManager.h"
#include "ClassFactory.h"
#include "ExternalDependencies.h"

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_DataProcessingService, OT_INFO_SERVICE_TYPE_DataProcessingService, new UiNotifier(), new ModelNotifier())
{
	
}

Application::~Application()
{

}

// ##################################################################################################################################################################################################################

// Custom functions



// ##################################################################################################################################################################################################################

// Required functions
#include "TemplateDefaultManager.h"
#include "ResultCollectionHandler.h"

void Application::run(void)
{
	if (!EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}

	// Add code that should be executed when the service is started and may start its work
}

#include "BlockEntityHandler.h"
#include "BlockHandlerDatabaseAccess.h"
#include "BlockHandlerPlot1D.h"
#include "MeasurementCampaignHandler.h"

#include "DataBase.h"
#include "CrossCollectionAccess.h"
#include "MeasurementCampaignFactory.h"

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonRunPipeline.GetFullDescription())
		{
			//_pipelineManager.RunAll();
			//std::unique_ptr<EntityBlockDatabaseAccess> block(new EntityBlockDatabaseAccess(m_modelComponent->createEntityUID(),nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
			//std::list<std::string>projects{"KWT_Demo"};
			//block->createProperties(projects, *projects.begin());
			
			//BlockHandlerDatabaseAccess dbAccess(block.get());
			//BlockHandler::genericDataBlock parameter, result;
			//result = dbAccess.Execute(parameter);

			//std::unique_ptr<EntityBlockDatabaseAccess> block(new EntityBlockDatabaseAccess(m_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
			//BlockHandlerPlot1D plot;
			//plot.Execute(result);
		}
	}
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		assert(m_selectedEntities.size() == 1);
		std::list<ot::EntityInformation> entityInfos;
		m_modelComponent->getEntityInformation(m_selectedEntities, entityInfos);
		ClassFactory classFactory;
		auto entBase =	m_modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), classFactory);
		auto dbAccess =	dynamic_cast<EntityBlockDatabaseAccess*>(entBase);
		if (dbAccess != nullptr)
		{
			const std::string queryDimension = dbAccess->getQueryDimension();
			const std::string projectName = dbAccess->getSelectedProjectName();
			
			auto modelService = instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_MODEL);
			CrossCollectionAccess access(projectName, instance()->sessionServiceURL(), modelService->serviceURL());
			if (access.ConnectedWithCollection())
			{
				auto rmd = access.getMeasurementCampaignMetadata(m_modelComponent);
				auto msmds = access.getMeasurementMetadata(m_modelComponent);

				MeasurementCampaignFactory factory;
				MeasurementCampaign measurementCampaign = factory.Create(rmd, msmds);
				std::map <std::string, MetadataQuantity> quantities = measurementCampaign.getMetadataQuantities();
				std::list<std::string> quantityNames;
				for (auto& quantity : quantities)
				{
					quantityNames.push_back(quantity.first);
				}
				std::list<std::string> msmdNames;
				for (auto& msmd : msmds)
				{
					msmdNames.push_back(msmd->getName());
				}
				EntityProperties properties;
				EntityPropertiesSelection::createProperty("Query Specification", "Measurement Series", msmdNames, "", "default", properties);
				EntityPropertiesSelection::createProperty("Query Specification", "Quantity", quantityNames, "", "default", properties);

				OT_rJSON_createDOC(requestDoc);
				ot::rJSON::add(requestDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities);
				ot::UIDList ids{ dbAccess->getEntityID() };
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_EntityIDList, ids);
				ot::rJSON::add(requestDoc, OT_ACTION_PARAM_MODEL_JSON, properties.getJSON(nullptr, true));

				sendMessage(true, OT_INFO_SERVICE_TYPE_MODEL, requestDoc);
			}	
		}
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDropped)
	{
		// Get item information
		std::string itemName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
		std::string editorName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);	
		
		// Generate UID
		
		std::string itemUid = "Some fancy new UID"; // <----- Add code here to generate the new UID
		
		ExternalDependencies dependencies;
		if (dependencies.getPythonScriptFolderID() == 0)
		{
			ot::EntityInformation entityInfo;
			m_modelComponent->getEntityInformation("Scripts", entityInfo);
			ExternalDependencies dependencies;
			dependencies.setPythonScriptFolderID(entityInfo.getID());
		}

		//                                                                                     \/     This value was the UID send from the UI before. Instead of 0 the new UID should be passed
		auto blockEntity = BlockEntityHandler::GetInstance().CreateBlock(editorName, itemName, 0);
		if (blockEntity != nullptr)
		{
			ot::UIDList topoEntID{ blockEntity->getEntityID() }, topoEntVers{ blockEntity->getEntityStorageVersion() }, dataEnt{};
			std::list<bool> forceVis{false,false};
			m_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEnt, dataEnt, dataEnt, "Added Block: " + itemName);
		}

		OT_rJSON_createDOC(argDoc);
		ot::rJSON::add(argDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId, itemUid);
		
		ot::ReturnMessage response(ot::ReturnMessage::Ok, ot::rJSON::toJSON(argDoc));
		return response.toJson();

	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionDropped)
	{
		ot::UID uidOrigin = ot::rJSON::getULongLong(_doc,OT_ACTION_PARAM_GRAPHICSEDITOR_OriginId);
		ot::UID uidDestination = ot::rJSON::getULongLong(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_DestId);
		std::string connectorNameOrigin = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_OriginConnetableName);
		std::string connectorNameDestination = ot::rJSON::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_DestConnetableName);

		BlockEntityHandler::GetInstance().AddBlockConnection(uidOrigin, uidDestination, connectorNameOrigin, connectorNameDestination);
	}

	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	const std::string pageName = "Data Processing";
	const std::string groupName = "Pipeline Handling";
	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	_ui->addMenuPage(pageName);
	_ui->addMenuGroup(pageName, groupName);
	_buttonRunPipeline.SetDescription(pageName, groupName, "Run");
	_ui->addMenuButton(_buttonRunPipeline, modelWrite, "Kriging");

	BlockItemManager blockItemManger;
	blockItemManger.OrderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	 BlockEntityHandler::GetInstance().setModelComponent(_model);
	_pipelineManager.setModelComponent(_model);
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

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

// ##################################################################################################################################################################################################################