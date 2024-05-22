//! @file GraphicsItemDesignerNavigationRoot.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "GraphicsItemDesignerPropertyHandler.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

class GraphicsItemDesignerNavigationRoot : public QTreeWidgetItem, public GraphicsItemDesignerPropertyHandler {
public:
	GraphicsItemDesignerNavigationRoot();
	virtual ~GraphicsItemDesignerNavigationRoot() {};

protected:
	virtual void fillPropertyGrid(void) override;
	virtual void propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;
	virtual void propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) override;

private:

};