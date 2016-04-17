// HistoryModel.h

// Declares the HistoryModel class representing a Qt model for displaying the project history in a view / graph





#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H





#include <memory>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QColor>
#include "AllocationPath.h"
#include "AllocationStats.h"





// fwd:
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class Allocation;





/** Basic model that provides the raw data for the sorted model. */
class HistoryModel:
	public QAbstractTableModel
{
	typedef QAbstractTableModel Super;

	Q_OBJECT

public:

	/* Column indices. */
	enum
	{
		colFunctionName = 0,
		colMinAllocationSize = 1,
		colMaxAllocationSize = 2,
		colAvgAllocationSize = 3,
		colDiffAllocationSize = 4,
		colHierarchy = 5,
		colPosition = 6,

		colMaxPlusOne,
		colMax = colMaxPlusOne - 1,
	};

	// fwd:
	struct GraphedAllocationPath;
	typedef std::shared_ptr<GraphedAllocationPath> GraphedAllocationPathPtr;


	/** Represents a single allocation path that has been selected for graphing. */
	struct GraphedAllocationPath
	{
		/** The allocation path represented by this instance. */
		AllocationPath m_AllocationPath;

		/** The stats for this path. */
		AllocationStats m_Stats;

		/** Color of this path used for the graph. */
		QColor m_Color;

		/** Creates a new instance of this class and fills it with the specified data. */
		GraphedAllocationPath(
			const AllocationPath & a_AllocationPath,
			const AllocationStats & a_Stats
		);
	};


	explicit HistoryModel(ProjectPtr a_Project);

	/** Returns all the allocation paths that are to be graphed. */
	const std::vector<GraphedAllocationPathPtr> & getGraphedAllocationPaths() const { return m_GraphedAllocationPaths; }

signals:

	/** Emitted after the model has expanded or collapsed a path, or a path's position changed. */
	void modelDataChanged();

public slots:

	/** Resets the entire model.
	Called after the underlying project changes so much that incremental changes are no longer easy. */
	void resetModel();

	/** Called by the treeview when the item is expanded and thus its children should be graphed. */
	void expandItem(const QModelIndex & a_Item);

	/** Called by the treeview when the item is collapsed and thus its children should not be graphed anymore. */
	void collapseItem(const QModelIndex & a_Item);

	void expandItem(int a_Index);

protected:

	/** The project for which the history is being modelled. */
	ProjectPtr m_Project;

	/** The allocation paths that are to be graphed, together with their color and stats. */
	std::vector<GraphedAllocationPathPtr> m_GraphedAllocationPaths;


	// QAbstractItemModel overrides:
	virtual int rowCount(const QModelIndex & a_Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex & a_Parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & a_Index, int a_Role) const override;
	virtual QVariant headerData(int a_Section, Qt::Orientation a_Orientation, int a_Role = Qt::DisplayRole) const override;
};





#endif // HISTORYMODEL_H
