#include "EntityBlock.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, factory, owner)
{
}

void EntityBlock::AddConnector(const ot::Connector& connector)
{
	bool exists = false;
	for (auto& currentConnector : _connectors)
	{
		if (connector.getConnectorName() == currentConnector.getConnectorName())
		{
			exists = true;
		}
	}
	if (!exists)
	{
		_connectors.push_back(connector);
	}
	setModified();
}

void EntityBlock::RemoveConnector(const ot::Connector& connector)
{
	bool exists = false;
	std::list<ot::Connector> newConnectorList;
	for (auto& currentConnector : _connectors)
	{
		if (connector.getConnectorName() != currentConnector.getConnectorName())
		{
			newConnectorList.push_back(currentConnector);
		}
	}
	_connectors = newConnectorList;
	setModified();
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	
	storage.append(
		bsoncxx::builder::basic::kvp("BlockID", _blockID),
		bsoncxx::builder::basic::kvp("CoordinatesEntityID", static_cast<int64_t>(_coordinate2DEntityID)),
		bsoncxx::builder::basic::kvp("ServiceName", _info.serviceName()),
		bsoncxx::builder::basic::kvp("ServiceType", _info.serviceType()),
		bsoncxx::builder::basic::kvp("GraphicPackageName", _graphicsScenePackage)
	);

	auto connectorsArray = bsoncxx::builder::basic::array();
	for (const ot::Connector& connector : _connectors)
	{
		auto subDocument = connector.SerializeBSON();
		connectorsArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connectors", connectorsArray));

	auto outgoingConnectionArray = bsoncxx::builder::basic::array();
	for (ot::BlockConnection& connection : _outgoingConnections)
	{
		auto subDocument = connection.SerializeBSON();
		outgoingConnectionArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("OutgoingConnections", outgoingConnectionArray));

	auto ingoingConnectionArray = bsoncxx::builder::basic::array();
	for (ot::BlockConnection& connection : _ingoingConnections)
	{
		auto subDocument = connection.SerializeBSON();
		ingoingConnectionArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("IngoingConnections", ingoingConnectionArray));
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	
	_blockID = doc_view["BlockID"].get_utf8().value.data();
	_coordinate2DEntityID = static_cast<ot::UID>(doc_view["CoordinatesEntityID"].get_int64());
	_info.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
	_info.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	_graphicsScenePackage = doc_view["GraphicPackageName"].get_utf8().value.data();

	auto allOutgoingConnections = doc_view["OutgoingConnections"].get_array();
	for (auto& element : allOutgoingConnections.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::BlockConnection connection;
		connection.DeserializeBSON(subDocument);
		_outgoingConnections.push_back(connection);
	}

	auto allIngoingConnections = doc_view["IngoingConnections"].get_array();
	for (auto& element : allIngoingConnections.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::BlockConnection connection;
		connection.DeserializeBSON(subDocument);
		_ingoingConnections.push_back(connection);
	}

	auto allConnectors = doc_view["Connectors"].get_array();
	for (auto& element : allConnectors.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::Connector connector;
		connector.DeserializeBSON(subDocument);
		_connectors.push_back(connector);
	}
}
