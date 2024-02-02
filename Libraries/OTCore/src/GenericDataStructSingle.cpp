#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/VariableToJSONConverter.h"

ot::GenericDataStructSingle::GenericDataStructSingle()
	:ot::GenericDataStruct(getClassName(), 1)
{
}

ot::GenericDataStructSingle::~GenericDataStructSingle()
{
}

void ot::GenericDataStructSingle::setValue(const ot::Variable& value)
{
	_value = value;
}

void ot::GenericDataStructSingle::setValue(ot::Variable&& value)
{
	_value = std::move(value);
}

const ot::Variable& ot::GenericDataStructSingle::getValue() const
{
	return _value;
}

void ot::GenericDataStructSingle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	VariableToJSONConverter converter;
	_object.AddMember("Value", converter(_value, _allocator), _allocator);
}

void ot::GenericDataStructSingle::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	JSONToVariableConverter converter;
	_value = converter(_object["Value"]);
}
