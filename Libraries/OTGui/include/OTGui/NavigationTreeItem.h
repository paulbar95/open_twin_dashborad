//! @file NavigationTreeItem.h
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	enum NavigationTreeItemFlag {
		NoNavigationTreeItemFlags = 0x00, //! @brief Empty mask, no flags set
		RemoveItemWhenEmpty       = 0x01, //! @brief If set the item will be removed when all the child items are removed
		ItemMayBeAdded            = 0x02, //! @brief If set the item may be added/dragged/dropped
		ItemIsSelected            = 0x04  //! @brief If set this item is selected
	};

	OT_GUI_API_EXPORT std::string toString(NavigationTreeItemFlag _flag);
	OT_GUI_API_EXPORT std::list<std::string> toStringList(NavigationTreeItemFlag _flags);
	OT_GUI_API_EXPORT NavigationTreeItemFlag stringToNavigationItemFlag(const std::string& _flag);
	OT_GUI_API_EXPORT NavigationTreeItemFlag stringListToNavigationItemFlags(const std::list<std::string>& _flags);

	class OT_GUI_API_EXPORT NavigationTreeItem : public ot::Serializable {
	public:
		NavigationTreeItem();
		NavigationTreeItem(const std::string& _text, ot::NavigationTreeItemFlag _flags = ot::NoNavigationTreeItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, ot::NavigationTreeItemFlag _flags = ot::NoNavigationTreeItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, ot::NavigationTreeItemFlag _flags = ot::NoNavigationTreeItemFlags);
		NavigationTreeItem(const NavigationTreeItem& _other);
		virtual ~NavigationTreeItem();

		NavigationTreeItem& operator = (const NavigationTreeItem& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string text(void) const { return m_text; };

		void setIconPath(const std::string& _iconPath) { m_iconPath = _iconPath; };
		const std::string& iconPath(void) const { return m_iconPath; };

		void addChildItem(const NavigationTreeItem& _item);
		void setChildItems(const std::list<NavigationTreeItem>& _items);
		const std::list<NavigationTreeItem>& childItems(void) const { return m_childs; };

		void setFlags(ot::NavigationTreeItemFlag _flags) { m_flags = _flags; };
		ot::NavigationTreeItemFlag flags(void) const { return m_flags; };

		void merge(const NavigationTreeItem& _other);

		std::string itemPath(char _delimiter = '/', const std::string& _suffix = std::string());

	protected:
		void setParentNavigationTreeItem(NavigationTreeItem* _parent) { m_parent = _parent; };
		NavigationTreeItem* parentNavigationTreeItem(void) const { return m_parent; };

	private:
		NavigationTreeItem* m_parent;

		std::string m_text;
		std::string m_iconPath;
		std::list<NavigationTreeItem> m_childs;
		ot::NavigationTreeItemFlag m_flags;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::NavigationTreeItemFlag);