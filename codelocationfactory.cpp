// codelocationfactory.cpp

// Implements the CodeLocationFactory class representing a factory that creates (and caches) CodeLocations





#include "globals.h"
#include "codelocationfactory.h"
#include "codelocation.h"





CodeLocationFactory::CodeLocationFactory()
{
}





CodeLocationPtr CodeLocationFactory::getCodeLocation(quint64 a_Address, bool & a_IsNew)
{
	auto itr = m_CodeLocations.find(a_Address);
	if (itr != m_CodeLocations.end())
	{
		a_IsNew = false;
		return itr->second;
	}
	auto loc = std::make_shared<CodeLocation>(a_Address);
	m_CodeLocations.insert(itr, std::make_pair(a_Address, loc));
	a_IsNew = true;
	return loc;
}




