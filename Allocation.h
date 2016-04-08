// Allocation.h

// Declares the Allocation class representing a single entry in the heap allocation tree

#ifndef ALLOCATION_H
#define ALLOCATION_H





#include <memory>
#include <vector>
#include <QString>
#include "CodeLocation.h"





// fwd:
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;
typedef std::weak_ptr  <Allocation> AllocationWeakPtr;
typedef std::vector<AllocationPtr> AllocationPtrs;





/** Contains a single allocation point. */
class Allocation:
	public std::enable_shared_from_this<Allocation>
{
public:
	enum Type
	{
		atRoot,            ///< The root Allocation entry, corresponds to the while program heap usage
		atRegular,         ///< Regular Allocation entry
		atBelowThreshold,  ///< The entry size is below Massif's threshold, no more detailed data available
		atUnknown,         ///< Unknown entry, hasn't been filled yet (default type)
	};

	/** Creates a new instance that has no parent set. */
	Allocation();

	/** Creates a new instance that is attached to the specified parent. */
	Allocation(AllocationWeakPtr a_Parent);

	/** Creates a new Allocation instance that is a child of this instance. */
	AllocationPtr addChild();

	/** Returns the parent Allocation of this instance.
	Returns nullptr if this is the top-level instance. */
	AllocationPtr getParent();

	void setAllocationSize(quint64 a_AllocationSize) { m_AllocationSize = a_AllocationSize; }
	void setCodeLocation(CodeLocationPtr a_CodeLocation) { m_CodeLocation = a_CodeLocation; }
	void setType(Type a_Type) { m_Type = a_Type; }

	quint64         getAllocationSize() const { return m_AllocationSize; }
	quint64         getAddress()        const { return m_CodeLocation->getAddress(); }
	const QString & getFunctionName()   const { return m_CodeLocation->getFunctionName(); }
	const QString & getFileName()       const { return m_CodeLocation->getFileName(); }
	quint32         getFileLineNum()    const { return m_CodeLocation->getFileLineNum(); }
	Type            getType()           const { return m_Type; }
	CodeLocationPtr getCodeLocation()   const { return m_CodeLocation; }

	/** Returns true if the allocation has any children. */
	bool hasChildren() const { return m_Children.empty(); }

	const AllocationPtrs & getChildren() const { return m_Children; }

	/** Sorts the children (recursively) by their AllocationSize. */
	void sortBySize();

	/** Returns the immediate child that has the specified CodeLocation.
	Returns nullptr if no such child. */
	AllocationPtr findCodeLocationChild(CodeLocationPtr a_CodeLocation);

	/** Returns the child (any depth descendant) that has the specified CodeLocation.
	Returns nullptr if no such child. */
	AllocationPtr recursiveFindCodeLocationChild(CodeLocation * a_CodeLocation);

protected:

	/** The parent allocation, if any. */
	AllocationWeakPtr m_Parent;

	/** Number of bytes of allocated memory. */
	quint64 m_AllocationSize;

	/** Stack entry code location of where the allocation was created.
	May be nullptr for unknown locations. */
	CodeLocationPtr m_CodeLocation;

	/** Type of the entry - root, regular, belowthreshold, unknown. */
	Type m_Type;

	/** Child allocations - where from has this location been called. */
	AllocationPtrs m_Children;
};






#endif // ALLOCATION_H
