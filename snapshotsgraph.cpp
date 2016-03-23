// snapshotsgraph.cpp

// Implements the SnapshotsGraph class representing a UI widget that displays the allocation history of several snapshots





#include "globals.h"
#include "snapshotsgraph.h"
#include <QPainter>
#include "project.h"
#include "snapshot.h"





SnapshotsGraph::SnapshotsGraph(QWidget * a_Parent):
	Super(a_Parent)
{
}





void SnapshotsGraph::setProject(ProjectPtr a_Project)
{
	m_Project = a_Project;
	projectChanged();
}





void SnapshotsGraph::projectChanged()
{
	updateProjection();
	repaint();
}





void SnapshotsGraph::updateProjection()
{
	// Find the extremes:
	m_MinTimestamp = std::numeric_limits<quint64>::max();
	m_MaxTimestamp = std::numeric_limits<quint64>::min();
	m_MaxHeapSize = 0;
	m_MaxHeapExtraSize = 0;
	m_MaxTotalSize = 0;
	for (const auto & s: m_Project->getSnapshots())
	{
		auto timestamp = s->getTimestamp();
		if (timestamp < m_MinTimestamp)
		{
			m_MinTimestamp = timestamp;
		}
		if (timestamp > m_MaxTimestamp)
		{
			m_MaxTimestamp = timestamp;
		}
		auto heapSize = s->getHeapSize();
		if (heapSize > m_MaxHeapSize)
		{
			m_MaxHeapSize = heapSize;
		}
		auto heapExtraSize = s->getHeapExtraSize();
		if (heapExtraSize > m_MaxHeapExtraSize)
		{
			m_MaxHeapExtraSize = heapExtraSize;
		}
		if (heapSize + heapExtraSize > m_MaxTotalSize)
		{
			m_MaxTotalSize = heapSize + heapExtraSize;
		}
	}
	m_RangeTimestamp = std::max<quint64>(m_MaxTimestamp - m_MinTimestamp, 1);
	m_RangeTotalSize = std::max<quint64>(m_MaxTotalSize, 1);
}





void SnapshotsGraph::paintEvent(QPaintEvent * a_PaintEvent)
{
	Super::paintEvent(a_PaintEvent);
	if (m_Project == nullptr)
	{
		return;
	}
	QPainter painter(this);
	QRect rect = contentsRect();

	// Draw the graph:
	m_Width = rect.width();
	m_Height = rect.height();
	const auto & snapshots = m_Project->getSnapshots();
	if (snapshots.empty())
	{
		return;
	}
	int prevX = projectionX(snapshots.front()->getTimestamp());
	int prevY1 = projectionY(snapshots.front()->getHeapSize());
	int prevY2 = projectionY(snapshots.front()->getTotalSize());
	for (const auto & s: snapshots)
	{
		int x = projectionX(s->getTimestamp());
		int y1 = projectionY(s->getHeapSize());
		int y2 = projectionY(s->getTotalSize());
		painter.drawLine(prevX, prevY1, x, y1);
		painter.drawLine(prevX, prevY2, x, y2);
		prevX = x;
		prevY1 = y1;
		prevY2 = y2;
	}
}





int SnapshotsGraph::projectionX(quint64 a_ValueX)
{
	return static_cast<int>(m_Width * (a_ValueX - m_MinTimestamp) / m_RangeTimestamp);
}





int SnapshotsGraph::projectionY(quint64 a_ValueY)
{
	return m_Height - static_cast<int>(m_Height * a_ValueY / m_RangeTotalSize);
}





