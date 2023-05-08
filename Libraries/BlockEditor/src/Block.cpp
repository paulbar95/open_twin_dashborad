//! @file Block.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/Block.h"

ot::Block::Block() {}

ot::Block::~Block() {}

QRectF ot::Block::boundingRect(void) const {
	return QRectF(
		QPointF(pos().x() - (blockWidth() / 2), pos().y() - (blockHeigth() / 2)), 
		QSizeF(blockWidth(), blockHeigth())
	);
}