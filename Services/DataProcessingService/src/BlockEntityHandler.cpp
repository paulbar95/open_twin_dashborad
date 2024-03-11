#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "ExternalDependencies.h"

#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"
#include "AdvancedQueryBuilder.h"
#include "EntityBlockDataDimensionReducer.h"
#include "EntityBlockStorage.h"
#include "EntityBlockConnection.h"
#include "EntityBlockDisplay.h"

void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
{
	ClassFactory& factory = Application::instance()->getClassFactory();
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);		
	std::shared_ptr<EntityBlock> blockEntity (dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(_blockFolder+"/"+ editorName, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->SetGraphicsScenePackageName(_packageName);
	
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	InitSpecialisedBlockEntity(blockEntity);

	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);
}

void BlockEntityHandler::AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections, const std::string& editorName)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();

	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	ot::UIDList topoEntIDs, topoEntVers;
	const std::string connectionFolderName = _blockFolder + "/" + editorName + "/" + _connectionFolder;
	for (auto& connection : connections)
	{
		EntityBlockConnection connectionEntity(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService);
		connectionEntity.createProperties();
		ot::GraphicsConnectionCfg newConnection(connection);
		newConnection.setUid(connectionEntity.getEntityID());
		newConnection.setStyle(ot::GraphicsConnectionCfg::SmoothLine);
		const std::string connectionName = CreateNewUniqueTopologyName(connectionFolderName, "Connection", 1, false);
		connectionEntity.setName(connectionName);
		connectionEntity.setConnectionCfg(newConnection);
		connectionEntity.SetServiceInformation(Application::instance()->getBasicServiceInformation());
		connectionEntity.setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
		connectionEntity.SetGraphicsScenePackageName(_packageName);
		connectionEntity.createProperties();

		connectionEntity.StoreToDataBase();
		topoEntIDs.push_back(connectionEntity.getEntityID());
		topoEntVers.push_back(connectionEntity.getEntityStorageVersion());
		
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		if (blockEntitiesByBlockID.find(newConnection.getOriginUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getOriginUid()];
			
			originConnectorIsTypeOut = connectorHasTypeOut(blockEntity, newConnection.originConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getOriginUid()) + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(newConnection.getDestinationUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getDestinationUid()];
			destConnectorIsTypeOut = connectorHasTypeOut(blockEntity, newConnection.destConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getDestinationUid()) + " was not found.");
			continue;
		}

		if (originConnectorIsTypeOut != destConnectorIsTypeOut)
		{
			blockEntitiesByBlockID[newConnection.getOriginUid()]->AddConnection(newConnection.getUid());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[newConnection.getOriginUid()]);
			blockEntitiesByBlockID[newConnection.getDestinationUid()]->AddConnection(newConnection.getUid());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[newConnection.getDestinationUid()]);
		}
		else
		{
			_uiComponent->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
		}
	}

	if (entitiesForUpdate.size() != 0)
	{
		for (auto entityForUpdate : entitiesForUpdate)
		{
			entityForUpdate->StoreToDataBase();
			topoEntIDs.push_back(entityForUpdate->getEntityID());
			topoEntVers.push_back(entityForUpdate->getEntityStorageVersion());
		}
		_modelComponent->updateTopologyEntities(topoEntIDs, topoEntVers, "Added new connection to BlockEntities.");
	}
}

void BlockEntityHandler::OrderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	_uiComponent->sendMessage(true, doc);
}

void BlockEntityHandler::UpdateBlockPosition(const ot::UID& blockID, ot::Point2DD& position, ClassFactory* classFactory)
{
	std::list<ot::EntityInformation> entityInfos;
	ot::UIDList entityIDList{ blockID };
	_modelComponent->getEntityInformation(entityIDList, entityInfos);
	auto entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), *classFactory);
	std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	
	ot::UID positionID = blockEnt->getCoordinateEntityID();
	entityInfos.clear();
	entityIDList = { positionID };
	_modelComponent->getEntityInformation(entityIDList, entityInfos);
	entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), *classFactory);
	std::unique_ptr<EntityCoordinates2D> coordinateEnt(dynamic_cast<EntityCoordinates2D*>(entBase));
	coordinateEnt->setCoordinates(position);
	coordinateEnt->StoreToDataBase();
	_modelComponent->addEntitiesToModel({}, {}, {}, { coordinateEnt->getEntityID() }, { coordinateEnt->getEntityStorageVersion() }, { blockID }, "Update BlockItem position");
}

void BlockEntityHandler::InitSpecialisedBlockEntity(std::shared_ptr<EntityBlock> blockEntity)
{
	EntityBlockPython* pythonBlock = dynamic_cast<EntityBlockPython*>(blockEntity.get());
	if (pythonBlock != nullptr)
	{
		ExternalDependencies dependency;
		pythonBlock->createProperties(ot::FolderNames::PythonScriptFolder, dependency.getPythonScriptFolderID());
		return;
	}
	EntityBlockPlot1D* plotBlock = dynamic_cast<EntityBlockPlot1D*>(blockEntity.get());
	if (plotBlock != nullptr)
	{
		plotBlock->createProperties();
	}

	EntityBlockDatabaseAccess* dbaBlock = dynamic_cast<EntityBlockDatabaseAccess*>(blockEntity.get());
	if (dbaBlock != nullptr)
	{
		auto comparators = AdvancedQueryBuilder::getComparators();
		comparators.push_back(getQueryForRangeSelection());
		comparators.push_back(" ");
		dbaBlock->createProperties(comparators);
	}

	EntityBlockDataDimensionReducer* dataAR = dynamic_cast<EntityBlockDataDimensionReducer*>(blockEntity.get());
	if (dataAR)
	{
		dataAR->createProperties();
	}

	EntityBlockStorage* storage = dynamic_cast<EntityBlockStorage*>(blockEntity.get());
	if (storage)
	{
		storage->createProperties();
	}

	EntityBlockDisplay* display = dynamic_cast<EntityBlockDisplay*>(blockEntity.get());
	if(display)
	{
		display->createProperties();
	}
}

ot::GraphicsNewEditorPackage* BlockEntityHandler::BuildUpBlockPicker()
{
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");

	//ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");
	

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	EntityBlockPython pythonBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	customizedBlockCollection->addItem(pythonBlock.CreateBlockCfg());

	EntityBlockDatabaseAccess dbAccessBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(dbAccessBlock.CreateBlockCfg());

	EntityBlockPlot1D plotBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockVisualizationCollection->addItem(plotBlock.CreateBlockCfg());

	EntityBlockDisplay displayBlock(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockVisualizationCollection->addItem(displayBlock.CreateBlockCfg());

	EntityBlockDataDimensionReducer dimensionReducer(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(dimensionReducer.CreateBlockCfg());

	EntityBlockStorage storage(0, nullptr, nullptr, nullptr, nullptr, "");
	controlBlockDatabaseCollection->addItem(storage.CreateBlockCfg());

	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	//pckg->addCollection(mathBlockCollection);

	return pckg;
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID()
{
	std::list<std::string> blockItemNames = _modelComponent->getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	
	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), Application::instance()->getClassFactory());
		if (baseEntity != nullptr) //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others
		{
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			if (blockEntity != nullptr)
			{
				blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
			}
		}
	}
	return blockEntitiesByBlockID;
}

bool BlockEntityHandler::connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName)
{
	auto allConnectors = blockEntity->getAllConnectorsByName();
	const ot::ConnectorType connectorType = allConnectors[connectorName].getConnectorType();
	if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS("Unset connectortype of connector: " + allConnectors[connectorName].getConnectorName()); }
	if (connectorType == ot::ConnectorType::In || connectorType == ot::ConnectorType::InOptional)
	{
		return false;
	}
	else
	{
		return true;
	}
}
