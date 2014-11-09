#include "BaseDocument.h"

#include <QPlainTextDocumentLayout>
#include <QMimeDatabase>
#include <QDebug>

#include "util/File.h"

BaseDocument::BaseDocument(BaseProject *project, const QString &filename, QObject *parent) :
	QObject(parent), m_document(new QTextDocument(this)), m_project(project), m_filename(filename)
{
	connect(m_document, &QTextDocument::contentsChanged, this, &BaseDocument::contentsChanged);

	m_document->setDocumentLayout(new QPlainTextDocumentLayout(m_document));
	reloadDocument();
}

void BaseDocument::reloadDocument()
{
	m_document->setPlainText(QString::fromUtf8(File::openAndRead(m_filename)));
	m_document->setMetaInformation(QTextDocument::DocumentTitle, m_filename);
}
void BaseDocument::saveDocument()
{
	File::openAndWrite(m_filename, m_document->toPlainText().toUtf8());
}
