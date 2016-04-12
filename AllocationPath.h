// AllocationPath.h

// Declares the AllocationPath class representing a path in the Allocation tree





#ifndef ALLOCATIONPATH_H
#define ALLOCATIONPATH_H





#include <vector>





// fwd:
class CodeLocation;





/** Represents a path in the allocation tree of any of the project's snapshots.
CodeLocation pointers are used to identify the path segments.
Note that not every snapshot needs to contain all the possible paths. */
class AllocationPath
{
public:
	/** Creates a new path instance corresponding to the root allocation. */
	AllocationPath();

	/** Creates a new path instance corresponding to a child of the root allocation. */
	AllocationPath(CodeLocation * a_CodeLocation);

	/** Move-constructor. */
	AllocationPath(AllocationPath && a_Src);

	/** Copy-constructor. */
	AllocationPath(const AllocationPath & a_Src);

	/** Comparison to other instances. */
	bool operator ==(const AllocationPath & a_Other);

	/** Create a new path representing a child of this instance with the specified code location. */
	AllocationPath makeChild(CodeLocation * a_ChildCodeLocation) const;

	/** Creates a new path representing the parent of this instance (or root, for root allocation). */
	AllocationPath makeParent() const;

	/** Returns the individual segments of the path. */
	const std::vector<CodeLocation *> getSegments() const { return m_Segments; }

protected:
	/** The path to the allocation, using CodeLocation pointers from the root allocation.
	[0] is the code location of the root allocation's child, etc. */
	std::vector<CodeLocation *> m_Segments;
};





#endif // ALLOCATIONPATH_H




