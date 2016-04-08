// DlgSnapshotDiffs.cpp

// Implements the DlgSnapshotDiffs class representing the dialog for displaying multiple diffs between snapshots





#include "Globals.h"
#include "DlgSnapshotDiffs.h"
#include "ui_DlgSnapshotDiffs.h"
#include "SnapshotDiff.h"
#include "Snapshot.h"
#include "Allocation.h"
#include "SnapshotDiffModel.h"





static const int TW_DATA_ROLE_DIFFPTR = 1037;





DlgSnapshotDiffs::DlgSnapshotDiffs(QWidget * a_Parent):
	Super(a_Parent),
	m_UI(new Ui::DlgSnapshotDiffs)
{
	m_UI->setupUi(this);

	// Connect the signals:
	connect(m_UI->twDiffs, SIGNAL(itemSelectionChanged()), this, SLOT(onCurrentDiffChanged()));

	// Set up the sorting proxy model:
	m_SortingModel = std::make_shared<QSortFilterProxyModel>();
	m_SortingModel->setSortRole(SnapshotDiffModel::dataRoleSort);
	m_UI->tvDiff->setModel(m_SortingModel.get());
}





void DlgSnapshotDiffs::show(SnapshotDiffPtrs && a_Diffs)
{
	// Store the diffs internally:
	std::swap(a_Diffs, m_Diffs);
	for (auto d: m_Diffs)
	{
		m_Models[d.get()] = nullptr;
	}

	// Create an item for each diff:
	auto root = m_UI->twDiffs->invisibleRootItem();
	for (auto d: m_Diffs)
	{
		QStringList columns;
		columns << tr("%1").arg(d->getFirstSnapshot()->getTimestamp());
		columns << tr("%1").arg(d->getSecondSnapshot()->getTimestamp());
		auto item = new QTreeWidgetItem(columns);
		item->setData(0, TW_DATA_ROLE_DIFFPTR, reinterpret_cast<quint64>(d.get()));
		root->addChild(item);
	}

	Super::showMaximized();
}





void DlgSnapshotDiffs::onCurrentDiffChanged()
{
	// Get the SnapshotDiffPtr representing the diff:
	auto diff = reinterpret_cast<SnapshotDiff *>(m_UI->twDiffs->currentItem()->data(0, TW_DATA_ROLE_DIFFPTR).toULongLong());
	SnapshotDiffPtr diffPtr;
	for (auto d: m_Diffs)
	{
		if (d.get() == diff)
		{
			diffPtr = d;
			break;
		}
	}
	if (diffPtr == nullptr)
	{
		assert(!"Cannot find SnapshotDiffPtr for current item");
		return;
	}

	// Get the model, create it if it doesn't exist yet
	auto model = m_Models[diffPtr.get()];
	if (model == nullptr)
	{
		model = std::make_shared<SnapshotDiffModel>(diffPtr);
		m_Models[diffPtr.get()] = model;
	}
	m_SortingModel->setSourceModel(model.get());
}





