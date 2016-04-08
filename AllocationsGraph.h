// AllocationsGraph.h

// Declares the AllocationsGraph class representing a UI widget to display Allocation's children in a graph





#ifndef ALLOCATIONSGRAPH_H
#define ALLOCATIONSGRAPH_H





#include <memory>
#include <QWidget>





// fwd:
class Allocation;
typedef std::shared_ptr<Allocation> AllocationPtr;





class AllocationsGraph:
	public QWidget
{
	typedef QWidget Super;

	Q_OBJECT

public:

	explicit AllocationsGraph(QWidget * a_Parent = nullptr);

	/** Sets the allocation whose children are visualised. */
	void setAllocation(AllocationPtr a_Allocation);

signals:

public slots:

protected:

	AllocationPtr m_Allocation;


	// QWidget overrides:
	virtual void paintEvent(QPaintEvent * a_Event) override;

	/** Draws a single pie chart based on the specified Allocation's children. */
	void drawChart(QPainter & a_Painter, QRect & a_Rect, AllocationPtr a_Allocation);
};





#endif // ALLOCATIONSGRAPH_H




