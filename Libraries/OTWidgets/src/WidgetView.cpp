//! @file WidgetView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewManager.h"

// ADS header
#include <ads/DockWidget.h>
#include <ads/DockAreaWidget.h>
#include <ads/DockManager.h>

ot::WidgetView::WidgetView()
	: m_isProtected(false), m_isDeletedByManager(false),
	m_isModified(false), m_dockWidget(nullptr)
{
	m_dockWidget = new ads::CDockWidget("");
	m_dockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
	m_dockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
}

ot::WidgetView::~WidgetView() {
	m_dockWidget->takeWidget();

	if (!m_isDeletedByManager) {
		WidgetViewManager::instance().forgetView(this);
	}

	ads::CDockManager* adsManager = m_dockWidget->dockManager();
	if (adsManager) {
		adsManager->removeDockWidget(m_dockWidget);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

bool ot::WidgetView::setupFromConfig(WidgetViewCfg* _config) {
	this->setViewData(*_config);
	return true;
}

QAction* ot::WidgetView::getViewToggleAction(void) const {
	if (!m_dockWidget) return nullptr;
	else return m_dockWidget->toggleViewAction();
}

void ot::WidgetView::setViewData(const WidgetViewBase& _data) {
	m_data = _data;
	m_dockWidget->setObjectName(QString::fromStdString(_data.name()));
	m_dockWidget->toggleViewAction()->setText(QString::fromStdString(_data.title()));
	this->setViewContentModified(m_isModified);
}

void ot::WidgetView::setViewContentModified(bool _isModified) {
	m_isModified = _isModified;
	m_dockWidget->setWindowTitle((m_isModified ? QString::fromStdString(m_data.title()) + "*" : QString::fromStdString(m_data.title())));
}

QString ot::WidgetView::currentViewTitle(void) const {
	return m_dockWidget->windowTitle();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::WidgetView::addWidgetToDock(QWidget* _widget) {
	m_dockWidget->setWidget(_widget);
}