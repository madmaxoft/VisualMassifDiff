// DlgLiveCapture.cpp

// Implements the DlgLiveCapture class representing the dialog that is shown while the liva capture is in progress





#include "Globals.h"
#include "DlgLiveCapture.h"
#include <QMessageBox>
#include "ui_DlgLiveCapture.h"
#include "LiveCapture.h"





DlgLiveCapture::DlgLiveCapture(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::DlgLiveCapture)
{
	m_UI->setupUi(this);
}





void DlgLiveCapture::show(LiveCapture & a_LiveCapture, LiveCaptureSettings & a_Settings)
{
	m_LiveCapture = &a_LiveCapture;
	m_Settings = &a_Settings;
	connect(m_LiveCapture, SIGNAL(logEvent(QString)),                    this, SLOT(captureLogEvent(QString)));
	connect(m_LiveCapture, SIGNAL(error(QString)),                       this, SLOT(captureError(QString)));
	connect(m_LiveCapture, SIGNAL(timerTick(int)),                       this, SLOT(captureTimerTick(int)));
	connect(m_LiveCapture, SIGNAL(snapshotRecorded(QString)),            this, SLOT(captureSnapshotRecorded(QString)));
	connect(m_LiveCapture, SIGNAL(snapshotParsed(QString, SnapshotPtr)), this, SLOT(captureSnapshotParsed(QString, SnapshotPtr)));
	connect(m_LiveCapture, SIGNAL(snapshotAdded(QString, SnapshotPtr)),  this, SLOT(captureSnapshotAdded(QString, SnapshotPtr)));
	m_UI->pbSecondsTillNext->setMaximum(a_Settings.m_CaptureIntervalSec);
	Super::show();
	a_LiveCapture.start();
	Super::exec();
	a_LiveCapture.stop();
}





void DlgLiveCapture::captureLogEvent(const QString & a_Message)
{
	m_UI->txtLog->appendPlainText(a_Message + "\n");
}





void DlgLiveCapture::captureError(const QString & a_ErrorMessage)
{
	m_UI->txtLog->appendPlainText(tr("ERROR: %1\n").arg(a_ErrorMessage));
	m_UI->txtLog->appendPlainText(tr("Capture terminated."));
	m_LiveCapture->stop();

	// Display the error to the user:
	QMessageBox::warning(
		this,
		tr("Live Capture error"),
		tr("Error occured while capturing live data: %1. The capture terminated").arg(a_ErrorMessage)
	);

	// Change the "Stop capturing" button into a "Close" button:
	m_UI->bClose->setText(tr("&Close"));
}





void DlgLiveCapture::captureTimerTick(int a_SecondsLeft)
{
	m_UI->pbSecondsTillNext->setValue(a_SecondsLeft);
}





void DlgLiveCapture::captureSnapshotRecorded(const QString & a_FileName)
{
	m_UI->txtLog->appendPlainText(tr("Captured a snapshot into file %1").arg(a_FileName));
}





void DlgLiveCapture::captureSnapshotParsed(const QString & a_FileName, SnapshotPtr a_Snapshot)
{
	Q_UNUSED(a_Snapshot);
	m_UI->txtLog->appendPlainText(tr("Parsed a snapshot from file %1").arg(a_FileName));
}





void DlgLiveCapture::captureSnapshotAdded(const QString & a_FileName, SnapshotPtr a_Snapshot)
{
	Q_UNUSED(a_Snapshot);
	m_UI->txtLog->appendPlainText(tr("Added a snapshot from file %1 into the project").arg(a_FileName));
}





