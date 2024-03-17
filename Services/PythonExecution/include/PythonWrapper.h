/*****************************************************************//**
 * \file   PythonWrapper.h
 * \brief  Wrapper around python code execution.
 * 
 * \author Wagner
 * \date   May 2023
 *********************************************************************/

#pragma once
#define PY_SSIZE_T_CLEAN // # 
#include <Python.h>
#include <string>
#include <vector>
#include <list>
#include "CPythonObjectBorrowed.h"
#include "CPythonObjectNew.h"
#include "PythonException.h"


	class  PythonWrapper
	{
		friend class FixturePythonWrapper;
	public:
		PythonWrapper();
		PythonWrapper(const PythonWrapper& other) = delete;
		PythonWrapper& operator=(const PythonWrapper& other) = delete;
		PythonWrapper(const PythonWrapper&& other) = delete;
		PythonWrapper& operator=(const PythonWrapper&& other) = delete;


		~PythonWrapper();
		
		void InitializePythonInterpreter();
		void ResetSysPath();
		void AddToSysPath(const std::string& newPathComponent);
		void ClosePythonInterpreter();

		CPythonObjectNew Execute(const std::string& executionCommand, const std::string& moduleName = "__main__");
		
		CPythonObjectNew ExecuteFunction(const std::string& functionName, CPythonObject& parameter, const std::string& moduleName = "__main__");
		
		CPythonObjectBorrowed GetGlobalVariable(const std::string& varName, const std::string& moduleName);

		CPythonObjectBorrowed GetGlobalDictionary(const std::string& moduleName);
		CPythonObjectNew GetFunction(const std::string& functionName, const std::string& moduleName = "__main__");

	private:
		std::list<std::string> _pythonPath;
		std::string _pythonRoot;
		std::string _defaultSitePackagesPath;
		bool _interpreterSuccessfullyInitialized = false;

		int initiateNumpy();
		std::string checkNumpyVersion();
		std::string DeterminePythonRootDirectory();
		std::string DeterminePythonSitePackageDirectory();

		static void signalHandlerAbort(int sig);

		CPythonObjectNew GetModule(const std::string& moduleName);
		
	};
