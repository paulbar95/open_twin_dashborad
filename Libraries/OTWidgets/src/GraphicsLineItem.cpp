//! @file GraphicsLineItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLineItem.h"

ot::GraphicsLineItem::GraphicsLineItem() 
	: ot::GraphicsItem(false)
{
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsLineItem::~GraphicsLineItem() {

}

bool ot::GraphicsLineItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	// Ignore, return false in case the setup is called to ensure this item wont be setup
	return false;
}

void ot::GraphicsLineItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsLineItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
}

void ot::GraphicsLineItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	this->setPos(_rect.topLeft());
	QGraphicsLayoutItem::setGeometry(_rect);

	this->handleSetItemGeometry(_rect);
}

QRectF ot::GraphicsLineItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsLineItem::boundingRect());
	//return QGraphicsLineItem::boundingRect();
}

QVariant ot::GraphicsLineItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsLineItem::itemChange(_change, _value);
}

void ot::GraphicsLineItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLineItem::graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlags _flags) {
	// Ignore
}

QSizeF ot::GraphicsLineItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}