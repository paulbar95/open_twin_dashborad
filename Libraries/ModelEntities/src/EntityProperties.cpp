
#include "EntityProperties.h"

#include "OTCore/Logger.h"
#include "OTCore/Property.h"
#include "OTCore/PropertyGroup.h"
#include "OTCore/PropertyInt.h"
#include "OTCore/PropertyBool.h"
#include "OTCore/PropertyColor.h"
#include "OTCore/PropertyDouble.h"
#include "OTCore/PropertyString.h"
#include "OTCore/PropertyStringList.h"

#include <cassert>
#include <list>

EntityProperties::~EntityProperties()
{
	deleteAllProperties();
}

void EntityProperties::merge(EntityProperties& other)
{
	for (auto prop : other.propertiesList)
	{
		EntityPropertiesBase* property = getProperty(prop->getName());
		if (property == nullptr)
		{
			properties[prop->getName()] = prop;
			propertiesList.push_back(prop);
		}
	}
	other.properties.clear();
	other.propertiesList.clear();
}

void EntityProperties::deleteAllProperties(void)
{
	// Delete all properties
	for (std::map<std::string, EntityPropertiesBase *>::iterator it = properties.begin(); it != properties.end(); it++)
	{
		delete it->second;
	}

	// Now erase the map and the list
	properties.clear();
	propertiesList.clear();
}

bool EntityProperties::createProperty(EntityPropertiesBase *prop, const std::string &group)
{
	EntityPropertiesBase *property = getProperty(prop->getName());

	if (property != nullptr) return false; // Creation failed, because item already exits

	prop->setGroup(group);
	prop->setContainer(this);
	properties[prop->getName()] = prop;

	propertiesList.push_back(prop);

	return true;
}

bool EntityProperties::updateProperty(EntityPropertiesBase* prop, const std::string& group)
{
	EntityPropertiesBase* property = getProperty(prop->getName());

	if (property != nullptr)
	{
		propertiesList.remove(property);
		properties.erase(prop->getName());
		delete property;
		property = nullptr;
	}

	prop->setGroup(group);
	prop->setContainer(this);
	properties[prop->getName()] = prop;

	propertiesList.push_back(prop);

	return true;
}

bool EntityProperties::deleteProperty(const std::string &name)
{
	EntityPropertiesBase *property = getProperty(name);

	if (property == nullptr) return false; // Delete failed, because item does not exist

	properties.erase(name);
	propertiesList.remove(property);

	delete property;
	property = nullptr;

	return true;
}

EntityPropertiesBase *EntityProperties::getProperty(const std::string &name)
{
	if (properties.count(name) == 0) return nullptr;

	return properties[name];
}

bool EntityProperties::propertyExists(const std::string &name)
{
	return (properties.count(name) != 0);
}

void EntityProperties::checkWhetherUpdateNecessary(void)
{
	needsUpdate = false;

	for (std::map<std::string, EntityPropertiesBase *>::iterator it = properties.begin(); it != properties.end(); it++)
	{
		if (it->second->needsUpdate()) needsUpdate = true;
	}
}

void EntityProperties::forceResetUpdateForAllProperties(void)
{
	for (std::map<std::string, EntityPropertiesBase *>::iterator it = properties.begin(); it != properties.end(); it++)
	{
		it->second->resetNeedsUpdate();
	}

	needsUpdate = false;
}

EntityProperties::EntityProperties(const EntityProperties& other)
{
	needsUpdate = other.needsUpdate;

	for (auto prop : other.propertiesList)
	{
		// Add the current property
		EntityPropertiesBase* newProp = prop->createCopy();

		createProperty(newProp, prop->getGroup());
	}
}

EntityProperties& EntityProperties::operator=(const EntityProperties &other)
{
	deleteAllProperties();

	needsUpdate = other.needsUpdate;

	for (auto prop : other.propertiesList)
	{
		// Add the current property
		EntityPropertiesBase *newProp =prop->createCopy();

		createProperty(newProp, prop->getGroup());
	}

	return *this;
}

void EntityProperties::addToConfiguration(EntityBase *root, bool visibleOnly, ot::PropertyGridCfg& _configuration)
{
	// Here we convert the entire container with all its entities into a JSON document
		
	for (auto prop : propertiesList)
	{
		if (!visibleOnly || prop->getVisible())
		{
			prop->addToConfiguration(_configuration, root);
		}
	}
}

void EntityProperties::buildFromConfiguration(const ot::PropertyGridCfg& _config)
{
	// Here we re-build the current container with the settings in the JSON document. All previous settings will be overridden.

	deleteAllProperties();

	this->buildFromConfiguration(_config.defaultGroup());

	for (const ot::PropertyGroup* g : _config.rootGroups()) {
		this->buildFromConfiguration(g);
	}
}

void EntityProperties::buildFromConfiguration(const ot::PropertyGroup* _groupConfig)
{
	for (const ot::Property* p : _groupConfig->properties()) {
		EntityPropertiesBase* newSetting(nullptr);

		if (p->getPropertyType() == OT_PROPERTY_TYPE_Bool) newSetting = new EntityPropertiesBoolean;
		else if (p->getPropertyType() == OT_PROPERTY_TYPE_Color) newSetting = new EntityPropertiesColor;
		else if (p->getPropertyType() == OT_PROPERTY_TYPE_Int) newSetting = new EntityPropertiesInteger;
		else if (p->getPropertyType() == OT_PROPERTY_TYPE_Double) newSetting = new EntityPropertiesDouble;
		else if (p->getPropertyType() == OT_PROPERTY_TYPE_String) newSetting = new EntityPropertiesString;
		else if (p->getPropertyType() == OT_PROPERTY_TYPE_StringList) {
			if (p->specialType() == "EntityList") newSetting = new EntityPropertiesEntityList;
			else if (p->specialType() == "ProjectList") newSetting = new EntityPropertiesProjectList;
			else if (p->specialType().empty()) newSetting = new EntityPropertiesSelection;
			else {
				OT_LOG_E("Unknown string list property special type \"" + p->specialType() + "\"");
				return;
			}
		}
		else {
			OT_LOG_E("Unknown type \"" + p->getPropertyType() + "\"");
			return;
		}

		if (!newSetting) {
			OT_LOG_E("Failed to create property entity");
			return;
		}

		newSetting->setFromConfiguration(p);
		newSetting->setName(p->propertyName());
		newSetting->setHasMultipleValues(p->propertyFlags() & ot::Property::HasMultipleValues);
		newSetting->setReadOnly(p->propertyFlags() & ot::Property::IsReadOnly);
		newSetting->setProtected(!(p->propertyFlags() & ot::Property::IsDeletable));
		newSetting->setVisible(!(p->propertyFlags() & ot::Property::IsHidden));
		newSetting->setErrorState(p->propertyFlags() & ot::Property::HasInputError);

		this->createProperty(newSetting, _groupConfig->name());
	}
}

void EntityProperties::checkMatchingProperties(EntityProperties &other)
{
	// Here we check whether we have compatible properties to the collection handed over in the argument list.
	// If a property is not compatible, we remove it from the collection

	std::list<std::string> removeProperties;

	for (std::map<std::string, EntityPropertiesBase *>::const_iterator it = other.properties.begin(); it != other.properties.end(); it++)
	{
		EntityPropertiesBase *otherProp = it->second;

		EntityPropertiesBase *myProp = getProperty(otherProp->getName());

		bool compatible = false;

		if (myProp != nullptr)
		{
			if (otherProp->getType() == myProp->getType())
			{
				if (myProp->isCompatible(otherProp))
				{
					compatible = true;

					// Now check whether the properties have the same value
					if (!myProp->hasSameValue(otherProp))
					{
						otherProp->setHasMultipleValues(true);
					}

					// If one of the common properties is read only, the resulting property is read only
					if (myProp->getReadOnly())
					{
						otherProp->setReadOnly(true);
					}

					// If one of the common properties is protected, the resulting property is protected
					if (myProp->getProtected())
					{
						otherProp->setProtected(true);
					}
				}
			}
		}

		if (!compatible) removeProperties.push_back(otherProp->getName());
	}

	// Now finally remove the incompatible properties from the other container
	for (auto prop : removeProperties)
	{
		other.deleteProperty(prop);
	}
}

void EntityProperties::readFromProperties(const EntityProperties &other, EntityBase *root)
{
	// This function sets the properties of our entities from the values in the collection handed over in the argument list

	for (std::map<std::string, EntityPropertiesBase *>::const_iterator it = other.properties.begin(); it != other.properties.end(); it++)
	{
		EntityPropertiesBase *modified = it->second;

		EntityPropertiesBase *current = getProperty(modified->getName());
		assert(current != nullptr); // We need to have the same property localls

		if (current != nullptr)
		{
			// Check for compatible types
			bool isCompatible =
				modified->getType() == EntityPropertiesBase::SELECTION &&
				(current->getType() == EntityPropertiesBase::ENTITYLIST || current->getType() == EntityPropertiesBase::PROJECTLIST)
				|| modified->getType() == current->getType();
			assert(isCompatible);

			if (current->getReadOnly())
			{
				// The read only item can not be modified. Let's check whether there was an attempt to set another value.
				assert(current->hasSameValue(modified));
			}
			else
			{
				if (!modified->hasMultipleValues())
				{				
					if (isCompatible)
					{
						current->copySettings(modified, root);
					}
				}
			}
		}
	}
}

void EntityProperties::setAllPropertiesReadOnly(void)
{
	for (auto prop : properties)
	{
		prop.second->setReadOnly(true);
	}
}

void EntityProperties::setAllPropertiesNonProtected(void)
{
	for (auto prop : properties)
	{
		prop.second->setProtected(false);
	}
}

std::list<EntityPropertiesBase*> EntityProperties::getListOfAllProperties(void)
{
	std::list<EntityPropertiesBase*> allProperties;

	for (auto prop : properties)
	{
		allProperties.push_back(prop.second);
	}

	return allProperties;
}

std::list<EntityPropertiesBase *> EntityProperties::getListOfPropertiesWhichNeedUpdate(void)
{
	std::list<EntityPropertiesBase *> needsUpdate;

	for (auto prop : properties)
	{
		if (prop.second->needsUpdate())
		{
			needsUpdate.push_back(prop.second);
		}
	}

	return needsUpdate;
}

std::list<EntityPropertiesDouble *> EntityProperties::getListOfNumericalProperties(void)
{
	std::list<EntityPropertiesDouble *> numericalProperty;

	for (auto prop : properties)
	{
		EntityPropertiesDouble *doubleProp = dynamic_cast<EntityPropertiesDouble *>(prop.second);

		if (doubleProp != nullptr)
		{
			if (!prop.first.empty())
			{
				if (prop.first[0] == '#')
				{
					numericalProperty.push_back(doubleProp);
				}
			}
		}
	}

	return numericalProperty;
}

std::list<std::string> EntityProperties::getListOfPropertiesForGroup(const std::string &group)
{
	std::list<std::string> propertyList;

	for (auto prop : properties)
	{
		if (prop.second->getGroup() == group)
		{
			propertyList.push_back(prop.second->getName());
		}
	}

	return propertyList;
}

