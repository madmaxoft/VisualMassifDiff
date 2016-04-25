// Project.h

// Declares the project class representing a project - collection of snapshots.





#ifndef PROJECT_H
#define PROJECT_H





#include <memory>
#include <list>
#include <QObject>
#include "AllocationStats.h"





// fwd:
class QIODevice;
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
typedef std::list<SnapshotPtr> SnapshotPtrs;
class CodeLocationFactory;
typedef std::shared_ptr<CodeLocationFactory> CodeLocationFactoryPtr;
class CodeLocationStats;
typedef std::shared_ptr<CodeLocationStats> CodeLocationStatsPtr;
class AllocationPath;





class Project:
	public QObject
{
	Q_OBJECT

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

	void setCommand(const std::string & a_Command);
	void setTimeUnit(const std::string & a_TimeUnit);

	const SnapshotPtrs & getSnapshots() const { return m_Snapshots; }

	CodeLocationFactoryPtr getCodeLocationFactory() { return m_CodeLocationFactory; }
	const CodeLocationFactoryPtr getCodeLocationFactory() const { return m_CodeLocationFactory; }

	/** Returns the code location stats calculator.
	The instance keeps track of min, max and avg allocation sizes of each code location. */
	CodeLocationStatsPtr getCodeLocationStats() { return m_CodeLocationStats; }

	/** Returns the project-wide stats for the specified allocation path. */
	AllocationStats getStatsForAllocationPath(const AllocationPath & a_AllocationPath);

	/** Returns all paths across all snapshots that are immediate children to the specified path. */
	std::vector<AllocationPath> getAllAllocationPathsImmediateChildren(const AllocationPath & a_Path);

	/** Saves the entire project data into the specified file.
	Throws an exception on failure. */
	void save(const QString & a_FileName);

	/** Saves the entire project data into the specified IO device.
	Throws an exception on failure. */
	void save(QIODevice & a_Device);

	/** Loads the entire project data from the specified file.
	Throws an exception on failure. */
	bool load(const QString & a_FileName);

	/** Loads the entire project data from the specified IO device.
	Throws an exception on failure. */
	bool load(QIODevice & a_Device);

	/** Returns the filename used for the last save or load operation.
	Empty if not saved or loaded yet. */
	const QString & getFileName() const { return m_FileName; }

	/** Returns true if the project has changed since it was last saved.
	Upon creation, the project hasn't been saved yet but the flag is still set to true
	so that a newly created project doesn't prompt for saving. */
	bool hasChangedSinceSave() const { return m_HasChangedSinceSave; }

signals:

	/** Emitted just before a snapshot is added to the project. */
	void addingSnapshot(SnapshotPtr a_Snapshot);

	/** Emitted just after a snapshot is added to the project. */
	void addedSnapshot(SnapshotPtr a_Snapshot);

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

	/** The filename used to load / save the project last. */
	QString m_FileName;

	/** True iff the project has changed since it was last saved. */
	bool m_HasChangedSinceSave;
};





/** The magic bytes at the start of a project file used to determine if it is a project file.
Shared between the ProjectSaver and ProjectLoader classes. */
static const char g_ProjectFileMagic[] = "VisualMassifDiff project file\n";





#endif // PROJECT_H




