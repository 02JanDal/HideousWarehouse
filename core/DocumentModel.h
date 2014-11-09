#pragma once

#include <QAbstractListModel>

class BaseProject;
class BaseDocument;

class DocumentModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit DocumentModel(BaseProject *project, QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	int rowFor(const BaseDocument *document) const;
	QModelIndex index(const BaseDocument *document) const { return index(rowFor(document), 0); }
	using QAbstractListModel::index;
	BaseDocument *at(const int row) const;
	BaseDocument *at(const QModelIndex &index) const { return at(index.row()); }
	BaseDocument *documentFor(const QString &filename) const;

	BaseDocument *open(const QString &filename);
	void close(int row);

	QList<BaseDocument *> getUnsavedDocuments() const;

	static DocumentModel *instance;

private:
	BaseProject *m_project;
	QList<BaseDocument *> m_documents;
};
