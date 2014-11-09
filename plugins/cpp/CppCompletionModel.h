#pragma once

#include <QString>
#include <QAbstractListModel>
#include <Index.h>

struct CompletionResult
{
	explicit CompletionResult() {}
	explicit CompletionResult(CXCompletionString string, CXCursorKind cursorKind);
	enum Kind
	{
		Other = 0,
		FunctionCompletionKind,
		ConstructorCompletionKind,
		DestructorCompletionKind,
		VariableCompletionKind,
		ClassCompletionKind,
		EnumCompletionKind,
		EnumeratorCompletionKind,
		NamespaceCompletionKind,
		PreProcessorCompletionKind,
		SignalCompletionKind,
		SlotCompletionKind,
		ObjCMessageCompletionKind,
		KeywordCompletionKind,
		ClangSnippetKind
	};

	enum Availability
	{
		Available,
		Deprecated,
		NotAvailable,
		NotAccessible
	};

	QString hint;
	QString text;
	QString snippet;
	QString comment;
	unsigned int priority;
	Kind kind;
	Availability availability;
};

class CppCompletionResultModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit CppCompletionResultModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	void setResults(const QVector<CompletionResult> &results);

private:
	QVector<CompletionResult> m_results;
};
