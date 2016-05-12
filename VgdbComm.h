// VgdbComm.cpp

// Implements the VgdbComm class representing an interface to communicate with a vgdb





#ifndef VGDBCOMM_H
#define VGDBCOMM_H





#include <vector>
#include <QObject>





class VgdbComm
{
public:
	/** Type used to describe a single instance of a process being run under Valgrind's Massif tool. */
	typedef std::pair<qint64, QString> InstanceDesc;


	VgdbComm();

	/** Checks whether vgdb is available on this system.
	Returns true if vgdb is available, false if not. */
	static bool checkAvailability();

	/** Returns the descriptions of running valgrind instances.
	Returns a vector of pairs, each pair describes one instance as a pid and command. */
	static std::vector<InstanceDesc> listRunningInstances();

	/** Captures a snapshot of the instance specified by its Pid, into a file. */
	static bool captureSnapshot(
		quint64 a_InstancePid,
		const QString & a_FileName,
		QByteArray & a_StdOut,
		QByteArray & a_StdErr
	);

	/** Launches a new valgrind massif instance for the specified executable, passing the parameters to it.
	Returns the pid of the new instance on success, -1 on error.
	The process is started detached, so that it continues running even if VMD terminates. */
	static qint64 launchNewInstance(
		const QString & a_Executable,
		const QString & a_Parameters,
		const QString & a_StartFolder
	);

protected:
	/** Runs vgdb with the specified parameters, and captures its stdout and stderr.
	Returns the process exit code, or -1 if the process errored. */
	static int runVgdbCommand(const QStringList & a_Command, QByteArray & a_StdOut, QByteArray & a_StdErr);
};





#endif // VGDBCOMM_H




