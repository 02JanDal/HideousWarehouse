#pragma once

#include <QSyntaxHighlighter>
#include <QPair>
#include <QRegularExpression>

class CppSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit CppSyntaxHighlighter(QTextDocument *doc);

private:
	void highlightBlock(const QString &text) override;

	QList<QPair<QRegularExpression, QTextCharFormat>> m_formats;
};
