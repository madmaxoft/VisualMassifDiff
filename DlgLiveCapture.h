// DlgLiveCapture.h

// Declares the DlgLiveCapture class representing the dialog that is shown while the liva capture is in progress





#ifndef DLGLIVECAPTURE_H
#define DLGLIVECAPTURE_H





#include <memory>
#include <QDialog>





// fwd:
class LiveCapture;
class LiveCaptureSettings;
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;





namespace Ui
{
	class DlgLiveCapture;
}





class DlgLiveCapture:
	public QDialog
{
	typedef QDialog Super;

	Q_OBJECT

public:
	explicit DlgLiveCapture(QWidget * a_Parent = nullptr);

	/** Connects to the specified LiveCapture instance and displays its progress.*/
	void show(LiveCapture & a_LiveCapture, LiveCaptureSettings & a_Settings);


private slots:
	/** Displays the specified log message in the UI. */
	void captureLogEvent(const QString & a_Message);

	/** Displays the error message and closes the dialog. */
	void captureError(const QString & a_ErrorMessage);

	/** Updates the number of seconds till the next capture changes. */
	void captureTimerTick(int a_SecondsLeft);

	/** Logs a message about the recorded snapshot. */
	void captureSnapshotRecorded(const QString & a_FileName);

	/** Logs a message about the parsing. */
	void captureSnapshotParsed(const QString & a_FileName, SnapshotPtr a_Snapshot);

	/** Logs a message about the addition. */
	void captureSnapshotAdded(const QString & a_FileName, SnapshotPtr a_Snapshot);

private:

	/** The QTCreator-managed UI. */
	std::shared_ptr<Ui::DlgLiveCapture> m_UI;

	/** The LiveCapture instance that is being controlled by this dlg. */
	LiveCapture * m_LiveCapture;

	/** The settings used for this capture. */
	LiveCaptureSettings * m_Settings;
};





#endif // DLGLIVECAPTURE_H




