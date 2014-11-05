#include "Project.h"

#include <QFileInfo>
#include <QDebug>

ProjectCodeModel::ProjectCodeModel(const QList<QDir> &includeDirectories, const QList<QString> &definitions, const QList<QString> &otherFlags, const QList<QString> &files, QObject *parent)
	: QObject(parent), includeDirectories(includeDirectories), definitions(definitions), otherFlags(otherFlags), files(files)
{
}

BaseProject *BaseProject::createProject(const QFileInfo &info, QObject *parent)
{
	if (info.isFile() && info.fileName() == "CMakeLists.txt")
	{
		return new CMakeProject(info.dir(), parent);
	}
	else if (info.isDir())
	{
		return new PlainProject(info.absoluteDir(), parent);
	}
	else
	{
		return new PlainProject(info.dir(), parent);
	}
}

BaseProject::BaseProject(const QDir &projectRoot, const BaseProject::Type type, QObject *parent)
	: QObject(parent), m_projectRoot(projectRoot), m_type(type)
{
}

CMakeProject::CMakeProject(const QDir &projectRoot, QObject *parent)
	: BaseProject(projectRoot, CMake, parent)
{
	qDebug() << "Loading a CMake project in" << projectRoot.absolutePath();
}
void CMakeProject::updateCodeModel()
{
	// TODO
}

PlainProject::PlainProject(const QDir &projectRoot, QObject *parent)
	: BaseProject(projectRoot, Plain, parent)
{
	qDebug() << "Loading a Plain project in" << projectRoot.absolutePath();
}
void PlainProject::updateCodeModel()
{
	// TODO
}
