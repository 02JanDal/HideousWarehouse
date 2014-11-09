#include "BaseProject.h"

#include <QDebug>

ProjectCodeModel::ProjectCodeModel(const QList<QDir> &includeDirectories, const QList<QString> &definitions, const QList<QString> &otherFlags, const QList<QString> &files, QObject *parent)
	: QObject(parent), includeDirectories(includeDirectories), definitions(definitions), otherFlags(otherFlags), files(files)
{
}

BaseProject::BaseProject(const QDir &projectRoot, const BaseProject::Type type, QObject *parent)
	: QObject(parent), m_projectRoot(projectRoot), m_type(type)
{
}
