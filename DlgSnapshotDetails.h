// DlgSnapshotDetails.h

// Declares the DlgSnapshotDetails class representing the UI dialog to display details about a single snapshot





#ifndef DLGSNAPSHOTDETAILS_H
#define DLGSNAPSHOTDETAILS_H





#include <memory>
#include <QMainWindow>
#include <QTreeWidgetItem>





// fwd:
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;





namespace Ui
{
	class DlgSnapshotDetails;
}





class DlgSnapshotDetails:
	public QMainWindow
{
	typedef QMainWindow Super;

	Q_OBJECT

public:

	explicit DlgSnapshotDetails(QWidget * a_Parent = nullptr);

	/** Loads data from the specified snapshot and shows the window. */
	void show(SnapshotPtr a_Snapshot);

private slots:

	/** A tree item has expanded, make sure its children have correct child-indicators. */
	void onTreeItemExpanded(QTreeWidgetItem * a_TreeItem);

private:

	std::shared_ptr<Ui::DlgSnapshotDetails> m_UI;

	/** The snapshot being displayed. */
	SnapshotPtr m_Snapshot;


	/** Updates all the items in the allocations tree widget. */
	void updateAllocationsTree();

	/** Inserts immediate children of the specified allocation into the specified tree item. */
	void insertChildAllocations(QTreeWidgetItem * a_TreeItem, Allocation * a_Allocation);

	/** For each child of the specified tree item, makes sure any applicable grandchildren are added.
	This makes the child indicators correct for all children of a_TreeItem. */
	void ensureCorrectChildIndicatorsForChildren(QTreeWidgetItem * a_TreeItem);
};





#endif // DLGSNAPSHOTDETAILS_H




