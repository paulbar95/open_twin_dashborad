#pragma
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "OpenTwinCore/FolderNames.h"
#include "EntityBlock.h"
#include "Graph.h"

#include<map>

class ValidityHandler : public BusinessLogicHandler
{
public:
	bool blockDiagramIsValid(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);

	const std::map<std::string, std::shared_ptr<GraphNode>>& getgraphNodesByBlockID() const { return _graphNodeByBlockID; };
	const std::list < std::shared_ptr<GraphNode>> getRootNodes() const { return _rootNodes; }

private:
	std::string _blockFolder = ot::FolderNames::BlockFolder;
	std::map<std::string, std::shared_ptr<GraphNode>> _graphNodeByBlockID;
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>>_entityByGraphNode;
	std::list < std::shared_ptr<GraphNode>> _rootNodes;

	const std::string getNameWithoutRoot(std::shared_ptr<EntityBlock> blockEntity);

	bool allRequiredConnectionsSet(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	bool entityHasIncommingConnectionsSet(std::shared_ptr<EntityBlock>& blockEntity, std::string& uiMessage);
	bool hasNoCycle(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	Graph buildGraph(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
};
