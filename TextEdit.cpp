#include "TextEdit.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QDebug>

TextEdit::TextEdit(QWidget *parent)
	: QPlainTextEdit(parent)
{
	m_completer = new QCompleter(this);
	m_completer->setWidget(this);
	m_completer->setCompletionMode(QCompleter::PopupCompletion);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	connect(m_completer, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated), this, &TextEdit::insertCompletion);
}

void TextEdit::insertCompletion(const QString &completion)
{
	QTextCursor cursor = textCursor();
	int extra = completion.length() - m_completer->completionPrefix().length();
	cursor.movePosition(QTextCursor::Left);
	cursor.movePosition(QTextCursor::EndOfWord);
	cursor.insertText(completion.right(extra));
	setTextCursor(cursor);
}

QString TextEdit::textUnderCursor() const
{
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::WordUnderCursor);
	return cursor.selectedText();
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
	m_completer->setWidget(this);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
	if (m_completer->popup()->isVisible())
	{
		switch (e->key())
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			e->ignore();
			return;
		default:
			break;
		}
	}

	const bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space;
	if (!isShortcut)
	{
		QPlainTextEdit::keyPressEvent(e);
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (ctrlOrShift && e->text().isEmpty() && !isShortcut)
	{
		return;
	}

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
	const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	const QString completionPrefix = textUnderCursor();
	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
	{
		m_completer->popup()->hide();
		return;
	}

	if (completionPrefix != m_completer->completionPrefix())
	{
		m_completer->setCompletionPrefix(completionPrefix);
		m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
	m_completer->complete(cr);
}
