// snapshot.cpp

// Implements the Snapshot class, representing a single snapshot of the heap





#include "globals.h"
#include "snapshot.h"





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




