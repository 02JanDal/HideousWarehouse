#pragma once

#include "core/BaseDocument.h"

class CppSyntaxHighlighter;
class CppOutlineModel;
class CppCompletionResultModel;
class CppUnit;

class CppDocument : public BaseDocument
{
	Q_OBJECT
	friend class CppManager;
public:
	explicit CppDocument(BaseProject *project, const QString &filename, QObject *parent = nullptr);

	QAbstractItemModel *outlineModel() const override;
	QAbstractItemModel *completionModel() const override;
	void complete(unsigned int line, unsigned int column) const override;

	CppUnit *unit() const { return m_unit; }

signals:
	void translationUnitUpdated(CppUnit *unit);

private:
	CppSyntaxHighlighter *m_highlighter;
	CppOutlineModel *m_outline;
	CppCompletionResultModel *m_completionModel;
	CppUnit *m_unit;
};
