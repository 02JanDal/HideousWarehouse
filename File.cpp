#include "File.h"

File::File(const QString &filename)
	: m_file(new QFile(filename))
{
}
File::File()
	: m_file(new QFile())
{
}

File::~File()
{
	delete m_file;
}

QByteArray File::openAndRead(const QString &filename)
{
	File file(filename);
	file.open(QFile::ReadOnly);
	return file.read();
}

void File::setFilename(const QString &filename)
{
	m_file->setFileName(filename);
}
QString File::filename() const
{
	return m_file->fileName();
}

void File::open(const QIODevice::OpenMode &mode)
{
	if (!m_file->open(mode))
	{
		throw Exception(m_file->errorString());
	}
}

void File::close()
{
	m_file->close();
}
void File::seek(const qint64 index)
{
	m_file->seek(index);
}
QByteArray File::read(const qint64 maxlength) const
{
	return m_file->read(maxlength == -1 ? m_file->size() : maxlength);
}
void File::write(const QByteArray &data)
{
	if (m_file->write(data) != data.size())
	{
		throw Exception(m_file->errorString());
	}
}
