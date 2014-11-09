#include "GitIgnoreFilterModel.h"

#include <QFileInfo>
#include <QFileSystemModel>

#include "util/File.h"

GitIgnoreFilterModel::GitIgnoreFilterModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

void GitIgnoreFilterModel::setGitIgnoreFile(const QString &file)
{
	const QList<QByteArray> lines = File::openAndRead(file).split('\n');
	for (const QByteArray line : lines)
	{
		if (line.startsWith('#') || line.isEmpty())
		{
			continue;
		}
		m_excludes.append(QRegularExpression("^" + QFileInfo(file).dir().absoluteFilePath(QString::fromLocal8Bit(line)).replace(".", "\\.").replace("*", ".*")));
	}
	sort(0);
}

bool GitIgnoreFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	const QString path = source_parent.child(source_row, 0).data(QFileSystemModel::FilePathRole).toString();
	for (const auto exclude : m_excludes)
	{
		if (exclude.match(path).hasMatch())
		{
			return false;
		}
	}
	return true;
}

bool GitIgnoreFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	const bool leftDir = QFileInfo(left.data(QFileSystemModel::FilePathRole).toString()).isDir();
	const bool rightDir = QFileInfo(right.data(QFileSystemModel::FilePathRole).toString()).isDir();
	if (leftDir == rightDir)
	{
		return QSortFilterProxyModel::lessThan(left, right);
	}
	else if (leftDir)
	{
		return true;
	}
	else
	{
		return false;
	}
}

