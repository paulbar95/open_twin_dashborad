/*
 * dllmain.cpp
 */

// C++ header
#include <Windows.h>
#include "MinimalSubService.h"
#include "ActionHandler.h"

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

ActionHandler* actionHandler = nullptr;

extern "C" {

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		assert(actionHandler != nullptr);
		return actionHandler->Handle(_json,_senderIP);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP)
	{
		assert(actionHandler != nullptr);
		return actionHandler->Handle(_json, _senderIP);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		return MinimalSubService::INSTANCE().getServiceURL();
	}

	_declspec(dllexport) void deallocateData(const char * _data)
	{
		// *****************
		// This code will deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr)
		{
			delete[] _data;
		}
		// *****************
	};

#include "OpenTwinCore/rJSON.h"
	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char * _urlOwn, const char * _urlMasterService, const char * unused1, const char * unused2)
	{
		MinimalSubService::INSTANCE().Startup(_urlOwn, _urlMasterService);
		actionHandler = new ActionHandler(_urlMasterService);

		return 0;
	};
}