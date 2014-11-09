#pragma once

#include <QString>
#include <QMimeType>
#include <QFileInfo>
#include <QtPlugin>

class BaseDocument;
class BaseProject;

class IPlugin
{
public:
	virtual ~IPlugin() {}

	virtual QString name() const = 0;
	virtual BaseDocument *createDocument(BaseProject *project, const QString &filename, const QMimeType &mime) = 0;
	virtual BaseProject *createProject(const QFileInfo &info) = 0;
};

#define IPLUGIN_iid "jan.dalheimer.HideousWarehouse.IPlugin"
Q_DECLARE_INTERFACE(IPlugin, IPLUGIN_iid)
