#include "Document.h"

#include <QPlainTextDocumentLayout>
#include <QMimeDatabase>
#include <QDebug>

#include "File.h"

DocumentModel *DocumentModel::instance;

BaseDocument *BaseDocument::createDocument(BaseProject *project, const QString &filename, QObject *parent)
{
	const QMimeType mime = QMimeDatabase().mimeTypeForFile(filename);
	if (mime.allAncestors().contains("text/x-csrc") || mime.name() == "text/x-csrc")
	{
		return new CppDocument(project, filename, parent);
	}
	else
	{
		return new BaseDocument(project, filename, parent);
	}
}

BaseDocument::BaseDocument(BaseProject *project, const QString &filename, QObject *parent) :
	QObject(parent), m_document(new QTextDocument(this)), m_project(project), m_filename(filename)
{
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
	File(m_filename).write(m_document->toPlainText().toUtf8());
}

CppDocument::CppDocument(BaseProject *project, const QString &filename, QObject *parent)
	: BaseDocument(project, filename, parent)
{
	m_highlighter = new CppSyntaxHighlighter(this);
	m_outline = new CppOutlineModel(this);
	CppManager::instance()->registerDocument(this, m_outline, m_highlighter);
}
QList<QPair<QString, int> > CppDocument::completions(unsigned int line, unsigned int column) const
{
	return CppManager::instance()->complete(this, line, column);
}

DocumentModel::DocumentModel(BaseProject *project, QObject *parent)
	: QAbstractListModel(parent), m_project(project)
{
	instance = this;
}

int DocumentModel::rowCount(const QModelIndex &) const
{
	return m_documents.size();
}
QVariant DocumentModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole && index.column() == 0)
	{
		return m_documents.at(index.row())->title();
	}
	return QVariant();
}

int DocumentModel::rowFor(const BaseDocument *document) const
{
	return m_documents.indexOf(const_cast<BaseDocument *>(document));
}
BaseDocument *DocumentModel::at(const int row) const
{
	return m_documents.at(row);
}
BaseDocument *DocumentModel::documentFor(const QString &filename) const
{
	for (auto doc : m_documents)
	{
		if (doc->filename() == filename)
		{
			return doc;
		}
	}
	return nullptr;
}

BaseDocument *DocumentModel::open(const QString &filename)
{
	if (auto doc = documentFor(filename))
	{
		return doc;
	}
	BaseDocument *document = BaseDocument::createDocument(m_project, filename, this);
	beginInsertRows(QModelIndex(), m_documents.size(), m_documents.size());
	m_documents.append(document);
	endInsertRows();
	return document;
}
void DocumentModel::close(int row)
{
	beginRemoveRows(QModelIndex(), row, row);
	BaseDocument *document = m_documents.takeAt(row);
	endRemoveRows();
	document->deleteLater();
}

QList<BaseDocument *> DocumentModel::getUnsavedDocuments() const
{
	QList<BaseDocument *> out;
	for (auto doc : m_documents)
	{
		if (doc->document()->isModified())
		{
			out.append(doc);
		}
	}
	return out;
}

#include "Document.moc"
