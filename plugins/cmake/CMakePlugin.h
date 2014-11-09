#pragma once

#include "core/IPlugin.h"

class CMakePlugin : public QObject, public IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "jan.dalheimer.HideousWarehouse.IPlugin")
	Q_INTERFACES(IPlugin)

public:
	QString name() const override { return "CMake Plugin"; }
	BaseDocument *createDocument(BaseProject *project, const QString &filename, const QMimeType &mime) override { return nullptr; }
	BaseProject *createProject(const QFileInfo &info) override;
};
