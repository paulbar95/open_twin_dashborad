//! @file GraphicsStackItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsStackItem : public QGraphicsItemGroup, public QGraphicsLayoutItem, public ot::GraphicsItem {
	public:
		struct GraphicsStackItemEntry {
			ot::GraphicsItem* item;
			bool isMaster;
			bool isSlave;
		};

		GraphicsStackItem();
		virtual ~GraphicsStackItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void removeAllConnections(void) override;

		virtual void prepareGraphicsItemGeometryChange(void) override;

		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual void graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlags _flags) override;

		//! @brief Will be called when this item was registered as an event handler and the child raised an event
		virtual void graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) override;

		virtual void setGraphicsItemRequestedSize(const QSizeF& _size) override;

		virtual QSizeF graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const;

		virtual QGraphicsLayoutItem* getQGraphicsLayoutItem(void) override { return this; };
		virtual QGraphicsItem* getQGraphicsItem(void) override { return this; };

		virtual ot::GraphicsItem* findItem(const std::string& _itemName) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsItem

		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

		virtual QRectF boundingRect(void) const override;

		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: QGraphicsLayoutItem

		virtual QSizeF sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addItem(ot::GraphicsItem* _item, bool _isMaster, bool _isSlave);

	private:
		void memClear(void);
		void adjustChildItems(void);

		std::list<GraphicsStackItemEntry> m_items;
		QSizeF m_lastCalculatedSize;

		GraphicsStackItem(const GraphicsStackItem&) = delete;
		GraphicsStackItem& operator = (const GraphicsStackItem&) = delete;
	};


}