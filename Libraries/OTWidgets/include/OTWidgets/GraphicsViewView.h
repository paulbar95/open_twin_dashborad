//! @file GraphicsViewView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/GraphicsView.h"

#define OT_WIDGETTYPE_GraphicsView "GraphicsView"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsViewView : public GraphicsView, public WidgetView {
	public:
		GraphicsViewView();
		virtual ~GraphicsViewView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual bool setupFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}
