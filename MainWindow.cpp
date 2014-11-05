#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QRegularExpression>
#include <QSettings>

#include "File.h"

class GitIgnoreFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit GitIgnoreFilterModel(QObject *parent = nullptr)
		: QSortFilterProxyModel(parent) {}

	void setGitIgnoreFile(const QString &file)
	{
		const QList<QByteArray> lines = File::openAndRead(file).split('\n');
		for (const QByteArray line : lines)
		{
			if (line.startsWith('#') || line.isEmpty())
			{
				continue;
			}
			m_excludes.append(QRegularExpression("^" + QFileInfo(file).dir().absoluteFilePath(QString::fromLocal8Bit(line)).replace(".", "\.").replace("*", ".*")));
		}
		sort(0);
	}

private:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
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
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override
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

	QList<QRegularExpression> m_excludes;
};

MainWindow::MainWindow(BaseProject *project, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_project(project)
{
	ui->setupUi(this);

	QSettings settings;
	restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
	restoreState(settings.value("MainWindow/State").toByteArray());
	for (auto dockWidget : {ui->documentsDockWidget, ui->consoleDockWidget, ui->outlineDockWidget})
	{
		restoreDockWidget(dockWidget);
	}

	m_fsModel = new QFileSystemModel(this);
	GitIgnoreFilterModel *filter = new GitIgnoreFilterModel(this);
	filter->setSourceModel(m_fsModel);
	ui->fileSystemView->setModel(filter);
	ui->fileSystemView->setRootIndex(filter->mapFromSource(m_fsModel->setRootPath(m_project->projectRoot().absolutePath())));
	ui->fileSystemView->header()->hideSection(1);
	ui->fileSystemView->header()->hideSection(2);
	ui->fileSystemView->header()->hideSection(3);
	if (project->projectRoot().exists(".gitignore"))
	{
		filter->setGitIgnoreFile(project->projectRoot().absoluteFilePath(".gitignore"));
	}
	connect(ui->fileSystemView, &QTreeView::doubleClicked, [this](const QModelIndex &index)
	{
		setCurrentDocument(m_docModel->open(index.data(QFileSystemModel::FilePathRole).toString()));
	});

	m_docModel = new DocumentModel(m_project, this);
	ui->openDocumentsView->setModel(m_docModel);
	connect(ui->openDocumentsView, &QListView::clicked, [this](const QModelIndex &index)
	{
		setCurrentDocument(m_docModel->at(index));
	});
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("MainWindow/State", saveState());
	settings.setValue("MainWindow/Geometry", saveGeometry());
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
	if (document)
	{
		ui->openDocumentsView->selectionModel()->clearSelection();
		ui->openDocumentsView->selectionModel()->setCurrentIndex(m_docModel->index(document), QItemSelectionModel::SelectCurrent);
	}
	else
	{
		ui->openDocumentsView->selectionModel()->clear();
	}
	ui->editor->setDocument(document->document());
}

#include "MainWindow.moc"
