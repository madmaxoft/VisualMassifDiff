// CodeLocationStats.cpp

// Implements the CodeLocationStats class that calculates project-wide statistics on CodeLocations





#include "Globals.h"
#include "CodeLocationStats.h"
#include "Project.h"
#include "Snapshot.h"
#include "Allocation.h"





////////////////////////////////////////////////////////////////////////////////
// CodeLocationStats::Stats:

decltype(CodeLocationStats::Stats::m_MaxAllocationSize) CodeLocationStats::Stats::unassignedMax = std::numeric_limits<decltype(m_MinAllocationSize)>::min();
decltype(CodeLocationStats::Stats::m_MinAllocationSize) CodeLocationStats::Stats::unassignedMin = std::numeric_limits<decltype(m_MinAllocationSize)>::max();

CodeLocationStats::Stats::Stats():
	m_MaxAllocationSize(unassignedMax),
	m_MinAllocationSize(unassignedMin),
	m_AvgAllocationSize(0),
	m_CodeLocation(nullptr)
{
}





////////////////////////////////////////////////////////////////////////////////
// CodeLocationStats:

CodeLocationStats::CodeLocationStats(Project * a_Project):
	m_Project(a_Project)
{
	connect(m_Project, SIGNAL(addingSnapshot(SnapshotPtr)), this, SLOT(onProjectAddingSnapshot(SnapshotPtr)));
}





CodeLocationStats::Stats * CodeLocationStats::findStats(CodeLocation * a_CodeLocation)
{
	auto itr = m_Stats.find(a_CodeLocation);
	if (itr == m_Stats.end())
	{
		return nullptr;
	}
	return &(itr->second);
}





void CodeLocationStats::onProjectAddingSnapshot(SnapshotPtr a_Snapshot)
{
	auto numSnapshots = m_Project->getNumSnapshots();
	const auto & sums = a_Snapshot->getFlatSums();
	for (const auto & sum: sums)
	{
		auto loc = sum.first;
		auto allocationSize = sum.second;
		auto & stats = m_Stats[loc];
		stats.m_CodeLocation = loc;  // If the location wasn't present yet - an empty new one was created
		if (stats.m_MaxAllocationSize < allocationSize)
		{
			stats.m_MaxAllocationSize = allocationSize;
		}
		if (stats.m_MinAllocationSize > allocationSize)
		{
			stats.m_MinAllocationSize = allocationSize;
		}
		stats.m_AvgAllocationSize = (stats.m_AvgAllocationSize * numSnapshots + allocationSize) / (numSnapshots + 1);
	}  // for sum: sums[]
}





std::vector<CodeLocationStats::Stats> CodeLocationStats::getAllStats()
{
	std::vector<Stats> res;
	for (const auto s: m_Stats)
	{
		res.push_back(s.second);
	}
	return res;
}





void CodeLocationStats::updateStatsByAllocation(Allocation * a_Allocation)
{
	auto numSnapshots = m_Project->getNumSnapshots();
	for (const auto ch: a_Allocation->getChildren())
	{
		auto loc = ch->getCodeLocation().get();
		auto & stats = m_Stats[loc];
		if (stats.m_CodeLocation != nullptr)
		{
			stats.m_CodeLocation = loc + 1;
		}
		stats.m_CodeLocation = loc;  // If the location wasn't present yet - a new one was created
		auto s = ch->getAllocationSize();
		if (stats.m_MaxAllocationSize < s)
		{
			stats.m_MaxAllocationSize = s;
		}
		if (stats.m_MinAllocationSize > s)
		{
			stats.m_MinAllocationSize = s;
		}
		stats.m_AvgAllocationSize = (stats.m_AvgAllocationSize * numSnapshots + s) / (numSnapshots + 1);
		updateStatsByAllocation(ch.get());
	}
}




