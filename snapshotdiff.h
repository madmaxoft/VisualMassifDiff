// snapshotdiff.h

// Implements the SnapshotDiff class representing a diff between two snapshots





#ifndef SNAPSHOTDIFF_H
#define SNAPSHOTDIFF_H





#include <memory>
#include <vector>
#include <Qt>





// fwd:
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;
class DiffItem;
typedef std::shared_ptr<DiffItem> DiffItemPtr;
typedef std::vector<DiffItemPtr> DiffItemPtrs;
class CodeLocation;
typedef std::shared_ptr<CodeLocation> CodeLocationPtr;





/** Represents a single item of difference between two corresponding allocations in the two snapshots.
Is arranged into a tree structure with a single DiffItem as the root.
The tree corresponds to the merged trees of the two snapshots' Allocations. */
class DiffItem
{
public:
	/** Creates a new instance representing a diff between the specified allocations. */
	DiffItem(DiffItem * a_Parent, AllocationPtr a_First, AllocationPtr a_Second);

	/** Adds a new child representing the diff between the specified allocations.
	Inserts the child to m_Children and sets its parent to self. */
	DiffItemPtr addChild(AllocationPtr a_First, AllocationPtr a_Second);

	DiffItem * getParent() const { return m_Parent; }
	AllocationPtr getFirst() const { return m_First; }
	AllocationPtr getSecond() const { return m_Second; }
	const DiffItemPtrs & getChildren() const { return m_Children; }

	/** Returns the code location representing this item.
	The code location is the same for m_First and m_Second, unless one of them is nullptr. */
	CodeLocationPtr getCodeLocation() const;

	/** Returns the difference in heap size between the first and second allocation.
	Handles nullptr properly - as a new or expired allocation. */
	qint64 getHeapSizeDiff() const;

	/** Returns the child at the specifid index, or nullptr if index out of bounds. */
	DiffItemPtr child(int a_Index) const;

	/** Returns the index of the specified child the m_Children, or -1 if not found. */
	int indexOfChild(DiffItem * a_Child) const;

	/** Recursively removes children that have zero difference and no children. */
	void pruneZeroDiffChildren();

protected:

	/** The parent DiffItem of this instance, or nullptr if this is the root. */
	DiffItem * m_Parent;

	/** The allocation from the first snapshot. */
	AllocationPtr m_First;

	/** The allocation from the second snapshot. */
	AllocationPtr m_Second;

	/** All DiffItem children of this instance. */
	DiffItemPtrs m_Children;
};





class SnapshotDiff
{
public:
	/** Creates a diff between the two specified snapshots. */
	SnapshotDiff(SnapshotPtr a_FirstSnapshot, SnapshotPtr a_SecondSnapshot);

	SnapshotPtr getFirstSnapshot() const { return m_FirstSnapshot; }
	SnapshotPtr getSecondSnapshot() const { return m_SecondSnapshot; }
	DiffItemPtr getRootDiffItem() const { return m_Root; }

protected:
	/** The root of the diff tree. */
	DiffItemPtr m_Root;

	/** The first snapshot to be diff-ed. */
	SnapshotPtr m_FirstSnapshot;

	/** The second snapshot to be diff-ed. */
	SnapshotPtr m_SecondSnapshot;


	/** Recursively matches the children Allocations of the specified DiffItem's Allocations. */
	void matchChildren(DiffItemPtr a_DiffItem);
};





#endif // SNAPSHOTDIFF_H




