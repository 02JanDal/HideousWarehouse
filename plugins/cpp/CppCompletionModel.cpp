#include "CppCompletionModel.h"

#include <QPixmap>
#include <QFont>

namespace Internal
{
CompletionResult::Availability getAvailability(CXCompletionString string)
{
	switch (clang_getCompletionAvailability(string))
	{
	case CXAvailability_Deprecated:
		return CompletionResult::Deprecated;
	case CXAvailability_NotAccessible:
		return CompletionResult::NotAccessible;
	case CXAvailability_NotAvailable:
		return CompletionResult::NotAvailable;
	default:
		return CompletionResult::Available;
	}
}
CompletionResult::Kind getKind(CXCompletionString string, CXCursorKind kind)
{
	switch (kind)
	{
	case CXCursor_Constructor: return CompletionResult::ConstructorCompletionKind;
	case CXCursor_Destructor: return CompletionResult::DestructorCompletionKind;
	case CXCursor_CXXMethod:
	case CXCursor_ConversionFunction:
	case CXCursor_FunctionDecl:
	case CXCursor_FunctionTemplate:
	case CXCursor_MemberRef:
	case CXCursor_MemberRefExpr:
		return CompletionResult::FunctionCompletionKind;
	case CXCursor_FieldDecl:
	case CXCursor_VarDecl:
	case CXCursor_ParmDecl:
	case CXCursor_NonTypeTemplateParameter:
		return CompletionResult::VariableCompletionKind;
	case CXCursor_Namespace:
	case CXCursor_NamespaceAlias:
	case CXCursor_NamespaceRef:
		return CompletionResult::NamespaceCompletionKind;
	case CXCursor_StructDecl:
	case CXCursor_UnionDecl:
	case CXCursor_ClassDecl:
	case CXCursor_TypeRef:
	case CXCursor_TemplateRef:
	case CXCursor_TypedefDecl:
	case CXCursor_ClassTemplate:
	case CXCursor_ClassTemplatePartialSpecialization:
	case CXCursor_TemplateTypeParameter:
	case CXCursor_TemplateTemplateParameter:
		return CompletionResult::ClassCompletionKind;
	case CXCursor_EnumConstantDecl:
		return CompletionResult::EnumeratorCompletionKind;
	case CXCursor_EnumDecl:
		return CompletionResult::EnumCompletionKind;
	case CXCursor_MacroDefinition:
	{
		const unsigned numChunks = clang_getNumCompletionChunks(string);
		for (unsigned int i = 0; i < numChunks; ++i)
		{
			CXCompletionChunkKind kind = clang_getCompletionChunkKind(string, i);
			if (kind == CXCompletionChunk_Placeholder)
			{
				return CompletionResult::FunctionCompletionKind;
			}
		}
		return CompletionResult::PreProcessorCompletionKind;
	}
	case CXCursor_PreprocessingDirective:
	case CXCursor_MacroExpansion:
	case CXCursor_InclusionDirective:
		return CompletionResult::PreProcessorCompletionKind;
	case CXCursor_ObjCClassMethodDecl:
	case CXCursor_ObjCInstanceMethodDecl:
		return CompletionResult::ObjCMessageCompletionKind;
	case CXCursor_NotImplemented:
	{
		const unsigned numChunks = clang_getNumCompletionChunks(string);
		for (unsigned int i = 0; i < numChunks; ++i)
		{
			CXCompletionChunkKind kind = clang_getCompletionChunkKind(string, i);
			if (kind == CXCompletionChunk_Placeholder)
			{
				return CompletionResult::ClangSnippetKind;
			}
		}
		return CompletionResult::KeywordCompletionKind;
	}
	default:
		return CompletionResult::Other;
	}

}
}

CompletionResult::CompletionResult(CXCompletionString string, CXCursorKind cursorKind)
{
	QString result;
	for (int i = 0; i < clang_getNumCompletionChunks(string); ++i)
	{
		switch (clang_getCompletionChunkKind(string, i))
		{
		case CXCompletionChunk_Text:
		case CXCompletionChunk_TypedText:
		case CXCompletionChunk_Comma:
		case CXCompletionChunk_HorizontalSpace:
		case CXCompletionChunk_LeftAngle:
		case CXCompletionChunk_RightAngle:
			result += QString::fromUtf8(clang_getCString(clang_getCompletionChunkText(string, i)));
		}
	}
	text = snippet = hint = result;
	availability = Internal::getAvailability(string);
	kind = Internal::getKind(string, cursorKind);
	priority = clang_getCompletionPriority(string);
}

CppCompletionResultModel::CppCompletionResultModel(QObject *parent)
	: QAbstractListModel(parent)
{
}
int CppCompletionResultModel::rowCount(const QModelIndex &parent) const
{
	return m_results.size();
}
QVariant CppCompletionResultModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || !hasIndex(index.row(), index.column(), index.parent()))
	{
		return 0;
	}

	if (role == Qt::DecorationRole)
	{
		switch (m_results[index.row()].kind)
		{
		case CompletionResult::FunctionCompletionKind: return QPixmap(":/function.png");
		default:
			return QVariant();
		}
	}
	else if (role == Qt::FontRole)
	{
		QFont font;
		if (m_results[index.row()].availability == CompletionResult::Deprecated)
		{
			font.setStrikeOut(true);
		}
		return font;
	}
	else if (role == Qt::DisplayRole)
	{
		return m_results[index.row()].text;
	}
	else if (role == Qt::EditRole)
	{
		return m_results[index.row()].snippet;
	}
	else
	{
		return QVariant();
	}
}
void CppCompletionResultModel::setResults(const QVector<CompletionResult> &results)
{
	beginResetModel();
	m_results = results;
	endResetModel();
}
