#pragma once
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/FolderNames.h"

class ExternalDependencies
{
public:
	void setPythonScriptFolderID(ot::UID id) { _pythonScriptFolderID = id; };
	ot::UID getPythonScriptFolderID() const { return _pythonScriptFolderID; }
private:
	static ot::UID _pythonScriptFolderID;
};