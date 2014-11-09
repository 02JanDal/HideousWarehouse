#include "CppPlugin.h"

#include <QMimeType>

#include "CppDocument.h"

BaseDocument *CppPlugin::createDocument(BaseProject *project, const QString &filename, const QMimeType &mime)
{
	if (mime.allAncestors().contains("text/x-csrc") || mime.name() == "text/x-csrc")
	{
		return new CppDocument(project, filename);
	}
	else
	{
		return nullptr;
	}
}
