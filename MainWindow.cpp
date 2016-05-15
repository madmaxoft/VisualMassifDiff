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
#include "LiveCaptureSettings.h"
#include "DlgLiveCaptureSettings.h"
#include "VgdbComm.h"
#include "LiveCapture.h"
#include "DlgLiveCapture.h"
#include "SnapshotModel.h"





MainWindow::MainWindow(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::MainWindow)
{
	m_UI->setupUi(this);

	// Add a context menu to twSnapshots:
	m_UI->tvSnapshots->addAction(m_UI->actCtxDiffSelected);
	m_UI->tvSnapshots->addAction(m_UI->actCtxDiffAll);

	// Create a new empty project:
	setProject(std::make_shared<Project>());
	m_UI->tvSnapshots->sortByColumn(0, Qt::AscendingOrder);  // Qt seems to default to Descending on Win8

	// Set up the History view delegates:
	m_UI->tvHistory->setItemDelegateForColumn(5, new HistoryModelHierarchyDelegate(this));
	// m_UI->tvHistory->setItemDelegateForColumn(6, new HistoryModelPositionDelegate(this));

	// Connect the UI signals / slots:
	connect(m_UI->actProjectNew,           SIGNAL(triggered()),                        this, SLOT(newProject()));
	connect(m_UI->actProjectOpen,          SIGNAL(triggered()),                        this, SLOT(loadProject()));
	connect(m_UI->actProjectSave,          SIGNAL(triggered()),                        this, SLOT(saveProject()));
	connect(m_UI->actProjectSaveAs,        SIGNAL(triggered()),                        this, SLOT(saveProjectAs()));
	connect(m_UI->actSnapshotsAdd,         SIGNAL(triggered()),                        this, SLOT(addSnapshotsFromFile()));
	connect(m_UI->actSnapshotsLiveCapture, SIGNAL(triggered()),                        this, SLOT(snapshotsLiveCapture()));
	connect(m_UI->tvSnapshots,             SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(tvItemDblClicked(const QModelIndex &)));
	connect(m_UI->actCtxDiffSelected,      SIGNAL(triggered()),                        this, SLOT(diffSelected()));
	connect(m_UI->actCtxDiffAll,           SIGNAL(triggered()),                        this, SLOT(diffAll()));
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





void MainWindow::snapshotsLiveCapture()
{
	// Check that vgdb is available:
	if (!VgdbComm::checkAvailability())
	{
		QMessageBox::warning(this,
			tr("Live Capture not available"),
			tr("Cannot start live capture, because Valgrind's \"vgdb\" program is not available.")
		);
		return;
	}

	// Query all the running valgrind instances:
	auto runningInstances = VgdbComm::listRunningInstances();

	// Ask for the settings:
	LiveCaptureSettings settings;
	QFileInfo projFile(m_Project->getFileName());
	settings.m_SnapshotFolder = projFile.absolutePath();
	settings.m_SnapshotFileNameFormat = projFile.completeBaseName() + QString::fromUtf8("-%1.massif");
	DlgLiveCaptureSettings dlgSettings;
	if (!dlgSettings.show(settings, runningInstances))
	{
		return;
	}

	// If we are to save the project and it hasn't been saved yet, ask for filename:
	if (settings.m_ShouldSaveProject)
	{
		if (m_Project->getFileName().isEmpty())
		{
			if (!saveProjectAs())
			{
				return;
			}
		}
	}

	// Start live capture:
	LiveCapture capture(m_Project, settings);
	DlgLiveCapture dlgCapture;
	dlgCapture.show(capture, settings);
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





void MainWindow::tvItemDblClicked(const QModelIndex & a_Item)
{
	auto snapshot = m_ProjectSnapshotsModel->getItemSnapshot(a_Item);
	if (snapshot != nullptr)
	{
		viewSnapshotDetails(snapshot);
	}
}





void MainWindow::tvItemSelChanged(const QItemSelection & a_Selected, const QItemSelection & a_Deselected)
{
	Q_UNUSED(a_Selected);
	Q_UNUSED(a_Deselected);

	m_UI->actCtxDiffSelected->setEnabled(m_UI->tvSnapshots->selectionModel()->selectedRows().size() > 1);
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
	for (const auto row: m_UI->tvSnapshots->selectionModel()->selectedRows())
	{
		auto snapshot = m_ProjectSnapshotsModel->getItemSnapshot(row);
		assert(snapshot != nullptr);
		snapshots.push_back(snapshot);
	}

	showDiffsForSnapshots(snapshots);
}





void MainWindow::diffAll()
{
	showDiffsForSnapshots(m_Project->getSnapshots());
}





void MainWindow::newProject()
{
	// Ask the user whether to save current project (if appropriate):
	if (!prepareCurrentProjectForUnload())
	{
		return;
	}

	// Create and set a new project:
	setProject(std::make_shared<Project>());
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
	loadProject(fileName);
}





void MainWindow::loadProject(const QString & a_FileName)
{
	// Ask the user whether to save current project (if appropriate):
	if (!prepareCurrentProjectForUnload())
	{
		return;
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

	// Mark the project as non-dirty:
	project->setSaved(a_FileName);

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
			tr("Failed to save project to file %1: %2").arg(a_FileName).arg(exc.what())
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





bool MainWindow::openUnknownFile(const QString & a_FileName)
{
	// Check whether the file is a project file:
	QFile f(a_FileName);
	if (!f.open(QFile::ReadOnly))
	{
		return false;
	}
	bool isProject = ProjectLoader::isProjectFile(f);
	f.close();

	// If the file is a project, load it:
	if (isProject)
	{
		loadProject(a_FileName);
		return true;
	}

	// It is not a project, assume it's a snapshot file:
	addSnapshotsFromFile(a_FileName);
	return true;
}





void MainWindow::showDiffsForSnapshots(const SnapshotPtrs & a_Snapshots)
{
	// Sort the snapshots by their timestamp:
	SnapshotPtrs snapshots(a_Snapshots);
	snapshots.sort([](SnapshotPtr a_First, SnapshotPtr a_Second)
		{
			assert(a_First != nullptr);
			assert(a_Second != nullptr);
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
	// Replace the ProjectSnapshots model:
	auto projectSnapshotsModel = std::make_shared<SnapshotModel>(a_Project);
	m_UI->tvSnapshots->setModel(projectSnapshotsModel.get());
	m_ProjectSnapshotsModel = projectSnapshotsModel;
	auto selectionModel = m_UI->tvSnapshots->selectionModel();
	connect(
		selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this,           SLOT(tvItemSelChanged(const QItemSelection &, const QItemSelection &))
	);

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
}




