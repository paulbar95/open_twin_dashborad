#include "ProjectToCollectionConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/ReturnMessage.h"


ProjectToCollectionConverter::ProjectToCollectionConverter(const std::string& sessionServiceURL)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, doc.GetAllocator()), doc.GetAllocator());
	ot::msg::send("", sessionServiceURL, ot::EXECUTE, doc.toJson(), _authorisationService);
}

std::string ProjectToCollectionConverter::NameCorrespondingCollection(const std::string& projectName, const std::string& userName, const std::string& userPSW)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(userPSW, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	
	ot::msg::send("", _authorisationService, ot::EXECUTE, doc.toJson(), response);
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);

	if (responseMessage.getStatus() == ot::ReturnMessage::Failed)
	{
		throw std::exception(std::string("Could not get information about project \"" + projectName + "\".").c_str());
	}
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());
	return ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
}
