#include "CppSyntaxHighlighter.h"

#include <QRegularExpression>

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

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument *doc)
	: QSyntaxHighlighter(doc)
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
