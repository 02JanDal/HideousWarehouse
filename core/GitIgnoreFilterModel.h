#pragma once

#include <QSortFilterProxyModel>
#include <QRegularExpression>

class GitIgnoreFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit GitIgnoreFilterModel(QObject *parent = nullptr);

	void setGitIgnoreFile(const QString &file);

private:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

	QList<QRegularExpression> m_excludes;
};
