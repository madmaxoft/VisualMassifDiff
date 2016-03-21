// dlgsnapshotdetails.cpp

// Implements the DlgSnapshotDetails class representing the UI dialog to display details about a single snapshot





#include "globals.h"
#include "dlgsnapshotdetails.h"
#include "ui_dlgsnapshotdetails.h"
#include "formatnumber.h"
#include "snapshot.h"





DlgSnapshotDetails::DlgSnapshotDetails(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::DlgSnapshotDetails)
{
	m_UI->setupUi(this);
}






void DlgSnapshotDetails::show(SnapshotPtr a_Snapshot)
{
	m_Snapshot = a_Snapshot;
	m_UI->txtTimestamp->setText(formatBigNumber(a_Snapshot->getTimestamp()));
	m_UI->txtHeapSize->setText(formatMemorySize(a_Snapshot->getHeapSize()));
	m_UI->txtHeapExtraSize->setText(formatMemorySize(a_Snapshot->getHeapExtraSize()));
	m_UI->grAllocations->setAllocation(a_Snapshot->getRootAllocation());
	Super::show();
}




