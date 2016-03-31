// codelocationfactory.cpp

// Declares the CodeLocationFactory class representing a factory that creates (and caches) CodeLocations





#ifndef CODELOCATIONFACTORY_H
#define CODELOCATIONFACTORY_H





#include <map>
#include <memory>
#include <Qt>





// fwd:
class CodeLocation;
typedef std::shared_ptr<CodeLocation> CodeLocationPtr;





/** An instance of this class manages all the CodeLocation instances for a single project. The point is to
share the same CodeLocationPtr in all Snapshots that reference the same address, thus making it simple
to query a history of allocations for a given CodeLocation. */
class CodeLocationFactory
{
public:
	CodeLocationFactory();

	/** Returns a CodeLocation instance corresponding to the specified address.
	If the address hasn't been seen yet, a new CodeLocation instance is created and a_IsNew is set to true;
	otherwise a cached CodeLocation is used instead and a_IsNew is set to false. */
	CodeLocationPtr getCodeLocation(quint64 a_Address, bool & a_IsNew);

protected:

	/** The type used for storing the CodeLocation instances with a fast address lookup. */
	typedef std::map<quint64, CodeLocationPtr> CodeLocationsMap;


	/** The map of already-known CodeLocation instances. */
	CodeLocationsMap m_CodeLocations;
};





#endif // CODELOCATIONFACTORY_H




