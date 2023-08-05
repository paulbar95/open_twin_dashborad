//! @file GraphicsItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::GraphicsItemCfg::GraphicsItemCfg() {}

ot::GraphicsItemCfg::~GraphicsItemCfg() {}

void ot::GraphicsItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, "Border", borderObj);

	OT_rJSON_createValueObject(marginObj);
	m_margins.addToJsonObject(_document, marginObj);
	ot::rJSON::add(_document, _object, "Margin", marginObj);

	ot::rJSON::add(_document, _object, "GIName", m_name);
}

void ot::GraphicsItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Border", Object);
	OT_rJSON_checkMember(_object, "Margin", Object);
	OT_rJSON_checkMember(_object, "GI", String);

	OT_rJSON_val borderObj = _object["Border"].GetObject();
	OT_rJSON_val marginObj = _object["Margin"].GetObject();

	m_border.setFromJsonObject(borderObj);
	m_margins.setFromJsonObject(marginObj);
}