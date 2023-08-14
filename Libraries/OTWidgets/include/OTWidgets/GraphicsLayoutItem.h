//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qgraphicslinearlayout.h>
#include <QtWidgets/qgraphicsgridlayout.h>

class QGraphicsWidget;

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem "OT_GILayV"
#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem "OT_GILayH"
#define OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem "OT_GILayG"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsLayoutItem : public ot::GraphicsItem {
	public:
		GraphicsLayoutItem();
		virtual ~GraphicsLayoutItem();
		
		virtual void finalizeItem(QGraphicsScene* _scene, QGraphicsItemGroup * _group, bool _isRoot) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const = 0;

	protected:
		virtual void callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) override;

	private:
		QGraphicsWidget* m_layoutWrap;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsBoxLayoutItem : public QGraphicsLinearLayout, public ot::GraphicsLayoutItem {
	public:
		GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsBoxLayoutItem() {};

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

	private:
		GraphicsBoxLayoutItem() = delete;
		GraphicsBoxLayoutItem(const GraphicsBoxLayoutItem&) = delete;
		GraphicsBoxLayoutItem& operator = (const GraphicsBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsVBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsVBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsVBoxLayoutItem(const GraphicsVBoxLayoutItem&) = delete;
		GraphicsVBoxLayoutItem& operator = (const GraphicsVBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsHBoxLayoutItem : public GraphicsBoxLayoutItem {
	public:
		GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsHBoxLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

	private:
		QSizeF m_size;

		GraphicsHBoxLayoutItem(const GraphicsHBoxLayoutItem&) = delete;
		GraphicsHBoxLayoutItem& operator = (const GraphicsHBoxLayoutItem&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT GraphicsGridLayoutItem : public QGraphicsGridLayout, public ot::GraphicsLayoutItem {
	public:
		GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem = (QGraphicsLayoutItem*)nullptr);
		virtual ~GraphicsGridLayoutItem() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem); };

		virtual bool setupFromConfig(ot::GraphicsItemCfg* _cfg) override;

		virtual void getAllItems(std::list<QGraphicsLayoutItem*>& _items) const override;

	private:
		QSizeF m_size;

		GraphicsGridLayoutItem(const GraphicsGridLayoutItem&) = delete;
		GraphicsGridLayoutItem& operator = (const GraphicsGridLayoutItem&) = delete;
	};

}