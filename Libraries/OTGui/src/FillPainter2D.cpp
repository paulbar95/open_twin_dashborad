//! @file FillPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/FillPainter2D.h"

static ot::SimpleFactoryRegistrar<ot::FillPainter2D> fillCfg(OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg);

ot::FillPainter2D::FillPainter2D() {}

ot::FillPainter2D::FillPainter2D(const ot::Color& _color) : m_color(_color) {}

ot::FillPainter2D::~FillPainter2D() {}

void ot::FillPainter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::Painter2D::addToJsonObject(_object, _allocator);
	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember("Color", colorObj, _allocator);
}

void ot::FillPainter2D::setFromJsonObject(const ConstJsonObject& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	m_color.setFromJsonObject(json::getObject(_object, "Color"));
}

std::string ot::FillPainter2D::generateQss(void) const {
	return "rgba(" + std::to_string(m_color.rInt()) + "," + std::to_string(m_color.gInt()) + "," + std::to_string(m_color.bInt()) + "," + std::to_string(m_color.aInt()) + ")";
}