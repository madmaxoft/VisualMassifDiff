// SnapshotDiff.cpp

// Implements the SnapshotDiff class representing a diff between two snapshots





#include "Globals.h"
#include "SnapshotDiff.h"
#include <unordered_set>
#include "Allocation.h"
#include "Snapshot.h"





////////////////////////////////////////////////////////////////////////////////
// DiffItem:

DiffItem::DiffItem(DiffItem * a_Parent, AllocationPtr a_First, AllocationPtr a_Second):
	m_Parent(a_Parent),
	m_First(a_First),
	m_Second(a_Second)
{
}





DiffItemPtr DiffItem::addChild(AllocationPtr a_First, AllocationPtr a_Second)
{
	auto ch = std::make_shared<DiffItem>(this, a_First, a_Second);
	m_Children.push_back(ch);
	return ch;
}





CodeLocationPtr DiffItem::getCodeLocation() const
{
	if (m_First != nullptr)
	{
		return m_First->getCodeLocation();
	}
	assert(m_Second != nullptr);
	return m_Second->getCodeLocation();
}





qint64 DiffItem::getHeapSizeDiff() const
{
	// If allocation is not present in first snapshot, return second allocation's full size:
	if (m_First == nullptr)
	{
		assert(m_Second != nullptr);
		return static_cast<qint64>(m_Second->getAllocationSize());
	}

	// If allocation is not present in second snapshot, return negative first allocation's full size:
	if (m_Second == nullptr)
	{
		return -static_cast<qint64>(m_First->getAllocationSize());
	}

	// Both allocations are present, return the difference between them:
	auto diff = static_cast<qint64>(m_Second->getAllocationSize()) - static_cast<qint64>(m_First->getAllocationSize());
	return diff;
}





DiffItemPtr DiffItem::child(int a_Index) const
{
	if ((a_Index < 0) || (static_cast<size_t>(a_Index) > m_Children.size()))
	{
		return nullptr;
	}
	return m_Children[a_Index];
}





int DiffItem::indexOfChild(DiffItem * a_Child) const
{
	int idx = 0;
	for (auto ch: m_Children)
	{
		if (ch.get() == a_Child)
		{
			return idx;
		}
		idx += 1;
	}
	return -1;
}





void DiffItem::pruneZeroDiffChildren()
{
	for (auto itr = m_Children.begin(); itr != m_Children.end(); )
	{
		auto & ch = *itr;
		ch->pruneZeroDiffChildren();
		if (
			(ch->getHeapSizeDiff() == 0) &&
			ch->m_Children.empty()
		)
		{
			// Child is empty, remove it
			itr = m_Children.erase(itr);
		}
		else
		{
			// Child has children / nonzero diff, keep it
			++itr;
		}
	}  // for itr - m_Children[]
}





////////////////////////////////////////////////////////////////////////////////
// SnapshotDiff:

SnapshotDiff::SnapshotDiff(SnapshotPtr a_FirstSnapshot, SnapshotPtr a_SecondSnapshot):
	m_FirstSnapshot(a_FirstSnapshot),
	m_SecondSnapshot(a_SecondSnapshot)
{
	m_Root = std::make_shared<DiffItem>(
		nullptr,
		a_FirstSnapshot->getRootAllocation(),
		a_SecondSnapshot->getRootAllocation()
	);
	matchChildren(m_Root);
	m_Root->pruneZeroDiffChildren();
}





void SnapshotDiff::matchChildren(DiffItemPtr a_DiffItem)
{
	auto firstAllocation = a_DiffItem->getFirst();
	auto secondAllocation = a_DiffItem->getSecond();

	// Match firstAllocation's children onto secondAllocation's:
	std::unordered_set<Allocation *> processed;  // set of firstAllocation's children that have been matched
	for (const auto & ch: firstAllocation->getChildren())
	{
		if (ch->getCodeLocation() == nullptr)
		{
			continue;
		}
		auto match = secondAllocation->findCodeLocationChild(ch->getCodeLocation());
		auto di = a_DiffItem->addChild(ch, match);
		if (match != nullptr)
		{
			matchChildren(di);
		}
		processed.insert(ch.get());  // Mark the child as processed
	}

	// Add secondAllocation's children that didn't have a match in firstAllocation's children:
	for (const auto & ch: secondAllocation->getChildren())
	{
		if (ch->getCodeLocation() == nullptr)
		{
			continue;
		}
		if (processed.find(ch.get()) == processed.end())
		{
			// Already matched with a firstAllocation's child
			continue;
		}
		a_DiffItem->addChild(nullptr, ch);
	}
}





