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





/** Parses a single Massif-generated output file into a series of Snapshot instances. */
class MassifParser:
	public QObject
{
	typedef QObject Super;
	Q_OBJECT

public:

	explicit MassifParser(void);

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

	/** The snapshot that is currently being parsed. */
	SnapshotPtr m_CurrentSnapshot;

	/** */
	AllocationPtr m_LastAllocation;

	/** Current line being parsed (in parse() method), 1-based. */
	quint32 m_CurrentLine;

	/** If set to false, the parser will abort at the next line.
	Used by abortParsing() to signal that the parsing should be aborted. */
	bool m_ShouldContinueParsing;


	/** Processes a single input line. */
	void processLine(char * a_Line, qint64 a_LineLen);

	/** Processes a single input line that describes an Allocation ("<spaces>n<number>: " lines). */
	void processAllocationLine(const char * a_Line, qint64 a_LineLen);

	/** If m_CurrentSnapshot is empty, creates a new snapshot and assigns it to m_CurrentSnapshot. */
	void createNewSnapshotIfNeeded(void);
};





#endif // MASSIFPARSER_H
