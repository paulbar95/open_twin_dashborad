//! @file NavigationTreeView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

#include "akWidgets/aTreeWidget.h"

#define OT_WIDGETTYPE_NavigationTree "NavigationTree"

namespace ot {

	class NavigationTreeView : public ak::aTreeWidget, public WidgetView {
	public:
		NavigationTreeView();
		virtual ~NavigationTreeView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual QWidget* getViewWidget(void) override;

		virtual bool setupFromConfig(WidgetViewCfg* _config) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}
