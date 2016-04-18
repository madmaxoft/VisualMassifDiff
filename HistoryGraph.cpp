// HistoryGraph.cpp

// Implements the HistoryGraph class representing a UI widget that displays the allocation history of the project's snapshots





#include "Globals.h"
#include "HistoryGraph.h"
#include <unordered_set>
#include <QPainter>
#include <QItemSelectionModel>
#include "Project.h"
#include "Snapshot.h"
#include "CodeLocationStats.h"
#include "Allocation.h"
#include "HistoryModel.h"





HistoryGraph::HistoryGraph(QWidget * a_Parent):
	Super(a_Parent)
{
}





void HistoryGraph::setProject(ProjectPtr a_Project, HistoryModel * a_Model, QItemSelectionModel * a_Selection)
{
	m_Project = a_Project;
	m_Model = a_Model;
	m_Selection = a_Selection;
	connect(m_Selection, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));
	projectDataChanged();
}





void HistoryGraph::projectDataChanged()
{
	updateProjection();
	repaint();
}





void HistoryGraph::selectionChanged()
{
	repaint();
}





void HistoryGraph::updateProjection()
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





void HistoryGraph::paintEvent(QPaintEvent * a_PaintEvent)
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

	paintGraph(painter);
}





void HistoryGraph::paintGraph(QPainter & a_Painter)
{
	// Draw the graph:
	const auto & snapshots = m_Project->getSnapshots();
	if (snapshots.empty() || (m_Model == nullptr))
	{
		return;
	}
	int prevX = projectionX(snapshots.front()->getTimestamp());
	std::vector<int> prevY;
	std::vector<int> y;
	int prevYH = projectionY(snapshots.front()->getHeapSize());
	int prevYT = projectionY(snapshots.front()->getTotalSize());
	const auto & graphedItems = m_Model->getGraphedAllocationPaths();
	auto numGraphedItems = graphedItems.size();
	prevY.resize(numGraphedItems);
	y.resize(numGraphedItems);
	projectCodeLocationsY(snapshots.front().get(), prevY);
	std::vector<bool> isSelected;
	isSelected.resize(numGraphedItems);
	for (const auto & s: m_Selection->selectedIndexes())
	{
		isSelected[s.row()] = true;
	}
	for (const auto & s: snapshots)
	{
		int x = projectionX(s->getTimestamp());
		projectCodeLocationsY(s.get(), y);
		assert(y.size() == numGraphedItems);
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
		for (size_t i = 0; i < numGraphedItems; i++)
		{
			points[0].setY(prevTop);
			points[1].setY(prevY[i]);
			points[2].setY(y[i]);
			points[3].setY(top);
			a_Painter.setBrush(QBrush(graphedItems[i]->m_Color, isSelected[i] ? Qt::DiagCrossPattern : Qt::SolidPattern));
			a_Painter.drawConvexPolygon(points, 4);
			top = y[i];
			prevTop = prevY[i];
		}
		prevX = x;
		prevYH = yH;
		prevYT = yT;
		std::swap(prevY, y);
	}
}





int HistoryGraph::projectionX(quint64 a_ValueX)
{
	return static_cast<int>(m_Width * (a_ValueX - m_MinTimestamp) / m_RangeTimestamp);
}





int HistoryGraph::projectionY(quint64 a_ValueY)
{
	return m_Height - static_cast<int>(m_Height * a_ValueY / m_RangeTotalSize);
}





void HistoryGraph::projectCodeLocationsY(Snapshot * a_Snapshot, std::vector<int> & a_OutCoords)
{
	quint64 acc = 0;
	auto root = a_Snapshot->getRootAllocation();
	const auto & graphedPaths = m_Model->getGraphedAllocationPaths();
	auto numGraphedPaths = graphedPaths.size();
	for (size_t idx = 0; idx < numGraphedPaths; idx++)
	{
		auto allocation = a_Snapshot->findAllocation(graphedPaths[idx]->m_AllocationPath);
		if (allocation != nullptr)
		{
			acc += allocation->getAllocationSize();
			assert(acc <= a_Snapshot->getHeapSize());
		}
		a_OutCoords[idx] = projectionY(acc);
	}
}





