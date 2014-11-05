#pragma once

#include <QFile>
#include "BaseException.h"

/**
 * @brief The File class is a thin wrapper around QFile that throws exceptions if needed
 */
class File
{
public:
	File(const QString &filename);
	File();
	~File();

	EXCEPTION_FOR_THIS_CLASS

	static QByteArray openAndRead(const QString &filename);

	QFile *file() const { return m_file; }

	void setFilename(const QString &filename);
	QString filename() const;

	void open(const QFile::OpenMode &mode);
	void close();
	void seek(const qint64 index);
	QByteArray read(const qint64 maxlength = -1) const;
	void write(const QByteArray &data);

private:
	QFile *m_file;
};
