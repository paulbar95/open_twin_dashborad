//! @file GraphicsPolygonItem.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsPolygonItem : public CustomGraphicsItem {
		OT_DECL_NOCOPY(GraphicsPolygonItem)
	public:
		GraphicsPolygonItem();
		virtual ~GraphicsPolygonItem();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::GraphicsItem

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions: ot::CustomGraphicsItem

		virtual QSizeF getPreferredGraphicsItemSize(void) const override;

	protected:

		//! @brief Paint the item inside the provided rect
		virtual void paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

	public:
		void setPlaneBrush(const QBrush& _brush) { m_brush = _brush; };
		const QBrush& planeBrush(void) const { return m_brush; };

		void setOutlinePen(const QPen& _pen) { m_pen = _pen; };
		const QPen& outlinePen(void) const { return m_pen; };

	private:
		QBrush m_brush;
		QPen m_pen;
	};
}