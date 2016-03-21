// dlgsnapshotdetails.cpp

// Implements the DlgSnapshotDetails class representing the UI dialog to display details about a single snapshot





#ifndef DLGSNAPSHOTDETAILS_H
#define DLGSNAPSHOTDETAILS_H





#include <memory>
#include <QMainWindow>





// fwd:
class Snapshot;
typedef std::shared_ptr<Snapshot> SnapshotPtr;





namespace Ui
{
	class DlgSnapshotDetails;
}





class DlgSnapshotDetails:
	public QMainWindow
{
	typedef QMainWindow Super;

	Q_OBJECT

public:

	explicit DlgSnapshotDetails(QWidget * a_Parent = nullptr);

	/** Loads data from the specified snapshot and shows the window. */
	void show(SnapshotPtr a_Snapshot);

private:

	std::shared_ptr<Ui::DlgSnapshotDetails> m_UI;

	/** The snapshot being displayed. */
	SnapshotPtr m_Snapshot;

};





#endif // DLGSNAPSHOTDETAILS_H




