// snapshotdiffmodel.cpp

// Implements the SnapshotDiffModel class representing a Qt model for visualising the snapshot diff





#include "globals.h"
#include "snapshotdiffmodel.h"
#include "snapshotdiff.h"
#include "codelocation.h"
#include "formatnumber.h"





SnapshotDiffModel::SnapshotDiffModel(SnapshotDiffPtr a_Diff):
	Super(nullptr),
	m_Diff(a_Diff)
{
}





QVariant SnapshotDiffModel::data(const QModelIndex & a_Index, int a_Role) const
{
	if (!a_Index.isValid())
	{
		return QVariant();
	}
	DiffItem * di = reinterpret_cast<DiffItem *>(a_Index.internalPointer());
	switch (a_Role)
	{
		case Qt::DisplayRole:
		{
			switch(a_Index.column())
			{
				case 0: return formatBigSignedNumber(di->getHeapSizeDiff());
				case 1: return di->getCodeLocation()->getFunctionName();
				case 2: return di->getCodeLocation()->getFileName();
				case 3: return di->getCodeLocation()->getFileLineNum();
			}
			break;
		}
		case dataRoleSort:
		{
			switch(a_Index.column())
			{
				case 0: return di->getHeapSizeDiff();
				case 1: return di->getCodeLocation()->getFunctionName();
				case 2: return di->getCodeLocation()->getFileName();
				case 3: return di->getCodeLocation()->getFileLineNum();
			}
			break;
		}
	}
	return QVariant();
}





Qt::ItemFlags SnapshotDiffModel::flags(const QModelIndex & a_Index) const
{
	if (!a_Index.isValid())
	{
		return 0;
	}

	return Super::flags(a_Index);
}





QVariant SnapshotDiffModel::headerData(int a_Section, Qt::Orientation a_Orientation, int a_Role) const
{
	if ((a_Orientation == Qt::Horizontal) && (a_Role == Qt::DisplayRole))
	{
		switch (a_Section)
		{
			case 0: return tr("Diff (B)");
			case 1: return tr("Function");
			case 2: return tr("File");
			case 3: return tr("Line");
		}
		return QVariant();
	}

	return QVariant();
}





QModelIndex SnapshotDiffModel::index(int a_Row, int a_Column, const QModelIndex & a_Parent) const
{
	if (!hasIndex(a_Row, a_Column, a_Parent))
	{
		return QModelIndex();
	}

	DiffItem * parentItem = nullptr;

	if (!a_Parent.isValid())
	{
		parentItem = m_Diff->getRootDiffItem().get();
	}
	else
	{
		parentItem = static_cast<DiffItem *>(a_Parent.internalPointer());
	}

	DiffItem * childItem = parentItem->child(a_Row).get();
	if (childItem != nullptr)
	{
		return createIndex(a_Row, a_Column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}





QModelIndex SnapshotDiffModel::parent(const QModelIndex & a_Index) const
{
	if (!a_Index.isValid())
	{
		return QModelIndex();
	}

	auto childItem = static_cast<DiffItem *>(a_Index.internalPointer());
	auto parentItem = childItem->getParent();

	if (parentItem == m_Diff->getRootDiffItem().get())
	{
		return QModelIndex();
	}

	return createIndex(parentItem->indexOfChild(childItem), 0, parentItem);
}





int SnapshotDiffModel::rowCount(const QModelIndex & a_Parent) const
{
	if (a_Parent.column() > 0)
	{
		return 0;
	}

	DiffItem * parentItem;
	if (!a_Parent.isValid())
	{
		parentItem = m_Diff->getRootDiffItem().get();
	}
	else
	{
		parentItem = static_cast<DiffItem *>(a_Parent.internalPointer());
	}

	return static_cast<int>(parentItem->getChildren().size());
}





int SnapshotDiffModel::columnCount(const QModelIndex &) const
{
	return 4;
}




