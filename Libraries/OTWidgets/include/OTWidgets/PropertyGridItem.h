//! @file PropertyGridEntry.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class Property;
	class PropertyInput;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QTreeWidgetItem {
		OT_DECL_NOCOPY(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		//! @brief Setup the item
		//! Note that the item must be placed in a tree before calling the setup
		virtual bool setupFromConfig(const Property* _config);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void setInput(PropertyInput* _input);
		PropertyInput* getInput(void) const { return m_input; };
		
		void setPropertyBrush(const QBrush& _brush) { m_propertyBrush = _brush; };
		const QBrush& propertyBrush(void) const { return m_propertyBrush; };

	private:
		std::string m_name;
		PropertyInput* m_input;
		QBrush m_propertyBrush;
	};

}