// CodeLocationStats.h

// Declares the CodeLocationStats class that calculates project-wide statistics on CodeLocations





#ifndef CODELOCATIONSTATS_H
#define CODELOCATIONSTATS_H





#include <map>
#include <vector>
#include <memory>
#include <QObject>





// fwd:
class CodeLocation;
class Project;
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;
class Allocation;






class CodeLocationStats:
	public QObject
{
	Q_OBJECT

public:

	/** Represents the statistics for a single CodeLocation. */
	struct Stats
	{
		quint64 m_MaxAllocationSize;
		quint64 m_MinAllocationSize;
		double m_AvgAllocationSize;
		CodeLocation * m_CodeLocation;

		static decltype(m_MaxAllocationSize) unassignedMax;
		static decltype(m_MinAllocationSize) unassignedMin;

		Stats();
	};


	/** Creates a new instance bound to the specified project. */
	CodeLocationStats(Project * a_Project);

	/** Retrieves the stats for the specified CodeLocation.
	Returns nullptr if CodeLocation is not found. */
	Stats * findStats(CodeLocation * a_CodeLocation);

	/** Returns a copy of all the stats, transformed into a vector. */
	std::vector<Stats> getAllStats();

public slots:

	/** Emitted by the underlying project just before a new snapshot is added. */
	void onProjectAddingSnapshot(SnapshotPtr a_Snapshot);

protected:

	/** The project for which the stats are being recorded. */
	Project * m_Project;

	/** Stats, in a map for fast CodeLocation-based retrieval. */
	std::map<CodeLocation *, Stats> m_Stats;


	/** Recursively updates the stats when adding a snapshot to the project.
	The specified allocation and all its (recursive) children are updated.
	The snapshot is not yet added to the project. */
	void updateStatsByAllocation(Allocation * a_Allocation);
};





#endif // CODELOCATIONSTATS_H
