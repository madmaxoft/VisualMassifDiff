// MainWindow.h

// Declares the MainWindow class representing the app's main UI window





#ifndef MAINWINDOW_H
#define MAINWINDOW_H





#include <memory>
#include <list>
#include <QMainWindow>





// fwd:
class QTreeWidgetItem;
class QSortFilterProxyModel;
class QAbstractItemModel;
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class Snapshot;
typedef std::shared_ptr<Snapshot>SnapshotPtr;
typedef std::list<SnapshotPtr> SnapshotPtrs;
class CodeLocationStatsModel;





namespace Ui
{
	class MainWindow;
}





class MainWindow:
	public QMainWindow
{
	Q_OBJECT

public:

	explicit MainWindow(QWidget * parent = nullptr);

	~MainWindow();


public slots:

	/** Opens a file dialog to choose file, then adds the file contents as new snapshots. */
	void addSnapshotsFromFile();

	/** Adds snapshots from the specified file into the project. */
	void addSnapshotsFromFile(const QString & a_FileName);

	/** Displays an error message containing the error. */
	void parseError(quint32 a_LineNum, const char * a_Msg, const char * a_Line);

	/** Adds the specified new snapshot to current project. */
	void newSnapshotParsed(SnapshotPtr a_Snapshot);

	/** The command used for creating the Massif file has been parsed.
	Checks with the project if it is the same, aborts parse if not. */
	void parsedCommand(const char * a_Command);

	/** The time unit used for creating the Massif file has been parsed.
	Checks with the project if it is the same, aborts parse if not. */
	void parsedTimeUnit(const char * a_TimeUnit);

	/** Triggered when a snapshot is double-clicked in the treeview. */
	void twItemDblClicked(QTreeWidgetItem * a_Item, int a_Column);

	/** Triggered when the selection in twSnapshots changes. */
	void twItemSelChanged();

	/** Opens a snapshot details window for the specified snapshot. */
	void viewSnapshotDetails(SnapshotPtr a_Snapshot);

	/** Creates a diff between the selected snapshots and shows it in a separate dialog. */
	void diffSelected();

	/** Creates a diff between all snapshots and shows it in a separate dialog. */
	void diffAll();

	/** Disposes the current project, and creates a new one.
	Asks the user for confirmation / save if the current project has been changed. */
	void newProject();

	/** Asks the user to select an existing file, then loads the project from that file. */
	void loadProject();

	/** Loads the project from the specified filename. */
	void loadProject(const QString & a_FileName);

	/** Saves the project to the file it was read from / saved to last.
	If the project has no filename attached yet, uses the SaveFile dialog to let the user choose.
	Returns true if the project has been saved, false on error or user cancel. */
	bool saveProject();

	/** Saves the project into the specified file.
	Returns true if successful, false on any kind of an error. */
	bool saveProject(const QString & a_FileName);

	/** Asks the user to select a file name, then saves the project to that file.
	Returns true if the project has been saved, false on error or if the user cancels. */
	bool saveProjectAs();

private:

	/** UI, managed by QTCreator */
	Ui::MainWindow * m_UI;

	/** Current project. */
	ProjectPtr m_Project;

	/**Icon displayed in the tree view if the snapshot has detailed allocations attached to it.  */
	QIcon m_IcoAllocations;

	/** The basic model for CodeLocation stats. */
	std::shared_ptr<CodeLocationStatsModel> m_CodeLocationStatsModel;

	/** The sorting proxy model for CodeLocation stats. */
	std::shared_ptr<QSortFilterProxyModel> m_CodeLocationStatsSortModel;

	/** The model used for the allocation history views. */
	std::shared_ptr<QAbstractItemModel> m_HistoryModel;


	/** Creates a treeview item for the specified new snapshot.
	The item is not inserted into any treewidget. */
	QTreeWidgetItem * createSnapshotTreeItem(SnapshotPtr a_Snapshot);

	/** Displays a new DlgSnapshotDiffs for diffs created between the specified snapshots. */
	void showDiffsForSnapshots(const SnapshotPtrs & a_Snapshots);

	/** If the current project is modified, asks the user whether to save it.
	Returns true if the project can unload (has been saved or didn't need saving.
	Returns false if the user cancelled or an error has occured. */
	bool prepareCurrentProjectForUnload();

	/** Sets the specified project as the current project.
	Updates all views and models based on the new project. */
	void setProject(ProjectPtr a_Project);
};





#endif // MAINWINDOW_H




