//! @file WidgetTypes.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OpenTwinCore/Flags.h"

namespace ot {

	enum WidgetFlag {
		ApplyFilterOnReturn = 0x01,
		ApplyFilterOnTextChange = 0x02
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetFlag);

namespace ot {

	typedef ot::Flags<ot::WidgetFlag> WidgetFlags;

}