#pragma once
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/rJSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport) VariableToJSONConverter
	{
	public:
		OT_rJSON_val operator() (const Variable& value, OT_rJSON_doc& emebeddingDocument);
		OT_rJSON_val operator() (const std::list<Variable>& value, OT_rJSON_doc& emebeddingDocument);
	};
}
