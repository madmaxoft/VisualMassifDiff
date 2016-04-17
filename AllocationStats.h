// AllocationStats.h

// Declares the AllocationStats class representing stats for a single allocation across the entire project





#ifndef ALLOCATIONSTATS_H
#define ALLOCATIONSTATS_H





#include "AllocationPath.h"





// fwd:
class Allocation;





class AllocationStats
{
public:

	/** The maximum size, across the entire project. */
	quint64 m_MaxAllocationSize;

	/** The minimum allocation size, across the entire project. */
	quint64 m_MinAllocationSize;

	/** The minimum allocation size, excluding zero sizes, across the entire project. */
	quint64 m_MinNonzeroAllocationSize;

	/** The average allocation size, across the entire project. */
	double m_AvgAllocationSize;

	/** The allocation path to which the stats belong. */
	AllocationPath m_AllocationPath;


	/** Creates a new instance with the specified allocation path set. */
	AllocationStats(const AllocationPath & a_AllocationPath = AllocationPath()):
		m_MaxAllocationSize(std::numeric_limits<decltype(m_MinAllocationSize)>::min()),
		m_MinAllocationSize(std::numeric_limits<decltype(m_MinAllocationSize)>::max()),
		m_MinNonzeroAllocationSize(std::numeric_limits<decltype(m_MinAllocationSize)>::max()),
		m_AvgAllocationSize(0),
		m_AllocationPath(a_AllocationPath)
	{
	}

	/** Processes a new value into the statistics - checks it against min and max, and updates the avg.
	Note that the avg value contains a temporary sum, clients need to call finishProcessing() to finalize the value. */
	inline void processNewValue(quint64 a_AllocationSize)
	{
		if (a_AllocationSize < m_MinAllocationSize)
		{
			m_MinAllocationSize = a_AllocationSize;
		}
		if ((a_AllocationSize > 0) && a_AllocationSize)
		{
			m_MinNonzeroAllocationSize = a_AllocationSize;
		}
		if (a_AllocationSize > m_MaxAllocationSize)
		{
			m_MaxAllocationSize = a_AllocationSize;
		}
		m_AvgAllocationSize += a_AllocationSize;
	}

	/** Finishes processing that has been done through multiple calls to processNewValue.
	a_TotalNumSnapshots is the number of snapshots that has been processed.
	Finalizes the Avg value. */
	inline void finishProcessing(size_t a_TotalNumSnapshots)
	{
		m_AvgAllocationSize = m_AvgAllocationSize / a_TotalNumSnapshots;
	}
};





#endif // ALLOCATIONSTATS_H
