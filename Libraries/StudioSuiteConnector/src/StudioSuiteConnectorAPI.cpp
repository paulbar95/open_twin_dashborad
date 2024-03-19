#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "StudioSuiteConnector/ProjectManager.h"

#include <QFileDialog>					// QFileDialog

void StudioSuiteConnectorAPI::openProject(std::string newProjectName)
{
	ProjectManager::getInstance().openProject(newProjectName);
}

void StudioSuiteConnectorAPI::setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject)
{
	ProjectManager::getInstance().setStudioServiceData(studioSuiteServiceURL, mainObject);
}

void StudioSuiteConnectorAPI::importProject(std::string fileName, std::string projectName)
{
	ProjectManager::getInstance().importProject(fileName, projectName);
}

std::string StudioSuiteConnectorAPI::getCurrentVersion(std::string fileName, std::string projectName)
{
	return ProjectManager::getInstance().getCurrentVersion(fileName, projectName);
}

void StudioSuiteConnectorAPI::commitProject(std::string fileName, std::string projectName, std::string changeComment)
{
	ProjectManager::getInstance().commitProject(fileName, projectName, changeComment);
}

void StudioSuiteConnectorAPI::getProject(std::string fileName, std::string projectName, std::string version)
{
	ProjectManager::getInstance().getProject(fileName, projectName, version);
}

void StudioSuiteConnectorAPI::uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList)
{
	ProjectManager::getInstance().uploadFiles(entityIDList, entityVersionList);
}

void StudioSuiteConnectorAPI::downloadFiles(std::string fileName, std::string projectName, std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, std::string version)
{
	ProjectManager::getInstance().downloadFiles(fileName, projectName, entityIDList, entityVersionList, version);
}

void StudioSuiteConnectorAPI::copyFiles(std::string newVersion)
{
	ProjectManager::getInstance().copyFiles(newVersion);
}

std::string StudioSuiteConnectorAPI::getLocalFileName()
{
	return ProjectManager::getInstance().getLocalFileName();
}

void StudioSuiteConnectorAPI::setLocalFileName(std::string projectName, std::string fileName)
{
	ProjectManager::getInstance().setLocalFileName(projectName, fileName);
}

bool StudioSuiteConnectorAPI::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists)
{
	return ProjectManager::getInstance().checkValidLocalFile(fileName, projectName, ensureProjectExists);
}
