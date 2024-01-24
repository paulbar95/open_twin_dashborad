#include "BlockHandlerPlot1D.h"
#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"
#include "GraphNode.h"
#include "OTCore/GenericDataStructSingle.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	_xlabel = blockEntity->getXLabel();
	_ylabel = blockEntity->getYLabel();
	
	_xunit = blockEntity->getXUnit();
	_yunit = blockEntity->getYUnit();

	_xDataConnector = blockEntity->getConnectorXAxis().getConnectorName();
	_yDataConnector = blockEntity->getConnectorYAxis().getConnectorName();

	const std::string fullName = blockEntity->getName();
	_plotName = fullName.substr(fullName.find_last_of("/")+1, fullName.size());
}
bool BlockHandlerPlot1D::executeSpecialized()
{
	bool allSet = (_dataPerPort.find(_xDataConnector) != _dataPerPort.end()) && (_dataPerPort.find(_yDataConnector) != _dataPerPort.end());
	if (allSet)
	{
		GenericDataList& genericXValues(_dataPerPort[_xDataConnector]);
		GenericDataList& genericYValues(_dataPerPort[_yDataConnector]);

		std::vector<double> xValues = transformDataToDouble(genericXValues);
		std::vector<double> yValues = transformDataToDouble(genericYValues);
		
		const int colorID(0);
		const std::string entityPath = _resultFolder + "1D/Plots";
		const std::string fullPlotName = CreateNewUniqueTopologyName(entityPath, _plotName);

		const std::string curvePath = _resultFolder + "1D/Curves";
		const std::string fullCurveName = curvePath + "/" + _curveName;//CreateNewUniqueTopologyName(fullPlotName, _curveName);
		EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, xValues, yValues, {}, _xlabel, _xunit, _ylabel, _yunit, colorID, true);
		std::list<std::pair<ot::UID, std::string>> curves{ std::pair<ot::UID, std::string>(curve->getEntityID(),_curveName) };
		EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curves);
		ot::UIDList topoEnt{ curve->getEntityID(),plotID->getEntityID() },
				topoVers{ curve->getEntityStorageVersion(),plotID->getEntityStorageVersion() },
				dataEntID{ (ot::UID)curve->getCurveDataStorageId()}, dataEntVers{ (ot::UID)curve->getCurveDataStorageVersion()},
		dataEntParent{ curve->getEntityID() };
			std::list<bool> forceVis{ false,false };
			_modelComponent->addEntitiesToModel(topoEnt, topoVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
	}
	return allSet;
}


std::vector<double> BlockHandlerPlot1D::transformDataToDouble(GenericDataList& genericDataBlocks)
{
	std::vector<double> doubleValues;
	doubleValues.reserve(genericDataBlocks.size());
	for (auto& genericDataBlock: genericDataBlocks)
	{
		ot::GenericDataStructSingle* entry = dynamic_cast<ot::GenericDataStructSingle*>(genericDataBlock.get());
		if (entry == nullptr)
		{
			throw std::exception("Datainput nopt matching format. Requried format: single entries");
		}

		const ot::Variable& value = entry->getValue();
		
		if (value.isInt32())
		{
			doubleValues.push_back(static_cast<double>(value.getInt32()));
		}
		else if (value.isInt64())
		{
			doubleValues.push_back(static_cast<double>(value.getInt64()));
		}
		else if (value.isFloat())
		{
			doubleValues.push_back(static_cast<double>(value.getFloat()));
		}
		else if (value.isDouble())
		{
			doubleValues.push_back(value.getDouble());
		}
		else
		{
			throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
		}
	}
	return doubleValues;
}