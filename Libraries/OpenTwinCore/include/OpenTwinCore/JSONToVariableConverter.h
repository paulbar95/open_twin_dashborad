#pragma once
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport)  JSONToVariableConverter
	{
	public:
		Variable operator() (rapidjson::Value& value);
	};
}