// massifparser.h

// Declares the MassifParser class representing the parser for Massif-generated datafiles

#ifndef MASSIFPARSER_H
#define MASSIFPARSER_H





#include <memory>
#include <QObject>





// fwd:
class QIODevice;
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class CodeLocationFactory;
typedef std::shared_ptr<CodeLocationFactory> CodeLocationFactoryPtr;
class CodeLocation;
typedef std::shared_ptr<CodeLocation> CodeLocationPtr;





/** Parses a single Massif-generated output file into a series of Snapshot instances. */
class MassifParser:
	public QObject
{
	typedef QObject Super;
	Q_OBJECT

public:

	/** Creates a new parser bound to the specified project.
	The parser doesn't insert the Snapshots to the project, but needs to bind to existing CodeLocations. */
	explicit MassifParser(ProjectPtr a_Project);

	/** Parses the data coming from the IODevice into snapshots, those are then reported via signals. */
	void parse(QIODevice & a_Device);

signals:

	/** Emitted when a complete new snapshot has been parsed. */
	void newSnapshotParsed(SnapshotPtr a_Snapshot);

	/** Emitted when the time unit is parsed.
	Clients can use this to abort (abortParsing()) if time unit doesn't match the data already present. */
	void parsedTimeUnit(const char * a_TimeUnitString);

	/** Emitted when the command that was used to generate the report is parsed.
	Clients can use this to abort (abortParsing()) if the command doesn't match the data already present. */
	void parsedCommand(const char * a_CommandString);

	/** An error has occurred while parsing.
	The handler may call abortParsing() to abort the current parsing operation. */
	void parseError(quint32 a_LineNum, const char * a_ErrorMessage, const char * a_Line);

public slots:

	/** Aborts current parse operation (in parse() function call).
	Sets the m_ShouldContinueParsing flag to false, the parser will abort upon parsing next line. */
	void abortParsing(void);

protected:

	/** The factory that manages CodeLocationPtr instances. */
	CodeLocationFactoryPtr m_CodeLocationFactory;

	/** The snapshot that is currently being parsed. */
	SnapshotPtr m_CurrentSnapshot;

	/** The last Allocation instance created from a data line in the file. */
	AllocationPtr m_LastAllocation;

	/** The nesting depth of m_LastAllocation.
	Used to find the correct parent for the next Allocation instance, by walking from m_LastAllocation up, if needed. */
	unsigned m_LastAllocationDepth;

	/** Current line being parsed (in parse() method), 1-based. */
	quint32 m_CurrentLine;

	/** If set to false, the parser will abort at the next line.
	Used by abortParsing() to signal that the parsing should be aborted. */
	bool m_ShouldContinueParsing;


	/** Processes a single input line. */
	void processLine(char * a_Line, int a_LineLen);

	/** Processes a single input line that describes an Allocation ("<spaces>n<number>: " lines). */
	void processAllocationLine(const char * a_Line, int a_LineLen);

	/** If m_CurrentSnapshot is empty, creates a new snapshot and assigns it to m_CurrentSnapshot. */
	void createNewSnapshotIfNeeded(void);

	/** If m_CurrentSnapshot is valid, signalizes that it has been parsed and resets it to empty. */
	void endCurrentSnapshot(void);

	/** Creates a new Allocation instance from the specified line, and assigns it into m_LastAllocation.
	Uses m_LastAllocationDepth to find the correct parent for the new Allocation. */
	void createAllocationFromLine(const char * a_Line, int a_LineLen);

	/** Parses the line contents into m_LastAllocation's details.
	Receives the entire Allocation line, including the spaces and "n<int>: " header. */
	void parseAllocationDetails(const char * a_Line, int a_LineLen);

	/** Parses the code location details from the given string.
	The string contains all the Massif's data after the hex address, starting with the colon:
	": cChunk::SetAllData(cSetChunkData&) (Chunk.cpp:313)" */
	void parseCodeLocation(CodeLocationPtr a_Location, const char * a_Line, int a_LineLength);
};





#endif // MASSIFPARSER_H
