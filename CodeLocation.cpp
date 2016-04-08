// CodeLocation.cpp

// Implements the CodeLocation class representing a container for a share-able Allocation's code location





#include "Globals.h"
#include "CodeLocation.h"





CodeLocation::CodeLocation(quint64 a_Address):
	m_Address(a_Address),
	m_FileLineNum(0)
{
}




