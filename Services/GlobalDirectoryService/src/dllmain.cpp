/*
 * dllmain.cpp
 */

// C++ header
#include <Windows.h>
#include <iostream>

// OpenTwin header
#include <OpenTwinCore/Logger.h>
#include <OpenTwinFoundation/Dispatcher.h>
#include <OpenTwinCommunication/ServiceLogNotifier.h>
#include <OpenTwinFoundation/ExternalServicesComponent.h>

// Service header
#include <Application.h>

// Service header
#include "Application.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" {

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderURL) {
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderURL, ot::EXECUTE);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderURL)
	{
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderURL, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		std::string serviceURL = Application::instance()->serviceURL();

		char *strServiceURL = new char[serviceURL.size() + 1];
		strcpy_s(strServiceURL, serviceURL.size() + 1, serviceURL.c_str());

		return strServiceURL;
	}

	_declspec(dllexport) void deallocateData(const char * _data)
	{
		// Deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr)
		{
			delete[] _data;
		}
	};

	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownURL, const char * _globalSessionServiceURL, const char * _unused2)
	{
		std::string loggerServiceURL(_loggerServiceURL);

#ifdef _DEBUG
		std::cout << "Global Directory Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, loggerServiceURL, false);
#endif
		return Application::instance()->initialize("1", _ownURL, _globalSessionServiceURL);
	};
}