#include "SubprocessHandler.h"
#include "openTwinSystem/Application.h"
#include "openTwinSystem/OperatingSystem.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCommunication/Msg.h"
#include <assert.h>
#include <condition_variable>
#include <chrono>

SubprocessHandler::SubprocessHandler()
{
	std::string envName = "OPENTWIN_DEV_ROOT";
	const char* envValue = ot::os::getEnvironmentVariable(envName.c_str());

	std::string baseDirectory;
	if (envValue == "")
	{
		baseDirectory = ot::os::getExecutablePath();
	}
	else
	{
		baseDirectory = envValue;
	}
	
	if (baseDirectory == "")
	{
		throw std::exception("Failed to determine launcherpath");
	}

	_launcherPath = baseDirectory + "\\Deployment\\open_twin.exe";
	_subprocessPath = baseDirectory + "\\Deployment\\PythonExecution.dll";
	
	OT_rJSON_createDOC(pingDoc);
	ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	_pingCommand = ot::rJSON::toJSON(pingDoc);
}

void SubprocessHandler::Create(const std::string& urlThisProcess)
{	
	assert(_launcherPath != "" && _subprocessPath != "");

	if (CheckAlive(_subprocess))
	{
		if (PingSubprocess())
		{
			Close();
		}
	}

	RunWithNextFreeURL(urlThisProcess);
}

void SubprocessHandler::RunWithNextFreeURL(const std::string& urlThisService)
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	using namespace std::chrono_literals;

	int counter = 0;
	while(true)
	{
		std::string urlSubprocess = urlThisService.substr(0, urlThisService.find(':') + 1) + std::to_string(_startPort + counter);
		std::string commandLine = _launcherPath + " \"" + _subprocessPath + "\" \"1\" \"" + urlSubprocess + "\" \"" + urlThisService+ "\" \"unused\"";
		
		ot::app::RunResult result = ot::app::GeneralError;
		result = ot::app::runApplication(_launcherPath, commandLine, _subprocess, false, 0);

		assert(result == ot::app::OK); //ToDo: When would this case occue?
		
		if (CheckAlive(_subprocess))
		{
			auto now = std::chrono::system_clock::now();
			bool receivedNotification = _receivedInitializationRequest.wait_until(lock, now + 5s, [this]() {return this->getReceivedInitializationRequest(); });
			if (receivedNotification)
			{
				_urlSubprocess = urlSubprocess;
				break;
			}
		}

		CloseProcess(urlSubprocess);
		counter++;
	}
}


bool SubprocessHandler::CheckAlive(OT_PROCESS_HANDLE& handle)
{
#if defined(OT_OS_WINDOWS)
	// Checking the exit code of the service
	if (handle == OT_INVALID_PROCESS_HANDLE) return false;

	
	DWORD exitCode = STILL_ACTIVE;
	
	if (GetExitCodeProcess(handle, &exitCode))
	{
		if (exitCode != STILL_ACTIVE) {
			CloseHandle(handle);
			handle = OT_INVALID_PROCESS_HANDLE;
			return false;
		}
		else {
			return true;
		}
	}
	else {

		CloseHandle(handle);
		handle = OT_INVALID_PROCESS_HANDLE;
		return false;
	}
#else
	#error ("Function is implemented only for Windows OS");
#endif // OT_OS_WINDOWS

}

bool SubprocessHandler::PingSubprocess()
{
	//for (int pingAttempt = 0; pingAttempt < _maxPingAttempts; pingAttempt++)
	//{
	//	std::string response;
	//	if (ot::msg::send("", _urlSubprocess, ot::EXECUTE, _pingCommand, response, 1000))
	//	{
	//		return true;
	//	}
	//}
	return false;
}

bool SubprocessHandler::CloseProcess(const std::string& url)
{
	CloseHandle(_subprocess);
	_subprocess = OT_INVALID_PROCESS_HANDLE;

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceShutdown);
	std::string response;
	if (ot::msg::send("", _urlSubprocess, ot::EXECUTE, ot::rJSON::toJSON(doc), response, 1000))
	{
		return true;
	}
	return false;
}

void SubprocessHandler::setReceivedInitializationRequest()
{
	_receivedInitializationRequestAlready = true;
	_receivedInitializationRequest.notify_all();
}

bool SubprocessHandler::Close()
{
	return CloseProcess(_urlSubprocess);
}
