// Project.cpp

// Implements the project class representing a project - collection of snapshots.





#include "Globals.h"
#include "Project.h"
#include <unordered_set>
#include "Snapshot.h"
#include "CodeLocationFactory.h"
#include "CodeLocationStats.h"
#include "AllocationPath.h"
#include "Allocation.h"





Project::Project():
	m_CodeLocationFactory(std::make_shared<CodeLocationFactory>()),
	m_CodeLocationStats(std::make_shared<CodeLocationStats>(this))
{
}





void Project::addSnapshot(SnapshotPtr a_Snapshot)
{
	// Emit the signal about the change to all listeners:
	emit addingSnapshot(a_Snapshot);

	// Insert the snapshot into the internal collection, so that it is sorted:
	auto timestamp = a_Snapshot->getTimestamp();
	for (auto itr = m_Snapshots.begin(), end = m_Snapshots.end(); itr != end; ++itr)
	{
		if ((*itr)->getTimestamp() > timestamp)
		{
			m_Snapshots.insert(itr, a_Snapshot);
			emit addedSnapshot(a_Snapshot);
			return;
		}
	}
	// Timestamp is larger than anything in the collection, append to back:
	m_Snapshots.push_back(a_Snapshot);
	emit addedSnapshot(a_Snapshot);
}





size_t Project::getNumSnapshots() const
{
	return m_Snapshots.size();
}




SnapshotPtr Project::getSnapshotAtTimestamp(quint64 a_Timestamp)
{
	for (auto itr = m_Snapshots.begin(), end = m_Snapshots.end(); itr != end; ++itr)
	{
		auto timestamp = (*itr)->getTimestamp();
		if (timestamp == a_Timestamp)
		{
			return *itr;
		}
		else if (timestamp > a_Timestamp)
		{
			return nullptr;
		}
	}
	return nullptr;
}





bool Project::hasSnapshotForTimestamp(quint64 a_Timestamp) const
{
	for (auto itr = m_Snapshots.begin(), end = m_Snapshots.end(); itr != end; ++itr)
	{
		auto timestamp = (*itr)->getTimestamp();
		if (timestamp >= a_Timestamp)
		{
			return (timestamp == a_Timestamp);
		}
	}
	return false;
}





bool Project::checkAndSetCommand(const char * a_Command)
{
	// If there is no snapshot yet, accept any command:
	if (m_Snapshots.empty())
	{
		m_Command.assign(a_Command);
		return true;
	}

	// Snapshots are already present, check the stored command:
	return (m_Command.compare(a_Command) == 0);
}






bool Project::checkAndSetTimeUnit(const char * a_TimeUnit)
{
	// If there is no snapshot yet, accept any time unit:
	if (m_Snapshots.empty())
	{
		m_TimeUnit.assign(a_TimeUnit);
		return true;
	}

	// Snapshots are already present, check the stored time unit:
	return (m_TimeUnit.compare(a_TimeUnit) == 0);
}





AllocationStats Project::getStatsForAllocationPath(const AllocationPath & a_AllocationPath)
{
	AllocationStats stats(a_AllocationPath);
	for (const auto & s: m_Snapshots)
	{
		const auto alloc = s->findAllocation(a_AllocationPath);
		auto size = (alloc == nullptr) ? 0 : alloc->getAllocationSize();
		stats.processNewValue(size);
	}
	stats.finishProcessing(m_Snapshots.size());
	return stats;
}





std::vector<AllocationPath> Project::getAllAllocationPathsImmediateChildren(const AllocationPath & a_Path)
{
	// Collect all children's CodeLocations:
	std::unordered_set<CodeLocation *> childrenCodeLocations;
	for (const auto & s: m_Snapshots)
	{
		const auto allocation = s->findAllocation(a_Path);
		if (allocation == nullptr)
		{
			continue;
		}
		for (const auto & a: allocation->getChildren())
		{
			childrenCodeLocations.insert(a->getCodeLocation().get());
		}
	}

	// Make the output by appending each child's CodeLocation to a_Path:
	std::vector<AllocationPath> paths;
	for (const auto & cl: childrenCodeLocations)
	{
		paths.emplace_back(a_Path.makeChild(cl));
	}

	return paths;
}




