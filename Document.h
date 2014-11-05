#pragma once

#include <QObject>
#include <QTextDocument>
#include <QAbstractListModel>

#include "Project.h"

class BaseDocument : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QTextDocument *document READ document CONSTANT)
	Q_PROPERTY(QString filename READ filename CONSTANT)
	Q_PROPERTY(QString title READ title CONSTANT STORED false)
public:
	explicit BaseDocument(BaseProject *project, const QString &filename, QObject *parent = 0);

	QTextDocument *document() const { return m_document; }
	QString filename() const { return m_filename; }
	QString title() const { return m_filename.mid(m_filename.lastIndexOf('/') + 1); }

signals:

public slots:
	void reloadDocument();
	void saveDocument();

private:
	QTextDocument *m_document;
	BaseProject *m_project;
	QString m_filename;
};

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

private:
	BaseProject *m_project;
	QList<BaseDocument *> m_documents;
};
