// OpenTwin header
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Logger.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Items "Items"
#define OT_JSON_Member_Collections "Collections"

ot::GraphicsCollectionCfg::GraphicsCollectionCfg() {}

ot::GraphicsCollectionCfg::GraphicsCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle) : m_name(_collectionName), m_title(_collectionTitle) {

}

ot::GraphicsCollectionCfg::~GraphicsCollectionCfg() {
	this->memFree();
}

void ot::GraphicsCollectionCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_Member_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Title, m_title);

	OT_rJSON_createValueArray(collectionArr);
	for (auto c : m_collections) {
		OT_rJSON_createValueObject(collectionObj);
		c->addToJsonObject(_document, collectionObj);
		collectionArr.PushBack(collectionObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Collections, collectionArr);

	OT_rJSON_createValueArray(itemArr);
	for (auto i : m_items) {
		OT_rJSON_createValueObject(itemObj);
		i->addToJsonObject(_document, itemObj);
		itemArr.PushBack(itemObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Items, itemArr);
}

void ot::GraphicsCollectionCfg::setFromJsonObject(OT_rJSON_val& _object) {
	this->memFree();

	OT_rJSON_checkMember(_object, OT_JSON_Member_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Collections, Array);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Items, Array);

	m_name = _object[OT_JSON_Member_Name].GetString();
	m_title = _object[OT_JSON_Member_Title].GetString();

	OT_rJSON_val collectionArr = _object[OT_JSON_Member_Collections].GetArray();
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(collectionArr, i, Object);
		OT_rJSON_val collectionObj = collectionArr[i].GetObject();
		GraphicsCollectionCfg* newChild = new GraphicsCollectionCfg;
		try {
			newChild->setFromJsonObject(collectionObj);
			m_collections.push_back(newChild);
		}
		catch (...) {
			OT_LOG_E("Failed to create child collection. Abort");
			delete newChild;
			this->memFree();
			throw std::exception("Failed to create graphics collection");
		}
	}

	OT_rJSON_val itemArr = _object[OT_JSON_Member_Items].GetArray();
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemArr, i, Object);
		OT_rJSON_val itemObj = itemArr[i].GetObject();

		GraphicsItemCfg* itm = nullptr;
		try {
			
			itm = ot::SimpleFactory::instance().createType<GraphicsItemCfg>(itemObj);
			itm->setFromJsonObject(itemObj);
			m_items.push_back(itm);
		}
		catch (const std::exception& _e) {
			OT_LOG_E("Failed to create item by factory. " + std::string(_e.what()) + ". Abort");
			if (itm) delete itm;
			this->memFree();
			throw std::exception("Failed to create graphics item");
		}
		catch (...) {
			OT_LOG_E("Failed to create item by factory. Unknown error. Abort");
			if (itm) delete itm;
			this->memFree();
			throw std::exception("Failed to create graphics item");
		}
	}
}

void ot::GraphicsCollectionCfg::addChildCollection(GraphicsCollectionCfg* _child) {
	m_collections.push_back(_child);
}

void ot::GraphicsCollectionCfg::addItem(GraphicsItemCfg* _item) {
	m_items.push_back(_item);
}

void ot::GraphicsCollectionCfg::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();

	for (auto i : m_items) delete i;
	m_items.clear();

}
