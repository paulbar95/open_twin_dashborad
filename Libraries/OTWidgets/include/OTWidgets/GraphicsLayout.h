//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicslinearlayout.h>
#include <QtWidgets/qgraphicsgridlayout.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayout : public QGraphicsLinearLayout {
	public:
		GraphicsBoxLayout(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayout() {};

	private:
		GraphicsBoxLayout() = delete;
		GraphicsBoxLayout(const GraphicsBoxLayout&) = delete;
		GraphicsBoxLayout& operator = (const GraphicsBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayout : public GraphicsBoxLayout {
	public:
		GraphicsVBoxLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayout() {};

	private:
		GraphicsVBoxLayout(const GraphicsVBoxLayout&) = delete;
		GraphicsVBoxLayout& operator = (const GraphicsVBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayout : public GraphicsBoxLayout {
	public:
		GraphicsHBoxLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayout() {};

	private:
		GraphicsHBoxLayout(const GraphicsHBoxLayout&) = delete;
		GraphicsHBoxLayout& operator = (const GraphicsHBoxLayout&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGridLayout : public QGraphicsGridLayout {
	public:
		GraphicsGridLayout(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayout() {};

	private:
		GraphicsGridLayout(const GraphicsGridLayout&) = delete;
		GraphicsGridLayout& operator = (const GraphicsGridLayout&) = delete;
	};

}