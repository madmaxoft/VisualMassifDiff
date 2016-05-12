// ProcessReader.cpp

// Implements the ProcessReader class representing a helper class used for reading process stdout and stderr
/*
The process needs to have its stdout and stderr read periodically, otherwise it could deadlock by having
its OS IO pipe full. This helper class is used to provide the reading for both pipes as needed.
*/





#ifndef PROCESSREADER_H
#define PROCESSREADER_H





#include <assert.h>
#include <QObject>
#include <QProcess>





class ProcessReader:
	public QObject
{
	typedef QObject Super;

	Q_OBJECT

public:
	explicit ProcessReader(QProcess & a_Process, QByteArray & a_StdOut, QByteArray & a_StdErr);

	/** Returns the data read from the process' stdout.
	Only call this function after the process terminates. */
	const QByteArray & getStdOut() const { assert(m_HasTerminated); return m_StdOut; }

	/** Returns the data read from the process' stderr.
	Only call this function after the process terminates. */
	const QByteArray & getStdErr() const { assert(m_HasTerminated); return m_StdErr; }

	/** Waits for the process to terminate, while reading both its streams. */
	bool waitForFinished(int a_TimeoutMsec = 30000);

	/** Returns true if the process has encountered an error (not started, crashed etc.).
	Only call this function after the process terminates. */
	bool hasHadError() const { assert(m_HasTerminated); return m_HasHadError; }


public slots:

	/** Emitted by QProcess when there's something to read from its stdout. */
	void onProcessStdOutReady();

	/** Emitted by QProcess when there's something to read from its stderr. */
	void onProcessStdErrReady();

	/** Emitted by QProcess when the process terminates. */
	void onProcessFinished(int a_ExitCode, QProcess::ExitStatus a_ExitStatus);

	/** Emitted by QProcess when there is an error with the process (not started, crashed etc.) */
	void onError(QProcess::ProcessError a_Error);


protected:

	/** The process being monitored for read operations. */
	QProcess & m_Process;

	/** The data read from process' stdout. */
	QByteArray & m_StdOut;

	/** The data read from process' stderr. */
	QByteArray & m_StdErr;

	/** Set to true when the process terminates. */
	bool m_HasTerminated;

	/** Set to true if the process encountered an error (onError() handler). */
	bool m_HasHadError;
};





#endif // PROCESSREADER_H




