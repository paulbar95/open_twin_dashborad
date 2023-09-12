#pragma once
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToJSONConverter
	{
	public:
		rapidjson::Value operator() (Variable& value, OT_rJSON_doc& emebeddingDocument);
	};
}