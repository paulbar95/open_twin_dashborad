#include "BlockHandlerDatabaseAccess.h"
#include "ResultCollectionHandler.h"
//#include "OpenTwinCore/JSONToVariableConverter.h"
//#include "PropertyHandlerDatabaseAccessBlock.h"

BlockHandlerDatabaseAccess::BlockHandlerDatabaseAccess(EntityBlockDatabaseAccess* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	const std::string projectName =	blockEntity->getSelectedProjectName();

	ResultCollectionHandler resultCollectionHandler;
	std::string resultCollectionName = resultCollectionHandler.getProjectCollection(projectName) + ".results";
	_isValid  = resultCollectionHandler.CollectionExists(resultCollectionName);
	if (_isValid)
	{
		_quantityConnectorName = blockEntity->getConnectorQuantity().getConnectorName();
		_parameterConnectorName =  blockEntity->getConnectorParameter1().getConnectorName();
		//const std::string dbURL = "Projects";
		//_dataStorageAccess = new DataStorageAPI::DocumentAccess(dbURL, resultCollectionName);
		//
		//OT_rJSON_createDOC(query);
		//OT_rJSON_createDOC(projection);
		//ot::rJSON::add(projection, "Value", 1);
		//ot::rJSON::add(projection, "P_1", 1);
		//ot::rJSON::add(projection, "_id", 0);
	

		//const MeasurementCampaign measurementCampaign =	GetMeasurementCampaign(dbAccessEntity, sessionServiceURL, modelServiceURL);
		//_collectionInfos =	&PropertyHandlerDatabaseAccessBlock::instance().getBuffer(blockEntity->getEntityID());
		//auto selectedQuantity = _collectionInfos->quantities[_collectionInfos->SelectedQuantity];
		//
		//ot::rJSON::add(query, "Quantity", selectedQuantity.quantityIndex);
		//_queryString = ot::rJSON::toJSON(query);
		//_projectionString = ot::rJSON::toJSON(projection);
	}
}

BlockHandlerDatabaseAccess::~BlockHandlerDatabaseAccess()
{
	if (_dataStorageAccess != nullptr)
	{
		delete _dataStorageAccess;
		_dataStorageAccess = nullptr;
	}
}

bool BlockHandlerDatabaseAccess::executeSpecialized()
{
	genericDataBlock quantity{0,2,4,8,16,32,64,128,256,512,1024};
	genericDataBlock parameter{ 0,1,2,3,4,5,6,7,8,9,10 };

	_dataPerPort[_quantityConnectorName] = quantity;
	_dataPerPort[_parameterConnectorName] = parameter;

	return true;
}

//BlockHandler::genericDataBlock BlockHandlerDatabaseAccess::Execute(BlockHandler::genericDataBlock& inputData)
//{
//	if (_output.size() == 0)
//	{
//		auto selectedParameter = _collectionInfos->parameters[_collectionInfos->SelectedParameter1];
//		auto& temp = selectedParameter.values;
//		std::vector<ot::Variable> parameterValues(temp.begin(), temp.end());
//
//		auto dbResponse = _dataStorageAccess->GetAllDocuments(_queryString, _projectionString, 0);
//		bool success = dbResponse.getSuccess();
//
//		auto resultDoc = ot::rJSON::fromJSON(dbResponse.getResult());
//		auto allEntries = resultDoc["Documents"].GetArray();
//
//		BlockHandler::genericDataBlock result;
//		ot::JSONToVariableConverter converter;
//
//		for (uint32_t i = 0; i < allEntries.Size(); i++)
//		{
//			if (!allEntries[i].IsNull())
//			{
//				auto arrayEntry = allEntries[i].GetObject();
//				result[_dataConnectorName].push_back(converter(arrayEntry["Value"]));
//				int32_t parameterIndexZeroBased = arrayEntry["P_1"].GetInt() - 1;
//				result[_parameterConnectorName].push_back(parameterValues[parameterIndexZeroBased]);
//			}
//		}
//		_output = std::move(result);
//
//		return _output;
//	}
//	else
//	{
//		return _output;
//	}
//}
