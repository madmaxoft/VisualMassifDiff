// snapshotsgraph.cpp

// Implements the SnapshotsGraph class representing a UI widget that displays the allocation history of several snapshots





#include "globals.h"
#include "snapshotsgraph.h"
#include <QPainter>
#include "project.h"
#include "snapshot.h"
#include "codelocationstats.h"
#include "allocation.h"





/** Maximum number of CodeLocations that are graphed together with the main memory stats. */
static const size_t MaxCodeLocations = 10;

/** Colors used by the CodeLocation graph. */
static const QBrush colors[] =
{
	QColor(0xff, 0x00, 0x00),
	QColor(0x00, 0xff, 0x00),
	QColor(0x00, 0x00, 0xff),
	QColor(0xff, 0xff, 0x00),
	QColor(0x00, 0xff, 0xff),
	QColor(0xff, 0x00, 0xff),
	QColor(0x7f, 0x00, 0x00),
	QColor(0x00, 0x7f, 0x00),
	QColor(0x7f, 0x7f, 0x00),
	QColor(0x00, 0x7f, 0x7f),
	QColor(0x7f, 0x00, 0x7f),
};





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
	selectCodeLocations();
	repaint();
}





void SnapshotsGraph::selectCodeLocations()
{
	m_GraphedCodeLocations.clear();

	// Get all stats, sorted by Avg:
	auto stats = m_Project->getCodeLocationStats()->getAllStats();
	std::sort(stats.begin(), stats.end(), [](const CodeLocationStats::Stats & a_First, const CodeLocationStats::Stats & a_Second)
		{
			return (a_First.m_AvgAllocationSize > a_Second.m_AvgAllocationSize);
		}
	);

	// Graph the top ten:
	auto count = std::min<size_t>(stats.size(), MaxCodeLocations);
	m_GraphedCodeLocations.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		m_GraphedCodeLocations.push_back(stats[i].m_CodeLocation);
	}
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
	m_Width = rect.width();
	m_Height = rect.height() - 2;

	// Decide whether the legend is to be drawn:
	auto numCodeLocations = std::min<size_t>(m_GraphedCodeLocations.size(), 10);
	auto legendHeight = painter.fontMetrics().lineSpacing() * static_cast<int>(numCodeLocations);
	if (m_Height - legendHeight > m_Width / 50)
	{
		paintLegend(painter);
		m_Height -= legendHeight + 2;
	}
	paintGraph(painter);
}





void SnapshotsGraph::paintLegend(QPainter & a_Painter)
{
	int bottom = m_Height;
	int lineSpacing = a_Painter.fontMetrics().lineSpacing();
	size_t idx = 0;
	for (const auto & cl: m_GraphedCodeLocations)
	{
		bottom -= lineSpacing;
		a_Painter.drawText(
			20, bottom, m_Width - 24, lineSpacing,
			Qt::AlignLeft | Qt::AlignVCenter,
			cl->getFunctionName()
		);
		a_Painter.fillRect(2, bottom, 16, lineSpacing - 2, colors[idx % ARRAYCOUNT(colors)]);
		idx += 1;
	}
}





void SnapshotsGraph::paintGraph(QPainter & a_Painter)
{
	// Draw the graph:
	const auto & snapshots = m_Project->getSnapshots();
	if (snapshots.empty())
	{
		return;
	}
	int prevX = projectionX(snapshots.front()->getTimestamp());
	int prevY[MaxCodeLocations];
	projectCodeLocationsY(snapshots.front().get(), prevY);
	int prevYH = projectionY(snapshots.front()->getHeapSize());
	int prevYT = projectionY(snapshots.front()->getTotalSize());
	auto numCodeLocations = std::min<size_t>(m_GraphedCodeLocations.size(), 10);
	for (const auto & s: snapshots)
	{
		int x = projectionX(s->getTimestamp());
		int y[MaxCodeLocations];
		projectCodeLocationsY(s.get(), y);
		int yH = projectionY(s->getHeapSize());
		int yT = projectionY(s->getTotalSize());
		a_Painter.drawLine(prevX, prevYH, x, yH);
		a_Painter.drawLine(prevX, prevYT, x, yT);
		int top = m_Height - 1;
		int prevTop = m_Height - 1;
		QPoint points[4] =
		{
			QPoint(prevX, 0),
			QPoint(prevX, 0),
			QPoint(x, 0),
			QPoint(x, 0),
		};
		for (size_t i = 1; i < numCodeLocations; i++)
		{
			points[0].setY(prevTop);
			points[1].setY(prevY[i]);
			points[2].setY(y[i]);
			points[3].setY(top);
			a_Painter.setBrush(colors[(i - 1) % ARRAYCOUNT(colors)]);
			a_Painter.drawConvexPolygon(points, 4);
			top = y[i];
			prevTop = prevY[i];
		}
		prevX = x;
		prevYH = yH;
		prevYT = yT;
		memcpy(prevY, y, sizeof(prevY));
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





void SnapshotsGraph::projectCodeLocationsY(Snapshot * a_Snapshot, int * a_OutCoords)
{
	quint64 acc = 0;
	size_t idx = 0;
	auto root = a_Snapshot->getRootAllocation();
	for (const auto & cl: m_GraphedCodeLocations)
	{
		auto stat = root->recursiveFindCodeLocationChild(cl);
		if (stat != nullptr)
		{
			acc += stat->getAllocationSize();
		}
		a_OutCoords[idx] = projectionY(acc);
		idx += 1;
	}
}





