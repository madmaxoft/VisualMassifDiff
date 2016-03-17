// snapshot.cpp

// Declares the Snapshot class, representing a single snapshot of the heap

#ifndef SNAPSHOT_H
#define SNAPSHOT_H





#include <memory>
#include <assert.h>
#include <Qt>





// fwd:
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;





/** Contains the information provided by a single Massif snapshot. */
class Snapshot
{
public:

	/** Creates a new empty snapshot with zero time and sizes. */
	Snapshot();

	void setTimestamp(quint64 a_Timestamp) { m_Timestamp = a_Timestamp; }
	void setHeapSize(quint64 a_HeapSize) { m_HeapSize = a_HeapSize; }
	void setHeapExtraSize(quint64 a_HeapExtraSize) { m_HeapExtraSize = a_HeapExtraSize; }

	/** Sets the root allocation to the specified one.
	Cannot replace an allocation that has already been set (write-once).*/
	void setRootAllocation(AllocationPtr a_RootAllocation);

	quint64 getTimestamp(void) const { return m_Timestamp; }
	quint64 getHeapSize(void) const { return m_HeapSize; }
	quint64 getHeapExtraSize(void) const { return m_HeapExtraSize; }
	AllocationPtr getRootAllocation(void) { return m_RootAllocation; }

	/** Returns true if the snapshot has detailed allocations attached to it. */
	bool hasAllocations(void) const { return (m_RootAllocation != nullptr); }


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
};

typedef std::shared_ptr<Snapshot> SnapshotPtr;





#endif // SNAPSHOT_H
