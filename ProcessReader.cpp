// ProcessReader.cpp

// Implements the ProcessReader class representing a helper class used for reading process stdout and stderr





#include "ProcessReader.h"





ProcessReader::ProcessReader(QProcess & a_Process, QByteArray & a_StdOut, QByteArray & a_StdErr):
	Super(nullptr),
	m_Process(a_Process),
	m_StdOut(a_StdOut),
	m_StdErr(a_StdErr),
	m_HasTerminated(false),
	m_HasHadError(false)
{
	connect(&a_Process, SIGNAL(readyReadStandardOutput()),           this, SLOT(onProcessStdOutReady()));
	connect(&a_Process, SIGNAL(readyReadStandardError()),            this, SLOT(onProcessStdErrReady()));
	connect(&a_Process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));
	connect(&a_Process, SIGNAL(error(QProcess::ProcessError)),       this, SLOT(onError(QProcess::ProcessError)));
}





bool ProcessReader::waitForFinished(int a_TimeoutMsec)
{
	return m_Process.waitForFinished(a_TimeoutMsec);
}





void ProcessReader::onProcessStdOutReady()
{
	m_StdOut.append(m_Process.readAllStandardOutput());
}





void ProcessReader::onProcessStdErrReady()
{
	m_StdErr.append(m_Process.readAllStandardError());
}





void ProcessReader::onProcessFinished(int a_ExitCode, QProcess::ExitStatus a_ExitStatus)
{
	Q_UNUSED(a_ExitCode);
	Q_UNUSED(a_ExitStatus);

	m_HasTerminated = true;
}





void ProcessReader::onError(QProcess::ProcessError a_Error)
{
	Q_UNUSED(a_Error);

	m_HasTerminated = true;
	m_HasHadError = true;
}




