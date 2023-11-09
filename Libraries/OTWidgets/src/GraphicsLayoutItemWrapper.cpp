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

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

QVariant ot::GraphicsLayoutItemWrapper::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	OTAssertNullptr(m_owner);
	m_owner->handleItemChange(_change, _value);
	return QGraphicsWidget::itemChange(_change, _value);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

void ot::GraphicsLayoutItemWrapper::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();

	OTAssertNullptr(m_owner);
	m_owner->removeAllConnections();
}