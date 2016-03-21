// allocation.h

// Declares the Allocation class representing a single entry in the heap allocation tree





#include "globals.h"
#include "allocation.h"





Allocation::Allocation():
	m_AllocationSize(0),
	m_Address(0),
	m_FileLineNum(0),
	m_Type(atUnknown)
{
}





Allocation::Allocation(AllocationWeakPtr a_Parent):
	m_Parent(a_Parent),
	m_AllocationSize(0),
	m_Address(0),
	m_FileLineNum(0),
	m_Type(atUnknown)
{
}




AllocationPtr Allocation::addChild()
{
	auto res = std::make_shared<Allocation>(shared_from_this());
	m_Children.push_back(res);
	return res;
}




AllocationPtr Allocation::getParent()
{
	return m_Parent.lock();
}





void Allocation::sortBySize()
{
	// Sort the immediate children:
	std::sort(m_Children.begin(), m_Children.end(), [](AllocationPtr & a_First, AllocationPtr & a_Second)
		{
			return (a_First->m_AllocationSize > a_Second->m_AllocationSize);
		}
	);

	// Recursively sort grandchildren:
	for (auto & a: m_Children)
	{
		a->sortBySize();
	}
}




