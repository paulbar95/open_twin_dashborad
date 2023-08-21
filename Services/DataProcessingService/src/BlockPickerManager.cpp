#include "BlockPickerManager.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

#include "BlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCommunication/Msg.h"
#include "Application.h"
#include "OpenTwinFoundation/UiComponent.h"

void BlockPickerManager::OrderUIToCreateBlockPicker()
{

	auto graphicsEditorPackage = BuildUpBlockPicker();
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(pckgObj);
	graphicsEditorPackage->addToJsonObject(doc, pckgObj);

	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
	ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
	ot::GlobalOwner::instance().addToJsonObject(doc, doc);

	std::string response;
	std::string req = ot::rJSON::toJSON(doc);
	
	auto uiComponent = Application::instance()->uiComponent();
	if (!ot::msg::send("", uiComponent->serviceURL(), ot::QUEUE, req, response)) {
		assert(0);
	}

	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Invalid response from UI");
		uiComponent->displayDebugMessage("Invalid response\n");
	}
}

ot::GraphicsEditorPackage* BlockPickerManager::BuildUpBlockPicker()
{
	ot::GraphicsEditorPackage* pckg = new ot::GraphicsEditorPackage("Data Processing", "Data Processing");
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");

	ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	BlockDatabaseAccess dbA("BlockDatabaseAccess");

	controlBlockDatabaseCollection->addItem(dbA.GetBlock());

	//a2->addItem(createTestBlock2("Alpha 3"));
	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	pckg->addCollection(mathBlockCollection);

	return pckg;
}
