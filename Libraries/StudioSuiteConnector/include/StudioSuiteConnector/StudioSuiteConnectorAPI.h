#pragma once

#include <string>

#include <qobject.h>

#include "OTCore/CoreTypes.h"

namespace StudioSuiteConnectorAPI
{
	__declspec(dllexport) void setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject);
	__declspec(dllexport) void importProject(std::string fileName, std::string projectName);
	__declspec(dllexport) void uploadAndCopyFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList);



}