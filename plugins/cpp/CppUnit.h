#pragma once

#include <QObject>
#include <Index.h>

class CppDocument;
class CppOutlineModel;
class CppSyntaxHighlighter;
class ProjectCodeModel;
struct CompletionResult;

class CppUnit : public QObject
{
	Q_OBJECT
public:
	explicit CppUnit(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter, QObject *parent = nullptr);

	QVector<CompletionResult> complete(unsigned int line, unsigned int column) const;

	CppDocument *document() const { return m_document; }
	QString filename() const;
	CXTranslationUnit clangTU() const { return m_tu; }

public slots:
	void parseTU();

private:
	CppDocument *m_document;
	CppOutlineModel *m_outline;
	CppSyntaxHighlighter *m_highlighter;
	CXIndex m_index;
	CXTranslationUnit m_tu = nullptr;

	CXUnsavedFile *createUnsavedFiles(const QList<class BaseDocument *> &docs) const;
	static QStringList createFlags(ProjectCodeModel *codeModel);
};
