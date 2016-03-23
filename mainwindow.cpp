// mainwindow.cpp

// Implements the MainWindow class representing the main app window





#include "globals.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include "ui_mainwindow.h"
#include "massifparser.h"
#include "project.h"
#include "snapshot.h"
#include "dlgsnapshotdetails.h"
#include "formatnumber.h"





/** Role used as a storage for the snapshot data in the tree widget. */
const int TW_ITEM_DATAROLE_SNAPSHOT_TIMESTAMP = 1001;





MainWindow::MainWindow(QWidget * parent):
	QMainWindow(parent),
	m_UI(new Ui::MainWindow),
	m_IcoAllocations(":/res/icoAllocations.png")
{
	m_UI->setupUi(this);

	// Create a new empty project:
	m_Project = std::make_shared<Project>();
	m_UI->graph->setProject(m_Project);

	// Connect the UI signals / slots:
	connect(m_UI->actSnapshotsAdd, SIGNAL(triggered()),                               this, SLOT(addSnapshotsFromFile()));
	connect(m_UI->twSnapshots,     SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(twItemDblClicked(QTreeWidgetItem *, int)));
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
	MassifParser parser;
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
	m_UI->graph->projectChanged();
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





void MainWindow::viewSnapshotDetails(SnapshotPtr a_Snapshot)
{
	auto dlg = new DlgSnapshotDetails;
	dlg->show(a_Snapshot);
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




