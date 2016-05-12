// LiveCapture.cpp

// Implements the LiveCapture class encapsulating the Live Capture session





#include "Globals.h"
#include "LiveCapture.h"
#include <QDir>
#include <QCoreApplication>
#include "VgdbComm.h"
#include "MassifParser.h"
#include "Project.h"
#include "ProjectSaver.h"





LiveCapture::LiveCapture(ProjectPtr a_Project, const LiveCaptureSettings & a_Settings):
	Super(nullptr),
	m_Project(a_Project),
	m_Settings(a_Settings)
{
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}





void LiveCapture::start()
{
	// Start the new process, if requested:
	if (m_Settings.m_ShouldCreateNewProcess)
	{
		m_Pid = VgdbComm::launchNewInstance(
			m_Settings.m_NewProcessExecutable,
			m_Settings.m_NewProcessParams,
			m_Settings.m_NewProcessStartFolder
		);
	}
	else
	{
		m_Pid = m_Settings.m_ExistingProcessID;
	}
	if (m_Pid <= 0)
	{
		emit error(tr("Cannot start the process"));
		return;
	}

	// If this was an existing process, capture immediately:
	if (!m_Settings.m_ShouldCreateNewProcess)
	{
		doCapture();
	}

	// Start a timer:
	m_SecondsTillCapture = m_Settings.m_CaptureIntervalSec;
	m_Timer.start(1000);
}





void LiveCapture::stop()
{
	m_Timer.stop();
	m_Pid = 0;
}





void LiveCapture::onTimer()
{
	// Update the timer:
	m_SecondsTillCapture -= 1;
	emit timerTick(m_SecondsTillCapture);

	// If it's time, run a capture:
	if (m_SecondsTillCapture == 0)
	{
		m_SecondsTillCapture = m_Settings.m_CaptureIntervalSec;
		doCapture();  // May call stop() in a handler
	}
}





void LiveCapture::onParserNewSnapshot(SnapshotPtr a_Snapshot)
{
	emit snapshotParsed(m_SnapshotFileName, a_Snapshot);
	m_Project->addSnapshot(a_Snapshot);
	emit snapshotAdded(m_SnapshotFileName, a_Snapshot);

	if (m_Settings.m_ShouldSaveProject)
	{
		saveProject();
	}
}





void LiveCapture::onParserTimeUnit(const char * a_TimeUnitString)
{
	const auto & timeUnit = m_Project->getTimeUnit();
	if (timeUnit.empty() || (timeUnit.compare(a_TimeUnitString) == 0))
	{
		// The time units are the same, continue processing
		return;
	}

	// The time units are different, abort parsing and report an error:
	static_cast<MassifParser *>(sender())->abortParsing();
	emit error(tr(
		"Cannot use snapshots from this capture, the valgrind instance has a different time unit."
		"Expected \"%1\", got \"%2\".")
		.arg(m_Project->getTimeUnit().c_str())
		.arg(a_TimeUnitString)
	);
}





void LiveCapture::onParserCommand(const char * a_CommandString)
{
	const auto & cmd = m_Project->getCommand();
	if (cmd.empty() || (cmd.compare(a_CommandString) == 0))
	{
		// The command used to launch the process is the same, continue processing
		return;
	}

	// The command is different, abort parsing and report an error:
	static_cast<MassifParser *>(sender())->abortParsing();
	emit error(tr(
		"Cannot use snapshots from this capture, the valgrind instance has a different commandline."
		"Expected \"%1\", got \"%2\".")
		.arg(m_Project->getCommand().c_str())
		.arg(a_CommandString)
						 );
}





void LiveCapture::onParserError(quint32 a_LineNum, const char * a_ErrorMessage, const char * a_Line)
{
	emit logEvent(tr(
		"Parser error on line %1: %2\nOffending line:\n%3\n"
		"Continuing the capture, but this snapshot will not be included.")
		.arg(a_LineNum)
		.arg(a_ErrorMessage)
		.arg(a_Line)
	);
}





void LiveCapture::doCapture()
{
	emit logEvent(tr("Capturing a new snapshot..."));

	// Choose a filename for the snapshot:
	if (m_Settings.m_ShouldSaveSnapshots)
	{
		m_SnapshotFileName = createSnapshotFileName();
	}
	else
	{
		static int counter = 0;
		m_SnapshotFileName = QString("%1/VisualMassifDiff-%2-capture-pid-%3-%4.tmp")
			.arg(QDir::tempPath())
			.arg(QCoreApplication::applicationPid())
			.arg(m_Pid)
			.arg(++counter);
	}

	// Perform the capture:
	QByteArray stdOut, stdErr;
	auto res = VgdbComm::captureSnapshot(m_Pid, m_SnapshotFileName, stdOut, stdErr);
	emit logEvent(tr("stdout: %1").arg(QString::fromUtf8(stdOut)));
	emit logEvent(tr("stderr: %1").arg(QString::fromUtf8(stdErr)));
	if (!res)
	{
		emit logEvent(tr("Failed to capture the snapshot"));
		return;
	}
	emit snapshotRecorded(m_SnapshotFileName);

	// Process the snapshot:
	processSnapshotFile();

	// If not saving the snapshots, remove the temp file:
	if (!m_Settings.m_ShouldSaveSnapshots)
	{
		QFile::remove(m_SnapshotFileName);
	}
}





void LiveCapture::processSnapshotFile()
{
	// Open the file:
	QFile f(m_SnapshotFileName);
	if (!f.open(QFile::ReadOnly))
	{
		emit error(QString::fromUtf8("Cannot open snapshot file %1 for reading").arg(m_SnapshotFileName));
		return;
	}

	// Parse the file:
	MassifParser parser(m_Project);
	connect(&parser, SIGNAL(newSnapshotParsed(SnapshotPtr)),                  this, SLOT(onParserNewSnapshot(SnapshotPtr)));
	connect(&parser, SIGNAL(parsedTimeUnit(const char *)),                    this, SLOT(onParserTimeUnit(const char *)));
	connect(&parser, SIGNAL(parsedCommand(const char *)),                     this, SLOT(onParserCommand(const char *)));
	connect(&parser, SIGNAL(parseError(quint32, const char *, const char *)), this, SLOT(onParserError(quint32, const char *, const char *)));
	parser.parse(f);
}





QString LiveCapture::createSnapshotFileName()
{
	auto fnam = m_Settings.m_SnapshotFileNameFormat
		.arg(m_CurrentSnapshotIndex++);
	return m_Settings.m_SnapshotFolder + "/" + fnam;
}





bool LiveCapture::saveProject()
{
	const auto & fileName = m_Project->getFileName();
	QFile f(fileName);
	if (!f.open(QFile::WriteOnly))
	{
		emit logEvent(tr("Failed to save project: cannot write to file %1").arg(fileName));
		return false;
	}

	// Save the project:
	try
	{
		ProjectSaver::saveProject(*m_Project, f);
		m_Project->setSaved(fileName);
	}
	catch (const std::exception & exc)
	{
		emit logEvent(tr("Failed to save project to file %1: %2").arg(fileName).arg(exc.what()));
		return false;
	}
	return true;
}





