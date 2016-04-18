// HistoryGraph.h

// Declares the SnapshotsGraph class representing a UI widget that displays the allocation history of several snapshots





#ifndef SNAPSHOTSGRAPH_H
#define SNAPSHOTSGRAPH_H





#include <memory>
#include <QWidget>





// fwd:
class QItemSelectionModel;
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class CodeLocation;
class Snapshot;
class HistoryModel;
typedef std::shared_ptr<HistoryModel> HistoryModelPtr;





class HistoryGraph:
	public QWidget
{
	typedef QWidget Super;

	Q_OBJECT

public:

	explicit HistoryGraph(QWidget * a_Parent = nullptr);

	/** Sets the project whose snapshots are to be displayed. */
	void setProject(ProjectPtr a_Project, HistoryModel * a_Model, QItemSelectionModel * a_Selection);

signals:

public slots:

	/** Called when the project data has changed and the display should be recalculated. */
	void projectDataChanged();

	/** Called when the selection of the project data has changed and the display should redraw. */
	void selectionChanged();

protected:

	/** The project whose snapshots are to be displayed. */
	ProjectPtr m_Project;

	/** The minimum timestamp value out of all snapshots in the project. */
	quint64 m_MinTimestamp;

	/** The maximum timestamp value out of all snapshots in the project. */
	quint64 m_MaxTimestamp;

	/** The difference between max and min timestamps, or 1 if below. */
	quint64 m_RangeTimestamp;

	/** The maximum heap size value out of all snapshots in the project. */
	quint64 m_MaxHeapSize;

	/** The maximum heap extra size value out of all snapshots in the project. */
	quint64 m_MaxHeapExtraSize;

	/** The maximum sum of heap size and heap extra size out of all snapshots in the project. */
	quint64 m_MaxTotalSize;

	/** The maximum value of the total size, as used for the graph coord projection (never zero) */
	quint64 m_RangeTotalSize;

	/** Width of the widget's paint area, used for projection. */
	int m_Width;

	/** Height of the widget's paint area, used for projection. */
	int m_Height;

	/** The model that provides the graphed data. */
	HistoryModel * m_Model;

	/** The selection that is used to display the highlights. */
	QItemSelectionModel * m_Selection;


	/** Updates the internal variables needed for correct projection of X and Y values. */
	void updateProjection();

	// QWidget overrides:
	virtual void paintEvent(QPaintEvent * a_PaintEvent) override;

	/** Paints the graph part of the widget, into the rect [0, 0, m_Width, m_Height]. */
	void paintGraph(QPainter & a_Painter);

	/** Projects the specified value into the graph X coordinate. */
	int projectionX(quint64 a_ValueX);

	/** Projects the specified value into the graph Y coordinate. */
	int projectionY(quint64 a_ValueY);

	/** Projects the m_GraphedCodeLocations[] values into graph Y coordinates.
	a_OutCoords is an array that receives the Y coords.
	The code locations' sizes are accumulated on top of each other. */
	void projectCodeLocationsY(Snapshot * a_Snapshot, std::vector<int> & a_OutCoords);
};





#endif // SNAPSHOTSGRAPH_H




