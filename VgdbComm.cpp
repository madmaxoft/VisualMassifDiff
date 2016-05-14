// VgdbComm.cpp

// Implements the VgdbComm class representing an interface to communicate with a vgdb





#include "Globals.h"
#include "VgdbComm.h"
#include <QDebug>
#include "ProcessReader.h"
#include "ParseInteger.h"





#ifdef _WIN32
	#define VALGRIND_EXECUTABLE_NAME "valgrind.cmd"
	#define VGDB_EXECUTABLE_NAME "vgdb.cmd"
#else
	#define VALGRIND_EXECUTABLE_NAME "valgrind"
	#define VGDB_EXECUTABLE_NAME "vgdb"
#endif





bool VgdbComm::checkAvailability()
{
	QByteArray stdOut, stdErr;
	return (runVgdbCommand(QStringList() << "--help", stdOut, stdErr) == 0);
}





std::vector<VgdbComm::InstanceDesc> VgdbComm::listRunningInstances()
{
	QByteArray stdOut, stdErr;
	if (runVgdbCommand(QStringList() << "-l", stdOut, stdErr) != 1)
	{
		// No running instances of valgrind at all
		return std::vector<InstanceDesc>();
	}

	// Parse stdOut for individual instances:
	/*
	Example output:
	use --pid=24397 for /usr/bin/valgrind.bin --tool=massif top
	use --pid=24411 for /usr/bin/valgrind.bin --tool=massif top
	*/
	std::vector<InstanceDesc> res;
	for (const auto & line: stdOut.split('\n'))
	{
		if (!line.startsWith("use --pid="))
		{
			continue;
		}
		if (!line.contains(" --tool=massif "))
		{
			continue;
		}
		auto idxFor = line.indexOf(" for ");
		if (idxFor < 0)
		{
			continue;
		}
		auto idxSpace = line.indexOf(' ', 10);
		InstanceDesc d;
		if (!parseInteger(line.data() + 10, d.first, idxSpace - 10))
		{
			continue;
		}
		d.second = QString::fromUtf8(line.data() + idxFor + 5);
		res.push_back(std::move(d));
	}
	return res;
}





bool VgdbComm::captureSnapshot(
	quint64 a_InstancePid,
	const QString & a_FileName,
	QByteArray & a_StdOut,
	QByteArray & a_StdErr
)
{
	auto exitCode = runVgdbCommand(QStringList()
		<< QString::fromUtf8("--pid=%1").arg(a_InstancePid)
		<< "detailed_snapshot"
		<< a_FileName,
		a_StdOut, a_StdErr
	);

	return (exitCode == 0);
}





qint64 VgdbComm::launchNewInstance(
	const QString & a_Executable,
	const QString & a_Parameters,
	const QString & a_StartFolder
)
{
	qint64 pid = 0;
	if (!QProcess::startDetached(
		VALGRIND_EXECUTABLE_NAME,
		QStringList()
			<< "--tool=massif"
			<< a_Executable
			<< a_Parameters,
		a_StartFolder,
		&pid
	))
	{
		return -1;
	}
	return pid;
}





int VgdbComm::runVgdbCommand(const QStringList & a_Command, QByteArray & a_StdOut, QByteArray & a_StdErr)
{
	QProcess proc;
	ProcessReader reader(proc, a_StdOut, a_StdErr);
	proc.start(VGDB_EXECUTABLE_NAME, a_Command);
	reader.waitForFinished();
	if (reader.hasHadError())
	{
		return -1;
	}
	return proc.exitCode();
}





