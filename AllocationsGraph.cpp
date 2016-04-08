// AllocationsGraph.cpp

// Implements the AllocationsGraph class representing a UI widget to display Allocation's children in a graph





#include "Globals.h"
#include "AllocationsGraph.h"
#include <QPainter>
#include "Allocation.h"





AllocationsGraph::AllocationsGraph(QWidget * a_Parent):
	Super(a_Parent)
{
}





void AllocationsGraph::setAllocation(AllocationPtr a_Allocation)
{
	m_Allocation = a_Allocation;
	repaint();
}





void AllocationsGraph::paintEvent(QPaintEvent * a_Event)
{
	Super::paintEvent(a_Event);
	if (m_Allocation == nullptr)
	{
		return;
	}
	QPainter painter(this);
	QRect rect = contentsRect();
	drawChart(painter, rect, m_Allocation);
}





void AllocationsGraph::drawChart(QPainter & a_Painter, QRect & a_Rect, AllocationPtr a_Allocation)
{
	// Fit a square into the output rectangle:
	auto size = std::min(a_Rect.width(), a_Rect.height()) - 1;
	auto centerX = a_Rect.center().x();
	auto centerY = a_Rect.center().y();
	QRect rect(centerX - size / 2, centerY - size / 2, size, size);

	// Draw all children:
	a_Painter.drawRect(rect);
	auto parentSize = a_Allocation->getAllocationSize();
	if (parentSize == 0)
	{
		a_Painter.drawText(rect, Qt::AlignCenter, tr("Zero allocation size"));
		return;
	}
	quint64 startSize = 0;
	static const QColor colors[] =
	{
		{0xff, 0x00, 0x00},
		{0x00, 0xff, 0x00},
		{0x00, 0x00, 0xff},
		{0xff, 0xff, 0x00},
		{0x00, 0xcf, 0xcf},
		{0xcf, 0x00, 0xff},
	};
	unsigned idx = 0;
	for (const auto & a: a_Allocation->getChildren())
	{
		int startAngle = static_cast<int>(16 * 360 * startSize / parentSize);
		int endAngle = static_cast<int>(16 * 360 * (startSize + a->getAllocationSize()) / parentSize);
		if (startAngle < endAngle)
		{
			a_Painter.setBrush(QBrush(colors[idx % ARRAYCOUNT(colors)]));
			a_Painter.drawPie(rect, startAngle, endAngle - startAngle);
			idx += 1;
		}
		startSize += a->getAllocationSize();
	}
}




