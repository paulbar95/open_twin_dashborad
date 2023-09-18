#pragma once
#include "MetadataEntry.h"
#include "OpenTwinCore/Variable.h"

#include <list>

class MetadataEntryArray : public MetadataEntry
{
public:
	MetadataEntryArray(const std::string name, std::list<ot::Variable>&& values) : MetadataEntry(name), _values(values){}
	MetadataEntryArray(const std::string name, const std::list<ot::Variable>& values) : MetadataEntry(name), _values(values){}
	const std::list<ot::Variable>& getValues() const { return _values; }

private:
	std::list<ot::Variable> _values;
};