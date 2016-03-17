// allocation.h

// Declares the Allocation class representing a single entry in the heap allocation tree

#ifndef ALLOCATION_H
#define ALLOCATION_H





#include <memory>
#include <QString>





// fwd:
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;
typedef std::weak_ptr  <Allocation> AllocationWeakPtr;





/** Contains a single allocation point. */
class Allocation
{
public:
	Allocation();
	Allocation(
		const QString & a_StackEntry,
		uint64_t a_AllocationSize,
		AllocationWeakPtr a_Parent
	);

protected:
	/** The location of the code.
	Uniquely identifies the location at the time of allocation.
	Usually function name plus filename plus line. */
	QString m_StackEntry;

	/** Number of bytes of allocated memory. */
	uint64_t m_AllocationSize;

	/** The parent allocation, if any. */
	AllocationWeakPtr m_Parent;
};






#endif // ALLOCATION_H
