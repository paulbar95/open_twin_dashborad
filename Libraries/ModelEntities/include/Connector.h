#pragma once
#include <string>
#include "SerializeWithBSON.h"

namespace ot
{
	enum ConnectorType {Source = 0, Sink = 1, Filter = 2, UNKNOWN = -1};

	class __declspec(dllexport) Connector : public SerializeWithBSON
	{
	public:
		Connector(ConnectorType type, const std::string& connectorName);
		Connector();
		ConnectorType getConnectorType() const { return _connectorType; }
		virtual bsoncxx::builder::basic::document SerializeBSON() const  override;
		virtual void DeserializeBSON(bsoncxx::v_noabi::types::b_document& storage) override;

	private:
		ConnectorType _connectorType;
		std::string _connectorName;

	};
}