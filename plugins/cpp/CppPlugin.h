#pragma once

#include "core/IPlugin.h"

class CppPlugin : public QObject, public IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "jan.dalheimer.HideousWarehouse.IPlugin")
	Q_INTERFACES(IPlugin)

public:
	QString name() const override { return "C++ Plugin"; }
	BaseDocument *createDocument(BaseProject *project, const QString &filename, const QMimeType &mime) override;
	BaseProject *createProject(const QFileInfo &info) override { return nullptr; }
};
