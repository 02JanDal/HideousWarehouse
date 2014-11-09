#pragma once

#include <QObject>
#include <QDir>

class ProjectCodeModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QList<QDir> includeDirectories MEMBER includeDirectories CONSTANT)
	Q_PROPERTY(QList<QString> definitions MEMBER definitions CONSTANT)
	Q_PROPERTY(QList<QString> otherFlags MEMBER otherFlags CONSTANT)
	Q_PROPERTY(QList<QString> files MEMBER files CONSTANT)
public:
	explicit ProjectCodeModel(const QList<QDir> &includeDirectories,
							  const QList<QString> &definitions,
							  const QList<QString> &otherFlags,
							  const QList<QString> &files,
							  QObject *parent = nullptr);

	const QList<QDir> includeDirectories;
	const QList<QString> definitions;
	const QList<QString> otherFlags;
	const QList<QString> files;
};

class BaseProject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QDir projectRoot READ projectRoot)
	Q_PROPERTY(Type type READ type)
	Q_PROPERTY(ProjectCodeModel codeModel READ codeModel NOTIFY codeModelChanged)
public:
	enum Type
	{
		Plain,
		CMake
	};

	explicit BaseProject(const QDir &projectRoot, const Type type, QObject *parent = nullptr);

	QDir projectRoot() const { return m_projectRoot; }
	Type type() const { return m_type; }
	ProjectCodeModel *codeModel() const { return m_codeModel; }

signals:
	void codeModelChanged();

public slots:
	virtual void updateCodeModel() {}

protected:
	Type m_type;
	QDir m_projectRoot;
	ProjectCodeModel *m_codeModel = nullptr;
};
