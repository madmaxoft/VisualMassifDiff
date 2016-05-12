// LiveCapture.h

// Declares the LiveCapture class encapsulating the Live Capture session





#ifndef LIVECAPTURE_H
#define LIVECAPTURE_H





#include <memory>
#include <QObject>
#include <QTimer>
#include "LiveCaptureSettings.h"





// fwd:
class Project;
class Snapshot;
typedef std::shared_ptr<Project> ProjectPtr;
typedef std::shared_ptr<Snapshot> SnapshotPtr;





class LiveCapture:
	public QObject
{
	typedef QObject Super;

	Q_OBJECT

public:
	explicit LiveCapture(ProjectPtr a_Project, const LiveCaptureSettings & a_Settings);

	/** Starts the capture. */
	void start();

	/** Stops the capture. */
	void stop();

signals:

	/** Emitted when a message should be appended to the log. */
	void logEvent(const QString & a_Message);

	/** Emitted when an irrecoverable error is encountered.
	The capture terminates after this signal is emitted. */
	void error(const QString & a_ErrorMessage);

	/** Emitted when the number of seconds till the next capture changes. */
	void timerTick(int a_SecondsLeft);

	/** Emitted when a new snapshot is saved into the specified file. */
	void snapshotRecorded(const QString & a_FileName);

	/** Emitted when a new snapshot is parsed. */
	void snapshotParsed(const QString & a_FileName, SnapshotPtr a_Snapshot);

	/** Emitted when a new snapshot is added into the project. */
	void snapshotAdded(const QString & a_FileName, SnapshotPtr a_Snapshot);


private slots:

	/** Handles the m_Timer's ticks. Schedules the captures. */
	void onTimer();

	/** Called when the parser has finished parsing a new snapshot. */
	void onParserNewSnapshot(SnapshotPtr a_Snapshot);

	/** Called when the parser parses the time unit used in the snapshot file. */
	void onParserTimeUnit(const char * a_TimeUnitString);

	/** Called when the command that was used to generate the report is parsed. */
	void onParserCommand(const char * a_CommandString);

	/** An error has occurred while parsing. */
	void onParserError(quint32 a_LineNum, const char * a_ErrorMessage, const char * a_Line);


protected:

	/** The project to which the snapshots are to be parsed.
	Used mainly for its CodeLocationFactory. */
	ProjectPtr m_Project;

	/** The settings for the LiveCapture session. */
	LiveCaptureSettings m_Settings;

	/** Number of seconds left until the next capture.
	Updated using a timer, and when it reaches zero, the actual capture is performed. */
	int m_SecondsTillCapture;

	/** Filename for the snapshot currently being processed. */
	QString m_SnapshotFileName;

	/** PID of the process to capture. */
	qint64 m_Pid;

	/** The timer used for scheduling the capture. */
	QTimer m_Timer;

	/** Holds the number of snapshot files created so far; used for creating a new snapshot filename.
	Used only if the settings indicate that snapshots should be saved. */
	int m_CurrentSnapshotIndex;


	/** Performs the capture. */
	void doCapture();

	/** Parses the snapshot data from file m_FileName, adds the snapshot to the project. */
	void processSnapshotFile();

	/** Returns a filename to be used for saving the next snapshot. */
	QString createSnapshotFileName();

	/** Saves m_Project into its file.
	Returns true if the project was saved properly, false on failure. */
	bool saveProject();
};





#endif // LIVECAPTURE_H




