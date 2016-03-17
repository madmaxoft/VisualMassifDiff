// project.h

// Declares the project class representing a project - collection of snapshots.

#ifndef PROJECT_H
#define PROJECT_H





#include <memory>
#include <list>
#include <Qt>





// fwd:
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;





class Project
{
public:
	Project();

	/** Adds the specified snapshot to the project. */
	void addSnapshot(SnapshotPtr a_Snapshot);

	/** Returns the number of snapshots contained in the project. */
	size_t getNumSnapshots(void) const;

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

protected:

	/** The type used for the internal collection of snapshots. */
	typedef std::list<SnapshotPtr> SnapshotPtrs;


	/** The snapshots contained within the project. */
	SnapshotPtrs m_Snapshots;

	/** The command used to create the Massif files for this project.
	Used to check when adding new files that they come from the same Massif settings. */
	std::string m_Command;

	/** The time unit used when creating the Massif files for this project.
	Used to check when adding new files that they come from the same Massif settings. */
	std::string m_TimeUnit;

};





#endif // PROJECT_H



