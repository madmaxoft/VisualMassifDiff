// dlgsnapshotdiffs.h

// Implements the DlgSnapshotDiffs class representing the dialog for displaying multiple diffs between snapshots





#ifndef DLGSNAPSHOTDIFFS_H
#define DLGSNAPSHOTDIFFS_H





#include <memory>
#include <vector>
#include <QMainWindow>
#include <QSortFilterProxyModel>





// fwd:
class SnapshotDiff;
typedef std::shared_ptr<SnapshotDiff> SnapshotDiffPtr;
typedef std::vector<SnapshotDiffPtr> SnapshotDiffPtrs;
class SnapshotDiffModel;
typedef std::shared_ptr<SnapshotDiffModel> SnapshotDiffModelPtr;

namespace Ui
{
	class DlgSnapshotDiffs;
}





class DlgSnapshotDiffs:
	public QMainWindow
{
	typedef QMainWindow Super;

	Q_OBJECT

public:
	explicit DlgSnapshotDiffs(QWidget * a_Parent = nullptr);

	/** Shows the dialog with the specified diffs filled in. */
	void show(SnapshotDiffPtrs && a_Diffs);

private slots:

	void onCurrentDiffChanged();

private:

	/** QtCreator-managed UI. */
	std::shared_ptr<Ui::DlgSnapshotDiffs> m_UI;

	/** The diffs that are being shown. */
	SnapshotDiffPtrs m_Diffs;

	/** The models for the diffs being show.
	The models are lazily created and cached. */
	std::map<SnapshotDiff *, SnapshotDiffModelPtr> m_Models;

	/** The sorting proxy model for the diff model. */
	std::shared_ptr<QSortFilterProxyModel> m_SortingModel;
};

#endif // DLGSNAPSHOTDIFFS_H
