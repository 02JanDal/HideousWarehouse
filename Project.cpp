#include "Project.h"

#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QTime>

#include "Json.h"

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
	updateCodeModel();
}
void CMakeProject::updateCodeModel()
{
	QTime time = QTime::currentTime();
	qDebug() << "Running CMake...";
	QProcess proc;
	proc.setProgram("cmake");
	proc.setArguments(QStringList() << "-DCMAKE_EXPORT_COMPILE_COMMANDS=1" << ".");
	proc.setWorkingDirectory(m_projectRoot.absoluteFilePath("build/"));
	proc.start();
	proc.waitForFinished();
	qDebug() << "  Done after" << time.elapsed() << "ms";
	const auto root = Json::ensureArray(Json::ensureDocument(m_projectRoot.absoluteFilePath("build/compile_commands.json")));

	const QString command = Json::ensureIsType<QString>(Json::ensureObject(root.first()), "command");
	m_codeModel = new ProjectCodeModel(
				QList<QDir>(),
				QList<QString>(),
				QList<QString>(command.mid(command.indexOf(' ') + 1).trimmed().split(' ', QString::SkipEmptyParts)),
				QList<QString>(),
				this);
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
