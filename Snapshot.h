// Snapshot.h

// Declares the Snapshot class, representing a single snapshot of the heap





#ifndef SNAPSHOT_H
#define SNAPSHOT_H





#include <memory>
#include <unordered_map>
#include <assert.h>
#include <Qt>





// fwd:
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;
class AllocationPath;
class CodeLocation;





/** Contains the information provided by a single Massif snapshot. */
class Snapshot
{
public:
	/** Type used for storing a flat sum of per-CodeLocation allocation size.
	Since the allocation tree may contain the same entry in multiple instances, we need to represent
	the sum of all such entries, in a per-CodeLocation way. */
	typedef std::unordered_map<CodeLocation *, quint64> FlatSums;
	

	/** Creates a new empty snapshot with zero time and sizes. */
	Snapshot();

	void setTimestamp(quint64 a_Timestamp) { m_Timestamp = a_Timestamp; }
	void setHeapSize(quint64 a_HeapSize) { m_HeapSize = a_HeapSize; }
	void setHeapExtraSize(quint64 a_HeapExtraSize) { m_HeapExtraSize = a_HeapExtraSize; }

	/** Sets the root allocation to the specified one.
	Cannot replace an allocation that has already been set (write-once).*/
	void setRootAllocation(AllocationPtr a_RootAllocation);

	quint64 getTimestamp() const { return m_Timestamp; }
	quint64 getHeapSize() const { return m_HeapSize; }
	quint64 getHeapExtraSize() const { return m_HeapExtraSize; }
	quint64 getTotalSize() const { return m_HeapSize + m_HeapExtraSize; }
	AllocationPtr getRootAllocation() { return m_RootAllocation; }

	/** Returns the allocation specified by its full path.
	Returns nullptr if no such allocation in this snapshot. */
	AllocationPtr findAllocation(const AllocationPath & a_Path) const;

	/** Returns true if the snapshot has detailed allocations attached to it. */
	bool hasAllocations() const { return (m_RootAllocation != nullptr); }


	/** Updates the flat sums of allocations.
	Called by the parser after it finishes parsing the allocation tree. */
	void updateFlatSums();
	
	const FlatSums & getFlatSums() const { return m_FlatSums; }
	
protected:

	/** The snapshot's timestamp, in whatever unit Massif used to generate the output (Project::m_TimeUnit) */
	quint64 m_Timestamp;

	/** The total amount of memory allocated by the program. */
	quint64 m_HeapSize;

	/** The amount of heap memory allocated but not used (fragmentation etc.) */
	quint64 m_HeapExtraSize;

	/** The root allocation element, if the snapshot has a detailed report.
	The root element represents all the allocations,
	its children are individual places on the stack which allocated memory, together with their stacktraces. */
	AllocationPtr m_RootAllocation;
	
	/** The sums of all CodeLocations' allocations within this snapshot. */
	FlatSums m_FlatSums;
	
	
	/** Recursively adds the specified allocation's children to m_FlatSums. */
	void addChildrenToFlatSums(const AllocationPtr & a_Allocation);
};

typedef std::shared_ptr<Snapshot> SnapshotPtr;





#endif // SNAPSHOT_H
