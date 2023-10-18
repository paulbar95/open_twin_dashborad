#include "EntityBlockPython.h"
#include "OpenTwinCommunication/ActionTypes.h"


EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner), 
	_colourTitle(ot::Color::Blue), _colourBackground(ot::Color::White)
{
}

void EntityBlockPython::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "python";
		treeIcons.hiddenIcon = "python";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);

		std::map<ot::UID, EntityBase*> entityMap;
		EntityBase* entBase = readEntityFromEntityID(this, _coordinate2DEntityID, entityMap);
		std::unique_ptr<EntityCoordinates2D> entCoordinate( dynamic_cast<EntityCoordinates2D*>(entBase));
		if(entCoordinate->getObserver() != nullptr){ entCoordinate->setObserver(nullptr); }
			

		ot::GraphicsItemCfg* blockCfg = CreateBlockCfg();
		blockCfg->setUid(std::to_string(_blockID));
		blockCfg->setPosition(entCoordinate->getCoordinates());
		
		ot::GraphicsScenePackage pckg(_graphicsScenePackage);
		pckg.addItem(blockCfg);

		OT_rJSON_createDOC(reqDoc);
		ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);
		_owner.addToJsonObject(reqDoc, reqDoc);

		OT_rJSON_createValueObject(pckgDoc);
		pckg.addToJsonObject(reqDoc, pckgDoc); 
		ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgDoc);

		getObserver()->sendMessageToViewer(reqDoc);


		ot::GraphicsConnectionPackage connectionPckg;

		// Store connection information
		for (auto& connection : _outgoingConnections) {
			ot::GraphicsConnectionPackage::ConnectionInfo connectionItem;
			connectionItem.fromConnectable = connection.getConnectorOrigin();
			connectionItem.toConnectable = connection.getConnectorDestination();
			connectionItem.fromUID = std::to_string(getBlockID());
			connectionItem.toUID = std::to_string( connection.getIDDestination());
		}

		// Request UI to add connections
		OT_rJSON_createDOC(connectionReqDoc);
		ot::rJSON::add(connectionReqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);

		OT_rJSON_createValueObject(reqConnectionPckgObj);
		connectionPckg.addToJsonObject(connectionReqDoc, reqConnectionPckgObj);
		ot::rJSON::add(connectionReqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqConnectionPckgObj);
	}
}

void EntityBlockPython::createProperties(const std::string& scriptFolder, ot::UID scriptFolderID)
{
	EntityPropertiesEntityList::createProperty("Script properties", "Script", scriptFolder, scriptFolderID, "", -1, "default", getProperties());
}

std::string EntityBlockPython::getSelectedScript()
{
	auto propBase = getProperties().getProperty("Script");
	auto scriptSelection = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	assert(scriptSelection != nullptr);

	return scriptSelection->getValueName();
}

ot::GraphicsItemCfg* EntityBlockPython::CreateBlockCfg()
{
	ot::GraphicsFlowItemCfg* block = new ot::GraphicsFlowItemCfg;

	block->setTitleBackgroundColor(_colourTitle.rInt(), _colourTitle.gInt(), _colourTitle.bInt());
	block->setBackgroundColor(_colourBackground.rInt(), _colourBackground.gInt(), _colourBackground.gInt());

	block->addLeft("C0", "Parameter", ot::GraphicsFlowConnectorCfg::Square);
	block->addRight("C0", "Output", ot::GraphicsFlowConnectorCfg::Square);

	return block->createGraphicsItem("Python", "Python");
}
