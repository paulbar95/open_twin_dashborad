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
#include "OTCore/ReturnMessage.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "TemplateDefaultManager.h"

#include "ClassFactory.h"
#include "ExternalDependencies.h"
#include "ClassFactoryBlock.h"

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
#include "EntityCoordinates2D.h"
#include "DataBase.h"
#include "ResultCollectionAccess.h"

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
		if (action == _buttonRunPipeline.GetFullDescription())
		{
			auto allBlockEntities = _blockEntityHandler.findAllBlockEntitiesByBlockID();
			const bool isValid = _validityHandler.blockDiagramIsValid(allBlockEntities);
			if (isValid)
			{
				const std::list<std::shared_ptr<GraphNode>>& rootNodes= _validityHandler.getRootNodes();
				const std::map<std::string, std::shared_ptr<GraphNode>> graphNodesByBlockID = _validityHandler.getgraphNodesByBlockID();
				_pipelineHandler.RunAll(rootNodes, graphNodesByBlockID,allBlockEntities);
			}
		}
	}
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		assert(m_selectedEntities.size() == 1);
		std::list<ot::EntityInformation> entityInfos;
		m_modelComponent->getEntityInformation(m_selectedEntities, entityInfos);
		ClassFactoryBlock classFactory;
		auto entBase =	m_modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), classFactory);
		auto dbAccess =	std::shared_ptr<EntityBlockDatabaseAccess>(dynamic_cast<EntityBlockDatabaseAccess*>(entBase));
		if (dbAccess != nullptr)
		{
			auto modelService = instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_MODEL);
			PropertyHandlerDatabaseAccessBlock::instance().PerformUpdateIfRequired(dbAccess, instance()->sessionServiceURL(), modelService->serviceURL());
		}
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem)
	{
		std::string itemName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
		std::string editorName = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
		
		ot::Point2DD position;
		position.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));
		
		//Needs to be set once but modelConnect event cannot be used currently, since the modelstate in that point in time is a dummy.
		ExternalDependencies dependencies;
		if (dependencies.getPythonScriptFolderID() == 0)
		{
			ot::EntityInformation entityInfo;
			m_modelComponent->getEntityInformation("Scripts", entityInfo);
			ExternalDependencies dependencies;
			dependencies.setPythonScriptFolderID(entityInfo.getID());
		}

		_blockEntityHandler.CreateBlockEntity(editorName, itemName,position);
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection)
	{
		ot::GraphicsConnectionPackage pckg;
		pckg.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

		_blockEntityHandler.AddBlockConnection(pckg.connections());
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemMoved)
	{
		const std::string blockID = ot::json::getString(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId);
		ot::Point2DD position;
		position.setFromJsonObject(ot::json::getObject(_doc, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));
		_blockEntityHandler.UpdateBlockPosition(blockID,position,&getClassFactory());
	}
	else if (_action == OT_ACTION_CMD_UI_GRAPHICSEDITOR_SelectionChanged)
	{
		
	}

	return ""; // Return empty string if the request does not expect a return
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
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
	_blockEntityHandler.setUIComponent(_ui);
	_blockEntityHandler.OrderUIToCreateBlockPicker();
	
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	_validityHandler.setUIComponent(_ui);
	_pipelineHandler.setUIComponent(_ui);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_blockEntityHandler.setModelComponent(_model);
	_pipelineHandler.setModelComponent(_model);
	
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