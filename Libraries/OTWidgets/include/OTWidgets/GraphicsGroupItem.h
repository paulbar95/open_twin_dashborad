//! @file GraphicsGroupItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsGroupItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		GraphicsGroupItem(bool _isStackOrLayout = false);
		virtual ~GraphicsGroupItem();

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: GraphicsItem

		virtual QRectF boundingRect(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: GraphicsLayoutItem

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual void setGeometry(const QRectF& rect) override;

		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual void graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlags _flags) override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

	private:
		GraphicsGroupItem(const GraphicsGroupItem&) = delete;
		GraphicsGroupItem& operator = (const GraphicsGroupItem&) = delete;
	};

}