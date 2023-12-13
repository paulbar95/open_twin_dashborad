#pragma once
//Service Header

// Open twin header
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"

// C++ header
#include <string>
#include <memory>

class BlockEntityHandler : public BusinessLogicHandler
{
public:
	void CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void OrderUIToCreateBlockPicker();
private:
	const std::string _blockFolder = ot::FolderNames::BlockFolder;
	const std::string _packageName = "Circuit Simulator";

	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();

};


