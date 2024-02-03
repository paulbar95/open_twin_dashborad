#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/ServiceConnector.h"
#include "StudioSuiteConnector/StudioConnector.h"
#include "StudioSuiteConnector/VersionFile.h"
#include "StudioSuiteConnector/ProgressInfo.h"

#include "OTCommunication/ActionTypes.h"

#include "EntityBinaryData.h"
#include "EntityFile.h"
#include "DataBase.h"

#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>

#include <QFileDialog>					// QFileDialog
#include <qdir.h>						// QDir

void ProjectManager::setStudioServiceData(const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	ServiceConnector::getInstance().setServiceURL(studioSuiteServiceURL);
	ServiceConnector::getInstance().setMainObject(mainObject);

	ProgressInfo::getInstance().setMainObject(mainObject);
}

void ProjectManager::importProject(const std::string& fileName, const std::string& prjName)
{
	try
	{
		uploadFileList.clear();
		projectName.clear();
		baseProjectName.clear();
		cacheFolderName.clear();
		newOrModifiedFiles.clear();
		dependentDataFiles.clear();
		deletedFiles.clear();

		projectName = prjName;
		changeMessage = "Import CST Studio Suite project";

		ProgressInfo::getInstance().setProgressState(true, "Importing project", false);
		ProgressInfo::getInstance().setProgressValue(0);

		// Determine the base project name (without .cst extension)
		baseProjectName = getBaseProjectName(fileName);

		// Create the cache folder
		cacheFolderName = createCacheFolder(baseProjectName);

		// Open the cst project in a studio suite instance
		StudioConnector::getInstance().openProject(fileName);

		// Now save the project (if needed) and extract the data 
		StudioConnector::getInstance().saveProject();
		StudioConnector::getInstance().extractInformation();

		// Get the files to be uploaded
		uploadFileList = determineUploadFiles(baseProjectName);

		ProgressInfo::getInstance().setProgressValue(10);

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_COUNT, 2 * uploadFileList.size(), doc.GetAllocator());  // We need ids for the data entities and the file entities

		ServiceConnector::getInstance().sendExecuteRequest(doc);
	}
	catch (std::string &error)
	{
		ProgressInfo::getInstance().setProgressState(false, "", false);
		ProgressInfo::getInstance().showError(error);
		ProgressInfo::getInstance().unlockGui();
	}
}

void ProjectManager::uploadFiles(std::list<ot::UID> &entityIDList, std::list<ot::UID> &entityVersionList)
{
	ProgressInfo::getInstance().setProgressValue(10);

	try
	{
		// Determine the project root folder
		std::filesystem::path projectPath(baseProjectName);
		std::string projectRoot = projectPath.parent_path().string();

		// Upload files
		uploadFiles(projectRoot, uploadFileList, entityIDList, entityVersionList);

		// Create the new version
		commitNewVersion(changeMessage);

		ProgressInfo::getInstance().setProgressValue(70);
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().showError(error);
	}

	ProgressInfo::getInstance().setProgressState(false, "", false);
	ProgressInfo::getInstance().unlockGui();
}

void ProjectManager::copyFiles(const std::string &newVersion)
{
	try
	{
		// Copy the files to the cache directory
		copyCacheFiles(baseProjectName, newVersion, cacheFolderName);

		// Store version information
		writeVersionFile(baseProjectName, projectName, newVersion, cacheFolderName);

		ProgressInfo::getInstance().setProgressValue(100);
		ProgressInfo::getInstance().showInformation("The CST Studio Suite project has been imported successfully.");
	}
	catch (std::string& error)
	{
		ProgressInfo::getInstance().showError(error);
	}

	ProgressInfo::getInstance().setProgressState(false, "", false);
	ProgressInfo::getInstance().unlockGui();
}

std::string ProjectManager::getBaseProjectName(const std::string& cstFileName)
{
	size_t index = cstFileName.rfind('.');
	if (index == std::string::npos) throw("Unable to determine project base name from cst file name: " + cstFileName);

	return cstFileName.substr(0, index);
}

std::string ProjectManager::createCacheFolder(const std::string& baseProjectName)
{
	std::string cacheFolderName = baseProjectName + ".cache";

	if (std::filesystem::exists(cacheFolderName))
	{
		throw("Unable to create cache folder since it already exists: " + cacheFolderName);
	}

	if (!std::filesystem::create_directory(cacheFolderName))
	{
		throw("Unable to create cache folder: " + cacheFolderName);
	}

	return cacheFolderName;
}

std::list<std::string> ProjectManager::determineUploadFiles(const std::string& baseProjectName)
{
	std::list<std::string> uploadFiles;
	std::string path;

	path = baseProjectName + ".cst";
	std::replace(path.begin(), path.end(), '\\', '/');

	uploadFiles.push_back(path);

	// Now add the content of the results folder
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(baseProjectName + "/Result"))
	{
		if (!dirEntry.is_directory())
		{
			path = dirEntry.path().string();
			std::replace(path.begin(), path.end(), '\\', '/');

			uploadFiles.push_back(path);
		}
	}

	return uploadFiles;
}

void ProjectManager::copyCacheFiles(const std::string& baseProjectName, const std::string &newVersion, const std::string &cacheFolderName)
{
	// Create new version subfolder
	std::string versionFolderName = cacheFolderName + "/" + newVersion;

	if (std::filesystem::exists(versionFolderName))
	{
		throw("Unable to create cache version folder since it already exists: " + versionFolderName);
	}

	if (!std::filesystem::create_directory(versionFolderName))
	{
		throw("Unable to create cache version folder: " + versionFolderName);
	}

	if (!std::filesystem::create_directory(versionFolderName + "/Result"))
	{
		throw("Unable to create cache result folder: " + versionFolderName + "/Result");
	}

	// Now copy all upload files to the cache version folder
	std::string cstFileName = baseProjectName + ".cst";
	std::string resultFolderName = baseProjectName + "/Result";

	std::replace(cstFileName.begin(), cstFileName.end(), '\\', '/');
	std::replace(resultFolderName.begin(), resultFolderName.end(), '\\', '/');

	try
	{
		std::filesystem::copy(cstFileName, versionFolderName);
		std::filesystem::copy(resultFolderName, versionFolderName + "/Result", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
	}
	catch (std::exception &error)
	{
		throw("Unable to copy data to cache (" + std::string(error.what()) + ") : " + versionFolderName);
	}
}

void ProjectManager::writeVersionFile(const std::string& baseProjectName, const std::string& projectName, const std::string& newVersion, const std::string& cacheFolderName)
{
	VersionFile version(std::string(cacheFolderName + "/version.info"));
	version.write(baseProjectName, projectName, newVersion);
}

void ProjectManager::uploadFiles(const std::string &projectRoot, std::list<std::string>& uploadFileList, std::list<ot::UID>& entityIDList, std::list<ot::UID>& entityVersionList)
{
	size_t dataSize = 0;

	DataBase::GetDataBase()->queueWriting(true);

	for (auto file : uploadFileList)
	{
		ot::UID dataEntityID = entityIDList.front(); entityIDList.pop_front();
		ot::UID fileEntityID = entityIDList.front(); entityIDList.pop_front();

		ot::UID dataVersion = entityVersionList.front(); entityVersionList.pop_front();
		ot::UID fileVersion = entityVersionList.front(); entityVersionList.pop_front();

		// add the file to the list of modified files
		newOrModifiedFiles[file] = std::pair<ot::UID, ot::UID>(fileEntityID, fileVersion);
		dependentDataFiles[file] = std::pair<ot::UID, ot::UID>(dataEntityID, dataVersion);

		// upload the binary data entity
		EntityBinaryData *dataEntity = new EntityBinaryData(dataEntityID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::ifstream dataFile(file, std::ios::binary | std::ios::ate);

		std::streampos size = dataFile.tellg();

		char *memBlock = new char[size];
		dataFile.seekg(0, std::ios::beg);
		dataFile.read(memBlock, size);
		dataFile.close();

		dataSize = size;

		dataEntity->setData(memBlock, size);
		dataEntity->StoreToDataBase(dataVersion);

		delete dataEntity;
		dataEntity = nullptr;

		delete[] memBlock;
		memBlock = nullptr;

		// Upload the file entity
		EntityFile* fileEntity = new EntityFile(fileEntityID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_STUDIOSUITE);

		std::string pathName = file.substr(projectRoot.size() + 1);
		std::filesystem::path filePath(pathName);

		fileEntity->setName("Files/" + pathName);
		fileEntity->setFileProperties(pathName, filePath.filename().string(), "Absolute");
		fileEntity->setData(dataEntityID, dataVersion);
		fileEntity->setEditable(false);

		fileEntity->StoreToDataBase(fileVersion);

		delete fileEntity;
		fileEntity = nullptr;

		if (dataSize > 100000000)
		{
			// We have more than 100 MB since the last store
			DataBase::GetDataBase()->flushWritingQueue();
			dataSize = 0;
		}
	}

	DataBase::GetDataBase()->queueWriting(false);
}

void ProjectManager::commitNewVersion(const std::string &changeMessage)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_FILES_UPLOADED, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(changeMessage, doc.GetAllocator()), doc.GetAllocator());

	// add the list of new files and modified files together with the entityID and versions
	std::list<std::string> fileNameList;
	std::list<ot::UID> fileEntityIDList;
	std::list<ot::UID> fileVersionList;
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataVersionList;

	for (auto item : newOrModifiedFiles)
	{
		fileNameList.push_back(item.first);
		fileEntityIDList.push_back(item.second.first);
		fileVersionList.push_back(item.second.second);

		dataEntityIDList.push_back(dependentDataFiles[item.first].first);
		dataVersionList.push_back(dependentDataFiles[item.first].second);
	}

	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonArray(fileNameList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, ot::JsonArray(fileEntityIDList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, ot::JsonArray(fileVersionList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_DataID, ot::JsonArray(dataEntityIDList, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_DataVersion, ot::JsonArray(dataVersionList, doc.GetAllocator()), doc.GetAllocator());

	// add the list of deleted files
	doc.AddMember(OT_ACTION_CMD_MODEL_DeleteEntity, ot::JsonArray(deletedFiles, doc.GetAllocator()), doc.GetAllocator());

	// Send the message to the service
	ServiceConnector::getInstance().sendExecuteRequest(doc);
}


