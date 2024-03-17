//! @file GraphicsStackItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/KeyMap.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsStackItem.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsStackItem> stackItem(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);
static ot::GlobalKeyMapRegistrar stackItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);

ot::GraphicsStackItem::GraphicsStackItem() 
	: ot::GraphicsGroupItem(true), m_lastCalculatedSize(-1., -1.) 
{
	//this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
}

ot::GraphicsStackItem::~GraphicsStackItem() {
	//this->memClear();
}

bool ot::GraphicsStackItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsStackItemCfg* cfg = dynamic_cast<ot::GraphicsStackItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->memClear();

	for (auto itm : cfg->items()) {
		OTAssertNullptr(itm.item);

		ot::GraphicsItem* i = nullptr;
		try {
			i = ot::GraphicsFactory::itemFromConfig(itm.item);
			if (i) {
				this->addItem(i, itm.isMaster, itm.isSlave);
			}
			else {
				OT_LOG_EA("Failed to created graphics item from factory");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item: " + std::string(_e.what()));
			if (i) delete i;
			throw _e;
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			if (i) delete i;
			throw std::exception("[FATAL] Unknown error");
		}
	}

	return ot::GraphicsGroupItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::removeAllConnections(void) {
	ot::GraphicsGroupItem::removeAllConnections();
	for (auto itm : m_items) {
		itm.item->removeAllConnections();
	}
}

void ot::GraphicsStackItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
	//for (auto itm : m_items) itm.item->callPaint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->adjustChildItems();
	ot::GraphicsGroupItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlag _flags) {
	ot::GraphicsGroupItem::graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsStackItem::graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {
	if (_event == ot::GraphicsItem::ItemResized) {
		this->adjustChildItems();
	}
}

void ot::GraphicsStackItem::memClear(void) {
	for (auto itm : m_items) delete itm.item;
	m_items.clear();
}

void ot::GraphicsStackItem::adjustChildItems(void) {
	QSizeF masterSize(-1., -1.);
	for (auto itm : m_items) {
		if (itm.isMaster) {
			//OT_LOG_D("< Handling ItemResized: MasterDetected { \"Root.UID\": \"" + itm.item->getRootItem()->graphicsItemUid() + "\", \"Item.Name\": \"" + itm.item->graphicsItemName() + "\" }");
			masterSize = masterSize.expandedTo(itm.item->getQGraphicsItem()->boundingRect().size());
		}
	}

	if (masterSize.width() < 0. || masterSize.height() < 0.) {
		OT_LOG_WA("It appears that no master item was provided to this stack item");
		return;
	}

	if (masterSize == m_lastCalculatedSize) return;

	this->prepareGeometryChange();

	m_lastCalculatedSize = masterSize;

	for (GraphicsStackItemEntry itm : m_items) {
		if (itm.isSlave) {
			//OT_LOG_D("< Handling ItemResized: SlaveDetected { \"Root.UID\": \"" + itm.item->getRootItem()->graphicsItemUid() + "\", \"Item.Name\": \"" + itm.item->graphicsItemName() + "\" }");
			itm.item->setGraphicsItemRequestedSize(m_lastCalculatedSize);
		}
	}
}

QSizeF ot::GraphicsStackItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	QSizeF s;
	for (auto itm : m_items) {
		if (itm.isMaster) {
			s = s.expandedTo(itm.item->graphicsItemSizeHint(_hint, _constrains));
		}
		else {
			s = s.expandedTo(itm.item->graphicsItemSizeHint(Qt::MinimumSize, _constrains));
		}
	}
	return s;
}

QRectF ot::GraphicsStackItem::boundingRect(void) const {
	QRectF ret = GraphicsGroupItem::boundingRect();
	QPointF tl = ret.topLeft();
	QSizeF s = ret.size();

	for (auto itm : m_items) {
		QRectF r = itm.item->getQGraphicsItem()->boundingRect();
		tl.setX(std::min(tl.x(), r.topLeft().x()));
		tl.setY(std::min(tl.y(), r.topLeft().y()));
		s = s.expandedTo(QSizeF(r.topLeft().x() + r.size().width(), r.topLeft().y() + r.size().height()));
	}
	return this->handleGetGraphicsItemBoundingRect(QRectF(tl, s));
}

void ot::GraphicsStackItem::setGraphicsItemRequestedSize(const QSizeF& _size) {
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
	for (auto itm : m_items) {
		itm.item->setGraphicsItemRequestedSize(_size);
	}
}

void ot::GraphicsStackItem::addItem(ot::GraphicsItem* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	_item->setParentGraphicsItem(this);
	if (_isMaster) {
		// If the item is a master item, install an event filter for resizing the child items
		_item->addGraphicsItemEventHandler(this);
	}
	m_items.push_back(e);

	this->addToGroup(e.item->getQGraphicsItem());
}