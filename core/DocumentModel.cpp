#include "DocumentModel.h"

#include "core/BaseDocument.h"
#include "core/PluginManager.h"

DocumentModel *DocumentModel::instance;

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
	BaseDocument *document = PluginManager::instance()->createDocument(m_project, filename);
	document->setParent(this);
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
