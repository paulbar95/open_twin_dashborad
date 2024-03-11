/*
 *	File:		aTreeWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 10, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akCore/aAssert.h>
#include <akGui/aColor.h>
#include <akGui/aColorStyle.h>

#include <akWidgets/aTreeWidget.h>
#include <akWidgets/aLineEditWidget.h>

// Qt header
#include <qmessagebox.h>
#include <qmargins.h>
#include <qevent.h>

#include <qheaderview.h>

ak::aTreeWidget::aTreeWidget(
	aColorStyle *	_colorStyle
) : ak::aWidget(otTree, _colorStyle),
	m_tree(nullptr), m_filter(nullptr), m_layout(nullptr),
	m_filterCaseSensitive(false), m_filterRefreshOnChange(true), m_currentId(0), m_itemsAreEditable(false),
	m_selectAndDeselectChildren(false), m_ignoreEvents(false), m_focusedItem(invalidID), m_isReadOnly(false),
	m_displayChildsOnFilter(false)
{
	// Create tree
	m_tree = new ak::aTreeWidgetBase(this, m_colorStyle);
	assert(m_tree != nullptr); // Failed to create

	// Create filter
	m_filter = new aLineEditWidget();
	assert(m_filter != nullptr); // Failed to create
	m_filter->setVisible(false);
	m_filter->setPlaceholderText("Search...");

	// Create widget
	m_widget = new QWidget;
	m_widget->setContentsMargins(0, 0, 0, 0);

	// Create layout
	m_layout = new QVBoxLayout(m_widget);
	assert(m_layout != nullptr); // Failed to create
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->addWidget(m_filter);
	m_layout->addWidget(m_tree);

	m_tree->setHeaderLabel(QString(""));

	m_tree->setMouseTracking(true);
	m_tree->setHeaderHidden(true);
	m_tree->header()->setSortIndicator(0, Qt::SortOrder::AscendingOrder);
	m_tree->setSortingEnabled(true);
	m_tree->sortByColumn(0, Qt::AscendingOrder);

	connect(m_filter, &aLineEditWidget::keyPressed, this, &aTreeWidget::slotFilterKeyPressed);
	connect(m_filter, &QLineEdit::textChanged, this, &aTreeWidget::slotFilterTextChanged);

	connect(m_tree, &aTreeWidgetBase::keyPressed, this, &aTreeWidget::slotTreeKeyPressed);
	connect(m_tree, &aTreeWidgetBase::keyReleased, this, &aTreeWidget::slotTreeKeyReleased);
	connect(m_tree, &QTreeWidget::itemActivated, this, &aTreeWidget::slotTreeItemActivated);
	connect(m_tree, &QTreeWidget::itemChanged, this, &aTreeWidget::slotTreeItemChanged);
	connect(m_tree, &QTreeWidget::itemClicked, this, &aTreeWidget::slotTreeItemClicked);
	connect(m_tree, &QTreeWidget::itemCollapsed, this, &aTreeWidget::slotTreeItemCollapsed);
	connect(m_tree, &QTreeWidget::itemDoubleClicked, this, &aTreeWidget::slotTreeItemDoubleClicked);
	connect(m_tree, &QTreeWidget::itemExpanded, this, &aTreeWidget::slotTreeItemExpanded);
	connect(m_tree, &QTreeWidget::itemSelectionChanged, this, &aTreeWidget::slotTreeSelectionChanged);
	connect(m_tree, &aTreeWidgetBase::mouseMove, this, &aTreeWidget::slotTreeMouseMove);
	connect(m_tree, &aTreeWidgetBase::leave, this, &aTreeWidget::slotTreeLeave);
	connect(m_tree, &QTreeWidget::customContextMenuRequested, this, &aTreeWidget::slotCustomContextMenuRequested);
}

ak::aTreeWidget::~aTreeWidget() {
	A_OBJECT_DESTROYING
	memFree();
}

QWidget * ak::aTreeWidget::widget(void) { return m_widget; }

void ak::aTreeWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	m_tree->setColorStyle(m_colorStyle);
	m_filter->setColorStyle(m_colorStyle);
}

// ###########################################################################################################################

// Data manipulation

ak::ID ak::aTreeWidget::add(
	ak::ID							_parentId,
	const QString &					_text,
	ak::textAlignment		_textAlignment,
	QIcon							_icon
) {
	if (_parentId == -1) {
		// Check if top level item already exists
		ak::aTreeWidgetItem * itm = m_tree->topLevelItem(_text);
		if (itm == nullptr) {
			// Create and add new item
			itm = createItem(_text, _textAlignment, _icon);
			m_tree->AddTopLevelItem(itm);
			m_items.insert_or_assign(itm->id(), itm);
		}
		return itm->id();
	}
	else {
		// Find parent object
		auto parent = m_items.find(_parentId);
		assert(parent != m_items.end()); // Invalid ID provided
		ak::aTreeWidgetItem * itm = parent->second->findChild(_text);
		if (itm == nullptr) {
			itm = createItem(_text, _textAlignment, _icon);
			parent->second->AddChild(itm);
			if (parent->second->isSelected() && m_selectAndDeselectChildren) { itm->setSelected(true); }
			m_items.insert_or_assign(itm->id(), itm);
		}
		return itm->id();
	}
}

ak::ID ak::aTreeWidget::add(
	const QString &					_cmd,
	char							_delimiter,
	ak::textAlignment		_textAlignment,
	const QIcon  &					_icon
) {
	assert(_cmd.length() > 0); // Provided command is empty
	QStringList items = _cmd.split(_delimiter);

	assert(items.count() != 0); // Split failed

	// Create a pointer to store the current item
	ak::aTreeWidgetItem * currentItem = nullptr;

	// Search for the root element
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) {
		if (itm->second != nullptr) {
			if (itm->second->parent() == nullptr && itm->second->text(0) == items.at(0)) { currentItem = itm->second; break; }
		}
	}

	// Create a new root item
	if (currentItem == nullptr) {
		currentItem = createItem(items.at(0), _textAlignment);
		assert(currentItem != nullptr); // Failed to create item
		m_tree->AddTopLevelItem(currentItem);
		m_items.insert_or_assign(currentItem->id(), currentItem);
	}

	m_ignoreEvents = true;

	for (int i = 1; i < items.count(); i++) {
		ak::aTreeWidgetItem * nItm = currentItem->findChild(items.at(i));
		if (nItm == nullptr) {
			// Create new item
			if (i == items.count() - 1) { nItm = createItem(items.at(i), _textAlignment, _icon); }
			else { nItm = createItem(items.at(i), _textAlignment); }
			assert(nItm != nullptr); // Failed to create
			// Add item
			currentItem->AddChild(nItm);
			if (currentItem->isSelected() && m_selectAndDeselectChildren) { nItm->setSelected(true); }

			// Store data
			m_items.insert_or_assign(nItm->id(), nItm);
		}
		currentItem = nItm;
	}

	m_ignoreEvents = false;
	selectionChangedEvent(false);

	return currentItem->id();
}

void ak::aTreeWidget::clear(bool _emitEvent) {
	m_tree->Clear();
	m_items.clear();
	m_currentId = 0;
	if (_emitEvent) { emit cleared(); }
}

void ak::aTreeWidget::setItemEnabled(
	ak::ID							_itemId,
	bool							_enabled
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid ID provided
	m_ignoreEvents = true;
	itm->second->setDisabled(!_enabled);
	if (m_selectAndDeselectChildren) { itm->second->setChildsEnabled(_enabled); }
	m_ignoreEvents = false;
}

void ak::aTreeWidget::setIsReadOnly(
	bool							_readOnly
) {
	m_isReadOnly = _readOnly;
	for (auto itm : m_items) {
		itm.second->setLocked(m_isReadOnly);
	}
}

void ak::aTreeWidget::setItemSelected(
	ak::ID							_itemId,
	bool							_selected
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	if (itm == m_items.end()) return;

	m_ignoreEvents = true;
	itm->second->setSelected(_selected);
	if (m_selectAndDeselectChildren) { itm->second->setChildsSelected(_selected); }
	m_ignoreEvents = false;
	selectionChangedEvent(true);
}

void ak::aTreeWidget::setItemVisible(
	ak::ID							_itemId,
	bool							_visible
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	m_ignoreEvents = true;
	itm->second->setVisible(_visible);
	m_ignoreEvents = false;
	//if (m_selectAndDeselectChildren) { itm->second->setChildsVisible(_visible); }
}

void ak::aTreeWidget::setItemText(
	ak::ID							_itemId,
	const QString &					_text
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	m_ignoreEvents = true;
	itm->second->setText(0, _text);
	m_ignoreEvents = false;
}

void ak::aTreeWidget::setSingleItemSelected(
	ak::ID							_itemId,
	bool							_selected
) {
	deselectAllItems(false);
	setItemSelected(_itemId, _selected);
}

void ak::aTreeWidget::toggleItemSelection(
	ak::ID							_itemId
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	m_ignoreEvents = true;
	bool state = itm->second->isSelected();
	itm->second->setSelected(!state);
	if (m_selectAndDeselectChildren) { itm->second->setChildsSelected(!state); }
	m_ignoreEvents = false;
	selectionChangedEvent(true);
}

void ak::aTreeWidget::deselectAllItems(
	bool							_emitEvent
) {
	m_ignoreEvents = true;
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) { itm->second->setSelected(false); }
	m_ignoreEvents = false;

	if (_emitEvent) { emit selectionChanged(); }
}

void ak::aTreeWidget::setEnabled(
	bool							_enabled
) { m_tree->setEnabled(_enabled); }

void ak::aTreeWidget::setVisible(
	bool							_visible
) { m_tree->setVisible(_visible); }

void ak::aTreeWidget::setItemIcon(
	ak::ID							_itemId,
	const QIcon &					_icon
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	if (itm != m_items.end())
	{
		itm->second->setIcon(0, _icon);
	}
}

void ak::aTreeWidget::setSortingEnabled(
	bool							_enabled
) { 
	m_tree->setSortingEnabled(_enabled);
	if (_enabled) {
		m_tree->sortByColumn(0, Qt::AscendingOrder);
	}
}

// ###########################################################################################################################################

// Filter

void ak::aTreeWidget::setFilterVisible(
	bool							_visible
) { m_filter->setVisible(_visible); }

void ak::aTreeWidget::applyCurrentFilter(void) {
	QString filter = m_filter->text();
	if (filter.isEmpty()) {
		// Show all items
		m_ignoreEvents = true;
		for (auto itm = m_items.begin(); itm != m_items.end(); itm++) { 
			if (itm->second->isVisible()) {
				itm->second->setHidden(false);
			}
		}
		collapseAllItems();
		selectionChangedEvent(false);
		m_ignoreEvents = false;
	}
	else {
		m_ignoreEvents = true;
		for (auto itm : m_items) {
			itm.second->setHidden(true);
		}
		collapseAllItems();

		// Check filter
		Qt::CaseSensitivity sens = Qt::CaseSensitivity::CaseInsensitive;
		if (m_filterCaseSensitive) { sens = Qt::CaseSensitivity::CaseSensitive; }
		for (auto itm = m_items.begin(); itm != m_items.end(); itm++) {
			if (itm->second->text(0).toLower().contains(filter, sens)) {
				if (itm->second->isVisible()) {
					itm->second->setVisible(true);
					itm->second->expandAllParents(false);
					if (m_displayChildsOnFilter) {
						itm->second->setChildsVisible(true);
						itm->second->collapse();
					}
				}
				else {
					aTreeWidgetItem * p = itm->second->parentItem();
					while (p) {
						if (p->isVisible()) { p->setVisible(true); break; }
						else { p = p->parentItem(); }
					}
				}
			}
			else if (itm->second->isSelected()) {
				itm->second->setSelected(false);
			}
		}
		m_ignoreEvents = false;
		selectionChangedEvent(false);
	}
}

void ak::aTreeWidget::setFilterCaseSensitive(
	bool							_caseSensitive,
	bool							_refresh
) { m_filterCaseSensitive = _caseSensitive; if (_refresh) { applyCurrentFilter(); } }

void ak::aTreeWidget::setFilterRefreshOnChange(
	bool							_refreshOnChange
) { m_filterRefreshOnChange = _refreshOnChange; }

void ak::aTreeWidget::setMultiSelectionEnabled(
	bool							_multiSelection
) {
	if (_multiSelection) { m_tree->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection); }
	else { m_tree->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection); }
}

bool ak::aTreeWidget::getMultiSelectionEnabled(void)
{
	return (m_tree->selectionMode() == QAbstractItemView::SelectionMode::ExtendedSelection);
}

void ak::aTreeWidget::setAutoSelectAndDeselectChildrenEnabled(
	bool							_enabled
) { m_selectAndDeselectChildren = _enabled; }

void ak::aTreeWidget::expandAllItems(void) {
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) { itm->second->setExpanded(true); }
}

void ak::aTreeWidget::expandItem(
	ak::ID			_itemID,
	bool		_expanded
) 
{
	auto itm = m_items.find(_itemID);
	if (itm == m_items.end()) return;

	itm->second->setExpanded(_expanded);
}

bool ak::aTreeWidget::isItemExpanded(
	ak::ID			_itemID
) 
{
	auto itm = m_items.find(_itemID);
	if (itm == m_items.end()) return false;

	return itm->second->isExpanded();
}


void ak::aTreeWidget::collapseAllItems(void) {
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) { itm->second->setExpanded(false); }
}

void ak::aTreeWidget::deleteItem(
	ak::ID												_itemID,
	bool												_supressSelectionChangedEvent
) {
	m_ignoreEvents = true;

	bool prevMultiselectionState = getMultiSelectionEnabled();
	setMultiSelectionEnabled(true);

	auto itm = m_items.find(_itemID);
	assert(itm != m_items.end());		// Invalid item id
	aTreeWidgetItem * item = itm->second;
	for (auto cId : item->allChildsIDs()) { m_items.erase(cId); }
	if (item->parentId() == ak::invalidID) {
		m_tree->removeTopLevelItem(item->id());
	}
	delete item;
	m_items.erase(_itemID);

	setMultiSelectionEnabled(prevMultiselectionState);
	m_ignoreEvents = false;
	
	if (!_supressSelectionChangedEvent)	selectionChangedEvent(true);
}

void ak::aTreeWidget::deleteItems(
	const std::vector<ak::ID> &							_itemIDs,
	bool												_supressSelectionChangedEvent
) {
	m_ignoreEvents = true;

	bool prevMultiselectionState = getMultiSelectionEnabled();
	setMultiSelectionEnabled(true);

	for (auto id : _itemIDs) {
		auto itm = m_items.find(id);
		if (itm != m_items.end()) {
			aTreeWidgetItem * item = itm->second;
			for (auto cId : item->allChildsIDs()) 
			{ 		
				m_items.erase(cId); 
			}
			if (item->parentId() == ak::invalidID) {
				m_tree->removeTopLevelItem(item->id());
			}
			delete item;
			m_items.erase(id);
		}
	}

	setMultiSelectionEnabled(prevMultiselectionState);
	m_ignoreEvents = false;

	if (!_supressSelectionChangedEvent)	selectionChangedEvent(true);
}

void ak::aTreeWidget::setItemsAreEditable(
	bool												_editable,
	bool												_applyToAll
) {
	m_itemsAreEditable = _editable;
	if (_applyToAll) {
		for (auto itm : m_items) {
			itm.second->setEditable(_editable);
		}
	}
}

void ak::aTreeWidget::setItemIsEditable(
	ak::ID												_itemID,
	bool												_editable
) {
	auto itm = m_items.find(_itemID);
	assert(itm != m_items.end()); // Invalid item ID
	itm->second->setEditable(_editable);
}

void ak::aTreeWidget::setItemIsEditable(
	const std::vector<ak::ID> &							_itemIDs,
	bool												_editable
) {
	for (auto id : _itemIDs) { setItemIsEditable(id, _editable); }
}

void ak::aTreeWidget::setItemSelectChildren(
	ak::ID												_itemID,
	bool												_selectChildren
) {
	auto itm = m_items.find(_itemID);
	assert(itm != m_items.end()); // Invalid item ID
	itm->second->setSelectChilds(_selectChildren);
}

// ###########################################################################################################################

// Information gathering

std::vector<ak::ID> ak::aTreeWidget::selectedItems(void) {
	std::vector<ak::ID> r;
	for (QTreeWidgetItem * itm : m_tree->selectedItems()) {
		r.push_back(aTreeWidgetBase::getItemId(itm));
	}
	return r;
}

std::vector<QString> ak::aTreeWidget::getItemPath(
	ak::ID							_itemId
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	return toVector(itm->second->getItemPath());
}

QString ak::aTreeWidget::getItemPathString(
	ak::ID							_itemId,
	char							_delimiter
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	return itm->second->getItemPathString(_delimiter);
}

ak::ID ak::aTreeWidget::getItemID(
	const QString &					_itemPath,
	char							_delimiter
) {
	assert(_itemPath.length() != 0); // No item path provided
	QStringList lst = _itemPath.split(_delimiter);
	assert(lst.count() > 0); // split error
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) {
		if (itm->second->text(0) == lst.at(0)) {
			if (lst.count() == 1) { return itm->second->id(); }
			return itm->second->getItemID(lst, 1);
		}
	}
	return ak::invalidID;
}

QString ak::aTreeWidget::getItemText(
	ak::ID							_itemId
) {
	auto itm = m_items.find(_itemId);
	assert(itm != m_items.end()); // Invalid item ID
	return itm->second->text(0);
}

ak::aTreeWidgetItem * ak::aTreeWidget::item(ID _itemID) {
	auto itm = m_items.find(_itemID);
	assert(itm != m_items.end());
	return itm->second;
}

bool ak::aTreeWidget::enabled() const { return m_tree->isEnabled(); }

int ak::aTreeWidget::itemCount(void) const { return m_items.size(); }

bool ak::aTreeWidget::isSortingEnabled(void) const { return m_tree->isSortingEnabled(); }

// ###########################################################################################################################

// Events

void ak::aTreeWidget::selectionChangedEvent(
	bool							_emitEvent
) {
	bool ignoreBackup = m_ignoreEvents;
	m_ignoreEvents = true;
	// Get selected items
	QList<QTreeWidgetItem *> selected = m_tree->selectedItems();
	// Select childs of selected items
	for (QTreeWidgetItem * itm : selected) {
		// Cast item
		ak::aTreeWidgetItem * i = nullptr;
		i = dynamic_cast<ak::aTreeWidgetItem *>(itm);
		assert(i != nullptr); // Cast failed

		if (m_selectAndDeselectChildren && i->getSelectChilds()) {
			i->setChildsSelected(true);
		}

		i->ensureTopLevelSelectionVisible();
	}
	m_ignoreEvents = ignoreBackup;

	if (_emitEvent) { emit selectionChanged(); }
}

ak::aTreeWidgetItem * ak::aTreeWidget::itemAt(const QPoint& _pos) {
	QTreeWidgetItem * itm = m_tree->itemAt(_pos);
	if (itm == nullptr) return nullptr;
	else {
		aTreeWidgetItem * actualItem = dynamic_cast<aTreeWidgetItem *>(itm);
		assert(actualItem);
		return actualItem;
	}
}

// ###########################################################################################################################

void ak::aTreeWidget::slotTreeKeyPressed(QKeyEvent * _event) {
	if (m_ignoreEvents) { return; }
	emit keyPressed(_event);
}

void ak::aTreeWidget::slotTreeKeyReleased(QKeyEvent * _event) {
	if (m_ignoreEvents) { return; }
	emit keyReleased(_event);
}

void ak::aTreeWidget::slotTreeItemActivated(QTreeWidgetItem * _item, int _column) {
	if (m_ignoreEvents) { return; }
	emit itemActivated(_item, _column);
}

void ak::aTreeWidget::slotTreeItemChanged(QTreeWidgetItem * _item, int _column) {
	aTreeWidgetItem * itm = nullptr;
	itm = dynamic_cast<aTreeWidgetItem *>(_item);
	if (itm == nullptr) {
		assert(0);		
		return;
	}
	
	if (itm->text(0) != itm->storedText()) {
		// Item text changed
		itm->setStoredText(itm->text(0));
		if (m_ignoreEvents) { return; }
		emit itemTextChanged(_item, _column);
	}
	else if (!m_ignoreEvents) {
		// Regular change
		emit itemChanged(_item, _column);
	}
}

void ak::aTreeWidget::slotTreeItemClicked(QTreeWidgetItem * _item, int _column) {
	if (m_ignoreEvents) { return; }
	emit itemClicked(_item, _column);
}

void ak::aTreeWidget::slotTreeItemCollapsed(QTreeWidgetItem * _item) {
	aTreeWidgetItem * itm = nullptr;
	itm = dynamic_cast<aTreeWidgetItem *>(_item);
	assert(itm != nullptr); // Cast failed
	itm->collapse();
	if (m_ignoreEvents) { return; }
	emit itemCollapsed(_item);

}

void ak::aTreeWidget::slotTreeItemDoubleClicked(QTreeWidgetItem * _item, int _column) {
	if (m_ignoreEvents) { return; }
	emit itemDoubleClicked(_item, _column);
}

void ak::aTreeWidget::slotTreeItemExpanded(QTreeWidgetItem * _item) {
	if (m_ignoreEvents) { return; }
	emit itemExpanded(_item);
}

void ak::aTreeWidget::slotTreeSelectionChanged() {
	if (m_ignoreEvents) { return; }
	selectionChangedEvent(true);
}

void ak::aTreeWidget::slotTreeMouseMove(QMouseEvent * _event) {
	QTreeWidgetItem * item = m_tree->itemAt(_event->pos());
	if (item == nullptr) {
		if (m_focusedItem != invalidID) {
			m_focusedItem = invalidID;
			if (m_ignoreEvents) { return; }
			emit focusLost();
		}
	}
	else {
		aTreeWidgetItem * actualItem = nullptr;
		actualItem = dynamic_cast<aTreeWidgetItem *>(item);
		assert(actualItem != nullptr);
		if (actualItem->id() != m_focusedItem) {
			m_focusedItem = actualItem->id();
			if (m_ignoreEvents) { return; }
			emit itemFocused(item);
		}
	}
}

void ak::aTreeWidget::slotTreeLeave(QEvent * _event) {
	if (m_focusedItem != invalidID) {
		m_focusedItem = invalidID;
		if (m_ignoreEvents) { return; }
		emit focusLost();
	}
}

void ak::aTreeWidget::slotFilterTextChanged(void) {
	if (m_filterRefreshOnChange) { applyCurrentFilter(); }
}

void ak::aTreeWidget::slotFilterKeyPressed(QKeyEvent * _event) {
	if (toKeyType(_event) == keyReturn) {
		applyCurrentFilter();
	}
}

void ak::aTreeWidget::slotCustomContextMenuRequested(const QPoint& _pos) {
	emit customContextMenuRequested(_pos);
}

// ###########################################################################################################################

ak::aTreeWidgetItem * ak::aTreeWidget::createItem(
	const QString &					_text,
	ak::textAlignment		_textAlignment,
	QIcon							_icon
) {
	// Create item
	ak::aTreeWidgetItem * itm = nullptr;
	itm = new ak::aTreeWidgetItem(m_currentId);
	// Set params
	itm->setTextAlignment(0, ak::toQtAlignmentFlag(_textAlignment));
	itm->setText(0, _text);
	itm->setIcon(0, _icon);
	itm->setStoredText(_text);
	itm->setEditable(m_itemsAreEditable);
	itm->setLocked(m_isReadOnly);

	//if (m_colorStyle != nullptr) {
		//itm->setTextColor(0, m_colorStyle->getControlsMainForecolor().toQColor());
		//itm->setBackgroundColor(0, m_colorStyle->getControlsMainBackcolor().toQColor());
	//}
	m_currentId++;
	return itm;
}

void ak::aTreeWidget::refreshAfterItemsMoved(const QList<aTreeWidgetItem *>& _items, const QList<ID>& _itemIds,
	const QList<ID>& _oldParentIds, const QList<ID>& _newParentIds) 
{


	emit itemsMoved(_itemIds, _oldParentIds, _newParentIds);
}

void ak::aTreeWidget::memFree(void) {
	// Delete items
	while (m_items.begin() != m_items.end()) 
	{
		ak::aTreeWidgetItem * itm = m_items.begin()->second;
		m_items.erase(m_items.begin()->first);
		clearItem(itm);
		delete itm;
	}

	// Delete members
	if (m_filter != nullptr) { delete m_filter; m_filter = nullptr; }
	if (m_tree != nullptr) { delete m_tree; m_tree = nullptr; }
	if (m_layout != nullptr) { delete m_layout; m_layout = nullptr; }
	if (m_widget != nullptr) { delete m_widget; m_widget = nullptr; }
}

void ak::aTreeWidget::clearItem(
	ak::aTreeWidgetItem *			_item
) {
	while (_item->hasChilds()) {
		auto itm = _item->popFirstChild();
		clearItem(itm);
		m_items.erase(itm->id());
		delete itm;
	}
}

// ###########################################################################################################################################

// ###########################################################################################################################################

// ###########################################################################################################################################

ak::aTreeWidgetBase::aTreeWidgetBase(aTreeWidget * _ownerTree, aColorStyle * _colorStyle)
	: QTreeWidget(), ak::aWidget(otTree, _colorStyle), m_ownerTree(_ownerTree), m_moveNotifier(nullptr)
{
	setStyleSheet("");
	if (m_colorStyle != nullptr) { setColorStyle(m_colorStyle); }
	setContextMenuPolicy(Qt::CustomContextMenu);
}

ak::aTreeWidgetBase::~aTreeWidgetBase() { A_OBJECT_DESTROYING }

// #######################################################################################################
// Event handling

void ak::aTreeWidgetBase::keyPressEvent(QKeyEvent *_event)
{
	QTreeWidget::keyPressEvent(_event); emit keyPressed(_event);
}

void ak::aTreeWidgetBase::keyReleaseEvent(QKeyEvent * _event)
{
	QTreeWidget::keyReleaseEvent(_event); emit keyReleased(_event);
}

void ak::aTreeWidgetBase::mouseMoveEvent(QMouseEvent * _event)
{
	QTreeWidget::mouseMoveEvent(_event); emit mouseMove(_event);
}

void ak::aTreeWidgetBase::leaveEvent(QEvent *_event)
{
	QTreeWidget::leaveEvent(_event); emit leave(_event);
}

void ak::aTreeWidgetBase::dropEvent(QDropEvent * _event) {
	// Store current item positions
	QList<aTreeWidgetItem *> actualItemList = selectedItemsRef();

	aTreeWidgetItem * dropToItem = dynamic_cast<aTreeWidgetItem *>(itemAt(_event->pos()));
	if (dropToItem == nullptr) {
		assert(0);
		return;
	}


	DropIndicatorPosition dropPos = dropIndicatorPosition();

	switch (dropPos)
	{
	case QAbstractItemView::OnItem: break;
		// If dropping above or below, then the actual item is the parent
		// of the drop to item
	case QAbstractItemView::AboveItem: 
	case QAbstractItemView::BelowItem: 
		if (dropToItem == nullptr) {
			aAssert(0, "No drop to item found");
			break;
		}
		dropToItem = dropToItem->parentItem();
	break;
	// If dropping to top level set the drop to item to nullptr
	case QAbstractItemView::OnViewport: dropToItem = nullptr; break;
	default:
		aAssert(0, "Unknown drop indicator position");
		break;
	}

	if (m_moveNotifier) {
		if (!m_moveNotifier->allowItemDrop(actualItemList, dropToItem)) {
			_event->ignore();
			return;
		}
	}

	QList<ID> itemIdList;
	QList<ID> oldParentList;

	extendedItemSelectionInformation(actualItemList, itemIdList, oldParentList);

	// Perform the drop
	QTreeWidget::dropEvent(_event);

	// Get the new parents and refresh the item parents
	QList<ID> newParentList;
	for (auto itm : actualItemList) {
		QTreeWidgetItem * par = itm->parent();
		if (par) {
			aTreeWidgetItem * actualPar = dynamic_cast<aTreeWidgetItem *>(par);
			if (actualPar) {
				itm->setParentItem(actualPar);
				newParentList.push_back(actualPar->id());
			}
			else {
				assert(0); // Item cast failed
				itm->setParentItem(nullptr);
				newParentList.push_back(invalidID);
			}
		}
		else {
			itm->setParentItem(nullptr);
			newParentList.push_back(invalidID);
		}
	}

	// Update the top level item list
	for (int i = 0; i < itemIdList.count(); i++) {
		// Item moved to lower level than top
		if (oldParentList[i] == invalidID && newParentList[i] != invalidID) {
			m_topLevelItems.erase(itemIdList[i]);
		}
		// Item moved from lower level to top
		if (oldParentList[i] != invalidID && newParentList[i] == invalidID) {
			m_topLevelItems.insert_or_assign(itemIdList[i], actualItemList[i]);
		}
	}

	// Notify owner
	m_ownerTree->refreshAfterItemsMoved(actualItemList, itemIdList, oldParentList, newParentList);
}

void ak::aTreeWidgetBase::dragEnterEvent(QDragEnterEvent * _event) {
	if (m_moveNotifier) {
		if (m_moveNotifier->allowItemGrab(selectedItemsRef())) {
			QTreeWidget::dragEnterEvent(_event);
		}
		else {
			// Ignore the event
			_event->ignore();
		}
	}
	else {
		QTreeWidget::dragEnterEvent(_event);
	}
}

void ak::aTreeWidgetBase::dragLeaveEvent(QDragLeaveEvent * _event) {
	QTreeWidget::dragLeaveEvent(_event);
	if (m_moveNotifier) m_moveNotifier->itemMoveEnded();
}

// #######################################################################################################

QWidget * ak::aTreeWidgetBase::widget(void) { return this; }

void ak::aTreeWidgetBase::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls | cafDefaultBorderControls, "QTreeWidget{", "}"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls, "QTreeWidget::item{", "}"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus |
		cafBackgroundColorFocus, "QTreeWidget::item:hover{", "}"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected |
		cafBackgroundColorSelected, "QTreeWidget::item:selected:!hover{", "}"));

	if (!sheet.isEmpty()) {
		sheet.append("QTreeWidget::branch:has-siblings:!adjoins-item{border-image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_End_Root.png"));
		sheet.append(") 0;}");

		sheet.append("QTreeWidget::branch:has-siblings:adjoins-item{border-image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_HasSiblings.png"));
		sheet.append(") 0;}");

		sheet.append("QTreeWidget::branch:!has-children:!has-siblings:adjoins-item{border-image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_End.png"));
		sheet.append(") 0;}");

		sheet.append("QTreeWidget::branch:has-children:!has-siblings:closed:!hover,"
			"QTreeWidget::branch:closed:has-children:has-siblings:!hover{border-image: none; image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_HasChildren.png"));
		sheet.append(");}");

		sheet.append("QTreeWidget::branch:has-children:!has-siblings:closed:hover,"
			"QTreeWidget::branch:closed:has-children:has-siblings:hover{border-image: none; image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_HasChildren_Focus.png"));
		sheet.append(");}");

		sheet.append("QTreeWidget::branch:open:has-children:!has-siblings:!hover,"
			"QTreeWidget::branch:open:has-children:has-siblings:!hover{border-image: none; image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_Open.png"));
		sheet.append(");}");

		sheet.append("QTreeWidget::branch:open:has-children:!has-siblings:hover,"
			"QTreeWidget::branch:open:has-children:has-siblings:hover{border-image: none; image: url(");
		sheet.append(m_colorStyle->getFilePath("Tree/Tree_Branch_Open_Focus.png"));
		sheet.append(");}");
	}
	this->setStyleSheet(sheet);
}

// ####################################################################################################################################

void ak::aTreeWidgetBase::AddTopLevelItem(
	aTreeWidgetItem *						_item
) {
	try {
		if (_item == nullptr) { throw aException("Is nullptr", "Check aTreeWidgetItem"); }
		if (_item->id() == ak::invalidID) { throw aException("Is invalid ID", "Check item ID"); }
		//if (topLevelItem(_item->text(0)) != nullptr) { throw aException("Item does already exist", "Check for duplicates"); }
		addTopLevelItem(_item);
		m_topLevelItems.insert_or_assign(_item->id(), _item);
	}
	catch (const aException & e) { throw aException(e, "ak::aTreeWidgetBase::AddTopLevelItem()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aTreeWidgetBase::AddTopLevelItem()"); }
	catch (...) { throw aException("Unknown error", "ak::aTreeWidgetBase::AddTopLevelItem()"); }
}

void ak::aTreeWidgetBase::itemWasMovedToTopLevel(aTreeWidgetItem * _item) {
	m_topLevelItems.insert_or_assign(_item->id(), _item);
}

ak::aTreeWidgetItem * ak::aTreeWidgetBase::topLevelItem(
	const QString &					_text
) {
	try {
		for (auto itm{ m_topLevelItems.begin() }; itm != m_topLevelItems.end(); itm++) {
			assert(itm->second != nullptr); // That should not happen..
			if (itm->second->text(0) == _text) { return itm->second; }
		}
		return nullptr;
	}
	catch (const aException & e) { throw aException(e, "ak::aTreeWidgetBase::topLevelItem(QString)"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aTreeWidgetBase::topLevelItem(QString)"); }
	catch (...) { throw aException("Unknown error", "ak::aTreeWidgetBase::topLevelItem(QString)"); }
}

ak::aTreeWidgetItem * ak::aTreeWidgetBase::topLevelItem(
	ak::ID							_id
) {
	try {
		for (auto itm{ m_topLevelItems.begin() }; itm != m_topLevelItems.end(); itm++) {
			if (itm->second->id() == _id) { return itm->second; }
		}
		return nullptr;
	}
	catch (const aException & e) { throw aException(e, "ak::aTreeWidgetBase::topLevelItem(ID)"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aTreeWidgetBase::topLevelItem(ID)"); }
	catch (...) { throw aException("Unknown error", "ak::aTreeWidgetBase::topLevelItem(ID)"); }
}

std::vector<QString> ak::aTreeWidgetBase::topLevelItemsText(void) {
	std::vector<QString> r;
	for (auto itm{ m_topLevelItems.begin() }; itm != m_topLevelItems.end(); itm++) {
		r.push_back(itm->second->text(0));
	}
	return r;
}

void ak::aTreeWidgetBase::Clear(void) {
	m_topLevelItems.clear();
	clear();
}

void ak::aTreeWidgetBase::removeTopLevelItem(
	ak::ID							_id
) {
	m_topLevelItems.erase(_id);
}

// ####################################################################################################################################

ak::ID ak::aTreeWidgetBase::getItemId(
	QTreeWidgetItem *		_item
) {
	if (_item == nullptr) { return ak::invalidID; }
	aTreeWidgetItem * itm = nullptr;
	itm = dynamic_cast<aTreeWidgetItem *>(_item);
	assert(itm != nullptr); // Cast failed
	return itm->id();
}

QList<ak::aTreeWidgetItem *> ak::aTreeWidgetBase::selectedItemsRef(void) const {
	QList<QTreeWidgetItem*> itemList = selectedItems();
	QList<ak::aTreeWidgetItem *> ret;
	for (auto itm : itemList) {
		aTreeWidgetItem * actualItem = dynamic_cast<aTreeWidgetItem *>(itm);
		if (actualItem == nullptr) assert(0); // Unexpected item
		else ret.push_back(actualItem);
	}
	return ret;
}

void ak::aTreeWidgetBase::extendedItemSelectionInformation(const QList<aTreeWidgetItem *>& _selectedItems, QList<ID>& _selectedItemIds, QList<ID>& _itemParentIds) const {
	for (auto itm : _selectedItems) _selectedItemIds.push_back(itm->id());
	for (auto itm : _selectedItems) {
		aTreeWidgetItem * par = itm->parentItem();
		if (par) _itemParentIds.push_back(par->id());
		else _itemParentIds.push_back(invalidID);
	}
	assert(_selectedItems.count() == _selectedItemIds.count() && _selectedItemIds.count() == _itemParentIds.count());
}

// ###########################################################################################################################################

// ###########################################################################################################################################

// ###########################################################################################################################################

ak::aTreeWidgetItem::aTreeWidgetItem(
	ak::ID							_newId,
	aTreeWidgetItem *						_parent,
	int								_type
) : ak::aObject(otTreeItem), QTreeWidgetItem(_type), m_isVisible(true),
m_id(_newId), m_parent(_parent), m_isEditable(false), m_isLockedForEdit(false) {}

ak::aTreeWidgetItem::aTreeWidgetItem(
	ak::aTreeWidgetBase *			_view,
	ak::ID							_newId,
	aTreeWidgetItem *						_parent,
	int								_type
) : ak::aObject(otTreeItem), QTreeWidgetItem(_view, _type), m_isVisible(true),
m_id(_newId), m_parent(_parent), m_isEditable(false), m_isLockedForEdit(false) {}

ak::aTreeWidgetItem::~aTreeWidgetItem() {
	A_OBJECT_DESTROYING
	if (m_parent) { m_parent->eraseChild(m_id); }

	// The child items themselves will be deleted by the corresponding functionality in the QTreeWidgetItem
	// However, they must no longer refer to this deleted item. Therefore we set their parent to nullptr.
	while (!m_childs.empty()) {
		aTreeWidgetItem * childItm = m_childs.front();
		m_childs.pop_front();
		childItm->setParentItem(nullptr);
	}
	m_childs.clear();
}

// ##############################################################################################

// Setter

void ak::aTreeWidgetItem::AddChild(
	aTreeWidgetItem *						_child
) {
	try {
		if (findChild(_child->text(0)) != nullptr) { throw aException("Item does already exist", "Check duplicate"); }
		if (_child == nullptr) { throw aException("Is nullptr", "Check child"); }
		_child->setParentItem(this);
		m_childs.push_back(_child);
		addChild(_child);
	}
	catch (const aException & e) { throw aException(e, "ak::aTreeWidgetItem::AddChild(aTreeWidgetItem)"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aTreeWidgetItem::AddChild(aTreeWidgetItem)"); }
	catch (...) { throw aException("Unknown error", "ak::aTreeWidgetItem::AddChild(aTreeWidgetItem)"); }
}

void ak::aTreeWidgetItem::setParentItem(
	aTreeWidgetItem *						_parent
) {
	m_parent = _parent;
}

void ak::aTreeWidgetItem::setChildsEnabled(
	bool							_enabled
) {
	for (auto itm : m_childs) {
		itm->setDisabled(!_enabled);
		itm->setChildsEnabled(_enabled);
	}
}

void ak::aTreeWidgetItem::setChildsSelected(
	bool							_selected
) {
	if (getSelectChilds())
	{
		for (auto itm : m_childs) {
			itm->setSelected(_selected);
			itm->setChildsSelected(_selected);
		}
	}
}

void ak::aTreeWidgetItem::setChildsVisible(
	bool							_visible
) {
	for (auto itm : m_childs) {
		if (itm->isVisible()) {
			itm->setVisible(_visible);
			itm->setChildsVisible(_visible);
		}
	}
}

void ak::aTreeWidgetItem::ensureTopLevelSelectionVisible() {
	if (m_parent != nullptr) {
		if (m_parent->isSelected()) { m_parent->ensureTopLevelSelectionVisible(); }
		else if (isSelected()) {
			m_parent->expandAllParents(true);
		}
	}
	//else if (isSelected()) { setExpanded(true); }
}

void ak::aTreeWidgetItem::expandAllParents(
	bool							_expandThis
) {
	if (_expandThis) setExpanded(true);
	if (m_parent) m_parent->expandAllParents(true);
}

void ak::aTreeWidgetItem::collapse(void) {
	if (isExpanded()) { setExpanded(false); }
	for (auto itm : m_childs) { itm->collapse(); }
}

void ak::aTreeWidgetItem::setEditable(bool _editable) {
	m_isEditable = _editable;
	refreshEditableState();
}

void ak::aTreeWidgetItem::setLocked(bool _locked) {
	m_isLockedForEdit = _locked;
	refreshEditableState();
}

void ak::aTreeWidgetItem::setVisible(
	bool							_isVisible,
	bool							_expandParents
) {
	m_isVisible = _isVisible;
	setHidden(!m_isVisible);
	if (m_isVisible && _expandParents && m_parent != nullptr) {
		m_parent->setExpanded(true);
		m_parent->setVisible(true);
	}
}

// ##############################################################################################

// Getter

ak::aTreeWidgetItem * ak::aTreeWidgetItem::findChild(
	ak::ID							_id
) {
	for (auto itm : m_childs) { if (itm->id() == _id) { return itm; } }
	return nullptr;
}

ak::aTreeWidgetItem * ak::aTreeWidgetItem::findChild(
	const QString &					_text
) {
	for (auto itm : m_childs) { if (itm->text(0) == _text) { return itm; } }
	return nullptr;
}

ak::ID ak::aTreeWidgetItem::getItemID(
	const QStringList &						_itemPath,
	int										_currentIndex
) {
	try {
		assert(_currentIndex < _itemPath.count()); // Invalid index provided
		ak::aTreeWidgetItem * child = findChild(_itemPath.at(_currentIndex));
		if (child == nullptr) { return ak::invalidID; }
		if (_currentIndex == _itemPath.count() - 1) { return child->id(); }
		return child->getItemID(_itemPath, _currentIndex + 1);
	}
	catch (const aException & e) { throw aException(e, "ak::aTreeWidgetItem::getItemID()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aTreeWidgetItem::getItemID()"); }
	catch (...) { throw aException("Unknown error", "ak::aTreeWidgetItem::getItemID()"); }
}

void ak::aTreeWidgetItem::eraseChild(
	ak::ID							_id
) {
	std::list<aTreeWidgetItem *>::iterator it1 = m_allChilds.begin();
	std::list<ak::ID>::iterator it2 = m_allChildsIDs.begin();
	for (auto itm : m_allChilds) {
		if (itm->id() == _id) { m_allChilds.erase(it1); break; }
		it1++;
	}
	it1 = m_childs.begin();
	for (auto itm : m_childs) {
		if (itm->id() == _id) { m_childs.erase(it1); break; }
		it1++;
	}
	for (auto itm : m_allChildsIDs) {
		if (itm == _id) { m_allChildsIDs.erase(it2); break; }
		it2++;
	}
}

bool ak::aTreeWidgetItem::hasChilds(void) {
	return !m_childs.empty();
}

ak::aTreeWidgetItem * ak::aTreeWidgetItem::popFirstChild(void) {
	if (m_childs.empty()) { return nullptr; }
	aTreeWidgetItem * itm = m_childs.front();
	m_childs.pop_front();
	return itm;
}

const std::list<ak::aTreeWidgetItem *> & ak::aTreeWidgetItem::childs(void) { return m_childs; }

const std::list<ak::aTreeWidgetItem *> & ak::aTreeWidgetItem::allChilds(void) {
	m_allChilds.clear();
	for (auto itm : m_childs) {
		m_allChilds.push_back(itm);
		const std::list<ak::aTreeWidgetItem *> & lst = itm->allChilds();
		for (auto cpy : lst) { m_allChilds.push_back(cpy); }
	}
	return m_allChilds;
}

const std::list<ak::ID> & ak::aTreeWidgetItem::allChildsIDs(void) {
	m_allChildsIDs.clear();
	for (auto itm : m_childs) {
		m_allChildsIDs.push_back(itm->id());
		const std::list<ak::ID> & lst = itm->allChildsIDs();
		for (auto cpy : lst) { m_allChildsIDs.push_back(cpy); }
	}
	return m_allChildsIDs;
}

int ak::aTreeWidgetItem::childCount(void) const { return m_childs.size(); }

ak::ID ak::aTreeWidgetItem::id(void) const { return m_id; }

ak::ID ak::aTreeWidgetItem::parentId(void) const {
	if (m_parent == nullptr) { return ak::invalidID; }
	else { return m_parent->id(); }
}

std::list<QString> ak::aTreeWidgetItem::getItemPath() {
	std::list<QString> ret;
	if (m_parent != nullptr) { ret = m_parent->getItemPath(); }
	ret.push_back(text(0));
	return ret;
}

QString ak::aTreeWidgetItem::getItemPathString(
	char									_delimiter
) {
	if (m_parent != nullptr) {
		QString ret(m_parent->getItemPathString(_delimiter));
		ret.append(_delimiter);
		ret.append(text(0));
		return ret;
	}
	else { return text(0); }
}

// ##############################################################################################

// Private functions

void ak::aTreeWidgetItem::refreshEditableState(void) {
	auto f = flags();
	f.setFlag(Qt::ItemIsEditable, m_isEditable && !m_isLockedForEdit);
	setFlags(f);
}