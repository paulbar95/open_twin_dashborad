//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/Border.h"
#include "OTGui/Margins.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {
	
	class OT_GUI_API_EXPORT GraphicsItemCfg : ot::Serializable, public ot::SimpleFactoryObject {
	public:
		GraphicsItemCfg();
		virtual ~GraphicsItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setGraphicsItemName(const std::string& _name) { m_name = _name; };
		const std::string& graphicsItemName(void) const { return m_name; };

		void setBorder(const Border& _border) { m_border = _border; };
		const Border& border(void) const { return m_border; };

		void setMargin(const Margins& _margin) { m_margins = _margin; };
		const Margins& margin(void) const { return m_margins; };

	private:
		std::string m_name;
		Border m_border;
		Margins m_margins;

	};
}