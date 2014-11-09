#include "CppUnit.h"

#include <QTime>
#include <QDebug>

#include "CppDocument.h"
#include "CppCompletionModel.h"
#include "core/DocumentModel.h"

CppUnit::CppUnit(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter, QObject *parent)
	: QObject(parent), m_document(document), m_outline(outline), m_highlighter(highlighter)
{
	m_index = clang_createIndex(0, 0);
	parseTU();
	connect(document, &CppDocument::contentsChanged, this, &CppUnit::parseTU);
}

QVector<CompletionResult> CppUnit::complete(unsigned int line, unsigned int column) const
{
	const auto unsaved = DocumentModel::instance->getUnsavedDocuments();

	CXCodeCompleteResults *results = clang_codeCompleteAt(m_tu, m_document->filename().toLocal8Bit().constData(), line + 1, column + 1,
														 createUnsavedFiles(unsaved), unsaved.size(), clang_defaultCodeCompleteOptions() | CXCodeComplete_IncludeMacros | CXCodeComplete_IncludeCodePatterns | CXCodeComplete_IncludeBriefComments);
	QVector<CompletionResult> out(results->NumResults);
	for (int i = 0; i < results->NumResults; ++i)
	{
		out[i] = CompletionResult(results->Results[i].CompletionString, results->Results[i].CursorKind);
	}

	clang_disposeCodeCompleteResults(results);

	return out;
}

QString CppUnit::filename() const
{
	return m_document->filename();
}

void CppUnit::parseTU()
{
	auto args = createFlags(m_document->project()->codeModel());
	const char *arguments[args.size()];
	for (int i = 0; i < args.size(); ++i)
	{
		arguments[i] = args[i].toLocal8Bit().constData();
	}
	const auto unsaved = DocumentModel::instance->getUnsavedDocuments();
	QTime time = QTime::currentTime();
	if (m_tu)
	{
		qDebug() << "Reparsing C++...";
		clang_reparseTranslationUnit(m_tu, unsaved.size(), createUnsavedFiles(unsaved), clang_defaultReparseOptions(m_tu));
	}
	else
	{
		qDebug() << "Parsing C++...";
		m_tu = clang_parseTranslationUnit(m_index, m_document->filename().toLocal8Bit().constData(), arguments, args.size(), createUnsavedFiles(unsaved), unsaved.size(),
										  clang_defaultEditingTranslationUnitOptions() | CXTranslationUnit_CacheCompletionResults | CXTranslationUnit_IncludeBriefCommentsInCodeCompletion);
	}
	qDebug() << "Done after" << time.elapsed() << "ms";
	emit m_document->translationUnitUpdated(this);
}

CXUnsavedFile *CppUnit::createUnsavedFiles(const QList<BaseDocument *> &docs) const
{
	CXUnsavedFile *files = (CXUnsavedFile *)malloc(docs.size() * sizeof(CXUnsavedFile));
	for (int i = 0; i < docs.size(); ++i)
	{
		const QByteArray data = docs[i]->document()->toPlainText().toUtf8();
		files[i].Filename = docs[i]->filename().toLocal8Bit().constData();
		files[i].Contents = data.constData();
		files[i].Length = data.size();
	}
	return files;
}

QStringList CppUnit::createFlags(ProjectCodeModel *codeModel)
{
	QStringList out = codeModel->otherFlags;
	for (const auto incDir : codeModel->includeDirectories)
	{
		out += QString("-I" + incDir.absolutePath());
	}
	for (const auto def : codeModel->definitions)
	{
		out += QString("-D" + def);
	}
	return out;
}
