#include "CppOutlineModel.h"

#include <QPixmap>
#include <QFileInfo>

#include "CppUnit.h"

CppOutlineModel::CppOutlineModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	m_root = new Node;
}

QModelIndex CppOutlineModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
	{
		return QModelIndex();
	}

	Node *parentNode;
	if (!parent.isValid())
	{
		parentNode = m_root;
	}
	else
	{
		parentNode = static_cast<Node *>(parent.internalPointer());
	}

	Node *childNode = parentNode->children.at(row);
	if (childNode)
	{
		return createIndex(row, column, childNode);
	}
	else
	{
		return QModelIndex();
	}
}
QModelIndex CppOutlineModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	Node *childNode = static_cast<Node *>(child.internalPointer());
	Node *parentNode = childNode->parent;

	if (parentNode == m_root)
	{
		return QModelIndex();
	}

	return createIndex(parentNode->row(), 0, parentNode);
}

int CppOutlineModel::rowCount(const QModelIndex &parent) const
{
	Node *parentNode;
	if (!parent.isValid())
	{
		parentNode = m_root;
	}
	else
	{
		parentNode = static_cast<Node *>(parent.internalPointer());
	}

	return parentNode->children.size();
}
int CppOutlineModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant CppOutlineModel::data(const QModelIndex &index, int role) const
{
	Node *node = static_cast<Node *>(index.internalPointer());
	if (index.column() == 0 && role == Qt::DisplayRole)
	{
		return node->name;
	}
	else if (index.column() == 0 && role == Qt::DecorationRole)
	{
		switch (node->type)
		{
		case Node::Function:
			return QPixmap(":/function.png");
		case Node::Class:
		case Node::Variable:
		default:
			return QVariant();
		}
	}
	return QVariant();
}

static CppOutlineModel::Node::Type cursorKindToNodeType(const CXCursorKind kind)
{
	switch (kind)
	{
	case CXCursor_FunctionDecl: return CppOutlineModel::Node::Function;
	case CXCursor_CXXMethod: return CppOutlineModel::Node::Method;
	case CXCursor_VarDecl: return CppOutlineModel::Node::Variable;
	case CXCursor_FieldDecl: return CppOutlineModel::Node::Field;
	case CXCursor_ClassDecl: return CppOutlineModel::Node::Class;
	case CXCursor_StructDecl: return CppOutlineModel::Node::Struct;
	case CXCursor_TypedefDecl: return CppOutlineModel::Node::Class;
	default: return CppOutlineModel::Node::Invalid;
	}
}
void CppOutlineModel::updateTranslationUnit(CppUnit *unit)
{
	CXTranslationUnit clangTU = unit->clangTU();

	CXFile file = clang_getFile(clangTU, unit->filename().toLocal8Bit().constData());
	CXSourceRange range = clang_getRange(clang_getLocationForOffset(clangTU, file, 0), clang_getLocationForOffset(clangTU, file, QFileInfo(unit->filename()).size()));
	CXToken *tokens;
	unsigned int numTokens;
	clang_tokenize(clangTU, range, &tokens, &numTokens);
	CXCursor cursors[numTokens];
	clang_annotateTokens(clangTU, tokens, numTokens, cursors);

	QMap<QString, Node *> nodes;
	for (int i = 0; i < numTokens; ++i)
	{
		ensureExists(nodes, cursors[i]);
	}

	beginResetModel();
	qDeleteAll(m_root->children);
	m_root->children.clear();
	for (auto node : nodes.values())
	{
		if (node->parent)
		{
			continue;
		}
		node->parent = m_root;
		m_root->children.append(node);
	}
	endResetModel();

	clang_disposeTokens(clangTU, tokens, numTokens);
}
QString CppOutlineModel::ensureExists(QMap<QString, CppOutlineModel::Node *> &nodes, const CXCursor cursor)
{
	if (clang_Cursor_isNull(clang_getCursorSemanticParent(cursor)))
	{
		return QString();
	}
	const QString parent = ensureExists(nodes, clang_getCursorSemanticParent(cursor));
	if (clang_isDeclaration(clang_getCursorKind(cursor)) || clang_getCursorKind(cursor) == CXCursor_CXXMethod)
	{
		const QString current = clang_getCString(clang_getCursorSpelling(cursor));
		if (nodes.contains(parent + "::" + current))
		{
			return parent + "::" + current;
		}
		Node *node = new Node;
		node->type = cursorKindToNodeType(clang_getCursorKind(cursor));
		node->name = current;
		node->parent = nodes.value(parent);
		if (node->parent)
		{
			node->parent->children.append(node);
		}
		nodes.insert(parent + "::" + node->name, node);
		return parent + "::" + node->name;
	}
	else
	{
		return parent;
	}
}
