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
	auto allYAxisConnectors = blockEntity->getConnectorsYAxis();
	for (const auto yAxisConnector : allYAxisConnectors)
	{
		_yDataConnectors.push_back(yAxisConnector->getConnectorName());
	}

	_curveNames =	blockEntity->getCurveNames();
	_curveNames.unique();
	if (_curveNames.size() != _yDataConnectors.size())
	{
		throw std::exception("Plot Block detected non unique identifier amongst the curve names.");
	}
	for (const std::string& curveName : _curveNames)
	{
		if (curveName == "")
		{
			throw std::exception("Plot Block detected an empty curve name.");
		}
	}

	const std::string fullName = blockEntity->getName();
	_plotName = fullName.substr(fullName.find_last_of("/")+1, fullName.size());
}
bool BlockHandlerPlot1D::executeSpecialized()
{
	bool allSet = (_dataPerPort.find(_xDataConnector) != _dataPerPort.end());
	for (std::string& yDataConnector : _yDataConnectors)
	{
		if (_dataPerPort.find(yDataConnector) == _dataPerPort.end())
		{
			allSet = false;
			break;
		}
	}
	
	if (allSet)
	{
		GenericDataList& genericXValues(_dataPerPort[_xDataConnector]);
		if (genericXValues.size() == 0)
		{
			throw std::exception("X axis values are empty. Nothing to plot.");
		}
		std::vector<double> xValues = transformDataToDouble(genericXValues);
		const int colorID(0);

		auto currentCurveName = _curveNames.begin();
		
		std::list<std::pair<ot::UID, std::string>> curves;
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;
		const std::string plotFolder = _resultFolder + "1D/Plots";
		const std::string fullPlotName = plotFolder + _plotName;

		for (std::string& yAxisData : _yDataConnectors)
		{
			GenericDataList& genericYValues(_dataPerPort[yAxisData]);
			if (genericYValues.size() == 0)
			{
				throw std::exception("Y axis values are empty. Nothing to plot.");
			}

			std::vector<double> yValues = transformDataToDouble(genericYValues);
			const std::string curveName = *currentCurveName;
			currentCurveName++;
			
			const std::string fullCurveName = _curveFolderPath + "/" + curveName;
			EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, xValues, yValues, {}, _xlabel, _xunit, _ylabel, _yunit, colorID, true);
			
			//const std::string curveReferencePath =  fullPlotName + "/" + curveName;
			curves.push_back(std::pair<ot::UID, std::string>(curve->getEntityID(),curveName));
			
			topoEntID.push_back(curve->getEntityID());
			topoEntVers.push_back(curve->getEntityStorageVersion());
			dataEntID.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntVers.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntParent.push_back(curve->getEntityID());
			forceVis.push_back(false);
		}

		EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curves);
		topoEntID.push_back(plotID->getEntityID());
		topoEntVers.push_back(plotID->getEntityStorageVersion());
		forceVis.push_back(false);
		_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
	}
	return allSet;
}

std::vector<double> BlockHandlerPlot1D::transformDataToDouble(GenericDataList& genericDataBlocks)
{
	std::vector<double> doubleValues;
	doubleValues.reserve(genericDataBlocks.size());
	if (genericDataBlocks.size() > 1)
	{
		for (auto& genericDataBlock : genericDataBlocks)
		{
			ot::GenericDataStructSingle* entry = dynamic_cast<ot::GenericDataStructSingle*>(genericDataBlock);
			if (entry == nullptr)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported");
			}

			const ot::Variable& value = entry->getValue();
			doubleValues.push_back(VariableToDouble(value));
		}
	}
	else
	{
		ot::GenericDataStructVector* entry = dynamic_cast<ot::GenericDataStructVector*>(*genericDataBlocks.begin());
		if (entry == nullptr)
		{
			throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported.");
		}
		const std::vector<ot::Variable> values = entry->getValues();
		for (const ot::Variable& value : values)
		{
			doubleValues.push_back(VariableToDouble(value));
		}

	}
	return doubleValues;
}

double BlockHandlerPlot1D::VariableToDouble(const ot::Variable& value)
{
	if (value.isInt32())
	{
		return (static_cast<double>(value.getInt32()));
	}
	else if (value.isInt64())
	{
		return (static_cast<double>(value.getInt64()));
	}
	else if (value.isFloat())
	{
		return (static_cast<double>(value.getFloat()));
	}
	else if (value.isDouble())
	{
		return (value.getDouble());
	}
	else
	{
		throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
	}
}
