#pragma once

#include <QObject>

class BaseDocument;
class BaseProject;
class IPlugin;
class QFileInfo;

class PluginManager : public QObject
{
	Q_OBJECT
public:
	static PluginManager *instance();

	void load();

	BaseDocument *createDocument(BaseProject *project, const QString &filename) const;
	BaseProject *createProject(const QFileInfo &info, QObject *parent = nullptr) const;

private:
	explicit PluginManager();

	QList<IPlugin *> m_plugins;
};
