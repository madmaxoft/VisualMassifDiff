// MainWindow.cpp

// Implements the MainWindow class representing the main app window





#include "Globals.h"
#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include "ui_MainWindow.h"
#include "MassifParser.h"
#include "Project.h"
#include "Snapshot.h"
#include "DlgSnapshotDetails.h"
#include "FormatNumber.h"
#include "SnapshotDiff.h"
#include "DlgSnapshotDiffs.h"
#include "CodeLocationStatsModel.h"
#include "HistoryModel.h"
#include "HistoryModelHierarchyDelegate.h"
#include "ProjectLoader.h"
#include "ProjectSaver.h"





/** Role used as a storage for the snapshot data in the tree widget. */
const int TW_ITEM_DATAROLE_SNAPSHOT_TIMESTAMP = 1001;





MainWindow::MainWindow(QWidget * parent):
	QMainWindow(parent),
	m_UI(new Ui::MainWindow),
	m_IcoAllocations(":/res/icoAllocations.png")
{
	m_UI->setupUi(this);

	// Add a context menu to twSnapshots:
	m_UI->twSnapshots->addAction(m_UI->actCtxDiffSelected);
	m_UI->twSnapshots->addAction(m_UI->actCtxDiffAll);

	// Create a new empty project:
	setProject(std::make_shared<Project>());

	// Set up the History view delegates:
	m_UI->tvHistory->setItemDelegateForColumn(5, new HistoryModelHierarchyDelegate(this));
	// m_UI->tvHistory->setItemDelegateForColumn(6, new HistoryModelPositionDelegate(this));

	// Connect the UI signals / slots:
	connect(m_UI->actProjectOpen,     SIGNAL(triggered()),                               this, SLOT(loadProject()));
	connect(m_UI->actProjectSave,     SIGNAL(triggered()),                               this, SLOT(saveProject()));
	connect(m_UI->actProjectSaveAs,   SIGNAL(triggered()),                               this, SLOT(saveProjectAs()));
	connect(m_UI->actSnapshotsAdd,    SIGNAL(triggered()),                               this, SLOT(addSnapshotsFromFile()));
	connect(m_UI->twSnapshots,        SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(twItemDblClicked(QTreeWidgetItem *, int)));
	connect(m_UI->twSnapshots,        SIGNAL(itemSelectionChanged()),                    this, SLOT(twItemSelChanged()));
	connect(m_UI->actCtxDiffSelected, SIGNAL(triggered()),                               this, SLOT(diffSelected()));
	connect(m_UI->actCtxDiffAll,      SIGNAL(triggered()),                               this, SLOT(diffAll()));
}





MainWindow::~MainWindow()
{
	delete m_UI;
}





void MainWindow::addSnapshotsFromFile(void)
{
	// Get the filename(s):
	auto fileNames = QFileDialog::getOpenFileNames(this);
	for (const auto & fileName: fileNames)
	{
		addSnapshotsFromFile(fileName);
	}
}





void MainWindow::addSnapshotsFromFile(const QString & a_FileName)
{
	// Open the file:
	QFile file(a_FileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,
			tr("File error"),
			tr("Failed to open\n%1").arg(a_FileName)
		);
		return;
	}

	// Parse the file:
	// TODO: Move this to a background thread (?)
	MassifParser parser(m_Project);
	connect(&parser, SIGNAL(parseError(quint32, const char *, const char *)), this, SLOT(parseError(quint32, const char *, const char *)));
	connect(&parser, SIGNAL(newSnapshotParsed(SnapshotPtr)),                  this, SLOT(newSnapshotParsed(SnapshotPtr)));
	connect(&parser, SIGNAL(parsedCommand(const char *)),                     this, SLOT(parsedCommand(const char *)));
	connect(&parser, SIGNAL(parsedTimeUnit(const char *)),                    this, SLOT(parsedTimeUnit(const char *)));
	parser.parse(file);
}





void MainWindow::parseError(quint32 a_LineNum, const char * a_Msg, const char * a_Line)
{
	QMessageBox::warning(this,
		tr("Parse error"),
		tr("Error while parsing line %1:\n\n%2\n\nThe offending line is:\n%3")
			.arg(a_LineNum)
			.arg(a_Msg)
			.arg(a_Line)
	);
}





void MainWindow::newSnapshotParsed(SnapshotPtr a_Snapshot)
{
	assert(m_Project != nullptr);

	// If the project already contains a snapshot with this timestamp, skip:
	if (m_Project->hasSnapshotForTimestamp(a_Snapshot->getTimestamp()))
	{
		return;
	}

	// Add snapshot to project:
	m_Project->addSnapshot(a_Snapshot);

	// Add snapshot to list:
	auto item = createSnapshotTreeItem(a_Snapshot);
	m_UI->twSnapshots->addTopLevelItem(item);
	m_CodeLocationStatsModel->addedSnapshot();
}






void MainWindow::parsedCommand(const char * a_Command)
{
	assert(m_Project != nullptr);
	if (!m_Project->checkAndSetCommand(a_Command))
	{
		reinterpret_cast<MassifParser *>(QObject::sender())->abortParsing();
		QMessageBox::warning(this,
			tr("Bad commandline"),
			tr("The commandline in the specified file is different, it is likely not created from the same Massif run.\n\n" \
				"This project uses data from commandline \"%1\", but the file specified the commandline \"%2\"")
				.arg(m_Project->getCommand().c_str())
				.arg(a_Command)
		);
	}
}





void MainWindow::parsedTimeUnit(const char * a_TimeUnit)
{
	assert(m_Project != nullptr);
	if (!m_Project->checkAndSetTimeUnit(a_TimeUnit))
	{
		reinterpret_cast<MassifParser *>(QObject::sender())->abortParsing();
		QMessageBox::warning(this,
			tr("Bad time unit"),
			tr("The time unit in the specified file is different, it is likely not created from the same Massif run.\n\n" \
				"This project uses time units \"%1\", but the file specified the time unit \"%2\"")
				.arg(m_Project->getTimeUnit().c_str())
				.arg(a_TimeUnit)
		);
	}
}





void MainWindow::twItemDblClicked(QTreeWidgetItem * a_Item, int a_Column)
{
	a_Column;

	auto timestamp = a_Item->data(0, TW_ITEM_DATAROLE_SNAPSHOT_TIMESTAMP);
	auto snapshot = m_Project->getSnapshotAtTimestamp(timestamp.toULongLong());
	if (snapshot != nullptr)
	{
		viewSnapshotDetails(snapshot);
	}
}





void MainWindow::twItemSelChanged()
{
	m_UI->actCtxDiffSelected->setEnabled(m_UI->twSnapshots->selectedItems().size() > 1);
}





void MainWindow::viewSnapshotDetails(SnapshotPtr a_Snapshot)
{
	auto dlg = new DlgSnapshotDetails;
	dlg->show(a_Snapshot);
}





void MainWindow::diffSelected()
{
	// Collect all selected snapshots:
	SnapshotPtrs snapshots;
	// foreach(item, m_UI->twSnapshots->selectedItems())
	for (auto item: m_UI->twSnapshots->selectedItems())
	{
		auto timestamp = item->data(0, TW_ITEM_DATAROLE_SNAPSHOT_TIMESTAMP);
		auto snapshot = m_Project->getSnapshotAtTimestamp(timestamp.toULongLong());
		snapshots.push_back(snapshot);
	}

	showDiffsForSnapshots(snapshots);
}





void MainWindow::diffAll()
{
	showDiffsForSnapshots(m_Project->getSnapshots());
}





void MainWindow::loadProject()
{
	auto fileName = QFileDialog::getOpenFileName(
		nullptr,                                      // Parent widget
		tr("Open a project file"),                    // Title
		QString(),                                    // Initial folder
		tr("VisualMassifDiff project file (*.vmdp)")  // Filter
	);
	if (fileName.isEmpty())
	{
		return;
	}
	emit loadProject(fileName);
}





void MainWindow::loadProject(const QString & a_FileName)
{
	// If current project is modified, ask to save:

	if (m_Project->hasChangedSinceSave())
	{
	}

	// Load the project from the file:
	QFile f(a_FileName);
	if (!f.open(QFile::ReadOnly))
	{
		QMessageBox::warning(this,
			tr("File error"),
			tr("Failed to open project file\n%1").arg(a_FileName)
		);
		return;
	}
	ProjectPtr project;
	try
	{
		project = ProjectLoader::loadProject(f);
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(this,
			tr("File error"),
			tr("Failed to load project from file\n%1\n\n%2").arg(a_FileName).arg(QString::fromUtf8(exc.what()))
		);
		return;
	}
	if (project == nullptr)
	{
		QMessageBox::warning(this,
			tr("File error"),
			tr("Failed to load project from file\n%1").arg(a_FileName)
		);
		return;
	}

	// Replace the currently loaded project:
	setProject(project);
}





bool MainWindow::saveProject()
{
	if (m_Project->getFileName().isEmpty())
	{
		return saveProjectAs();
	}
	return saveProject(m_Project->getFileName());
}





bool MainWindow::saveProject(const QString & a_FileName)
{
	// Open the file for writing:
	QFile f(a_FileName);
	if (!f.open(QFile::WriteOnly))
	{
		QMessageBox::warning(
			this,
			tr("VisualMassifDiff: Failed to save project"),
			tr("Failed to write to file %1").arg(a_FileName)
		);
		return false;
	}

	// Save the project:
	try
	{
		ProjectSaver::saveProject(*m_Project, f);
		m_Project->setSaved(a_FileName);
	}
	catch (const std::exception & exc)
	{
		QMessageBox::warning(
			this,
			tr("VisualMassifDiff: Failed to save project"),
			tr("Failed to save project: %1").arg(exc.what())
		);
		return false;
	}
	return true;
}





bool MainWindow::saveProjectAs()
{
	auto fileName = QFileDialog::getSaveFileName(
		nullptr,                                      // Parent widget
		tr("Save the project file"),                  // Title
		QString(),                                    // Initial folder
		tr("VisualMassifDiff project file (*.vmdp)")  // Filter
	);
	if (fileName.isEmpty())
	{
		return false;
	}
	return saveProject(fileName);
}





QTreeWidgetItem * MainWindow::createSnapshotTreeItem(SnapshotPtr a_Snapshot)
{
	QStringList columns;
	columns << tr("%1").arg(a_Snapshot->getTimestamp());
	columns << formatMemorySize(a_Snapshot->getHeapSize());
	columns << formatMemorySize(a_Snapshot->getHeapExtraSize());
	auto res = new QTreeWidgetItem(columns);
	res->setData(0, TW_ITEM_DATAROLE_SNAPSHOT_TIMESTAMP, a_Snapshot->getTimestamp());
	res->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
	res->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
	res->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	if (a_Snapshot->hasAllocations())
	{
		res->setIcon(0, m_IcoAllocations);
	}
	return res;
}





void MainWindow::showDiffsForSnapshots(const SnapshotPtrs & a_Snapshots)
{
	// Sort the snapshots by their timestamp:
	SnapshotPtrs snapshots(a_Snapshots);
	snapshots.sort([](SnapshotPtr a_First, SnapshotPtr a_Second)
		{
			return (a_First->getTimestamp() < a_Second->getTimestamp());
		}
	);

	// Create the diffs:
	SnapshotDiffPtrs diffs;
	SnapshotPtr prevSnapshot;
	for (auto s: snapshots)
	{
		if (prevSnapshot != nullptr)
		{
			auto diff = std::make_shared<SnapshotDiff>(prevSnapshot, s);
			diffs.push_back(diff);
		}
		prevSnapshot = s;
	}

	// Show the diffs:
	auto dlg = new DlgSnapshotDiffs(this);
	dlg->show(std::move(diffs));
}





bool MainWindow::prepareCurrentProjectForUnload()
{
	// If not changed, doesn't need any confirmation
	if (!m_Project->hasChangedSinceSave())
	{
		return true;
	}

	// Ask the user what to do:
	auto answer = QMessageBox::question(
		this,                                                                                    // Parent widget
		tr("VisualMassifDiff: Save project?"),                                                   // Title
		tr("Current project has not been saved, would you like to save it now?"),                // Text
		QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel),  // Buttons
		QMessageBox::Yes                                                                         // Default button
	);
	switch (answer)
	{
		case QMessageBox::No:
		{
			// User wants to throw away the current project's changes
			return true;
		}
		case QMessageBox::Yes:
		{
			return saveProject();
		}
		case QMessageBox::Cancel:
		default:
		{
			// User wants to abort any operation and keep the current project as is, unsaved:
			return false;
		}
	}
}





void MainWindow::setProject(ProjectPtr a_Project)
{
	// Replace the CodeLocationStats model:
	auto codeLocationStatsModel = std::make_shared<CodeLocationStatsModel>(a_Project);
	auto codeLocationStatsSortModel = std::make_shared<QSortFilterProxyModel>();
	codeLocationStatsSortModel->setSourceModel(codeLocationStatsModel.get());
	codeLocationStatsSortModel->setSortRole(CodeLocationStatsModel::SortRole);
	m_UI->tvCodeLocations->setModel(codeLocationStatsSortModel.get());
	m_CodeLocationStatsModel = codeLocationStatsModel;
	m_CodeLocationStatsSortModel = codeLocationStatsSortModel;

	// Replace the History model:
	auto historyModel = std::make_shared<HistoryModel>(a_Project);
	m_UI->tvHistory->setModel(historyModel.get());
	m_UI->grHistory->setProject(
		a_Project,
		reinterpret_cast<HistoryModel *>(historyModel.get()),
		m_UI->tvHistory->selectionModel()
	);
	m_HistoryModel = historyModel;

	// Replace the current project (and free the old one):
	m_Project = a_Project;

	// Insert the snapshots into twSnapshots:
	m_UI->twSnapshots->clear();
	for (const auto & s: m_Project->getSnapshots())
	{
		auto item = createSnapshotTreeItem(s);
		m_UI->twSnapshots->addTopLevelItem(item);
	}
}




