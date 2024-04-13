//! @file PropertyBool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/PropertyBool.h"
#include "OTCore/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyBool> propertyBoolRegistrar(OT_PROPERTY_TYPE_Bool);

ot::PropertyBool::PropertyBool(const PropertyBool* _other)
	: Property(_other), m_value(_other->m_value)
{}

ot::Property* ot::PropertyBool::createCopy(void) const {
	return new PropertyBool(this);
}

void ot::PropertyBool::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
}

void ot::PropertyBool::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getBool(_object, "Value");
}