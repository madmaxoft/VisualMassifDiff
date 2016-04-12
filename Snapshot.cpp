// Snapshot.cpp

// Implements the Snapshot class, representing a single snapshot of the heap





#include "Globals.h"
#include "Snapshot.h"
#include "AllocationPath.h"
#include "Allocation.h"





Snapshot::Snapshot():
	m_Timestamp(0),
	m_HeapSize(0),
	m_HeapExtraSize(0)
{
}





void Snapshot::setRootAllocation(AllocationPtr a_RootAllocation)
{
	assert(m_RootAllocation == nullptr);  // Only allow a single assignment to the root allocation

	m_RootAllocation = a_RootAllocation;
}





AllocationPtr Snapshot::findAllocation(const AllocationPath & a_Path) const
{
	auto a = m_RootAllocation;
	const auto & segments = a_Path.getSegments();
	for (const auto & s: segments)
	{
		if (a == nullptr)
		{
			return nullptr;
		}
		a = a->findCodeLocationChild(s);
	}
	return a;
}




