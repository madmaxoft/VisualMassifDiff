// LiveCaptureSettings.h

// Declares the LiveCaptureSettings class representing the settings for a live capture session





#ifndef LIVECAPTURESETTINGS_H
#define LIVECAPTURESETTINGS_H





class LiveCaptureSettings
{
public:
	LiveCaptureSettings():
		m_ShouldCreateNewProcess(true),
		m_ExistingProcessID(0),
		m_CaptureIntervalSec(30),
		m_ShouldSaveProject(true),
		m_ShouldSaveSnapshots(true)
	{
	}

	// Process settings:
	bool m_ShouldCreateNewProcess;
	QString m_NewProcessExecutable;
	QString m_NewProcessParams;
	QString m_NewProcessStartFolder;
	qint64 m_ExistingProcessID;

	// Capture settings:
	int m_CaptureIntervalSec;
	bool m_ShouldSaveProject;
	bool m_ShouldSaveSnapshots;
	QString m_SnapshotFolder;
	QString m_SnapshotFileNameFormat;
};





#endif // LIVECAPTURESETTINGS_H




