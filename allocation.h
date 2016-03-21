// allocation.h

// Declares the Allocation class representing a single entry in the heap allocation tree

#ifndef ALLOCATION_H
#define ALLOCATION_H





#include <memory>
#include <vector>
#include <QString>





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
	void setAddress(quint64 a_Address) { m_Address = a_Address; }
	void setFunctionName(const QString & a_FunctionName) { m_FunctionName = a_FunctionName; }
	void setFileName(const QString & a_FileName) { m_FileName = a_FileName; }
	void setFileLineNum(quint32 a_FileLineNum) { m_FileLineNum = a_FileLineNum; }
	void setType(Type a_Type) { m_Type = a_Type; }

	quint64 getAllocationSize() const { return m_AllocationSize; }
	quint64 getAddress() const { return m_Address; }
	const QString & getFunctionName() const { return m_FunctionName; }
	const QString & getFileName() const { return m_FileName; }
	quint32 getFileLineNum() const { return m_FileLineNum; }
	Type getType() const { return m_Type; }

	/** Returns true if the allocation has any children. */
	bool hasChildren() const { return m_Children.empty(); }

	const AllocationPtrs & getChildren() const { return m_Children; }

	/** Sorts the children (recursively) by their AllocationSize. */
	void sortBySize();

protected:

	/** The parent allocation, if any. */
	AllocationWeakPtr m_Parent;

	/** Number of bytes of allocated memory. */
	quint64 m_AllocationSize;

	/** The address of the allocation stack entry. */
	quint64 m_Address;

	/** The name of the function representing this allocation stack entry.
	Empty if not available, may also be "???" if valgrind fails to identify the location. */
	QString m_FunctionName;

	/** Source code file representing this allocation stack entry.
	Empty if not available. */
	QString m_FileName;

	/** Source code line number representing this allocation stack entry.
	0 if not available. */
	quint32 m_FileLineNum;

	/** Type of the entry - root, regular, belowthreshold, unknown. */
	Type m_Type;

	/** Child allocations - where from has this location been called. */
	AllocationPtrs m_Children;
};






#endif // ALLOCATION_H
