#pragma once
#include <Python.h>
#include "CPythonObjectNew.h"
#include <stdint.h>
#include <string>

class PythonObjectBuilder
{
public:
	static PythonObjectBuilder* INSTANCE()
	{
		static PythonObjectBuilder instance;
		return &instance;
	}

	void StartTupleAssemply(int size);
	void operator<<(CPythonObject&& newEntry);
	CPythonObjectNew getAssembledTuple();

	int32_t getInt32Value(const CPythonObject& pValue, const std::string& varName);
	double getDoubleValue(const CPythonObject& pValue, const std::string& varName);
	std::string getStringValue(const CPythonObject& pValue, const std::string& varName);
	bool getBoolValue(const CPythonObject& pValue, const std::string& varName);

	CPythonObjectNew setInt32(const int32_t value);
	CPythonObjectNew setDouble(const double value);
	CPythonObjectNew setString(const std::string& value);
	CPythonObjectNew setBool(const bool value);

private:
	PythonObjectBuilder() : _assembly(nullptr) {};
	
	CPythonObjectNew _assembly;
	int _assemblySize = 0;
	int _currentSize = 0;
};
