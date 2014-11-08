#include "CppParser.h"

#include <QDebug>
#include <QTime>

#include "Document.h"

enum
{
	InComment,
	OutsideComment
};

QTextCharFormat createFmt(const QColor &color, bool bold = false, bool italic = false)
{
	QTextCharFormat fmt;
	fmt.setFontFamily("Monospace");
	fmt.setForeground(QBrush(color));
	fmt.setFontWeight(bold ? QFont::Bold : QFont::Normal);
	fmt.setFontItalic(italic);
	return fmt;
}

CppSyntaxHighlighter::CppSyntaxHighlighter(CppDocument *doc)
	: QSyntaxHighlighter(doc->document())
{
	m_formats.append(qMakePair(QRegularExpression("\\b(alignas|alignof|and|and_eq|asm|auto|bitand|bitor|bool|break|case|catch|char|char16_t|char32_t|class|compl|const|constexpr|const_cast|continue|decltype|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|or_eq|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\\b"),
							   createFmt(Qt::darkYellow)));
	m_formats.append(qMakePair(QRegularExpression("\\d"), createFmt(Qt::darkBlue)));
	m_formats.append(qMakePair(QRegularExpression("<.*?>"), createFmt(Qt::darkGreen)));
	m_formats.append(qMakePair(QRegularExpression("\".*?\""), createFmt(Qt::darkGreen)));
	m_formats.append(qMakePair(QRegularExpression("\'.\'"), createFmt(Qt::darkGreen)));
	m_formats.append(qMakePair(QRegularExpression(" *#(define|undef|include|if|ifdef|ifndef|else|elif|endif|line|error|pragma)"), createFmt(Qt::darkBlue)));
	m_formats.append(qMakePair(QRegularExpression("//.*"), createFmt(Qt::darkGreen)));
}
void CppSyntaxHighlighter::highlightBlock(const QString &text)
{
	setFormat(0, text.size(), createFmt(Qt::black));
	for (const auto pair : m_formats)
	{
		QRegularExpressionMatchIterator it = pair.first.globalMatch(text);
		while (it.hasNext())
		{
			const auto match = it.next();
			setFormat(match.capturedStart(), match.capturedLength(), pair.second);
		}
	}

	// multiline comments
	{
		QRegularExpression startExpression("/\\*");
		QRegularExpression endExpression("\\*/");

		setCurrentBlockState(0);

		int startIndex = 0;
		if (previousBlockState() != 1)
		{
			startIndex = startExpression.match(text).capturedStart();
		}

		while (startIndex >= 0)
		{
			QRegularExpressionMatch match = endExpression.match(text, startIndex);
			int commentLength;
			if (!match.hasMatch())
			{
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			}
			else
			{
				commentLength = match.capturedEnd() - startIndex
						+ match.capturedLength();
			}
			setFormat(startIndex, commentLength, createFmt(Qt::darkGreen));
			startIndex = startExpression.match(text, startIndex + commentLength)
					.capturedStart();
		}
	}
}

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
	return QVariant();
}

void CppOutlineModel::updateTranslationUnit(CppDocument *document, CXTranslationUnit unit)
{
	CXFile file = clang_getFile(unit, document->filename().toLocal8Bit().constData());
	CXSourceRange range = clang_getRange(clang_getLocationForOffset(unit, file, 0), clang_getLocationForOffset(unit, file, QFileInfo(document->filename()).size()));
	CXToken *tokens;
	unsigned int numTokens;
	clang_tokenize(unit, range, &tokens, &numTokens);
	CXCursor cursors[numTokens];
	clang_annotateTokens(unit, tokens, numTokens, cursors);

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

	clang_disposeTokens(unit, tokens, numTokens);
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

void CppManager::registerDocument(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter)
{
	m_documents.insert(document, new Unit(document, outline, highlighter, this));
}
void CppManager::unregisterDocument(CppDocument *document)
{
	m_documents.remove(document);
}

QStringList CppManager::createFlags(ProjectCodeModel *codeModel)
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

Unit::Unit(CppDocument *document, CppOutlineModel *outline, CppSyntaxHighlighter *highlighter, QObject *parent)
	: QObject(parent), m_document(document), m_outline(outline), m_highlighter(highlighter)
{
	m_index = clang_createIndex(0, 0);
	parseTU();
	connect(document->document(), &QTextDocument::contentsChanged, this, &Unit::parseTU);
}

CompletionResult Unit::complete(unsigned int line, unsigned int column) const
{
	const auto unsaved = DocumentModel::instance->getUnsavedDocuments();

	CXCodeCompleteResults *results = clang_codeCompleteAt(m_tu, m_document->filename().toLocal8Bit().constData(), line + 1, column + 1,
														 createUnsavedFiles(unsaved), unsaved.size(), clang_defaultCodeCompleteOptions());
	CompletionResult out;
	for (int i = 0; i < results->NumResults; ++i)
	{
		const auto string = results->Results[i].CompletionString;
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
		out.append(qMakePair(result, results->Results[i].CursorKind));
	}

	clang_disposeCodeCompleteResults(results);

	return out;
}

void Unit::parseTU()
{
	auto args = CppManager::createFlags(m_document->project()->codeModel());
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
	m_outline->updateTranslationUnit(m_document, m_tu);
}

CXUnsavedFile *Unit::createUnsavedFiles(const QList<BaseDocument *> &docs) const
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
