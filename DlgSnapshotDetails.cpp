// DlgSnapshotDetails.cpp

// Implements the DlgSnapshotDetails class representing the UI dialog to display details about a single snapshot





#include "Globals.h"
#include "DlgSnapshotDetails.h"
#include "ui_DlgSnapshotDetails.h"
#include "FormatNumber.h"
#include "Snapshot.h"
#include "Allocation.h"





/** Role used as a storage for the allocation data in the tree widget. */
static const int TW_ITEM_DATAROLE_ALLOCATIONPTR = 10035;





DlgSnapshotDetails::DlgSnapshotDetails(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::DlgSnapshotDetails)
{
	m_UI->setupUi(this);
	connect(m_UI->twAllocations, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(onTreeItemExpanded(QTreeWidgetItem *)));
}






void DlgSnapshotDetails::show(SnapshotPtr a_Snapshot)
{
	m_Snapshot = a_Snapshot;
	m_UI->txtTimestamp->setText(formatBigNumber(a_Snapshot->getTimestamp()));
	m_UI->txtHeapSize->setText(formatMemorySize(a_Snapshot->getHeapSize()));
	m_UI->txtHeapExtraSize->setText(formatMemorySize(a_Snapshot->getHeapExtraSize()));
	m_UI->grAllocations->setAllocation(a_Snapshot->getRootAllocation());
	updateAllocationsTree();
	Super::show();
}





void DlgSnapshotDetails::onTreeItemExpanded(QTreeWidgetItem * a_TreeItem)
{
	ensureCorrectChildIndicatorsForChildren(a_TreeItem);
}





void DlgSnapshotDetails::updateAllocationsTree()
{
	m_UI->twAllocations->clear();
	insertChildAllocations(m_UI->twAllocations->invisibleRootItem(), m_Snapshot->getRootAllocation().get());
	ensureCorrectChildIndicatorsForChildren(m_UI->twAllocations->invisibleRootItem());
}





void DlgSnapshotDetails::insertChildAllocations(QTreeWidgetItem * a_TreeItem, Allocation * a_Allocation)
{
	for (const auto & ch: a_Allocation->getChildren())
	{
		QStringList columns;
		const auto codeLoc = ch->getCodeLocation();
		if (codeLoc == nullptr)
		{
			columns << tr("?");
			columns << formatMemorySize(ch->getAllocationSize());
			columns << QString();
			columns << QString();
		}
		else
		{
			columns << ch->getFunctionName();
			columns << formatMemorySize(ch->getAllocationSize());
			columns << ch->getFileName();
			columns << tr("%1").arg(ch->getFileLineNum());
		}
		auto twi = new QTreeWidgetItem(columns);
		twi->setData(0, TW_ITEM_DATAROLE_ALLOCATIONPTR, reinterpret_cast<qulonglong>(ch.get()));
		twi->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
		twi->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
		twi->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
		a_TreeItem->addChild(twi);
	}
}





void DlgSnapshotDetails::ensureCorrectChildIndicatorsForChildren(QTreeWidgetItem * a_TreeItem)
{
	for (int i = a_TreeItem->childCount() - 1; i >= 0; i--)
	{
		auto child = a_TreeItem->child(i);
		if (child->childCount() > 0)
		{
			// Already has children populated
			continue;
		}
		// Insert any children, if appropriate:
		auto allocation = reinterpret_cast<Allocation *>(child->data(0, TW_ITEM_DATAROLE_ALLOCATIONPTR).toULongLong());
		insertChildAllocations(child, allocation);
	}  // for i - a_TreeItem->children[]
}




