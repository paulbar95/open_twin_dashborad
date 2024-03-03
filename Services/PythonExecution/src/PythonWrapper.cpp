#include <signal.h>
#include "PythonWrapper.h"
#include "PythonObjectBuilder.h"
#include "PythonExtension.h"
#include "OTSystem/OperatingSystem.h"

PythonWrapper::PythonWrapper()
{
	//_pythonRoot = DeterminePythonRootDirectory();
	_pythonRoot = "C:\\ThirdParty\\Python\\Python3913";
	
	OT_LOG_D("Setting Python root path: " + _pythonRoot);
	_pythonPath.push_back(_pythonRoot);
	_pythonPath.push_back(_pythonRoot + "\\Lib");
	_pythonPath.push_back(_pythonRoot + "\\DLLs");
	std::string sitePackageDirectory = DeterminePythonSitePackageDirectory();
	_pythonPath.push_back(sitePackageDirectory);
	OT_LOG_D("Setting Python site-package path: " + sitePackageDirectory);
	signal(SIGABRT, &signalHandlerAbort);
}

PythonWrapper::~PythonWrapper()
{
	if (!_interpreterSuccessfullyInitialized)
	{
		ClosePythonInterpreter();
	}
}

void PythonWrapper::ClosePythonInterpreter()
{
	int success = Py_FinalizeEx();
	if (success == -1)
	{
		throw PythonException();
	}
}

void PythonWrapper::InitializePythonInterpreter()
{
	std::wstring allPaths;
	
	for (std::string& pathComponent : _pythonPath)
	{
		std::wstring temp(pathComponent.begin(), pathComponent.end());
		allPaths += temp + L";";
	}
	//PyStatus status;
	//PyConfig config;
	//config.program_name = "OpenTwinPython";
	//config.isolated = 1;
	//config.dump_refs = 1; // Dump all objects which are still alive at exit
	//config.pythonpath_env = pyhtonPath.c_str();
	//PyConfig_InitIsolatedConfig(&config);

	//status = Py_InitializeFromConfig(&config);
	//if (PyStatus_Exception(status))
	//{
	//	throw std::exception(("Initialization failed: " + std::string(status.err_msg)).c_str());
	//}
	//PyConfig_Clear(&config);

	int errorCode = PyImport_AppendInittab("OpenTwin", PythonExtensions::PyInit_OpenTwin);
	Py_SetPath(allPaths.c_str());
	int skipSignalHandlerRegistration = 0; //0: Skips; 1: not skipping (equivalent to Py_Initialize())
	Py_DontWriteBytecodeFlag = 1;
	Py_QuietFlag = 1;
	Py_InitializeEx(skipSignalHandlerRegistration);
	if (Py_IsInitialized() != 1)
	{
		throw PythonException();
	}
	_interpreterSuccessfullyInitialized = true;
}

void PythonWrapper::ResetSysPath()
{
	std::string resetPythonPath = "import sys\n"
		"sys.path.clear()\n";
	
	for (std::string& pathComponent : _pythonPath)
	{
		resetPythonPath += "sys.path.append(\"" +pathComponent +"\")\n";
	}
	Execute(resetPythonPath);
}

void PythonWrapper::AddToSysPath(const std::string& newPathComponent)
{
	const std::string addToPythonPath = "import sys\n"
		"sys.path.append(\""+newPathComponent+"\")\n";
	Execute(addToPythonPath);
}

std::string PythonWrapper::DeterminePythonRootDirectory()
{
	std::string envName = "OT_PYTHON_ROOT";
	const char*  pythonRoot = ot::os::getEnvironmentVariable(envName.c_str());
	
	if (pythonRoot == nullptr)
	{
		//Enduser execution from deployment folder
		return ".\\Python";
	}
	return std::string(pythonRoot);
}

std::string PythonWrapper::DeterminePythonSitePackageDirectory()
{
	std::string envName = "OT_PYTHON_SITE_PACKAGE_PATH";
	
	const char * pythonSitePackagePath = ot::os::getEnvironmentVariable(envName.c_str());


	std::string path;
	if (pythonSitePackagePath == nullptr)
	{
		envName = "OT_PYTHON_ROOT";
		const char* pythonRoot = ot::os::getEnvironmentVariable(envName.c_str());
		if (pythonRoot == nullptr)
		{
			//Enduser execution from deployment folder
			path = ".\\Python";
		}
		else
		{
			path = pythonRoot;
			path += "\\Lib";
		}
		path += "\\site-packages";
	}
	else
	{
		path =(pythonSitePackagePath);
	}
	return path;
}

void PythonWrapper::signalHandlerAbort(int sig)
{
	signal(SIGABRT, &signalHandlerAbort);
	throw std::exception("Abort was called.");
}

CPythonObjectNew PythonWrapper::Execute(const std::string& executionCommand, const std::string& moduleName)
{
	CPythonObjectNew module(GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectNew result(PyRun_String(executionCommand.c_str(), Py_file_input, globalDirectory, globalDirectory));
	if (result == nullptr)
	{
		throw PythonException();
	}
	return result;
}

CPythonObjectNew PythonWrapper::ExecuteFunction(const std::string& functionName, CPythonObject& parameter, const std::string& moduleName)
{
	CPythonObjectNew function(GetFunction(functionName, moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, parameter));
	
	if (!returnValue.ReferenceIsSet())
	{
		throw PythonException();
	}
	return returnValue;
}

CPythonObjectBorrowed PythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	CPythonObjectNew module = (GetModule(moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectBorrowed pythonVar(PyDict_GetItemString(globalDirectory, varName.c_str()));
	if (pythonVar == nullptr)
	{
		throw PythonException();
	}
	return pythonVar;
}

CPythonObjectBorrowed PythonWrapper::GetGlobalDictionary(const std::string& moduleName)
{
	CPythonObjectBorrowed module(PyImport_AddModule(moduleName.c_str()));
	return PyModule_GetDict(module);
}

CPythonObjectNew PythonWrapper::GetFunction(const std::string& functionName, const std::string& moduleName)
{

	CPythonObjectNew module = GetModule(moduleName);
	CPythonObjectNew function(PyObject_GetAttrString(module, functionName.c_str()));
	if (function == nullptr)
	{
		throw PythonException();
	}

	if (PyCallable_Check(function) != 1)
	{
		throw std::exception(("Function " + functionName + " does not exist in module " + moduleName).c_str());
	}
	return function;
}

CPythonObjectNew PythonWrapper::GetModule(const std::string& moduleName)
{
	PythonObjectBuilder builder;
	PyObject* module(PyImport_ImportModule(moduleName.c_str()));

	if (module == nullptr)
	{
		PyObject* type, * value, * traceback;
		PyErr_Fetch(&type, &value, &traceback);
		if (moduleName == "__main__")
		{
			throw PythonException();
		}
		CPythonObjectBorrowed newModule(PyImport_AddModule(moduleName.c_str()));
		return PyImport_ImportModule(moduleName.c_str());
	}
	else
	{
		return module;	
	}
}


