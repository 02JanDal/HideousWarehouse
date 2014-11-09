#pragma once

#include <QObject>
#include <QTextDocument>
#include <QAbstractListModel>

#include "BaseProject.h"

class BaseDocument : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QTextDocument *document READ document CONSTANT)
	Q_PROPERTY(BaseProject *project READ project CONSTANT)
	Q_PROPERTY(QString filename READ filename CONSTANT)
	Q_PROPERTY(QString title READ title CONSTANT STORED false)
public:
	explicit BaseDocument(BaseProject *project, const QString &filename, QObject *parent = nullptr);

	QTextDocument *document() const { return m_document; }
	BaseProject *project() const { return m_project; }
	QString filename() const { return m_filename; }
	QString title() const { return m_filename.mid(m_filename.lastIndexOf('/') + 1); }

	virtual QAbstractItemModel *outlineModel() const { return nullptr; }
	virtual QAbstractItemModel *completionModel() const { return nullptr; }
	virtual void complete(unsigned int line, unsigned int column) const {}

signals:
	void contentsChanged();

public slots:
	void reloadDocument();
	void saveDocument();

private:
	QTextDocument *m_document;
	BaseProject *m_project;
	QString m_filename;
};
