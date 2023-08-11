//! @file GraphicsLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

// Qt header
#include <QtWidgets/qgraphicswidget.h>

ot::GraphicsLayoutItem::GraphicsLayoutItem() : m_layoutWrap(nullptr) {}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {}

void ot::GraphicsLayoutItem::finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup* _group, bool _isRoot) {
	std::list<QGraphicsLayoutItem*> lst;
	this->getAllItems(lst);

	for (auto itm : lst) {
		// Finalize child
		ot::GraphicsItem* gi = dynamic_cast<ot::GraphicsItem*>(itm);
		if (gi) gi->finalizeItem(_scene, _group, false);
		else {
			OT_LOG_EA("Failed to cast GrahicsLayoutItem child to GraphicsItem");
		}
	}

	if (_isRoot) {
		otAssert(m_layoutWrap == nullptr, "Should not be happening");

		QGraphicsLayout* lay = dynamic_cast<QGraphicsLayout *>(this);
		if (lay == nullptr) {
			OT_LOG_EA("OT::GraphicsLayoutItem cast to QGraphicsLayout failed");
		}
		else {
			// Add wrapped layout item
			m_layoutWrap = new QGraphicsWidget;
			m_layoutWrap->setLayout(lay);

			if (_group) _group->addToGroup(m_layoutWrap);
			_scene->addItem(m_layoutWrap);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem) {}

bool ot::GraphicsBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsBoxLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsBoxLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	for (auto itm : cfg->items()) {
		if (itm.first) {
			ot::GraphicsItem* i = ot::GraphicsFactory::itemFromConfig(itm.first);
			if (i == nullptr) {
				OT_LOG_EA("GraphicsFactory failed");
				return false;
			}
			QGraphicsLayoutItem* ii = dynamic_cast<QGraphicsLayoutItem*>(i);
			if (ii) {
				this->addItem(ii);
			}
			else {
				OT_LOG_EA("GraphicsItem cast to QGraphicsLayoutItem failed");
			}
		}
		else {
			this->addStretch(itm.second);
		}
	}

	return true;
}

void ot::GraphicsBoxLayoutItem::getAllItems(std::list<QGraphicsLayoutItem *>& _items) const {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) _items.push_back(this->itemAt(i));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsVBoxLayoutItem::GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayoutItem(Qt::Vertical, _parentItem) {}

bool ot::GraphicsVBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsHBoxLayoutItem::GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayoutItem(Qt::Horizontal, _parentItem) {}

bool ot::GraphicsHBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGridLayoutItem::GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem) : QGraphicsGridLayout(_parentItem) {}

bool ot::GraphicsGridLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {


	return true;
}

void ot::GraphicsGridLayoutItem::getAllItems(std::list<QGraphicsLayoutItem*>& _items) const {
	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			if (this->itemAt(r, c)) _items.push_back(this->itemAt(r, c));
		}
	}
}

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItem> vBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItem> hBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsGridLayoutItem> gridItem(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar vBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::GlobalKeyMapRegistrar hBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::GlobalKeyMapRegistrar gridItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);