// CodeLocation.h

// Declares the CodeLocation class representing a container for a share-able Allocation's code location





#ifndef CODELOCATION_H
#define CODELOCATION_H





#include <memory>
#include <QString>





/** Represents a single code location in the memoryspace of the examined process,
together with any available debugging information.
Generally returned from a CodeLocationFactory which maintains a map of address -> CodeLocation instances,
so that multiple queries for the same address return the same code location. */
class CodeLocation
{
public:
	CodeLocation(quint64 a_Address);

	void setAddress(quint64 a_Address) { m_Address = a_Address; }
	void setFunctionName(const QString & a_FunctionName) { m_FunctionName = a_FunctionName; }
	void setFileName(const QString & a_FileName) { m_FileName = a_FileName; }
	void setFileLineNum(quint32 a_FileLineNum) { m_FileLineNum = a_FileLineNum; }
	void setHasTriedParsing() { m_HasTriedParsing = true; }

	quint64 getAddress() const { return m_Address; }
	const QString & getFunctionName() const { return m_FunctionName; }
	const QString & getFileName() const { return m_FileName; }
	quint32 getFileLineNum() const { return m_FileLineNum; }
	bool hasTriedParsing() const { return m_HasTriedParsing; }

protected:

	/** The raw address in the memoryspace. */
	quint64 m_Address;

	/** The name of the function.
	Empty if not available, may also be "???" if valgrind fails to identify the location. */
	QString m_FunctionName;

	/** Source code file.
	Empty if not available. */
	QString m_FileName;

	/** Source code line number.
	0 if not available. */
	quint32 m_FileLineNum;

	/** True if the code location details have been parsed from the Massif log.
	False for newly created CodeLocation instance.
	Used by the parser to skip parsing of known locations. */
	bool m_HasTriedParsing;
};

typedef std::shared_ptr<CodeLocation> CodeLocationPtr;





#endif // CODELOCATION_H




