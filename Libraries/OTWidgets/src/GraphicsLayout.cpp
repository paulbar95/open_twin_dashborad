//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTWidgets/GraphicsLayout.h"

ot::GraphicsBoxLayout::GraphicsBoxLayout(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem) {}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsVBoxLayout::GraphicsVBoxLayout(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayout(Qt::Vertical, _parentItem) {}

void ot::GraphicsVBoxLayout::setupFromConfig(ot::GraphicsItemCfg* _cfg) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsHBoxLayout::GraphicsHBoxLayout(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayout(Qt::Horizontal, _parentItem) {}

void ot::GraphicsHBoxLayout::setupFromConfig(ot::GraphicsItemCfg* _cfg) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGridLayout::GraphicsGridLayout(QGraphicsLayoutItem* _parentItem) : QGraphicsGridLayout(_parentItem) {}

void ot::GraphicsGridLayout::setupFromConfig(ot::GraphicsItemCfg* _cfg) {

}
