#pragma once

#include <QObject>
#include <QTextDocument>
#include <QAbstractListModel>

#include "Project.h"
#include "CppParser.h"

class BaseDocument : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QTextDocument *document READ document CONSTANT)
	Q_PROPERTY(BaseProject *project READ project CONSTANT)
	Q_PROPERTY(QString filename READ filename CONSTANT)
	Q_PROPERTY(QString title READ title CONSTANT STORED false)
public:
	static BaseDocument *createDocument(BaseProject *project, const QString &filename, QObject *parent = nullptr);

	QTextDocument *document() const { return m_document; }
	BaseProject *project() const { return m_project; }
	QString filename() const { return m_filename; }
	QString title() const { return m_filename.mid(m_filename.lastIndexOf('/') + 1); }

	virtual QAbstractItemModel *outlineModel() {}
	virtual QList<QPair<QString, int>> completions(unsigned int line, unsigned int column) const {}

protected:
	explicit BaseDocument(BaseProject *project, const QString &filename, QObject *parent = nullptr);


signals:

public slots:
	void reloadDocument();
	void saveDocument();

private:
	QTextDocument *m_document;
	BaseProject *m_project;
	QString m_filename;
};

class CppDocument : public BaseDocument
{
	Q_OBJECT
public:
	explicit CppDocument(BaseProject *project, const QString &filename, QObject *parent = nullptr);

	QAbstractItemModel *outlineModel() override { return m_outline; }
	QList<QPair<QString, int>> completions(unsigned int line, unsigned int column) const override;

private:
	CppSyntaxHighlighter *m_highlighter;
	CppOutlineModel *m_outline;
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

	QList<BaseDocument *> getUnsavedDocuments() const;

	static DocumentModel *instance;

private:
	BaseProject *m_project;
	QList<BaseDocument *> m_documents;
};
