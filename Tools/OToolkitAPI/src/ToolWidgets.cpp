//! @file ToolWidgets.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit API header
#include "OToolkitAPI/ToolWidgets.h"

otoolkit::ToolWidgets::ToolWidgets()
	: m_widget(nullptr), m_toolBar(nullptr)
{}

otoolkit::ToolWidgets::ToolWidgets(const ToolWidgets& _other) 
{
	*this = _other;
}

otoolkit::ToolWidgets& otoolkit::ToolWidgets::operator = (const ToolWidgets& _other) {
	m_widget = _other.m_widget;
	m_toolBar = _other.m_toolBar;
	m_statusWidgets = _other.m_statusWidgets;

	return *this;
}
