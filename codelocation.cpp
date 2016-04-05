// codelocation.cpp

// Implements the CodeLocation class representing a container for a share-able Allocation's code location





#include "globals.h"
#include "codelocation.h"





CodeLocation::CodeLocation(quint64 a_Address):
	m_Address(a_Address),
	m_FileLineNum(0)
{
}




