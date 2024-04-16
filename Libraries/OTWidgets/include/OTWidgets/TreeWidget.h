//! @file TreeWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qlist.h>
#include <QtWidgets/qtreewidget.h>

// std header
#include <list>

namespace ot {

	class OT_WIDGETS_API_EXPORT TreeWidgetItemInfo {
	public:
		TreeWidgetItemInfo();
		TreeWidgetItemInfo(const NavigationTreeItem& _config);
		TreeWidgetItemInfo(const TreeWidgetItemInfo& _other);
		virtual ~TreeWidgetItemInfo();

		TreeWidgetItemInfo& operator = (const TreeWidgetItemInfo& _other);

		void setText(const QString& _text) { m_text = _text; };
		const QString& text(void) const { return m_text; };

		void setIcon(const QIcon& _icon) { m_icon = _icon; };
		const QIcon& icon(void) const { return m_icon; };

		void setFlag(NavigationTreeItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(const NavigationTreeItemFlags _flags) { m_flags = _flags; };
		NavigationTreeItemFlags flags(void) const { return m_flags; };

		void addChildItem(const TreeWidgetItemInfo& _info);
		void setChildItems(const std::list<TreeWidgetItemInfo>& _childs) { m_childs = _childs; };
		const std::list<TreeWidgetItemInfo>& childItems(void) const { return m_childs; };
		void clearChildItems(void);

	private:
		QString m_text;
		QIcon m_icon;
		NavigationTreeItemFlags m_flags;
		std::list<TreeWidgetItemInfo> m_childs;

	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT TreeWidget : public QTreeWidget, public ot::QWidgetInterface {
		Q_OBJECT
	public:
		struct ItemInformation {
			QString text;
			QIcon icon;
			NavigationTreeItemFlag flags;
		};

		TreeWidget(QWidget * _parentWidget = (QWidget*)nullptr);
		virtual ~TreeWidget();

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };

		virtual void mousePressEvent(QMouseEvent* _event) override;

		virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override;

		QTreeWidgetItem* findItem(const QString& _itemPath, char _delimiter = '/') const;
		bool itemExists(const QString& _itemPath, char _delimiter = '/') const { return this->findItem(_itemPath, _delimiter); };

		QString itemPath(QTreeWidgetItem* _item, char _delimiter = '/') const;

		QTreeWidgetItem* addItem(const TreeWidgetItemInfo& _item);

	private Q_SLOTS:
		void slotColorStyleAboutToChange(void);
		void slotColorStyleChanged(const ColorStyle& _style);

	private:
		std::list<int> m_columnWidths;

		QTreeWidgetItem* findItem(QTreeWidgetItem* _item, const QStringList& _childPath) const;

		QTreeWidgetItem* addItem(QTreeWidgetItem* _parent, const TreeWidgetItemInfo& _item);
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT TreeWidgetItem : public QTreeWidgetItem {
	public:
		TreeWidgetItem(int _type = 0);
		TreeWidgetItem(const TreeWidgetItemInfo& _itemInfo, int _type = 0);
		virtual ~TreeWidgetItem();

		void setNavigationItemFlag(NavigationTreeItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setNavigationItemFlags(NavigationTreeItemFlags _flags) { m_flags = _flags; };
		NavigationTreeItemFlags navigationItemFlags(void) const { return m_flags; };

		//! @brief Returns the item info
		//! The information returned only contains the path to this item (other childs of parent items are ignored)
		TreeWidgetItemInfo getFullInfo(void) const;

		void expandAllParents(bool _expandThis = false);

	private:
		NavigationTreeItemFlags m_flags;
	};
}