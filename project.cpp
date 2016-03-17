// project.cpp

// Implements the project class representing a project - collection of snapshots.

#include "project.h"
#include "snapshot.h"





Project::Project()
{
}





void Project::addSnapshot(SnapshotPtr a_Snapshot)
{
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





bool Project::hasSnapshotForTimestamp(quint64 a_Timestamp) const
{
	for (auto itr = m_Snapshots.begin(), end = m_Snapshots.end(); itr != end; ++itr)
	{
		if ((*itr)->getTimestamp() >= a_Timestamp)
		{
			return ((*itr)->getTimestamp() == a_Timestamp);
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




