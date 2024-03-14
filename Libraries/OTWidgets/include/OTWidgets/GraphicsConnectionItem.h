//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreTypes.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtGui/qpen.h>

namespace ot {
	
	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public QGraphicsItem {
		OT_DECL_NOCOPY(GraphicsConnectionItem)
	public:
		enum GraphicsItemState {
			NoState = 0x00, //! @brief Default state
			HoverState = 0x01, //! @brief Item is hovered over by user
			SelectedState = 0x02  //! @brief Item is selected
		};

		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// QGraphicsItem

		virtual QRectF boundingRect(void) const override;
		virtual void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;
		virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* _event) override;
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Will set the connection properties as provided in the configuration
		//! The origin and destination item won't be set!
		bool setupFromConfig(const ot::GraphicsConnectionCfg& _cfg);

		//! @brief Creates a configuration object containing the items origin and destination information
		GraphicsConnectionCfg getConnectionInformation(void) const;

		void updateConnection(void);

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		GraphicsItem* originItem(void) const { return m_origin; };
		GraphicsItem* destItem(void) const { return m_dest; };

		void setUid(const ot::UID& _uid) { m_uid = _uid; };
		const ot::UID& uid(void) const { return m_uid; };

		const std::string& getConnectionKey() const { return m_connectionKey; };


	private:
		void calculateDirectLinePoints(QPointF& _origin, QPointF& _destination) const;
		void calculateSmoothLinePoints(QPointF& _origin, QPointF& _control1, QPointF& _control2, QPointF& _destination) const;
		void calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const;

		GraphicsItemState m_state;

		GraphicsItem* m_origin;
		GraphicsItem* m_dest;

		std::string m_connectionKey;

		ot::UID m_uid;

		QRectF m_lastRect;

		ot::GraphicsConnectionCfg::ConnectionStyle m_style;
		QPen m_pen;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsConnectionItem::GraphicsItemState)