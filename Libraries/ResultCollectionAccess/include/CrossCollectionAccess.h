/*****************************************************************//**
 * \file   CrossCollectionAccess.h
 * \brief  Interface to access a different project and get its metadata regarding the result collection.
 * 
 * \author Wagner
 * \date   September 2023
 *********************************************************************/

#pragma once
#include <memory>
#include <list>

#include "OpenTwinCore/CoreTypes.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"
#include "OpenTwinFoundation/ModelComponent.h"

class __declspec(dllexport) CrossCollectionAccess
{
public:
	CrossCollectionAccess(const std::string& projectName, const std::string& sessionServiceURL, const std::string& modelServiceURL);
	std::list<std::shared_ptr<EntityMeasurementMetadata>> getMeasurementMetadata(ot::components::ModelComponent* modelComponent);
	std::shared_ptr<EntityResearchMetadata> getMeasurementCampaignMetadata(ot::components::ModelComponent* modelComponent);

	bool ConnectedWithCollection() { return _collectionName != ""; }
private:
	std::string _projectName = "";
	std::string _collectionName = "";
	std::string _modelServiceURL = "";
	std::string InquireAuthorisationURL(const std::string& sessionServiceURL);
	void InquireProjectCollection(const std::string& authorisationURL);
	std::pair<ot::UIDList,ot::UIDList> InquireMetadataEntityIdentifier(const std::string& className);

};

class DataBaseWrapper
{
public:
	DataBaseWrapper(const std::string& collectionName);
	~DataBaseWrapper();
private:
	const std::string _oldCollectionName;
};