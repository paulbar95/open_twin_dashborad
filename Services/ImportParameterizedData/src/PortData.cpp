#include "PortData.h"
#include "OTCore/JSON.h"

ts::PortData::PortData(const uint32_t portNumber)
	:_numberOfEntries(portNumber*portNumber)
{
	_portData.reserve(_numberOfEntries);
}

ts::PortData::PortData(PortData&& other)
	:_numberOfEntries(other._numberOfEntries),_frequency(other._frequency), _portData(std::move(other._portData)), _buffer(other._buffer)
{
	other._frequency = nullptr;
	other._buffer = nullptr;
}

ts::PortData& ts::PortData::operator=(PortData&& other)
{
	_numberOfEntries = other._numberOfEntries;
	
	_frequency = other._frequency;
	other._frequency = nullptr;

	_portData = (std::move(other._portData));

	_buffer = (other._buffer);
	_buffer = nullptr;

	return *this;
}

ts::PortData::~PortData()
{
	if (_frequency != nullptr)
	{
		delete _frequency;
		_frequency = nullptr;
	}
	if (_buffer != nullptr)
	{
		delete _buffer;
		_buffer = nullptr;
	}
}


bool ts::PortData::AddValue(const std::string& value)
{
	if (isFilled())
	{
		return false;
	}
	else
	{
		if (_frequency == nullptr)
		{
			_frequency = new ot::Variable(StringToPortDataSingleEntry(value));
		}
		else
		{
			if (_buffer == nullptr)
			{
				_buffer = new ot::Variable(StringToPortDataSingleEntry(value));
			}
			else
			{
				const PortDataEntry entry(std::move(*_buffer), StringToPortDataSingleEntry(value));
				_portData.push_back(entry);
				_buffer = nullptr;
			}
		}
		return true;
	}
}

ot::Variable ts::PortData::StringToPortDataSingleEntry(const std::string& value)
{
	const std::string valueString = "{\"value\":" + value + "}";
	ot::JsonDocument doc;
	doc.fromJson(valueString);
	if (doc["value"].IsFloat())
	{
		return ot::Variable(doc["value"].GetFloat());
	}
	else if (doc["value"].IsDouble())
	{
		return ot::Variable(doc["value"].GetDouble());
	}
	else if (doc["value"].IsInt())
	{
		return ot::Variable(doc["value"].GetInt());
	}
	else if (doc["value"].IsInt64())
	{
		return ot::Variable(doc["value"].GetInt64());
	}
	else
	{
		throw std::exception(std::string("Failed to convert \"" + value + "\" into a floating point or integer value.").c_str());
	}
}
