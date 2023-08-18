#pragma once
#include "EntityBlock.h"

class __declspec(dllexport)  EntityBlockDatabaseAccess : public EntityBlock
{
public:
	EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockDatabaseAccess"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	virtual void addVisualizationNodes(void) override;
	virtual void createProperties(void);
	

protected:
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

};