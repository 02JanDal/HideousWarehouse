#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QRegularExpression>
#include <QSettings>
#include <QCompleter>

#include "core/PluginManager.h"
#include "core/GitIgnoreFilterModel.h"
#include "core/DocumentModel.h"

MainWindow::MainWindow(BaseProject *project, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_project(project)
{
	ui->setupUi(this);

	m_fsModel = new QFileSystemModel(this);
	GitIgnoreFilterModel *filter = new GitIgnoreFilterModel(this);
	filter->setSourceModel(m_fsModel);
	ui->fileSystemView->setModel(filter);
	const QModelIndex rootIndex = m_fsModel->setRootPath(m_project->projectRoot().absolutePath());
	ui->fileSystemView->setRootIndex(filter->mapFromSource(rootIndex));
	ui->fileSystemView->header()->hideSection(1);
	ui->fileSystemView->header()->hideSection(2);
	ui->fileSystemView->header()->hideSection(3);
	if (project->projectRoot().exists(".gitignore"))
	{
		filter->setGitIgnoreFile(project->projectRoot().absoluteFilePath(".gitignore"));
	}
	connect(ui->fileSystemView, &QTreeView::doubleClicked, [this](const QModelIndex &index)
	{
		const QString filename = index.data(QFileSystemModel::FilePathRole).toString();
		if (QFileInfo(filename).isDir())
		{
			return;
		}
		setCurrentDocument(m_docModel->open(filename));
	});

	m_docModel = new DocumentModel(m_project, this);
	ui->openDocumentsView->setModel(m_docModel);
	connect(ui->openDocumentsView, &QListView::clicked, [this](const QModelIndex &index)
	{
		setCurrentDocument(m_docModel->at(index));
	});

	connect(ui->editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateCompletionModel, Qt::QueuedConnection);

	QSettings settings;
	restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
	restoreState(settings.value("MainWindow/State").toByteArray());
	for (auto dockWidget : {ui->documentsDockWidget, ui->consoleDockWidget, ui->outlineDockWidget})
	{
		restoreDockWidget(dockWidget);
	}
	if (!settings.value("MainWindow/CurrentDocument").toString().isEmpty())
	{
		setCurrentDocument(m_docModel->open(settings.value("MainWindow/CurrentDocument").toString()));
	}
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("MainWindow/State", saveState());
	settings.setValue("MainWindow/Geometry", saveGeometry());
	settings.setValue("MainWindow/CurrentDocument", m_currentDocument ? m_currentDocument->filename() : QString());
	qDebug() << "Saving settings...";
	delete ui;
}

void MainWindow::on_action_Change_Project_triggered()
{
	// TODO: ensure documents are saved
	hide();
	deleteLater();
	QMetaObject::invokeMethod(this, "changeProject", Qt::QueuedConnection);
}
void MainWindow::on_action_Properties_triggered()
{

}
void MainWindow::on_action_Settings_triggered()
{

}
void MainWindow::on_action_Quit_triggered()
{
	qApp->quit();
}
void MainWindow::on_action_Undo_triggered()
{

}
void MainWindow::on_action_Redo_triggered()
{

}
void MainWindow::on_action_Cut_triggered()
{

}
void MainWindow::on_action_Copy_triggered()
{

}
void MainWindow::on_action_Paste_triggered()
{

}
void MainWindow::on_action_About_triggered()
{

}
void MainWindow::on_actionAbout_Qt_triggered()
{

}

void MainWindow::setCurrentDocument(BaseDocument *document)
{
	if (m_currentDocument)
	{
	}
	if (document)
	{
		ui->editor->completer()->setModel(document->completionModel());
		ui->openDocumentsView->selectionModel()->clearSelection();
		ui->openDocumentsView->selectionModel()->setCurrentIndex(m_docModel->index(document), QItemSelectionModel::SelectCurrent);
		ui->outlineView->setModel(document->outlineModel());
	}
	else
	{
		ui->editor->completer()->setModel(nullptr);
		ui->openDocumentsView->selectionModel()->clear();
		ui->outlineView->setModel(nullptr);
	}
	ui->editor->setDocument(document->document());
	m_currentDocument = document;
}

void MainWindow::updateCompletionModel()
{
	QTextCursor cursor = ui->editor->textCursor();
	cursor.movePosition(QTextCursor::Left);
	cursor.movePosition(QTextCursor::StartOfWord);
	m_currentDocument->complete(cursor.blockNumber(), cursor.positionInBlock());
}

#include "MainWindow.moc"
