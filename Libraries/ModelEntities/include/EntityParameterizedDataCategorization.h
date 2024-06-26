#pragma once
#include "EntityContainer.h"
#include <map>
#include <string>

class __declspec(dllexport) EntityParameterizedDataCategorization: public EntityContainer
{
public:
	enum DataCategorie { researchMetadata, measurementSeriesMetadata, quantity, parameter, UNKNOWN};
	EntityParameterizedDataCategorization(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);

	virtual std::string getClassName(void) override { return "EntityParameterizedDataCategorization"; };
	virtual void addVisualizationNodes() override;
	
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	void CreateProperties(DataCategorie categorie);
	DataCategorie GetSelectedDataCategorie() { return _selectedCategory; };

	static std::string GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie category);


private:
	static const std::string _dataCategorieGroup;
	static const std::string _defaultCategory ;
	static const std::string _dataCategorieRMD;
	static const std::string _dataCategorieMSMD ;
	static const std::string _dataCategorieParam;
	static const std::string _dataCategorieQuant;

	DataCategorie _selectedCategory = UNKNOWN;
	
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	virtual int getSchemaVersion() { return 1; };

	static std::map<std::string, DataCategorie> _stringDataCategorieMapping;
	static std::map<DataCategorie, std::string> _dataCategorieStringMapping;
};
