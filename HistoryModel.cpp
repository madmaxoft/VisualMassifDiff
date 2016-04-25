// HistoryModel.cpp

// Implements the HistoryModel class representing a Qt model for displaying the project history in a view / graph





#include "Globals.h"
#include "HistoryModel.h"
#include <assert.h>
#include <QSize>
#include "CodeLocation.h"
#include "CodeLocationStats.h"
#include "FormatNumber.h"
#include "Project.h"





static QColor getNextColor()
{
	static size_t idx = 0;
	static QColor colors[] =
	{
		// Regular colors:
		QColor(0xff, 0x00, 0x00),
		QColor(0x00, 0xff, 0x00),
		QColor(0x00, 0x00, 0xff),
		QColor(0xff, 0xff, 0x00),
		QColor(0x00, 0xff, 0xff),
		QColor(0xff, 0x00, 0xff),

		// Dark colors:
		QColor(0x7f, 0x00, 0x00),
		QColor(0x00, 0x7f, 0x00),
		QColor(0x7f, 0x7f, 0x00),
		QColor(0x00, 0x7f, 0x7f),
		QColor(0x7f, 0x00, 0x7f),

		// Light colors:
		QColor(0xff, 0x7f, 0x7f),
		QColor(0x7f, 0xff, 0x7f),
		QColor(0x7f, 0x7f, 0xff),
		QColor(0xff, 0xff, 0x7f),
		QColor(0x7f, 0xff, 0xff),
		QColor(0xff, 0x7f, 0xff),
	};
	return colors[(idx++) % ARRAYCOUNT(colors)];
}





////////////////////////////////////////////////////////////////////////////////
// HistoryModel::GraphedAllocationPath:

HistoryModel::GraphedAllocationPath::GraphedAllocationPath(
	const AllocationPath & a_AllocationPath,
	const AllocationStats & a_Stats
):
	m_AllocationPath(a_AllocationPath),
	m_Stats(a_Stats),
	m_Color(getNextColor())
{
}





////////////////////////////////////////////////////////////////////////////////
// HistoryModel:

HistoryModel::HistoryModel(ProjectPtr a_Project):
	m_Project(a_Project)
{
	connect(m_Project.get(), SIGNAL(addedSnapshot(SnapshotPtr)), this, SLOT(resetModel()));
	resetModel();
}





bool HistoryModel::canItemExpand(const QModelIndex & a_Index) const
{
	if (!isValidIndex(a_Index))
	{
		return false;
	}
	auto gap = m_GraphedAllocationPaths[a_Index.row()];
	if (gap == nullptr)
	{
		return false;
	}
	auto path = gap->m_AllocationPath;
	for(;;)
	{
		const auto & children = m_Project->getAllAllocationPathsImmediateChildren(path);
		if (children.empty())
		{
			return false;
		}
		if (children.size() > 1)
		{
			return true;
		}
		path = children[0];
	}
}





bool HistoryModel::canItemCollapse(const QModelIndex & a_Index) const
{
	if (!isValidIndex(a_Index))
	{
		return false;
	}
	auto gap = m_GraphedAllocationPaths[a_Index.row()];
	if (gap == nullptr)
	{
		return false;
	}
	const auto & path = gap->m_AllocationPath;
	return (path.getSegments().size() > 1);  // At least two segments
}





bool HistoryModel::isValidIndex(const QModelIndex & a_Index) const
{
	if ((a_Index.row() < 0) || (a_Index.row() >= static_cast<int>(m_GraphedAllocationPaths.size())))
	{
		return false;
	}
	if ((a_Index.column() < 0) || (a_Index.column() > colMax))
	{
		return false;
	}

	return true;
}





void HistoryModel::resetModel()
{
	beginResetModel();

	// Insert all top-level paths:
	m_GraphedAllocationPaths.clear();
	m_GraphedAllocationPaths.push_back(std::make_shared<GraphedAllocationPath>(AllocationPath(), AllocationStats()));

	endResetModel();

	expandItem(0);
}





void HistoryModel::expandItem(const QModelIndex & a_Item)
{
	expandItem(a_Item.row());
}





void HistoryModel::collapseItem(const QModelIndex & a_Item)
{
	if (!isValidIndex(a_Item))
	{
		return;
	}
	auto gap = m_GraphedAllocationPaths[a_Item.row()];
	auto parentPath = gap->m_AllocationPath.makeParent();

	// Change current item into its parent:
	gap->m_AllocationPath = parentPath;
	gap->m_Stats = m_Project->getStatsForAllocationPath(parentPath);

	// Remove all children of the parent:
	auto numGraphedAllocationPaths = m_GraphedAllocationPaths.size();
	bool wasLastDeleted = false;
	int lastDeletionStart = 0;
	for (int i = static_cast<int>(numGraphedAllocationPaths) - 1; i >= 0; --i)
	{
		if (m_GraphedAllocationPaths[i]->m_AllocationPath.isChildPathOf(parentPath))
		{
			// The item should be deleted
			if (!wasLastDeleted)
			{
				lastDeletionStart = i;
				wasLastDeleted = true;
			}
		}
		else
		{
			// The item shouldn't be deleted. If we've been queueing items up for deletion until now, delete them:
			if (wasLastDeleted)
			{
				beginRemoveRows(QModelIndex(), i + 1, lastDeletionStart);
				m_GraphedAllocationPaths.erase(m_GraphedAllocationPaths.begin() + i + 1, m_GraphedAllocationPaths.begin() + lastDeletionStart + 1);
				endRemoveRows();
				wasLastDeleted = false;
			}
		}
	}  // for i - m_GraphedAllocationPaths[]
	if (wasLastDeleted)
	{
		beginRemoveRows(QModelIndex(), 0, lastDeletionStart);
		m_GraphedAllocationPaths.erase(m_GraphedAllocationPaths.begin(), m_GraphedAllocationPaths.begin() + lastDeletionStart + 1);
		endRemoveRows();
	}
	emit modelDataChanged();
}





void HistoryModel::expandItem(int a_Index)
{
	if ((a_Index < 0) | (a_Index >= static_cast<int>(m_GraphedAllocationPaths.size())))
	{
		assert(!"Invalid expansion index");
		return;
	}

	// Get a list of children paths and their stats:
	const auto & path = m_GraphedAllocationPaths[a_Index]->m_AllocationPath;
	std::vector<std::pair<AllocationPath, AllocationStats>> tmp;
	for (const auto & p: m_Project->getAllAllocationPathsImmediateChildren(path))
	{
		const auto & stats = m_Project->getStatsForAllocationPath(p);
		tmp.emplace_back(p, stats);
	}
	if (tmp.empty())
	{
		// Do not expand leaf items
		return;
	}

	// Sort the children:
	std::sort(tmp.begin(), tmp.end(),
		[](
			const std::pair<AllocationPath, AllocationStats> & a_First,
			const std::pair<AllocationPath, AllocationStats> & a_Second
		)
		{
			// TODO: Improve the logic by moving items with zero max-min difference to bottom
			return (a_First.second.m_AvgAllocationSize > a_Second.second.m_AvgAllocationSize);
		}
	);

	// Insert the children into a temp vector (for later insertion):
	std::vector<GraphedAllocationPathPtr> paths;
	for (const auto & t: tmp)
	{
		paths.push_back(std::make_shared<GraphedAllocationPath>(t.first, t.second));
	}

	// Replace the parent row with rows representing the children in the model:
	beginRemoveRows(QModelIndex(), a_Index, a_Index);
	m_GraphedAllocationPaths.erase(m_GraphedAllocationPaths.begin() + a_Index);
	endRemoveRows();
	beginInsertRows(QModelIndex(), a_Index, a_Index + static_cast<int>(paths.size()) - 1);
	m_GraphedAllocationPaths.insert(m_GraphedAllocationPaths.begin() + a_Index, paths.begin(), paths.end());
	endInsertRows();
	emit modelDataChanged();
}





int HistoryModel::rowCount(const QModelIndex & a_Parent) const
{
	if (a_Parent.isValid())
	{
		return 0;
	}
	return static_cast<int>(m_GraphedAllocationPaths.size());
}





int HistoryModel::columnCount(const QModelIndex &) const
{
	return colMaxPlusOne;
}





QVariant HistoryModel::data(const QModelIndex & a_Index, int a_Role) const
{
	if (!a_Index.isValid())
	{
		return QVariant();
	}
	assert(a_Index.row() >= 0);
	assert(a_Index.row() < static_cast<int>(m_GraphedAllocationPaths.size()));

	const auto gap = m_GraphedAllocationPaths[a_Index.row()];
	switch (a_Role)
	{
		case Qt::DecorationRole:
		{
			if (a_Index.column() == 0)
			{
				return gap->m_Color;
			}
			return QVariant();
		}  // case DecorationRole

		case Qt::DisplayRole:
		{
			const auto & stats = gap->m_Stats;
			switch (a_Index.column())
			{
				case colFunctionName:
				{
					auto lcl = gap->m_AllocationPath.getLeafSegment();
					if (lcl == nullptr)
					{
						return "<unknown location>";
					}
					return lcl->getFunctionName();
				}
				case colMinAllocationSize:  return formatBigNumber(stats.m_MinAllocationSize);
				case colMaxAllocationSize:  return formatBigNumber(stats.m_MaxAllocationSize);
				case colAvgAllocationSize:  return formatBigNumber(stats.m_AvgAllocationSize);
				case colDiffAllocationSize: return formatBigNumber(stats.m_MaxAllocationSize - stats.m_MinAllocationSize);
			}
			break;
		}  // case DisplayRole
	}
	return QVariant();
}





QVariant HistoryModel::headerData(int a_Section, Qt::Orientation a_Orientation, int a_Role) const
{
	if (a_Orientation != Qt::Horizontal)
	{
		return QVariant();
	}
	switch (a_Role)
	{
		case Qt::DisplayRole:
		{
			switch (a_Section)
			{
				case colFunctionName:       return tr("Function");
				case colMinAllocationSize:  return tr("Min");
				case colMaxAllocationSize:  return tr("Max");
				case colAvgAllocationSize:  return tr("Avg");
				case colDiffAllocationSize: return tr("Max - Min");
				case colHierarchy:          return tr("Hierarchy");
				case colPosition:           return tr("Pos");
			}
			break;
		}
	}
	return QVariant();
}




