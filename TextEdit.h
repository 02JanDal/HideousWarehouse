#pragma once

#include <QPlainTextEdit>

class QCompleter;

class TextEdit : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit TextEdit(QWidget *parent = nullptr);

	QCompleter *completer() const { return m_completer; }

protected:
	void keyPressEvent(QKeyEvent *e) override;
	void focusInEvent(QFocusEvent *e) override;

private slots:
	void insertCompletion(const QString &completion);

private:
	QString textUnderCursor() const;

private:
	QCompleter *m_completer;
};
