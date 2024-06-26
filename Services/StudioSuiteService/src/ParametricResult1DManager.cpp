#include "ParametricResult1DManager.h"
#include "Result1DManager.h"
#include "Application.h"
#include "DataBase.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTCore/FolderNames.h"

#include "ResultCollectionExtender.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataEntrySingle.h"

#include <boost/algorithm/string.hpp>

ParametricResult1DManager::ParametricResult1DManager(Application *app) :
	resultFolderName(ot::FolderNames::DatasetFolder),
	application(app)
{

}

ParametricResult1DManager::~ParametricResult1DManager()
{

}

void ParametricResult1DManager::clear()
{
	// We delete all previous result data (series), since there was a non-parametric change
	std::list<std::string> resultEntity{ resultFolderName + "/1D Results" };

	application->modelComponent()->deleteEntitiesFromModel(resultEntity, false);
}

void ParametricResult1DManager::add(Result1DManager& result1DManager)
{
	// We add the data in result1DManager to the parametric storage
	std::list<int> runIDList = result1DManager.getRunIDList();
	if (runIDList.empty()) return;

	std::string runIDLabel = determineRunIDLabel(runIDList);

	std::string collectionName = DataBase::GetDataBase()->getProjectName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *application->modelComponent(), &application->getClassFactory(), OT_INFO_SERVICE_TYPE_STUDIOSUITE);
	resultCollectionExtender.setSaveModel(false);

	// Now we process the different types of data entries
	processCurves("1D Results/Balance", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Energy", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Port signals", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Power", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Reference Impedance", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processSparameters("1D Results/S-Parameters", runIDLabel, runIDList,result1DManager, resultCollectionExtender);
}

std::string ParametricResult1DManager::determineRunIDLabel(std::list<int> &runIDList)
{
	assert(!runIDList.empty());

	int minRunID = runIDList.front();
	int maxRunID = runIDList.back();

	if (minRunID == maxRunID)
	{
		return "Run " + std::to_string(minRunID);
	}

	return "Run " + std::to_string(minRunID) + "-" + std::to_string(maxRunID);
}

void ParametricResult1DManager::processCurves(const std::string& category, const std::string &runIDLabel, std::list<int> &runIDList, Result1DManager& result1DManager, ResultCollectionExtender &resultCollectionExtender)
{
	for (auto runID : runIDList)
	{
		RunIDContainer* container = result1DManager.getContainer(runID);
		assert(container != nullptr);

		std::map<std::string, Result1DData*> categoryResults = container->getResultsForCategory(category);
		if (categoryResults.empty()) continue;

		// Determine x-axis type and unit
		std::string xLabel, xUnit;
		parseAxisLabel(categoryResults.begin()->second->getXLabel(), xLabel, xUnit);

		size_t numberOfXValues = categoryResults.begin()->second->getXValues().size();

		bool hasRealPart    = !(categoryResults.begin()->second->getYreValues().empty());
		bool hasImagPart    = !(categoryResults.begin()->second->getYimValues().empty());
		int numberOfVectors = (hasRealPart ? 1 : 0) + (hasImagPart ? 1 : 0);

		size_t numberOfQuantities = categoryResults.size() * numberOfVectors;

		std::string seriesName = ot::FolderNames::DatasetFolder + "/" + category + "/" + runIDLabel;
		
		std::list<ot::Variable> xValues;

		// Add the meta data
		addCurveSeriesMetadata(resultCollectionExtender, category, seriesName, xLabel, xUnit, container, categoryResults, xValues, hasRealPart, hasImagPart, numberOfQuantities);

		// Now we add the quantity data
		addCurveSeriesQuantityData(resultCollectionExtender, seriesName, xLabel, numberOfXValues, categoryResults, hasRealPart, hasImagPart);
	}
}

void ParametricResult1DManager::addCurveSeriesMetadata(ResultCollectionExtender &resultCollectionExtender, const std::string& category, const std::string &seriesName, 
													   const std::string &xLabel, const std::string &xUnit, RunIDContainer* container, 
													   std::map<std::string, Result1DData*> &categoryResults, std::list<ot::Variable> &xValues,
													   bool hasRealPart, bool hasImagPart, size_t numberOfQuantities)
{
	ot::UID seriesID = application->modelComponent()->createEntityUID();

	MetadataSeries seriesMetadata(seriesName, seriesID);

	// Add all structure parameters
	for (auto param : container->getParameters())
	{
		std::shared_ptr<MetadataEntry>mdParameter(new MetadataEntrySingle(param.first, ot::Variable(param.second)));
		seriesMetadata.AddMetadata(mdParameter);
	}

	// Determine the xvalues
	for (auto x : categoryResults.begin()->second->getXValues())
	{
		xValues.push_back(x);
	}

	// Create the axis parameter
	MetadataParameter parameter;
	std::shared_ptr<MetadataEntry> mdXaxisSetting(new MetadataEntrySingle("Unit", xUnit));
	parameter.metaData.insert(std::make_pair<>("Unit", mdXaxisSetting));
	parameter.parameterName = xLabel;
	parameter.values = xValues;
	parameter.typeName = "double";

	seriesMetadata.AddParameter(std::move(parameter));

	// Add the quantities (each curve will be added as a separate quantity)
	std::vector<MetadataQuantity> quantities;
	quantities.resize(numberOfQuantities);

	int quantityIndex = 0;
	for (auto curve : categoryResults)
	{
		std::string prefix = curve.first.substr(category.size() + 1);

		if (hasRealPart && hasImagPart)
		{
			quantities[quantityIndex].quantityName = prefix + " (Re)";
			quantityIndex++;
			quantities[quantityIndex].quantityName = prefix + " (Im)";
			quantityIndex++;
		}
		else if (hasRealPart)
		{
			quantities[quantityIndex].quantityName = prefix;
			quantityIndex++;
		}
		else if (hasImagPart)
		{
			quantities[quantityIndex].quantityName = prefix;
			quantityIndex++;
		}
		else
		{
			assert(0);
		}
	}

	for (auto quantity : quantities)
	{
		quantity.typeName = "double";
		quantity.dataRows = 1;
		quantity.dataColumns = 1;

		seriesMetadata.AddQuantity(std::move(quantity));
	}

	resultCollectionExtender.AddSeries(std::move(seriesMetadata));
	resultCollectionExtender.setBucketSize(1);
}

void ParametricResult1DManager::addCurveSeriesQuantityData(ResultCollectionExtender& resultCollectionExtender, const std::string& seriesName, const std::string& xLabel, size_t numberOfXValues,
														   std::map<std::string, Result1DData*>& categoryResults, bool hasRealPart, bool hasImagPart)
{
	const auto seriesMetadata = resultCollectionExtender.FindMetadataSeries(seriesName);
	const MetadataParameter* parameter = resultCollectionExtender.FindMetadataParameter(xLabel);
	assert(parameter != nullptr);
	if (parameter == nullptr) return;

	std::list<std::string> parameterAbbrev{ parameter->parameterAbbreviation };

	const auto allQuantities = seriesMetadata->getQuantities();

	size_t xIndex = 0;
	for (size_t xIndex = 0; xIndex < numberOfXValues; xIndex++)
	{
		auto currentQuantity = allQuantities.begin();

		for (auto curve : categoryResults)
		{
			Result1DData* data = curve.second;

			double xValue = data->getXValues()[xIndex];

			if (hasRealPart && hasImagPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, {xValue}, currentQuantity->quantityIndex, data->getYreValues()[xIndex]);
				currentQuantity++;
				resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYimValues()[xIndex]);
				currentQuantity++;
			}
			else if (hasRealPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYreValues()[xIndex]);
				currentQuantity++;
			}
			else if (hasImagPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYimValues()[xIndex]);
				currentQuantity++;
			}
		}
	}

	resultCollectionExtender.FlushQuantityContainer();
}

void ParametricResult1DManager::parseAxisLabel(const std::string& value, std::string& label, std::string& unit)
{
	// Here we separate the value into the actual axis type and the unit. We assume that both are separated by a / character
	size_t separatorIndex = value.find('/');

	label = value.substr(0, separatorIndex);
	unit = value.substr(separatorIndex + 1);

	boost::trim(label);
	boost::trim(unit);
}

void ParametricResult1DManager::processSparameters(const std::string& category, const std::string& runIDLabel, std::list<int>& runIDList, Result1DManager& result1DManager, ResultCollectionExtender& resultCollectionExtender)
{
	for (auto runID : runIDList)
	{
		RunIDContainer* container = result1DManager.getContainer(runID);
		assert(container != nullptr);

		std::map<std::string, Result1DData*> categoryResults = container->getResultsForCategory(category);
		if (categoryResults.empty()) continue;

		// Determine x-axis type and unit
		std::string xLabel, xUnit;
		parseAxisLabel(categoryResults.begin()->second->getXLabel(), xLabel, xUnit);

		size_t numberOfXValues = categoryResults.begin()->second->getXValues().size();

		std::vector<Result1DData*> sources;
		int numberPorts = determineNumberOfPorts(category, categoryResults, sources);

		std::string seriesName = ot::FolderNames::DatasetFolder + "/" + category + "/" + runIDLabel;

		std::list<ot::Variable> xValues;

		// Add the meta data
		addSparameterSeriesMetadata(resultCollectionExtender, category, seriesName, xLabel, xUnit, container, numberPorts, categoryResults, xValues);

		// Now we add the quantity data
		addSparameterSeriesQuantityData(resultCollectionExtender, seriesName, xLabel, numberOfXValues, categoryResults, sources);
	}
}

int ParametricResult1DManager::determineNumberOfPorts(const std::string &category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*> &sources)
{
	int maxPort = 0;

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index+1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		maxPort = std::max(maxPort, nPort1);
		maxPort = std::max(maxPort, nPort2);
	}

	sources.resize(maxPort * maxPort);

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index + 1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		sources[(nPort1 - 1) * maxPort + (nPort2 - 1)] = item.second;
	}

	return maxPort;
}

void ParametricResult1DManager::addSparameterSeriesMetadata(ResultCollectionExtender& resultCollectionExtender, const std::string& category, const std::string& seriesName, 
													        const std::string& xLabel, const std::string& xUnit, RunIDContainer* container, int numberPorts,
													        std::map<std::string, Result1DData*>& categoryResults, std::list<ot::Variable>& xValues)
{
	ot::UID seriesID = application->modelComponent()->createEntityUID();

	MetadataSeries seriesMetadata(seriesName, seriesID);

	// Add all structure parameters
	for (auto param : container->getParameters())
	{
		std::shared_ptr<MetadataEntry>mdParameter(new MetadataEntrySingle(param.first, ot::Variable(param.second)));
		seriesMetadata.AddMetadata(mdParameter);
	}

	// Determine the xvalues
	for (auto x : categoryResults.begin()->second->getXValues())
	{
		xValues.push_back(x);
	}

	// Create the axis parameter
	MetadataParameter parameter;
	std::shared_ptr<MetadataEntry> mdXaxisSetting(new MetadataEntrySingle("Unit", xUnit));
	parameter.metaData.insert(std::make_pair<>("Unit", mdXaxisSetting));
	parameter.parameterName = xLabel;
	parameter.values        = xValues;
	parameter.typeName      = "double";

	seriesMetadata.AddParameter(std::move(parameter));

	// Add the quantities (there will be two matrix quantities (real and imaginary))
	MetadataQuantity quantityRe, quantityIm;

	quantityRe.quantityName = "S-Parameter (Re)";
	quantityRe.typeName     = "double";
	quantityRe.dataRows     = numberPorts;
	quantityRe.dataColumns  = numberPorts;

	seriesMetadata.AddQuantity(std::move(quantityRe));

	quantityIm.quantityName = "S-Parameter (Im)";
	quantityIm.typeName     = "double";
	quantityIm.dataRows     = numberPorts;
	quantityIm.dataColumns  = numberPorts;

	seriesMetadata.AddQuantity(std::move(quantityIm));

	resultCollectionExtender.AddSeries(std::move(seriesMetadata));
	resultCollectionExtender.setBucketSize(numberPorts * numberPorts);
}

void ParametricResult1DManager::addSparameterSeriesQuantityData(ResultCollectionExtender& resultCollectionExtender, const std::string& seriesName, const std::string& xLabel, size_t numberOfXValues,
																std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*> &sources)
{
	const auto seriesMetadata = resultCollectionExtender.FindMetadataSeries(seriesName);
	const MetadataParameter* parameter = resultCollectionExtender.FindMetadataParameter(xLabel);
	assert(parameter != nullptr);
	if (parameter == nullptr) return;

	std::list<std::string> parameterAbbrev{ parameter->parameterAbbreviation };

	const auto allQuantities = seriesMetadata->getQuantities();

	size_t xIndex = 0;
	for (size_t xIndex = 0; xIndex < numberOfXValues; xIndex++)
	{
		auto currentQuantity = allQuantities.begin();
		double xValue = categoryResults.begin()->second->getXValues()[xIndex];

		for (auto port : sources)
		{
			ot::Variable data = (port == nullptr) ? 0.0 : port->getYreValues()[xIndex];
			resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data);
		}

		currentQuantity++;

		for (auto port : sources)
		{
			ot::Variable data = (port == nullptr) ? 0.0 : port->getYimValues()[xIndex];
			resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data);
		}
	}

	resultCollectionExtender.FlushQuantityContainer();
}
