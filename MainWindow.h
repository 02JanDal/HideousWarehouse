#pragma once

#include <QMainWindow>

#include "Project.h"
#include "Document.h"

namespace Ui {
class MainWindow;
}

class QFileSystemModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(BaseProject *project, QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_action_Change_Project_triggered();
	void on_action_Properties_triggered();
	void on_action_Settings_triggered();
	void on_action_Quit_triggered();
	void on_action_Undo_triggered();
	void on_action_Redo_triggered();
	void on_action_Cut_triggered();
	void on_action_Copy_triggered();
	void on_action_Paste_triggered();
	void on_action_About_triggered();
	void on_actionAbout_Qt_triggered();

	void setCurrentDocument(BaseDocument *document);

private:
	Ui::MainWindow *ui;
	BaseProject *m_project;
	QFileSystemModel *m_fsModel;
	DocumentModel *m_docModel;

signals:
	void changeProject();
};
