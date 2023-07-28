/*****************************************************************//**
 * \file   VariableType.h
 * \brief  Classes that convert from the JSON format that is being used for the inter service communication to the internally used variant and vice versa.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <variant>
#include <stdint.h>
#include "OpenTwinCore/rJSONHelper.h"
#include "openTwinCore/CoreAPIExport.h"

namespace ot
{
	using variable_t = std::variant<int32_t, int64_t, bool, float, double, const char*>;

	class VariableToJSONConverter
	{
	public:
		rapidjson::Value operator() (variable_t&& value);
	};

	class JSONToVariableConverter
	{
	public:
		variable_t operator() (rapidjson::Value& value);
	};

}
