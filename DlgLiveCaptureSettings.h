// DlgLiveCaptureSettings.h

// Declares the DlgLiveCaptureSettings class representing the UI dialog to set settings for a live capture session





#ifndef DLGLIVESESSIONSETTINGS_H
#define DLGLIVESESSIONSETTINGS_H





#include <memory>
#include <QDialog>





#include "VgdbComm.h"





// fwd:
class QLineEdit;
class LiveCaptureSettings;





namespace Ui
{
	class DlgLiveCaptureSettings;
}





class DlgLiveCaptureSettings:
	public QDialog
{
	typedef QDialog Super;

	Q_OBJECT

public:
	explicit DlgLiveCaptureSettings(QWidget * a_Parent = nullptr);

	/** Shows the dialog, initialized to the specified settings.
	If the user clicks "Capture", returns true and the settings are updated to match whatever the user entered.
	If the user cancels the dialog, returns false.
	a_RunningInstances is a vector containing the instances that the user can choose as the "existing process".
	Returns only after the dialog is closed. */
	bool show(
		LiveCaptureSettings & a_Settings,
		const std::vector<VgdbComm::InstanceDesc> & a_RunningInstances
	);

private slots:
	void bNewProcessExecutableBrowseClicked();
	void bNewProcessStartFolderBrowseClicked();
	void bSnapshotFolderBrowseClicked();

private:
	std::shared_ptr<Ui::DlgLiveCaptureSettings> m_UI;

	LiveCaptureSettings * m_Settings;


	/** Displays the BrowseForFolder dialog and sets the selected folder into the specified edit box. */
	void browseForFolder(QLineEdit * a_DstLineEdit);
};





#endif // DLGLIVESESSIONSETTINGS_H




