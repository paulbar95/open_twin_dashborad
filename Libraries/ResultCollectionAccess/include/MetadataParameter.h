#pragma once
#include "OpenTwinCore/Variable.h"
#include "MetadataEntry.h"

#include <string>
#include <list>
#include <map>
#include <stdint.h>
#include <memory>

struct __declspec(dllexport) MetadataParameter
{
	std::string parameterName;
	std::string parameterAbbreviation;
	std::string typeName;
	std::list<ot::Variable> values;
	std::map < std::string, std::shared_ptr<MetadataEntry>> metaData;
};