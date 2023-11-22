//! @file GraphicsLayoutItemWrapper.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsLayoutItemWrapper.h"

ot::GraphicsLayoutItemWrapper::GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner) 
	: ot::GraphicsItem(false), m_owner(_owner)
{
	OTAssertNullptr(m_owner);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsLayoutItemWrapper::~GraphicsLayoutItemWrapper() {}

void ot::GraphicsLayoutItemWrapper::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsLayoutItemWrapper::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMousePressEvent(_event);
	QGraphicsWidget::mousePressEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMouseReleaseEvent(_event);
	QGraphicsWidget::mouseReleaseEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleHoverEnterEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleHoverLeaveEvent(_event);
}

QVariant ot::GraphicsLayoutItemWrapper::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	OTAssertNullptr(m_owner);
	m_owner->handleItemChange(_change, _value);
	return QGraphicsWidget::itemChange(_change, _value);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & GraphicsItemCfg::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & GraphicsItemCfg::ItemIsMoveable);
}

void ot::GraphicsLayoutItemWrapper::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();

	OTAssertNullptr(m_owner);
	m_owner->removeAllConnections();
}

QSizeF ot::GraphicsLayoutItemWrapper::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}

void ot::GraphicsLayoutItemWrapper::setGraphicsItemRequestedSize(const QSizeF& _size) {
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
	this->setPreferredSize(_size);
}