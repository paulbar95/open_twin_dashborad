#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockDisplay : public EntityBlock
{
public:
	EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual std::string getClassName(void) override { return "EntityBlockDisplay"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
		
	void createProperties();
	const std::string& getDescription();
	virtual ot::GraphicsItemCfg* CreateBlockCfg() override;
	const ot::Connector& getConnectorInput() const { return _inputConnector; }

private:
	ot::Connector _inputConnector;
};
