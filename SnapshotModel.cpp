// SnapshotModel.cpp

// Implements the SnapshotModel class representing the model for displaying project's snapshots





#include "Globals.h"
#include "SnapshotModel.h"
#include <QStandardItem>
#include "Project.h"
#include "Snapshot.h"
#include "FormatNumber.h"





SnapshotModel::SnapshotModel(ProjectPtr a_Project):
	Super(nullptr),
	m_Project(a_Project),
	m_IcoAllocations(":/icoAllocations.png")
{
	// Insert the columns:
	initColumns();

	// Initialize the basic parameters:
	setSortRole(SortRole);

	// Insert current snapshots, and connect to project's signal to insert future added snapshots:
	for (const auto & s: a_Project->getSnapshots())
	{
		addSnapshot(s);
	}
	connect(m_Project.get(), SIGNAL(addedSnapshot(SnapshotPtr)), this, SLOT(onProjectAddedSnapshot(SnapshotPtr)));
}





SnapshotPtr SnapshotModel::getItemSnapshot(const QModelIndex & a_Index) const
{
	auto timestamp = data(a_Index, SnapshotTimestampRole);
	return m_Project->getSnapshotAtTimestamp(timestamp.toULongLong());
}





Qt::ItemFlags SnapshotModel::flags(const QModelIndex & a_Index) const
{
	// Drop the "editable" flag for all items:
	auto res = Super::flags(a_Index);
	return res & (~Qt::ItemIsEditable);
}





void SnapshotModel::onProjectAddedSnapshot(SnapshotPtr a_Snapshot)
{
	addSnapshot(a_Snapshot);
}





void SnapshotModel::initColumns()
{
	auto cTimestamp = new QStandardItem(tr("Timestamp"));
	cTimestamp->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	setHorizontalHeaderItem(colTimestamp, cTimestamp);
	auto cHeapSize = new QStandardItem(tr("Heap size (KiB)"));
	cHeapSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	setHorizontalHeaderItem(colHeapSize, cHeapSize);
	auto cHeapExtraSize = new QStandardItem(tr("Heap extra size (KiB)"));
	cHeapExtraSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	setHorizontalHeaderItem(colHeapExtraSize, cHeapExtraSize);
}





void SnapshotModel::addSnapshot(SnapshotPtr a_Snapshot)
{
	QList<QStandardItem *> row;
	auto iTimestamp = new QStandardItem(formatBigNumber(a_Snapshot->getTimestamp()));
	iTimestamp->setData(a_Snapshot->getTimestamp(), SnapshotTimestampRole);
	iTimestamp->setData(a_Snapshot->getTimestamp(), SortRole);
	iTimestamp->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	if (a_Snapshot->hasAllocations())
	{
		iTimestamp->setIcon(m_IcoAllocations);
	}
	row << iTimestamp;

	auto iHeapSize = new QStandardItem(formatMemorySize(a_Snapshot->getHeapSize()));
	iHeapSize->setData(a_Snapshot->getHeapSize(), SortRole);
	iHeapSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	row << iHeapSize;

	auto iHeapExtraSize = new QStandardItem(formatMemorySize(a_Snapshot->getHeapExtraSize()));
	iHeapExtraSize->setData(a_Snapshot->getHeapExtraSize(), SortRole);
	iHeapExtraSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	row << iHeapExtraSize;

	appendRow(row);
}




