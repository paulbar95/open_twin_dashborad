//! @file BlockPickerWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPickerWidget.h"
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/BlockNetworkEditor.h"

// Qt Header
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>

ot::BlockPickerWidget::BlockPickerWidget(Qt::Orientation _orientation) : m_navigation(nullptr), m_view(nullptr), m_scene(nullptr), m_splitter(nullptr) {
	// Create controls
	m_splitter = new QSplitter(_orientation);
	m_navigation = new QTreeWidget;
	m_view = new GraphicsView;
	m_scene = new GraphicsScene;
}

ot::BlockPickerWidget::~BlockPickerWidget() {

}

QWidget* ot::BlockPickerWidget::widget(void) {
	return m_splitter;
}

void ot::BlockPickerWidget::setOrientation(Qt::Orientation _orientation) {
	m_splitter->setOrientation(_orientation);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::BlockPicker::BlockPicker(QWidget* _parentWidget)
	: QDockWidget(_parentWidget), m_widget(nullptr)
{
	m_widget = new BlockPickerWidget(this->calcWidgetOrientation());
}

ot::BlockPicker::BlockPicker(const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget), m_widget(nullptr)
{
	m_widget = new BlockPickerWidget(this->calcWidgetOrientation());
}

ot::BlockPicker::~BlockPicker() {
	if (m_widget) delete m_widget;
}

void ot::BlockPicker::resizeEvent(QResizeEvent* _event) {
	QDockWidget::resizeEvent(_event);
	m_widget->setOrientation(this->calcWidgetOrientation());
}