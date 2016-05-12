// DlgLiveCaptureSettings.cpp

// Implements the DlgLiveCaptureSettings class representing the UI dialog to set settings for a live session





#include "Globals.h"
#include "DlgLiveCaptureSettings.h"
#include <QFileDialog>
#include "ui_DlgLiveCaptureSettings.h"
#include "LiveCaptureSettings.h"





DlgLiveCaptureSettings::DlgLiveCaptureSettings(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::DlgLiveCaptureSettings)
{
	m_UI->setupUi(this);

	connect(m_UI->bNewProcessExecutableBrowse,  SIGNAL(clicked()), this, SLOT(bNewProcessExecutableBrowseClicked()));
	connect(m_UI->bNewProcessStartFolderBrowse, SIGNAL(clicked()), this, SLOT(bNewProcessStartFolderBrowseClicked()));
	connect(m_UI->bSnapshotFolderBrowse,        SIGNAL(clicked()), this, SLOT(bSnapshotFolderBrowseClicked()));
}





bool DlgLiveCaptureSettings::show(
	LiveCaptureSettings & a_Settings,
	const std::vector<VgdbComm::InstanceDesc> & a_RunningInstances
)
{
	// Copy the settings into the UI:
	m_Settings = &a_Settings;
	m_UI->rbCreateProcess->setChecked(a_Settings.m_ShouldCreateNewProcess);
	m_UI->eNewProcessExecutable->setText(a_Settings.m_NewProcessExecutable);
	m_UI->eNewProcessParams->setText(a_Settings.m_NewProcessParams);
	m_UI->eNewProcessStartFolder->setText(a_Settings.m_NewProcessStartFolder);
	m_UI->rbExistingProcess->setChecked(!a_Settings.m_ShouldCreateNewProcess);
	m_UI->sbCaptureInterval->setValue(a_Settings.m_CaptureIntervalSec);
	m_UI->chbSaveProject->setChecked(a_Settings.m_ShouldSaveProject);
	m_UI->chbSaveSnapshots->setChecked(a_Settings.m_ShouldSaveSnapshots);
	m_UI->eSnapshotFolder->setText(a_Settings.m_SnapshotFolder);
	m_UI->eSnapshotFileNameFormat->setText(a_Settings.m_SnapshotFileNameFormat);
	if (a_RunningInstances.empty())
	{
		m_UI->cbExistingProcess->addItem("<No Valgrind instances are currently running>", -1);
		m_UI->cbExistingProcess->setEnabled(false);
	}
	else
	{
		for (const auto & inst: a_RunningInstances)
		{
			m_UI->cbExistingProcess->addItem(
				QString::fromUtf8("%2: %1").arg(inst.second).arg(inst.first),
				inst.first
			);
		}
	}

	// Show the dialog:
	Super::show();
	if (exec() == QDialog::Rejected)
	{
		return false;
	}

	// Copy the settings from the UI:
	a_Settings.m_ShouldCreateNewProcess = m_UI->rbCreateProcess->isChecked();
	a_Settings.m_NewProcessExecutable   = m_UI->eNewProcessExecutable->text();
	a_Settings.m_NewProcessParams       = m_UI->eNewProcessParams->text();
	a_Settings.m_NewProcessStartFolder  = m_UI->eNewProcessStartFolder->text();
	a_Settings.m_ExistingProcessID      = m_UI->cbExistingProcess->itemData(m_UI->cbExistingProcess->currentIndex()).toInt();
	a_Settings.m_CaptureIntervalSec     = m_UI->sbCaptureInterval->value();
	a_Settings.m_ShouldSaveProject      = m_UI->chbSaveProject->isChecked();
	a_Settings.m_ShouldSaveSnapshots    = m_UI->chbSaveSnapshots->isChecked();
	a_Settings.m_SnapshotFolder         = m_UI->eSnapshotFolder->text();
	a_Settings.m_SnapshotFileNameFormat = m_UI->eSnapshotFileNameFormat->text();
	return true;
}





void DlgLiveCaptureSettings::bNewProcessExecutableBrowseClicked()
{
	auto fileName = QFileDialog::getOpenFileName(
		this,                          // Parent
		tr("Select executable file"),  // Title
		QString(),                     // Start folder
		#ifdef _WIN32
			tr("Executables (*.exe)")    // Filter (Win)
		#else
			tr("Executables (*.*)")      // Filter (other)
		#endif
	);
	if (!fileName.isEmpty())
	{
		m_UI->eNewProcessExecutable->setText(fileName);
	}
}





void DlgLiveCaptureSettings::bNewProcessStartFolderBrowseClicked()
{
	browseForFolder(m_UI->eNewProcessStartFolder);
}




void DlgLiveCaptureSettings::bSnapshotFolderBrowseClicked()
{
	browseForFolder(m_UI->eSnapshotFolder);
}





void DlgLiveCaptureSettings::browseForFolder(QLineEdit * a_DstLineEdit)
{
	auto folder = QFileDialog::getExistingDirectory(
		this,
		tr("Select folder")
	);
	if (!folder.isEmpty())
	{
		a_DstLineEdit->setText(folder);
	}
}




