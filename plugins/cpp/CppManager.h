#pragma once

#include <QObject>

class CppManager : public QObject
{
	Q_OBJECT
public:
	static CppManager *instance()
	{
		static CppManager man;
		return &man;
	}

private:
	explicit CppManager()
		: QObject()
	{
	}
};
