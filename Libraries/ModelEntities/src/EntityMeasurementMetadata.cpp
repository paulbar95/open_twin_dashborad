#include "EntityMeasurementMetadata.h"

#include "OTCommunication/ActionTypes.h"

EntityMeasurementMetadata::EntityMeasurementMetadata(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	: EntityWithDynamicFields(ID,parent,mdl,ms,factory,owner)
{
	CreatePlainDocument(_parameterDocument);
	CreatePlainDocument(_quantityDocument);
}

bool EntityMeasurementMetadata::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

void EntityMeasurementMetadata::addVisualizationNodes()
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "MSMD";
		treeIcons.hiddenIcon = "MSMD";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

std::vector<std::string> EntityMeasurementMetadata::getAllParameterDocumentNames()
{
	return getDocumentsNames(_parameterDocument);
}

std::vector<std::string> EntityMeasurementMetadata::getAllQuantityDocumentNames()
{
	return getDocumentsNames(_quantityDocument);
}

void EntityMeasurementMetadata::InsertToParameterField(std::string fieldName, std::list<ot::Variable> values, std::string documentName)
{
	std::string fullDocumentPath = _parameterDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, values, fullDocumentPath);
}

void EntityMeasurementMetadata::InsertToQuantityField(std::string fieldName, std::list<ot::Variable> values, std::string documentName)
{
	std::string fullDocumentPath = _quantityDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, values, fullDocumentPath);
}

