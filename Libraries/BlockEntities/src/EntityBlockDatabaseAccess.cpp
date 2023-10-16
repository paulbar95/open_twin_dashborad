#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"

EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
}

void EntityBlockDatabaseAccess::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "BlockDataBaseAccess";
		treeIcons.hiddenIcon = "BlockDataBaseAccess";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
		
	}
}

void EntityBlockDatabaseAccess::createProperties()
{
	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Projectname");
	getProperties().createProperty(projectList, "Database");

	//Basic properties:
	EntityPropertiesSelection::createProperty(_groupQuerySettings, _propertyNameDimension, { _propertyValueDimension1,_propertyValueDimension2, _propertyValueDimension3, _propertyValueDimensionCustom }, _propertyValueDimension1, "default", getProperties());
	//EntityPropertiesSelection::createProperty(_groupQuerySettings, _msmdPropertyName, {}, "", "default", getProperties());

	//Quantity Settings
	EntityPropertiesSelection::createProperty(_groupQuantitySetttings, _propertyNameQuantity, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelQuantity = new EntityPropertiesString();
	typeLabelQuantity->setReadOnly(true);
	typeLabelQuantity->setName(_propertyDataTypeQuantity);
	typeLabelQuantity->setGroup(_groupQuantitySetttings);
	typeLabelQuantity->setValue("");
	getProperties().createProperty(typeLabelQuantity, _groupQuantitySetttings);

	EntityPropertiesSelection::createProperty(_groupQuantitySetttings, _propertyComparator, _comparators, "", "default", getProperties());
	EntityPropertiesString::createProperty(_groupQuantitySetttings, _propertyValueQuantity, "", "default", getProperties());

	//Parameter 1 settings
	EntityPropertiesSelection::createProperty(_groupParamSettings1, _propertyNameP1, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP1 = new EntityPropertiesString();
	typeLabelP1->setReadOnly(true);
	typeLabelP1->setName(_propertyDataTypeP1);
	typeLabelP1->setGroup(_groupParamSettings1);
	typeLabelP1->setValue("");
	getProperties().createProperty(typeLabelP1, _groupParamSettings1);
	EntityPropertiesSelection::createProperty(_groupParamSettings1, _propertyComparatorP1, _comparators, "", "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings1, _propertyValueP1, "", "default", getProperties());

	EntityPropertiesSelection::createProperty(_groupParamSettings2, _propertyNameP2, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP2 = new EntityPropertiesString();
	typeLabelP2->setReadOnly(true);
	typeLabelP2->setName(_propertyDataTypeP2);
	typeLabelP2->setGroup(_groupParamSettings2);
	typeLabelP2->setValue("");
	getProperties().createProperty(typeLabelP2, _groupParamSettings2);
	EntityPropertiesSelection::createProperty(_groupParamSettings2, _propertyComparatorP2, _comparators, "", "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings2, _propertyValueP2, "", "default", getProperties());

	EntityPropertiesSelection::createProperty(_groupParamSettings3, _propertyNameP3, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP3 = new EntityPropertiesString();
	typeLabelP3->setReadOnly(true);
	typeLabelP3->setName(_propertyDataTypeP3);
	typeLabelP3->setGroup(_groupParamSettings3);
	typeLabelP3->setValue("");
	getProperties().createProperty(typeLabelP3, _groupParamSettings3);
	EntityPropertiesSelection::createProperty(_groupParamSettings3, _propertyComparatorP3, _comparators, "", "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings3, _propertyValueP3, "", "default", getProperties());

	SetVisibleParameter2(false);
	SetVisibleParameter3(false);
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	auto propertyBase = getProperties().getProperty("Projectname");
	auto selectedProjectName = dynamic_cast<EntityPropertiesProjectList*>(propertyBase);
	assert(selectedProjectName != nullptr);

	return  selectedProjectName->getValue();
}

std::string EntityBlockDatabaseAccess::getQueryDimension()
{
	auto propertyBase = getProperties().getProperty("Outcome dimension");
	auto outcome = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(outcome != nullptr);

	return outcome->getValue();
}

bool EntityBlockDatabaseAccess::SetVisibleParameter2(bool visible)
{
	const bool isVisible = getProperties().getProperty(_propertyNameP2)->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty(_propertyNameP2)->setVisible(visible);
		getProperties().getProperty(_propertyDataTypeP2)->setVisible(visible);
		getProperties().getProperty(_propertyComparatorP2)->setVisible(visible);
		getProperties().getProperty(_propertyValueP2)->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

bool EntityBlockDatabaseAccess::SetVisibleParameter3(bool visible)
{
	bool isVisible = getProperties().getProperty(_propertyNameP3)->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty(_propertyNameP3)->setVisible(visible);
		getProperties().getProperty(_propertyDataTypeP3)->setVisible(visible);
		getProperties().getProperty(_propertyComparatorP3)->setVisible(visible);
		getProperties().getProperty(_propertyValueP3)->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

ot::GraphicsItemCfg* EntityBlockDatabaseAccess::CreateBlockCfg()
{
	return nullptr;
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{
	auto baseProperty = getProperties().getProperty(_propertyNameDimension);
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);
	bool refresh = false;
	if (selectionProperty->getValue() == _propertyValueDimension1)
	{
		refresh = SetVisibleParameter2(false);
		refresh |= SetVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == _propertyValueDimension2)
	{
		refresh = SetVisibleParameter2(true);
		refresh |= SetVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == _propertyValueDimension3)
	{
		refresh = SetVisibleParameter2(true);
		refresh |= SetVisibleParameter3(true);
	}
	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}
	return refresh;
}

void EntityBlockDatabaseAccess::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

const std::string& EntityBlockDatabaseAccess::getQuantityQueryValue()
{
	auto baseProp = getProperties().getProperty(_propertyValueQuantity);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	return valueProp->getValue();

}

const std::string& EntityBlockDatabaseAccess::getQuantityQueryComparator()
{
	auto baseProp = getProperties().getProperty(_propertyComparator);
	auto selectProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	return selectProp->getValue();
}

const std::string& EntityBlockDatabaseAccess::getParameter1QueryValue()
{
	auto baseProp = getProperties().getProperty(_propertyValueP1);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	return valueProp->getValue();
}

const std::string& EntityBlockDatabaseAccess::getParameter1QueryComparator()
{
	auto baseProp = getProperties().getProperty(_propertyComparatorP1);
	auto selectProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	return selectProp->getValue();
}

const std::string& EntityBlockDatabaseAccess::getParameter2QueryValue()
{

	auto baseProp = getProperties().getProperty(_propertyValueP2);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	return valueProp->getValue();
}

const std::string& EntityBlockDatabaseAccess::getParameter2QueryComparator()
{
	auto baseProp = getProperties().getProperty(_propertyComparatorP2);
	auto selectProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	return selectProp->getValue();
}


const std::string& EntityBlockDatabaseAccess::getParameter3QueryValue()
{
	auto baseProp = getProperties().getProperty(_propertyValueP3);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	return valueProp->getValue();
}

const std::string& EntityBlockDatabaseAccess::getParameter3QueryComparator()
{
	auto baseProp = getProperties().getProperty(_propertyComparatorP3);
	auto selectProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	return selectProp->getValue();
}