#pragma once
#include <stdint.h>
#include <string>
#include <map>
#include <list>

#include "OpenTwinCore/Variable.h"

struct MetadataQuantity
{
	std::string quantityName;
	std::string quantityAbbreviation;
	std::string typeName;
	int32_t quantityIndex;
	std::map < std::string, std::list<ot::Variable>> metaData;
};