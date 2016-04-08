// CodeLocationStatsModel.cpp

// Implements the CodeLocationStatsModel class representing the model for visualising CodeLocationStats





#include "Globals.h"
#include "CodeLocationStatsModel.h"
#include "Project.h"
#include "CodeLocation.h"
#include "CodeLocationStats.h"
#include "FormatNumber.h"





CodeLocationStatsModel::CodeLocationStatsModel(ProjectPtr a_Project):
	Super(nullptr),
	m_Project(a_Project)
{
	rebuildModel();
}





void CodeLocationStatsModel::addedSnapshot()
{
	rebuildModel();
}





void CodeLocationStatsModel::rebuildModel()
{
	beginResetModel();
	m_Stats = std::move(m_Project->getCodeLocationStats()->getAllStats());
	endResetModel();
}





int CodeLocationStatsModel::rowCount(const QModelIndex & a_Parent) const
{
	if (a_Parent.column() > 0)
	{
		return 0;
	}

	return static_cast<int>(m_Stats.size());
}





int CodeLocationStatsModel::columnCount(const QModelIndex &) const
{
	return 7;
}





QVariant CodeLocationStatsModel::data(const QModelIndex & a_Index, int a_Role) const
{
	if ((a_Index.row() < 0) || (static_cast<size_t>(a_Index.row()) > m_Stats.size()))
	{
		return QVariant();
	}
	const auto & stat = m_Stats[a_Index.row()];

	switch (a_Role)
	{
		case Qt::TextAlignmentRole:
		{
			switch (a_Index.column())
			{
				case 0: return Qt::AlignRight;
				case 1: return Qt::AlignLeft;
				case 2: return Qt::AlignLeft;
				case 3: return Qt::AlignRight;
				case 4: return Qt::AlignRight;
				case 5: return Qt::AlignRight;
				case 6: return Qt::AlignRight;
			}
			break;
		}  // case Qt::TextAlignmentRole

		case Qt::DisplayRole:
		{
			switch (a_Index.column())
			{
				case 0:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return QString::fromUtf8("0x%1").arg(
						static_cast<qulonglong>(stat.m_CodeLocation->getAddress()),
						16,
						16,
						static_cast<QChar>('0')
					);
				}
				case 1:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFunctionName();
				}
				case 2:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFileName();
				}
				case 3:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFileLineNum();
				}
				case 4: return formatBigNumber(stat.m_MaxAllocationSize);
				case 5: return formatBigNumber(stat.m_MinAllocationSize);
				case 6: return formatBigNumber(stat.m_AvgAllocationSize);
			}
			break;
		}  // case Qt::DisplayRole

		case SortRole:
		{
			switch (a_Index.column())
			{
				case 0:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getAddress();
				}
				case 1:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFunctionName();
				}
				case 2:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFileName();
				}
				case 3:
				{
					if (stat.m_CodeLocation == nullptr)
					{
						return QString::fromUtf8("<unknown>");
					}
					return stat.m_CodeLocation->getFileLineNum();
				}
				case 4: return stat.m_MaxAllocationSize;
				case 5: return stat.m_MinAllocationSize;
				case 6: return stat.m_AvgAllocationSize;
			}
			break;
		}
	}
	return QVariant();
}





QVariant CodeLocationStatsModel::headerData(int a_Section, Qt::Orientation a_Orientation, int a_Role) const
{
	if ((a_Orientation != Qt::Horizontal) || (a_Role != Qt::DisplayRole))
	{
		return Super::headerData(a_Section, a_Orientation, a_Role);
	}
	switch (a_Section)
	{
		case 0: return tr("Address");
		case 1: return tr("Function");
		case 2: return tr("File");
		case 3: return tr("Line");
		case 4: return tr("Max");
		case 5: return tr("Min");
		case 6: return tr("Avg");
	}
	return QVariant();
}




