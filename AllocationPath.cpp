// AllocationPath.cpp

// Implements the AllocationPath class representing a path in the Allocation tree





#include "Globals.h"
#include <cstddef>  // for size_t
#include "AllocationPath.h"





AllocationPath::AllocationPath()
{
}





AllocationPath::AllocationPath(CodeLocation * a_CodeLocation)
{
	m_Segments.push_back(a_CodeLocation);
}





AllocationPath::AllocationPath(AllocationPath && a_Src):
	m_Segments(std::move(a_Src.m_Segments))
{
}





AllocationPath::AllocationPath(const AllocationPath & a_Src):
	m_Segments(a_Src.m_Segments)
{
}





bool AllocationPath::operator ==(const AllocationPath & a_Other)
{
	auto numSegments = m_Segments.size();
	if (numSegments != a_Other.m_Segments.size())
	{
		return false;
	}
	for (size_t i = 0; i < numSegments; ++i)
	{
		if (m_Segments[i] != a_Other.m_Segments[i])
		{
			return false;
		}
	}
	return true;
}





AllocationPath & AllocationPath::operator =(const AllocationPath & a_Src)
{
	if (this == &a_Src)
	{
			return *this;
	}
	m_Segments = a_Src.m_Segments;
	return *this;
}





AllocationPath AllocationPath::makeChild(CodeLocation * a_ChildCodeLocation) const
{
	AllocationPath res(*this);
	res.m_Segments.push_back(a_ChildCodeLocation);
	return res;
}






AllocationPath AllocationPath::makeParent() const
{
	AllocationPath res(*this);
	if (!res.m_Segments.empty())
	{
		res.m_Segments.pop_back();
	}
	return res;
}





CodeLocation * AllocationPath::getLeafSegment() const
{
	if (m_Segments.empty())
	{
		return nullptr;
	}
	return m_Segments.back();
}





