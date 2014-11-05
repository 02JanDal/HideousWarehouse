#include <QApplication>
#include <QInputDialog>
#include <functional>

#include "MainWindow.h"
#include "Project.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("CEdit");
	app.setOrganizationName("JanDalheimer");

	MainWindow *current;

	std::function<bool()> lambda;lambda = [&current, lambda]()
	{
		bool ok = false;
		const QString dir = QInputDialog::getItem(nullptr, QObject::tr("Choose Project"), QObject::tr("Please input the full path of the project you want to open"),
												  QStringList() << "/home/jan/projects/MultiMC5/CMakeLists.txt", -1, true, &ok);
		if (!ok)
		{
			qApp->quit();
			return false;
		}
		if (dir.isEmpty())
		{
			return lambda();
		}
		current = new MainWindow(BaseProject::createProject(QFileInfo(dir)));
		current->show();
		QObject::connect(current, &MainWindow::changeProject, lambda);
		return true;
	};


	if (!lambda())
	{
		return 0;
	}

	int ret = app.exec();
	if (current) delete current;
	return ret;
}
