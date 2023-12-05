//! @file GraphicsTextItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Text "Text"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextPainter "TextPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg);

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text), m_textPainter(nullptr) 
{
	this->setTextColor(_textColor);
}

ot::GraphicsTextItemCfg::~GraphicsTextItemCfg() {}

void ot::GraphicsTextItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_Text, JsonString(m_text, _allocator), _allocator);
	
	JsonObject fontObj;
	m_textFont.addToJsonObject(fontObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_TextFont, fontObj, _allocator);

	JsonObject colorObj;
	m_textPainter->addToJsonObject(colorObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_TextPainter, colorObj, _allocator);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, OT_JSON_MEMBER_Text);
	m_textFont.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_TextFont));
	
	ConstJsonObject colorObj = json::getObject(_object, OT_JSON_MEMBER_TextPainter);
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(colorObj);
	p->setFromJsonObject(colorObj);
	this->setTextPainter(p);
}

void ot::GraphicsTextItemCfg::setTextColor(const ot::Color& _color) {
	this->setTextPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsTextItemCfg::setTextPainter(ot::Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (m_textPainter) {
		delete m_textPainter;
	}
	m_textPainter = _painter;
}