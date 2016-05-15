// SnapshotModel.cpp

// Implements the SnapshotModel class representing the model for displaying project's snapshots





#ifndef SNAPSHOTMODEL_H
#define SNAPSHOTMODEL_H





#include <memory>
#include <QStandardItemModel>





// fwd:
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;





class SnapshotModel:
	public QStandardItemModel
{
	typedef QStandardItemModel Super;

	Q_OBJECT


public:
	/** Column indices. */
	enum
	{
		colTimestamp = 0,
		colHeapSize = 1,
		colHeapExtraSize = 2,

		colMaxPlusOne,
		colMax = colMaxPlusOne - 1,
	};

	enum
	{
		/** Data role used for sorting (rather than sorting on the DisplayRole) */
		SortRole = Qt::UserRole + 1,

		/** Data role used to store snapshot's timestamp in the QModelIndex. */
		SnapshotTimestampRole = Qt::UserRole + 2,
	};


	/** Creates a new model for the specified project. */
	explicit SnapshotModel(ProjectPtr a_Project);

	/** Returns the snapshot represented by the specified item. */
	SnapshotPtr getItemSnapshot(const QModelIndex & a_Index) const;

	virtual Qt::ItemFlags flags(const QModelIndex & a_Index) const override;

signals:


protected slots:

	/** Called from m_Project after a new snapshot has been added to the project. */
	void onProjectAddedSnapshot(SnapshotPtr a_Snapshot);


protected:

	/** The project whose snapshots are modelled. */
	ProjectPtr m_Project;

	/** Icon displayed in the tree view if the snapshot has detailed allocations attached to it.  */
	QIcon m_IcoAllocations;


	/** Sets the model's columns. Called from the constructor. */
	void initColumns();

	/** Adds the specified snapshot to the model. */
	void addSnapshot(SnapshotPtr a_Snapshot);
};





#endif // SNAPSHOTMODEL_H




