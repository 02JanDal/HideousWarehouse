#pragma once

#include <QSyntaxHighlighter>
#include <QAbstractItemModel>
#include <QRegularExpression>
#include <clang-c/Index.h>

#include "Project.h"

class CppDocument;

typedef QList<QPair<QString, int>> CompletionResult;

class CppSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit CppSyntaxHighlighter(CppDocument *doc);

private:
	void highlightBlock(const QString &text) override;

	QList<QPair<QRegularExpression, QTextCharFormat>> m_formats;
};

class CppOutlineModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit CppOutlineModel(QObject *parent = nullptr);

	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

public slots:
	void updateTranslationUnit(CppDocument *document, CXTranslationUnit unit);

private:
	struct Node
	{
		QString name;
		Node *parent = nullptr;
		QList<Node *> children;

		int row() const
		{
			if (parent)
			{
				return parent->children.indexOf(const_cast<Node *>(this));
			}
			return 0;
		}
	};
	Node *m_root;

	static QString ensureExists(QMap<QString, Node *> &nodes, const CXCursor cursor);
};

class Unit : public QObject
{
	Q_OBJECT
public:
	explicit Unit(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter, QObject *parent = nullptr);

	CompletionResult complete(unsigned int line, unsigned int column) const;

public slots:
	void parseTU();

private:
	CppDocument *m_document;
	CppOutlineModel *m_outline;
	CppSyntaxHighlighter *m_highlighter;
	CXIndex m_index;
	CXTranslationUnit m_tu = nullptr;

	CXUnsavedFile *createUnsavedFiles(const QList<class BaseDocument *> &docs) const;
};

class CppManager : public QObject
{
	Q_OBJECT
public:
	static CppManager *instance()
	{
		static CppManager man;
		return &man;
	}

	void registerDocument(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter);
	void unregisterDocument(CppDocument *document);
	CompletionResult complete(const CppDocument *document, unsigned int line, unsigned int column) const { return m_documents[const_cast<CppDocument *>(document)]->complete(line, column); }

	static QStringList createFlags(ProjectCodeModel *codeModel);

private:
	explicit CppManager()
		: QObject()
	{
	}

	QHash<CppDocument *, Unit *> m_documents;
};
