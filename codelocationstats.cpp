// codelocationstats.cpp

// Implements the CodeLocationStats class that calculates project-wide statistics on CodeLocations





#include "globals.h"
#include "codelocationstats.h"
#include "project.h"
#include "snapshot.h"
#include "allocation.h"





////////////////////////////////////////////////////////////////////////////////
// CodeLocationStats::Stats:

decltype(CodeLocationStats::Stats::m_MaxAllocationSize) CodeLocationStats::Stats::unassignedMax = std::numeric_limits<decltype(m_MinAllocationSize)>::min();
decltype(CodeLocationStats::Stats::m_MinAllocationSize) CodeLocationStats::Stats::unassignedMin = std::numeric_limits<decltype(m_MinAllocationSize)>::max();

CodeLocationStats::Stats::Stats():
	m_MaxAllocationSize(unassignedMax),
	m_MinAllocationSize(unassignedMin),
	m_AvgAllocationSize(0)
{
}





////////////////////////////////////////////////////////////////////////////////
// CodeLocationStats:

CodeLocationStats::CodeLocationStats(Project * a_Project):
	m_Project(a_Project)
{
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





void CodeLocationStats::addingSnapshot(Snapshot * a_Snapshot)
{
	updateStatsByAllocation(a_Snapshot->getRootAllocation().get());
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
		stats.m_CodeLocation = loc;
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
	}
}




