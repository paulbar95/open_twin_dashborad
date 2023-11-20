//! @file GraphicsEditorPackage.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTCore/SimpleFactory.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsCollectionCfg.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Items "Items"
#define OT_JSON_Member_Collections "Collections"
#define OT_JSON_Member_Connections "Connections"

ot::GraphicsCollectionPackage::GraphicsCollectionPackage() {

}

ot::GraphicsCollectionPackage::~GraphicsCollectionPackage() {
	this->memFree();
}

void ot::GraphicsCollectionPackage::addCollection(GraphicsCollectionCfg* _collection) {
	m_collections.push_back(_collection);
}

void ot::GraphicsCollectionPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueArray(collectionArr);
	for (auto c : m_collections) {
		OT_rJSON_createValueObject(collectionObj);
		c->addToJsonObject(_document, collectionObj);
		collectionArr.PushBack(collectionObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Collections, collectionArr);
}

void ot::GraphicsCollectionPackage::setFromJsonObject(OT_rJSON_val& _object) {
	this->memFree();

	OT_rJSON_checkMember(_object, OT_JSON_Member_Collections, Array);

	OT_rJSON_val collectionArr = _object[OT_JSON_Member_Collections].GetArray();
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(collectionArr, i, Object);
		OT_rJSON_val collectionObj = collectionArr[i].GetObject();
		GraphicsCollectionCfg* newCollection = new GraphicsCollectionCfg;
		try {
			newCollection->setFromJsonObject(collectionObj);
			m_collections.push_back(newCollection);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
			this->memFree();
			throw _e;
		}
	}
}

void ot::GraphicsCollectionPackage::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsNewEditorPackage::GraphicsNewEditorPackage(const std::string& _packageName, const std::string& _editorTitle)
	: m_name(_packageName), m_title(_editorTitle) {}

ot::GraphicsNewEditorPackage::~GraphicsNewEditorPackage() {
	
}

void ot::GraphicsNewEditorPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsCollectionPackage::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Title, m_title);
}

void ot::GraphicsNewEditorPackage::setFromJsonObject(OT_rJSON_val& _object) {
	ot::GraphicsCollectionPackage::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Title, String);
	m_name = ot::rJSON::getString(_object, OT_JSON_Member_Name);
	m_title = ot::rJSON::getString(_object, OT_JSON_Member_Title);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsScenePackage::GraphicsScenePackage(const std::string& _editorName) : m_name(_editorName) {

}

ot::GraphicsScenePackage::~GraphicsScenePackage() {
	this->memFree();
}

void ot::GraphicsScenePackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_Member_Name, m_name);

	OT_rJSON_createValueArray(itemsArr);
	for (auto itm : m_items) {
		OT_rJSON_createValueObject(itemObj);
		itm->addToJsonObject(_document, itemObj);
		itemsArr.PushBack(itemObj, _document.GetAllocator());
	}

	ot::rJSON::add(_document, _object, OT_JSON_Member_Items, itemsArr);
}

void ot::GraphicsScenePackage::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_Member_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Items, Array);

	this->memFree();

	m_name = ot::rJSON::getString(_object, OT_JSON_Member_Name);
	
	OT_rJSON_val itemsArr = _object[OT_JSON_Member_Items].GetArray();
	for (rapidjson::SizeType i = 0; i < itemsArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemsArr, i, Object);
		OT_rJSON_val itemObj = itemsArr[i].GetObject();
		ot::GraphicsItemCfg* itm = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(itemObj);
		OTAssertNullptr(itm);
		if (itm) {
			itm->setFromJsonObject(itemObj);
			if (itm) m_items.push_back(itm);
		}
	}
}

void ot::GraphicsScenePackage::addItem(GraphicsItemCfg* _item) {
	OTAssertNullptr(_item);
	m_items.push_back(_item);
}

void ot::GraphicsScenePackage::memFree(void) {
	for (auto itm : m_items) delete itm;
	m_items.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsConnectionPackage::GraphicsConnectionPackage(const std::string& _editorName) : m_name(_editorName) {

}

ot::GraphicsConnectionPackage::~GraphicsConnectionPackage() {

}

void ot::GraphicsConnectionPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueArray(cArr);
	for (auto c : m_connections) {
		OT_rJSON_createValueObject(cObj);
		c.addToJsonObject(_document, cObj);
		cArr.PushBack(cObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Connections, cArr);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Name, m_name);
}

void ot::GraphicsConnectionPackage::setFromJsonObject(OT_rJSON_val& _object) {
	m_connections.clear();

	OT_rJSON_checkMember(_object, OT_JSON_Member_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Connections, Array);
	
	m_name = _object[OT_JSON_Member_Name].GetString();

	OT_rJSON_val cArr = _object[OT_JSON_Member_Connections].GetArray();
	for (rapidjson::SizeType i = 0; i < cArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(cArr, i, Object);
		OT_rJSON_val cObj = cArr[i].GetObject();

		GraphicsConnectionCfg newConnection;
		newConnection.setFromJsonObject(cObj);

		m_connections.push_back(newConnection);
	}
}

void ot::GraphicsConnectionPackage::addConnection(const std::string& _fromUid, const std::string& _fromConnectable, const std::string& _toUid, const std::string& _toConnectable) {
	GraphicsConnectionCfg newConnection(_fromUid, _fromConnectable, _toUid, _toConnectable);
	this->addConnection(newConnection);
}
