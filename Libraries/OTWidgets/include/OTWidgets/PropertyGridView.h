//! @file PropertyGridView.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/PropertyGrid.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PropertyGridView : public PropertyGrid, public WidgetView {
		OT_DECL_NOCOPY(PropertyGridView)
	public:
		PropertyGridView();
		virtual ~PropertyGridView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

	};

}