#include "BlockEntityHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "ExternalDependencies.h"

#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"


void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName,ot::Point2DD& position)
{
	ClassFactoryBlock factory;
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);		
	std::shared_ptr<EntityBlock> blockEntity (dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(_blockFolder+"/"+ editorName, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_DataProcessingService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->setBlockID(_modelComponent->createEntityUID());
	blockEntity->SetGraphicsScenePackageName(_packageName);
	
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	InitSpecialisedBlockEntity(blockEntity);

	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);
}

void BlockEntityHandler::AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();

	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	for (auto& connection : connections)
	{
		if (blockEntitiesByBlockID.find(connection.originUid()) != blockEntitiesByBlockID.end())
		{
			blockEntitiesByBlockID[connection.originUid()]->AddConnection(connection);
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.originUid()]);
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + connection.originUid() + " was not found");
		}
		if (blockEntitiesByBlockID.find(connection.destUid()) != blockEntitiesByBlockID.end())
		{
			blockEntitiesByBlockID[connection.destUid()]->AddConnection(connection);
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.destUid()]);
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + connection.destUid() + " was not found");
		}
	}

	if (entitiesForUpdate.size() != 0)
	{
		ot::UIDList topoEntIDs, topoEntVers;

		for (auto entityForUpdate : entitiesForUpdate)
		{
			entityForUpdate->StoreToDataBase();
			topoEntIDs.push_back(entityForUpdate->getEntityID());
			topoEntVers.push_back(entityForUpdate->getEntityStorageVersion());
		}
		_modelComponent->updateTopologyEntities(topoEntIDs, topoEntVers, "Added Connection(s) to BlockEntity(ies).");
	}
}

void BlockEntityHandler::OrderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(pckgObj);
	graphicsEditorPackage->addToJsonObject(doc, pckgObj);

	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor);
	ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc);

	// Message is queued, no response here
	_uiComponent->sendMessage(true, doc);
}

void BlockEntityHandler::UpdateBlockPosition(const std::string& blockID, ot::Point2DD& position)
{
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();
	if (blockEntitiesByBlockID.find(blockID) == blockEntitiesByBlockID.end())
	{
		OT_LOG_EAS("Position of block item cannot be updated because a block with id: " + blockID + " was not found");
	}
	auto blockEntity = blockEntitiesByBlockID[blockID];
	std::list<ot::EntityInformation> entityInfos;
	ot::UIDList entityList{ blockEntity->getCoordinateEntityID() };
	_modelComponent->getEntityInformation(entityList, entityInfos);
	ClassFactory classFactory;
	auto entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), classFactory);
	std::unique_ptr<EntityCoordinates2D> coordinateEnt(dynamic_cast<EntityCoordinates2D*>(entBase));
	coordinateEnt->setCoordinates(position);
	coordinateEnt->StoreToDataBase();
	_modelComponent->addEntitiesToModel({}, {}, {}, { coordinateEnt->getEntityID() }, { coordinateEnt->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Update BlockItem position");
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
		dbaBlock->createProperties();
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

	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	//pckg->addCollection(mathBlockCollection);

	return pckg;
}

std::map<std::string, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID()
{
	std::list<std::string> blockItemNames = _modelComponent->getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<std::string, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr) //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others
		{
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			assert(blockEntity != nullptr);
			blockEntitiesByBlockID[blockEntity->getBlockID()] = blockEntity;
		}
	}
	return blockEntitiesByBlockID;
}
