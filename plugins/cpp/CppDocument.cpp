#include "CppDocument.h"

#include "CppSyntaxHighlighter.h"
#include "CppOutlineModel.h"
#include "CppCompletionModel.h"
#include "CppUnit.h"

CppDocument::CppDocument(BaseProject *project, const QString &filename, QObject *parent)
	: BaseDocument(project, filename, parent)
{
	m_highlighter = new CppSyntaxHighlighter(document());
	m_outline = new CppOutlineModel(this);
	m_completionModel = new CppCompletionResultModel(this);
	m_unit = new CppUnit(this, m_outline, m_highlighter, this);
	connect(this, &CppDocument::translationUnitUpdated, m_outline, &CppOutlineModel::updateTranslationUnit);
}
void CppDocument::complete(unsigned int line, unsigned int column) const
{
	m_completionModel->setResults(m_unit->complete(line, column));
}

QAbstractItemModel *CppDocument::outlineModel() const
{
	return m_outline;
}
QAbstractItemModel *CppDocument::completionModel() const
{
	return m_completionModel;
}
