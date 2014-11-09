#include "CMakeProject.h"

#include <QDebug>
#include <QProcess>
#include <QTime>

#include "util/Json.h"

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
