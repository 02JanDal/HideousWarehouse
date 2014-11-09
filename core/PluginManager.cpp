#include "PluginManager.h"

#include <QPluginLoader>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QMimeDatabase>
#include <QFileInfo>

#include "IPlugin.h"
#include "BaseDocument.h"
#include "BaseProject.h"

PluginManager *PluginManager::instance()
{
	static PluginManager mngr;
	return &mngr;
}

void PluginManager::load()
{
	for (const auto file : QDir(qApp->applicationDirPath() + "/plugins").entryInfoList(QStringList() << "*.so"))
	{
		QPluginLoader loader(file.absoluteFilePath(), this);
		if (!loader.isLoaded() && !loader.load())
		{
			qWarning() << "Unable to load plugin" << loader.fileName() << ":" << loader.errorString();
			continue;
		}
		IPlugin *plugin = qobject_cast<IPlugin *>(loader.instance());
		if (plugin)
		{
			m_plugins.append(plugin);
			qDebug() << "Successfully loaded" << plugin->name();
		}
	}
}

BaseDocument *PluginManager::createDocument(BaseProject *project, const QString &filename) const
{
	QMimeType mime = QMimeDatabase().mimeTypeForFile(filename);
	for (const auto plugin : m_plugins)
	{
		BaseDocument *doc = plugin->createDocument(project, filename, mime);
		if (doc)
		{
			return doc;
		}
	}
	return new BaseDocument(project, filename, project);
}
BaseProject *PluginManager::createProject(const QFileInfo &info, QObject *parent) const
{
	for (const auto plugin : m_plugins)
	{
		BaseProject *proj = plugin->createProject(info);
		if (proj)
		{
			proj->setParent(parent);
			return proj;
		}
	}
	return new BaseProject(info.dir(), BaseProject::Plain, parent);
}

PluginManager::PluginManager()
	: QObject()
{
}
