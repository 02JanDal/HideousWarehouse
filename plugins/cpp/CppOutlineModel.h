#pragma once

#include <QAbstractItemModel>
#include <Index.h>

class CppUnit;

class CppOutlineModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit CppOutlineModel(QObject *parent = nullptr);

	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	struct Node
	{
		enum Type
		{
			Invalid,
			Function,
			Method = Function,
			Class,
			Struct = Class,
			Variable,
			Field = Variable
		} type = Invalid;

		QString name;
		Node *parent = nullptr;
		QList<Node *> children;

		int row() const
		{
			if (parent)
			{
				return parent->children.indexOf(const_cast<Node *>(this));
			}
			return 0;
		}
	};

public slots:
	void updateTranslationUnit(CppUnit *unit);

private:
	Node *m_root;

	static QString ensureExists(QMap<QString, Node *> &nodes, const CXCursor cursor);
};
