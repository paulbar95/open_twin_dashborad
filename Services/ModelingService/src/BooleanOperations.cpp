#include "BooleanOperations.h"
#include "ShapesBase.h"
#include "EntityCache.h"

#include "EntityGeometry.h"
#include "ClassFactoryCAD.h"
#include "ClassFactory.h"

#include "OTServiceFoundation/EntityInformation.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

#include <string>
#include <list>
#include <map>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>

BooleanOperations::BooleanOperations(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, const std::string &_serviceName, EntityCache *_entityCache, ot::serviceID_t _serviceID) :
	uiComponent(_uiComponent),
	modelComponent(_modelComponent),
	serviceName(_serviceName),
	entityCache(_entityCache),
	serviceID(_serviceID)
{

}

void BooleanOperations::enterAddMode(const std::list<ot::EntityInformation> &selectedGeometryEntities)
{
	// First, we check which objects are currently selected

	if (selectedGeometryEntities.size() != 1)
	{
		uiComponent->displayErrorPrompt("Please select one (and only one) geometry object before entering the boolean add mode");
		return;
	}

	std::string baseShapeName = selectedGeometryEntities.front().getName();

	std::map<std::string, std::string> options;
	options["BaseShape"] = baseShapeName;

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySlectionType::SHAPE, 
		true, "", ot::components::UiComponent::entitySelectionAction::BOOLEAN_ADD, "add", options, serviceID);
}

void BooleanOperations::enterSubtractMode(const std::list<ot::EntityInformation> &selectedGeometryEntities)
{
	// First, we check which objects are currently selected

	if (selectedGeometryEntities.size() != 1)
	{
		uiComponent->displayErrorPrompt("Please select one (and only one) geometry object before entering the boolean subtract mode");
		return;
	}

	std::string baseShapeName = selectedGeometryEntities.front().getName();

	std::map<std::string, std::string> options;
	options["BaseShape"] = baseShapeName;

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySlectionType::SHAPE, 
		true, "", ot::components::UiComponent::entitySelectionAction::BOOLEAN_SUBTRACT, "subtract", options, serviceID);
}

void BooleanOperations::enterIntersectMode(const std::list<ot::EntityInformation> &selectedGeometryEntities)
{
	// First, we check which objects are currently selected

	if (selectedGeometryEntities.size() != 1)
	{
		uiComponent->displayErrorPrompt("Please select one (and only one) geometry object before entering the boolean intersect mode");
		return;
	}

	std::string baseShapeName = selectedGeometryEntities.front().getName();

	std::map<std::string, std::string> options;
	options["BaseShape"] = baseShapeName;

	uiComponent->enterEntitySelectionMode(modelComponent->getCurrentVisualizationModelID(), ot::components::UiComponent::entitySlectionType::SHAPE, 
		true, "", ot::components::UiComponent::entitySelectionAction::BOOLEAN_INTERSECT, "intersect", options, serviceID);
}

void BooleanOperations::perfromOperationForSelectedEntities(const std::string &selectionAction, const std::string &selectionInfo, std::map<std::string, std::string> &options)
{
	// Get the base entity for the operation
	std::string baseEntityName = options["BaseShape"];

	ot::EntityInformation baseEntityInfo;
	modelComponent->getEntityInformation(baseEntityName, baseEntityInfo);

	// Get the information about the selected entities.
	OT_rJSON_parseDOC(doc, selectionInfo.c_str());
	std::list<ot::UID> selectedEntities = ot::rJSON::getULongLongList(doc, "modelID");

	std::list<ot::EntityInformation> entityInfo;
	modelComponent->getEntityInformation(selectedEntities, entityInfo);

	// Build a list of all geometry shapes and check whether the original shape is also selected as tool shape
	std::list<ot::EntityInformation> geometryEntities;
	for (auto entity : entityInfo)
	{
		if (entity.getType() == "EntityGeometry")
		{
			geometryEntities.push_back(entity);
		}

		if (entity.getName() == baseEntityName)
		{
			uiComponent->displayErrorPrompt("The base shape must not be selected as tool shape for the operation as well");
			return;
		}
	}

	// Prefetch all geometry entities
	std::list<ot::EntityInformation> allGeomEntities = geometryEntities;
	allGeomEntities.push_front(baseEntityInfo);
	entityCache->prefetchEntities(allGeomEntities);

	// Load all geometry entities and determine a list of their brep ids
	ClassFactory baseFactory;
	ClassFactoryCAD classFactory;
	classFactory.SetNextHandler(&baseFactory);
	baseFactory.SetChainRoot(&classFactory);

	EntityGeometry *baseEntity = dynamic_cast<EntityGeometry*>(entityCache->getEntity(baseEntityInfo.getID(), baseEntityInfo.getVersion()));
	if (baseEntity == nullptr) { assert(0); return; }

	std::list<ot::UID> requiredBreps;
	requiredBreps.push_back(baseEntity->getBrepStorageObjectID());

	std::list<EntityGeometry*> toolEntities;

	for (auto entity : geometryEntities)
	{
		EntityGeometry *toolEntity = dynamic_cast<EntityGeometry*>(entityCache->getEntity(entity.getID(), entity.getVersion()));

		if (toolEntity != nullptr)
		{
			toolEntities.push_back(toolEntity);
			requiredBreps.push_back(toolEntity->getBrepStorageObjectID());
		}
	}

	// Now get the information about all the breps
	std::list<ot::EntityInformation> brepEntityInfo;
	modelComponent->getEntityInformation(requiredBreps, brepEntityInfo);

	// Prefetch all the brep entities
	entityCache->prefetchEntities(brepEntityInfo);

	// Load all brep entities 
	std::list<EntityBrep *> brepEntities;
	for (auto entity : brepEntityInfo)
	{
		EntityBrep *brepEntity = dynamic_cast<EntityBrep*>(entityCache->getEntity(entity.getID(), entity.getVersion()));
		brepEntities.push_back(brepEntity);
	}

	EntityBrep *baseBrepEntity = brepEntities.front();
	brepEntities.pop_front();

	TopoDS_Shape shape;
	std::string treeIconVisible, treeIconHidden;

	bool success = performOperation(selectionAction, baseBrepEntity, brepEntities, shape, treeIconVisible, treeIconHidden);

	// Build a new geometry entity and store it 
	if (success)
	{
		// Ensure that we have a valid solid topology
		BRepBuilderAPI_MakeSolid ms;
		TopExp_Explorer exp;
		for (exp.Init(shape, TopAbs_SHELL); exp.More(); exp.Next()) 
		{
			ms.Add(TopoDS::Shell(exp.Current()));
		}

		shape = ms.Shape();

		// First, delete all the entities
		std::list<std::string> entityNameList;
		entityNameList.push_back(baseEntityName);
		for (auto tool : geometryEntities)
		{
			entityNameList.push_back(tool.getName());
		}

		// Now we update the base entity
		ot::UID entityID = modelComponent->createEntityUID();
		ot::UID brepID   = modelComponent->createEntityUID();
		ot::UID facetsID = modelComponent->createEntityUID();

		EntityGeometry *geometryEntity = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, serviceName);
		geometryEntity->setName(baseEntity->getName());
		geometryEntity->setEditable(true);
		geometryEntity->setSelectChildren(false);
		geometryEntity->setManageChildVisibility(false);
		//geometryEntity->setManageParentVisibility(false);  // The new boolean entity should manage the parent visibility as usual
		geometryEntity->setBrep(shape);
		geometryEntity->setTreeIcons(treeIconVisible, treeIconHidden);

		geometryEntity->getProperties() = baseEntity->getProperties();

		deletePropertyCategory(geometryEntity, "Dimensions");

		geometryEntity->resetTransformation();

		EntityPropertiesString *typeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("shapeType"));

		if (typeProperty != nullptr)
		{
			typeProperty->setValue(selectionAction);
		}
		else
		{
			typeProperty = new EntityPropertiesString("shapeType", selectionAction);
			typeProperty->setVisible(false);
			geometryEntity->getProperties().createProperty(typeProperty, "Internal");
		}

		EntityPropertiesString *baseShapeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("baseShape"));

		if (baseShapeProperty != nullptr)
		{
			baseShapeProperty->setValue(std::to_string(baseEntity->getEntityID()));
		}
		else
		{
			baseShapeProperty = new EntityPropertiesString("baseShape", std::to_string(baseEntity->getEntityID()));
			baseShapeProperty->setVisible(false);
			geometryEntity->getProperties().createProperty(baseShapeProperty, "Internal");
		}

		std::string toolPropertyValue;

		for (auto entity : toolEntities)
		{
			toolPropertyValue.append(std::to_string(entity->getEntityID()) + ",");
		}

		EntityPropertiesString *toolShapeProperty = dynamic_cast<EntityPropertiesString*>(geometryEntity->getProperties().getProperty("toolShapes"));

		if (toolShapeProperty != nullptr)
		{
			toolShapeProperty->setValue(toolPropertyValue);
		}
		else
		{
			toolShapeProperty = new EntityPropertiesString("toolShapes", toolPropertyValue);
			toolShapeProperty->setVisible(false);
			geometryEntity->getProperties().createProperty(toolShapeProperty, "Internal");
		}

		// The geometry entity has two children: brep and facets. We need to assign ids to both of them (since we do not have a model state object here)
		geometryEntity->getBrepEntity()->setEntityID(brepID);
		geometryEntity->getFacets()->setEntityID(facetsID);

		// Now we facet the entity and store it to the data base afterward.
		geometryEntity->facetEntity(false);
		geometryEntity->StoreToDataBase();

		ot::UID entityVersion = geometryEntity->getEntityStorageVersion();
		ot::UID brepVersion   = geometryEntity->getBrepEntity()->getEntityStorageVersion();
		ot::UID facetsVersion = geometryEntity->getFacets()->getEntityStorageVersion();

		entityCache->cacheEntity(geometryEntity->getBrepEntity());
		geometryEntity->detachBrep();

		// Finally add the new entity to the model (data children need to come first)

		std::list<ot::UID> dataEntityIDList = { brepID, facetsID};
		std::list<ot::UID> dataEntityVersionList = { brepVersion , facetsVersion };
		std::list<ot::UID> dataEntityParentList = { entityID, entityID };

		//modelComponent->enableMessageQueueing(true);

		//modelComponent->deleteEntitiesFromModel(entityNameList, false);

		modelComponent->addGeometryOperation(entityID, entityVersion, baseEntity->getName(), dataEntityIDList, dataEntityVersionList, dataEntityParentList, entityNameList, "boolean operation: " + geometryEntity->getName());

		delete geometryEntity;
		geometryEntity = nullptr;

		//modelComponent->enableMessageQueueing(false);
	}

	// We do not need to delete the base and tool entities here, since they are managed by the entity cache.
	toolEntities.clear();

	// We do not need to delete the brep entities here, since they are managed by the entity cache.
	brepEntities.clear();
}

void BooleanOperations::deletePropertyCategory(EntityGeometry *geometryEntity, const std::string category)
{
	std::list<std::string> deleteProperties = geometryEntity->getProperties().getListOfPropertiesForGroup(category);

	for (auto prop : deleteProperties)
	{
		geometryEntity->getProperties().deleteProperty(prop);
	}
}

bool BooleanOperations::add(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape)
{
	shape = base->getBrep();

	for (auto tool : tools)
	{
		Standard_Boolean bRunParallel = Standard_True;
		Standard_Real aFuzzyValue = 0.0;

		BRepAlgoAPI_Fuse aBoolean(shape, tool->getBrep());

		aBoolean.SetRunParallel(bRunParallel);
		aBoolean.SetFuzzyValue(aFuzzyValue);

		//Standard_Boolean bSafeMode = Standard_True;
		//aBuilder.SetNonDestructive(bSafeMode);

		//BOPAlgo_GlueEnum aGlueOpt = BOPAlgo_GlueFull;
		//aBuilder.SetGlue(aGlueOpt);

		aBoolean.Build();

		if (aBoolean.HasErrors())
		{
			return false;
		}

		shape = aBoolean.Shape();
	}

	return true;
}

bool BooleanOperations::subtract(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape)
{
	shape = base->getBrep();

	for (auto tool : tools)
	{
		Standard_Boolean bRunParallel = Standard_True;
		Standard_Real aFuzzyValue = 0.0;

		BRepAlgoAPI_Cut aBoolean(shape, tool->getBrep());

		aBoolean.SetRunParallel(bRunParallel);
		aBoolean.SetFuzzyValue(aFuzzyValue);

		//Standard_Boolean bSafeMode = Standard_True;
		//aBoolean.SetNonDestructive(bSafeMode);

		//BOPAlgo_GlueEnum aGlueOpt = BOPAlgo_GlueFull;
		//aBuilder.SetGlue(aGlueOpt);

		aBoolean.Build();

		// If the boolean operation succeeds, we take the result of the operation. Otherwise we keep the based shape and
		// ignore this operation (might happen. e.g. if the tool shape is empty).
		if (!aBoolean.HasErrors())
		{
			shape = aBoolean.Shape();
		}
	}

	return true;
}

bool BooleanOperations::intersect(EntityBrep *base, std::list<EntityBrep *> &tools, TopoDS_Shape &shape)
{
	shape = base->getBrep();

	for (auto tool : tools)
	{
		Standard_Boolean bRunParallel = Standard_True;
		Standard_Real aFuzzyValue = 0.0;

		BRepAlgoAPI_Common aBoolean(shape, tool->getBrep());

		aBoolean.SetRunParallel(bRunParallel);
		aBoolean.SetFuzzyValue(aFuzzyValue);

		//Standard_Boolean bSafeMode = Standard_True;
		//aBuilder.SetNonDestructive(bSafeMode);

		//BOPAlgo_GlueEnum aGlueOpt = BOPAlgo_GlueFull;
		//aBuilder.SetGlue(aGlueOpt);

		aBoolean.Build();

		if (aBoolean.HasErrors())
		{
			return false;
		}

		shape = aBoolean.Shape();
	}

	return true;
}

bool BooleanOperations::performOperation(const std::string &selectionAction, EntityBrep *baseBrepEntity, std::list<EntityBrep *> &brepEntities, TopoDS_Shape &shape, std::string &treeIconVisible, std::string &treeIconHidden)
{
	bool success = true;

	// Call the requested operation and build the resulting Brep
	if (selectionAction == "BOOLEAN_ADD")
	{
		success = add(baseBrepEntity, brepEntities, shape);

		treeIconVisible = "BooleanAddVisible";
		treeIconHidden  = "BooleanAddHidden";
	}
	else if (selectionAction == "BOOLEAN_SUBTRACT")
	{
		success = subtract(baseBrepEntity, brepEntities, shape);

		treeIconVisible = "BooleanSubtractVisible";
		treeIconHidden  = "BooleanSubtractHidden";
	}
	else if (selectionAction == "BOOLEAN_INTERSECT")
	{
		success = intersect(baseBrepEntity, brepEntities, shape);

		treeIconVisible = "BooleanIntersectVisible";
		treeIconHidden  = "BooleanIntersectHidden";
	}
	else
	{
		assert(0); // Unknown operation
		success = false;
	}

	return success;
}
