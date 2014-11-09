#pragma once

#include "core/BaseProject.h"

class CMakeProject : public BaseProject
{
	Q_OBJECT
public:
	explicit CMakeProject(const QDir &projectRoot, QObject *parent = nullptr);

private:
	void updateCodeModel() override;
};
