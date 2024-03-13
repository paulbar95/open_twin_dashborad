/*
 *	File:		aWindow.cpp
 *	Package:	akWidgets
 *
 *  Created on: November 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akGui/aWindowEventHandler.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aAnimationOverlayWidget.h>

// Qt header
#include <qevent.h>
#include <qwidget.h>
#include <qframe.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qmovie.h>
#include <qscreen.h>
#include <qlayout.h>
#include <QOpenGLWidget>



ak::aWindow::aWindow()
	: QMainWindow(), aPaintable(otMainWindow), m_waitingWidget(nullptr), m_centralWidget(nullptr)
{
	m_waitingWidget = new QLabel();
	m_centralWidget = new aAnimationOverlayWidget();
	setCentralWidget(m_centralWidget);
}

ak::aWindow::~aWindow() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Base class functions

void ak::aWindow::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr);		// Nullptr provided
	m_colorStyle = _colorStyle;

	/*QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow));

	setStyleSheet(sheet);*/

	// Double paint to not mess up the tab toolbar
	QString sheet = m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow, "QMainWindow{", "}\n");

	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorWindow | cafBackgroundColorWindow,
		"QTabBar{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabBar::tab{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorFocus | cafForegroundColorFocus,
		"QTabBar::tab:hover{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorSelected | cafForegroundColorSelected,
		"QTabBar::tab:selected{", "}"));

	setStyleSheet(sheet);

	auto sB = statusBar();
	if (sB)	statusBar()->setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow | cafBackgroundColorWindow));
}

void ak::aWindow::closeEvent(QCloseEvent * _event) {
	for (auto handler : m_eventHandler) {
		if (!handler.first->closeEvent()) {
			_event->ignore();
			return;
		}
	}
	_event->accept();
	QMainWindow::closeEvent(_event);
}

// #######################################################################################################

// Event handling

void ak::aWindow::addEventHandler(
	aWindowEventHandler *					_eventHandler
) {
	assert(m_eventHandler.find(_eventHandler) == m_eventHandler.end());	// Event handler already set
	m_eventHandler.insert_or_assign(_eventHandler, true);
}

void ak::aWindow::removeEventHandler(
	aWindowEventHandler *					_eventHandler
) { m_eventHandler.erase(_eventHandler); }

void ak::aWindow::SetCentralWidget(
	QWidget *								_widget
) {
	setCentralWidget(m_centralWidget);
	m_centralWidget->setChild(_widget);
}

void ak::aWindow::setWaitingAnimationVisible(
	bool									_visible
) {
	m_centralWidget->setWaitingAnimationVisible(_visible);
}

void ak::aWindow::setWaitingAnimation(
	QMovie *							_movie
) {
	m_centralWidget->setWaitingAnimation(_movie);
}

void ak::aWindow::showEvent(QShowEvent* event)
{
	// Since Qt6, there is the following change (from Qt documentation):
	//    When dynamically adding a QOpenGLWidget into a widget hierarchy, e.g.by parenting a 
	//    new QOpenGLWidget to a widget where the corresponding top - level widget is already 
	//    shown on screen, the associated native window may get implicitly destroyed and recreated 
	//    if the QOpenGLWidget is the first of its kind within its window.This is because the window 
	//    type changes from RasterSurface to OpenGLSurface and that has platform - specific 
	//    implications.This behavior is new in Qt 6.4.

	// This meand that the top window disappears and re-appears when the 3D view tab is added to the
	// scene and therefore an OpenGL view is activated for the first time. In order to fix this,
	// we add a dummy OpenGL window to the main window when it is shown for the very first time and delete
	// it again right away. This fixes the flickering of the main window.

	QOpenGLWidget w;

	layout()->addWidget(&w);
	layout()->removeWidget(&w);
}
