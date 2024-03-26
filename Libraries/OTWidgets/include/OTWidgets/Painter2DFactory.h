//! @file Painter2DFactory.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qbrush.h>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DFactory {
		OT_DECL_NOCOPY(Painter2DFactory)
	public:
		static QBrush brushFromPainter2D(const ot::Painter2D* _painter);

	private:
		Painter2DFactory() {};
		~Painter2DFactory() {};
	};
}