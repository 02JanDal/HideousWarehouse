#include "CMakePlugin.h"

#include "CMakeProject.h"

BaseProject *CMakePlugin::createProject(const QFileInfo &info)
{
	if (info.isFile() && info.fileName() == "CMakeLists.txt")
	{
		return new CMakeProject(info.dir());
	}
	return nullptr;
}
