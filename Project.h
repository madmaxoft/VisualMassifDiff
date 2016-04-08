// Project.h

// Declares the project class representing a project - collection of snapshots.





#ifndef PROJECT_H
#define PROJECT_H





#include <memory>
#include <list>
#include <Qt>





// fwd:
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
typedef std::list<SnapshotPtr> SnapshotPtrs;
class CodeLocationFactory;
typedef std::shared_ptr<CodeLocationFactory> CodeLocationFactoryPtr;
class CodeLocationStats;
typedef std::shared_ptr<CodeLocationStats> CodeLocationStatsPtr;





class Project
{
public:
	Project();

	/** Adds the specified snapshot to the project. */
	void addSnapshot(SnapshotPtr a_Snapshot);

	/** Returns the number of snapshots contained in the project. */
	size_t getNumSnapshots(void) const;

	/** Returns the snapshot with the specified timestamp, or nullptr if no such timestamp found. */
	SnapshotPtr getSnapshotAtTimestamp(quint64 a_Timestamp);

	/** Returns true if the project already contains a snapshot with the specified timestamp. */
	bool hasSnapshotForTimestamp(quint64 a_Timestamp) const;

	/** Checks the command used to create the Massif files for this project.
	If there is no snapshot, sets the command and returns true.
	If there are snapshots already, compares the stored command with the parameter and returns true if they are the same. */
	bool checkAndSetCommand(const char * a_Command);

	/** Checks the time unit used to create the Massif files for this project.
	If there is no snapshot, sets the command and returns true.
	If there are snapshots already, compares the stored command with the parameter and returns true if they are the same. */
	bool checkAndSetTimeUnit(const char * a_TimeUnit);

	std::string getCommand(void) const { return m_Command; }
	std::string getTimeUnit(void) const { return m_TimeUnit; }

	const SnapshotPtrs & getSnapshots() const { return m_Snapshots; }

	CodeLocationFactoryPtr getCodeLocationFactory() { return m_CodeLocationFactory; }

	/** Returns the code location stats calculator.
	The instance keeps track of min, max and avg allocation sizes of each code location. */
	CodeLocationStatsPtr getCodeLocationStats() { return m_CodeLocationStats; }

protected:

	/** The snapshots contained within the project. */
	SnapshotPtrs m_Snapshots;

	/** The factory that manages all CodeLocation instances used by the project's snapshots. */
	CodeLocationFactoryPtr m_CodeLocationFactory;

	/** The command used to create the Massif files for this project.
	Used to check when adding new files that they come from the same Massif settings. */
	std::string m_Command;

	/** The time unit used when creating the Massif files for this project.
	Used to check when adding new files that they come from the same Massif settings. */
	std::string m_TimeUnit;

	/** The code location stats calculator.
	Keeps track of min, max and avg allocation sizes of each code location. */
	CodeLocationStatsPtr m_CodeLocationStats;

};





#endif // PROJECT_H




