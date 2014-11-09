#pragma once

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
	BaseException(const QString &error) : std::exception(), m_what(error)
	{
		qDebug("%s", what());
	}
	BaseException(const QByteArray &error) : BaseException(QString::fromLocal8Bit(error)) {}
	BaseException(const char *error) : BaseException(QString::fromLocal8Bit(error)) {}
	virtual ~BaseException() noexcept {}
	char *what() noexcept { return m_what.toLocal8Bit().data(); }

private:
	QString m_what;
};

#define EXCEPTION_FOR_THIS_CLASS typedef ::BaseException Exception;
