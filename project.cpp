// project.cpp

// Implements the project class representing a project - collection of snapshots.





#include "globals.h"
#include "project.h"
#include "snapshot.h"
#include "codelocationfactory.h"
#include "codelocationstats.h"





Project::Project():
	m_CodeLocationFactory(std::make_shared<CodeLocationFactory>()),
	m_CodeLocationStats(std::make_shared<CodeLocationStats>(this))
{
}





void Project::addSnapshot(SnapshotPtr a_Snapshot)
{
	// Update the stats:
	m_CodeLocationStats->addingSnapshot(a_Snapshot.get());

	// Insert the snapshot into the internal collection, so that it is sorted:
	auto timestamp = a_Snapshot->getTimestamp();
	for (auto itr = m_Snapshots.begin(), end = m_Snapshots.end(); itr != end; ++itr)
	{
		if ((*itr)->getTimestamp() > timestamp)
		{
			m_Snapshots.insert(itr, a_Snapshot);
			return;
		}
	}
	// Timestamp is larger than anything in the collection, append to back:
	m_Snapshots.push_back(a_Snapshot);
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




