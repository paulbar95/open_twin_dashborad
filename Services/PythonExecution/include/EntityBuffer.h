/*****************************************************************//**
 * \file   EntityBuffer.h
 * \brief  Meyer' singleton that buffers all entities and their properties that were used so far.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include "CPythonObjectNew.h"
#include "EntityBase.h"
#include <string>
#include <memory>
#include "OTServiceFoundation/ModelServiceAPI.h"
#include "EntityResultTable.h"
#include "ClassFactory.h"


class EntityBuffer
{
public:
	friend class FixtureEntityBuffer;
	static EntityBuffer& INSTANCE()
	{
		static EntityBuffer instance;
		return instance;
	} 
	void setModelServiceAPI(ot::ModelServiceAPI* modelServiceAPI) { _modelServiceAPI = modelServiceAPI; };

	PyObject* GetEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName);
	PyObject* GetTableCellValue(const std::string& absoluteEntityName, int32_t row, int32_t column);
	void UpdateEntityPropertyValue(const std::string& absoluteEntityName, const std::string& propertyName, const CPythonObject& values);
	std::shared_ptr<EntityBase> GetEntity (const std::string& absoluteEntityName);
	
	void SaveChangedEntities();
	bool SaveChangedEntities(std::string absoluteEntityName);

	void ClearBuffer();

private:
	EntityBuffer();

	std::map<std::string, std::shared_ptr<EntityBase>> _bufferedEntities;
	std::map<std::string, std::shared_ptr<EntityResultTable<std::string>>> _bufferedTableEntities;
	std::map<std::string, EntityPropertiesBase*> _bufferedEntityProperties;

	ot::ModelServiceAPI* _modelServiceAPI = nullptr;
	void EnsurePropertyToBeLoaded(const std::string& absoluteEntityName, const std::string& propertyName);
	void EnsureTableToBeLoaded(const std::string& absoluteEntityName);
	std::shared_ptr<EntityBase> LoadEntity(const std::string& absoluteEntityName);


};
