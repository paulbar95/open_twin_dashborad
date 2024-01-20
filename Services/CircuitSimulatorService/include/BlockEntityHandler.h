#pragma once
//Service Header
#include "NGSpice.h"

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
	std::map<std::string, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections,std::string name);
	NGSpice m_ngSpice;
private:
	const std::string _blockFolder = ot::FolderNames::BlockFolder;
	const std::string _packageName = "Circuit Simulator";

	void InitSpecialisedCircuitElementEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsNewEditorPackage* BuildUpBlockPicker();
	

};


