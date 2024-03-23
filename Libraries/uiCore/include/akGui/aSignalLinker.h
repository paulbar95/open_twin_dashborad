/*
 *	File:		aSignalLinker.h
 *	Package:	akGui
 *
 *  Created on: February 21, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// C++ header
#include <map>

// Qt header
#include <qobject.h>				// base class
#include <qdockwidget.h>			// QDockWidgetArea

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>

// Forward declaration
class QKeyEvent;
class QTreeWidgetItem;

namespace ak {

	// Forward declaration
	class aMessenger;
	class aUidManager;
	class aObjectManager;
	class aAction;
	class aCheckBoxWidget;
	class aColorEditButtonWidget;
	class aComboBoxWidget;
	class aComboButtonWidget;
	class aDockWidget;
	class aLineEditWidget;
	class aLogInDialog;
	class aNiceLineEditWidget;
	class aPushButtonWidget;
	class aTableWidget;
	class aTabWidget;
	class aTextEditWidget;
	class aTimer;
	class aToolButtonWidget;
	class aToolButtonCustomContextMenu;
	class aGlobalKeyListener;

	//! @brief Used to link [Widget] signals to the messaging system
	//! Every added widget will get all its possible/most important signals connected to slots.
	//! In case of an inbound signal a message will be generated and sent to the messaging system.
	//! Every object requires a uid which should be generated with the global uid manager.
	class UICORE_API_EXPORT aSignalLinker : public QObject {
		Q_OBJECT
	public:

		//! @brief Default constructor
		//! @param _messanger A pointer to the globally used messaging system
		//! @param _uidManager A pointer to the globally used uid_manager
		aSignalLinker(
			aMessenger *						_messanger,
			aUidManager *						_uidManager
		);

		//! @brief Deconstructor		
		virtual ~aSignalLinker();

		// ###################################################################################
		// Add objects

		//! @brief Will create a new link for the provided action. Will return the object uid
		//! @param _object The action to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aAction *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided checkBox. Will return the object uid
		//! @param _object The checkBox to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aCheckBoxWidget *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided colorEditButton. Will return the object uid
		//! @param _object The colorEditButton to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aColorEditButtonWidget *							_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided comboBox. Will return the object uid
		//! @param _object The comboBox to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aComboBoxWidget *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided comboButton. Will return the object uid
		//! @param _object The comboButton to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid=invalidUID a new uid will be generated by the uid manager
		UID addLink(
			aComboButtonWidget *								_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided dock. Will return the object uid
		//! @param _object The dock to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid=invalidUID a new uid will be generated by the uid manager
		UID addLink(
			aDockWidget *										_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided lineEdit. Will return the object uid
		//! @param _object The lineEdit to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aLineEditWidget *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided lineEdit. Will return the object uid
		//! @param _object The lineEdit to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aLogInDialog *										_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided niceLineEdit. Will return the object uid
		//! @param _object The niceLineEdit to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aNiceLineEditWidget *								_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided pushButton. Will return the object uid
		//! @param _object The pushButton to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aPushButtonWidget *								_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided table. Will return the object uid
		//! @param _object The table to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aTableWidget *										_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided tabView. Will return the object uid
		//! @param _object The tabView to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aTabWidget *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided textEdit. Will return the object uid
		//! @param _object The textEdit to be linked to the messaging system
		//! @param _objectUid The uid of the object. If the provided uid is 0 a new uid will be generated by the uid manager
		UID addLink(
			aTextEditWidget *									_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided timer. Will return the object uid
		//! @param _object The timer to be linked to the messaging system
		//! @param _objectUid The UID of the object. If the provided UID = invalidUID a new UID will be generated by the UID manager
		UID addLink(
			aTimer *										_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided toolButton. Will return the object uid
		//! @param _object The toolButton to be linked to the messaging system
		//! @param _objectUid The UID of the object. If the provided UID = invalidUID a new UID will be generated by the UID manager
		UID addLink(
			aToolButtonWidget *								_object,
			UID													_objectUid = invalidUID
		);

		//! @brief Will create a new link for the provided toolButton. Will return the object uid
		//! @param _object The toolButton to be linked to the messaging system
		//! @param _objectUid The UID of the object. If the provided UID = invalidUID a new UID will be generated by the UID manager
		UID addLink(
			aToolButtonCustomContextMenu *						_object,
			UID													_objectUid = invalidUID
		);
				
		//! @brief Will create a new link for the provided global key listener. Will return the object uid
		//! @param _object The tree to be global key listener to the messaging system
		//! @param _objectUid The UID of the object. If the provided UID = invalidUID a new UID will be generated by the UID manager
		UID addLink(
			aGlobalKeyListener *								_object,
			UID													_objectUid = invalidUID
		);

	private slots:

		// ###################################################################################
		// ###################################################################################
		// ###################################################################################
		// SLOTS

		void slotIndexActivated(int);
		void slotChanged();
		void slotCleared();
		void slotClicked();
		void slotCursorPositionChanged();
		void slotCursorPositionChangedIndex(int _oldPos, int _newPos);
		void slotFocused();
		void slotFocusLost();
		void slotIndexChanged(int);
		void slotKeyPressed(QKeyEvent *);
		void slotKeyReleased(QKeyEvent *);
		void slotSelectionChanged();
		void slotStateChanged(int);
		void slotToggled(bool);
		void slotTimeout(void);
		void slotContextMenuItemClicked(ID);
		void slotContextMenuItemCheckedChanged(ID, bool);
		void slotVisibilityChanged(bool);
		void slotClosing(void);
		void slotReturnPressed(void);
		void slotEditingFinished(void);
		void slotKeyCombinationPressed(void);

		// ##### Items

		void slotItemChanged(ID);
		void slotItemDeleted(ID);
		void slotItemClicked(ID);
		void slotItemCloseRequested(ID);
		void slotItemDoubleClicked(ID);

		// ##### Table

		void tableCellActivated(int row, int coloumn);
		void tableCellChanged(int row, int coloumn);
		void tableCellClicked(int row, int coloumn);
		void tableCellDoubleClicked(int row, int coloumn);
		void tableCellEntered(int row, int coloumn);
		//void tableCellPressed(int row, int coloumn);

		// ##### Tree

		// ###################################################################################
		// ###################################################################################
		// ###################################################################################
		// Private members

	private:

		UID												m_uid;						//! The UID of the signal linker
		aUidManager *									m_uidManager;				//! The UID manager used in this object
		aMessenger *									m_messanger;				//! The messanger used in this object

		struct struct_object {														//! Contains information about an object
			QObject *		object;													//! A pointer to an object
			objectType		type;													//! The type of the object
		};

		std::map<UID, struct_object>					m_objects;					//! All objects registered in the signal linker
		typedef std::map<UID, struct_object>::iterator	m_objectsIterator;			//! Iterator used to iterate through the objects

		//! @brief Will create and send a event message with the provided attributes
		//! @param _senderUid The sender UID
		//! @param _eventType The type of the event which will be send as the message
		//! @param _info1 The message info 1
		//! @param _info2 The message info 2
		void raiseEvent(
			UID											_senderUid,
			eventType								_eventType,
			int												_info1,
			int												_info2
		);

		//! @brief Will return the UID of the specified object (object must contain a ui::core::aObject)
		//! @param _sender The object to get the UID from
		UID getSenderUid(QObject * _sender);

	}; // class signalLinker
} // namespace ak
