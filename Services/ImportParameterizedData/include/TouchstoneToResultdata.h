#pragma once
#include "BusinessLogicHandler.h"

#include "TouchstoneHandler.h"
#include "MetadataSeries.h"

class TouchstoneToResultdata : public BusinessLogicHandler
{
public:
	TouchstoneToResultdata();
	~TouchstoneToResultdata();
	int getAssumptionOfPortNumber(const std::string& fileName);
	void SetResultdata(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength);
	void CreateResultdata(int numberOfPorts);

private:
	std::string _collectionName = "";
	std::string _fileName = "";
	std::string _fileContent = "";
	uint64_t _uncompressedLength = 0;

	const std::string CreateSeriesName(const std::string& fileName);
	bool SeriesAlreadyExists(const std::string& seriesName);

	TouchstoneHandler ImportTouchstoneFile(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength, int numberOfPorts);
	void BuildSeriesMetadataFromTouchstone(TouchstoneHandler& touchstoneHandler, MetadataSeries& series, int numberOfPorts);
	//std::string DetermineDominantType(const std::list<std::string>& typeNames);
};

